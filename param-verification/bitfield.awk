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

        bitfields_list = "cl_device_type cl_mem_flags cl_map_flags cl_mem_migration_flags cl_svm_mem_flags cl_device_affinity_domain"
        split(bitfields_list, bitfields)

        # parse bitfield
        for (i in bitfields) {
            rest = definitions
            while (match(rest, "<require comment=\"[^\042]*" bitfields[i] "[^\042]*\"[^>]*>") != 0) {
                print "    if (strcmp(name, \"" bitfields[i] "\") == 0) {"

                # cut bitfield values list - begin
                values = substr(rest, RSTART + RLENGTH)
                sub(/^[ \n]*/, "", values)
                # find position after current bitfield values list
                match(values, /[ \n]*<\/require>(.*)/, list)
                rest = list[1]
                # cut bitfield values list - end
                sub(/[ \n]*<\/require>.*/, "", values)

                # parse bitfield values
                while (match(values, /<enum name="([^\042]*)"[^>]*>/, list)) {
                    values = substr(values, RSTART + RLENGTH)
                    print "       mask |= " list[1] ";"
                    ++n
                }
                print "    }"
            }
        }

        print "  }\n"
    }
    else
        print("unexpected feature description:", ERRNO) > "/dev/stderr"
}

BEGIN {
    print "// function checks if there are set bits in the bitfield outside of defined"
    print "// 0 is then always valid param"
    print "template<typename T>"
    print "bool bitfield_violation(const char * name, T param)"
    print "{"
    print "  T mask = 0;"
}

END {
    print "  return (param & ~mask);"
    print "}\n\n"
}
