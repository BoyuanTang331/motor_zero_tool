#ifndef ECAT_ERROR_HPP
#define ECAT_ERROR_HPP

#include "ecat/detail/config.hpp"
#include <cassert>
#include <system_error>

namespace ecat {
inline namespace errors {
enum class error {
  success = 0,
  no_domain = 1,
  not_activated,
  no_devices,
  invalid_al_state,
  sm_idx_too_large,
  kernel_module,
  no_pdo_entry_registered,
};

class ecat_category_impl final : public std::error_category {
public:
  const char *name() const noexcept final { return "ethercat"; }

  std::string message(int code) const noexcept final {
    switch (static_cast<error>(code)) {
    case error::success:
      return "Success";
    case error::no_domain:
      return "No domain";
    case error::not_activated:
      return "Not activated";
    case error::no_devices:
      return "No devices";
    case error::invalid_al_state:
      return "Invalid AL state";
    case error::sm_idx_too_large:
      return "Sync manager index too large";
    case error::kernel_module:
      return "Kernel module error";
    case error::no_pdo_entry_registered:
      return "No PDO entry registered";
    default:
      assert(0);
      return "Unknown";
    }
  }
};

inline const std::error_category &ecat_category() {
  const static ecat_category_impl category;
  return category;
}

inline std::error_code make_error_code(error v) {
  return std::error_code(static_cast<int>(v), ecat_category());
}

enum sdo_abort_code {
  toggle_bit_not_alternate = 0x0503'0000,

  timed_out = 0x0504'0000,
  command_specifier_invalid = 0x0504'0001,
  invalid_block_size = 0x0504'0002,
  invalid_sequence_number = 0x0504'0003,
  crc_error = 0x0504'0004,
  out_of_memory = 0x0504'0005,

  unsupported_access = 0x0601'0000,
  object_not_readable = 0x0601'0001,
  object_not_writable = 0x0601'0002,

  object_not_exist = 0x0602'0000,

  object_not_pdo_mappable = 0x0604'0041,
  pdo_length_excceeded = 0x0604'0042,
  parameter_incompatible = 0x0604'0043,
  device_internal_incompatible = 0x0604'0047,

  hardware_error = 0x0606'0000,

  parameter_length_not_match = 0x0607'0010,
  parameter_too_long = 0x0607'0012,
  parameter_too_short = 0x0607'0013,

  subindex_not_exist = 0x0609'0011,
  value_range_exceeded = 0x0609'0030,
  value_too_large = 0x0609'0031,
  value_too_small = 0x0609'0032,

  maximum_value_less_than_minimum_value = 0x0609'0036,

  general_error = 0x0800'0000,
  data_cannot_be_transferred = 0x0800'0020,
  data_transfer_prohibited = 0x0800'0021,
  data_cannot_be_transferred_in_present_state = 0x0800'0022,

  no_object_dictionary = 0x0800'0023,
};

class sdo_abort_code_category_impl : public std::error_category {
public:
  const char *name() const noexcept { return "ethercat::sdo"; }

  std::string message(int code) const noexcept {
    switch (static_cast<sdo_abort_code>(code)) {
    case toggle_bit_not_alternate:
      return "Toggle bit not alternated";

    case timed_out:
      return "SDO protocol timed out";

    case command_specifier_invalid:
      return "Client/server command specifier not valid or unknown";

    case invalid_block_size:
      return "Invalid block size";

    case invalid_sequence_number:
      return "Invalid sequence number";

    case crc_error:
      return "CRC error";

    case out_of_memory:
      return "Out of memory";

    case unsupported_access:
      return "Unsupport access to an object";

    case object_not_readable:
      return "Attempt to read a write only object";

    case object_not_writable:
      return "Attempt to write a read only object";

    case object_not_exist:
      return "Object does not exist in the object dictionary";

    case object_not_pdo_mappable:
      return "Object cannot be mapped to the PDO";

    case pdo_length_excceeded:
      return "The number and length of the objects to be mapped would excceed "
             "PDO length";

    case parameter_incompatible:
      return "Parameter incompatible";

    case device_internal_incompatible:
      return "Device internal incompatibility";

    case hardware_error:
      return "Access failed due to an hardware error";

    case parameter_length_not_match:
      return "Data type does not match, length of service parameter does not "
             "match";

    case parameter_too_long:
      return "Data type does not match, length of service parameter too high";

    case parameter_too_short:
      return "Data type does not match, length of service parameter too low";

    case subindex_not_exist:
      return "Sub-index does not exist";

    case value_range_exceeded:
      return "Value range of parameter exceeded";

    case value_too_large:
      return "Value of parameter written too high";

    case value_too_small:
      return "Value of parameter written too low";

    case maximum_value_less_than_minimum_value:
      return "Maximum value is less than minimum value";

    case general_error:
      return "General error";

    case data_cannot_be_transferred:
      return "Data cannot be transferred or stored to the application";

    case data_transfer_prohibited:
      return "Data cannot be transferred or stored to the application because "
             "of local control";

    case data_cannot_be_transferred_in_present_state:
      return "Data cannot be transferred or stored to the application because "
             "of the present device state";

    case no_object_dictionary:
      return "Object dictionary dynamic generation fails or no object "
             "dictionary is present";

    default:
      return "Unknown abort code";
    }
  }
};

inline std::error_category const &sdo_abort_code_category() {
  const static sdo_abort_code_category_impl instance;
  return instance;
}

inline std::error_code make_error_code(sdo_abort_code c) {
  return {c, sdo_abort_code_category()};
}

} // namespace errors
} // namespace ecat

namespace std {
template <> struct is_error_code_enum<ecat::error> : std::true_type {};

template <> struct is_error_code_enum<ecat::sdo_abort_code> : std::true_type {};

} // namespace std

#endif // ECAT_ERROR_HPP
