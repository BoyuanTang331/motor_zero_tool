#ifndef ECAT_DETAIL_FILE_HPP
#define ECAT_DETAIL_FILE_HPP

// 包含必要的头文件
#include <ecat/detail/config.hpp>  // 配置文件
#include <ecat/detail/os_string.hpp>  // 操作系统相关的字符串处理
#include <system_error>  // 系统错误处理

// 定义命名空间 ecat::detail
namespace ecat {
namespace detail {

// 定义文件操作类
class file {
  int fd_ = -1;  // 文件描述符，初始化为无效值
  const std::error_code success_code_;  // 成功状态码

public:
  file() = default;  // 默认构造函数

  // 显式构造函数，接受文件描述符
  ECAT_DECL explicit file(int fd);

  // 析构函数，确保文件关闭
  ECAT_DECL ~file() noexcept;

  // 关闭文件
  ECAT_DECL void close() noexcept;

  // 定义文件打开模式的静态常量
  ECAT_DECL static const unsigned long readonly;  // 只读模式
  ECAT_DECL static const unsigned long writeonly;  // 只写模式
  ECAT_DECL static const unsigned long readwrite;  // 读写模式

  // 打开文件，接受文件名、标志和错误码
  ECAT_DECL void open(os_string filename, unsigned long flags,
                      std::error_code &ec) noexcept;

  // 打开文件的重载版本，不接受错误码，抛出异常
  void open(os_string filename, unsigned long flags) {
    std::error_code ec;  // 创建错误码对象
    open(filename, flags, ec);  // 调用带错误码的 open 方法
    if (ec)  // 如果有错误
      throw std::system_error(ec, "open");  // 抛出系统错误
  }

  // 获取文件描述符
  int get() const { return fd_; }
};

} // namespace detail
} // namespace ecat

#endif // ECAT_DETAIL_FILE_HPP
