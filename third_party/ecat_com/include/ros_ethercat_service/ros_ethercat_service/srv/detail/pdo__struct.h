// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from ros_ethercat_service:srv/Pdo.idl
// generated code does not contain a copyright notice

#ifndef ROS_ETHERCAT_SERVICE__SRV__DETAIL__PDO__STRUCT_H_
#define ROS_ETHERCAT_SERVICE__SRV__DETAIL__PDO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/Pdo in the package ros_ethercat_service.
typedef struct ros_ethercat_service__srv__Pdo_Request
{
  int16_t master_id;
  uint16_t slave_position;
  uint16_t control_word;
  uint16_t status_word;
  int32_t actual_position;
  int32_t actual_velocity;
  int16_t actual_torque;
} ros_ethercat_service__srv__Pdo_Request;

// Struct for a sequence of ros_ethercat_service__srv__Pdo_Request.
typedef struct ros_ethercat_service__srv__Pdo_Request__Sequence
{
  ros_ethercat_service__srv__Pdo_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ros_ethercat_service__srv__Pdo_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/Pdo in the package ros_ethercat_service.
typedef struct ros_ethercat_service__srv__Pdo_Response
{
  uint16_t control_word;
  int32_t target_position;
  int32_t target_velocity;
  int16_t target_torque;
} ros_ethercat_service__srv__Pdo_Response;

// Struct for a sequence of ros_ethercat_service__srv__Pdo_Response.
typedef struct ros_ethercat_service__srv__Pdo_Response__Sequence
{
  ros_ethercat_service__srv__Pdo_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} ros_ethercat_service__srv__Pdo_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROS_ETHERCAT_SERVICE__SRV__DETAIL__PDO__STRUCT_H_
