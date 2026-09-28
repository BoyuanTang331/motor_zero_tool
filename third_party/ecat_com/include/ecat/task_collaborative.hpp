#ifndef ECAT_TASK_COLLAB_HPP
#define ECAT_TASK_COLLAB_HPP

#include "buffer_locked.hpp"
#include "task.hpp"
#include "task_interface.hpp"

namespace ecat {


class task_collaborative: public task
{
protected:
  buffer_locked buffer_;
public:
  task_collaborative(int master_index):task(master_index)
  {
    
  }
  virtual ~task_collaborative()
  {
  }
  bool init_buffer(int item_size, int max_number, buffer::full_add_mode full_mode)
  {
    return buffer_.init(item_size, max_number, full_mode);
  }
};

}

#endif // ECAT_TASK_COLLAB_HPP
