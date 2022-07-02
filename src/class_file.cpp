#include "class_file.h"

static const char* CP_Name[] = {
    "",
    "Utf8",
    "",
    "Int",
    "Float",
    "Long",
    "Double",
    "Class",
    "String",
    "Fieldref",
    "Methodref",
    "InterfaceMethodref",
    "NameAndType"};



u4 JavaClass::read_u4(char* ptr) {
    return (((u4)(((u1)ptr[0])) << 24) & 0xFF000000)
        | (((u4)(((u1)ptr[1])) << 16) & 0x00FF0000)
        | (((u4)(((u1)ptr[2])) << 8) & 0x0000FF00)
        | (((u4)(((u1)ptr[3]))) & 0x000000FF);
}

u2 JavaClass::read_u2(char* ptr) {
    return (((u2)(((u1)ptr[0])) << 8) & 0xFF00)
        | ((u2)(((u1)ptr[1])));
}

u1 JavaClass::read_u1(char* ptr) {
    return (u1)ptr[0];
}


bool JavaClass::parse_class() {

    char* p = (char *)(&m_bytecode[0]);

    magic = read_u4(p);
    p += 4;

    if (magic != 0xCAFEBABE) {
        std::cerr << "Invalid magic number\n";
        return false;
    }

    // Parse version
    minor_version = read_u2(p);
    p += 2;

    major_version = read_u2(p);
    p += 2;

    // Size of constant pool
    constant_pool_count = read_u2(p);
    p += 2;

    std::cout << "Size of constant pool " << constant_pool_count-1 << "\n";

    // Parse constant pool if available
    if (constant_pool_count > 0) {
        parse_constant_pool(p);
    }

    access_flags = read_u2(p);
    p += 2;

    this_class = read_u2(p);
    p += 2;

    super_class = read_u2(p);
    p += 2;

    interfaces_count = read_u2(p);
    p += 2;

    // Parse interfaces if available
    if (interfaces_count > 0) {
        parse_interfaces(p);
    }

    fields_count = read_u2(p);
    p += 2;

    if (fields_count > 0) {
        parse_fields(p);
    }

    methods_count = read_u2(p);
    p += 2;


    if (methods_count > 0) {
        parse_methods(p);
    }

    attributes_count = read_u2(p);
    p += 2;

    if (attributes_count > 0) {
        parse_attributes(p);
    }

    return true;
}

bool JavaClass::parse_constant_pool(char*& p) {

    // Reserve memory for constant_pool_count-1 many elements in pool
    constant_pool.resize(constant_pool_count);

    if (&constant_pool[0] == NULL) {
        std::cerr << "Could not reserve memory for constant pool\n";
        return false;
    }

    for (int i = 1; i < constant_pool_count; i++) {
        constant_pool[i] = (cp_info*)p;

        switch (constant_pool[i]->tag) {
            case CP_Type::CONSTANT_Methodref: {
                CONSTANT_Methodref_info * ref = (CONSTANT_Methodref_info*)constant_pool[i];
                std::cout << "Methodref Class idx: " << ref->class_index << ". NameAndType: " << (ref->name_and_type_index & 0xFFFF) << "\n"; 

            }
            break;

            default:
                break;
        }

        int size = get_constant_pool_elem_size(p);
        p += size;
    }

    return true;
}

uint32_t JavaClass::get_constant_pool_elem_size(char* ptr) {
    cp_info *cp = (cp_info*)ptr;
    u2 length;
    u1 a;
    u1 b;
    char* op;
    
    switch (cp->tag) {
        case CP_Type::CONSTANT_Class:
            return 3;
        case CP_Type::CONSTANT_Fieldref:
            return 5;
        case CP_Type::CONSTANT_Methodref:
            return 5;
        case CP_Type::CONSTANT_InterfaceMethodref:
            return 5;
        case CP_Type::CONSTANT_NameAndType:
            return 5;
        case CP_Type::CONSTANT_Integer:
            return 5;
        case CP_Type::CONSTANT_Float:
            return 5;
        case CP_Type::CONSTANT_Long:
            return 9;
        case CP_Type::CONSTANT_Double:
            return 9;
        case CP_Type::CONSTANT_String:
            return 3;
        case CP_Type::CONSTANT_Utf8:
            op = ptr + 1;
            a = op[0];
            b = op[1];
            length = (u2)(a << 8 & 0x0000FF00 | b);
            return 3 + length;
        default:
            std::cerr << "Unknown type of constant_pool element\n";
            return 0;
    }
}

bool JavaClass::parse_interfaces(char*& p) {
    interfaces.reserve(interfaces_count);

    for (int i = 0; i < interfaces_count; i++) {
        interfaces[i] = read_u2(p);
    }

    return true;
}

