#pragma once

// cia402_zeroing — Taihu 转向电机标零纯逻辑状态机（无 QT / 无 ecat_com 依赖，可单测）
//
// 结构严格仿 motion_control/src/merman_driver/src/ethercat/
// ti5_homing_commissioning.cpp：
//   - 常量（CiA402 状态字掩码 / HM 错误位 / 位置容差）原样沿用
//   - StageExpectation + matches() 逐状态判定，逻辑等价
//   - pollStage()：每步连续 stable_frame_count 帧匹配才算通过，单步超时 transition_timeout
//   - 7 步厂商控制字时序 sequence_template 原样（0x0006/0 → 0x0086/0 → 0x0086/6
//     → 0x0006/6 → 0x0007/6 → 0x000F/6 → 0x001F/6）
//   - 失能收尾：0x0006/6 退 HM → 0x0006/8 恢复失能 CSP
//   - failAndContain：任何失败尝试 0x0006/8 冻结目标位置回失能
//   - runTi5HomingCommissioning → runHm35Zeroing 一一对应
// 差异：
//   - SOEM 的 ecx_send/receive_processdata 手工收发，换成 ZeroOps.exchange
//     （ecat_com 周期线程自动收发，exchange = stage + 等一帧 + 读快照）
//   - 另加软件零位路径 runSoftwareZeroCapture（Taihu 现有驱动 offset 方案）

#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace taihu_zero {

// ---- CiA402 常量（与 ti5_homing_commissioning.cpp 一致） ----
constexpr std::uint16_t kStatusStateMask = 0x006FU;
constexpr std::uint16_t kStatusReadyToSwitchOn = 0x0021U;
constexpr std::uint16_t kStatusSwitchedOn = 0x0023U;
constexpr std::uint16_t kStatusOperationEnabled = 0x0027U;
constexpr std::uint16_t kStatusFault = 0x0008U;
constexpr std::uint16_t kStatusHomingError = 0x2000U;
constexpr std::int32_t kHomingPositionToleranceCounts = 1;
constexpr std::uint16_t kHomingMethodIndex = 0x6098U;      // CiA402 homing method
constexpr std::int8_t kHomingMethodCurrentPosition = 35;   // 方法 35 = 当前位置设为零点
constexpr std::uint16_t kActualPositionIndex = 0x6064U;
constexpr std::int32_t kStationaryVelocityToleranceCounts = 2;

bool isHomingPositionZero(std::int32_t position_counts) noexcept;

// ---- 命令 / 观测（对应 Ti5HomingPdoCommand / Ti5HomingPdoObservation） ----
struct ZeroCommand {
  std::size_t axis = 0;
  std::uint16_t slave_pos = 0;
  std::uint16_t control_word = 0;
  std::int8_t operation_mode = 0;
  std::int32_t target_position = 0;
  bool target_position_valid = false;
};

struct ZeroObservation {
  bool exchange_succeeded = false;
  std::uint16_t status_word = 0;
  std::uint16_t error_code = 0;
  std::int32_t actual_position = 0;
  std::int32_t actual_velocity = 0;
  std::int8_t operation_mode_display = 0;
};

using ZeroPdoExchange = std::function<ZeroObservation(const ZeroCommand&)>;

// SDO 读写函数指针（对应 Ti5HomingRawOperations，换成本地模板友好形式）
struct ZeroSdoOps {
  std::function<bool(std::uint16_t slave_pos, std::uint16_t idx,
                     std::uint8_t sub_idx, std::int8_t* value)> read_i8;
  std::function<bool(std::uint16_t slave_pos, std::uint16_t idx,
                     std::uint8_t sub_idx, std::int32_t* value)> read_i32;
  std::function<bool(std::uint16_t slave_pos, std::uint16_t idx,
                     std::uint8_t sub_idx, std::int8_t value)> write_i8;
  // Flash 保存需要 16/32 位写入（0x2000 / 0x1010）
  std::function<bool(std::uint16_t slave_pos, std::uint16_t idx,
                     std::uint8_t sub_idx, std::uint16_t value)> write_u16;
  std::function<bool(std::uint16_t slave_pos, std::uint16_t idx,
                     std::uint8_t sub_idx, std::uint32_t value)> write_u32;
  std::function<bool(std::uint16_t slave_pos, std::uint16_t idx,
                     std::uint8_t sub_idx, std::uint16_t* value)> read_u16;
};

struct ZeroingOptions {
  std::chrono::microseconds transition_timeout{std::chrono::seconds(2)};
  std::size_t stable_frame_count = 3;
  std::size_t software_capture_samples = 50;
  std::chrono::microseconds software_capture_interval{std::chrono::milliseconds(20)};
  std::function<bool()> cancellation_requested;
};

