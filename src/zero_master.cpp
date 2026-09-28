#include "zero_master.hpp"

#include <ctime>
#include <iostream>

namespace taihu_zero {

ZeroMaster::ZeroMaster() = default;

ZeroMaster::~ZeroMaster() { stop(); }

bool ZeroMaster::start(const MasterOptions& options, std::string* error) {
  options_ = options;
  task_ = std::make_unique<ecat::task>(options.master_id);
  try {
    task_->priority(options.priority);
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(options.cpu_affinity, &cpus);
    task_->cpu_affinity(&cpus, sizeof(cpus));
    task_->set_interval(options.interval);

    // ENI 模式：与 taihu_steer_driver 相同，按 XML 对表 vendor/product/PDO/SM/DC
    std::int64_t cycle_ns = options.cycle_time_ns;
    task_->load_eni(options.eni_file, cycle_ns);

    task_->set_config_callback([this]() {
      // PDO 注册逻辑照搬 taihu_steer_driver/src/ecat_master.cpp init()
      const std::uint16_t slave_count = task_->slave_count();
      for (std::uint16_t slave_pos = 0; slave_pos < slave_count; ++slave_pos) {
        const auto profile_no = task_->profile_no(slave_pos);
        if (profile_no != 402) continue;
        int n_axis_in_slave = 1;
        const int slots_count = task_->slots_count(slave_pos);
        const int slots_index_increment = task_->slot_index_increment(slave_pos);
        if (slots_count > 0) {
          if (slots_index_increment == -1) continue;
          n_axis_in_slave = slots_count;
        }
        if (slots_count < 0) continue;
        for (int slot_pos = 0; slot_pos < n_axis_in_slave; ++slot_pos) {
          if (axes_.size() >= kMaxAxes) break;
          const int index_offset = slot_pos * slots_index_increment;
          auto axis = std::make_unique<AxisPdo>();
          axis->slave_pos = slave_pos;
          task_->try_register_pdo_entry(
              axis->control_word, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6040 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->target_position, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x607a + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->target_velocity, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x60ff + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->target_torque, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6071 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->mode_of_operation, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6060 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->status_word, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6041 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->position_actual, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6064 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->velocity_actual, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x606c + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->torque_actual, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6077 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->mode_display, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x6061 + index_offset), 0});
          task_->try_register_pdo_entry(
              axis->error_code, slave_pos,
              {static_cast<ecat::pdo_index_type>(0x603f + index_offset), 0});
          axes_.push_back(std::move(axis));
        }
      }
    });

    task_->set_receive_callback([this]() { on_receive(); });
    task_->start();
  } catch (const std::exception& e) {
    if (error) {
      *error = std::string("EtherCAT 主站启动失败（可能 IgH 未启动，"
                           "或 taihu_steer_driver 仍在运行占用 master）: ") +
               e.what();
    }
    return false;
  } catch (...) {
    if (error) *error = "EtherCAT 主站启动失败：未知异常";
    return false;
  }

  axis_count_ = axes_.size();

  // 拓扑 fail-closed 校验（仿 merman_driver production 模式）
  if (axis_count_ != options.expected_axis_count) {
    stop();
    if (error) {
      *error = "拓扑校验失败：映射到 " + std::to_string(axis_count_) +
               " 个 CiA402 轴，期望 " + std::to_string(options.expected_axis_count) +
               " 个（检查网线/ENI/是否误连其他总线）";
    }
    return false;
  }

  for (std::size_t i = 0; i < axis_count_; ++i) {
    const auto* axis = axes_[i].get();
    snapshots_[i].slave_pos.store(axis->slave_pos);
    try {
      const auto info = task_->get_slave_info(axis->slave_pos);
      snapshots_[i].vendor_id.store(static_cast<std::int32_t>(info.id.vendor_id));
      snapshots_[i].product_code.store(static_cast<std::int32_t>(info.id.product_code));
    } catch (...) {
      // vendor/product 读取失败不致命，界面显示 0
    }
    // 上电默认 staged = 失能 CSP（与恢复动作一致，不使能电机）
    staged_[i].control_word.store(0x0006);
    staged_[i].mode.store(8);
  }

  running_.store(true);
  return true;
}

