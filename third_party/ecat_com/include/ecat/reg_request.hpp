#ifndef ECAT_REG_REQUEST_HPP
#define ECAT_REG_REQUEST_HPP

#include <cstring>
#include <ecat/slave_config.hpp>
#include <ecat/types.hpp>
#include <iostream>

namespace ecat {

/// reg request interface.
class reg_request {
  slave_config config_;
  unsigned int index_;
  std::uint16_t address_;
  std::uint8_t *data_{};
  std::size_t mem_size_{};
  std::size_t data_size_{};

public:
  /** Creates an register request to exchange regs during non-realtime operation.
   * 
   * This function has to be called in non-realtime context before
   * master::activate().
   */
  reg_request(slave_config sc, std::uint16_t offset, std::size_t size);

  reg_request(const reg_request &) = delete;

  reg_request(reg_request &&) = delete;

  reg_request &operator=(const reg_request &) = delete;

  reg_request &operator=(reg_request &&) = delete;

  /** Destroys an register request. 
   *
   * This function must not be called in realtime context. 
   */
  ~reg_request();

  /** Get the current state of the request. */
  request_state state();

  /** Schedule an register read operation. */
  void read();

  /** Access to the register request's data. */
  slice<std::uint8_t> data() { return slice{data_, data_size_}; }
  slice<const std::uint8_t> data() const { return slice{data_, data_size_}; }

  template <typename T> T data() const {
    auto x = T();
    assert(sizeof(T) == data_size_);
    std::memcpy(&x, data_, data_size_);
    return x;
  }
};

} // namespace ecat

#endif // ECAT_REG_REQUEST_HPP
