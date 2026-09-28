#include "cia402_zeroing.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <thread>

namespace taihu_zero {
namespace {

enum class StageExpectation {
  StationaryDisabled,
  ReadyModeZero,
  ReadyHoming,
  SwitchedOnHoming,
  SwitchedOnOrOperationEnabledHoming,
  Homed,
  DisabledCsp,
};

const char* stageExpectationName(StageExpectation e) noexcept {
  switch (e) {
    case StageExpectation::StationaryDisabled: return "stationary_disabled";
    case StageExpectation::ReadyModeZero: return "ready_mode_zero";
    case StageExpectation::ReadyHoming: return "ready_homing";
    case StageExpectation::SwitchedOnHoming: return "switched_on_homing";
    case StageExpectation::SwitchedOnOrOperationEnabledHoming:
      return "switched_on_or_operation_enabled_homing";
    case StageExpectation::Homed: return "homed_at_zero";
    case StageExpectation::DisabledCsp: return "disabled_csp";
  }
  return "unknown";
}

// matches() — 与 ti5_homing_commissioning.cpp 逐条等价
bool matches(const ZeroObservation& o, StageExpectation e) noexcept {
  const auto state = o.status_word & kStatusStateMask;
  switch (e) {
    case StageExpectation::StationaryDisabled:
      return (o.status_word & (kStatusFault | 0x0004U | 0x0010U)) == 0U &&
             o.actual_velocity == 0;
    case StageExpectation::ReadyModeZero:
      return state == kStatusReadyToSwitchOn;
    case StageExpectation::ReadyHoming:
      return state == kStatusReadyToSwitchOn && o.operation_mode_display == 6;
    case StageExpectation::SwitchedOnHoming:
      return state == kStatusSwitchedOn && o.operation_mode_display == 6;
    case StageExpectation::SwitchedOnOrOperationEnabledHoming:
      return (state == kStatusSwitchedOn || state == kStatusOperationEnabled) &&
             o.operation_mode_display == 6;
    case StageExpectation::Homed:
      return (state == kStatusSwitchedOn || state == kStatusOperationEnabled) &&
             o.operation_mode_display == 6 &&
             (o.status_word & kStatusHomingError) == 0U &&
             isHomingPositionZero(o.actual_position) &&
             o.actual_velocity == 0;
    case StageExpectation::DisabledCsp:
      return state == kStatusReadyToSwitchOn &&
             o.operation_mode_display == 8 &&
             o.actual_velocity == 0;
  }
  return false;
}

struct PollResult {
  bool success = false;
  ZeroObservation observation;
  ZeroIssueCode failure = ZeroIssueCode::StateTransitionTimeout;
  std::string detail;
  ZeroTransitionDiagnostics transition;
};

// pollStage() — 与 ti5_homing_commissioning.cpp 逻辑等价：
// 连续 stable_frame_count 帧匹配才通过；每帧先做致命检查（交换失败/故障）
PollResult pollStage(const ZeroCommand& command, StageExpectation expectation,
                     const ZeroPdoExchange& exchange,
                     const ZeroingOptions& options, std::string stage,
                     std::optional<std::size_t> sequence_index = std::nullopt) {
  PollResult result;
  result.transition.stage = std::move(stage);
  result.transition.sequence_index = sequence_index;
  result.transition.commanded_control_word = command.control_word;
  result.transition.commanded_operation_mode = command.operation_mode;
  result.transition.expected_state = stageExpectationName(expectation);
  const auto started = std::chrono::steady_clock::now();
  const auto deadline = started + options.transition_timeout;
  std::size_t consecutive = 0;
  const auto update = [&]() {
    result.transition.last_status_word = result.observation.status_word;
    result.transition.last_error_code = result.observation.error_code;
    result.transition.last_actual_position = result.observation.actual_position;
    result.transition.last_actual_velocity = result.observation.actual_velocity;
    result.transition.last_operation_mode_display =
        result.observation.operation_mode_display;
    result.transition.matching_frame_count = consecutive;
    result.transition.elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - started);
  };
  do {
    result.observation = exchange(command);
    ++result.transition.observed_frame_count;
    if (!result.observation.exchange_succeeded) {
      result.failure = ZeroIssueCode::PdoExchangeFailed;
      result.detail = "PDO 交换未完成（周期帧丢失或 PDO 未映射）";
      update();
      return result;
    }
    if (result.observation.error_code != 0U ||
        (result.observation.status_word & kStatusFault) != 0U) {
      result.failure = ZeroIssueCode::DriveFault;
      result.detail = "驱动器报错或进入 Fault";
      update();
      return result;
    }
    if (matches(result.observation, expectation)) {
      ++consecutive;
      if (consecutive >= options.stable_frame_count) {
        result.success = true;
        update();
        return result;
      }
    } else {
      consecutive = 0;
    }
  } while (std::chrono::steady_clock::now() < deadline);

