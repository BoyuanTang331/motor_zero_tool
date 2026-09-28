#ifndef ECAT_DETAIL_LOGGER_HPP
#define ECAT_DETAIL_LOGGER_HPP

#include <ecat/detail/config.hpp>
#include <spdlog/logger.h>

namespace ecat::detail
{

ECAT_DECL extern const std::shared_ptr<spdlog::logger> logger;

}

#endif // ECAT_DETAIL_LOGGER_HPP
