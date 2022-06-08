#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include "rapidxml-1.13/rapidxml.hpp"

using namespace rapidxml;

std::map<std::string, std::string> func_params;

// prototypes
std::string parse_violation(xml_node<> const * const violation);
std::array<std::string, 2> parse_2expressions(xml_node<> const * const node);

std::string parse_expression(xml_node<> const * const node)
{
    std::string res;
    char const * const name = node->name();

    if (strcmp(name, "name") == 0) {
        res = node->value();
    }
    else if (strcmp(name, "literal") == 0) {
        res = node->value();
    }
    else if (strcmp(name, "literal_list") == 0) {
        res = node->value();
        res = "literal_list(\"" + func_params[res] + "\", " + res + ")";
    }
    else if (strcmp(name, "sizeof") == 0) {
        res = "sizeof(" + std::string(node->value()) + ")";
    }
    else if (strcmp(name, "mod") == 0) {
        auto list = parse_2expressions(node);

        res = "(" + list[0] + " % " + list[1] + ")";
    }
    else if (strcmp(name, "mult") == 0) {
        auto list = parse_2expressions(node);

        res = "(" + list[0] + " * " + list[1] + ")";
    }
    else if (strcmp(name, "max") == 0) {
        auto list = parse_2expressions(node);

        res = "std::max(" + list[0] + ", " + list[1] + ")";
    }
    else if (strcmp(name, "query") == 0) {
        res = "query<" + std::string(node->first_attribute("property")->value()) + ">(" +
            node->first_attribute("object")->value() + ")";
    }

    return res;
}

std::array<std::string, 2> parse_2expressions(xml_node<> const * const node)
{
    std::array<std::string, 2> res;

    res[0] = parse_expression(node->first_node());
    res[1] = parse_expression(node->first_node()->next_sibling());

    return res;
}

std::vector<std::string> parse_list(xml_node<> const * const node)
{
    std::vector<std::string> res;

    for (xml_node<> const * list_node = node->first_node();
        list_node != nullptr;
        list_node = list_node->next_sibling())
    {
        res.push_back(parse_violation(list_node));
        //res.push_back(std::string(list_node->value()));
    }

    return res;
}

