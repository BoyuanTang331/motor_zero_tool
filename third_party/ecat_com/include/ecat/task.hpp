#ifndef ECAT_TASK_HPP
#define ECAT_TASK_HPP

#include <ecat/master.hpp>
#include <ecat/slave_config.hpp>
#include <functional>
#include <memory>
#include <ServoIpcStruct.h>
#include "ecat/sdo_request.hpp"
#include "ecat/s2s_func.hpp"
#include "ecat/run_time_stat.hpp"

namespace ecat
{
  void setSync();
  long getSync();
  enum class dc_mode
  {
    not_using_dc,
    master_follow_slave,
    slave_follow_master,
};
typedef struct slot_info {
  int slave_id;
  int slot_id;
  std::string slot_type;
  std::string slot_name;
}slot_info;
typedef std::vector<slot_info> slots_info;
struct slave_config_info;

enum PdoEntryType{
  PdoTypeRx = 0,
  PdoTypeTx = 1,
};

// runtime pdo注册管理
typedef struct PdoRegInfoT {
  uint16_t masterId = -1;
  uint16_t slaveId = -1;
  PdoEntryType pdoType = PdoTypeTx; // 默认TxPdo，因为权限最小
  pdo_entry_idx entry;
  // PdoEntryIdx entry{0};
  size_t bitLen = 0;
  std::string datatype; // 数据类型
  volatile void *dataPtr = nullptr;
  // bool isBind = false; //是否已经绑定变量
} PdoRegInfo;
typedef std::vector<PdoRegInfo *> PdoRegInfoVec;


class task
{
public:
  using dc_mode_type = enum dc_mode;

  using config_callback_type = std::function<void()>;
  using activation_callback_type = std::function<void()>;
  using cycle_callback_type = std::function<void()>;
  using receive_callback_type = std::function<void()>;
  using send_callback_type = std::function<void()>;

  explicit task(int master_index);

  task(const task &) = delete;
  task &operator=(const task &) = delete;

  ~task();

  void setAsMaster();
  // 主线程的周期必须是最小的，而且从线程的周期必须是主线程的整数倍;
  void setAsSlave(task* master, int period_times, int num);//从主站调用此函数来进行set
  void setAsSlave(task* master, int period_times);//slave1调用此函数来进行set
  void setAsSlave2(task* master, int period_times);//slave2调用此函数来进行set

  /** Parse the eni xml file to obtain information,
   *  include vendor ID, Product Code, PDO, SM, DC cycle time, etc.
   */
  void load_eni(const std::string &path, int64_t& cycle_time);

  /** Set DC cycle time, only can used in ESI mode. */
  void cycle_time(std::int64_t t, std::int64_t shiftTime);

  /** Set sync mode, only can used in ESI mode. */
  void dc_mode(dc_mode_type mode);

  void set_interval(int interval);

  /** Set the priority level of real time thread. */
  void priority(int prio);

  /** Set whether record EtherCAT time cost. */
  void record(bool recordFlag);

  /** Set the cpu affinity of real time thread. */
  void cpu_affinity(const cpu_set_t *cpus, size_t cpu_set_size);

  /** Set config callback function.
   *  This function will be called after all salves are configured. 
   */
  void set_config_callback(config_callback_type cb);

  /** Set activation callback function.
   *  This function will be called after the master is activated. 
   */
  void set_activation_callback(activation_callback_type cb);

  /** Set cycle callback function.
   *  This function will be called in every communication cycle
   *  after receiving datagrams.
   */
  // __attribute__((deprecated))
  void set_cycle_callback(cycle_callback_type cb);

  /** Set the receive callback function.
   *  This function will be called in every communication cycle
   *  after receiving datagrams.
   */
  void set_receive_callback(receive_callback_type cb);

  /** Set the send callbeck function.
   *  This function will be called in every communication cycle
   *  after sending datagrams.
   */
  void set_send_callback(send_callback_type cb);

  /** Configure EtherCAT slaves.
   *  Usually the application do not need to call this function.
   *  It will be automatically invoked during start().
   */
  void configure(std::int64_t parseXml = 0);

  /** Start the EtherCAT task. */
  virtual void start();

  /** Wait the task to terminal. */
  void wait();

  /** Resource recovery */
  void resource_recovery();

  bool isExiting();


    /** Request the task to break. */
  void break_();

  // Preconditions: configure() finished.
  /** Get the number of slaves. */
  std::uint16_t slave_count() const;
  std::vector<slave_by_alias> slave_alias_count();
  int get_slave_by_alias(slave_by_alias slave_alias);
  int get_slave_by_alias(uint16_t alias);
  pdo_cfg* get_slot_rx_config(int slave_pos, int slot_pos);
  pdo_cfg* get_slot_tx_config(int slave_pos, int slot_pos);
  
  std::vector<pdo_entry_info> get_io_rx_config(int slave_pos, int slot_pos);
  std::vector<pdo_entry_info> get_io_tx_config(int slave_pos, int slot_pos);

  // Preconditions: configure() finished.
  std::vector<ecat::pdo_entry_idx> mapped_pdo_entries(std::uint16_t pos) const;

  // Preconditions: configure() finished.
  /** Get the profile no of slave, such as 402. */
  int profile_no(std::uint16_t slave_pos);

  ecat::slice<std::uint8_t> domain_data(ecat::domain_index_type domain = 0);

