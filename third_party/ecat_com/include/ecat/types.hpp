#ifndef ECAT_TYPES_HPP
#define ECAT_TYPES_HPP

#include "ecat/error.hpp"
#include "ecat/slice.hpp"
#include <array>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <variant>
#include <vector>

namespace ecat {

using domain_index_type = std::uint32_t;
using slave_index_type = std::uint32_t;

/** Vendor ID and Product Code of a slave */
struct slave_id {
  std::uint32_t vendor_id;
  std::uint32_t product_code;
};

inline bool operator<(const slave_id &x, const slave_id &y)
{
  if (x.vendor_id < y.vendor_id)
    return true;
  if (x.vendor_id > y.vendor_id)
    return false;
  return x.product_code < y.product_code;
}

inline bool operator==(const slave_id &x, const slave_id &y)
{
  return x.vendor_id == y.vendor_id && x.product_code == y.product_code;
}

inline bool operator!=(const slave_id &x, const slave_id &y)
{
  return !(x == y);
}

/** Revision number and serial number */
struct slave_rev {
  std::uint32_t revision_number;
  std::uint32_t serial_number;
};

struct slave_by_pos {
  std::uint16_t position;

  constexpr std::pair<std::uint16_t, std::uint16_t> as_pair() const {
    return {0, position};
  }
};

struct slave_by_alias {
  std::uint16_t alias;
  std::uint16_t offset;

  constexpr std::pair<std::uint16_t, std::uint16_t> as_pair() const {
    return {alias, offset};
  }
};

/* master addressing mode */
enum addr_mode {
  logical_addressing,
  alias_addressing,
  logical_alias_addressing
};
struct slave_addr : std::variant<slave_by_pos, slave_by_alias> {
  using std::variant<slave_by_pos, slave_by_alias>::variant;

  std::pair<std::uint16_t, std::uint16_t> as_pair() const {
    return std::visit(
        [](const auto &x) { return x.as_pair(); },
        static_cast<std::variant<slave_by_pos, slave_by_alias>>(*this));
  }
};

/** Application layer state type */
enum class al_state_type : std::uint8_t {
  init = 1,
  preop = 2,
  safeop = 4,
  op = 8,
  boot = 3,
};
std::string to_string(al_state_type state);

ECAT_DECL
std::ostream &operator<<(std::ostream &out, al_state_type alstate);

/** EtherCAT master information */
struct master_info {
  /** Number of slaves in the bus. */
  std::uint32_t slave_count;
  std::uint32_t config_count;
  std::uint32_t domain_count;

  /** Whether the network link is up. */
  bool link_up;

  /** Whether the master is scanning the bus. */
  bool scan_busy;

  /** Application time. */
  std::uint64_t app_time;

  /** Device address. */
  std::array<std::uint8_t, 6> address;

  uint64_t tx_count;
  uint64_t rx_count;
};

/** EtherCAT master state */
struct master_state {

  /** Sum of responding slaves on the given link */
  std::uint32_t slaves_responding;

  /** Application-layer states of the slaves on the given link.
   * If a bit is set, it means that at least one slave in the bus is in the
   * corresponding state:
   *  - Bit 0: \a INIT
   *  - Bit 1: \a PREOP
   *  - Bit 2: \a SAFEOP
   *  - Bit 3: \a OP
   */
  std::uint8_t al_states;

