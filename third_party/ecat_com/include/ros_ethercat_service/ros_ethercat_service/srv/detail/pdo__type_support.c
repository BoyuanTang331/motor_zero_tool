// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from ros_ethercat_service:srv/Pdo.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "ros_ethercat_service/srv/detail/pdo__rosidl_typesupport_introspection_c.h"
#include "ros_ethercat_service/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "ros_ethercat_service/srv/detail/pdo__functions.h"
#include "ros_ethercat_service/srv/detail/pdo__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  ros_ethercat_service__srv__Pdo_Request__init(message_memory);
}

void ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_fini_function(void * message_memory)
{
  ros_ethercat_service__srv__Pdo_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_member_array[7] = {
  {
    "master_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, master_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "slave_position",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, slave_position),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "control_word",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, control_word),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "status_word",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, status_word),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "actual_position",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, actual_position),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "actual_velocity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, actual_velocity),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "actual_torque",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Request, actual_torque),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_members = {
  "ros_ethercat_service__srv",  // message namespace
  "Pdo_Request",  // message name
  7,  // number of fields
  sizeof(ros_ethercat_service__srv__Pdo_Request),
  ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_member_array,  // message members
  ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_type_support_handle = {
  0,
  &ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ros_ethercat_service
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo_Request)() {
  if (!ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_type_support_handle.typesupport_identifier) {
    ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ros_ethercat_service__srv__Pdo_Request__rosidl_typesupport_introspection_c__Pdo_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "ros_ethercat_service/srv/detail/pdo__rosidl_typesupport_introspection_c.h"
// already included above
// #include "ros_ethercat_service/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "ros_ethercat_service/srv/detail/pdo__functions.h"
// already included above
// #include "ros_ethercat_service/srv/detail/pdo__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  ros_ethercat_service__srv__Pdo_Response__init(message_memory);
}

void ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_fini_function(void * message_memory)
{
  ros_ethercat_service__srv__Pdo_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_member_array[4] = {
  {
    "control_word",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Response, control_word),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "target_position",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Response, target_position),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "target_velocity",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Response, target_velocity),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "target_torque",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(ros_ethercat_service__srv__Pdo_Response, target_torque),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_members = {
  "ros_ethercat_service__srv",  // message namespace
  "Pdo_Response",  // message name
  4,  // number of fields
  sizeof(ros_ethercat_service__srv__Pdo_Response),
  ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_member_array,  // message members
  ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_type_support_handle = {
  0,
  &ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ros_ethercat_service
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo_Response)() {
  if (!ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_type_support_handle.typesupport_identifier) {
    ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &ros_ethercat_service__srv__Pdo_Response__rosidl_typesupport_introspection_c__Pdo_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "ros_ethercat_service/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "ros_ethercat_service/srv/detail/pdo__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_members = {
  "ros_ethercat_service__srv",  // service namespace
  "Pdo",  // service name
  // these two fields are initialized below on the first access
  NULL,  // request message
  // ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_Request_message_type_support_handle,
  NULL  // response message
  // ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_Response_message_type_support_handle
};

static rosidl_service_type_support_t ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_type_support_handle = {
  0,
  &ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_members,
  get_service_typesupport_handle_function,
};

// Forward declaration of request/response type support functions
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo_Request)();

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo_Response)();

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_ros_ethercat_service
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo)() {
  if (!ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_type_support_handle.typesupport_identifier) {
    ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, ros_ethercat_service, srv, Pdo_Response)()->data;
  }

  return &ros_ethercat_service__srv__detail__pdo__rosidl_typesupport_introspection_c__Pdo_service_type_support_handle;
}