  void register_pdo_entry(void *addr, std::uint16_t slave_pos,
                          ecat::pdo_entry_idx idx, std::uint8_t *bit_pos,
                          ecat::domain_index_type domain);

  template <typename T>
  void register_pdo_entry(T *&addr, std::uint16_t slave_pos,
                          ecat::pdo_entry_idx idx, std::uint8_t *bit_pos,
                          ecat::domain_index_type domain = 0)
  {
    register_pdo_entry(&addr, slave_pos, idx, bit_pos, domain);
  }

  /// Registers a PDO entry.
  /** @param addr address of a pointer that will contain the PDO entry's address
   * after activation.
   *  @param slave_pos position of a slave.
   *  @param idx index of the PDO entry, include index and subindex.
   *  @param domain domain of the PDO entry. currently only 0 is allowed.
   *  @return true on success. Otherwise false.
   */
  template <typename T>
  bool try_register_pdo_entry(T *&addr, std::uint16_t slave_pos,
                              ecat::pdo_entry_idx idx, std::uint8_t *bit_pos = nullptr,
                              ecat::domain_index_type domain = 0)
  {
    try
    {
      // if (S2SConfig::use().has(std::make_pair(slave_pos, idx)))
      {
        S2SConfig::use().get((void **)&addr, sizeof(*addr), std::make_pair(slave_pos, idx));
      }
      register_pdo_entry(addr, slave_pos, idx, bit_pos, domain);
      return true;
    }
    catch (const std::exception &)
    {
      return false;
    }
  }

  template <typename T>
  bool try_register_pdo_entry(T *&addr, std::size_t size, std::uint16_t slave_pos,
                              ecat::pdo_entry_idx idx, std::uint8_t *bit_pos = nullptr,
                              ecat::domain_index_type domain = 0)
  {
    try
    {

      // if (S2SConfig::use().has(std::make_pair(slave_pos, idx)))
      {
        S2SConfig::use().get((void **)&addr, size, std::make_pair(slave_pos, idx));
      }
      register_pdo_entry(addr, slave_pos, idx, bit_pos, domain);
      return true;
    }
    catch (const std::exception &)
    {
      return false;
    }
  }

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

  /**
   * Executes an SDO upload request to write data to a slave.
   *
   * This request is processe by the master state machine. This method blocks,
   * until the request has been processed and may not be called in realtime
   * context.
   */
uint16_t sdo_upload(std::uint16_t pos, sdo_idx t_sdo_idx, bool complete_access);


  /** Get the slot count of multi-axis motion controller. */
  std::uint16_t slots_count(std::uint16_t slave_pos) const;

  std::uint16_t slot_pdo_increment(std::uint16_t slave_pos) const;

  /** Get the slot index increment of multi-axis motion controller.
   *  Usual is 0x800.
   */
  std::uint16_t slot_index_increment(std::uint16_t slave_pos) const;

  std::uint16_t rx_pdos_size(std::uint16_t slave_pos) const;

  std::uint16_t tx_pdos_size(std::uint16_t slave_pos) const;

  std::uint32_t axis_vid(std::uint16_t slave_pos) const;
  std::uint32_t axis_pid(std::uint16_t slave_pos) const;
  std::uint16_t axis_name(std::uint16_t slave_pos, std::string& namep);

  wc_state_type get_wcstate();

  void check_eni(bool check = true);

  void setSlaveState(std::uint16_t slave_pos, al_state_type als);

  sdo_request* create_sdo(std::uint16_t slave_pos, sdo_idx idx, std::size_t size, bool complete_access);

  master* get_master_ptr();

  void set_configure(int log_level);

  slave_config_info* get_slave_info(std::uint16_t slave_pos);

  RunTimeData* get_runtime_data();

  std::pair<char*, int> get_pdo_list(std::uint16_t slave_pos);

  int compare_tx_pdo(std::uint16_t slave_pos, std::uint16_t idx, std::uint8_t subIdx);

  slots_info* get_slots_info();

  void release();

  void auto_register_pdo_entry(ecat::pdo_cfg* pdos, int slave_pos, ecat::PdoEntryType pdoType);

  void auto_register_pdo_entry(std::vector<ecat::pdo_entry_info> pdos, int slave_pos, ecat::PdoEntryType pdoType);

  PdoRegInfo *get_pdo_by_entry(int slave_pos, ecat::pdo_entry_idx entry);

  PdoRegInfoVec get_pdo_register_info();

  PdoRegInfoVec get_pdo_register_info(int slave_pos);

protected:
  virtual void on_config();

  virtual void on_activation();

  virtual void on_cycle();

  virtual void on_receive();

  virtual void on_send();

protected:
  struct impl;

  std::unique_ptr<impl> d;
  PdoRegInfoVec PdoRegInfos;
  std::vector<slave_by_alias> SlaveByAlias;
public:
  std::vector<std::string> mName;
  slots_info SlotsInfo;
  char tcp_buffer[2048] = {0};
  std::vector<sdo_request*> SdoPtrS;
  int cycleTimer = 0;
  int task_index = 0;
};

class TaskMgr
{
public:
    TaskMgr();
    ~TaskMgr();
    static TaskMgr& getInstance();
    task* getTask(){return mTaskptr;}
    void setTask(task& ecatTask){mTaskptr = &ecatTask;}
    task** getTaskDptr() { return &mTaskptr; }

protected:
    static TaskMgr mInstance;
    task* mTaskptr = nullptr;
};

} // namespace ecat

#endif // ECAT_TASK_HPP

