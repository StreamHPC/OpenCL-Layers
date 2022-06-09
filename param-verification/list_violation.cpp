// version that checks number of subdevices for clCreateSubDevices
template<typename T>
bool list_violation(
  const char * name,
  T param,
  cl_device_id device,
  cl_uint num_devices)
{
  if (strcmp(name, "cl_device_partition_property") == 0)
  { // clCreateSubDevices
    cl_uint cu;
    cl_uint sd;

    // only single partition scheme is allowed
    size_t pos = 0;
    cl_uint curr_cu = 0;
    cl_uint curr_sd = 0;

    switch (param[0]) {
      case CL_DEVICE_PARTITION_EQUALLY:
        clGetDeviceInfo(device,
          CL_DEVICE_MAX_COMPUTE_UNITS,
          sizeof(cl_uint),
          &cu,
          NULL);

        if ((param[1] <= 0) || (param[1] > cu) || (param[2] != 0))
          return true;
        if (cu / param[1] > num_devices)
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_COUNTS:
        clGetDeviceInfo(device,
          CL_DEVICE_MAX_COMPUTE_UNITS,
          sizeof(cl_uint),
          &cu,
          NULL);

        clGetDeviceInfo(device,
          CL_DEVICE_PARTITION_MAX_SUB_DEVICES,
          sizeof(cl_uint),
          &sd,
          NULL);

        ++pos;
        while ((param[pos] != 0) && (param[pos] != CL_DEVICE_PARTITION_BY_COUNTS_LIST_END))
        {
          curr_cu += param[pos];
          curr_sd++;
          if ((param[pos] < 0) || (curr_cu > cu) || (curr_sd > sd))
            return true;
          ++pos;
        }

        ++pos;
        if (param[pos] != 0)
          return true;
        if (curr_sd > num_devices)
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN:
        if (bitfield_violation("cl_device_affinity_domain", param[1]) || (param[1] == 0) || (param[2] != 0))
          return true;
        return false;

      default:
        return true;
    }
  }

  printf("Wrong list: %s, expected cl_device_partition_property\n", name);
  return true;
}

// version that checks device limits for clCreateSubDevices
// and max size of device queue and support for for clCreateCommandQueueWithProperties
template<typename T>
bool list_violation(
  const char * name,
  T param,
  cl_device_id device)
{
  if (strcmp(name, "cl_device_partition_property") == 0)
  { // clCreateSubDevices
    cl_uint cu;
    cl_uint sd;

    // only single partition scheme is allowed
    size_t pos = 0;
    cl_uint curr_cu = 0;
    cl_uint curr_sd = 0;

    switch (param[0]) {
      case CL_DEVICE_PARTITION_EQUALLY:
        clGetDeviceInfo(device,
          CL_DEVICE_MAX_COMPUTE_UNITS,
          sizeof(cl_uint),
          &cu,
          NULL);

        if ((param[1] <= 0) || (param[1] > cu) || (param[2] != 0))
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_COUNTS:
        clGetDeviceInfo(device,
          CL_DEVICE_MAX_COMPUTE_UNITS,
          sizeof(cl_uint),
          &cu,
          NULL);

        clGetDeviceInfo(device,
          CL_DEVICE_PARTITION_MAX_SUB_DEVICES,
          sizeof(cl_uint),
          &sd,
          NULL);

        ++pos;
        while ((param[pos] != 0) && (param[pos] != CL_DEVICE_PARTITION_BY_COUNTS_LIST_END))
        {
          curr_cu += param[pos];
          curr_sd++;
          if ((param[pos] < 0) || (curr_cu > cu) || (curr_sd > sd) || (curr_sd > cu))
            return true;
          ++pos;
        }

        ++pos;
        if (param[pos] != 0)
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN:
        if (bitfield_violation("cl_device_affinity_domain", param[1]) || (param[1] == 0) || (param[2] != 0))
          return true;
        return false;

      default:
        return true;
    }
  }

  if (strcmp(name, "cl_queue_properties") == 0)
  { // clCreateCommandQueueWithProperties - min 2.0
    if (param == NULL)
      return false;

    // any order of properties is allowed
    size_t pos = 0;
    // and not once ???
    cl_uint qs = 0;
    clGetDeviceInfo(device,
      CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE,
      sizeof(cl_uint),
      &qs,
      NULL);
    cl_uint curr_qs = 0;
    cl_device_device_enqueue_capabilities ddec = 0;
    if (from("3.0"))
      clGetDeviceInfo(device,
        CL_DEVICE_DEVICE_ENQUEUE_CAPABILITIES,
        sizeof(cl_device_device_enqueue_capabilities),
        &ddec,
        NULL);
    cl_command_queue_properties qp = 0;

    while (param[pos] != 0)
    {
      switch (param[pos]) {
        case CL_QUEUE_PROPERTIES:
          ++pos;
          qp = param[pos];
          ++pos;
          if (bitfield_violation("cl_command_queue_properties", qp))
            return true;
          if ((qp & CL_QUEUE_ON_DEVICE) && !(qp & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE))
            return true;
          if ((qp & CL_QUEUE_ON_DEVICE_DEFAULT) && !(qp & CL_QUEUE_ON_DEVICE))
            return true;
          if (from("3.0") && (qp & CL_QUEUE_ON_DEVICE) && !(ddec & CL_DEVICE_QUEUE_SUPPORTED))
            return true;
          break;

        case CL_QUEUE_SIZE:
          ++pos;
          curr_qs = param[pos];
          ++pos;
          if (curr_qs > qs)
            return true;
          break;

        default:
          return true;
      }
    }

    if ((curr_qs > 0) && !(qp & CL_QUEUE_ON_DEVICE))
      return true;
    return false;
  }

  printf("Wrong list: %s, expected cl_device_partition_property or cl_queue_properties\n", name);
  return true;
}

