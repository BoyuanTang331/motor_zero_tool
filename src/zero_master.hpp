#pragma once

// ZeroMaster — 底盘 Taihu 转向电机 IgH 通讯封装（自包含版）
//
// 只依赖 ecat/task.hpp + libethercat-cpp.so，不依赖 qiuniu/Eigen/spdlog。
// PDO 注册方式照搬 taihu_steer_driver/src/ecat_master.cpp 的 4 轴 CiA402 映射；
// 职责模型仿 motion_control/merman_driver EthercatMaster：
//   - 内建实时周期线程（task 自带 SCHED_FIFO + 绑核）只做 PDO 收发；
//   - 快照（原子）只读暴露给界面/逻辑层；
//   - 输出侧经 staged 暂存，由周期回调每帧应用（等价 taihu main.cpp ecat_cycle 的写法）；
//   - SDO 阻塞读写，供非实时线程调用。

#include <atomic>
#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "ecat/task.hpp"

namespace taihu_zero {

constexpr std::size_t kMaxAxes = 8;
constexpr std::size_t kExpectedAxes = 4;

// 单轴 PDO 句柄（volatile 指针指向 IgH 域内存，与 taihu axis_data 同构）
struct AxisPdo {
  std::uint16_t slave_pos = 0;

  // 输出（主站 -> 从站）
  volatile std::uint16_t* control_word = nullptr;      // 0x6040
  volatile std::int32_t* target_position = nullptr;    // 0x607A
  volatile std::int32_t* target_velocity = nullptr;    // 0x60FF
  volatile std::int16_t* target_torque = nullptr;      // 0x6071
  volatile std::int8_t* mode_of_operation = nullptr;   // 0x6060

  // 输入（从站 -> 主站）
  volatile const std::uint16_t* status_word = nullptr; // 0x6041
  volatile const std::int32_t* position_actual = nullptr; // 0x6064
  volatile const std::int32_t* velocity_actual = nullptr; // 0x606C
  volatile const std::int16_t* torque_actual = nullptr;   // 0x6077
  volatile const std::int8_t* mode_display = nullptr;     // 0x6061
  volatile const std::uint16_t* error_code = nullptr;     // 0x603F
};

// 单轴原子快照（UI/逻辑层无锁只读）
struct AxisSnapshotAtomic {
  std::atomic<std::int32_t> position_counts{0};
  std::atomic<std::int32_t> velocity_counts{0};
  std::atomic<std::int32_t> torque_milli{0};
  std::atomic<std::int32_t> status_word{0};
  std::atomic<std::int32_t> error_code{0};
  std::atomic<std::int32_t> mode_display{0};
  std::atomic<std::int32_t> slave_pos{0};
  std::atomic<std::int32_t> vendor_id{0};
  std::atomic<std::int32_t> product_code{0};
  std::atomic<std::int32_t> pdo_valid{0};  // 1 = PDO 指针已映射且周期帧在收
};

// 单轴快照 POD（一次性 load）
struct AxisSnapshot {
  std::int32_t position_counts = 0;
  std::int32_t velocity_counts = 0;
  std::int32_t torque_milli = 0;
  std::uint16_t status_word = 0;
  std::uint16_t error_code = 0;
  std::int8_t mode_display = 0;
  std::uint16_t slave_pos = 0;
  std::uint32_t vendor_id = 0;
  std::uint32_t product_code = 0;
  bool pdo_valid = false;
};

// staged 输出（逻辑层原子暂存，周期线程每帧应用）
struct StagedOutputAtomic {
  std::atomic<std::int32_t> control_word{0x0006};
  std::atomic<std::int32_t> mode{8};             // 默认恢复 CSP 失能态
  std::atomic<std::int32_t> target_position{0};
  std::atomic<std::int32_t> target_position_valid{0};
};

struct MasterOptions {
  int master_id = 0;
  int cpu_affinity = 1;        // 与 taihu_steer_driver_config.yaml 一致
  int priority = 90;           // 同上（task 内建实时线程优先级）
  int interval = 0;
  std::int64_t cycle_time_ns = 5000000;   // 5ms
  std::int64_t shift_time_ns = 0;
  std::string eni_file;        // 必填（ENI 模式）
  std::uint32_t expected_vendor_id = 5382695;
  std::uint32_t expected_product_code = 37459;
  std::size_t expected_axis_count = kExpectedAxes;
};

class ZeroMaster {
public:
  ZeroMaster();
  ~ZeroMaster();

  ZeroMaster(const ZeroMaster&) = delete;
  ZeroMaster& operator=(const ZeroMaster&) = delete;

  // 启动主站：load_eni -> task.start() -> 校验拓扑
  // 失败返回 false 并填 error（典型：IgH 未启动 / master 已被驱动占用 / 拓扑不符）
  bool start(const MasterOptions& options, std::string* error);
  // 停止：全部轴失能（0x0006/8）-> break_ -> wait -> release
  void stop() noexcept;
  bool running() const noexcept { return running_.load(); }

  std::size_t axis_count() const noexcept { return axis_count_; }

  // ---- 快照（QT 主线程 / 逻辑层只读） ----
  AxisSnapshot snapshot(std::size_t axis) const;
  std::uint64_t frame_counter() const noexcept { return frame_counter_.load(); }

  // ---- 输出暂存（逻辑层调用，下一帧生效） ----
  void stage_command(std::size_t axis, std::uint16_t control_word,
                     std::int8_t mode, std::int32_t target_position,
                     bool target_position_valid) noexcept;

  // ---- SDO（阻塞，仅非实时线程调用） ----
  template <typename T>
  bool sdo_read(std::uint16_t slave_pos, std::uint16_t idx,
                std::uint16_t sub_idx, T* value, std::string* error) {
    try {
      // ecat::task::sdo_upload 为非模板（仅返回 uint16_t，被 task.hpp 声明遮蔽）；
      // 通用宽度读取（i8/u16/i32）走 master 的模板版本 sdo_upload<T>
      ecat::master* m = task_->get_master_ptr();
      if (!m) {
        if (error) *error = "task 未持有 master（主站未启动）";
        return false;
      }
      *value = m->sdo_upload<T>(slave_pos, ecat::sdo_idx{idx, sub_idx}, false);
      return true;
    } catch (const std::exception& e) {
      if (error) *error = e.what();
      return false;
    } catch (...) {
      if (error) *error = "sdo_upload threw unknown exception";
      return false;
    }
  }

  template <typename T>
  bool sdo_write(std::uint16_t slave_pos, std::uint16_t idx,
                 std::uint16_t sub_idx, T value, std::string* error) {
    try {
      task_->sdo_download<T>(slave_pos, {idx, sub_idx}, false, value);
      return true;
    } catch (const std::exception& e) {
      if (error) *error = e.what();
      return false;
    } catch (...) {
      if (error) *error = "sdo_download threw unknown exception";
      return false;
    }
  }

private:
  void on_receive() noexcept;  // 周期回调：应用 staged -> 刷新快照

  // task 禁用拷贝/移动构造，改为 unique_ptr 在 start() 时按 master_id 构造
  std::unique_ptr<ecat::task> task_;
  std::vector<std::unique_ptr<AxisPdo>> axes_;
  std::array<AxisSnapshotAtomic, kMaxAxes> snapshots_;
  std::array<StagedOutputAtomic, kMaxAxes> staged_;
  std::atomic<std::uint64_t> frame_counter_{0};
  std::atomic<bool> running_{false};
  std::size_t axis_count_ = 0;
  MasterOptions options_{};
};

}  // namespace taihu_zero
