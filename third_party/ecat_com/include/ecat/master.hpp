// master.hpp
// ~~~~~~~~~~

/** \file
 *
 * EtherCAT master C++ application interface.
 *
 * \defgroup CppApplicationInterface EtherCAT C++ Application Interface
 *
 * EtherCAT C++ interface for realtime applications. This interface is designed
 * for realtime modules that want to use EtherCAT. There are functions to
 * request a master, to map process data, to communicate with slaves via CoE
 * and to configure and activate the bus.
 *
 * \ingroup CppApplicationInterface
 * @{
 */

#ifndef ECAT_MASTER_HPP
#define ECAT_MASTER_HPP

#include <ecat/detail/config.hpp>
#include <ecat/detail/file.hpp>
#include <ecat/slave_config.hpp>
#include <ecat/types.hpp>
#include <ecat/version.hpp>
#include <map>
#include <sys/mman.h>

/** EtherCAT interfaces */
namespace ecat {
class master;

class domain {
public:
  typedef class master master_type;

  domain(master_type &m, domain_index_type idx) : m_(&m), idx_(idx) {}

  master_type &master() { return *m_; }

  domain_index_type index() const { return idx_; }

  ECAT_DECL std::size_t size();

  ECAT_DECL domain_state state();

  ECAT_DECL void process();

  ECAT_DECL void queue();

private:
  master_type *m_;
  domain_index_type idx_{};
};

/// EtherCAT master.
class master {
  friend class slave_config;

  friend class domain;

public:
  enum access_type { read_only, read_write };

  using index_type = unsigned long;
  using size_type = std::size_t;
  using domain_index_type = ecat::domain_index_type;
  using domain_type = class domain;
  using domain_data_placement_type = class domain_data_placement;

  master(const master &) = delete;

  master(master &&) = delete;

  detail::file &file() { return file_; }

  /// Open an EtherCAT master.
  ECAT_DECL master(index_type idx, access_type access);

  master &operator=(const master &) = delete;

  master &operator=(master &&) = delete;

  /// Get the number of EtherCAT masters.
  static ECAT_DECL size_type master_count();

  /// Reserves the EtherCAT master in order to operate it.
  ECAT_DECL void reserve();

  /** Creates a process data domain.
   */
  ECAT_DECL domain_index_type create_domain();

  /// Obtains a domain object.
  domain_type domain(domain_index_type idx) { return domain_type{*this, idx}; }

  /// Obtains domain's data.
  ECAT_DECL slice<std::uint8_t> domain_data(domain_index_type idx);

  /** setup the domain's process data memory.
   *
   * Call this after all PDO entries have been registered and before activating
   * the master.
   *
   * Call this if you need to access the domain memory before activating the
   * master
   */
  ECAT_DECL void setup_domain_memory();

  /** Finishes the configuration phase and prepares for cyclic operation.
   *
   * This function tells the master that the configuration phase is finished and
   * the realtime operation will begin. The function allocates internal memory
   * for the domains and calculates the logical FMMU addresses for domain
   * members. It tells the master state machine that the bus configuration is
   * now to be applied.
   *
   * \attention After this function has been called, the realtime application is
   * in charge of cyclically calling master::send() and
   * master::receive() to ensure bus communication. Before calling this
   * function, the master thread is responsible for that, so these functions may
   * not be called! The method itself allocates memory and should not be called
   * in realtime context.
   */
  ECAT_DECL void activate();

  /** Deactivates the master.
   *
   * Removes the bus configuration. All objects created by
   * master::create_domain(), master::configure_slave(), domain::data() and
   * slave_config::create_sdo_request() are freed, so pointers to them
   * become invalid.
   *
   * This method should not be called in realtime context.
   */
  ECAT_DECL void deactivate();

  /** Set interval between calls to ecrt_master_send().
   *
   * This information helps the master to decide, how much data can be appended
   * to a frame by the master state machine. When the master is configured with
   * --enable-hrtimers, this is used to calculate the scheduling of the master
   * thread.
   */
  ECAT_DECL void set_send_interval(unsigned long interval_us);

  /** Sends all datagrams in the queue.
   *
   * This method takes all datagrams, that have been queued for transmission,
   * puts them into frames, and passes them to the Ethernet device for sending.
   *
   * Has to be called cyclically by the application after master::activate()
   * has returned.
   *
   * Returns the number of bytes sent.
   */
  ECAT_DECL std::size_t send();

  /** Fetches received frames from the hardware and processes the datagrams.
   *
   * Queries the network device for received frames by calling the interrupt
   * service routine. Extracts received datagrams and dispatches the results to
   * the datagram objects in the queue. Received datagrams, and the ones that
   * timed out, will be marked, and dequeued.
   *
   * Has to be called cyclically by the realtime application after
   * master::activate() has returned.
   */
  ECAT_DECL void receive();

  /** Retry configuring slaves.
   *
   * Via this method, the application can tell the master to bring all slaves to
   * OP state. In general, this is not necessary, because it is automatically
   * done by the master. But with special slaves, that can be reconfigured by
   * the vendor during runtime, it can be useful.
   */
  ECAT_DECL void reset();

