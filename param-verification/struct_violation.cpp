// auxilary functions

bool is_3D_image_fits(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t width, height, depth;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE3D_MAX_WIDTH,
      sizeof(size_t),
      &width,
      NULL);
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE3D_MAX_HEIGHT,
      sizeof(size_t),
      &height,
      NULL);
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE3D_MAX_DEPTH,
      sizeof(size_t),
      &depth,
      NULL);
    if ((image_desc->image_width <= width ) &&
      (image_desc->image_height <= height) &&
      (image_desc->image_depth <= depth ))
    {
      free(devices);
      return true;
    }
  }

  free(devices);
  return false;
}

bool is_2D_image_fits(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t width, height;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE2D_MAX_WIDTH,
      sizeof(size_t),
      &width,
      NULL);
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE2D_MAX_HEIGHT,
      sizeof(size_t),
      &height,
      NULL);
    if ((image_desc->image_width <= width ) &&
      (image_desc->image_height <= height))
    {
      free(devices);
      return true;
    }
  }

  free(devices);
  return false;
}

bool is_1D_image_fits(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t width;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE2D_MAX_WIDTH,
      sizeof(size_t),
      &width,
      NULL);
    if (image_desc->image_width <= width)
    {
      free(devices);
      return true;
    }
  }

  free(devices);
  return false;
}

bool is_2D_array_fits(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t width, height, size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE2D_MAX_WIDTH,
      sizeof(size_t),
      &width,
      NULL);
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE2D_MAX_HEIGHT,
      sizeof(size_t),
      &height,
      NULL);
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE_MAX_ARRAY_SIZE,
      sizeof(size_t),
      &size,
      NULL);
    if ((image_desc->image_width   <= width ) &&
      (image_desc->image_height   <= height) &&
      (image_desc->image_array_size <= size ))
    {
      free(devices);
      return true;
    }
  }

  free(devices);
  return false;
}

bool is_1D_array_fits(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t width, size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE2D_MAX_WIDTH,
      sizeof(size_t),
      &width,
      NULL);
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE_MAX_ARRAY_SIZE,
      sizeof(size_t),
      &size,
      NULL);
    if ((image_desc->image_width   <= width ) &&
      (image_desc->image_array_size <= size ))
    {
      free(devices);
      return true;
    }
  }

  free(devices);
  return false;
}

bool is_1D_buffer_fits(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t width;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      CL_DEVICE_IMAGE_MAX_BUFFER_SIZE,
      sizeof(size_t),
      &width,
      NULL);
    if (image_desc->image_width <= width)
    {
      free(devices);
      return true;
    }
  }

  free(devices);
  return false;
}

cl_device_id * get_devices(cl_context context, cl_uint * number)
{
  size_t size;
  cl_device_id * devices = NULL;

  if (from("1.1"))
  {
    clGetContextInfo(context,
      CL_CONTEXT_NUM_DEVICES,
      sizeof(cl_uint),
      number,
      &size);

    size = *number * sizeof(cl_device_id);
    devices = (cl_device_id *)malloc(size);

    clGetContextInfo(context,
      CL_CONTEXT_DEVICES,
      size,
      devices,
      &size);
  }
  else
  {
    *number = 65535;
    size = *number * sizeof(cl_device_id);
    devices = (cl_device_id *)malloc(size);

    clGetContextInfo(context,
      CL_CONTEXT_DEVICES,
      size,
      devices,
      &size);

    *number = size / sizeof(cl_device_id);
    devices = (cl_device_id *)realloc(devices, size);
  }

  return devices;
}

size_t max_image_width(cl_context context, cl_mem_object_type image_type)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t res = 0;
  cl_device_info name;
  switch (image_type) {
    case CL_MEM_OBJECT_IMAGE1D_BUFFER:
      name = CL_DEVICE_IMAGE_MAX_BUFFER_SIZE;
      break;

    case CL_MEM_OBJECT_IMAGE1D:
    case CL_MEM_OBJECT_IMAGE1D_ARRAY:
    case CL_MEM_OBJECT_IMAGE2D:
    case CL_MEM_OBJECT_IMAGE2D_ARRAY:
      name = CL_DEVICE_IMAGE2D_MAX_WIDTH;
      break;

    case CL_MEM_OBJECT_IMAGE3D:
      name = CL_DEVICE_IMAGE3D_MAX_WIDTH;
      break;

    default:
      printf("Bad image type\n");
      return 0;
  }

  // find maximum image width for all devices in the context
  size_t size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      name,
      sizeof(size_t),
      &size,
      NULL);
    if (size > res)
      res = size;
  }

  free(devices);
  return res;
}

