#ifndef ECAT_DETAIL_OS_STRING_HPP
#define ECAT_DETAIL_OS_STRING_HPP

#include <cassert>
#include <ecat/detail/config.hpp>
#include <functional>
#include <string>
#include <string_view>
#include <variant>

namespace ecat {
namespace detail {

class os_string {
public:
  os_string(const char *str) noexcept
      : data_(std::in_place_type<const char *>, str) {}

  os_string(std::string const &str) noexcept
      : data_(std::in_place_type<std::reference_wrapper<const std::string>>,
              std::cref(str)) {}

  os_string(std::string_view str)
      : data_(std::in_place_type<std::string>, std::string(str)) {}

  const char *get() const noexcept {
    if (data_.index() == 0) {
      return std::get<0>(data_);
    }
    if (data_.index() == 1) {
      return std::get<1>(data_).get().c_str();
    }
    if (data_.index() == 2) {
      return std::get<2>(data_).c_str();
    }

    assert(0 && "unreachable");
    return NULL;
  }

private:
  std::variant<const char *, std::reference_wrapper<const std::string>,
               std::string>
      data_;
};
} // namespace detail
} // namespace ecat

#endif // ECAT_DETAIL_OS_STRING_HPP