  /** Reads the current master state.
   *
   * Stores the master state information in the given \a state structure.
   *
   * This method returns a global state. For the link-specific states in a
   * redundant bus topology, use the master::link_state() method.
   */
  ECAT_DECL master_state state();

  /** Reads the current state of a redundant link.
   *
   * Stores the link state information in the given \a state structure.
   *
   * \return Zero on success, otherwise negative error code.
   */
  ECAT_DECL master_state link_state(std::uint32_t dev_idx);

  /** Obtains master information. */
  ECAT_DECL master_info get_info();

  /** Obtains slave information. */
  ECAT_DECL slave_info get_slave_info(std::uint16_t pos);

  /** Obtains the slave configuration information. */
  ECAT_DECL config_info get_config_info(std::uint16_t idx);

  /** Obtains a slave configuration.
   *
   * Creates a slave configuration object for the given slave and returns it.
   * If a configuration for the slave already exists, it will be reused. In the
   * latter case, the given vendor ID and product code are compared to the
   * stored ones. On mismatch, an exception is thrown.
   *
   * This method allocates memory and should be called in non-realtime context
   * before master::activate().
   */
  ECAT_DECL slave_config configure_slave(slave_addr addr, slave_id expected);

  /**
   * Obtains an SDO information.
   */
  ECAT_DECL sdo_info get_sdo_info(std::uint16_t slave_pos,
                                  std::uint32_t sdo_pos);

  /// Obtains an SDO entry information.
  ECAT_DECL sdo_entry_info get_sdo_entry(std::uint16_t slave_pos,
                                         sdo_entry_addr addr);

  /**
   * Executes an SDO download request to write data to a slave.
   *
   * This request is processe by the master state machine. This method blocks,
   * until the request has been processed and may not be called in realtime
   * context.
   */
  ECAT_DECL void sdo_download(std::uint16_t pos, sdo_idx t_sdo_idx,
                              bool complete_access,
                              std::uint8_t const *data_ptr,
                              std::size_t data_size);

  template <typename T>
  void sdo_download(std::uint16_t pos, sdo_idx t_sdo_idx, bool complete_access,
                    const T &arg) {
    sdo_download(pos, t_sdo_idx, complete_access,
                 sdo_data_traits<T>::data_ptr(arg),
                 sdo_data_traits<T>::data_size(arg));
  }

  /** Executes an SDO upload request to read data from a slave.
   *
   * This request is processed by the master state machine. This method blocks,
   * until the request has been processed and may not be called in realtime
   * context.
   */
  ECAT_DECL slice<std::uint8_t> sdo_upload(std::uint16_t pos, sdo_idx t_sdo_idx,
                                           bool complete_access,
                                           slice<std::uint8_t> target);

  template <typename T>
  T sdo_upload(std::uint16_t pos, sdo_idx t_sdo_idx, bool complete_access) {
    T target = T();
    sdo_upload(pos, t_sdo_idx, complete_access,
               slice<std::uint8_t>{reinterpret_cast<std::uint8_t *>(&target),
                                   sizeof(T)});
    return target;
  }

  /** Obtains the register data. 
   * This method blocks, until the request has been 
   * processed and may not be called in realtime context.
   */
  ECAT_DECL void reg_read(std::uint16_t pos, std::uint16_t address, std::uint16_t size, std::uint8_t *data_);

  /** Sets the register data.
   * This method blocks, until the request has been
   * processed and may not be called in realtime context.
   */
  ECAT_DECL void reg_write(std::uint16_t pos, std::uint16_t address, std::uint16_t size, std::uint8_t* data_);

  /** Obtains a PDO information.
   */
  ECAT_DECL pdo_info get_pdo(std::uint16_t slave_pos, std::uint16_t sync_index,
                             std::uint32_t pdo_position);

  /// Obtains PDO entry information.
  ECAT_DECL pdo_entry_info get_pdo_entry(std::uint16_t slave_pos,
                                         std::uint16_t sync_index,
                                         std::uint32_t pdo_pos,
                                         std::uint32_t pdo_entry_pos);

  /// Obtains sync manager information.
  ECAT_DECL sm_info get_sync(std::uint16_t slave_pos, std::uint16_t sm);

  /// Requests a slave to switch to the specified state.
  ECAT_DECL void request_state(std::uint16_t slave_pos, al_state_type state);

  ECAT_DECL void dict_upload(std::uint16_t slave_pos);

  /** Sets the application time.
   *
   * The master has to know the application's time when operating slaves
   * with distributed clocks. The time is not incremented by the master itself,
   * so this function has to be called cyclically.
   *
   * \attention The time passed to this method is used to calculate the phase of
   * the slaves' SYNC0/1 interrupts. It should be called constantly at the same
   * point of the realtime cycle. So it is recommended to call it at the start
   * of the calculations to avoid deviancies due to changing execution times.
   *
   * The time is used when setting the slaves' <tt>System Time Offset</tt> and
   * <tt>Cyclic Operation Start Time</tt> registers and when synchronizing the
   * DC reference clock to the application time via
   * master::sync_reference_clocks().
   */
  ECAT_DECL void set_application_time(std::uint64_t app_time) noexcept;

