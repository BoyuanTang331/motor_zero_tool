#ifndef ECAT_DETAIL_IOCTL_HPP
#define ECAT_DETAIL_IOCTL_HPP

#include "ecat/detail/config.hpp"
#include "ecat/detail/file.hpp"
#include "ecat/detail/igh/ioctl.h"
#include <string>
#include <sys/ioctl.h>
#include <system_error>
#include "ecat/run_time_stat.hpp"

namespace ecat {
namespace detail {

inline const std::error_code success_code_;



template <typename T>
unsigned ioctl_impl(file &f, unsigned long cmd, std::error_code &error,
                  T data = nullptr) {
  static_assert(sizeof(T) <= sizeof(void *), "");

  static RunStat srunStat;
//   srunStat.mRecordFlag = true;
  srunStat.statDotStart();
  int ret = NECRO_RT(ioctl(f.get(), cmd, data));
  srunStat.statDot(1, "ioctl");
  srunStat.statAgv();

  if (ret >= 0) {
    error = success_code_;
  } else if (ret == -1) {
    error = std::make_error_code(static_cast<std::errc>(errno));
  } else if (ret < 0) {
    error = std::make_error_code(static_cast<std::errc>(-ret));
  }
  return ret;
}

template <typename T = std::nullptr_t>
unsigned ioctl(file &f, unsigned long cmd, std::error_code &error,
                  T data = nullptr) {
  return ioctl_impl(f, cmd, error, data);
}
} // namespace detail
} // namespace ecat

#endif //