void ZeroMaster::stop() noexcept {
  if (!running_.exchange(false)) return;
  try {
    // 全部轴恢复失能 CSP（0x0006/8），等周期线程应用后退出
    for (std::size_t i = 0; i < axis_count_ && i < kMaxAxes; ++i) {
      staged_[i].control_word.store(0x0006);
      staged_[i].mode.store(8);
      staged_[i].target_position_valid.store(0);
    }
    const std::uint64_t deadline = frame_counter_.load() + 20;  // ~100ms
    while (frame_counter_.load() < deadline) {
      struct timespec ts {0, 2000000};
      nanosleep(&ts, nullptr);
    }
    task_->break_();
    task_->wait();
    task_->resource_recovery();
  } catch (...) {
    // 关闭路径不抛异常
  }
}

AxisSnapshot ZeroMaster::snapshot(std::size_t axis) const {
  AxisSnapshot s;
  if (axis >= kMaxAxes) return s;
  const auto& a = snapshots_[axis];
  s.position_counts = a.position_counts.load();
  s.velocity_counts = a.velocity_counts.load();
  s.torque_milli = a.torque_milli.load();
  s.status_word = static_cast<std::uint16_t>(a.status_word.load());
  s.error_code = static_cast<std::uint16_t>(a.error_code.load());
  s.mode_display = static_cast<std::int8_t>(a.mode_display.load());
  s.slave_pos = static_cast<std::uint16_t>(a.slave_pos.load());
  s.vendor_id = static_cast<std::uint32_t>(a.vendor_id.load());
  s.product_code = static_cast<std::uint32_t>(a.product_code.load());
  s.pdo_valid = a.pdo_valid.load() != 0;
  return s;
}

void ZeroMaster::stage_command(std::size_t axis, std::uint16_t control_word,
                               std::int8_t mode, std::int32_t target_position,
                               bool target_position_valid) noexcept {
  if (axis >= kMaxAxes) return;
  auto& s = staged_[axis];
  s.control_word.store(control_word);
  s.mode.store(mode);
  if (target_position_valid) {
    s.target_position.store(target_position);
    s.target_position_valid.store(1);
  }
}

void ZeroMaster::on_receive() noexcept {
  // 1) 应用 staged 输出（等价 taihu main.cpp 在 ecat_cycle 里写 control_word）
  for (std::size_t i = 0; i < axes_.size(); ++i) {
    auto* axis = axes_[i].get();
    const auto& s = staged_[i];
    if (axis->control_word != nullptr) {
      *axis->control_word = static_cast<std::uint16_t>(s.control_word.load());
    }
    if (axis->mode_of_operation != nullptr) {
      *axis->mode_of_operation = static_cast<std::int8_t>(s.mode.load());
    }
    if (axis->target_position != nullptr && s.target_position_valid.load()) {
      *axis->target_position = s.target_position.load();
    }
    if (axis->target_velocity != nullptr) *axis->target_velocity = 0;
    if (axis->target_torque != nullptr) *axis->target_torque = 0;
  }
  // 2) 刷新快照（只读输入侧）
  for (std::size_t i = 0; i < axes_.size(); ++i) {
    const auto* axis = axes_[i].get();
    auto& snap = snapshots_[i];
    if (axis->position_actual != nullptr) snap.position_counts.store(*axis->position_actual);
    if (axis->velocity_actual != nullptr) snap.velocity_counts.store(*axis->velocity_actual);
    if (axis->torque_actual != nullptr) snap.torque_milli.store(*axis->torque_actual);
    if (axis->status_word != nullptr) snap.status_word.store(*axis->status_word);
    if (axis->error_code != nullptr) snap.error_code.store(*axis->error_code);
    if (axis->mode_display != nullptr) snap.mode_display.store(*axis->mode_display);
    snap.pdo_valid.store(axis->position_actual != nullptr &&
                         axis->status_word != nullptr ? 1 : 0);
  }
  frame_counter_.fetch_add(1);
}

}  // namespace taihu_zero
