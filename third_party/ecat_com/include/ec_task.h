#ifndef EC_TASK_H
#define EC_TASK_H

#include <sched.h>
#include <stdint.h>
#include <stdbool.h>
#include "ServoIpcStruct.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
  long getsync();
  typedef struct ec_spi ec_spi_t;
  typedef struct ec_task ec_task_t;

  enum ec_dc_mode
  {
    EC_DC_UNUSED,
    EC_DC_MASTER_FOLLOW_SLAVE,
    EC_DC_SLAVE_FOLLOW_MASTER,
  };

  struct ec_pdo_entry_index
  {
    int index;
    int subindex;
  };

  typedef struct ec_slot_info_t {
    int slave_id;
    int slot_id;
    char slot_type[255];
    char slot_name[255];
  }ec_slot_info;
  
  typedef struct ec_slots_info_t {
    int slots_count;
    ec_slot_info ec_slot[255];
  }ec_slots_info;

  /// Creates a new EtherCAT task.
  /** @param master EtherCAT master to use for the task.
   *  @return the created EtherCAT task.
   */
  ec_task_t *ec_task_new(int master);

  /// Destroys an EtherCAT task.
  /** @param task an EtehrCAT task.
   */
  void ec_task_finalize(ec_task_t *task);

  /// Configure a task using an ENI XML file.
  /** @param task an EtherCAT task.
   *  @param path path to the ENI XML file.
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_load_eni(ec_task_t *task, const char *path, int64_t* cycle_time);

  /// Sets the cycle time of a task.
  /** @param task an EtherCAT task.
   *  @param t cycle time in nanoseconds
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_set_cycle_time(ec_task_t *task, int64_t t, int64_t shiftTime);

  /// Sets the DC synchronization mode.
  /** @param task an EtherCAT task.
   *  @param mode DC mode to set.
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_set_dc_mode(ec_task_t *task, enum ec_dc_mode mode);

  
  int ec_task_set_interval(ec_task_t *task, int interval);

  /// Sets the priority of the realtime thread.
  /** @param task an EtherCAT task.
   *  @param prio priority of the task.
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_set_priority(ec_task_t *task, int prio);
  int ec_task_set_record(ec_task_t *task, bool recordFlag);

  /// Sets the CPU affinity of the realtime thread.
  /** @param task an EtherCAT task.
   *  @param cpus a cpu_set_t struct. Refer sched(7) for details.
   *  @param cpu_set_size size of a cpu_set_t struct. You can always pass
   * sizeof(cpu_set_t).
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_set_affinity(ec_task_t *task, const cpu_set_t *cpus,
                           size_t cpu_set_size);

  /// Sets the config callback function.
  /** @param task an EtherCAT task.
   *  @param cb the callback function.
   *  @param user_data data passed to the callback function.
   *  @return zero on success. Otherwise non-zero.
   *
   *  The config callback function will be called after all slaves are
   * configured.
   */
  int ec_task_set_config_callback(ec_task_t *task, void cb(void *),
                                  void *user_data);

  /// Sets the config callback function.
  /** @param task an EtherCAT task.
   *  @param cb the callback function.
   *  @param user_data data passed to the callback function.
   *  @return zero on success. Otherwise non-zero.
   *
   *  The activation callback function will be called after the master is
   * activated.
   */
  int ec_task_set_activation_callback(ec_task_t *task, void cb(void *),
                                      void *user_data);

  /// Sets the cycle callback function.
  /** @param task an EtherCAT task.
   *  @param cb the callback function.
   *  @param user_data data passed to the callback function.
   *  @return zero on success. Otherwise non-zero.
   *
   *  The cycle callback function will be called in every communication cycle
   *  after receiving datagrams.
   */
  // __attribute__((deprecated))
  int ec_task_set_cycle_callback(ec_task_t *task, void cb(void *),
                                 void *user_data);

  /// Sets the receive callback function.
  /** @param task an EtherCAT task.
   *  @param cb the callback function.
   *  @param user_data data passed to the callback function.
   *  @return zero on success. Otherwise non-zero.
   *
   *  The receive callback function will be called in every communication cycle
   *  after receiving datagrams.
   */
  int ec_task_set_receive_callback(ec_task_t *task, void cb(void *),
                                   void *user_data);

  /// Sets the send callback function.
  /** @param task an EtherCAT task.
   *  @param cb the callback function.
   *  @param user_data data passed to the callback function.
   *  @return zero on success. Otherwise non-zero.
   *
   *  The send callback function will be called in every communication cycle
   *  after sending datagrams
   */
  int ec_task_set_send_callback(ec_task_t *task, void cb(void *),
                                void *user_data);

  /// Configure ethercat slaves.
  /** @param task an EtherCAT task.
   *  @return zero on success. Otherwise non-zero.
   *
   * Usually the application do not need to call this function.
   * It will be automatically invoked during ec_task_start().
   */
  int ec_task_configure(ec_task_t *task);

  /// Start the EtherCAT task.
  /** @param task an EtherCAT task.
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_start(ec_task_t *task);

  /// Wait for the task to terminate.
  /** @param task an EtherCAT task.
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_wait(ec_task_t *task);

  /// Request the task to break
  /** @param task an EtherCAT task.
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_break(ec_task_t *task);

  /// Returns number of slaves.
  /** @param task an EtherCAT task.
   *  @return a positive value indicating the number of the slaves. Otherwise
   * non-zero.
   *  @note The function call only be called after the task is configured.
   */
  int ec_task_get_slave_count(ec_task_t *task);

  /// Returns mapped PDO entries of a slave.
  /** @param task an EtherCAT task.
   *  @param slave_pos position of a slave.
   *  @return an array of struct ec_pdo_entry_index on success, or else NULL.
   *
   *  { -1, -1 } indicates the end of the return array.
   *
   * @note this function can only be called after the task is configured.
   */
  const struct ec_pdo_entry_index *
  ec_task_get_mapped_pdo_entries(ec_task_t *task, int slave_pos);

  /// Returns the profile number of a slave.
  /** @param task an EtherCAT task.
   * @param slave_pos position of a slave.
   * @return the profile number on success, or else non-zero.
   */
  int ec_task_get_profile_no(ec_task_t *task, int slave_pos);

  /// Registers a PDO entry.
  /** @param task an EtherCAT task.
   *  @param addr address of a pointer that will contain the PDO entry's address
   * after activation.
   *  @param slave_pos position of a slave.
   *  @param index index of the PDO entry.
   *  @param subindex subindex of the PDO entry.
   *  @param domain domain of the PDO entry. currently only 0 is allowed.
   *  @param bit_pos the bit offset position 
   *  @return zero on success. Otherwise non-zero.
   */
  int ec_task_register_pdo_entry(ec_task_t *task, void *addr, int slave_pos,
                                 int index, int subindex, uint8_t *bit_pos, int domain);

  /// Executes an SDO download request to write data to a slave.
  /** @param slave_pos slave position
   *  @param sdo_index
   *  @param sdo_subindex
   *  @param complete_access
   *  @param data_ptr
   *  @param data_size
   *  @return zero on success. Otherwise non-zero.
   * 
   * This request is processe by the master state machine. This method blocks,
   * until the request has been processed and may not be called in realtime
   * context.
   */
  int ec_task_sdo_download(ec_task_t *task, int slave_pos, int sdo_index, 
                           int sdo_subindex, bool complete_access,
                           uint8_t const *data_ptr, size_t data_size,
                           uint16_t *abort_code);

  int ec_task_sdo_upload(ec_task_t *task, int slave_pos, int sdo_index,
                         int sdo_subindex, bool complete_access,
                         uint16_t *data_ptr,
                         uint16_t *abort_code);                         

  /// Returns the number of slots in the slave.
  /** @param task
   *  @param slave_pos
   *  @return a positive number indicating the number of slots. Otherwise negative error code.
   */
  int ec_task_slave_slots_count(ec_task_t *task, int slave_pos);

  /// Return the slots info in ENI file;
  int ec_task_get_slots_info(ec_task_t *task, ec_slots_info* slots_info);

  /// Returns the increment value of PDO index of each slots in the slave.
  int ec_task_slave_slot_pdo_increment(ec_task_t *task, int slave_pos);

  /// Returns the increment value of CiA 402 process data index of each slots.
  int ec_task_slave_slot_index_increment(ec_task_t *task, int slave_pos);

  /// Set the internal log level.
  /** @param level 0 (most verbose) ~ 6
   */
  void ec_task_set_log_level(int level);

  int ec_task_rx_pdos_size(ec_task_t *task, int slave_pos);

  int ec_task_tx_pdos_size(ec_task_t *task, int slave_pos);

  // PWM C语言接口
  // 初始化
  int ec_pwm_init();
  // 打开指定PWM通道（0，1，2，3）
  int ec_pwm_open(int pwmchip);
  // 关闭所有通道
  int ec_pwm_close();
  // 修改指定通道的周期，单位微秒
  int ec_pwm_config(int pwmchip, int num);

  // 查询轴名，vendor_id，product_code
  int ec_task_axis_name(ec_task_t *task, int slave_pos, char *name);
  uint32_t ec_task_axis_vid(ec_task_t *task, int slave_pos);
  uint32_t ec_task_axis_pid(ec_task_t *task, int slave_pos);

  // ipc C语言接口
  // 接收外部进程信息，存进缓冲区
  int ec_ipc_push(ServoControl_t *pTmpReq);
  // 判断缓冲区为空
  bool ec_ipc_empty();
  // 查询缓冲区内信息数量
  int ec_ipc_size();
  // 导出第一个信息
  ServoControl_t *ec_ipc_pop();

  // 初始化IPC
  int ec_ipc_init(int chip);
  // 关闭IPC
  int ec_ipc_deinit(int socketFd);
  // 发送信息
  int ec_ipc_send(int socketFd, uint8_t *data, int len);
  // 接收信息
  int ec_ipc_receive(int socketFd, uint8_t *data, int len);
  // 接收并解析信息
  int ec_ipc_poll(int socketFd, ServoControl_t **pReq);

  // 存入算法缓冲区
  int ec_algo_push(ServoControl_t *pTmpReq);
  // 断缓冲区为空
  bool ec_algo_empty();
  // 查询缓冲区内信息数量
  int ec_algo_size();
  // 导出第一个信息
  ServoControl_t *ec_algo_pop();

  // 释放内存
  int ec_algo_ipc_del(ServoControl_t *pTmpReq);
  
  // 创建算法线程
  int ec_algo_create_thread(int pori);
  // 关闭算法线程
  int ec_algo_close_thread();
  // 注册回调函数
  int ec_algo_set_cycle(void cb());
  // 结束算法线程
  int ec_algo_break();

  // 创建一个spi对象
  ec_spi_t *ec_spi_new();
  // 初始化spi
  int ec_spi_init(ec_spi_t *spi, const char *file);
  // 读取spi设备文件名称
  int ec_spi_get_device(ec_spi_t *spi, char *device, int size);
  // 退出spi并释放
  void ec_spi_finalize(ec_spi_t *spi);
  // 创建spi线程（固定在#0核）并注册回调函数
  int ec_spi_create_pthread(ec_spi_t *spi, int priority, int64_t clock_cycle, void func());
  // 写入spi数据
  int ec_spi_set_parameter(ec_spi_t *spi, unsigned short *data);
  // 读取spi数据
  int ec_spi_get_parameter(ec_spi_t *spi, unsigned short *data);

#ifdef __cplusplus
}
#endif

#endif // EC_TASK_H
