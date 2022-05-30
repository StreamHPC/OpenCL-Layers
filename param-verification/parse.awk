function after(string, pattern, tmp) {
    tmp = index(string, pattern);
    return (tmp != 0) ? (tmp + length(pattern)) : 0
}

function short_tag(string, tag, prop_list, start, end, tmp, n) {
    delete prop_list

    start = after(string, "<" tag " ")
    end = index(string, "/>")
    if ((start != 0) && (end != 0)) {
        string = substr(string, start, end - start)
        sub(/^[ ]+/, "", string)
        sub(/[ ]+$/, "", string)

        n = 0
        while (match(string, /[ ]*([^= ]+)[ ]*=[ ]*"([^\042]+)[ ]*"/, tmp) != 0) {
            prop_list[tmp[1]] = tmp[2]
            string = substr(string, RSTART + RLENGTH)
            ++n
        }
    }
    else {
        print "bad shor tag: " string
        exit 1
    }

    return n
}

function full_tag(string, tag, start, end, n, m, pos, i, j, tmp, open_pos, close_pos) {
    n = 0
    tmp = string
    pos = 0
    delete open_pos
    while ((start = after(tmp, "<" tag ">")) != 0) {
        pos += start
        open_pos[n] = pos
        ++n
        tmp = substr(tmp, start)
    }

    m = 0
    tmp = string
    pos = 0
    delete close_pos
    while ((end = after(tmp, "</" tag ">")) != 0) {
        pos += end
        close_pos[m] = pos
        ++m
        tmp = substr(tmp, end)
    }

    if (n != m) {
        print "non-pair tags in:\n" string
        exit 1
    }

    open_pos[m] = length(string) + 10
    close_pos[m] = length(string) + 10

    n = 0
    i = 0
    j = 0
    end = 0
    do {
        if (open_pos[i] < close_pos[j]) {
            if (n == 0)
                start = open_pos[i]
            ++n
            ++i
        } 
        else if (open_pos[i] > close_pos[j]) {
            --n
            if (n == 0)
                end = end + close_pos[j] - length("</" tag ">")
            else # some magic - somehow it moves border by +1 for each nested tag - this compensate the pb
                --end
            ++j
        } 
    } while ((n > 0) && ((i < m) || (j < m)))

    if (n != 0) {
        print "bad tags in:\n" string
        exit 1
    }

    tmp = substr(string, start, end - start)
    return tmp
}

function parse_expression(string, tmp, list, temp_str, end) {
    if (index(string[0], "<name>") == 1) {
        tmp = full_tag(string[0], "name")
        sub(/^[ \n]+/, "", tmp)
        sub(/[ \n]+$/, "", tmp)
        end = after(string[0], "</name>")

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
        
        return tmp
    }
    if (index(string[0], "<name field=") == 1) {
        match(string[0], /<name field="([^\042]+)">([^<]+)<\/name>/, temp_str)
        sub(/^[ \n]+/, "", temp_str[1])
        sub(/[ \n]+$/, "", temp_str[1])
        sub(/^[ \n]+/, "", temp_str[2])
        sub(/[ \n]+$/, "", temp_str[2])

        string[0] = substr(string[0], RLENGTH)
        sub(/^[^<]+/, "", string[0])
        
        return temp_str[2] "->" temp_str[1]
    }
    if (index(string[0], "<name elem=") == 1) {
        print string
        match(string[0], /<name elem="([^\042]+)">([^<]+)<\/name>/, temp_str)
        sub(/^[ \n]+/, "", temp_str[1])
        sub(/[ \n]+$/, "", temp_str[1])
        sub(/^[ \n]+/, "", temp_str[2])
        sub(/[ \n]+$/, "", temp_str[2])

        string[0] = substr(string[0], RLENGTH)
        sub(/^[^<]+/, "", string[0])
        
        return temp_str[2] "[" temp_str[1] "]"
    }
    if (index(string[0], "<literal>") == 1) {
        tmp = full_tag(string[0], "literal")
        sub(/^[ \n]+/, "", tmp)
        sub(/[ \n]+$/, "", tmp)
        end = after(string[0], "</literal>")

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
        
        return tmp
    }
    if (index(string[0], "<literal_list>") == 1) {
        tmp = full_tag(string[0], "literal_list")
        sub(/^[ \n]+/, "", tmp)
        sub(/[ \n]+$/, "", tmp)
        end = after(string[0], "</literal_list>")

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
        
        return "literal_list(\042" params[tmp] "\042, " tmp ")"
    }
    if (index(string[0], "<sizeof>") == 1) {
        tmp = full_tag(string[0], "sizeof")
        sub(/^[ \n]+/, "", tmp)
        sub(/[ \n]+$/, "", tmp)
        end = after(string[0], "</sizeof>")

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
        
        return "sizeof(" tmp ")"
    }
    if (index(string[0], "<mod>") == 1) {
        temp_str[0] = full_tag(string[0], "mod")
        end = after(string[0], temp_str[0]) + length("</mod>")
        sub(/^[ \n]+/, "", temp_str[0])
        sub(/[ \n]+$/, "", temp_str[0])

        delete list
        parse_2expressions(temp_str, list)

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
        
        return "(" list[0] ") % (" list[1] ")"
    }
    if (index(string[0], "<mult>") == 1) {
        temp_str[0] = full_tag(string[0], "mult")
        end = after(string[0], temp_str[0]) + length("</mult>")
        sub(/^[ \n]+/, "", temp_str[0])
        sub(/[ \n]+$/, "", temp_str[0])

        delete list
        parse_2expressions(temp_str, list)

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
        
        return "(" list[0] ") * (" list[1] ")"
    }

    tmp = string[0]
    string[0] = ""
    print tmp
    exit 1
    return tmp
}

