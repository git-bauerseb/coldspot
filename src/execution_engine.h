#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include "frame.h"
#include "class_heap.h"

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
    iadd = 0x60,
    isub = 0x64,

    bipush = 0x10,

    ireturn = 0xac,

    ldc = 0x12,

    invokestatic = 0xb8,

    return_ = 0xb1
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

        void execute_static_method(Frame* current, u2 idx);


        /*
            Gets an unsigned 16-bit integer from the buffer pointed to
            by ptr. Assumes that enough memory is available
        */
        u2 get_u2(char* ptr);
        u4 get_u4(char* ptr);


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
};

#endif