size_t max_image_height(cl_context context, cl_mem_object_type image_type)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t res = 0;
  cl_device_info name;
  switch (image_type) {
    case CL_MEM_OBJECT_IMAGE2D:
    case CL_MEM_OBJECT_IMAGE2D_ARRAY:
      name = CL_DEVICE_IMAGE2D_MAX_HEIGHT;
      break;

    case CL_MEM_OBJECT_IMAGE3D:
      name = CL_DEVICE_IMAGE3D_MAX_HEIGHT;
      break;

    default:
      printf("Bad image type\n");
      return 0;
  }

  // find maximum image height for all devices in the context
  size_t size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      name,
      sizeof(size_t),
      &size,
      NULL);
    if (size > res)
      res = size;
  }

  free(devices);
  return res;
}

size_t max_image_depth(cl_context context, cl_mem_object_type image_type)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t res = 0;
  cl_device_info name;
  switch (image_type) {
    case CL_MEM_OBJECT_IMAGE3D:
      name = CL_DEVICE_IMAGE3D_MAX_DEPTH;
      break;

    default:
      printf("Bad image type\n");
      return 0;
  }

  // find maximum image depth for all devices in the context
  size_t size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      name,
      sizeof(size_t),
      &size,
      NULL);
    if (size > res)
      res = size;
  }

  free(devices);
  return res;
}

size_t max_image_array(cl_context context, cl_mem_object_type image_type)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  size_t res = 0;
  cl_device_info name;
  switch (image_type) {
    case CL_MEM_OBJECT_IMAGE1D_ARRAY:
    case CL_MEM_OBJECT_IMAGE2D_ARRAY:
      name = CL_DEVICE_IMAGE_MAX_ARRAY_SIZE;
      break;

    default:
      printf("Bad image type\n");
      return 0;
  }

  // find maximum image array size for all devices in the context
  size_t size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo(
      devices[i],
      name,
      sizeof(size_t),
      &size,
      NULL);
    if (size > res)
      res = size;
  }

  free(devices);
  return res;
}

cl_uint max_pitch_al(cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  cl_uint res = 0;

  // find maximum row pitch alignment size in pixels for 2D images 
  // created from a buffer for all devices in the context
  cl_uint size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo( // give 0 for devices not supporting such image creation
      devices[i],
      CL_DEVICE_IMAGE_PITCH_ALIGNMENT,
      sizeof(cl_uint),
      &size,
      NULL);
    if (size > res)
      res = size;
  }

  free(devices);
  return res;
}

cl_uint max_base_al(cl_context context)
{
  cl_uint nd;
  cl_device_id * devices = NULL;
  devices = get_devices(context, &nd);

  cl_uint res = 0;

  // find maximum base address alignment size in pixels for 2D images 
  // created from a buffer for all devices in the context
  cl_uint size;
  for (cl_uint i = 0; i < nd; ++i)
  {
    clGetDeviceInfo( // give 0 for devices not supporting such image creation
      devices[i],
      CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT,
      sizeof(cl_uint),
      &size,
      NULL);
    if (size > res)
      res = size;
  }

  free(devices);
  return res;
}