function parse_2expressions(string, list) {
    sub(/^[^<]*/, "", string[0])

    delete list
    list[0] = parse_expression(string)
    list[1] = parse_expression(string)

    sub(/^[^<]*/, "", string[0])
    if (length(string[0]) != 0) {
        print "bad expression"
        exit 1
    }
}

# sending string by reference as an array element
function parse_list(string, list, n) {
    sub(/^[^<]*/, "", string[0])

    n = 0
    while (length(string[0]) != 0) {
        list[n] = parse_violation(string)
        ++n
    }
}

# sending string by reference as an array element
function parse_violation(string, violation, deeper, a, n, end, list, tmp, tmp_list) {
    sub(/^[^<]*/, "", string[0])

    if (length(string[0]) != 0) {
        if (index(string[0], "<or>") == 1) {
            deeper[0] = full_tag(string[0], "or")
            end = after(string[0], deeper[0]) + length("</or>")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])

            delete list
            parse_list(deeper, list)

            n = 0
            for (a in list) {
                tmp = (n == 0) ? "(" : " ||\n    "
                violation = violation tmp list[a]
                ++n
            }
            violation = violation ")"
        }
        else if (index(string[0], "<and>") == 1) {
            deeper[0] = full_tag(string[0], "and")
            end = after(string[0], deeper[0]) + length("</and>")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])

            delete list
            parse_list(deeper, list)

            n = 0
            for (a in list) {
                tmp = (n == 0) ? "(" : " &&\n    "
                violation = violation tmp list[a]
                ++n
            }
            violation = violation ")"
        }
        else if (index(string[0], "<mutex_violation>") == 1) {
            deeper[0] = full_tag(string[0], "mutex_violation")
            end = after(string[0], deeper[0]) + length("</mutex_violation>")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])

            delete list
            parse_list(deeper, list)

            n = 0
            for (a in list) {
                tmp = (n == 0) ? "((bool)" : " +\n    (bool)"
                violation = violation tmp list[a]
                ++n
            }
            violation = violation " > 1)"
        }
        else if (index(string[0], "<not>") == 1) {
            tmp = full_tag(string[0], "not")
            sub(/^[ \n]+/, "", tmp)
            sub(/[ \n]+$/, "", tmp)
            end = after(string[0], "</not>")

            delete list
            list[0] = tmp
            tmp = parse_violation(list)

            violation = violation "(!(" tmp "))"
        }
        else if (index(string[0], "<eq>") == 1) {
            deeper[0] = full_tag(string[0], "eq")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])
            end = after(string[0], "</eq>")

            delete list
            parse_2expressions(deeper, list)

            violation = violation "(" list[0] " == " list[1] ")"
        }
        else if (index(string[0], "<neq>") == 1) {
            deeper[0] = full_tag(string[0], "neq")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])
            end = after(string[0], "</neq>")

            delete list
            parse_2expressions(deeper, list)

            violation = violation "(" list[0] " != " list[1] ")"
        }
        else if (index(string[0], "<ls>") == 1) {
            deeper[0] = full_tag(string[0], "ls")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])
            end = after(string[0], "</ls>")

            delete list
            parse_2expressions(deeper, list)

            violation = violation "(" list[0] " < " list[1] ")"
        }
        else if (index(string[0], "<gt>") == 1) {
            deeper[0] = full_tag(string[0], "gt")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])
            end = after(string[0], "</gt>")

            delete list
            parse_2expressions(deeper, list)

            violation = violation "(" list[0] " > " list[1] ")"
        }
        else if (index(string[0], "<bit_and>") == 1) {
            deeper[0] = full_tag(string[0], "bit_and")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])
            end = after(string[0], "</bit_and>")

            delete list
            parse_2expressions(deeper, list)

            violation = violation "(" list[0] " & " list[1] ")"
        }
        else if (index(string[0], "<array_len_ls>") == 1) {
            deeper[0] = full_tag(string[0], "array_len_ls")
            sub(/^[ \n]+/, "", deeper[0])
            sub(/[ \n]+$/, "", deeper[0])
            end = after(string[0], "</array_len_ls>")

            delete list
            parse_2expressions(deeper, list)

            violation = violation "(array_len_ls(" list[0] ", " list[1] "))"
        }
        else if (index(string[0], "<enum_violation") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "enum_violation", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(enum_violation(\042" params[tmp_list["name"]] "\042, " tmp_list["name"] "))"
        }
        else if (index(string[0], "<list_violation") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "list_violation", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(list_violation(" tmp_list["name"] "))"
        }
        else if (index(string[0], "<bitfield_violation") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "bitfield_violation", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(bitfield_violation(\042" params[tmp_list["name"]] "\042, " tmp_list["name"] "))"
        }
        else if (index(string[0], "<struct_violation") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "struct_violation", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(struct_violation(" tmp_list["name"] "))"
        }
        else if (index(string[0], "<any_nullptr") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "any_nullptr", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(any_nullptr(" tmp_list["array"] ", " tmp_list["elements"] "))"
        }
        else if (index(string[0], "<any_zero") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "any_zero", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(any_zero(" tmp_list["array"] ", " tmp_list["elements"] "))"
        }
        else if (index(string[0], "<from") == 1) {
            delete tmp_list
            tmp = short_tag(string[0], "from", tmp_list)
            end = after(string[0], "/>")

            violation = violation "(from(\"" tmp_list["version"] "\"))"
        }
        else {
            end = length(string[0])
            violation = violation "( " string[0] " )"
            print "The great unknown " string[0]
            exit 1
        }

        string[0] = substr(string[0], end)
        sub(/^[^<]+/, "", string[0])
    }

    return violation
}

