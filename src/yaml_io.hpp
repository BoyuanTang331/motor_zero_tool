#pragma once

// yaml_io — taihu_steer_driver_config.yaml 的 motor_offset 读写（无第三方依赖）
//
// 只动 `motor_offset: [...]` 一行（正则匹配），写前自动备份 <file>.bak-<时间戳>。
// 对应驱动公式: q = (counts - motor_offset) * motor_dir / 262144 * 2π

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace taihu_zero {

struct MotorOffsetReadResult {
  bool ok = false;
  std::array<std::int32_t, 4> offsets{0, 0, 0, 0};
  std::string error;
};

struct MotorOffsetWriteResult {
  bool ok = false;
  std::string backup_path;
  std::string error;
};

// 解析 motor_offset 行（支持行内注释与负数）
MotorOffsetReadResult readMotorOffsets(const std::string& yaml_path);

// 备份并改写 motor_offset 行；文件其余内容逐字节保留
MotorOffsetWriteResult writeMotorOffsets(
    const std::string& yaml_path,
    const std::array<std::int32_t, 4>& offsets);

}  // namespace taihu_zero
