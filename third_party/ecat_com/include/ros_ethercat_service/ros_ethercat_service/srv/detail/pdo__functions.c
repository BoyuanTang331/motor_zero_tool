// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from ros_ethercat_service:srv/Pdo.idl
// generated code does not contain a copyright notice
#include "ros_ethercat_service/srv/detail/pdo__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
ros_ethercat_service__srv__Pdo_Request__init(ros_ethercat_service__srv__Pdo_Request * msg)
{
  if (!msg) {
    return false;
  }
  // master_id
  // slave_position
  // control_word
  // status_word
  // actual_position
  // actual_velocity
  // actual_torque
  return true;
}

void
ros_ethercat_service__srv__Pdo_Request__fini(ros_ethercat_service__srv__Pdo_Request * msg)
{
  if (!msg) {
    return;
  }
  // master_id
  // slave_position
  // control_word
  // status_word
  // actual_position
  // actual_velocity
  // actual_torque
}

bool
ros_ethercat_service__srv__Pdo_Request__are_equal(const ros_ethercat_service__srv__Pdo_Request * lhs, const ros_ethercat_service__srv__Pdo_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // master_id
  if (lhs->master_id != rhs->master_id) {
    return false;
  }
  // slave_position
  if (lhs->slave_position != rhs->slave_position) {
    return false;
  }
  // control_word
  if (lhs->control_word != rhs->control_word) {
    return false;
  }
  // status_word
  if (lhs->status_word != rhs->status_word) {
    return false;
  }
  // actual_position
  if (lhs->actual_position != rhs->actual_position) {
    return false;
  }
  // actual_velocity
  if (lhs->actual_velocity != rhs->actual_velocity) {
    return false;
  }
  // actual_torque
  if (lhs->actual_torque != rhs->actual_torque) {
    return false;
  }
  return true;
}

bool
ros_ethercat_service__srv__Pdo_Request__copy(
  const ros_ethercat_service__srv__Pdo_Request * input,
  ros_ethercat_service__srv__Pdo_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // master_id
  output->master_id = input->master_id;
  // slave_position
  output->slave_position = input->slave_position;
  // control_word
  output->control_word = input->control_word;
  // status_word
  output->status_word = input->status_word;
  // actual_position
  output->actual_position = input->actual_position;
  // actual_velocity
  output->actual_velocity = input->actual_velocity;
  // actual_torque
  output->actual_torque = input->actual_torque;
  return true;
}