std::string parse_violation(xml_node<> const * const violation)
{
    std::string test;

    if (violation != nullptr)
    {
        char const * const name = violation->name();

        if (strcmp(name, "or") == 0)
        {
            //printf("or:");
            std::vector<std::string> list(parse_list(violation));

            int n = 0;
            for (auto a : list)
            {
                test += (n == 0) ? "(" : " ||\n    ";
                test += a;
                ++n;
                //printf("%s ", a.c_str());
            }
            test += ")";
            //printf("\n");
        }
        else if (strcmp(name, "and") == 0)
        {
            //printf("and:");
            std::vector<std::string> list(parse_list(violation));

            int n = 0;
            for (auto a : list)
            {
                test += (n == 0) ? "(" : " &&\n    ";
                test += a;
                ++n;
                //printf("%s ", a.c_str());
            }
            test += ")";
            //printf("\n");
        }
        else if (strcmp(name, "mutex_violation") == 0)
        {
            std::vector<std::string> list(parse_list(violation));

            int n = 0;
            for (auto a : list)
            {
                test += (n == 0) ? "((bool)" : " +\n    (bool)";
                test += a;
                ++n;
            }
            test += " > 1)";
        }
        else if (strcmp(name, "not") == 0)
        {
            test = "(!(" + parse_violation(violation->first_node()) + "))";
        }
        else if (strcmp(name, "eq") == 0)
        {
            auto list = parse_2expressions(violation);

            test = "(" + list[0] + " == " + list[1] + ")";
        }
        else if (strcmp(name, "neq") == 0)
        {
            auto list = parse_2expressions(violation);

            test = "(" + list[0] + " != " + list[1] + ")";
        }
        else if (strcmp(name, "ls") == 0)
        {
            auto list = parse_2expressions(violation);

            test = "(" + list[0] + " < " + list[1] + ")";
        }
        else if (strcmp(name, "gt") == 0)
        {
            auto list = parse_2expressions(violation);

            test = "(" + list[0] + " > " + list[1] + ")";
        }
        else if (strcmp(name, "bit_and") == 0)
        {
            auto list = parse_2expressions(violation);

            test = "(" + list[0] + " & " + list[1] + ")";
        }
        else if (strcmp(name, "array_len_ls") == 0)
        {
            auto list = parse_2expressions(violation);

            test = "(array_len_ls(" + list[0] + ", " + list[1] + "))";
        }
        else if (strcmp(name, "enum_violation") == 0)
        {
            std::string tmp = violation->first_attribute("name")->value();

            test = "(enum_violation(\"" + func_params[tmp] + "\", " + tmp + "))";
        }
        else if (strcmp(name, "bitfield_violation") == 0)
        {
            std::string tmp = violation->first_attribute("name")->value();

            test = "(bitfield_violation(\"" + func_params[tmp] + "\", " + tmp + "))";
        }
        else if (strcmp(name, "list_violation") == 0)
        {
            std::string tmp = violation->first_attribute("name")->value();

            if (violation->first_attribute("param"))
                test = "(list_violation(\"" + func_params[tmp] + "\", " + tmp + ", "
                    + violation->first_attribute("param")->value() + "))";
            else
                test = "(list_violation(\"" + func_params[tmp] + "\", " + tmp + "))";
        }
        else if (strcmp(name, "struct_violation") == 0)
        {
            std::string tmp = violation->first_attribute("name")->value();

            if (violation->first_attribute("param"))
                test = "(struct_violation(" + tmp + ", "
                    + violation->first_attribute("param")->value() + "))";
            else
                test = "(struct_violation(" + tmp + "))";
        }
        else if (strcmp(name, "object_is_invalid") == 0)
        {
            std::string tmp = violation->first_attribute("name")->value();

            if (violation->first_attribute("type"))
                test = "(!object_is_valid(" + tmp + ", "
                    + violation->first_attribute("type")->value() + "))";
            else
                test = "(!object_is_valid(" + tmp + "))";
        }
        else if (strcmp(name, "any_zero") == 0)
        {
            test = "(any_zero("
                + std::string(violation->first_attribute("array")->value())
                + ", "
                + std::string(violation->first_attribute("elements")->value())
                + "))";
        }
        else if (strcmp(name, "any_nullptr") == 0)
        {
            test = "(any_nullptr("
                + std::string(violation->first_attribute("array")->value())
                + ", "
                + std::string(violation->first_attribute("elements")->value())
                + "))";
        }
        else if (strcmp(name, "any_invalid") == 0)
        {
            test = "(any_invalid("
                + std::string(violation->first_attribute("array")->value())
                + ", "
                + std::string(violation->first_attribute("elements")->value())
                + "))";
        }
        else if (strcmp(name, "any_not_available") == 0)
        {
            test = "(any_not_available("
                + std::string(violation->first_attribute("array")->value())
                + ", "
                + std::string(violation->first_attribute("elements")->value())
                + "))";
        }
        else if (strcmp(name, "object_not_in") == 0)
        {
            test = "(object_not_in("
                + std::string(violation->first_attribute("object")->value())
                + ", "
                + std::string(violation->first_attribute("in")->value())
                + "))";
        }
        else if (strcmp(name, "any_object_not_in") == 0)
        {
            test = "(any_object_not_in("
                + std::string(violation->first_attribute("array")->value())
                + ", "
                + std::string(violation->first_attribute("elements")->value())
                + ", "
                + std::string(violation->first_attribute("in")->value())
                + "))";
        }
        else if (strcmp(name, "from") == 0)
        {
            test = "(from(\"" + std::string(violation->first_attribute("version")->value()) + "\"))";
        }
/*        else if (strcmp(name, "name") == 0)
        {
            test = std::string(node->value());
        }
        else if (strcmp(name, "literal") == 0)
        {
            test = std::string(node->value());
        }*/
    }

    return test;
}

