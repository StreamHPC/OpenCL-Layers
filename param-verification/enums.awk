/<feature .*CL_VERSION_/ {
    if (match($0, / number=("[^\042]+") /, list) != 0) {
        print "  if (from(" list[1] ")) {"

        definitions = ""
        while (index($0, "</feature>") == 0) {
            # get more text
            if (getline <= 0) {
                print("unexpected EOF or error:", ERRNO) > "/dev/stderr"
                exit
            }
            # remove comments
            sub(/\/\/.+$/, "", $0)
            # build up the line using string concatenation
            definitions = definitions "\n" $0
        }

        enums_list = "cl_platform_info cl_device_info cl_context_info cl_command_queue_info cl_image_info cl_mem_info cl_addressing_mode cl_filter_mode cl_sampler_info cl_program_info cl_program_build_info cl_kernel_exec_info cl_kernel_info cl_kernel_work_group_info cl_kernel_sub_group_info cl_kernel_arg_info cl_event_info cl_profiling_info cl_channel_order cl_channel_type"
        split(enums_list, enums)

        # parse enum
        for (i in enums) {
            rest = definitions
            while (match(rest, "<require comment=\"[^\042]*" enums[i] "[^\042]*\"[^>]*>") != 0) {
                print "    if (strcmp(name, \"" enums[i] "\") == 0)"
                print "      switch (param) {"

                # cut enum values list - begin
                values = substr(rest, RSTART + RLENGTH)
                sub(/^[ \n]*/, "", values)
                # find position after current enum values list
                match(values, /[ \n]*<\/require>(.*)/, list)
                rest = list[1]
                # cut enum values list - end
                sub(/[ \n]*<\/require>.*/, "", values)

                # parse enum values
                while (match(values, /<enum name="([^\042]*)"[^>]*>/, list)) {
                    values = substr(values, RSTART + RLENGTH)
                    print "        case " list[1] ":"
                }
                #print values
                print "          return false;"
                print "      }"
            }
        }

        print "  }\n"
    }
    else
        print("unexpected feature description:", ERRNO) > "/dev/stderr"
}

BEGIN {
    print "template<typename T>"
    print "bool enum_violation(const char * name, T param)"
    print "{"
}

END {
    print "  return true;"
    print "}\n\n"
}