  if (expectation == StageExpectation::StationaryDisabled &&
      result.observation.actual_velocity != 0) {
    result.failure = ZeroIssueCode::DriveNotStationary;
    result.detail = "电机实际速度非零（标零时电机必须静止）";
  } else if (expectation == StageExpectation::Homed) {
    result.failure = ZeroIssueCode::HomingVerificationFailed;
    result.detail = "HM 未达到稳定零位（位置/速度/错误位/模式不满足）";
  } else if (expectation == StageExpectation::DisabledCsp) {
    result.failure = ZeroIssueCode::DisableVerificationFailed;
    result.detail = "电机未回到失能 CSP";
  } else {
    result.failure = ZeroIssueCode::StateTransitionTimeout;
    result.detail = "控制字时序超时，电机未到目标状态";
  }
  update();
  return result;
}

ZeroIssue issueFor(ZeroIssueCode code, std::optional<std::size_t> axis,
                   std::optional<std::uint16_t> slave_pos, std::string detail,
                   std::optional<ZeroTransitionDiagnostics> t = std::nullopt) {
  return {code, axis, slave_pos, std::move(detail), std::move(t)};
}

// verifyDisabledAfterFailure — 对应 Ti5 的失败遏制（0x0006/8 冻结目标回失能 CSP）
bool verifyDisabledAfterFailure(std::size_t axis, std::uint16_t slave_pos,
                                std::optional<std::int32_t> frozen_target,
                                const ZeroPdoExchange& exchange,
                                const ZeroingOptions& options) noexcept {
  try {
    ZeroCommand disable{axis, slave_pos, 0x0006U, 8};
    if (frozen_target.has_value()) {
      disable.target_position = *frozen_target;
      disable.target_position_valid = true;
    }
    return pollStage(disable, StageExpectation::DisabledCsp, exchange, options,
                     "failure_containment").success;
  } catch (...) {
    return false;
  }
}

bool writeHomingMethod35(const ZeroSdoOps& sdo, std::uint16_t slave_pos) {
  return sdo.write_i8(slave_pos, kHomingMethodIndex, 0, kHomingMethodCurrentPosition);
}

bool readHomingMethod(const ZeroSdoOps& sdo, std::uint16_t slave_pos,
                      std::int8_t* value) {
  return sdo.read_i8(slave_pos, kHomingMethodIndex, 0, value);
}

}  // namespace

bool isHomingPositionZero(std::int32_t position_counts) noexcept {
  return position_counts >= -kHomingPositionToleranceCounts &&
         position_counts <= kHomingPositionToleranceCounts;
}

bool probeHm35Support(const ZeroSdoOps& sdo, std::uint16_t slave_pos,
                      std::int8_t* current_method) {
  if (sdo.read_i8 == nullptr) return false;
  std::int8_t method = 0;
  if (!sdo.read_i8(slave_pos, kHomingMethodIndex, 0, &method)) return false;
  if (current_method) *current_method = method;
  return true;
}

