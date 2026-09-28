#ifndef ECAT_DETAIL_SII_CRC_HPP
#define ECAT_DETAIL_SII_CRC_HPP

#include <ecat/detail/config.hpp>
#include <ecat/types.hpp>

namespace ecat
{
namespace detail
{

/** Calculates the SII checksum field.
 *
 * The checksum is generated with the polynom x^8+x^2+x+1 (0x07) and an
 * initial value of 0xff (see IEC 61158-6-12 ch. 5.4).
 *
 * The code was originally generated with PYCRC
 * http://www.tty1.net/pycrc
 *
 * ./pycrc.py --width=8 --poly=0x07 --reflect-in=0 --xor-in=0xff
 *   --reflect-out=0 --xor-out=0 --generate c --algorithm=bit-by-bit
 *
 * \return CRC8
 */
ECAT_DECL uint8_t calc_sii_crc(const uint8_t *data, size_t length);

inline uint8_t calc_sii_crc(slice<const uint8_t> data)
{
  return calc_sii_crc(data.data(), data.size());
}

} // namespace detail
} // namespace ecat

#endif