/<command/ {
    # Concatenate description of command
    start = index($0, "<command")
    full_command = substr($0, start + length("<command"))
    while ((end = index(full_command, "</command>")) == 0) {  # is end in trailing part?
        # get more text
        if (getline <= 0) {
            print("unexpected EOF or error:", ERRNO) > "/dev/stderr"
            exit
        }
        # remove comments
        sub(/\/\/.+$/, "", $0)
        # build up the line using string concatenation
        full_command = full_command " " $0
    }
    # command is here
    $0 = substr(full_command, 1, end - 1)
    gsub(/ [ ]+/, " ", $0)

    # get function prototype
    start = after($0, "<proto>")
    end = index($0, "</proto>")
    proto = substr($0, start, end - start)
    # cut it from command
    $0 = substr($0, end + 8)

    command_name = full_tag(proto, "name")
    invoke = command_name "(\n"

    sub(/<type>/, "", proto)
    sub(/<\/type>/, " ", proto)
    sub(/[ ]+<name>/, " ", proto)
    sub(/<\/name>/, "_layer(\n", proto)
    while (index(proto, "  ") != 0)
        sub(/[ ]+/, " ", proto)
    # start to construct layer function
    full_function = proto

    # parse function parameters
    n = 0
    while (index($0, "<param>") != 0) {
        # copy parameter to param
        param = full_tag($0, "param")

        param_type = full_tag(param, "type")
        param_name = full_tag(param, "name")

        # needed for literal_list
        params[param_name] = param_type

        # clear parameter from tags and spaces
        sub(/<type>/, " ", param)
        sub(/<\/type>/, " ", param)
        sub(/[ ]+<name>/, " ", param)
        sub(/<\/name>/, " ", param)
        sub(/^[ ]+/, "", param)
        sub(/ [ ]+/, " ", param)

        # delete parameter from text
        #start = after($0, "<param>")
        end = after($0, "</param>")
        $0 = substr($0, end)

        # add parameter to function definition
        tmp = (n != 0) ? ",\n    " : "    "
        full_function = full_function tmp param
        invoke = invoke tmp param_name
        ++n
    }
    invoke = invoke ");\n"
    full_function = full_function ")\n{\n"

    # parse parameters violations
    # the order is important sometimes
    n = 0
    while (index($0, "<if>") != 0) {
        violation = full_tag($0, "if")
        result = full_tag($0, "then")

        # parse single violation
        violations[0] = violation
        full_function = full_function "  if " parse_violation(violations) "\n  {\n"

        # parse result of violation
        ret = ""
        while (index(result, "<name>") != 0) {
            result_name = full_tag(result, "name")
            result_value = full_tag(result, "value")

            if (result_name == command_name) {
                ret = ret "    return " result_value ";\n"
            }
            else {
                full_function = full_function "    if (" result_name " != NULL) *" result_name " = " result_value ";\n"
            }

            end = after(result, "</value>")
            result = substr(result, end)
        }
        full_function = full_function ret "  }\n\n"

        # delete parsed violation
        end = after($0, "</then>")
        $0 = substr($0, end)
    }

    print full_function "  return " invoke "}\n\n"
}

