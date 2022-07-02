#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include "frame.h"
#include "class_heap.h"

#define IF_ICMP(comp) u4 second = frame->stack[frame->stack_ptr--].int_value; \
                      u4 first = frame->stack[frame->stack_ptr--].int_value; \
                      i2 addr = get_i2((char*)&byte_code[frame->program_ctr+1]); \
                       if (first comp second) { \
                            frame->program_ctr += addr; \
                        } else { \
                            frame->program_ctr += 3; \
                        } \


#define I_OP(op) int first = frame->pop_i(); \
                int second = frame->pop_i(); \
                frame->push_i(first op second); \
                frame->program_ctr++;

/*
    Bytecode
        In Use (2012): 202/256
        Implemented: 33/202
*/
enum Opcode {
    // Load constants
    nop = 0x0,
    iconst_0 = 0x3,
    iconst_1 = 0x4,
    iconst_2 = 0x5,
    iconst_3 = 0x6,
    iconst_4 = 0x7,
    iconst_5 = 0x8,

    istore_0 = 0x3b,
    istore_1 = 0x3c,
    istore_2 = 0x3d,
    istore_3 = 0x3e,

    istore = 0x36,

    iload_0 = 0x1a,
    iload_1 = 0x1b,
    iload_2 = 0x1c,
    iload_3 = 0x1d,

    imul = 0x68,
    idiv = 0x6c,
    iadd = 0x60,
    isub = 0x64,

    // Push byte value on stack
    bipush = 0x10,

    // Push short value on stack
    sipush = 0x11,

    ireturn = 0xac,

    ldc = 0x12,

    /*
        If first >= second, then add 
            (b1 << 8) | b2
        to pc.
    */
    if_icmpge = 0xa2,

    /*
        If first > second, then add
            (b1 << 8) | b2
        to pc.
    */
    if_icmpgt = 0xa3,

    if_icmple = 0xa4,
    if_icmplt = 0xa1,
    if_icmpne = 0xa0,

    /*
        Increment local variable at index b1 by (signed) b2 
    */
    iinc = 0x84,

    /*
        Goto another instruction given by
        b1 << 8 | b2
    */
    goto_ = 0xa7,

    invokestatic = 0xb8,
    invokespecial = 0xb7,

    putstatic = 0xb3,
    getstatic = 0xb2,

    return_ = 0xb1,


    new_ = 0xbb,

    /*
        Duplicate the topmost stack element
    */
    dup = 0x59,

    /*
        Push object reference from local variable 0 onto stack.
    */
    aload_0 = 0x2a,
    aload_1 = 0x2b,
    aload_2 = 0x2c,
    aload_3 = 0x2d,

    astore_0 = 0x4b,
    astore_1 = 0x4c,
    astore_2 = 0x4d,
    astore_3 = 0x4e,


    putfield = 0xb5,
    getfield = 0xb4
};


class ExecutionEngine {
    public:
        ExecutionEngine(ClassHeap* c_heap) {
            m_class_heap = c_heap;
        }
        ~ExecutionEngine() {}

        u4 execute(Frame* frame);

    private:

        /*
            Loads a constant from the constant_pool of the respective
            class file
        */
        Variable load_constant(JavaClass* class_, u1 index);

        /*
            Executes the invokestatic instruction which calls a static method.
        */
        void execute_invokestatic(Frame* current, u2 idx);

        /*
            Executes the invokespecial instruction which calls an instance method
            that is specified in the constant pool by idx of the specified object.
        */
        void execute_invokespecial(Frame* current, Object obj, u2 idx);
        
        void execute_putfield(Frame* frame, Object obj, Variable var, u2 field_idx);
        Variable execute_getfield(Frame* frame, Object obj, u2 field_idx);

        /*
            Creates a new object from the class pointed to in the constant pool
            and stores it in the object heap. Returns a reference for the newly
            created object.
        */
        Variable create_new_object(Frame* frame, u2 class_idx);

        void store_in_static_field(Variable var, Frame* frame, u2 idx);
        Variable get_from_static_field(Frame* frame, u2 idx);

        /*
            Returns the type of an element of the constant pool
            given its descriptor.
        */
        CP_Type get_constant_elem_type(std::string& descr);

        /*
            Gets an unsigned 16-bit integer from the buffer pointed to
            by ptr. Assumes that enough memory is available
        */
        u2 get_u2(char* ptr);
        u4 get_u4(char* ptr);

        i2 get_i2(char* ptr);


        /*
            Computes the number of parameters from the description of
            a method.
        */
        u2 get_method_parameter(std::string& description);

        /*
            True if the method that is called returns a value,
            false otherwise.
        */
        bool is_returning(std::string& description);


        // Heap object where the classes reside
        ClassHeap* m_class_heap;

        ObjectHeap m_object_heap;

        /*
            An object representing the current object.
            If the method that is currently executed is static,
            then this represents no valid object.
        */
        Object m_object;
};

#endif