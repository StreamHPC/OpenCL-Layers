bool from(const char * version);
cl_device_id * get_devices(cl_context context, cl_uint * number);
size_t pixel_size(const cl_image_format * image_format);

#include <CL/cl_layer.h>

static struct _cl_icd_dispatch dispatch = {};

static const struct _cl_icd_dispatch *tdispatch;

  /* Layer API entry points */
CL_API_ENTRY cl_int CL_API_CALL
clGetLayerInfo(
    cl_layer_info  param_name,
    size_t         param_value_size,
    void          *param_value,
    size_t        *param_value_size_ret) {
  switch (param_name) {
  case CL_LAYER_API_VERSION:
    if (param_value) {
      if (param_value_size < sizeof(cl_layer_api_version))
        return CL_INVALID_VALUE;
      *((cl_layer_api_version *)param_value) = CL_LAYER_API_VERSION_100;
    }
    if (param_value_size_ret)
      *param_value_size_ret = sizeof(cl_layer_api_version);
    break;
  default:
    return CL_INVALID_VALUE;
  }
  return CL_SUCCESS;
}

static void init_dispatch(void);

CL_API_ENTRY cl_int CL_API_CALL
clInitLayer(
    cl_uint                         num_entries,
    const struct _cl_icd_dispatch  *target_dispatch,
    cl_uint                        *num_entries_out,
    const struct _cl_icd_dispatch **layer_dispatch_ret) {
  if (!target_dispatch || !layer_dispatch_ret || !num_entries_out || num_entries < sizeof(dispatch) / sizeof(dispatch.clGetPlatformIDs))
    return CL_INVALID_VALUE;

  tdispatch = target_dispatch;
  init_dispatch();

  *layer_dispatch_ret = &dispatch;
  *num_entries_out = sizeof(dispatch)/sizeof(dispatch.clGetPlatformIDs);
  return CL_SUCCESS;
}