size_t pixel_size(const cl_image_format * image_format)
{
  size_t channels = 0;
  switch (image_format->image_channel_order) {
    case CL_R:
    case CL_A:
    case CL_DEPTH:
    case CL_LUMINANCE:
    case CL_INTENSITY:
      channels = 1;
      break;

    case CL_RG:
    case CL_RA:
    case CL_Rx:
      channels = 2;
      break;

// For 3 channels we can safely use 4 for pixel_size as all supported data types
// are powers of 2 in bit size (see below) and we are interesed in 
// the next or equal power of two of (channels * channel_size)*8 bits:
// "The number of bits per element determined by the image_channel_data_type 
// and image_channel_order must be a power of two."
    case CL_RGB:
    case CL_RGx:
    case CL_sRGB:
      channels = 4;
      break;

    case CL_RGBA:
    case CL_ARGB:
    case CL_BGRA:
    case CL_ABGR:
    case CL_RGBx:
    case CL_sRGBA:
    case CL_sBGRA:
    case CL_sRGBx:
      channels = 4;
      break;

    default:
      printf("Wrong image channel order!\n");
      return -1;
  }

  size_t channel_size = 0;
  switch (image_format->image_channel_data_type) {
    case CL_SNORM_INT8:
    case CL_UNORM_INT8:
    case CL_SIGNED_INT8:
    case CL_UNSIGNED_INT8:
      channel_size = 1;
      break;

    case CL_SNORM_INT16:
    case CL_UNORM_INT16:
    case CL_UNORM_SHORT_565:
    case CL_UNORM_SHORT_555:
    case CL_SIGNED_INT16:
    case CL_UNSIGNED_INT16:
    case CL_HALF_FLOAT:
      channel_size = 2;
      break;

    case CL_UNORM_INT_101010:
    case CL_UNORM_INT_101010_2:
    case CL_SIGNED_INT32:
    case CL_UNSIGNED_INT32:
    case CL_FLOAT:
      channel_size = 4;
      break;

    default:
      printf("Wrong image channel data type!\n");
      return -1;
  }

  return channels * channel_size;
}

// check if the descriptor of image being created is compatible with
// one of the image from which the current image is being created
bool is_compatible_image(const cl_image_desc * const image_desc, const cl_image_format * image_format)
{
  cl_image_format format;
  clGetImageInfo(
    image_desc->mem_object,
    CL_IMAGE_FORMAT,
    sizeof(cl_image_format),
    &format,
    NULL);

  if (format.image_channel_data_type != image_format->image_channel_data_type)
    return false;
  // image channel order must be compatible
  switch (image_format->image_channel_order) {
    case CL_sBGRA:
      if (format.image_channel_order != CL_BGRA)
        return false;
      break;

    case CL_BGRA:
      if (format.image_channel_order != CL_sBGRA)
        return false;
      break;

    case CL_sRGBA:
      if (format.image_channel_order != CL_RGBA)
        return false;
      break;

    case CL_RGBA:
      if (format.image_channel_order != CL_sRGBA)
        return false;
      break;

    case CL_sRGB:
      if (format.image_channel_order != CL_RGB)
        return false;
      break;

    case CL_RGB:
      if (format.image_channel_order != CL_sRGB)
        return false;
      break;

    case CL_sRGBx:
      if (format.image_channel_order != CL_RGBx)
        return false;
      break;

    case CL_RGBx:
      if (format.image_channel_order != CL_sRGBx)
        return false;
      break;

    case CL_DEPTH:
      if (format.image_channel_order != CL_R)
        return false;
      break;

    default:
      return false;
  }

  size_t size = 0;

  clGetImageInfo(
    image_desc->mem_object,
    CL_IMAGE_WIDTH,
    sizeof(size_t),
    &size,
    NULL);
  if (size != image_desc->image_width)
    return false;

  clGetImageInfo(
    image_desc->mem_object,
    CL_IMAGE_HEIGHT,
    sizeof(size_t),
    &size,
    NULL);
  if (size != image_desc->image_height)
    return false;

  clGetImageInfo(
    image_desc->mem_object,
    CL_IMAGE_DEPTH,
    sizeof(size_t),
    &size,
    NULL);
  if (size != 0)
    return false;

  clGetImageInfo(
    image_desc->mem_object,
    CL_IMAGE_ARRAY_SIZE,
    sizeof(size_t),
    &size,
    NULL);
  if (size != 0)
    return false;

  size_t image_row_pitch = image_desc->image_width * pixel_size(image_format);
  if ((image_desc->image_row_pitch < image_row_pitch) &&
    (image_desc->image_row_pitch != 0))
    return false;
  if (image_desc->image_row_pitch != 0)
    image_row_pitch = image_desc->image_row_pitch;
  clGetImageInfo(
    image_desc->mem_object,
    CL_IMAGE_ROW_PITCH,
    sizeof(size_t),
    &size,
    NULL);
  if (size != image_row_pitch)
    return false;

  // num_mip_levels and num_samples are always the same and = 0 - no need to check
  // mem_object can be different

  return true;
}

