#ifndef ECAT_BUFFER_LOCKED_HPP
#define ECAT_BUFFER_LOCKED_HPP

#include "buffer.hpp"
#include "thread_mutex.hpp"
#include <string>

namespace ecat {

class buffer_locked: public buffer
{
private:
  thread_mutex mutex_;
  int lock_count = 0;
public:
  buffer_locked()
  {

  }
  virtual ~buffer_locked()
  {
  }

  virtual bool push_back(void* item_buffer)
  {
    auto_lock lock(&mutex_);
    if(lock_count > 0)
    {
      __RT(printf("....... lock conflict ........\n."));
      return false;
    }
    lock_count++;
    bool res = buffer::push_back(item_buffer);
    lock_count--;
    return res;
  }

  virtual bool get_front(void* item_buffer)
  {
    auto_lock lock(&mutex_);
    if(lock_count > 0)
    {
      __RT(printf("....... lock conflict ........\n."));
      return false;
    }
    lock_count++;
    bool res = buffer::get_front(item_buffer);
    lock_count--;
    return res;
  }
  virtual bool pop_front(void* item_buffer)
  {
    auto_lock lock(&mutex_);
    if(lock_count > 0)
    {
      __RT(printf("....... lock conflict ........\n."));
      return false;
    }
    lock_count++;
    bool res = buffer::pop_front(item_buffer);
    lock_count--;
    return res;
  }
  virtual bool get_back(void* item_buffer)
  {
    auto_lock lock(&mutex_);
    if(lock_count > 0)
    {
      __RT(printf("....... lock conflict ........\n."));
      return false;
    }
    lock_count++;
    bool res = buffer::get_back(item_buffer);
    lock_count--;
    return res;
  }
  virtual bool pop_back(void* item_buffer)
  {
    auto_lock lock(&mutex_);
    if(lock_count > 0)
    {
      __RT(printf("....... lock conflict ........\n."));
      return false;
    }
    lock_count++;
    bool res = buffer::pop_back(item_buffer);
    lock_count--;
    return res;
  }
  virtual void clear()
  {
    auto_lock lock(&mutex_);
    if(lock_count > 0)
    {
      __RT(printf("....... lock conflict ........\n."));
      return;
    }
    lock_count++;
    buffer::clear();
    lock_count--;
  }
};

}

#endif // ECAT_BUFFER_LOCKED_HPP