void parse_enums(std::stringstream& code, xml_node<> *& root_node)
{
    ///////////////////////////////////////////////////////////////////////
    // enums
    ///////////////////////////////////////////////////////////////////////

    code << "template<typename T>\n"
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

        for (auto a : enums_list) {
            for (xml_node<> * enum_node = version_node->first_node("require");
                enum_node != nullptr;
                enum_node = enum_node->next_sibling("require")) {
                    if (enum_node->first_attribute("comment") != nullptr) {
                        //printf("I have visited %s.\n",
                        //    enum_node->first_attribute("comment")->value());
                        if (strstr(enum_node->first_attribute("comment")->value(), a.c_str()) != nullptr) {
                            code << "    if (strcmp(name, \"" << a << "\") == 0)\n"
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
}

void parse_bitfields(std::stringstream& code, xml_node<> *& root_node)
{
    ///////////////////////////////////////////////////////////////////////
    // bitfields
    ///////////////////////////////////////////////////////////////////////

    code << "// function checks if there are set bits in the bitfield outside of defined\n"
         << "// 0 is then always valid param\n"
         << "template<typename T>\n"
         << "bool bitfield_violation(const char * name, T param)\n"
         << "{\n"
         << "  T mask = 0;\n\n";

    // Iterate over the versions
    for (xml_node<> * version_node = root_node->first_node("feature");
        version_node != nullptr;
        version_node = version_node->next_sibling("feature"))
    {
        code << "  if (from(\"" << version_node->first_attribute("number")->value() << "\")) {\n";

        std::vector<std::string> bitfields_list =
            {"cl_device_type",
            "cl_mem_flags",
            "cl_map_flags",
            "cl_mem_migration_flags",
            "cl_svm_mem_flags",
            "cl_device_affinity_domain"};

        for (auto i : bitfields_list) {
            for (xml_node<> * bitfield_node = version_node->first_node("require");
                bitfield_node != nullptr;
                bitfield_node = bitfield_node->next_sibling("require")) {
                    if (bitfield_node->first_attribute("comment") != nullptr) {
                        //printf("I have visited %s.\n",
                        //    bitfield_node->first_attribute("comment")->value());
                        if (strstr(bitfield_node->first_attribute("comment")->value(), i.c_str()) != nullptr) {
                            code << "    if (strcmp(name, \"" << i << "\") == 0) {\n";

                            for (xml_node<> * bitfield_val = bitfield_node->first_node("enum");
                                bitfield_val != nullptr;
                                bitfield_val = bitfield_val->next_sibling("enum")) {
                                code << "        mask |= " << bitfield_val->first_attribute("name")->value() << ";\n";
                            }

                            code << "      }\n\n";
                        }
                    }
                }
        }

        code << "  }\n\n";
        //printf("I have visited %s.\n",
        //    version_node->first_attribute("number")->value());
        //    version_node->value());
    }

    code << "  return (param & ~mask);\n"
         << "}\n\n";
    //printf("\n");
}

void parse_literal_lists(std::stringstream& code, xml_node<> *& root_node)
{
    ///////////////////////////////////////////////////////////////////////
    // literal lists
    ///////////////////////////////////////////////////////////////////////

    code << "template<typename T>\n"
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
                                // remove [] from the type - we demand at least 1 element for any returned array
                                std::string type = enum_val->first_attribute("return_type")->value();
                                type = std::regex_replace(type, std::regex("\\[\\]$"), "[1]");

                                code << "        case " << enum_val->first_attribute("name")->value() << ":\n"
                                     << "          return sizeof(" << type << ");\n";
                            }

                            code << "      }\n\n";
                        }
                    }
                }
        }

        code << "  }\n\n";
        //printf("I have visited %s.\n",
        //    version_node->first_attribute("number")->value());
        //    version_node->value());
    }

    code << "  printf(\"Unknown return type!\");\n"
         << "  return 0;\n"
         << "}\n\n";
    //printf("\n");

    code << "// special case of cl_image_format *\n"
         << "template<>\n"
         << "size_t literal_list(const char * name, cl_image_format * const param)\n"
         << "{\n"
         << "  (void)name;\n"
         << "  return pixel_size(param);\n"
         << "}\n"
         << "template<>\n"
         << "size_t literal_list(const char * name, const cl_image_format * const param)\n"
         << "{\n"
         << "  (void)name;\n"
         << "  return pixel_size(param);\n"
         << "}\n\n";

}

