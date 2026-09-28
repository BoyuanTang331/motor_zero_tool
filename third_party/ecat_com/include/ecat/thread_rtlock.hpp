#ifndef ECAT_THREAD_RTLOCK_HPP
#define ECAT_THREAD_RTLOCK_HPP

#include <pthread.h>
//#include <pppd/spinlock.h>
#include <boost/smart_ptr/detail/spinlock.hpp>

namespace ecat {

class thread_rtlock
{
public:
  thread_rtlock();
  ~thread_rtlock();

  thread_rtlock(const thread_rtlock &) = delete;
  thread_rtlock(thread_rtlock &&) = delete;
  thread_rtlock &operator=(thread_rtlock const &) = delete;
  thread_rtlock &operator=(thread_rtlock &&) = delete;

  void lock() ;
  void unlock() ;
private:
  //spinlock_t native_handle_;
  boost::detail::spinlock native_handle_;
};

class auto_rtlock
{
private:
  thread_rtlock* rtlock_ref_;
public:
  auto_rtlock(thread_rtlock* rtlock)
  {
    rtlock_ref_ = rtlock;
    rtlock_ref_->lock();
  }
  ~auto_rtlock()
  {
    rtlock_ref_->unlock();
  }
};

}

#endif // ECAT_THREAD_RTLOCK_HPP
