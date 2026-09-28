#ifndef ECAT_DC_HPP
#define ECAT_DC_HPP

#include <cstdint>
#include <ecat/detail/config.hpp>
#include <ecat/master.hpp>

#define ECAT_DC_IN_KERNEL 0


namespace ecat {

class dc {
public:
  /** Initialize the DC structure.
   *
   *  This function should be called in non-realtime threads just before the
   * master is activated.
   *
   * @param master     ethercat master
   * @param period_ns  the period of cycle task
   */
  ECAT_DECL dc(ecat::master &m, std::int32_t period_ns, bool master_follow_slave = true);

  /** Synchronize the distributed clock.
   *
   *  This function should be called periocally before master_send.
   */
  ECAT_DECL void sync() noexcept;

  /** Blocking the thread until the next control period.
   *
   * This function should be called periodically at the beginning of the cyclic
   * task.
   */
  ECAT_DECL void wait_period();

  /** Updates the master time based on differences to ref clock.
   *
   * This function should be called periodically at the end of the cyclic task
   * after the ethercat frame is sent to avoid time jitter in
   * synchronize().
   */
  ECAT_DECL void update_master_clock() noexcept;
  ECAT_DECL std::uint64_t system_time() noexcept;

private:

#if !ECAT_DC_IN_KERNEL

  ECAT_DECL std::uint64_t system_count(std::uint64_t time) noexcept;

  const bool master_follow_slave_;

  ecat::master &master_;
  std::int32_t period_ns_{};
  std::int64_t system_time_base_{};

  std::uint64_t start_time_ns_{};
  std::uint64_t time_ns_{};
  std::int32_t diff_ns_{};
  std::int32_t prev_diff_ns_{};

  bool started_{};
  std::int64_t diff_total_ns_{};
  std::int64_t delta_total_ns_{};
  int filter_index_{};
  std::int64_t adjust_ns_{};

  std::uint64_t wakeup_time_{};
  std::int32_t crazy_limit_{};

  static constexpr int dc_filter_count{10};
  
  struct timeval last_cycle_time_;
#else
  master *m_;

  int index_ = -1;
#endif
};

} // namespace ecat

#endif // ECAT_DC_HPP