enum class ZeroIssueCode {
  InvalidOperations,
  InvalidOptions,
  InvalidTarget,
  HomingMethodUnsupported,
  HomingMethodWriteFailed,
  HomingMethodReadbackFailed,
  PdoExchangeFailed,
  DriveFault,
  DriveNotStationary,
  StateTransitionTimeout,
  HomingVerificationFailed,
  DisableVerificationFailed,
  Canceled,
};

std::string zeroIssueCodeName(ZeroIssueCode code);

struct ZeroEntry {
  std::size_t axis = 0;
  std::uint16_t slave_pos = 0;
  std::optional<std::int32_t> position_before;
  std::optional<std::int32_t> position_after;
  bool homing_method_verified = false;
  bool zero_verified = false;
  bool disabled_and_csp_restored = false;
};

struct ZeroTransitionDiagnostics {
  std::string stage;
  std::optional<std::size_t> sequence_index;
  std::uint16_t commanded_control_word = 0;
  std::int8_t commanded_operation_mode = 0;
  std::string expected_state;
  std::uint16_t last_status_word = 0;
  std::uint16_t last_error_code = 0;
  std::int32_t last_actual_position = 0;
  std::int32_t last_actual_velocity = 0;
  std::int8_t last_operation_mode_display = 0;
  std::size_t observed_frame_count = 0;
  std::size_t matching_frame_count = 0;
  std::chrono::microseconds elapsed{0};
};

struct ZeroIssue {
  ZeroIssueCode code = ZeroIssueCode::InvalidOperations;
  std::optional<std::size_t> axis;
  std::optional<std::uint16_t> slave_pos;
  std::string detail;
  std::optional<ZeroTransitionDiagnostics> transition;
};

struct ZeroingResult {
  std::vector<ZeroEntry> entries;
  std::vector<ZeroIssue> issues;
  std::size_t target_count = 0;
  std::size_t homed_count = 0;
  std::size_t disabled_count = 0;
  bool ok() const noexcept;
};

// ---- HM35 硬件刷零（仿 runTi5HomingCommissioning，逐轴跑 7 步时序） ----
ZeroingResult runHm35Zeroing(const std::vector<std::pair<std::size_t, std::uint16_t>>& targets,
                             const ZeroSdoOps& sdo,
                             const ZeroPdoExchange& exchange,
                             const ZeroingOptions& options);

// ---- HM35 固件支持探测（SDO 读 0x6098:00） ----
// 返回 true 表示对象存在可读（即固件声明支持 homing method 对象）
bool probeHm35Support(const ZeroSdoOps& sdo, std::uint16_t slave_pos,
                      std::int8_t* current_method);

// ---- Flash 参数保存（仿 merman_driver ti5_parameter_persistence） ----
// 机械臂 Ti5 的做法：SDO 写 0x2000:00=1 → 轮询回读清零（写 Flash 完成标志）。
// Taihu 固件未知，按优先级探测两条路：
//   1) CiA 402 标准：0x1010:01 写 32 位 'save'（0x65766173）
//   2) Ti5 同款厂商对象：0x2000:00 写 16 位 1 → 回读清零
// 不做保存的 HM35 零点可能是易失的（掉电丢失）。
constexpr std::uint16_t kStoreParamsIndex = 0x1010U;  // CiA 标准 store parameters
constexpr std::uint32_t kStoreParamsSignature = 0x65766173U;  // 'save' (LE)
constexpr std::uint16_t kVendorSaveIndex = 0x2000U;   // Ti5 同款（备选）

enum class FlashSaveMethod { None, Standard1010, Vendor2000 };

struct FlashSaveResult {
  bool ok = false;
  FlashSaveMethod method = FlashSaveMethod::None;
  std::string error;
};

// 对单轴执行 Flash 保存；内部依次探测两条路径，回读验证
FlashSaveResult runFlashSave(std::uint16_t slave_pos, const ZeroSdoOps& sdo,
                             std::chrono::milliseconds completion_timeout =
                                 std::chrono::milliseconds(3000));

// ---- 软件零位采集（Taihu 现有 offset 方案：offset = 当前 counts） ----
struct SoftwareZeroCapture {
  bool ok = false;
  std::int32_t offset_counts = 0;
  std::int32_t median_counts = 0;
  std::size_t samples = 0;
  std::string error;
};

// 采集：预检（静止/无故障）后 SDO 读 0x6064 × N 次取中值
SoftwareZeroCapture runSoftwareZeroCapture(std::uint16_t slave_pos,
                                           const ZeroSdoOps& sdo,
                                           const ZeroPdoExchange& exchange,
                                           const ZeroingOptions& options);

// ---- 工具函数 ----
std::string formatZeroTransitionDiagnostics(const ZeroTransitionDiagnostics& d);

// 软件零位公式（与 taihu_steer_driver main.cpp counts_to_radians 一致）
double countsToRadians(std::int32_t counts, std::int32_t offset, int dir,
                       double encoder_resolution);

}  // namespace taihu_zero