ros_ethercat_service__srv__Pdo_Request *
ros_ethercat_service__srv__Pdo_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_ethercat_service__srv__Pdo_Request * msg = (ros_ethercat_service__srv__Pdo_Request *)allocator.allocate(sizeof(ros_ethercat_service__srv__Pdo_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(ros_ethercat_service__srv__Pdo_Request));
  bool success = ros_ethercat_service__srv__Pdo_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
ros_ethercat_service__srv__Pdo_Request__destroy(ros_ethercat_service__srv__Pdo_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    ros_ethercat_service__srv__Pdo_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
ros_ethercat_service__srv__Pdo_Request__Sequence__init(ros_ethercat_service__srv__Pdo_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_ethercat_service__srv__Pdo_Request * data = NULL;

  if (size) {
    data = (ros_ethercat_service__srv__Pdo_Request *)allocator.zero_allocate(size, sizeof(ros_ethercat_service__srv__Pdo_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = ros_ethercat_service__srv__Pdo_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        ros_ethercat_service__srv__Pdo_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
ros_ethercat_service__srv__Pdo_Request__Sequence__fini(ros_ethercat_service__srv__Pdo_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      ros_ethercat_service__srv__Pdo_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

ros_ethercat_service__srv__Pdo_Request__Sequence *
ros_ethercat_service__srv__Pdo_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_ethercat_service__srv__Pdo_Request__Sequence * array = (ros_ethercat_service__srv__Pdo_Request__Sequence *)allocator.allocate(sizeof(ros_ethercat_service__srv__Pdo_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = ros_ethercat_service__srv__Pdo_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
ros_ethercat_service__srv__Pdo_Request__Sequence__destroy(ros_ethercat_service__srv__Pdo_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    ros_ethercat_service__srv__Pdo_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
ros_ethercat_service__srv__Pdo_Request__Sequence__are_equal(const ros_ethercat_service__srv__Pdo_Request__Sequence * lhs, const ros_ethercat_service__srv__Pdo_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!ros_ethercat_service__srv__Pdo_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
ros_ethercat_service__srv__Pdo_Request__Sequence__copy(
  const ros_ethercat_service__srv__Pdo_Request__Sequence * input,
  ros_ethercat_service__srv__Pdo_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(ros_ethercat_service__srv__Pdo_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    ros_ethercat_service__srv__Pdo_Request * data =
      (ros_ethercat_service__srv__Pdo_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!ros_ethercat_service__srv__Pdo_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          ros_ethercat_service__srv__Pdo_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!ros_ethercat_service__srv__Pdo_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
ros_ethercat_service__srv__Pdo_Response__init(ros_ethercat_service__srv__Pdo_Response * msg)
{
  if (!msg) {
    return false;
  }
  // control_word
  // target_position
  // target_velocity
  // target_torque
  return true;
}

void
ros_ethercat_service__srv__Pdo_Response__fini(ros_ethercat_service__srv__Pdo_Response * msg)
{
  if (!msg) {
    return;
  }
  // control_word
  // target_position
  // target_velocity
  // target_torque
}

bool
ros_ethercat_service__srv__Pdo_Response__are_equal(const ros_ethercat_service__srv__Pdo_Response * lhs, const ros_ethercat_service__srv__Pdo_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // control_word
  if (lhs->control_word != rhs->control_word) {
    return false;
  }
  // target_position
  if (lhs->target_position != rhs->target_position) {
    return false;
  }
  // target_velocity
  if (lhs->target_velocity != rhs->target_velocity) {
    return false;
  }
  // target_torque
  if (lhs->target_torque != rhs->target_torque) {
    return false;
  }
  return true;
}

bool
ros_ethercat_service__srv__Pdo_Response__copy(
  const ros_ethercat_service__srv__Pdo_Response * input,
  ros_ethercat_service__srv__Pdo_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // control_word
  output->control_word = input->control_word;
  // target_position
  output->target_position = input->target_position;
  // target_velocity
  output->target_velocity = input->target_velocity;
  // target_torque
  output->target_torque = input->target_torque;
  return true;
}

ros_ethercat_service__srv__Pdo_Response *
ros_ethercat_service__srv__Pdo_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_ethercat_service__srv__Pdo_Response * msg = (ros_ethercat_service__srv__Pdo_Response *)allocator.allocate(sizeof(ros_ethercat_service__srv__Pdo_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(ros_ethercat_service__srv__Pdo_Response));
  bool success = ros_ethercat_service__srv__Pdo_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
ros_ethercat_service__srv__Pdo_Response__destroy(ros_ethercat_service__srv__Pdo_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    ros_ethercat_service__srv__Pdo_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
ros_ethercat_service__srv__Pdo_Response__Sequence__init(ros_ethercat_service__srv__Pdo_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_ethercat_service__srv__Pdo_Response * data = NULL;

  if (size) {
    data = (ros_ethercat_service__srv__Pdo_Response *)allocator.zero_allocate(size, sizeof(ros_ethercat_service__srv__Pdo_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = ros_ethercat_service__srv__Pdo_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        ros_ethercat_service__srv__Pdo_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
ros_ethercat_service__srv__Pdo_Response__Sequence__fini(ros_ethercat_service__srv__Pdo_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      ros_ethercat_service__srv__Pdo_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

ros_ethercat_service__srv__Pdo_Response__Sequence *
ros_ethercat_service__srv__Pdo_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ros_ethercat_service__srv__Pdo_Response__Sequence * array = (ros_ethercat_service__srv__Pdo_Response__Sequence *)allocator.allocate(sizeof(ros_ethercat_service__srv__Pdo_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = ros_ethercat_service__srv__Pdo_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
ros_ethercat_service__srv__Pdo_Response__Sequence__destroy(ros_ethercat_service__srv__Pdo_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    ros_ethercat_service__srv__Pdo_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
ros_ethercat_service__srv__Pdo_Response__Sequence__are_equal(const ros_ethercat_service__srv__Pdo_Response__Sequence * lhs, const ros_ethercat_service__srv__Pdo_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!ros_ethercat_service__srv__Pdo_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
ros_ethercat_service__srv__Pdo_Response__Sequence__copy(
  const ros_ethercat_service__srv__Pdo_Response__Sequence * input,
  ros_ethercat_service__srv__Pdo_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(ros_ethercat_service__srv__Pdo_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    ros_ethercat_service__srv__Pdo_Response * data =
      (ros_ethercat_service__srv__Pdo_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!ros_ethercat_service__srv__Pdo_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          ros_ethercat_service__srv__Pdo_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!ros_ethercat_service__srv__Pdo_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
