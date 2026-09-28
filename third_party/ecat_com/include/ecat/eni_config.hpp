#ifndef ENI_CONFIG_HPP
#define ENI_CONFIG_HPP
#include <ecat/types.hpp>
#include <ecat/slave_config.hpp>
#include <map>
#include <optional>
#include <string>

namespace ecat
{

struct dc_param
{
  std::uint16_t assign_activate{};

  std::int64_t cycle_time0{};
  std::int64_t shift_time{0};
  std::int64_t cycle_time1{};
};

struct slave_eni_config_info
{
  ecat::slave_id id;
  ecat::slave_rev rev;
  ecat::slave_addr addr;

  std::optional<ecat::slave_config> sc;
  std::optional<dc_param> dc_param_;
  bool is_reference_clock{};
  std::vector<ecat::pdo_cfg> rx_pdos;
  std::vector<ecat::pdo_cfg> tx_pdos;

  ecat::offset rx_offset;
  ecat::offset tx_offset;

  std::optional<int> profile_no;

  uint16_t slots_count = 0;
  uint32_t slot_index_increment = 0x800;
  uint32_t slot_pdo_increment = 16;

  std::uint16_t ethercat_addr;
  std::uint16_t index_in_eni;
};

class eni_config
{
  std::vector<slave_eni_config_info> slaves_config_;

public:
  eni_config(const std::string &path);
  eni_config(const eni_config &) = delete;
  eni_config(eni_config &&) = delete;
  eni_config &operator=(const eni_config &) = delete;
  eni_config &operator=(eni_config &&) = delete;
  std::optional<slave_eni_config_info> get_slave_config_by_id(slave_id &id);
  std::uint16_t get_slave_count();
  std::optional<slave_eni_config_info> get_slave_config_by_addr(std::uint16_t ethercat_addr);
  std::optional<slave_eni_config_info> get_slave_config_by_index(std::uint16_t index_in_eni);
};

} // namespace ecat

#endif
