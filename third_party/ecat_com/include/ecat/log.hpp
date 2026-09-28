#ifndef ECAT_LOG_HPP
#define ECAT_LOG_HPP

#include <ecat/detail/config.hpp>
#include <functional>

namespace ecat {

enum class log_level
{
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  error = 4,
  critical = 5,
};

using log_handler_type = std::function<void (log_level, const char *, std::size_t)>;

ECAT_DECL void set_log_handler(log_handler_type log_handler);

ECAT_DECL log_handler_type get_log_handler();

}

#endif 