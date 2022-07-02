#ifndef CLASS_FILE_H
#define CLASS_FILE_H

#include <map>
#include <vector>
#include <algorithm>

#include "definitions.h"
#include "class_reader.h"

/*
    Access flags for fields/methods/classes
*/
enum ClassAccFlags {
    ACC_PUBLIC = 0x1,       // Declared public; May be accessed outside of package
    ACC_PRIVATE = 0x2,      // Declared private; Only accesible within class
    ACC_PROTECTED = 0x4,    // Declared protected; May be accessed within subclasses
    ACC_FINAL = 0x10,       // Declared final; No subclass allowed
    ACC_SUPER = 0x20,       // Superclass methods treated specially
    ACC_INTERFACE = 0x200,  // Denote interface, not a class
    ACC_ABSTRACT = 0x400,   // Declared abstract; Must not be instantiated
    ACC_SYNTHETIC = 0x1000, // Declared synthetic; Not present in source code
    ACC_ANOTATION = 0x2000, // Declared as annotation type
    ACC_ENUM = 0x4000,      // Declared as enum type
    ACC_MODULE = 0x8000     // Module, not a class/interface
};


/*
    CONSTANT POOL
*/
enum CP_Type {
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Utf8 = 1,
    CONSTANT_String = 8,

    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12
};



struct cp_info {
    u1 tag;
    u1* info;
};

struct CONSTANT_Integer_info {
    u1 tag;
    u4 bytes;
};

struct CONSTANT_Float_info {
    u1 tag;
    u4 bytes;
};

struct CONSTANT_Long_info {
    u1 tag;
    u4 high_bytes;
    u4 low_bytes;
};

struct CONSTANT_Double_info {
    u1 tag;
    u4 high_bytes;
    u4 low_bytes;
};

struct CONSTANT_Utf8_info {
    u1 tag;
    u2 length;
    u1* bytes;
};

struct CONSTANT_String_info {
    u1 tag;
    u2 string_index;
};

struct CONSTANT_Class_info {
    u1 tag;
    u2 name_index;
};

struct CONSTANT_Fieldref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_Methodref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_InterfaceMethodref_info {
    u1 tag;
    u2 class_index;
    u2 name_and_type_index;
};

struct CONSTANT_NameAndType_info {
    u1 tag;
    u2 name_index;
    u2 descriptor_index;
};

/*
    ATTRIBUTE
*/
struct attribute_info {
    // Index into the constant pool of the class, pointing to CONSTANT_Utf8_info structure
    u2 attribute_name_index;
    // Indicates the length of subsequent information in bytes
    u4 attribute_length;
    u1* info;
};

/*
    FIELD
*/
/*
    Represents a field in a class.
*/
struct field_info {
    // See enum FieldAccFlags
    u2 access_flags;
    // Index into constant_pool table that is a CONSTANT_Utf8_info structure
    u2 name_index;
    // Index into constant_pool table that is a CONSTANT_Utf8_info structure
    u2 descriptor_index;
    // Number of additional attributes of the field
    u2 attributes_count;
    attribute_info* attributes;
};

struct field_info_extended : public field_info {
    field_info* base;
};


/*
    METHOD
*/
/*
    Represents a method in a class
*/
struct exception_table {
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
};

struct code_attribute {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code;
    u2 expection_table_length;
    exception_table* exception;
    u2 attributes_count;
    attribute_info* attributes;
};

struct method_info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info* attributes;
};

struct method_info_extended : public method_info{
    method_info* base;
    code_attribute* code;
};


/*
    CLASS
*/
struct JavaClassFileFormat {
    // Magic number identifying class file format:
    // 0xCAFEBABE.
    u4  magic;

    u2 minor_version;
    u2 major_version;

    // Number of entries in the constant_pool table
    // plus one.
    u2 constant_pool_count;
    
    // constant_pool_count - 1 many constant pool elements
    std::vector<cp_info*> constant_pool;

    // Maksk of flags used to denote access permissions
    u2 access_flags;

    // Index to constant_pool table
    u2 this_class;

    // Either zero or index to constant_pool table
    u2 super_class;

    u2 interfaces_count;

    std::vector<u2> interfaces;

    u2 fields_count;

    std::vector<field_info_extended> fields;

    u2 methods_count;

    // Extension used because code needs to be present in the method
    std::vector<method_info_extended> methods;

    u2 attributes_count;

    attribute_info* attributes;
};


class JavaClass : public JavaClassFileFormat {

    public:
        JavaClass(std::vector<u1>& bytecode) : m_bytecode{bytecode} {}

        ~JavaClass() {
            for (int i = 0; i < methods_count; i++) {
                delete[] methods[i].code->code;
                delete methods[i].code;
            }

            for(auto& p : static_fields) {
                delete p.second;
            }
        }

        bool parse_class();
        int get_method_index(std::string& name, std::string& descriptor);

        /*
            Returns a (Utf8) string from the constant pool given the index.
        */
        bool string_from_constant_pool(int idx, std::string& val);


        void set_static_value(std::string field_name, Variable val, CP_Type type);
        Variable* get_static_value(std::string field_name);

    private:
        // Methods
        u4 read_u4(char* ptr);
        u2 read_u2(char* ptr);
        u1 read_u1(char* ptr);

        bool parse_constant_pool(char* &ptr);
        bool parse_interfaces(char* &ptr);
        bool parse_fields(char* &ptr);
        bool parse_methods(char* &ptr);
        bool parse_attributes(char* &ptr);

        bool parse_method_code_attribute(int i, code_attribute* attr);

        uint32_t get_constant_pool_elem_size(char* ptr);



        // Fields
        std::vector<u1> m_bytecode;

        std::map<std::string, Variable*> static_fields;

};

#endif