#ifndef NIIC_API_HPP
#define NIIC_API_HPP

#include "ecat/task.hpp"
#include "ecat/master.hpp"
#include "ecat/types.hpp"
#include "ecat/rokae/extra.hpp"
#include "ecat/s2s_func.hpp"


namespace ecat {

struct ec_slave_sii_t{
  uint16_t slave_position = 0;
  uint16_t offset = 0;
  uint32_t nwords = 0;
  uint16_t *words = nullptr;

  ~ec_slave_sii_t(){
    if (words)
    {
      delete[] words;
      words = nullptr;
      nwords = 0;
    }
  }
};

struct ec_slave_foe_t{
  uint32_t password = 0;
  uint16_t slave_position = 0;
  uint16_t offset = 0;
  size_t buffer_size = 0;
  uint8_t *buffer = nullptr;
  size_t data_size = 0;
  uint32_t result;
  char file_name[255] = {0};

  ~ec_slave_foe_t(){
    if (buffer)
    {
      delete[] buffer;
      buffer = nullptr;
      buffer_size = 0;
      data_size = 0;
    }
  }
};



/////////////////////////////// Task ///////////////////////////////////

/*
调用所有API需先创建ECAT任务对象：
    ecat::task* task_;
    create_task(task_);
    此命令会自动创建主站对象，第一个主站编号通常设置为0

typedef struct {
    char flag = 0;
    int maxDelay = 0;
    int minDelay = 0;
    int avgDelay = 0;
}RunTimeData;

*/

// 初始化ECAT任务对象
int create_task(task *task_, int masterOrder = 0, int affinity = 1, int priority = 50, 
                std::int64_t cycleTime = 1000000, std::int64_t shiftTime = 0, std::string eniFilePath = "");

// 重新扫描所有从站
int rescan_Slaves(task* task_);

// ENI校验，默认开启
int check_eni_vendor(task* task_, bool check_eni = true);

// 获取延迟统计
int get_latency_in_task(task* task_, RunTimeData* data);






/////////////////////////////// SDO ///////////////////////////////////
/* 
enum class request_state {
  usused = 0,
  busy = 1,
  success = 2,
  error = 3,
};

struct sdo_idx {
  std::uint16_t idx;
  std::uint16_t sub_idx;
};

Function of sdo_request:

uint16_t param = 0;
sdo_request* sdo_;

1. get sdo state: request_state state = sdo_->state();
2. write data to: sdo_->data<uint16_t>(param);
3. read data from: param = sdo_->data<uint16_t>();

 */ 

int create_sdo(sdo_request*& sdo_, task* task_, std::uint16_t slave_position, sdo_idx idx, std::size_t size, bool complete_access);

int delete_sdo(sdo_request* sdo_);

int read_sdo(sdo_request* sdo_, std::uint32_t timeout);

int write_sdo(sdo_request* sdo_, std::uint32_t timeout);

int get_sdo_state(sdo_request* sdo_, request_state& state);

int sdo_data(sdo_request* sdo_, int& data, int data_type, int op_mode);

int change_sdo(sdo_request* sdo_, sdo_idx idx);




/////////////////////////////// EEPROM & FoE ///////////////////////////////////


int read_eeprom(task* task_, ec_slave_sii_t* info);

int write_eeprom(task* task_, ec_slave_sii_t* info);


int read_foe(task* task_, ec_slave_foe_t* info, int fileNameLen);

int write_foe(task* task_, ec_slave_foe_t* info, int fileNameLen);






/////////////////////////////// Slave ///////////////////////////////////

/*
enum class al_state_type : std::uint8_t {
  init = 1,
  preop = 2,
  safeop = 4,
  op = 8,
  boot = 3,
};

enum class slave_port_type {
  not_implemented,
  not_configured,
  ebus,
  mii,
};

struct slave_port_link {
  bool link_up;
  bool loop_closed;
  bool signal_detected;
};

struct slave_port_info {
  slave_port_type desc;
  slave_port_link link;
  std::uint32_t receive_time;
  std::uint16_t next_slave;
  std::uint32_t delay_to_next_dc;
};
*/

int get_Slave_State(task* task_, std::uint16_t slave_position, std::uint8_t& al_state);

int set_Slave_State(task* task_, std::uint16_t slave_position, std::uint8_t al_state);

// get slave configuration
int get_slave_config(task* task_, std::uint16_t slave_position, slave_config_info *data);

// get slots info
int get_slots_info(task* task_, slots_info *data);

// get slave ports info
int rescue_scan(task* task_, std::uint16_t slave_position, int port_pos, slave_port_info& data);

// 添加S2S，ESI模式
int regist_s2s_pdos(task* task_, std::uint16_t src_pos, pdo_entry_idx src_idx, std::uint16_t dst_pos, pdo_entry_idx dst_idx);





/////////////////////////////// Master ///////////////////////////////////


int get_Master_State(task* task_, std::uint8_t& al_state);

int set_Master_State(task* task_, std::uint8_t al_state);

// 获取丢帧统计，重置需重启服务
int get_lostframe_count(task* task_, int& ls_count);

// 获取WKC状态
int get_working_count_state(task* task_, wc_state_type& wc_state);

// 获取实际从站连接数
int get_slaves_responding(task* task_, int& res);




/////////////////////////////// Tools ///////////////////////////////////

// EC状态变量转为string
std::string to_strng_al_state(std::uint8_t al_state);




/////////////////////////////// Register ///////////////////////////////////

// get register info
int read_register(task* task_, std::uint16_t slave_position, std::uint16_t address, std::uint16_t size, std::uint8_t *data_);

// set register info
int write_register(task* task_, std::uint16_t slave_position, std::uint16_t address, std::uint16_t size, std::uint8_t* data_);

}


#endif