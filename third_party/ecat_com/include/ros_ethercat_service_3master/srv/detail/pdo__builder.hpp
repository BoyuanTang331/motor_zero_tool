// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from ros_ethercat_service_3master:srv/Pdo.idl
// generated code does not contain a copyright notice

#ifndef ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__BUILDER_HPP_
#define ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "ros_ethercat_service_3master/srv/detail/pdo__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace ros_ethercat_service_3master
{

namespace srv
{

namespace builder
{

class Init_Pdo_Request_actual_torque
{
public:
  explicit Init_Pdo_Request_actual_torque(::ros_ethercat_service_3master::srv::Pdo_Request & msg)
  : msg_(msg)
  {}
  ::ros_ethercat_service_3master::srv::Pdo_Request actual_torque(::ros_ethercat_service_3master::srv::Pdo_Request::_actual_torque_type arg)
  {
    msg_.actual_torque = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

class Init_Pdo_Request_actual_velocity
{
public:
  explicit Init_Pdo_Request_actual_velocity(::ros_ethercat_service_3master::srv::Pdo_Request & msg)
  : msg_(msg)
  {}
  Init_Pdo_Request_actual_torque actual_velocity(::ros_ethercat_service_3master::srv::Pdo_Request::_actual_velocity_type arg)
  {
    msg_.actual_velocity = std::move(arg);
    return Init_Pdo_Request_actual_torque(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

class Init_Pdo_Request_actual_position
{
public:
  explicit Init_Pdo_Request_actual_position(::ros_ethercat_service_3master::srv::Pdo_Request & msg)
  : msg_(msg)
  {}
  Init_Pdo_Request_actual_velocity actual_position(::ros_ethercat_service_3master::srv::Pdo_Request::_actual_position_type arg)
  {
    msg_.actual_position = std::move(arg);
    return Init_Pdo_Request_actual_velocity(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

class Init_Pdo_Request_status_word
{
public:
  explicit Init_Pdo_Request_status_word(::ros_ethercat_service_3master::srv::Pdo_Request & msg)
  : msg_(msg)
  {}
  Init_Pdo_Request_actual_position status_word(::ros_ethercat_service_3master::srv::Pdo_Request::_status_word_type arg)
  {
    msg_.status_word = std::move(arg);
    return Init_Pdo_Request_actual_position(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

class Init_Pdo_Request_control_word
{
public:
  explicit Init_Pdo_Request_control_word(::ros_ethercat_service_3master::srv::Pdo_Request & msg)
  : msg_(msg)
  {}
  Init_Pdo_Request_status_word control_word(::ros_ethercat_service_3master::srv::Pdo_Request::_control_word_type arg)
  {
    msg_.control_word = std::move(arg);
    return Init_Pdo_Request_status_word(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

class Init_Pdo_Request_slave_position
{
public:
  explicit Init_Pdo_Request_slave_position(::ros_ethercat_service_3master::srv::Pdo_Request & msg)
  : msg_(msg)
  {}
  Init_Pdo_Request_control_word slave_position(::ros_ethercat_service_3master::srv::Pdo_Request::_slave_position_type arg)
  {
    msg_.slave_position = std::move(arg);
    return Init_Pdo_Request_control_word(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

class Init_Pdo_Request_master_id
{
public:
  Init_Pdo_Request_master_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Pdo_Request_slave_position master_id(::ros_ethercat_service_3master::srv::Pdo_Request::_master_id_type arg)
  {
    msg_.master_id = std::move(arg);
    return Init_Pdo_Request_slave_position(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ros_ethercat_service_3master::srv::Pdo_Request>()
{
  return ros_ethercat_service_3master::srv::builder::Init_Pdo_Request_master_id();
}

}  // namespace ros_ethercat_service_3master


namespace ros_ethercat_service_3master
{

namespace srv
{

namespace builder
{

class Init_Pdo_Response_target_torque
{
public:
  explicit Init_Pdo_Response_target_torque(::ros_ethercat_service_3master::srv::Pdo_Response & msg)
  : msg_(msg)
  {}
  ::ros_ethercat_service_3master::srv::Pdo_Response target_torque(::ros_ethercat_service_3master::srv::Pdo_Response::_target_torque_type arg)
  {
    msg_.target_torque = std::move(arg);
    return std::move(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Response msg_;
};

class Init_Pdo_Response_target_velocity
{
public:
  explicit Init_Pdo_Response_target_velocity(::ros_ethercat_service_3master::srv::Pdo_Response & msg)
  : msg_(msg)
  {}
  Init_Pdo_Response_target_torque target_velocity(::ros_ethercat_service_3master::srv::Pdo_Response::_target_velocity_type arg)
  {
    msg_.target_velocity = std::move(arg);
    return Init_Pdo_Response_target_torque(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Response msg_;
};

class Init_Pdo_Response_target_position
{
public:
  explicit Init_Pdo_Response_target_position(::ros_ethercat_service_3master::srv::Pdo_Response & msg)
  : msg_(msg)
  {}
  Init_Pdo_Response_target_velocity target_position(::ros_ethercat_service_3master::srv::Pdo_Response::_target_position_type arg)
  {
    msg_.target_position = std::move(arg);
    return Init_Pdo_Response_target_velocity(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Response msg_;
};

class Init_Pdo_Response_control_word
{
public:
  Init_Pdo_Response_control_word()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Pdo_Response_target_position control_word(::ros_ethercat_service_3master::srv::Pdo_Response::_control_word_type arg)
  {
    msg_.control_word = std::move(arg);
    return Init_Pdo_Response_target_position(msg_);
  }

private:
  ::ros_ethercat_service_3master::srv::Pdo_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::ros_ethercat_service_3master::srv::Pdo_Response>()
{
  return ros_ethercat_service_3master::srv::builder::Init_Pdo_Response_control_word();
}

}  // namespace ros_ethercat_service_3master

#endif  // ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__BUILDER_HPP_
