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


static std::map<u1, std::string> opcode{};


bool JavaClass::parse_class(ClassReader& reader) {

    opcode.insert({0x2a, "aload_0"});

    magic = reader.read_u4();

    if (magic != 0xCAFEBABE) {
        std::cerr << "Invalid magic number\n";
        return false;
    }

    // Parse version
    minor_version = reader.read_u2();
    major_version = reader.read_u2();
    
    // Size of constant pool
    constant_pool_count = reader.read_u2();

    std::cout << "Size of constant pool " << constant_pool_count-1 << "\n";

    // Parse constant pool if available
    if (constant_pool_count > 0) {
        parse_constant_pool(reader);
    }

    access_flags = reader.read_u2();


    this_class = reader.read_u2();
    super_class = reader.read_u2();

    interfaces_count = reader.read_u2();


    // Parse interfaces if available
    if (interfaces_count > 0) {
        parse_interfaces(reader);
    }

    fields_count = reader.read_u2();

    if (fields_count > 0) {
        parse_fields(reader);
    }

    methods_count = reader.read_u2();


    if (methods_count > 0) {
        parse_methods(reader);
    }

    attributes_count = reader.read_u2();

    if (attributes_count > 0) {
        parse_attributes(reader);
    }

    return true;
}

bool JavaClass::parse_constant_pool(ClassReader& reader) {

    // Reserve memory for constant_pool_count-1 many elements in pool
    constant_pool = new cp_info*[constant_pool_count-1];

    if (constant_pool == NULL) {
        std::cerr << "Could not reserve memory for constant pool\n";
        return false;
    }

    char* p = reader.get_handle();

    for (int i = 1; i < constant_pool_count; i++) {
        constant_pool[i] = (cp_info*)p;

        // std::cout << "CP " << CP_Name[constant_pool[i]->tag] << "\n";

        uint32_t size = get_constant_pool_elem_size(p);
        p += size;
    }

    // IMPORTANT: set handle to read in information after constant pool.
    reader.set_handle(p);

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

bool JavaClass::parse_interfaces(ClassReader& reader) {
    interfaces = new u2[interfaces_count];

    for (int i = 0; i < interfaces_count; i++) {
        interfaces[i] = reader.read_u2();
    }

    return true;
}

bool JavaClass::parse_fields(ClassReader& reader) {
    fields = new field_info[fields_count];


    for (int i = 0; i < fields_count; i++) {
        char* ptr = reader.get_handle();
        fields[i] = *(field_info*)(ptr);
        fields[i].access_flags = reader.read_u2();
        fields[i].name_index = reader.read_u2();
        fields[i].descriptor_index = reader.read_u2();
        fields[i].attributes_count = reader.read_u2();


        if (fields[i].attributes_count > 0) {
            for (int j = 0; j < fields[j].attributes_count; j++) {
                
                
                u2 name_index = reader.read_u2();
                u4 length = reader.read_u4();

                reader.forward(length);
            }
        }
    }

    return true;
}

bool JavaClass::parse_methods(ClassReader& reader) {

    methods = new method_info_extended[methods_count];

    for (int i = 0; i < methods_count; i++) {
        char* ptr = reader.get_handle();
        methods[i].base = new method_info();
        methods[i].base = (method_info*)(ptr);
        methods[i].base->access_flags = reader.read_u2();
        methods[i].base->name_index = reader.read_u2();
        methods[i].base->descriptor_index = reader.read_u2();
        methods[i].base->attributes_count = reader.read_u2();

        std::string s;
        string_from_constant_pool(methods[i].base->name_index, s);

        std::cout << "Method name " << s << "\n";

        if (methods[i].base->attributes_count > 0) {

            // Skip attributes
            char* ptr = reader.get_handle();
            for (int j = 0; j < methods[i].base->attributes_count; j++) {
                u2 name_index = (((u2)ptr[0]) << 8 ) | ((u2)(ptr[1]));
                ptr += 2;
                u4 length = (((u4)ptr[0]) << 24) | (((u4)(ptr[1])) << 16) | (((u4)(ptr[2])) << 8) | (((u4)(ptr[3])));
                ptr += 4;
                ptr += length;
            }

            methods[i].code = new code_attribute();
            // Parse code attributes
            parse_method_code_attribute(reader, methods[i].base->attributes_count, i, methods[i].code);

            reader.set_handle(ptr);
        }
    }

    return true;
}


bool JavaClass::parse_attributes(ClassReader& reader) {
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


bool JavaClass::parse_method_code_attribute(ClassReader& reader, int n_attr, int i, code_attribute* attr) {
    if (i > methods_count) {
        return false;
    }


    if (n_attr > 0) {
        // Skip attributes
        for (int i = 0; i < n_attr; i++) {
            u2 attribute_name_index = reader.read_u2();

            std::string name;
            string_from_constant_pool(attribute_name_index, name);

            // name.compare("Code") = 0 iff name == "Code"
            if (!name.compare("Code")) {
                attr->attribute_name_index = attribute_name_index;
                attr->attribute_length = reader.read_u4();
                attr->max_stack = reader.read_u2();
                attr->max_locals = reader.read_u2();
                int len = attr->code_length = reader.read_u4();

                // Parse code
                if (methods[i].code->code_length > 0) {
                    attr->code = new u1[attr->code_length];

                    for (int j = 0; j < len; j++) {
                        attr->code[j] = reader.read_u1();
                    } 
                } else {
                    attr->code = NULL;
                }
            }
        }
    }
}

int JavaClass::get_method_index(std::string name, std::string descriptor) {
    for (int i = 0; i < methods_count; i++) {
        method_info_extended m = methods[i];
        u2 name_index = m.base->name_index;
        u2 descr_index = m.base->descriptor_index;

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