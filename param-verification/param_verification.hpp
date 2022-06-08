#pragma once

#include <CL/cl_layer.h>
#include "utils.hpp"

bool from(const char * version);
cl_device_id * get_devices(cl_context context, cl_uint * number);
size_t pixel_size(const cl_image_format * image_format);
void init_dispatch();

extern struct _cl_icd_dispatch dispatch;

extern const struct _cl_icd_dispatch *tdispatch;

extern ocl_layer_utils::stream_ptr log_stream;
