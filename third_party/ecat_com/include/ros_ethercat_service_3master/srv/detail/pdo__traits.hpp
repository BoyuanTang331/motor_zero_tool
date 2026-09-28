// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from ros_ethercat_service_3master:srv/Pdo.idl
// generated code does not contain a copyright notice

#ifndef ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__TRAITS_HPP_
#define ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "ros_ethercat_service_3master/srv/detail/pdo__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace ros_ethercat_service_3master
{

namespace srv
{

inline void to_flow_style_yaml(
  const Pdo_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: master_id
  {
    out << "master_id: ";
    rosidl_generator_traits::value_to_yaml(msg.master_id, out);
    out << ", ";
  }

  // member: slave_position
  {
    out << "slave_position: ";
    rosidl_generator_traits::value_to_yaml(msg.slave_position, out);
    out << ", ";
  }

  // member: control_word
  {
    out << "control_word: ";
    rosidl_generator_traits::value_to_yaml(msg.control_word, out);
    out << ", ";
  }

  // member: status_word
  {
    out << "status_word: ";
    rosidl_generator_traits::value_to_yaml(msg.status_word, out);
    out << ", ";
  }

  // member: actual_position
  {
    out << "actual_position: ";
    rosidl_generator_traits::value_to_yaml(msg.actual_position, out);
    out << ", ";
  }

  // member: actual_velocity
  {
    out << "actual_velocity: ";
    rosidl_generator_traits::value_to_yaml(msg.actual_velocity, out);
    out << ", ";
  }

  // member: actual_torque
  {
    out << "actual_torque: ";
    rosidl_generator_traits::value_to_yaml(msg.actual_torque, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Pdo_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: master_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "master_id: ";
    rosidl_generator_traits::value_to_yaml(msg.master_id, out);
    out << "\n";
  }

  // member: slave_position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "slave_position: ";
    rosidl_generator_traits::value_to_yaml(msg.slave_position, out);
    out << "\n";
  }

  // member: control_word
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "control_word: ";
    rosidl_generator_traits::value_to_yaml(msg.control_word, out);
    out << "\n";
  }

  // member: status_word
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "status_word: ";
    rosidl_generator_traits::value_to_yaml(msg.status_word, out);
    out << "\n";
  }

  // member: actual_position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "actual_position: ";
    rosidl_generator_traits::value_to_yaml(msg.actual_position, out);
    out << "\n";
  }

  // member: actual_velocity
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "actual_velocity: ";
    rosidl_generator_traits::value_to_yaml(msg.actual_velocity, out);
    out << "\n";
  }

  // member: actual_torque
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "actual_torque: ";
    rosidl_generator_traits::value_to_yaml(msg.actual_torque, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Pdo_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ros_ethercat_service_3master

namespace rosidl_generator_traits
{

[[deprecated("use ros_ethercat_service_3master::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ros_ethercat_service_3master::srv::Pdo_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  ros_ethercat_service_3master::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ros_ethercat_service_3master::srv::to_yaml() instead")]]
inline std::string to_yaml(const ros_ethercat_service_3master::srv::Pdo_Request & msg)
{
  return ros_ethercat_service_3master::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ros_ethercat_service_3master::srv::Pdo_Request>()
{
  return "ros_ethercat_service_3master::srv::Pdo_Request";
}

template<>
inline const char * name<ros_ethercat_service_3master::srv::Pdo_Request>()
{
  return "ros_ethercat_service_3master/srv/Pdo_Request";
}

template<>
struct has_fixed_size<ros_ethercat_service_3master::srv::Pdo_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<ros_ethercat_service_3master::srv::Pdo_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<ros_ethercat_service_3master::srv::Pdo_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace ros_ethercat_service_3master
{

namespace srv
{

inline void to_flow_style_yaml(
  const Pdo_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: control_word
  {
    out << "control_word: ";
    rosidl_generator_traits::value_to_yaml(msg.control_word, out);
    out << ", ";
  }

  // member: target_position
  {
    out << "target_position: ";
    rosidl_generator_traits::value_to_yaml(msg.target_position, out);
    out << ", ";
  }

  // member: target_velocity
  {
    out << "target_velocity: ";
    rosidl_generator_traits::value_to_yaml(msg.target_velocity, out);
    out << ", ";
  }

  // member: target_torque
  {
    out << "target_torque: ";
    rosidl_generator_traits::value_to_yaml(msg.target_torque, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Pdo_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: control_word
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "control_word: ";
    rosidl_generator_traits::value_to_yaml(msg.control_word, out);
    out << "\n";
  }

  // member: target_position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_position: ";
    rosidl_generator_traits::value_to_yaml(msg.target_position, out);
    out << "\n";
  }

  // member: target_velocity
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_velocity: ";
    rosidl_generator_traits::value_to_yaml(msg.target_velocity, out);
    out << "\n";
  }

  // member: target_torque
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_torque: ";
    rosidl_generator_traits::value_to_yaml(msg.target_torque, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Pdo_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace ros_ethercat_service_3master

namespace rosidl_generator_traits
{

[[deprecated("use ros_ethercat_service_3master::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const ros_ethercat_service_3master::srv::Pdo_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  ros_ethercat_service_3master::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use ros_ethercat_service_3master::srv::to_yaml() instead")]]
inline std::string to_yaml(const ros_ethercat_service_3master::srv::Pdo_Response & msg)
{
  return ros_ethercat_service_3master::srv::to_yaml(msg);
}

template<>
inline const char * data_type<ros_ethercat_service_3master::srv::Pdo_Response>()
{
  return "ros_ethercat_service_3master::srv::Pdo_Response";
}

template<>
inline const char * name<ros_ethercat_service_3master::srv::Pdo_Response>()
{
  return "ros_ethercat_service_3master/srv/Pdo_Response";
}

template<>
struct has_fixed_size<ros_ethercat_service_3master::srv::Pdo_Response>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<ros_ethercat_service_3master::srv::Pdo_Response>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<ros_ethercat_service_3master::srv::Pdo_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<ros_ethercat_service_3master::srv::Pdo>()
{
  return "ros_ethercat_service_3master::srv::Pdo";
}

template<>
inline const char * name<ros_ethercat_service_3master::srv::Pdo>()
{
  return "ros_ethercat_service_3master/srv/Pdo";
}

template<>
struct has_fixed_size<ros_ethercat_service_3master::srv::Pdo>
  : std::integral_constant<
    bool,
    has_fixed_size<ros_ethercat_service_3master::srv::Pdo_Request>::value &&
    has_fixed_size<ros_ethercat_service_3master::srv::Pdo_Response>::value
  >
{
};

template<>
struct has_bounded_size<ros_ethercat_service_3master::srv::Pdo>
  : std::integral_constant<
    bool,
    has_bounded_size<ros_ethercat_service_3master::srv::Pdo_Request>::value &&
    has_bounded_size<ros_ethercat_service_3master::srv::Pdo_Response>::value
  >
{
};

template<>
struct is_service<ros_ethercat_service_3master::srv::Pdo>
  : std::true_type
{
};

template<>
struct is_service_request<ros_ethercat_service_3master::srv::Pdo_Request>
  : std::true_type
{
};

template<>
struct is_service_response<ros_ethercat_service_3master::srv::Pdo_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // ROS_ETHERCAT_SERVICE_3MASTER__SRV__DETAIL__PDO__TRAITS_HPP_