size_t buffer_size(cl_mem buffer)
{
  size_t size = 0;
  clGetMemObjectInfo(
    buffer,
    CL_MEM_SIZE,
    sizeof(size_t),
    &size,
    NULL);
  return size;
}


// 5.3.1.1. Image Format Descriptor

// check image_format violation
bool struct_violation(
  const cl_image_format * const image_format)
{
  if (enum_violation("cl_channel_order", image_format->image_channel_order))
    return true;
  if (enum_violation("cl_channel_type", image_format->image_channel_data_type))
    return true;

  if (((image_format->image_channel_data_type == CL_UNORM_SHORT_555) ||
     (image_format->image_channel_data_type == CL_UNORM_SHORT_565) ||
     (image_format->image_channel_data_type == CL_UNORM_INT_101010)) &&
    !((image_format->image_channel_order == CL_RGB) || (image_format->image_channel_order == CL_RGBx)))
    return true;
  if ((image_format->image_channel_data_type == CL_UNORM_INT_101010_2) &&
    !(image_format->image_channel_order == CL_RGBA))
    return true;

  return false;
}

// check image_format violation and correctness of 2D image creation from buffer or 2D image
bool struct_violation(
  const cl_image_format * const image_format,
  cl_context context,
  const cl_image_desc * const image_desc)
{
  if (enum_violation("cl_channel_order", image_format->image_channel_order))
    return true;
  if (enum_violation("cl_channel_type", image_format->image_channel_data_type))
    return true;

  if (((image_format->image_channel_data_type == CL_UNORM_SHORT_555) ||
     (image_format->image_channel_data_type == CL_UNORM_SHORT_565) ||
     (image_format->image_channel_data_type == CL_UNORM_INT_101010)) &&
    !((image_format->image_channel_order == CL_RGB) || (image_format->image_channel_order == CL_RGBx)))
    return true;
  if ((image_format->image_channel_data_type == CL_UNORM_INT_101010_2) &&
    !(image_format->image_channel_order == CL_RGBA))
    return true;

  // if 2D image is created from the buffer
  if ((image_desc->image_type == CL_MEM_OBJECT_IMAGE2D) &&
    (image_desc->mem_object != NULL) &&
    object_is_valid(image_desc->mem_object, CL_MEM_OBJECT_BUFFER))
  {
    // calculate image_row_pitch and check if it is too low
    size_t image_row_pitch = image_desc->image_width * pixel_size(image_format);
    if ((image_desc->image_row_pitch < image_row_pitch) &&
      (image_desc->image_row_pitch != 0))
      return true;
    if (image_desc->image_row_pitch != 0)
      image_row_pitch = image_desc->image_row_pitch;
    // check if it is multiple of a pixel
    if (image_row_pitch % pixel_size(image_format) != 0)
      return true;
    // check if aligned properly
    if (image_row_pitch % max_pitch_al(context) != 0)
      return true;

    // check if base pointer is aligned properly
    cl_mem_flags flags;
    clGetMemObjectInfo(
      image_desc->buffer,
      CL_MEM_FLAGS,
      sizeof(cl_mem_flags),
      &flags,
      NULL);
    if (flags | CL_MEM_USE_HOST_PTR)
    { 
      void * host_ptr;
      clGetMemObjectInfo(
        image_desc->buffer,
        CL_MEM_HOST_PTR,
        sizeof(void *),
        &host_ptr,
        NULL);
      if ((uintptr_t)host_ptr % max_base_al(context) != 0)
        return true;
    }
  }

  // if 2D image is created from 2D image
  if ((image_desc->image_type == CL_MEM_OBJECT_IMAGE2D) &&
    (image_desc->mem_object != NULL) &&
    object_is_valid(image_desc->mem_object, CL_MEM_OBJECT_IMAGE2D))
  {
    if (!is_compatible_image(image_desc, image_format))
      return true;
  }

  return false;
}

// 5.3.1.2. Image Descriptor

