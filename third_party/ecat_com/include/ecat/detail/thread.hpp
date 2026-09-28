#ifndef ECAT_DETAIL_THREAD_HPP
#define ECAT_DETAIL_THREAD_HPP

#include <cassert>
#include <ecat/master.hpp>
#include <functional>
#include <memory>
#include <pthread.h>
#include <system_error>
#include <utility>

namespace ecat::detail
{

/** The class thread_attributes represents attributes of a thread.
 *
 * thread_attributes is implemented as a C++ wrapper around
 * the pthread_attr_t interface. See man page pthread_attr_init(3)
 * for details.
 */
class thread_attributes
{
public:
  enum detach_state_type
  {
    joinable = PTHREAD_CREATE_JOINABLE,
    detached = PTHREAD_CREATE_DETACHED,
  };

  enum inherit_sched_type
  {
    inherit = PTHREAD_INHERIT_SCHED,
    explicit_ = PTHREAD_EXPLICIT_SCHED,
  };

  enum sched_policy_type
  {
    other = SCHED_OTHER,
    fifo = SCHED_FIFO,
    round_robin = SCHED_RR,
  };

  thread_attributes();

  ~thread_attributes();

  thread_attributes(thread_attributes const &) = delete;
  thread_attributes &operator=(thread_attributes const &) = delete;

  thread_attributes &inherit_sched(inherit_sched_type type);

  thread_attributes &sched_policy(sched_policy_type policy);

  thread_attributes &priority(int v);

  thread_attributes &cpu_affinity(std::initializer_list<size_t> cpus)
  {
    cpu_set_t set;
    CPU_ZERO(&set);

    for (auto cpu : cpus)
    {
      CPU_SET(cpu, &set);
    }

    std::error_code ec = {
        pthread_attr_setaffinity_np(&data_, sizeof(set), &set),
        std::system_category()};
    if (ec)
      throw std::system_error{ec, __func__};
    return *this;
  }

  const pthread_attr_t *get() const { return &data_; }

  pthread_attr_t *get() { return &data_; }

private:
  pthread_attr_t data_;
};

/** The class thread represents a realtime thread.
 *
 *  Threads begin execution immediately upon construction of the associated
 *  thread object (pending any OS scheduling delays), starting at the top-level
 *  function provided as a constructor argument. The return value of
 *  the top-level function is ignored and if it terminates by throwing
 *  an exception, std::terminate is called.
 *
 * thread objects may also be in the state that does not represent
 * any thread (after default construction, move from, detach, or join),
 * and a thread of execution may be not associated with
 * any thread objects (after detach).
 *
 * No two thread objects may represent the same thread of execution;
 * thread is not CopyConstructible or CopyAssignable, although it is
 * MoveConstructible and MoveAssignable.
 *
 * \par Examples
 * \code
 *
 *   void thread_entry();
 *
 *   thread_attributes my_attr;
 *   my_attr.inherit_sched(thread_attributes::explicit_);
 *   my_attr.sched_policy(thread_attributu::fifo);
 *   my_attr.priority(80);
 *
 *   thread my_thread(my_attr, thread_entry);
 *   ...
 *   my_thread.join();
 *
 * \endcode
 */
class thread
{
  pthread_t handle_{};
  bool joinable_{true};

  static void *thread_func(void *func) noexcept
  {
    auto p = static_cast<std::function<void()> *>(func);
    auto f = std::move(*p);
    delete p;
    f();
    return 0;
  }

public:
  thread() noexcept : joinable_(false) {}

  template <typename F> thread(thread_attributes const &attr, F &&f)
  {
    auto func = std::make_unique<std::function<void()>>(std::forward<F>(f));

    std::error_code ec = {
        NECRO_RT(pthread_create(&handle_, attr.get(), thread_func, func.get())),
        std::system_category()};

    if (ec)
    {
      throw std::system_error{ec, __func__};
    }

    func.release();
  }

  template <typename F> explicit thread(F &f) : thread(thread_attributes(), f)
  {
  }

  thread(const thread &other) = delete;

  thread(thread &&other) noexcept
      : handle_(other.handle_), joinable_(std::exchange(other.joinable_, false))
  {
  }

  ~thread() noexcept
  {
    if (joinable_)
      join();
  }

  thread &operator=(thread const &other) = delete;

  thread &operator=(thread &&other) noexcept
  {
    swap(other);
    return *this;
  }

  void swap(thread &other) noexcept
  {
    using std::swap;
    swap(handle_, other.handle_);
    swap(joinable_, other.joinable_);
  }

  bool joinable() const { return joinable_; }

  void join()
  {
    assert(joinable_);

    std::error_code ec{NECRO_RT(pthread_join(handle_, NULL)),
                       std::system_category()};
    joinable_ = false;

    if (ec)
    {
      throw std::system_error{ec, __func__};
    }
  }

  void detach()
  {
    assert(joinable_);
    std::error_code ec{(pthread_detach(handle_)), std::system_category()};
    joinable_ = false;

    if (ec)
    {
      throw std::system_error{ec, __func__};
    }
  }
};

inline void swap(thread &t1, thread &t2) noexcept { t1.swap(t2); }

} // namespace ecat::detail

#endif // ECAT_DETAIL_THREAD_HPP