// version that checks platform for clCreateContext and clCreateContextFromType
template<typename T>
bool list_violation(
  const char * name,
  T param,
  void * user_data)
{
  // dummy param to separate the case
  (void)user_data;

  if (strcmp(name, "cl_context_properties") == 0)
  { // clCreateContext
    if (param == NULL)
      return false;

    // any order of properties is allowed
    size_t pos = 0;
    // but only once
    cl_uint cp_num = 0;
    cl_uint cius_num = 0;

    while (param[pos] != 0)
    {
      switch (param[pos]) {
        case CL_CONTEXT_PLATFORM:
          if (!object_is_valid((cl_platform_id)param[pos+1]))
            return true;
          pos += 2;
          ++cp_num;
          if (cp_num > 1)
            return true;
          break;

        case CL_CONTEXT_INTEROP_USER_SYNC:
          pos += 2;
          ++cius_num;
          if (cius_num > 1)
            return true;
          break;

        default:
          return true;
      }
    }

    return false;
  }

  printf("Wrong list: %s, expected cl_context_properties\n", name);
  return true;
}

// base version
template<typename T>
bool list_violation(const char * name, T param)
{
  if (strcmp(name, "cl_device_partition_property") == 0)
  { // clCreateSubDevices

    // only single partition scheme is allowed
    size_t pos = 0;

    switch (param[0]) {
      case CL_DEVICE_PARTITION_EQUALLY:
        if ((param[1] == 0) || (param[2] != 0))
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_COUNTS:
        ++pos;
        while ((param[pos] != 0) && (param[pos] != CL_DEVICE_PARTITION_BY_COUNTS_LIST_END))
        {
          ++pos;
        }

        ++pos;
        if (param[pos] != 0)
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_AFFINITY_DOMAIN:
        if (bitfield_violation("cl_device_affinity_domain", param[1]) || (param[1] == 0) || (param[2] != 0))
          return true;
        return false;

      default:
        return true;
    }
  }

  if (strcmp(name, "cl_context_properties") == 0)
  { // clCreateContext
    if (param == NULL)
      return false;

    // any order of properties is allowed
    size_t pos = 0;
    // but only once
    cl_uint cp_num = 0;
    cl_uint cius_num = 0;

    while (param[pos] != 0)
    {
      switch (param[pos]) {
        case CL_CONTEXT_PLATFORM:
          pos += 2;
          ++cp_num;
          if (cp_num > 1)
            return true;
          break;

        case CL_CONTEXT_INTEROP_USER_SYNC:
          pos += 2;
          ++cius_num;
          if (cius_num > 1)
            return true;
          break;

        default:
          return true;
      }
    }

    return false;
  }

  if (strcmp(name, "cl_queue_properties") == 0)
  { // clCreateCommandQueueWithProperties - min 2.0
    if (param == NULL)
      return false;

    // any order of properties is allowed
    size_t pos = 0;
    // and not once ???
    cl_uint curr_qs = 0;
    cl_command_queue_properties qp = 0;

    while (param[pos] != 0)
    {
      switch (param[pos]) {
        case CL_QUEUE_PROPERTIES:
          ++pos;
          qp = param[pos];
          ++pos;
          if (bitfield_violation("cl_command_queue_properties", qp))
            return true;
          if ((qp & CL_QUEUE_ON_DEVICE) && !(qp & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE))
            return true;
          if ((qp & CL_QUEUE_ON_DEVICE_DEFAULT) && !(qp & CL_QUEUE_ON_DEVICE))
            return true;
          break;

        case CL_QUEUE_SIZE:
          ++pos;
          curr_qs = param[pos];
          ++pos;
          break;

        default:
          return true;
      }
    }

    if ((curr_qs > 0) && !(qp & CL_QUEUE_ON_DEVICE))
      return true;
    return false;
  }

  if (strcmp(name, "cl_mem_properties") == 0)
  { // clCreateBufferWithProperties
    if (param == NULL)
      return false;
    // no properties yet
    if (param[0] == 0)
      return false;
    return true;
  }

  if (strcmp(name, "cl_sampler_properties") == 0)
  { // clCreateBufferWithProperties
    if (param == NULL)
      return false;

    // any order of properties is allowed
    size_t pos = 0;
    // but only once
    cl_uint snc_num = 0;
    cl_uint sam_num = 0;
    cl_uint sfm_num = 0;

    while (param[pos] != 0)
    {
      switch (param[pos]) {
        case CL_SAMPLER_NORMALIZED_COORDS:
          pos += 2;
          ++snc_num;
          if (snc_num > 1)
            return true;
          break;

        case CL_SAMPLER_ADDRESSING_MODE:
          ++pos;
          ++sam_num;
          if (sam_num > 1)
            return true;

          if (enum_violation("cl_addressing_mode", param[pos]))
            return true;

          ++pos;
          break;

        case CL_SAMPLER_FILTER_MODE:
          ++pos;
          ++sfm_num;
          if (sfm_num > 1)
            return true;

          if (enum_violation("cl_filter_mode", param[pos]))
            return true;

          ++pos;
          break;

        default:
          return true;
      }
    }

    return false;
  }

  printf("Bad list: %s\n", name);
  return true;
}


