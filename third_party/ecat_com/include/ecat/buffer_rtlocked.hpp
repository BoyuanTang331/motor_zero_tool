#ifndef ECAT_BUFFER_LOCKED_HPP
#define ECAT_BUFFER_LOCKED_HPP

#include "buffer.hpp"
#include "thread_rtlock.hpp"

namespace ecat {

class buffer_rtlocked: public buffer
{
private:
  thread_rtlock lock_;
public:
  buffer_rtlocked()
  {

  }
  virtual ~buffer_rtlocked()
  {
  }

  virtual bool push_back(void* item_buffer)
  {
    auto_rtlock lock(&lock_);
    return buffer::push_back(item_buffer);
  }
  virtual bool get_front(void* item_buffer)
  {
    auto_rtlock lock(&lock_);
    return buffer::get_front(item_buffer);
  }
  virtual bool pop_front(void* item_buffer)
  {
    auto_rtlock lock(&lock_);
    return buffer::pop_front(item_buffer);
  }
  virtual bool get_back(void* item_buffer)
  {
    auto_rtlock lock(&lock_);
    return buffer::get_back(item_buffer);
  }
  virtual bool pop_back(void* item_buffer)
  {
    auto_rtlock lock(&lock_);
    return buffer::pop_back(item_buffer);
  }
  virtual void clear()
  {
    auto_rtlock lock(&lock_);
    return buffer::clear();
  }
};

}

#endif // ECAT_BUFFER_LOCKED_HPP