#/\n/ {next}
#/<command/,/<\/command>/ {++n}
BEGIN {
    print "#include <cl.h>\n\n"
    #include <bool.h>
}

END {
# dummy
    print "template<typename T>"
    print "bool array_len_ls(T * ptr, size_t size)"
    print "{"
    print "  return false;"
    print "}\n\n"
# enum_violation is in enums.awk
#    print "template<typename T>"
#    print "bool enum_violation(const char * name, T param)"
#    print "{"
#    print "  return false;"
#    print "}\n\n"
# list_violation is in list_violation.cpp
#    print "template<typename T>"
#    print "bool list_violation(T param)"
#    print "{"
#    print "  return false;"
#    print "}\n\n"
# dummy
    print "template<typename T>"
    print "bool struct_violation(T param)"
    print "{"
    print "  return false;"
    print "}\n\n"
# bitfield_violation is in bitfield.awk
#    print "template<typename T>"
#    print "bool bitfield_violation(T param)"
#    print "{"
#    print "  if (param == 0) return true;"
#    print "  return false;"
#    print "}\n\n"
# literal_list is in lists.awk
#    print "template<typename T>"
#    print "size_t literal_list(T param)"
#    print "{"
#    print "  return 0;"
#    print "}\n\n"
# dummy
    print "bool from(char * version)"
    print "{"
    print "  return true;"
    print "}\n\n"

    print "template<typename T>"
    print "bool any_zero(T * ptr, size_t size)"
    print "{"
    print "  for (size_t i = 0; i < size; ++i)"
    print "    if (ptr[i] == 0) return true;"
    print "  return false;"
    print "}\n\n"

    print "template<typename T>"
    print "bool any_nullptr(T ** ptr, size_t size)"
    print "{"
    print "  for (size_t i = 0; i < size; ++i)"
    print "    if (ptr[i] == NULL) return true;"
    print "  return false;"
    print "}\n\n"
}