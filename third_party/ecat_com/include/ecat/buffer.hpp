#ifndef ECAT_BUFFER_INTERFACE_HPP
#define ECAT_BUFFER_INTERFACE_HPP


#include <vector>
#include <stdlib.h>

namespace ecat {

/*
class T must implement operator=
*/
class buffer
{
public:  
  typedef enum full_add_mode
  {
    full_add_fail = 0,
    full_add_cover = 1
  }full_add_mode;

protected:
  char* buffers_ = nullptr;
  int max_size_ = 0;
  int item_size_ = 0;
  int size_ = 0;
  int head_ = 0;
  int tail_ = 0;
  full_add_mode full_mode_ = full_add_cover;
  
public:
  buffer()
  {

  }
  virtual ~buffer()
  {
    if(buffers_ != nullptr)
    {
      free(buffers_);
      buffers_ = nullptr;
    }
  }

  bool init(int item_size, int max_number, full_add_mode full_mode)
  {
    max_size_ = max_number;
    buffers_ = (char*)malloc(item_size * max_number);
    item_size_ = item_size;
    head_ = 0;
    tail_ = -1;
    size_ = 0;
    full_mode_ = full_mode;
    return true;
  }

  bool is_empty()
  {
    return size_ == 0;
  }

  bool is_full()
  {
    return size_ == max_size_;
  }

  virtual bool push_back(void* item_buffer);
  virtual bool get_front(void* item_buffer);
  virtual bool pop_front(void* item_buffer);
  virtual bool get_back(void* item_buffer);
  virtual bool pop_back(void* item_buffer);
  virtual void clear();
};

}

#endif // ECAT_BUFFER_INTERFACE_HPP
