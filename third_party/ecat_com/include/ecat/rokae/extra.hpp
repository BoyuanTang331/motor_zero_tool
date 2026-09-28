#pragma once

#include <cstdint>
#include <cstring>

#include <ecat/master.hpp>

namespace ecat
{
namespace extra
{
inline void set_bits(std::uint8_t *dest, const std::uint8_t *src,
                     std::uint16_t bit_offset, std::uint16_t n_bits)
{
  std::uint16_t n_bytes = (bit_offset + n_bits + 7) / 8;

  std::uint16_t first_byte = bit_offset / 8;
  switch (bit_offset % 8)
  {
  case 0:
    dest[first_byte] = src[first_byte];
    break;
  case 1:
    dest[first_byte] &= 0x01;
    dest[first_byte] |= src[first_byte] & ~0x01;
    break;
  case 2:
    dest[first_byte] &= 0x03;
    dest[first_byte] |= src[first_byte] & ~0x03;
    break;
  case 3:
    dest[first_byte] &= 0x07;
    dest[first_byte] |= src[first_byte] & ~0x07;
    break;
  case 4:
    dest[first_byte] &= 0x0f;
    dest[first_byte] |= src[first_byte] & ~0x0f;
    break;
  case 5:
    dest[first_byte] &= 0x1f;
    dest[first_byte] |= src[first_byte] & ~0x1f;
    break;
  case 6:
    dest[first_byte] &= 0x3f;
    dest[first_byte] |= src[first_byte] & ~0x3f;
    break;
  case 7:
    dest[first_byte] &= 0x7f;
    dest[first_byte] |= src[first_byte] & ~0x7f;
    break;
  }

  std::memcpy(dest + first_byte + 1, src + first_byte + 1,
              n_bytes - first_byte - 2);

  std::uint16_t last_byte = n_bytes - 1;

  switch ((n_bits - bit_offset) % 8)
  {
  case 0:
    dest[last_byte] = src[last_byte];
    break;
  case 1:
    dest[last_byte] &= ~0x01;
    dest[last_byte] |= src[last_byte] & 0x01;
    break;
  case 2:
    dest[last_byte] &= ~0x03;
    dest[last_byte] |= src[last_byte] & 0x03;
    break;
  case 3:
    dest[last_byte] &= ~0x07;
    dest[last_byte] |= src[last_byte] & 0x07;
    break;
  case 4:
    dest[last_byte] &= ~0x0f;
    dest[last_byte] |= src[last_byte] & 0x0f;
    break;
  case 5:
    dest[last_byte] &= ~0x1f;
    dest[last_byte] |= src[last_byte] & 0x1f;
    break;
  case 6:
    dest[last_byte] &= ~0x3f;
    dest[last_byte] |= src[last_byte] & 0x3f;
    break;
  case 7:
    dest[last_byte] &= ~0x7f;
    dest[last_byte] |= src[last_byte] & 0x7f;
    break;
  }
}

inline void get_bits(const std::uint8_t *src, std::uint8_t *dest,
                     std::uint16_t bit_offset, std::uint16_t n_bits)
{
  std::uint16_t n_bytes = (bit_offset + n_bits + 7) / 8;
  std::memset(dest, 0, n_bytes);

  std::uint16_t first_byte = bit_offset / 8;
  switch (bit_offset % 8)
  {
  case 0:
    dest[first_byte] = src[first_byte];
    break;
  case 1:
    dest[first_byte] &= 0x01;
    dest[first_byte] |= src[first_byte] & ~0x01;
    break;
  case 2:
    dest[first_byte] &= 0x03;
    dest[first_byte] |= src[first_byte] & ~0x03;
    break;
  case 3:
    dest[first_byte] &= 0x07;
    dest[first_byte] |= src[first_byte] & ~0x07;
    break;
  case 4:
    dest[first_byte] &= 0x0f;
    dest[first_byte] |= src[first_byte] & ~0x0f;
    break;
  case 5:
    dest[first_byte] &= 0x1f;
    dest[first_byte] |= src[first_byte] & ~0x1f;
    break;
  case 6:
    dest[first_byte] &= 0x3f;
    dest[first_byte] |= src[first_byte] & ~0x3f;
    break;
  case 7:
    dest[first_byte] &= 0x7f;
    dest[first_byte] |= src[first_byte] & ~0x7f;
    break;
  }

  std::memcpy(dest + first_byte + 1, src + first_byte + 1,
              n_bytes - first_byte - 2);

  std::uint16_t last_byte = n_bytes - 1;

  switch ((n_bits - bit_offset) % 8)
  {
  case 0:
    dest[last_byte] = src[last_byte];
    break;
  case 1:
    dest[last_byte] &= ~0x01;
    dest[last_byte] |= src[last_byte] & 0x01;
    break;
  case 2:
    dest[last_byte] &= ~0x03;
    dest[last_byte] |= src[last_byte] & 0x03;
    break;
  case 3:
    dest[last_byte] &= ~0x07;
    dest[last_byte] |= src[last_byte] & 0x07;
    break;
  case 4:
    dest[last_byte] &= ~0x0f;
    dest[last_byte] |= src[last_byte] & 0x0f;
    break;
  case 5:
    dest[last_byte] &= ~0x1f;
    dest[last_byte] |= src[last_byte] & 0x1f;
    break;
  case 6:
    dest[last_byte] &= ~0x3f;
    dest[last_byte] |= src[last_byte] & 0x3f;
    break;
  case 7:
    dest[last_byte] &= ~0x7f;
    dest[last_byte] |= src[last_byte] & 0x7f;
    break;
  }
}
/** Obtains slave position by slave alias. */
inline int find_slave_by_alias(ecat::master &m, uint16_t slave_alias)
{
  auto master_info = m.get_info();
  for (uint32_t i = 0; i != master_info.slave_count; ++i)
  {
    auto slave_info = m.get_slave_info(i);
    if (slave_info.alias == slave_alias)
      return i;
  }

  return -1;
}

/** Executes an SDO download request to write data to a slave. This function is different 
 *  with master::sdo_download, it base the slave alias not slave position.
 *
 *  This request is processe by the master state machine. This method blocks,
 *  until the request has been processed and may not be called in realtime context.
 */
template <typename... Args>
decltype(auto) sdo_download(ecat::master &m, uint16_t slave_alias,
                            Args &&...args)
{
  int slave_pos = find_slave_by_alias(m, slave_alias);
  if (slave_pos == -1)
    throw std::runtime_error{"slave with specified alias not found"};

  return m.sdo_download(slave_pos, std::forward<Args>(args)...);
}

/** Obtain the process data base addresses for each slave.
 *  This function is different with master::slave_process_data, 
 *  it base the slave alias not slave position.
 */
std::uint8_t *get_slave_process_data(ecat::master &m, uint16_t slave_alias);

void set_master_state(ecat::master &m, al_state_type state);

al_state_type get_master_state(ecat::master &m);

} // namespace extra
} // namespace ecat
