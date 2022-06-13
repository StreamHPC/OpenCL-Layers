bool object_is_valid(cl_platform_id platform) {
  size_t size;
  cl_int res = tdispatch->clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &size);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_device_id device) {
  cl_device_type type;
  cl_int res = tdispatch->clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &type, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_context context) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_command_queue command_queue) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetCommandQueueInfo(command_queue, CL_QUEUE_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_mem mem) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetMemObjectInfo(mem, CL_MEM_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_mem mem, cl_mem_object_type type) {
  cl_mem_object_type curr_type = 0;
  cl_int res = tdispatch->clGetMemObjectInfo(mem, CL_MEM_TYPE, sizeof(cl_mem_object_type), &curr_type, nullptr);
  return (res == CL_SUCCESS) && (curr_type == type);
}

bool object_is_valid(cl_sampler sampler) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetSamplerInfo(sampler, CL_SAMPLER_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_program program) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetProgramInfo(program, CL_PROGRAM_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_kernel kernel) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetKernelInfo(kernel, CL_KERNEL_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}

bool object_is_valid(cl_event event) {
  cl_uint refcount;
  cl_int res = tdispatch->clGetEventInfo(event, CL_EVENT_REFERENCE_COUNT, sizeof(cl_uint), &refcount, nullptr);
  return res == CL_SUCCESS;
}
