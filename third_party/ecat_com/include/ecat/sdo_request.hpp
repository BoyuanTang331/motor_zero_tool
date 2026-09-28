#ifndef ECAT_SDO_REQUEST_HPP
#define ECAT_SDO_REQUEST_HPP

#include <cstring>
#include <ecat/slave_config.hpp>
#include <ecat/types.hpp>
#include <iostream>

namespace ecat {

/// SDO request interface.
class sdo_request {
  slave_config config_;
  unsigned int index_{};
  sdo_idx sdo_index_{};
  std::uint8_t *data_{};
  std::size_t mem_size_{};
  std::size_t data_size_{};

public:
  /** Creates an SDO request to exchange SDOs during realtime operation.
   * 
   * This function has to be called in non-realtime context before
   * master::activate().  (can be called after master::activate().)
   */
  sdo_request(slave_config sc, sdo_idx idx, std::size_t size,
      bool complete_access = false);

  sdo_request(const sdo_request &) = delete;

  sdo_request(sdo_request &&) = delete;

  sdo_request &operator=(const sdo_request &) = delete;

  sdo_request &operator=(sdo_request &&) = delete;

  /** Destroys an SDO request. 
   *
   * This function must not be called in realtime context. 
   */
  ~sdo_request();

  /** Reinitializes the request using another index. */
  void index(sdo_idx idx, bool complete_access = false);

  /** Reinitializes the request using another index. */
  void index(pdo_index_type idx, sub_index_type sub_idx,
             bool complete_access = false) {
    index({idx, sub_idx}, complete_access);
  }

  /** Returns the SDO index. */
  sdo_idx index() const { return sdo_index_; }

  /** Set the timeout of the request. */
  void timeout(std::uint32_t t);

  /** Get the current state of the request. */
  request_state state();

  /** Schedule an SDO read operation. */
  void read();

  /** Schedule an SDO write operation. */
  void write();

  /** Schedule an SDO write operation. */
  void write(std::size_t size);

  /** Access to the SDO request's data. */
  slice<std::uint8_t> data() { return slice{data_, data_size_}; }
  slice<const std::uint8_t> data() const { return slice{data_, data_size_}; }

  template <typename T> T data() const {
    auto x = T();
    assert(sizeof(T) == data_size_);
    std::memcpy(&x, data_, data_size_);
    return x;
  }

  template <typename T> void data(const T &x) {
    assert(sizeof(T) == data_size_);
    std::memcpy(data_, &x, data_size_);
  }

  std::size_t get_data_size() { return data_size_; }
};

} // namespace ecat

#endif // ECAT_SDO_REQUEST_HPP