// check memory flags
bool struct_violation(
  const cl_image_desc * const image_desc,
  cl_mem_flags flags)
{
  // check image creation from buffer or other image
  switch (image_desc->image_type) {
    case CL_MEM_OBJECT_IMAGE1D_BUFFER:
    case CL_MEM_OBJECT_IMAGE2D:
      if (image_desc->mem_object != NULL) {
        cl_mem_flags old_flags;
        clGetMemObjectInfo(
          image_desc->mem_object,
          CL_MEM_FLAGS,
          sizeof(cl_mem_flags),
          &old_flags,
          NULL);

        if ((old_flags | CL_MEM_WRITE_ONLY) && 
          ((flags | CL_MEM_READ_WRITE) || (flags | CL_MEM_READ_ONLY)))
          return true;
        if ((old_flags | CL_MEM_READ_ONLY) && 
          ((flags | CL_MEM_READ_WRITE) || (flags | CL_MEM_WRITE_ONLY)))
          return true;
        if ((flags | CL_MEM_USE_HOST_PTR) || 
          (flags | CL_MEM_ALLOC_HOST_PTR) ||
          (flags | CL_MEM_COPY_HOST_PTR))
          return true;
        if ((old_flags | CL_MEM_HOST_WRITE_ONLY) && 
          (flags | CL_MEM_HOST_READ_ONLY))
          return true;
        if ((old_flags | CL_MEM_HOST_READ_ONLY) && 
          (flags | CL_MEM_HOST_WRITE_ONLY))
          return true;
        if ((old_flags | CL_MEM_HOST_NO_ACCESS) && 
          ((flags | CL_MEM_HOST_READ_ONLY) || (flags | CL_MEM_HOST_WRITE_ONLY)))
          return true;
      }
      break;

    default:
      if (image_desc->mem_object != NULL)
        return true;
  }

  return false;
}

// check image sizes to fit into some device of the context
bool struct_violation(
  const cl_image_desc * const image_desc, 
  cl_context context)
{
  switch (image_desc->image_type) {
    case CL_MEM_OBJECT_IMAGE3D:
      return !is_3D_image_fits(image_desc, context);

    case CL_MEM_OBJECT_IMAGE2D:
      return !is_2D_image_fits(image_desc, context);

    case CL_MEM_OBJECT_IMAGE2D_ARRAY:
      return !is_2D_array_fits(image_desc, context);

    case CL_MEM_OBJECT_IMAGE1D:
      return !is_1D_image_fits(image_desc, context);

    case CL_MEM_OBJECT_IMAGE1D_ARRAY:
      return !is_1D_array_fits(image_desc, context);

    case CL_MEM_OBJECT_IMAGE1D_BUFFER:
      return !is_1D_buffer_fits(image_desc, context);

    default: // unknown type
      return true;
  }
}

// check size of host_ptr
bool struct_violation(
  const cl_image_desc * const image_desc, 
  void * host_ptr,
  const cl_image_format * image_format)
{
  if (host_ptr != NULL) {
    size_t image_row_pitch = 
      std::max(image_desc->image_row_pitch, 
        image_desc->image_width * pixel_size(image_format));

    switch (image_desc->image_type) {
      case CL_MEM_OBJECT_IMAGE1D:
      case CL_MEM_OBJECT_IMAGE1D_BUFFER:
        return array_len_ls(host_ptr, image_row_pitch);

      case CL_MEM_OBJECT_IMAGE1D_ARRAY:
        return array_len_ls(host_ptr, 
          image_desc->image_array_size * 
            std::max(image_desc->image_slice_pitch, image_row_pitch));

      case CL_MEM_OBJECT_IMAGE2D:
        return array_len_ls(host_ptr, 
          image_row_pitch * image_desc->image_height);

      case CL_MEM_OBJECT_IMAGE2D_ARRAY:
        return array_len_ls(host_ptr, 
          image_desc->image_array_size * 
            std::max(image_desc->image_slice_pitch, 
              image_row_pitch * image_desc->image_height));

      case CL_MEM_OBJECT_IMAGE3D:
        return array_len_ls(host_ptr, 
          image_desc->image_depth * 
            std::max(image_desc->image_slice_pitch, 
              image_row_pitch * image_desc->image_height));

      default:
        return true;
    }
  }
  return false;
}

