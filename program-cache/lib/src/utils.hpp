/*
 * Copyright (c) 2023 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * OpenCL is a trademark of Apple Inc. used under license by Khronos.
 */

#ifndef OCL_PROGRAM_CACHE_LIB_SRC_UTILS_HPP_
#define OCL_PROGRAM_CACHE_LIB_SRC_UTILS_HPP_

#include <ocl_program_cache/common.hpp>

#include <charconv>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#define CHECK_CL_ERROR(expression)                                             \
    {                                                                          \
        if (const cl_int __error = (expression); __error != CL_SUCCESS)        \
            throw ::ocl::program_cache::opencl_error(__error);                 \
    }

#define CHECK_CL_BUILD_ERROR(expression)                                       \
    {                                                                          \
        if (const cl_int __error = (expression); __error != CL_SUCCESS)        \
            throw ::ocl::program_cache::opencl_build_error(__error);           \
    }

namespace ocl::program_cache::utils {

inline std::vector<std::string_view> split(std::string_view input,
                                           char delimiter = ' ')
{
    std::size_t pos = input.find_first_not_of(delimiter);
    if (pos == std::string_view::npos) return {};
    std::vector<std::string_view> ret;
    do
    {
        const auto last_pos = pos;
        pos = input.find(delimiter, pos);
        ret.push_back(input.substr(last_pos, pos - last_pos));
        pos = input.find_first_not_of(delimiter, pos);
    } while (pos != std::string_view::npos);
    return ret;
}

inline bool starts_with(std::string_view str, std::string_view start)
{
    return str.find(start) == 0;
}

template <class T, class Fun, class Param>
std::string get_info_str(T obj, Fun fun, Param param_name)
{
    std::size_t param_value_size{};
    CHECK_CL_ERROR(
        fun(obj, param_name, param_value_size, nullptr, &param_value_size));
    std::string ret;
    ret.resize(param_value_size);
    CHECK_CL_ERROR(
        fun(obj, param_name, param_value_size, ret.data(), &param_value_size));
    return ret;
}

namespace detail {

inline int parse_int(char c)
{
    int val;
    if (auto [ptr, ec] = std::from_chars(&c, &c + 1, val); ec != std::errc{})
    {
        throw bad_opencl_version_format();
    }
    return val;
}

} // namespace detail

inline std::pair<int, int>
parse_platform_opencl_version(std::string_view version_string)
{
    // Version format must be
    // clang-format off
    // OpenCL<space><major_version.minor_version><space><platform-specific-information>
    // clang-format on
    if (version_string.size() < 10) throw bad_opencl_version_format();
    return { detail::parse_int(version_string[7]),
             detail::parse_int(version_string[9]) };
}

inline std::pair<int, int>
parse_device_opencl_c_version(std::string_view version_string)
{
    // Version format must be
    // clang-format off
    // OpenCL<space>C<space><major_version.minor_version><space><vendor-specific information>
    // clang-format on
    if (version_string.size() < 12) throw bad_opencl_version_format();
    return { detail::parse_int(version_string[9]),
             detail::parse_int(version_string[11]) };
}

template <class... Args> struct overloads : Args...
{
    using Args::operator()...;
};

template <class... Ts> overloads(Ts...) -> overloads<Ts...>;

template <class Fun> class final_action {
public:
    final_action(Fun fun): fun_(fun), execute_(true) {}

    ~final_action()
    {
        if (execute_)
        {
            fun_();
        }
    }

    final_action(const final_action& other) = delete;
    final_action(final_action&& other)
    {
        this->fun_ = other.fun_;
        other.execute_ = false;
    }

    final_action& operator=(const final_action& other) = delete;
    final_action& operator=(final_action&& other)
    {
        this->fun_ = other.fun_;
        other.execute_ = false;
    }

private:
    Fun fun_;
    bool execute_;
};

} // namespace ocl::program_cache::utils

#endif // OCL_PROGRAM_CACHE_LIB_SRC_UTILS_HPP_