#ifndef ECAT_SLICE_HPP
#define ECAT_SLICE_HPP

#include <cstdlib>
#include <ecat/detail/config.hpp>
#include <type_traits>

namespace ecat {
template <typename T> class slice {
  T *data_{};
  std::size_t size_{};

public:
  template <typename U>
  constexpr slice(
      slice<U> const &other,
      std::enable_if_t<std::is_convertible_v<U *, T *>> * = nullptr)
      : data_(other.data()), size_(other.size()) {}

  constexpr slice(T *data, std::size_t size) : data_(data), size_(size) {}

  constexpr T *data() const { return data_; }
  constexpr std::size_t size() const { return size_; }

  constexpr T *begin() const { return data_; }
  constexpr T *end() const { return data_ + size_; }

  template <std::size_t N> constexpr auto get() const {
    static_assert(N < 2, "");
    if constexpr (N == 0) {
      return data_;
    } else {
      return size_;
    }
  }
};

template <typename C>
constexpr auto make_slice(std::remove_const_t<C> &c)
    -> slice<typename std::remove_const_t<C>::value_type> {
  return {c.data(), c.size()};
}

template <typename C>
constexpr auto make_slice(const C &c) -> slice<const typename C::value_type> {
  return {c.data(), c.size()};
}

} // namespace ecat

namespace std {
template <std::size_t N, typename T>
constexpr decltype(auto) get(ecat::slice<T> const &s) {
  return s.template get<N>();
}
} // namespace std

#endif // ECAT_SLICE_HPP
