#ifndef ECAT_DETAIL_LOG_HANDLER_HPP
#define ECAT_DETAIL_LOG_HANDLER_HPP

#include <ecat/log.hpp>
#include <mutex>

namespace ecat { namespace detail {

// extern std::mutex log_handler_mutex;
extern log_handler_type log_handler;    

}}

#endif