bool JavaClass::parse_fields(char*& p) {

    fields.reserve(fields_count);


    for (int i = 0; i < fields_count; i++) {
        fields[i].base = (field_info*)p;

        fields[i].access_flags = read_u2(p);
        p += 2;

        fields[i].name_index = read_u2(p);
        p += 2;
        
        fields[i].descriptor_index = read_u2(p);
        p += 2;
        
        fields[i].attributes_count = read_u2(p);
        p += 2;

        if (fields[i].attributes_count > 0) {
            for (int j = 0; j < fields[j].attributes_count; j++) {
                
                
                u2 name_index = read_u2(p);
                p += 2;

                std::cout << "Field attribute index " << name_index << "\n";

                u4 length = read_u4(p);
                p += 4;

                p += length;
            }
        }
    }

    return true;
}

bool JavaClass::parse_methods(char*& p) {

    methods.reserve(methods_count);

    for (int i = 0; i < methods_count; i++) {
        methods[i].base = (method_info*)(p);

        methods[i].access_flags = read_u2(p);
        p += 2;

        methods[i].name_index = read_u2(p);
        p += 2;

        methods[i].descriptor_index = read_u2(p);
        p += 2;

        methods[i].attributes_count = read_u2(p);
        p += 2;

        std::string s;
        string_from_constant_pool(methods[i].name_index, s);

        std::cout << "Method name " << s << "\n";

        if (methods[i].attributes_count > 0) {

            // Skip attributes
            for (int j = 0; j < methods[i].attributes_count; j++) {
                u2 name_index = read_u2(p);
                p += 2;
                u4 length = read_u4(p);
                p += 4;
                p += length;
            }

            methods[i].code = new code_attribute();
            // Parse code attributes
            parse_method_code_attribute(i, methods[i].code);
        }
    }

    return true;
}

bool JavaClass::parse_method_code_attribute(int i, code_attribute* attr) {
    if (i > methods_count) {
        return false;
    }

    char* p = (char*)(methods[i].base);
    p += 6;
    u4 n_attr = read_u2(p);
    p += 2;


    if (n_attr > 0) {
        // Skip attributes
        for (int i = 0; i < n_attr; i++) {
            u2 attribute_name_index = read_u2(p);
            p += 2;

            std::string name;
            string_from_constant_pool(attribute_name_index, name);

            // name.compare("Code") = 0 iff name == "Code"
            if (!name.compare("Code")) {
                attr->attribute_name_index = attribute_name_index;
                attr->attribute_length = read_u4(p);
                p += 4;
                attr->max_stack = read_u2(p);
                p += 2;
                attr->max_locals = read_u2(p);
                p += 2;
                u4 len = attr->code_length = read_u4(p);
                p += 4;
                // Parse code
                if (methods[i].code->code_length > 0) {
                    attr->code = new u1[len];

                    for (int j = 0; j < len; j++) {
                        attr->code[j] = read_u1(p);
                        p += 1;
                    } 
                } else {
                    attr->code = NULL;
                }
            }
        }
    }

    return true;
}


bool JavaClass::parse_attributes(char* &ptr) {
    return true;
}

bool JavaClass::string_from_constant_pool(int idx, std::string& val) {
    if (idx < 1 || idx >= constant_pool_count) {
        return false;
    }

    if (constant_pool[idx]->tag != CP_Type::CONSTANT_Utf8) {
        std::cerr << "Cannot get string from constant pool because invalid type\n";
        return false;
    }

    u1* p = (u1 *)constant_pool[idx];

    u2 length = (u2)(((u2)p[1]) << 8 | (u2)p[2]);
    val.append((char*)&p[3], length);
    return true;
}




int JavaClass::get_method_index(std::string& name, std::string& descriptor) {
    for (int i = 0; i < methods_count; i++) {
        method_info_extended m = methods[i];
        u2 name_index = m.name_index;
        u2 descr_index = m.descriptor_index;

        std::string n;
        std::string d;

        string_from_constant_pool(name_index, n);
        string_from_constant_pool(descr_index, d);
        
        if (!n.compare(name) && !d.compare(descriptor)) {
            return i;
        }
    }

    return -1;
}


void JavaClass::set_static_value(std::string field_name, Variable val, CP_Type type) {
    if (static_fields[field_name] == nullptr) {
        static_fields[field_name] = new Variable();
    }

    switch (type) {
        case CP_Type::CONSTANT_Integer:
            static_fields[field_name]->int_value = val.int_value;
            break;
        default:
            break;
    }
}

Variable* JavaClass::get_static_value(std::string field_name) {

    if (static_fields[field_name] == nullptr) {
        static_fields[field_name] = new Variable();
        static_fields[field_name]->int_value = 0;
    }

    return static_fields[field_name];
}