  /** Queues the DC reference clock drift compensation datagram for sending.
   *
   * The reference clock will by synchronized to the time passed in the
   * sync_time parameter.
   */
  ECAT_DECL void sync_reference_clock_to(std::uint64_t sync_time) noexcept;

  /** Queues the DC reference clock drift compensation datagram for sending.
   *
   * The reference clock will by synchronized to the application time provided
   * by the last call off master::set_application_time().
   */
  ECAT_DECL void sync_reference_clock() noexcept;

  /** Queues the DC clock drift compensation datagram for sending.
   *
   * All slave clocks synchronized to the reference clock.
   */
  ECAT_DECL void sync_slave_clocks() noexcept;

  /** Queues the DC synchrony monitoring datagram for sending.
   *
   * The datagram broadcast-reads all "System time difference" registers (\a
   * 0x092c) to get an upper estimation of the DC synchrony. The result can be
   * checked with the master::sync_monitor_process() method.
   */
  ECAT_DECL void sync_monitor_queue() noexcept;

  /** Processes the DC synchrony monitoring datagram.
   *
   * If the sync monitoring datagram was sent before with
   * master::sync_monitor_queue(), the result can be queried with this
   * method.
   *
   * \return Upper estimation of the maximum time difference in ns.
   */
  ECAT_DECL std::uint32_t sync_monitor_process() noexcept;

  /** Get the lower 32 bit of the reference clock system time.
   *
   * This method can be used to synchronize the master to the reference clock.
   *
   * The reference clock system time is queried via the
   * master::sync_slave_clocks() method, that reads the system time of the
   * reference clock and writes it to the slave clocks (so be sure to call it
   * cyclically to get valid data).
   *
   * \attention The returned time is the system time of the reference clock
   * minus the transmission delay of the reference clock.
   */
  ECAT_DECL std::optional<std::uint32_t> get_reference_clock_time();

  /** Selects the reference clock for distributed clocks.
   *
   * If this method is not called for a certain master, or if the slave
   * configuration pointer is NULL, then the first slave with DC functionality
   * will provide the reference clock.
   */
  ECAT_DECL void select_reference_clock(const slave_config &sc);

  /** get master addressing mode.
  */
 ECAT_DECL addr_mode get_addr_mode();

  /** set master addressing mode.
  */
 ECAT_DECL void set_addr_mode(addr_mode mode);

  /** Writes the Secondary slave address (alias) to the slave's SII.
   */
  ECAT_DECL void write_slave_alias(std::uint16_t pos, std::uint16_t alias);

  /** Obtain the EEPROM by the slave's SII.
   */
  ECAT_DECL int read_slave_sii(std::uint16_t pos, std::uint32_t& sii_nwords, std::uint16_t offset, std::uint16_t* buffer);

  /** Write the EEPROM by the slave's SII.
   */
  ECAT_DECL int write_slave_sii(std::uint16_t pos, std::uint16_t* words, std::uint16_t nwords, std::uint16_t offset);

  /** Obtain the file by the slave's foe.
   */
  ECAT_DECL int read_slave_foe(std::uint16_t pos, char* fileName, int fileNameLen, std::uint16_t offset, std::uint32_t password, 
                                  std::size_t buffer_size, std::uint8_t* buffer, std::size_t& data_size, std::uint32_t& result);

  /** Write the file by the slave's foe.
   */
  ECAT_DECL int write_slave_foe(std::uint16_t pos, char* fileName, int fileNameLen, std::uint16_t offset, std::uint32_t password, 
                                  std::size_t buffer_size, std::uint8_t* buffer, std::size_t& data_size, std::uint32_t& result);

  /** Obtain slave's state.
   */
  ECAT_DECL std::uint8_t get_slave_state(std::uint16_t pos);

  /** Rescan all slaves.
   */
  ECAT_DECL int rescan();

  /** Obtain the process data base addresses for each slave.
   */
  ECAT_DECL std::uint8_t *slave_process_data(std::uint16_t pos);

  /** Obtain the input process data base addresses for each slave.
   */
  ECAT_DECL std::uint8_t *slave_input_pdo(std::uint16_t pos);

  /** Obtain the output process data base addresses for each slave.
   */
  ECAT_DECL std::uint8_t *slave_output_pdo(std::uint16_t pos);

  ECAT_DECL int set_log_level(std::uint16_t level);

  void release();

private:
  ECAT_DECL domain_data_placement_type
  domain_data_placement(domain_index_type idx);

  detail::file file_;
  std::optional<slice<std::uint8_t>> process_data_;
  std::map<domain_index_type, domain_data_placement_type> domains_;
  addr_mode addressing_mode { addr_mode::logical_addressing };

  const std::error_code success_code_;
};
} // namespace ecat

#endif // ECAT_MASTER_HPP

/** @}
 */
