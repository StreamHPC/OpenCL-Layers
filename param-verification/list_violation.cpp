// longer version that checks also device limits
template<typename T>
bool list_violation(const char * name, T param, cl_device_id device)
{
  if (strcmp(name, "cl_device_partition_property") == 0)
  { // clCreateSubDevices
    cl_uint cu;
    cl_uint sd;

    // only single partition scheme is allowed
    size_t pos = 0;

    switch (param[0]) {
      case CL_DEVICE_PARTITION_EQUALLY:
        clGetDeviceInfo(device,
          CL_DEVICE_MAX_COMPUTE_UNITS,
          sizeof(cl_uint),
          &cu,
          NULL);

        if ((param[1] == 0) || (param[1] > cu) || (param[2] != 0))
          return true;
        return false;

      case CL_DEVICE_PARTITION_BY_COUNTS:
        clGetDeviceInfo(device,
          CL_DEVICE_MAX_COMPUTE_UNITS,
          sizeof(cl_uint),
          &cu,
          NULL);
        cl_uint curr_cu = 0;

        clGetDeviceInfo(device,
          CL_DEVICE_PARTITION_MAX_SUB_DEVICES,
          sizeof(cl_uint),
          &sd,
          NULL);
        cl_uint curr_sd = 0;

        ++pos;
        while ((param[pos] != 0) && (param[pos] != CL_DEVICE_PARTITION_BY_COUNTS_LIST_END))
        {
          curr_cu += param[pos];
          curr_sd++;
          if ((curr_cu > cu) || (curr_sd > sd))
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

  printf("Unknown list: %s\n", name);
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

        case CL_QUEUE_SIZE:
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
  { // clCreateCommandQueueWithProperties
    if (param == NULL)
      return false;

    // any order of properties is allowed
    size_t pos = 0;
    // and not once ???
    cl_uint qs = 0;
    if (from("2.0"))
      clGetDeviceInfo(device,
        CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE,
        sizeof(cl_uint),
        &qs,
        NULL);
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
          // from("2.0") is not needed as if any of the flags are set 
          // for earlier implementation it is a bitfield violation above
          if ((qp | CL_QUEUE_ON_DEVICE) && !(qp | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE))
            return true;
          if ((qp | CL_QUEUE_ON_DEVICE_DEFAULT) && !(qp | CL_QUEUE_ON_DEVICE))
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

    if ((curr_qs > 0) && !(qp | CL_QUEUE_ON_DEVICE))
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

  printf("Unknown list: %s\n", name);
  return true;
}