void parse_queries(std::stringstream& code, xml_node<> *& root_node)
{
    ///////////////////////////////////////////////////////////////////////
    // queries
    ///////////////////////////////////////////////////////////////////////

    code << "template<cl_uint property>\n"
         << "using return_type =\n";

    std::string end = ";\n";

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

    // Iterate over the versions
    for (xml_node<> * version_node = root_node->first_node("feature");
        version_node != nullptr;
        version_node = version_node->next_sibling("feature"))
    {
        for (auto i : enums_list) {
            for (xml_node<> * enum_node = version_node->first_node("require");
                enum_node != nullptr;
                enum_node = enum_node->next_sibling("require")) {
                    if (enum_node->first_attribute("comment") != nullptr) {
                        //printf("I have visited %s.\n",
                        //    enum_node->first_attribute("comment")->value());
                        if (strstr(enum_node->first_attribute("comment")->value(), i.c_str()) != nullptr) {

                            for (xml_node<> * enum_val = enum_node->first_node("enum");
                                enum_val != nullptr;
                                enum_val = enum_val->next_sibling("enum")) {
                                code << "  std::conditional_t<property == " << enum_val->first_attribute("name")->value() << ", "
                                     << enum_val->first_attribute("return_type")->value() << ",\n";
                                end = "> " + end;
                            }
                        }
                    }
                }
        }

        //printf("I have visited %s.\n",
        //    version_node->first_attribute("number")->value());
        //    version_node->value());
    }
    code << "  void" << end << "\n\n";

    code << "template<typename T, cl_uint property>\n"
         << "auto query(T object);\n\n";

    code << "template<cl_uint property>\n"
         << "auto query(cl_platform_id platform)\n"
         << "{\n"
         << "  return_type<property> a;\n"
         << "  clGetPlatformInfo(platform, property, sizeof(a), &a, NULL);\n"
         << "  return a;\n"
         << "}\n\n";

    code << "template<cl_uint property>\n"
         << "auto query(cl_device_id device)\n"
         << "{\n"
         << "  return_type<property> a;\n"
         << "  clGetDeviceInfo(device, property, sizeof(a), &a, NULL);\n"
         << "  return a;\n"
         << "}\n\n";

    code << "template<cl_uint property>\n"
         << "auto query(cl_context context)\n"
         << "{\n"
         << "  return_type<property> a;\n"
         << "  clGetContextInfo(context, property, sizeof(a), &a, NULL);\n"
         << "  return a;\n"
         << "}\n\n";

    code << "template<cl_uint property>\n"
         << "auto query(cl_command_queue queue)\n"
         << "{\n"
         << "  return_type<property> a;\n"
         << "  if (!enum_violation(\"cl_command_queue_info\", property)) {\n"
         << "    clGetCommandQueueInfo(queue, property, sizeof(a), &a, NULL);\n"
         << "  } else if (!enum_violation(\"cl_device_info\", property)) {\n\n"
         << "    cl_device_id d;\n"
         << "    clGetCommandQueueInfo(queue, CL_QUEUE_DEVICE, sizeof(d), &d, NULL);\n"
         << "    clGetDeviceInfo(d, property, sizeof(a), &a, NULL);\n"
         << "  } else {\n"
         << "    printf(\"Wrong query on queue!\\n\");\n"
         << "    exit(-1);\n"
         << "  }\n"
         << "  return a;\n"
         << "}\n\n";

    code << "template<cl_uint property>\n"
         << "auto query(cl_mem object)\n"
         << "{\n"
         << "  return_type<property> a;\n"
         << "  clGetMemObjectInfo(object, property, sizeof(a), &a, NULL);\n"
         << "  return a;\n"
         << "}\n\n";
}

