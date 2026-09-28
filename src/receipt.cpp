#include "receipt.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h>

namespace taihu_zero {
namespace {

std::string isoNow() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
  localtime_r(&t, &tm);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", &tm);
  return buf;
}

std::string escape(const std::string& s) {
  std::string out;
  out.reserve(s.size() + 8);
  for (const char c : s) {
    switch (c) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n"; break;
      case '\t': out += "\\t"; break;
      default: out += c;
    }
  }
  return out;
}

}  // namespace

std::string makeOperationId() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
  localtime_r(&t, &tm);
  char buf[40];
  std::strftime(buf, sizeof(buf), "taihu-zero-%Y%m%d-%H%M%S", &tm);
  return buf;
}

std::string writeReceipt(const std::string& receipt_dir,
                         const ReceiptContext& ctx, std::string* error) {
  std::error_code ec;
  std::filesystem::create_directories(receipt_dir, ec);
  if (ec) {
    if (error) *error = "创建回执目录失败: " + ec.message();
    return {};
  }

  std::ostringstream j;
  j << "{\n";
  j << "  \"schema_version\": 1,\n";
  j << "  \"operation_id\": \"" << escape(ctx.operation_id) << "\",\n";
  j << "  \"timestamp\": \"" << isoNow() << "\",\n";
  j << "  \"mode\": \"" << escape(ctx.mode) << "\",\n";
  j << "  \"operator\": \"" << escape(ctx.operator_name) << "\",\n";
  j << "  \"note\": \"" << escape(ctx.note) << "\",\n";
  j << "  \"driver_yaml_path\": \"" << escape(ctx.driver_yaml_path) << "\",\n";
  j << "  \"backup_path\": \"" << escape(ctx.backup_path) << "\",\n";
  j << "  \"success\": " << (ctx.success ? "true" : "false") << ",\n";
  j << "  \"failure_summary\": \"" << escape(ctx.failure_summary) << "\",\n";
  j << "  \"motor_offsets\": [" << ctx.offsets[0] << ", " << ctx.offsets[1]
    << ", " << ctx.offsets[2] << ", " << ctx.offsets[3] << "],\n";
  j << "  \"hm35_entries\": [\n";
  for (std::size_t i = 0; i < ctx.hm35_result.entries.size(); ++i) {
    const auto& e = ctx.hm35_result.entries[i];
    j << "    {\"axis\": " << e.axis << ", \"slave_pos\": " << e.slave_pos
      << ", \"position_before\": "
      << (e.position_before.has_value() ? std::to_string(*e.position_before) : "null")
      << ", \"position_after\": "
      << (e.position_after.has_value() ? std::to_string(*e.position_after) : "null")
      << ", \"homing_method_verified\": "
      << (e.homing_method_verified ? "true" : "false")
      << ", \"zero_verified\": " << (e.zero_verified ? "true" : "false")
      << ", \"disabled_and_csp_restored\": "
      << (e.disabled_and_csp_restored ? "true" : "false") << "}";
    j << (i + 1 < ctx.hm35_result.entries.size() ? ",\n" : "\n");
  }
  j << "  ],\n";
  j << "  \"hm35_issues\": [\n";
  for (std::size_t i = 0; i < ctx.hm35_result.issues.size(); ++i) {
    const auto& issue = ctx.hm35_result.issues[i];
    j << "    {\"code\": \"" << zeroIssueCodeName(issue.code) << "\", \"axis\": "
      << (issue.axis.has_value() ? std::to_string(*issue.axis) : "null")
      << ", \"slave_pos\": "
      << (issue.slave_pos.has_value() ? std::to_string(*issue.slave_pos) : "null")
      << ", \"detail\": \"" << escape(issue.detail) << "\"";
    if (issue.transition.has_value()) {
      j << ", \"transition\": \""
        << escape(formatZeroTransitionDiagnostics(*issue.transition)) << "\"";
    }
    j << "}";
    j << (i + 1 < ctx.hm35_result.issues.size() ? ",\n" : "\n");
  }
  j << "  ],\n";
  j << "  \"snapshots\": [\n";
  for (std::size_t i = 0; i < ctx.snapshots.size(); ++i) {
    const auto& s = ctx.snapshots[i];
    j << "    {\"slave_pos\": " << s.slave_pos
      << ", \"vendor_id\": " << s.vendor_id
      << ", \"product_code\": " << s.product_code
      << ", \"position_counts\": " << s.position_counts
      << ", \"velocity_counts\": " << s.velocity_counts
      << ", \"status_word\": " << s.status_word
      << ", \"error_code\": " << s.error_code
      << ", \"mode_display\": " << static_cast<int>(s.mode_display) << "}";
    j << (i + 1 < ctx.snapshots.size() ? ",\n" : "\n");
  }
  j << "  ]\n";
  j << "}\n";

  const std::string final_path =
      receipt_dir + "/taihu-zero-" + ctx.operation_id + ".json";
  const std::string tmp_path = final_path + ".tmp";
  {
    std::ofstream out(tmp_path, std::ios::binary | std::ios::trunc);
    if (!out.good()) {
      if (error) *error = "无法写入临时回执: " + tmp_path;
      return {};
    }
    out << j.str();
    out.flush();
  }
  // fsync + rename（原子提交）
  FILE* fp = std::fopen(tmp_path.c_str(), "r");
  if (fp != nullptr) {
    ::fsync(::fileno(fp));
    std::fclose(fp);
  }
  std::filesystem::rename(tmp_path, final_path, ec);
  if (ec) {
    if (error) *error = "回执提交失败: " + ec.message();
    std::error_code remove_ec;
    std::filesystem::remove(tmp_path, remove_ec);
    return {};
  }
  return final_path;
}

}  // namespace taihu_zero
