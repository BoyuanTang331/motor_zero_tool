#ifndef ECAT_THREAD_MUTEX_HPP
#define ECAT_THREAD_MUTEX_HPP

#include <pthread.h>
#include <qiuniu/stdio.h>
#include <qiuniu/semaphore.h>
#include <qiuniu/time.h>
#include <qiuniu/sched.h>
#include <qiuniu/pthread.h>
#include <qiuniu/semaphore.h>
#include <qiuniu/sys/time.h>
#include <qiuniu/time.h>
#include <qiuniu/unistd.h>
namespace ecat {

class thread_mutex
{
public:
  thread_mutex();
  ~thread_mutex();

  thread_mutex(const thread_mutex &) = delete;
  thread_mutex(thread_mutex &&) = delete;
  thread_mutex &operator=(thread_mutex const &) = delete;
  thread_mutex &operator=(thread_mutex &&) = delete;

  void lock() ;
  void unlock() ;
  bool try_lock();
  pthread_mutex_t& get(){return native_handle_;}
private:
  pthread_mutex_t native_handle_;
};

class auto_lock
{
private:
  thread_mutex* mutex_ref_;
public:
  auto_lock(thread_mutex* mutex)
  {
    mutex_ref_ = mutex;
    mutex_ref_->lock();
  }
  ~auto_lock()
  {
    mutex_ref_->unlock();
  }
};


class thread_cond
{
public:
  thread_cond();
  ~thread_cond();

  thread_cond(const thread_cond &) = delete;
  thread_cond(thread_cond &&) = delete;
  thread_cond &operator=(thread_cond const &) = delete;
  thread_cond &operator=(thread_cond &&) = delete;

  void wait(thread_mutex& mutex) ;
  void notify_all() ;
private:
  pthread_cond_t native_handle_;
};


}

#endif // ECAT_MUTEX_HPP