void parse_commands(std::stringstream& code, xml_node<> *& root_node)
{
    ///////////////////////////////////////////////////////////////////////
    // commands
    ///////////////////////////////////////////////////////////////////////

    // Iterate over the commands
    for (xml_node<> * commands_node = root_node->first_node("commands");
        commands_node != nullptr;
        commands_node = commands_node->next_sibling("commands"))
    {
        for (xml_node<> * command_node = commands_node->first_node("command");
            command_node != nullptr;
            command_node = command_node->next_sibling("command"))
        {
            xml_node<> * proto_node = command_node->first_node("proto");
            std::string qual = (proto_node->value() != nullptr) ? std::regex_replace(proto_node->value(), std::regex("[ ]+"), " ") : "";
            const char * const name = proto_node->first_node("name")->value();
            std::string type = proto_node->first_node("type")->value();

            std::string invoke = name;
            invoke += "(\n";
            std::string proto = type + " " + qual + " " + name + "_layer(\n";
            proto = std::regex_replace(proto, std::regex("[ ]+"), " ");

//            if (param_node->value())
                //printf("I have visited %s\n", proto.c_str());
//            code << proto;

            int n = 0;
            func_params.clear();
            for (xml_node<> * param_node = command_node->first_node("param");
                param_node != nullptr;
                param_node = param_node->next_sibling("param"))
            {
                std::string tmp = (n != 0) ? ",\n  " : "  ";
                invoke += tmp + "  ";

                xml_node<> * node = param_node->first_node("type");
                if (node != nullptr)
                    func_params.insert(
                        std::pair<std::string, std::string>(
                            param_node->first_node("name")->value(),
                            param_node->first_node("type")->value()));

                // read all the contents as text omitting tags - works for 1-level tags only
                node = param_node->first_node();
                while (node != nullptr) {
                    if (tmp.back() != ' ')
                        tmp += " ";
                    tmp += std::regex_replace(node->value(), std::regex("[ ]+"), " ");
                    if (strstr(node->name(), "name") != nullptr)
                        invoke += node->value();
                    //printf("%s ", node->value());
                    node = node->next_sibling();
                }
                tmp = std::regex_replace(tmp, std::regex(" \\)"), ")");
                //printf("%s", tmp.c_str());

                proto += tmp;
                ++n;
            }
            proto += ")\n";
            invoke += ");\n";

            code << proto << "{\n";

            for (xml_node<> * violation_node = command_node->first_node("if"),
                            * result_node = command_node->first_node("then");
                (violation_node != nullptr) && (result_node != nullptr);
                violation_node = violation_node->next_sibling("if"),
                result_node = result_node->next_sibling("then"))
            {
                code << "  if " << parse_violation(violation_node->first_node()) << " {\n";

                for (xml_node<> * log_node = result_node->first_node("log");
                    log_node != nullptr;
                    log_node = log_node->next_sibling("log"))
                {
                    code << "    printf(\"" << name << ": \"\n"
                         << "      \"" << log_node->value() << "\\n\");\n\n";
                }

                std::string ret = "    return ";
                for (xml_node<> * name_node = result_node->first_node("name"),
                                * value_node = result_node->first_node("value");
                    (name_node != nullptr) && (value_node != nullptr);
                    name_node = name_node->next_sibling("name"),
                    value_node = value_node->next_sibling("value"))
                {
                    if (strcmp(name, name_node->value()) == 0)
                    {
                        ret += value_node->value();
                        ret += ";\n";
                    }
                    else
                    {
                        code << "    if (" << name_node->value() << " != NULL)\n"
                             << "      *" << name_node->value() << " = " << value_node->value() << ";\n";
                    }
                }
                //printf("%s %s", name, ret.c_str());
                code << ret << "  }\n\n";
            }

            code << "  return " << invoke << "}\n\n";
        }

    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path to cl.xml>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream theFile(argv[1]);
    if (!theFile) {
        std::cerr << "Error: failed to open '" << argv[1] << "'" << std::endl;
        return EXIT_FAILURE;
    }

    // stream for generated code
    std::stringstream code;
    code << "#include <CL/cl.h>\n"
         << "#include <string.h>\n"
         << "#include <stdio.h>\n"
         << "#include <algorithm>\n"
         << "#include \"func.h\"\n\n\n";

    xml_document<> doc;
    xml_node<> * root_node;
    // Read the xml file into a vector
    std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());\
    buffer.push_back('\0');
    // Parse the buffer using the xml file parsing library into doc
    doc.parse<0>(&buffer[0]);

    // Find our root node
    root_node = doc.first_node("registry");

    parse_enums(code, root_node);

    parse_bitfields(code, root_node);

    parse_literal_lists(code, root_node);

    parse_queries(code, root_node);

    // dummy funcs
    code << "bool from(const char * version)\n"
         << "{\n"
         << "  (void)version;\n"
         << "  return true;\n"
         << "}\n\n";

    code << "template<typename T>\n"
         << "bool array_len_ls(T * ptr, size_t size)\n"
         << "{\n"
         << "  (void)ptr;\n"
         << "  (void)size;\n"
         << "  return false;\n"
         << "}\n\n";

    // real funcs
    code << "template<typename T>\n"
         << "bool any_zero(T * ptr, size_t size)\n"
         << "{\n"
         << "  for (size_t i = 0; i < size; ++i)\n"
         << "    if (ptr[i] == 0) return true;\n"
         << "  return false;\n"
         << "}\n\n";

    code << "template<typename T>\n"
         << "bool any_nullptr(T ** ptr, size_t size)\n"
         << "{\n"
         << "  for (size_t i = 0; i < size; ++i)\n"
         << "    if (ptr[i] == NULL) return true;\n"
         << "  return false;\n"
         << "}\n\n";

    code << "template<typename T>\n"
         << "bool any_invalid(T * ptr, size_t size)\n"
         << "{\n"
         << "  for (size_t i = 0; i < size; ++i)\n"
         << "    if (!object_is_valid(ptr[i])) return true;\n"
         << "  return false;\n"
         << "}\n\n";

    code << "bool any_not_available(const cl_device_id * devices, size_t size)\n"
         << "{\n"
         << "  cl_bool avail = false;\n"
         << "  for (size_t i = 0; i < size; ++i) {\n"
         << "    clGetDeviceInfo(devices[i], CL_DEVICE_AVAILABLE, sizeof(cl_bool), &avail, NULL);\n"
         << "    if (!avail) return true;\n"
         << "    avail = false;\n"
         << "  }\n"
         << "  return false;\n"
         << "}\n\n";

    code << "//////////////////////////////////////////////////////////////////////\n\n";

    code << "#include \"object_is_valid.cpp\"\n"
         << "#include \"list_violation.cpp\"\n"
         << "#include \"struct_violation.cpp\"\n"
         << "\n\n";

    code << "//////////////////////////////////////////////////////////////////////\n\n";

    parse_commands(code, root_node);

    std::ofstream file("res.cpp");
    file << code.str();
    file.close();
    //std::cout << code.str();
    return 0;
}
