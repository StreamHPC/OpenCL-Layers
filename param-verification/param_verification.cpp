#include "param_verification.hpp"
#include <fstream>

struct _cl_icd_dispatch dispatch = {};

const struct _cl_icd_dispatch *tdispatch;

ocl_layer_utils::stream_ptr log_stream;

namespace {
  struct layer_settings {
    enum class DebugLogType { StdOut, StdErr, File };

    static layer_settings load();

    DebugLogType log_type = DebugLogType::StdErr;
    std::string log_filename;
  };

  layer_settings layer_settings::load() {
    const auto settings_from_file = ocl_layer_utils::load_settings();
    const auto parser =
      ocl_layer_utils::settings_parser("param_verification", settings_from_file);

    auto settings = layer_settings{};
    const auto debug_log_values =
      std::map<std::string, DebugLogType>{{"stdout", DebugLogType::StdOut},
                                          {"stderr", DebugLogType::StdErr},
                                          {"file", DebugLogType::File}};
    parser.get_enumeration("log_sink", debug_log_values, settings.log_type);
    parser.get_filename("log_filename", settings.log_filename);

    return settings;
  }

  layer_settings settings;

  void init_output_stream() {
    switch(settings.log_type) {
    case layer_settings::DebugLogType::StdOut:
      log_stream.reset(&std::cout);
      break;
    case layer_settings::DebugLogType::StdErr:
      log_stream.reset(&std::cerr);
      break;
    case layer_settings::DebugLogType::File:
      log_stream.reset(new std::ofstream(settings.log_filename));
      if (log_stream->fail()) {
        log_stream.reset(&std::cerr);
        *log_stream << "param_verification failed to open specified output stream: "
                    << settings.log_filename << ". Falling back to stderr." << '\n';
      }
      break;
    }
  }
}

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

CL_API_ENTRY cl_int CL_API_CALL
clInitLayer(
    cl_uint                         num_entries,
    const struct _cl_icd_dispatch  *target_dispatch,
    cl_uint                        *num_entries_out,
    const struct _cl_icd_dispatch **layer_dispatch_ret) {
  if (!target_dispatch || !layer_dispatch_ret || !num_entries_out || num_entries < sizeof(dispatch) / sizeof(dispatch.clGetPlatformIDs))
    return CL_INVALID_VALUE;

  settings = layer_settings::load();
  init_output_stream();

  tdispatch = target_dispatch;
  init_dispatch();

  *layer_dispatch_ret = &dispatch;
  *num_entries_out = sizeof(dispatch)/sizeof(dispatch.clGetPlatformIDs);
  return CL_SUCCESS;
}
