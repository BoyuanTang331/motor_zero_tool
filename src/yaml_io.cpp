#include "yaml_io.hpp"

#include <chrono>
#include <fstream>
#include <regex>
#include <sstream>
#include <vector>

namespace taihu_zero {
namespace {

// 匹配 motor_offset: [a, b, c, d]（容忍任意空白、负数、行内注释）
const std::regex kOffsetLine(
    R"(^(\s*motor_offset\s*:\s*\[)\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*(\])(.*)$)");

std::string timestampSuffix() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
  localtime_r(&t, &tm);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tm);
  return buf;
}

}  // namespace

MotorOffsetReadResult readMotorOffsets(const std::string& yaml_path) {
  MotorOffsetReadResult result;
  std::ifstream in(yaml_path);
  if (!in.good()) {
    result.error = "无法打开文件: " + yaml_path;
    return result;
  }
  std::string line;
  std::smatch match;
  while (std::getline(in, line)) {
    if (std::regex_match(line, match, kOffsetLine)) {
      result.offsets = {std::stoi(match[2]), std::stoi(match[3]),
                        std::stoi(match[4]), std::stoi(match[5])};
      result.ok = true;
      return result;
    }
  }
  result.error = "文件中未找到 motor_offset 行: " + yaml_path;
  return result;
}

MotorOffsetWriteResult writeMotorOffsets(
    const std::string& yaml_path,
    const std::array<std::int32_t, 4>& offsets) {
  MotorOffsetWriteResult result;

  std::ifstream in(yaml_path);
  if (!in.good()) {
    result.error = "无法打开文件: " + yaml_path;
    return result;
  }
  std::ostringstream buffer;
  buffer << in.rdbuf();
  in.close();
  const std::string original = buffer.str();

  // 1) 备份（时间戳后缀，永不被覆盖）
  result.backup_path = yaml_path + ".bak-" + timestampSuffix();
  {
    std::ofstream bak(result.backup_path, std::ios::binary | std::ios::trunc);
    if (!bak.good()) {
      result.error = "备份失败，中止写入: " + result.backup_path;
      return result;
    }
    bak << original;
  }

  // 2) 逐行替换 motor_offset
  std::istringstream lines(original);
  std::string line;
  std::smatch match;
  std::ostringstream updated;
  bool replaced = false;
  while (std::getline(lines, line)) {
    if (!replaced && std::regex_match(line, match, kOffsetLine)) {
      updated << match[1].str()
              << offsets[0] << ", " << offsets[1] << ", "
              << offsets[2] << ", " << offsets[3]
              << match[6].str()
              << "  # 标零工具写入（原值已备份 " << result.backup_path << "）";
      replaced = true;
    } else {
      updated << line;
    }
    updated << "\n";
  }
  if (!replaced) {
    result.error = "文件中未找到 motor_offset 行，未写入: " + yaml_path;
    return result;
  }

  // 3) 覆盖原文件
  {
    std::ofstream out(yaml_path, std::ios::binary | std::ios::trunc);
    if (!out.good()) {
      result.error = "无法写入文件: " + yaml_path;
      return result;
    }
    out << updated.str();
  }
  result.ok = true;
  return result;
}

}  // namespace taihu_zero
