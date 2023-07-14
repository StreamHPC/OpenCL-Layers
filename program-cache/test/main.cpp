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

#include <CL/opencl.hpp>

#include <iostream>
#include <string>

int main()
{
    auto context = cl::Context::getDefault();

    const std::string program_source = "kernel void foo(global int* i){ *i = 100; }";
    cl::Program program(context, program_source);
    program.build();

    cl::KernelFunctor<cl::Buffer> kernel(program, "foo");
    cl::Buffer output(context, CL_MEM_WRITE_ONLY, sizeof(cl_int));

    kernel(cl::EnqueueArgs(cl::NDRange(1)), output);

    cl_int result{};
    cl::enqueueReadBuffer(output, true, 0, sizeof(result), &result);

    std::cout << "Result: " << result << std::endl;
    if (result != 100)
    {
        return -1;
    }
}
