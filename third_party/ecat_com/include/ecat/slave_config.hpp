#ifndef ECAT_SLAVE_CONFIG_HPP
#define ECAT_SLAVE_CONFIG_HPP

#include "ecat/detail/config.hpp"
#include "ecat/types.hpp"

namespace ecat {
class master;
class sdo_request;

using config_index_type = std::uint32_t;

/// Handle to a slave configuration.
class slave_config {

public:
  typedef class master master_type;

private:
  friend class master;

public:
  slave_config(master_type &m, slave_addr addr, slave_id id);

  slave_config(master_type &m, config_index_type idx) : m_(&m), idx_(idx) {}

  slave_config(const slave_config &other) = default;

  slave_config &operator=(const slave_config &) = default;

  master_type &master() { return *m_; }

  config_index_type index() const { return idx_; }

  ECAT_DECL slave_config_state state();

  /** Specify a complete PDO configuration. */
  ECAT_DECL void config_sm_pdos(sm_cfg smcfg, slice<const pdo_cfg> pdocfgs);

  /** Configure a slave's watchdog times.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void config_watchdog(std::uint16_t divider,
                                 std::uint16_t intervals);

  /** Configure whether a slave allows overlapping PDOs.
   *
   * Overlapping PDOs allows inputs to use the same space as outputs on the
   * frame. This reduces the frame length.
   */
  ECAT_DECL void allow_overlapping_pdos(bool allow);

  /** Configure a sync manager.
   *
   * Sets the direction of a sync manager. This overrides the direction bits
   * from the default control register from SII.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void config_sync_manager(sm_cfg cfg);

  /** Clear a sync manager's PDO assignment.
   *
   * This can be called before assigning PDOs via
   * slave_config::add_pdo_assignement(), to clear the default assignment of a
   * sync manager.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void clear_pdo_assignments(std::uint16_t sm_idx);

  /** Add a PDO entry to the given PDO's mapping.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void add_pdo_assignment(std::uint16_t sm_idx,
                                    pdo_index_type pdo_idx);

  /** Clear the mapping of a given PDO.
   *
   * This can be called before mapping PDO entries via
   * slave_config::add_pdo_assignment(), to clear the default mapping.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void clear_pdo_mapping(pdo_index_type pdo_idx);

  ECAT_DECL void add_pdo_mapping(pdo_index_type pdo_idx,
                                 pdo_entry_info const &entry);

  /** Registers a PDO entry for process data exchange in a domain.
   *
   * Searches the assigned PDOs for the given PDO entry. An error is raised, if
   * the given entry is not mapped. Otherwise, the corresponding sync manager
   * and FMMU configurations are provided for slave configuration and the
   * respective sync manager's assigned PDOs are appended to the given domain,
   * if not already done.
   * This method has to be called in non-realtime context before
   * master::activate().
   *
   * \returns Offset of the PDO entry's process data.
   */
  ECAT_DECL offset register_pdo_entry(pdo_entry_idx index,
                                      domain_index_type domain);

  /** Registers a PDO entry using its position.
   *
   * Similar to slave_config::register_pdo_entry(), but not using PDO indices
   * but offsets in the PDO mapping, because PDO entry indices may not be unique
   * inside a slave's PDO mapping. An error is raised, if
   * one of the given positions is out of range.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   *
   * \returns Offset of the PDO entry's process data.
   */
  ECAT_DECL offset register_pdo_entry_by_position(std::uint16_t sync_index,
                                                  std::uint32_t pdo_pos,
                                                  std::uint32_t entry_pos,
                                                  domain_index_type domain);

  /** Configure distributed clocks.
   *
   * Sets the AssignActivate word and the cycle and shift times for the sync
   * signals.
   *
   * The AssignActivate word is vendor-specific and can be taken from the XML
   * device description file (Device -> Dc -> AssignActivate). Set this to zero,
   * if the slave shall be operated without distributed clocks (default).
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   *
   * \attention The \a sync1_shift time is ignored.
   */
  ECAT_DECL void config_dc(std::uint16_t assign_activate,
                           std::uint32_t sync0_cycle_time,
                           std::int32_t sync0_shift_time,
                           std::uint32_t sync1_cycle_time,
                           std::int32_t sync1_shift_time = 0);

  /** Add a configuration value for an SDO.
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void add_sdo(sdo_idx index, bool complete_access,
                         const std::uint8_t *data_ptr, std::size_t data_size);

  template <typename T>
  void add_sdo(sdo_idx index, bool complete_access, T &data) {
    add_sdo(index, complete_access, sdo_data_traits<T>::data_ptr(data),
            sdo_data_traits<T>::data_size(data));
  }

  ECAT_DECL void config_idn(std::uint8_t drive_no, std::uint16_t idn,
                            al_state_type al_state, slice<std::uint8_t> data);

  /** Set the size of the CoE emergency ring buffer.
   *
   * The initial size is zero, so all messages will be dropped. This method can
   * be called even after master activation, but it will clear the ring buffer!
   *
   * This method has to be called in non-realtime context before
   * master::activate().
   */
  ECAT_DECL void set_emerg_size(std::uint64_t elements);

  /** Read and remove one record from the CoE emergency ring buffer.
   *
   * A record consists of 8 bytes:
   *
   * Byte 0-1: Error code (little endian)
   * Byte   2: Error register
   * Byte 3-7: DataS
   */
  ECAT_DECL std::optional<std::uint8_t> pop_emerg() noexcept;

  /** Clears CoE emergency ring buffer and the overrun counter. */
  ECAT_DECL void clear_emerg();

  /** Read the number of CoE emergency overruns.
   *
   * The overrun counter will be incremented when a CoE emergency message could
   * not be stored in the ring buffer and had to be dropped. Call
   * slave_config::emerg_clear() to reset the counter.
   */
  ECAT_DECL std::int32_t emerg_overruns();

private:
  master_type *m_;
  config_index_type idx_{};

  // to avoid access to std::system_category().
  std::error_code success_code_;
};
} // namespace ecat

#endif // ECAT_SLAVE_CONFIG_HPP
