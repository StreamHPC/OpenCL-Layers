#include "object_lifetime_test.hpp"

int main(int argc, char *argv[]) {
  cl_platform_id platform;
  cl_device_id device;
  cl_int status;
  layer_test::setup(argc, argv, CL_MAKE_VERSION(1, 2, 0), platform, device);

  cl_context context = layer_test::createContext(platform, device);

  cl_image_desc desc = {
    CL_MEM_OBJECT_IMAGE2D, // image_type
    2,                     // image_width
    2,                     // image_height
    1,                     // image_depth
    1,                     // image_array size
    0,                     // image_row_pitch
    0,                     // image_slice_pitch
    0,                     // num_mip_levels
    0,                     // num_samples
    nullptr                // mem_object
  };
  cl_image_format format = {CL_R, CL_UNORM_INT8};
  cl_mem image = clCreateImage(context,
                               CL_MEM_READ_ONLY,
                               &format,
                               &desc,
                               nullptr,
                               &status);
  EXPECT_SUCCESS(status);
  EXPECT_REF_COUNT(image, 1, 0);

  cl_image_desc sub_desc = {
    CL_MEM_OBJECT_IMAGE2D, // image_type
    1,                     // image_width
    1,                     // image_height
    1,                     // image_depth
    1,                     // image_array size
    0,                     // image_row_pitch
    0,                     // image_slice_pitch
    0,                     // num_mip_levels
    0,                     // num_samples
    image                  // mem_object
  };
  cl_mem sub_image = clCreateImage(context,
                                   CL_MEM_READ_ONLY,
                                   &format,
                                   &desc,
                                   nullptr,
                                   &status);
  EXPECT_SUCCESS(status);
  EXPECT_REF_COUNT(sub_image, 1, 0);
  EXPECT_REF_COUNT(image, 1, 0);

  // Release image. In this case, sub_image should not keep image alive.
  EXPECT_SUCCESS(clReleaseMemObject(image));
  EXPECT_REF_COUNT(sub_image, 1, 0);
  EXPECT_DESTROYED(image);

  EXPECT_SUCCESS(clReleaseMemObject(sub_image));
  EXPECT_DESTROYED(sub_image);

  EXPECT_SUCCESS(clReleaseContext(context));
  EXPECT_DESTROYED(context);

  return layer_test::finalize();
}