  /** Whether the given Ethernet link is up. */
  bool link_up;
};

/** Slave configuration information */
struct config_info {
  std::uint16_t alias;
  std::uint16_t position;
  slave_id id;
  std::optional<std::uint16_t> slave_position;
  std::uint32_t sdo_count;
  std::uint32_t idn_count;
  // TODO: more attributes are returned:
  // syncs[*], watchdog_*, dc_*
};

enum class slave_port_type {
  not_implemented,
  not_configured,
  ebus,
  mii,
};

struct slave_port_link {
  bool link_up;
  bool loop_closed;
  bool signal_detected;
};

struct slave_port_info {
  slave_port_type desc;
  slave_port_link link;
  std::uint32_t receive_time;
  std::uint16_t next_slave;
  std::uint32_t delay_to_next_dc;
};

constexpr const std::size_t max_ports = 4;

struct slave_info {
  std::string name;
  std::uint16_t ring_pos;
  slave_id id;
  slave_rev rev;
  std::uint16_t alias;
  std::int16_t current_on_ebus;
  al_state_type al_state;
  std::uint8_t error_flag;
  std::uint8_t sync_count;
  std::uint8_t sdo_count;
  std::uint32_t sii_nwords;
  std::uint32_t device_index;
  std::array<slave_port_info, max_ports> ports;
};

struct slave_config_state {
  bool online;
  bool operational;
  al_state_type al_state;
};

enum class sync_direction {
  invalid,
  output,
  input,
};

ECAT_DECL std::ostream &operator<<(std::ostream &os, sync_direction direction);

enum class watchdog_mode {
  default_,
  enable,
  disable,
};

typedef int sm_index_type;

struct sm_info {
  sm_index_type idx;
  std::uint16_t start_addr;
  std::uint16_t default_size;
  std::uint8_t control_register;
  bool enable;
  std::uint8_t pdo_count;
};

struct sm_cfg {
  sm_index_type idx;
  watchdog_mode wd_mode;
  sync_direction direction;

  static constexpr sm_cfg input(sm_index_type idx) {
    return {idx, watchdog_mode::default_, sync_direction::input};
  }

  static constexpr sm_cfg output(sm_index_type idx) {
    return {idx, watchdog_mode::default_, sync_direction::output};
  }
};

typedef std::uint16_t pdo_index_type;
typedef std::uint8_t sub_index_type;
struct pdo_entry_idx {
  pdo_index_type idx;
  sub_index_type sub_idx;

  bool operator<(pdo_entry_idx const &rhs) const {
    return idx < rhs.idx || (idx == rhs.idx && sub_idx < rhs.sub_idx);
  }
};

ECAT_DECL std::ostream &operator<<(std::ostream &os, const pdo_entry_idx &idx);

struct pdo_entry_info {
  std::uint8_t pos;
  pdo_entry_idx entry_idx;
  std::uint8_t bit_len;
  std::string name;
  std::string datatype;
};

struct pdo_cfg {
  pdo_index_type idx;
  std::vector<pdo_entry_info> entries;

  explicit pdo_cfg(pdo_index_type idx, std::vector<pdo_entry_info> entries = std::vector<pdo_entry_info>()) : idx(idx), entries(std::move(entries)) {}
};

template <typename T> struct sdo_data_traits;

template <typename T> struct sdo_data_traits_default_impl {
  static const std::uint8_t *data_ptr(const T &data) {
    return reinterpret_cast<std::uint8_t const *>(&data);
  }

  static std::size_t data_size(const T &data) { return sizeof(data); }
};

template <>
struct sdo_data_traits<std::uint8_t>
    : sdo_data_traits_default_impl<std::uint8_t> {};
template <>
struct sdo_data_traits<std::uint16_t>
    : sdo_data_traits_default_impl<std::uint16_t> {};
template <>
struct sdo_data_traits<std::uint32_t>
    : sdo_data_traits_default_impl<std::uint32_t> {};
template <>
struct sdo_data_traits<std::uint64_t>
    : sdo_data_traits_default_impl<std::uint64_t> {};

template <>
struct sdo_data_traits<std::int8_t>
    : sdo_data_traits_default_impl<std::int8_t> {};
template <>
struct sdo_data_traits<std::int16_t>
    : sdo_data_traits_default_impl<std::int16_t> {};
template <>
struct sdo_data_traits<std::int32_t>
    : sdo_data_traits_default_impl<std::int32_t> {};
template <>
struct sdo_data_traits<std::int64_t>
    : sdo_data_traits_default_impl<std::int64_t> {};

template <typename T> struct sdo_data_traits<slice<T>> {
  static const std::uint8_t *data_ptr(slice<T> data) {
    return reinterpret_cast<std::uint8_t const *>(data.first);
  }

  static std::size_t data_size(slice<T> data) { return data.size(); }
};

enum class wc_state_type {
  zero = 0,
  incomplete = 1,
  complete = 2,
};

struct domain_state {
  std::uint32_t working_counter;
  wc_state_type wc_state;
  bool redundancy_active;
};

struct domain_data_placement {
  std::ptrdiff_t offset;
  std::size_t size;
};

/// SDO information.
struct sdo_info {
  std::uint32_t pos{};
  std::uint32_t idx{};
  std::uint32_t max_sub_idx{};
  std::optional<std::uint8_t> object_code;
  std::string name;
};

enum class access_type {
  read_only,
  read_write,
  write_only,
  unknown,
};

struct sdo_entry_access {
  access_type pre_op;
  access_type safe_op;
  access_type op;
};

enum class data_type_type {
  // bit
  bool_ = 0x001,
  // byte
  byte = 0x01e,