FlashSaveResult runFlashSave(std::uint16_t slave_pos, const ZeroSdoOps& sdo,
                             std::chrono::milliseconds completion_timeout) {
  FlashSaveResult result;
  // 路径 1: CiA 402 标准 0x1010:01 'save'
  if (sdo.write_u32) {
    if (sdo.write_u32(slave_pos, kStoreParamsIndex, 1, kStoreParamsSignature)) {
      // 标准做法：写入后固件执行保存；0x1010:01 回读通常返回 1 或保存位
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      if (sdo.read_u16) {
        std::uint16_t readback = 0;
        if (sdo.read_u16(slave_pos, kStoreParamsIndex, 1, &readback)) {
          result.ok = true;
          result.method = FlashSaveMethod::Standard1010;
          return result;
        }
      }
      // 写入成功但回读对象不存在/超时：保存动作仍可能已执行，按成功上报并注明
      result.ok = true;
      result.method = FlashSaveMethod::Standard1010;
      result.error = "0x1010:01 回读未确认（写入已接受）";
      return result;
    }
  }
  // 路径 2: 厂商 0x2000:00 = 1 → 轮询回读清零（Ti5 同款，对应 merman
  // saveTi5Parameters 的写 1 等 0 语义）
  if (sdo.write_u16 && sdo.read_u16) {
    if (sdo.write_u16(slave_pos, kVendorSaveIndex, 0, 1)) {
      const auto deadline =
          std::chrono::steady_clock::now() + completion_timeout;
      while (std::chrono::steady_clock::now() < deadline) {
        std::uint16_t readback = 0;
        if (!sdo.read_u16(slave_pos, kVendorSaveIndex, 0, &readback)) {
          result.error = "0x2000:00 完成回读失败";
          return result;
        }
        if (readback == 0U) {
          result.ok = true;
          result.method = FlashSaveMethod::Vendor2000;
          return result;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      result.error = "0x2000:00 保存未在时限内回读清零（可能仍在执行）";
      result.method = FlashSaveMethod::Vendor2000;
      return result;
    }
  }
  result.error = "0x1010:01 与 0x2000:00 写入均失败：固件不支持参数保存，"
                 "HM35 零点可能掉电丢失";
  return result;
}

ZeroingResult runHm35Zeroing(
    const std::vector<std::pair<std::size_t, std::uint16_t>>& targets,
    const ZeroSdoOps& sdo, const ZeroPdoExchange& exchange,
    const ZeroingOptions& options) {
  ZeroingResult result;
  if (!sdo.read_i8 || !sdo.write_i8 || !sdo.read_i32 || !exchange) {
    result.issues.push_back(issueFor(
        ZeroIssueCode::InvalidOperations, std::nullopt, std::nullopt,
        "SDO 读写与 PDO exchange 必须提供"));
    return result;
  }
  if (options.transition_timeout <= std::chrono::microseconds::zero() ||
      options.stable_frame_count == 0) {
    result.issues.push_back(issueFor(
        ZeroIssueCode::InvalidOptions, std::nullopt, std::nullopt,
        "超时与稳定帧数必须为正"));
    return result;
  }
  if (targets.empty()) {
    result.issues.push_back(issueFor(
        ZeroIssueCode::InvalidTarget, std::nullopt, std::nullopt,
        "至少需要一个目标轴"));
    return result;
  }
  result.target_count = targets.size();

  // 7 步厂商控制字时序 —— 与 ti5_homing_commissioning.cpp sequence_template 完全一致
  const std::array<std::pair<ZeroCommand, StageExpectation>, 7> sequence_template{{
      {{0, 0, 0x0006U, 0}, StageExpectation::ReadyModeZero},
      {{0, 0, 0x0086U, 0}, StageExpectation::ReadyModeZero},
      {{0, 0, 0x0086U, 6}, StageExpectation::ReadyHoming},
      {{0, 0, 0x0006U, 6}, StageExpectation::ReadyHoming},
      {{0, 0, 0x0007U, 6}, StageExpectation::SwitchedOnHoming},
      {{0, 0, 0x000FU, 6}, StageExpectation::SwitchedOnOrOperationEnabledHoming},
      {{0, 0, 0x001FU, 6}, StageExpectation::Homed},
  }};

  for (const auto& [axis, slave_pos] : targets) {
    if (options.cancellation_requested && options.cancellation_requested()) {
      result.issues.push_back(issueFor(ZeroIssueCode::Canceled, axis, slave_pos,
                                       "在下一个目标前被取消"));
      return result;
    }
    ZeroEntry entry;
    entry.axis = axis;
    entry.slave_pos = slave_pos;
    const auto failAndContain = [&](ZeroIssueCode code, std::string detail,
                                    std::optional<ZeroTransitionDiagnostics> t =
                                        std::nullopt) {
      result.issues.push_back(
          issueFor(code, axis, slave_pos, std::move(detail), std::move(t)));
      if (!verifyDisabledAfterFailure(axis, slave_pos, entry.position_before,
                                      exchange, options)) {
        result.issues.push_back(issueFor(
            ZeroIssueCode::DisableVerificationFailed, axis, slave_pos,
            "失败遏制后无法确认失能 CSP"));
      }
    };

    // 预检：静止 + 无故障（控制字 0x0000 只观察）
    const ZeroCommand preflight{axis, slave_pos, 0x0000U, 0};
    const auto stationary = pollStage(
        preflight, StageExpectation::StationaryDisabled, exchange, options,
        "preflight");
    if (stationary.observation.exchange_succeeded) {
      entry.position_before = stationary.observation.actual_position;
    }
    if (!stationary.success) {
      failAndContain(stationary.failure, stationary.detail, stationary.transition);
      result.entries.push_back(entry);
      return result;
    }
    const auto latched_target = *entry.position_before;

    // SDO 写 0x6098:00 = 35 + 回读验证（对应 Ti5 writeHomingMethod/readHomingMethod）
    if (!writeHomingMethod35(sdo, slave_pos)) {
      failAndContain(ZeroIssueCode::HomingMethodWriteFailed,
                     "0x6098:00 写方法 35 失败（固件可能不支持 HM）");
      result.entries.push_back(entry);
      return result;
    }
    std::int8_t method_readback = 0;
    if (!readHomingMethod(sdo, slave_pos, &method_readback) ||
        method_readback != kHomingMethodCurrentPosition) {
      failAndContain(ZeroIssueCode::HomingMethodReadbackFailed,
                     "0x6098:00 未回读到方法 35");
      result.entries.push_back(entry);
      return result;
    }
    entry.homing_method_verified = true;

    // 7 步控制字时序（冻结目标位置 = 预检锁存值）
    for (std::size_t i = 0; i < sequence_template.size(); ++i) {
      if (options.cancellation_requested && options.cancellation_requested()) {
        failAndContain(ZeroIssueCode::Canceled, "在时序阶段边界被取消");
        result.entries.push_back(entry);
        return result;
      }
      auto command = sequence_template[i].first;
      command.axis = axis;
      command.slave_pos = slave_pos;
      command.target_position = latched_target;
      command.target_position_valid = true;
      const auto polled = pollStage(command, sequence_template[i].second,
                                    exchange, options, "vendor_transition", i + 1);
      if (!polled.success) {
        failAndContain(polled.failure, polled.detail, polled.transition);
        result.entries.push_back(entry);
        return result;
      }
      if (sequence_template[i].second == StageExpectation::Homed) {
        entry.position_after = polled.observation.actual_position;
        entry.zero_verified = true;
        ++result.homed_count;
      }
    }

    // 收尾：退 HM（0x0006/6）→ 恢复失能 CSP（0x0006/8）
    const ZeroCommand disable_homing{axis, slave_pos, 0x0006U, 6, 0, true};
    auto disabled = pollStage(disable_homing, StageExpectation::ReadyHoming,
                              exchange, options, "disable_homing");
    if (!disabled.success) {
      failAndContain(disabled.failure, disabled.detail, disabled.transition);
      result.entries.push_back(entry);
      return result;
    }
    const ZeroCommand restore_csp{axis, slave_pos, 0x0006U, 8, 0, true};
    disabled = pollStage(restore_csp, StageExpectation::DisabledCsp, exchange,
                         options, "restore_csp");
    if (!disabled.success) {
      failAndContain(disabled.failure, disabled.detail, disabled.transition);
      result.entries.push_back(entry);
      return result;
    }
    entry.disabled_and_csp_restored = true;
    ++result.disabled_count;
    result.entries.push_back(entry);
  }
  return result;
}

SoftwareZeroCapture runSoftwareZeroCapture(std::uint16_t slave_pos,
                                           const ZeroSdoOps& sdo,
                                           const ZeroPdoExchange& exchange,
                                           const ZeroingOptions& options) {
  SoftwareZeroCapture capture;
  if (!sdo.read_i32 || !exchange) {
    capture.error = "SDO/PDO 操作不可用";
    return capture;
  }
  // 预检（与 HM35 同一 StationaryDisabled 判定）
  const ZeroCommand preflight{0, slave_pos, 0x0000U, 0};
  const auto stationary = pollStage(
      preflight, StageExpectation::StationaryDisabled, exchange, options,
      "software_preflight");
  if (!stationary.success) {
    capture.error = "预检失败（电机必须静止且无故障）: " + stationary.detail;
    return capture;
  }

  std::vector<std::int32_t> samples;
  samples.reserve(options.software_capture_samples);
  for (std::size_t i = 0; i < options.software_capture_samples; ++i) {
    if (options.cancellation_requested && options.cancellation_requested()) {
      capture.error = "采集被取消";
      return capture;
    }
    std::int32_t value = 0;
    if (!sdo.read_i32(slave_pos, kActualPositionIndex, 0, &value)) {
      capture.error = "SDO 读 0x6064:00 失败（第 " + std::to_string(i) + " 次）";
      return capture;
    }
    samples.push_back(value);
    if (options.software_capture_interval > std::chrono::microseconds::zero()) {
      std::this_thread::sleep_for(options.software_capture_interval);
    }
  }
  std::sort(samples.begin(), samples.end());
  capture.samples = samples.size();
  capture.median_counts = samples[samples.size() / 2];
  capture.offset_counts = capture.median_counts;
  capture.ok = true;
  return capture;
}

bool ZeroingResult::ok() const noexcept {
  return issues.empty() && target_count != 0 &&
         entries.size() == target_count && homed_count == target_count &&
         disabled_count == target_count &&
         std::all_of(entries.begin(), entries.end(), [](const auto& e) {
           return e.homing_method_verified && e.zero_verified &&
                  e.disabled_and_csp_restored && e.position_after.has_value() &&
                  isHomingPositionZero(*e.position_after);
         });
}

std::string zeroIssueCodeName(ZeroIssueCode code) {
  switch (code) {
    case ZeroIssueCode::InvalidOperations: return "invalid_operations";
    case ZeroIssueCode::InvalidOptions: return "invalid_options";
    case ZeroIssueCode::InvalidTarget: return "invalid_target";
    case ZeroIssueCode::HomingMethodUnsupported: return "homing_method_unsupported";
    case ZeroIssueCode::HomingMethodWriteFailed: return "homing_method_write_failed";
    case ZeroIssueCode::HomingMethodReadbackFailed:
      return "homing_method_readback_failed";
    case ZeroIssueCode::PdoExchangeFailed: return "pdo_exchange_failed";
    case ZeroIssueCode::DriveFault: return "drive_fault";
    case ZeroIssueCode::DriveNotStationary: return "drive_not_stationary";
    case ZeroIssueCode::StateTransitionTimeout: return "state_transition_timeout";
    case ZeroIssueCode::HomingVerificationFailed: return "homing_verification_failed";
    case ZeroIssueCode::DisableVerificationFailed:
      return "disable_verification_failed";
    case ZeroIssueCode::Canceled: return "canceled";
  }
  return "unknown";
}

std::string formatZeroTransitionDiagnostics(const ZeroTransitionDiagnostics& d) {
  std::ostringstream out;
  out << "stage=" << d.stage << " sequence=";
  if (d.sequence_index.has_value()) out << *d.sequence_index;
  else out << "none";
  out << " command_control_word=0x" << std::hex << std::setw(4)
      << std::setfill('0') << d.commanded_control_word << std::dec
      << std::setfill(' ') << " command_operation_mode="
      << static_cast<int>(d.commanded_operation_mode)
      << " expected_state=" << d.expected_state
      << " observed_frames=" << d.observed_frame_count
      << " matching_frames=" << d.matching_frame_count
      << " elapsed_us=" << d.elapsed.count() << " last_status_word=0x"
      << std::hex << std::setw(4) << std::setfill('0') << d.last_status_word
      << " last_error_code=0x" << std::setw(4) << d.last_error_code << std::dec
      << std::setfill(' ') << " last_mode_display="
      << static_cast<int>(d.last_operation_mode_display)
      << " last_position=" << d.last_actual_position
      << " last_velocity=" << d.last_actual_velocity;
  return out.str();
}

double countsToRadians(std::int32_t counts, std::int32_t offset, int dir,
                       double encoder_resolution) {
  constexpr double kTwoPi = 6.28318530717958647692;
  return static_cast<double>(counts - offset) * dir / encoder_resolution * kTwoPi;
}

}  // namespace taihu_zero
