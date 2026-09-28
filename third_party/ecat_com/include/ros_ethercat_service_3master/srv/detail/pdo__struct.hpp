// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from ros_ethercat_service_3master:srv/Pdo.idl
// generated code does not contain a copyright notice

#ifndef ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__STRUCT_HPP_
#define ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Request __attribute__((deprecated))
#else
# define DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Request __declspec(deprecated)
#endif

namespace ros_ethercat_service_3master
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct Pdo_Request_
{
  using Type = Pdo_Request_<ContainerAllocator>;

  explicit Pdo_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->master_id = 0;
      this->slave_position = 0;
      this->control_word = 0;
      this->status_word = 0;
      this->actual_position = 0l;
      this->actual_velocity = 0l;
      this->actual_torque = 0;
    }
  }

  explicit Pdo_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->master_id = 0;
      this->slave_position = 0;
      this->control_word = 0;
      this->status_word = 0;
      this->actual_position = 0l;
      this->actual_velocity = 0l;
      this->actual_torque = 0;
    }
  }

  // field types and members
  using _master_id_type =
    int16_t;
  _master_id_type master_id;
  using _slave_position_type =
    uint16_t;
  _slave_position_type slave_position;
  using _control_word_type =
    uint16_t;
  _control_word_type control_word;
  using _status_word_type =
    uint16_t;
  _status_word_type status_word;
  using _actual_position_type =
    int32_t;
  _actual_position_type actual_position;
  using _actual_velocity_type =
    int32_t;
  _actual_velocity_type actual_velocity;
  using _actual_torque_type =
    int16_t;
  _actual_torque_type actual_torque;

  // setters for named parameter idiom
  Type & set__master_id(
    const int16_t & _arg)
  {
    this->master_id = _arg;
    return *this;
  }
  Type & set__slave_position(
    const uint16_t & _arg)
  {
    this->slave_position = _arg;
    return *this;
  }
  Type & set__control_word(
    const uint16_t & _arg)
  {
    this->control_word = _arg;
    return *this;
  }
  Type & set__status_word(
    const uint16_t & _arg)
  {
    this->status_word = _arg;
    return *this;
  }
  Type & set__actual_position(
    const int32_t & _arg)
  {
    this->actual_position = _arg;
    return *this;
  }
  Type & set__actual_velocity(
    const int32_t & _arg)
  {
    this->actual_velocity = _arg;
    return *this;
  }
  Type & set__actual_torque(
    const int16_t & _arg)
  {
    this->actual_torque = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Request
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Request
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Pdo_Request_ & other) const
  {
    if (this->master_id != other.master_id) {
      return false;
    }
    if (this->slave_position != other.slave_position) {
      return false;
    }
    if (this->control_word != other.control_word) {
      return false;
    }
    if (this->status_word != other.status_word) {
      return false;
    }
    if (this->actual_position != other.actual_position) {
      return false;
    }
    if (this->actual_velocity != other.actual_velocity) {
      return false;
    }
    if (this->actual_torque != other.actual_torque) {
      return false;
    }
    return true;
  }
  bool operator!=(const Pdo_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Pdo_Request_

// alias to use template instance with default allocator
using Pdo_Request =
  ros_ethercat_service_3master::srv::Pdo_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ros_ethercat_service_3master


#ifndef _WIN32
# define DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Response __attribute__((deprecated))
#else
# define DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Response __declspec(deprecated)
#endif

namespace ros_ethercat_service_3master
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct Pdo_Response_
{
  using Type = Pdo_Response_<ContainerAllocator>;

  explicit Pdo_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->control_word = 0;
      this->target_position = 0l;
      this->target_velocity = 0l;
      this->target_torque = 0;
    }
  }

  explicit Pdo_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->control_word = 0;
      this->target_position = 0l;
      this->target_velocity = 0l;
      this->target_torque = 0;
    }
  }

  // field types and members
  using _control_word_type =
    uint16_t;
  _control_word_type control_word;
  using _target_position_type =
    int32_t;
  _target_position_type target_position;
  using _target_velocity_type =
    int32_t;
  _target_velocity_type target_velocity;
  using _target_torque_type =
    int16_t;
  _target_torque_type target_torque;

  // setters for named parameter idiom
  Type & set__control_word(
    const uint16_t & _arg)
  {
    this->control_word = _arg;
    return *this;
  }
  Type & set__target_position(
    const int32_t & _arg)
  {
    this->target_position = _arg;
    return *this;
  }
  Type & set__target_velocity(
    const int32_t & _arg)
  {
    this->target_velocity = _arg;
    return *this;
  }
  Type & set__target_torque(
    const int16_t & _arg)
  {
    this->target_torque = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Response
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__ros_ethercat_service_3master__srv__Pdo_Response
    std::shared_ptr<ros_ethercat_service_3master::srv::Pdo_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Pdo_Response_ & other) const
  {
    if (this->control_word != other.control_word) {
      return false;
    }
    if (this->target_position != other.target_position) {
      return false;
    }
    if (this->target_velocity != other.target_velocity) {
      return false;
    }
    if (this->target_torque != other.target_torque) {
      return false;
    }
    return true;
  }
  bool operator!=(const Pdo_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Pdo_Response_

// alias to use template instance with default allocator
using Pdo_Response =
  ros_ethercat_service_3master::srv::Pdo_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace ros_ethercat_service_3master

namespace ros_ethercat_service_3master
{

namespace srv
{

struct Pdo
{
  using Request = ros_ethercat_service_3master::srv::Pdo_Request;
  using Response = ros_ethercat_service_3master::srv::Pdo_Response;
};

}  // namespace srv

}  // namespace ros_ethercat_service_3master

#endif  // ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__STRUCT_HPP_