  /// sint
  i8 = 0x0002,
  /// int
  i16 = 0x0003,
  /// dint
  i32 = 0x0004,
  /// lint
  i64 = 0x0015,

  /// usint
  u8 = 0x0005,
  /// uint
  u16 = 0x0006,
  /// udint
  u32 = 0x0007,
  /// ulint
  u64 = 0x001b,

  /// real
  f32 = 0x0008,
  /// lreal
  f64 = 0x0011,

  /// string(n) a.k.a. visiable string
  string = 0x0009,

  /// array of byte a.k.a. octet string
  u8array = 0x000a,

  /// array of uint a.k.a. unicode string
  u16array = 0x000b,

  i24 = 0x0010,
  i40 = 0x0012,
  i48 = 0x0013,
  i56 = 0x0014,

  u24 = 0x0016,
  u40 = 0x0018,
  u48 = 0x0019,
  u56 = 0x001a,

  /// bit 1
  bit1 = 0x0030,
  /// bit 2
  bit2 = 0x0031,
  /// bit 3
  bit3 = 0x0032,
  /// bit 4
  bit4 = 0x0033,
  /// bit 5
  bit5 = 0x0034,
  /// bit 6
  bit6 = 0x0035,
  /// bit 7
  bit7 = 0x0036,
  /// bit 8
  bit8 = 0x0037,

  /// time of day
  timeofday = 0x000c,
  /// time difference
  timedifference = 0x000d,

  /// domain
  domain = 0x000f,

  raw = 0xffff,
};

struct sdo_entry_info {
  data_type_type data_type{};
  std::uint16_t bit_len{};
  sdo_entry_access access{};
  std::string description;
};

struct sdo_entry_pos {
  std::uint16_t pos;
  std::uint16_t sub_idx;
};

struct sdo_idx {
  std::uint16_t idx;
  std::uint16_t sub_idx;
};

struct sdo_entry_addr : std::variant<sdo_entry_pos, sdo_idx> {
  using std::variant<sdo_entry_pos, sdo_idx>::variant;

  constexpr std::pair<std::uint16_t, std::uint8_t> to_pair() const {
    if (index() == 0) {
      auto [pos, sub] = std::get<0>(*this);
      return std::make_pair(static_cast<int>(pos) * -1, sub);
    } else {
      auto [idx, sub] = std::get<1>(*this);
      return std::make_pair(idx, sub);
    }
  }
};

struct pdo_info {
  std::uint16_t sm;
  std::uint16_t pdo_pos;
  std::uint16_t idx;
  std::uint8_t entry_count;
  std::string name;
};

struct offset {
  std::size_t byte;
  std::uint32_t bit;
};

enum class sm_type {
  unused = 0,
  mbx_wr = 1,
  mbx_rd = 2,
  outputs = 3,
  inputs = 4,
};

enum class request_state {
  usused = 0,
  busy = 1,
  success = 2,
  error = 3,
};

} // namespace ecat

#endif // ECAT_TYPES_HPP