//template<typename T1, typename T2>
//bool object_not_in(T1 object, T2 in);

//template<>
bool object_not_in(cl_device_id device, cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  for (cl_uint i = 0; i < nd; ++i)
    if (device == devices[i]) {
      free(devices);
      return false;
    }
  free(devices);
  return true;
}

//template<>
bool object_not_in(cl_command_queue command_queue, cl_device_id device)
{
  cl_device_id q_device;
  clGetCommandQueueInfo(
    command_queue,
    CL_QUEUE_DEVICE,
    sizeof(cl_device_id),
    &q_device,
    NULL);

  if (device == q_device)
    return false;
  return true;
}

// command queue and buffer should belong to the same context
//template<>
bool object_not_in(cl_command_queue command_queue, cl_mem buffer)
{
  cl_context c_context;
  clGetCommandQueueInfo(
    command_queue,
    CL_QUEUE_CONTEXT,
    sizeof(cl_context),
    &c_context,
    NULL);
  cl_context b_context;
  clGetMemObjectInfo(
    buffer,
    CL_MEM_CONTEXT,
    sizeof(cl_context),
    &b_context,
    NULL);

  if (b_context == c_context)
      return false;
  return true;
}

// events and command queue should belong to the same context
//template<>
bool object_not_in(cl_event event, cl_command_queue command_queue)
{
  cl_context e_context;
  clGetEventInfo(
    event,
    CL_EVENT_CONTEXT,
    sizeof(cl_context),
    &e_context,
    NULL);
  cl_context c_context;
  clGetCommandQueueInfo(
    command_queue,
    CL_QUEUE_CONTEXT,
    sizeof(cl_context),
    &c_context,
    NULL);

  if (e_context == c_context)
      return false;
  return true;
}

// mem objects and command queue should belong to the same context
//template<>
bool object_not_in(cl_mem object, cl_command_queue command_queue)
{
  cl_context m_context;
  clGetMemObjectInfo(
    object,
    CL_MEM_CONTEXT,
    sizeof(cl_context),
    &m_context,
    NULL);
  cl_context c_context;
  clGetCommandQueueInfo(
    command_queue,
    CL_QUEUE_CONTEXT,
    sizeof(cl_context),
    &c_context,
    NULL);

  if (m_context == c_context)
      return false;
  return true;
}

template<typename T1, typename T2>
bool any_object_not_in(T1 * objects, size_t n, T2 in)
{
  for (size_t i = 0; i < n; ++i)
    if (object_not_in(objects[i], in))
      return true;
  return false;
}

template<cl_uint property>
bool for_all(cl_context context, std::function<bool(return_type<property>)> check)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  return_type<property> a;
  bool res = true;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      property,
      sizeof(a),
      &a,
      NULL);
    res = res && check(a);
  }

  free(devices);
  return res;
}

template<cl_uint property>
bool for_any(cl_context context, std::function<bool(return_type<property>)> check)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  return_type<property> a;
  bool res = false;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      property,
      sizeof(a),
      &a,
      NULL);
    res = res || check(a);
  }

  free(devices);
  return res;
}
