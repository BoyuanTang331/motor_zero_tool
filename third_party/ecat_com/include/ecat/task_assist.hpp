#ifndef ECAT_TASK_ASSIST_HPP
#define ECAT_TASK_ASSIST_HPP

#include "buffer_locked.hpp"
#include "task_interface.hpp"

namespace ecat {

class task_assist: public task_interface
{
protected:
  buffer_locked buffer_;
  buffer_locked* ecat_buff_ref_;
public:
  task_assist()
  {
    ecat_buff_ref_ = nullptr;
  }
  virtual ~task_assist()
  {
  }

  bool init_buffer(int item_size, int max_number, buffer::full_add_mode full_mode)
  {
    return buffer_.init(item_size, max_number, full_mode);
  }
 
  buffer_locked& get_buffer()
  {
    return buffer_;
  }

  void attach_ecat_buff(buffer_locked* ecat_buff)
  {
    ecat_buff_ref_ = ecat_buff;
  }

  buffer_locked* get_ecat_buff()
  {
    return ecat_buff_ref_;
  }
  
};

}

#endif // ECAT_TASK_ASSIST_HPP
