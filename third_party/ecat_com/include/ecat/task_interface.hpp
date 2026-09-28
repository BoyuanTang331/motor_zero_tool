#ifndef ECAT_TASK_INTERFACE_HPP
#define ECAT_TASK_INTERFACE_HPP


#include <cstdint>
#include <string.h>
#include <sys/time.h>
#include "buffer_locked.hpp"
#include "ecat/detail/thread.hpp"

namespace ecat {

class task_interface
{
protected:
  std::int64_t cycle_time_;  //us
  struct timeval last_cycle_time_;
  int prio_;

  cpu_set_t cpus_;
  int cup_size_;
  detail::thread thread_;
  bool is_running_;
public:
  task_interface()
  {
    cycle_time_ = 1000;
    prio_ = 50;
    CPU_ZERO(&cpus_);
    cup_size_ = 0;
    memset(&last_cycle_time_, 0, sizeof(last_cycle_time_));
    is_running_ = true;
  }
  virtual ~task_interface()
  {
  }

  virtual void on_cycle() = 0;
  virtual void run();
  void start();
  void stop(){is_running_ = false;}
  void wait_peroid();

  // t: nanosecond
  void cycle_time(std::int64_t t)
  {
    cycle_time_ = t;
  }
  /** Set the priority level of real time thread. */
  void priority(int prio)
  {
    prio_ = prio;
  }

  /** Set the cpu affinity of real time thread, call many times to set muti cpus. */
  void cpu_affinity(int cpu_index)
  {
    CPU_SET(cpu_index, &cpus_);
    cup_size_++;
  }
};

}

#endif // ECAT_TASK_INTERFACE_HPP