// check all besides checked above
bool struct_violation(
  const cl_image_desc * const image_desc, 
  cl_context context,
  void * host_ptr,
  const cl_image_format * image_format)
{
  // check image types and sizes (upper limits are checked in prev function)
  switch (image_desc->image_type) {
    case CL_MEM_OBJECT_IMAGE3D:
      if (image_desc->image_depth == 0)
        return true;

    case CL_MEM_OBJECT_IMAGE2D:
    case CL_MEM_OBJECT_IMAGE2D_ARRAY:
      if (image_desc->image_height == 0)
        return true;

    case CL_MEM_OBJECT_IMAGE1D:
    case CL_MEM_OBJECT_IMAGE1D_BUFFER:
    case CL_MEM_OBJECT_IMAGE1D_ARRAY:
      if (image_desc->image_width == 0)
        return true;
      break;
    
    default:
      return true;
  }

  // check array size
  switch (image_desc->image_type) {
    case CL_MEM_OBJECT_IMAGE2D_ARRAY: 
    case CL_MEM_OBJECT_IMAGE1D_ARRAY:
      if ((image_desc->image_array_size == 0) || 
        (image_desc->image_array_size > max_image_array(context, image_desc->image_type)))
        return true;
  }

  // check image_row_pitch
  if ((host_ptr == NULL) && (image_desc->image_row_pitch != 0))
    return true;
  if ((host_ptr != NULL) && (image_desc->image_row_pitch > 1) && 
    (image_desc->image_row_pitch < image_desc->image_width * pixel_size(image_format)))
    return true;
  if (image_desc->image_row_pitch % pixel_size(image_format) != 0)
    return true;
  size_t image_row_pitch = 
    std::max(image_desc->image_row_pitch, image_desc->image_width * pixel_size(image_format));
  // it is checked in image_format
  //if ((image_desc->image_type == CL_MEM_OBJECT_IMAGE2D) &&
  //  (image_desc->mem_object != NULL) &&
  //  is_valid_mem_obj(image_desc->mem_object, "buffer") &&
  //  (image_row_pitch % max_al(context) != 0))
  //  return true;

  // check image_slice_pitch
  if ((host_ptr == NULL) && (image_desc->image_slice_pitch != 0))
    return true;
  if (host_ptr != NULL)
    switch (image_desc->image_type) {
      case CL_MEM_OBJECT_IMAGE3D:
      case CL_MEM_OBJECT_IMAGE2D_ARRAY:
        if ((image_desc->image_slice_pitch > 1) && 
          (image_desc->image_slice_pitch < image_row_pitch * image_desc->image_height))
          return true;
        break;

      case CL_MEM_OBJECT_IMAGE1D_ARRAY:
        if ((image_desc->image_slice_pitch > 1) && 
          (image_desc->image_slice_pitch < image_row_pitch))
          return true;
        break;
    }
  if (image_desc->image_slice_pitch % image_row_pitch != 0)
    return true;

  // check image creation from buffer or other image
  switch (image_desc->image_type) {
    case CL_MEM_OBJECT_IMAGE1D_BUFFER:
      if (image_desc->mem_object != NULL)
        if (!object_is_valid(image_desc->mem_object, CL_MEM_OBJECT_BUFFER) ||
          (buffer_size(image_desc->mem_object) < image_desc->image_width * pixel_size(image_format)))
          return true;
      break;

    case CL_MEM_OBJECT_IMAGE2D:
      if (image_desc->mem_object != NULL)
        if ((!object_is_valid(image_desc->mem_object, CL_MEM_OBJECT_BUFFER) ||
          (buffer_size(image_desc->mem_object) < image_row_pitch * image_desc->image_height)) &&
          (!object_is_valid(image_desc->mem_object, CL_MEM_OBJECT_IMAGE2D)
          //|| !is_compatible_image(image_desc, image_format)
          ))
          return true;
      break;

    default:
      if (image_desc->mem_object != NULL)
        return true;
  }

  if ((image_desc->num_mip_levels != 0) || (image_desc->num_samples != 0))
    return true;

}

// tenmporary - to remove !!!
cl_int clSetCommandQueueProperty(
  cl_command_queue command_queue,
  cl_command_queue_properties properties,
  cl_bool enable,
  cl_command_queue_properties * old_properties)
{
  return 0;
}