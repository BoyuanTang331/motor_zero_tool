#pragma once

// receipt — 标零回执 JSON 原子写入（仿 merman_driver atomic_record_writer 思路：
// 先写 .tmp，fsync 后 rename，杜绝半截文件）

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "cia402_zeroing.hpp"

namespace taihu_zero {

struct ReceiptContext {
  std::string operation_id;              // 唯一操作 ID（时间戳生成）
  std::string mode;                      // "software_zero" | "hm35_zero"
  std::string driver_yaml_path;          // 写回的目标 YAML
  std::string backup_path;               // YAML 备份路径（有则填）
  std::string operator_name;             // 操作员
  std::string note;                      // 备注
  std::array<std::int32_t, 4> offsets{0, 0, 0, 0};  // 软件零位 offset（HM35 时为 0）
  std::vector<AxisSnapshot> snapshots;   // 标零后的轴快照
  ZeroingResult hm35_result;             // HM35 路径结果（软件路径为空）
  bool success = false;
  std::string failure_summary;
};

// 写入 <receipt_dir>/taihu-zero-<operation_id>.json（原子）
// 返回最终文件路径；失败返回空串并填 error
std::string writeReceipt(const std::string& receipt_dir,
                         const ReceiptContext& ctx, std::string* error);

// 生成 operation_id（ taihu-zero-YYYYMMDD-HHMMSS ）
std::string makeOperationId();

}  // namespace taihu_zero
