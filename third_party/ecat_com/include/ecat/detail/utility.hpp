#ifndef ECAT_DETAIL_UTILITY_HPP
#define ECAT_DETAIL_UTILITY_HPP

#include <exception>
#include <string>
#include <string_view>
#include <cassert>

namespace ecat
{
namespace detail
{
template <typename T> T parse_hex_dec_value(std::string_view v)
{
  /*
    <xs:pattern value="[+-]?[0-9]{1,}"/>
    <xs:pattern value="#x[0-9|a-f|A-F]{1,}"/>
  */
  if (v.empty())
    throw std::invalid_argument("HexDecValue cannot be empty: " +
                                std::string(v));

  if (v[0] == '#')
  {
    if (v.size() < 3 || v[1] != 'x')
      throw std::invalid_argument("HexDecValue: " + std::string(v));

    T x = T();
    bool error = false;
    std::for_each(v.cbegin() + 2, v.cend(), [&](char c) {
      if (c >= '0' && c <= '9')
      {
        x = x * 16 + c - '0';
      }
      else if (c >= 'a' && c <= 'f')
      {
        x = x * 16 + c - 'a' + 10;
      }
      else if (c >= 'A' && c <= 'F')
      {
        x = x * 16 + c - 'A' + 10;
      }
      else
      {
        error = true;
      }
    });

    if (error)
      throw std::invalid_argument("HexDecValue: " + std::string(v));

    return x;
  }
  else
  {
    auto first = v.cbegin();
    const auto end = v.cend();

    T sign = 1;

    if (*first == '+')
      ++first;
    else if (*first == '-')
    {
      sign = -1;
      assert(sign < 0);
      ++first;
    }
    if (first == end)
      throw std::invalid_argument("HexDecValue: " + std::string(v));

    T x = T();
    bool error = false;
    std::for_each(first, end, [&](char c) {
      if (c >= '0' && c <= '9')
      {
        x = x * 10 + c - '0';
      }
      else
      {
        error = true;
      }
    });

    if (error)
      throw std::invalid_argument("HexDecValue: " + std::string(v));

    return x * sign;
  }
}
} // namespace detail
} // namespace ecat

#endif
