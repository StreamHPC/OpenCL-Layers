#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"

using namespace rapidxml;

int main()
{
    // stream for generated code
    std::stringstream code;

    xml_document<> doc;
    xml_node<> * root_node;
    // Read the xml file into a vector
    std::ifstream theFile ("cl-avl.xml");
    std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());\
    buffer.push_back('\0');
    // Parse the buffer using the xml file parsing library into doc 
    doc.parse<0>(&buffer[0]);

    // Find our root node
    root_node = doc.first_node("registry");

    ///////////////////////////////////////////////////////////////////////
    // enums
    ///////////////////////////////////////////////////////////////////////

    code << "#include <cl.h>\n\n"
         << "template<typename T>\n"
         << "bool enum_violation(const char * name, T param)\n"
         << "{\n";

    // Iterate over the versions
    for (xml_node<> * version_node = root_node->first_node("feature");
        version_node != nullptr;
        version_node = version_node->next_sibling("feature"))
    {
        code << "  if (from(\"" << version_node->first_attribute("number")->value() << "\")) {\n";

        std::vector<std::string> enums_list = 
            {"cl_platform_info", 
            "cl_device_info",
            "cl_context_info",
            "cl_command_queue_info",
            "cl_image_info",
            "cl_mem_info",
            "cl_addressing_mode",
            "cl_filter_mode",
            "cl_sampler_info",
            "cl_program_info",
            "cl_program_build_info",
            "cl_kernel_exec_info",
            "cl_kernel_info",
            "cl_kernel_work_group_info",
            "cl_kernel_sub_group_info",
            "cl_kernel_arg_info",
            "cl_event_info",
            "cl_profiling_info",
            "cl_channel_order",
            "cl_channel_type"};
        
        for (auto i : enums_list) {
            for (xml_node<> * enum_node = version_node->first_node("require");
                enum_node != nullptr;
                enum_node = enum_node->next_sibling("require")) {
                    if (enum_node->first_attribute("comment") != nullptr) {
                        //printf("I have visited %s.\n", 
                        //    enum_node->first_attribute("comment")->value());
                        if (strstr(enum_node->first_attribute("comment")->value(), i.c_str()) != nullptr) {
                            code << "    if (strcmp(name, \"" << i << "\") == 0)\n"
                                 << "      switch (param) {\n";

                            for (xml_node<> * enum_val = enum_node->first_node("enum");
                                enum_val;
                                enum_val = enum_val->next_sibling("enum")) {
                                code << "        case " << enum_val->first_attribute("name")->value() << ":\n";
                            }

                            code << "          return false;\n"
                                 << "      }\n\n";
                        }
                    }
                }
        }

        code << "  }\n\n";
        //printf("I have visited %s.\n", 
        //    version_node->first_attribute("number")->value());
        //    version_node->value());
    }

    code << "  return true;\n"
         << "}\n\n";
    //printf("\n");

    ///////////////////////////////////////////////////////////////////////
    // literal lists
    ///////////////////////////////////////////////////////////////////////

    code << "#include <cl.h>\n\n"
         << "template<typename T>\n"
         << "size_t literal_list(const char * name, T param)\n"
         << "{\n";

    // Iterate over the versions
    for (xml_node<> * version_node = root_node->first_node("feature");
        version_node != nullptr;
        version_node = version_node->next_sibling("feature"))
    {
        code << "  if (from(\"" << version_node->first_attribute("number")->value() << "\")) {\n";

        std::vector<std::string> enums_list = 
            {"cl_platform_info",
            "cl_device_info",
            "cl_context_info",
            "cl_command_queue_info",
            "cl_image_info",
            "cl_pipe_info",
            "cl_mem_info",
            "cl_sampler_info",
            "cl_program_info",
            "cl_program_build_info",
            "cl_kernel_exec_info",
            "cl_kernel_info",
            "cl_kernel_work_group_info",
            "cl_kernel_sub_group_info",
            "cl_kernel_arg_info",
            "cl_event_info",
            "cl_profiling_info"};

        for (auto i : enums_list) {
            for (xml_node<> * enum_node = version_node->first_node("require");
                enum_node != nullptr;
                enum_node = enum_node->next_sibling("require")) {
                    if (enum_node->first_attribute("comment") != nullptr) {
                        //printf("I have visited %s.\n", 
                        //    enum_node->first_attribute("comment")->value());
                        if (strstr(enum_node->first_attribute("comment")->value(), i.c_str()) != nullptr) {
                            code << "    if (strcmp(name, \"" << i << "\") == 0)\n"
                                 << "      switch (param) {\n";

                            for (xml_node<> * enum_val = enum_node->first_node("enum");
                                enum_val != nullptr;
                                enum_val = enum_val->next_sibling("enum")) {
                                if (strstr(enum_val->first_attribute("return_type")->value(), "[]") == 
                                    enum_val->first_attribute("return_type")->value() + strlen(enum_val->first_attribute("return_type")->value()) - 2)
                                    enum_val->first_attribute("return_type")->value()[strlen(enum_val->first_attribute("return_type")->value()) - 2] = '\0';
                                code << "        case " << enum_val->first_attribute("name")->value() << ":\n"
                                     << "          return sizeof(" << enum_val->first_attribute("return_type")->value() << ");\n";
                            }

                            code << "          return false;\n"
                                 << "      }\n\n";
                        }
                    }
                }
        }

        code << "  }\n\n";
        printf("I have visited %s.\n", 
            version_node->first_attribute("number")->value());
        //    version_node->value());
    }

    code << "  return 0;\n"
         << "}\n\n";
    //printf("\n");






    std::ofstream file("res.cpp");
    file << code.str();
    file.close();
    //std::cout << code.str();
    return 0;
}