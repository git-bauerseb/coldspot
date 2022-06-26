#ifndef FRAME_H
#define FRAME_H

#include "object_heap.h"
#include "class_file.h"

class Frame {
    public:
        // Stack of the current method that holds
        // all the temporary values
        Variable* stack;

        // Method to be executed
        method_info_extended* method;

        JavaClass* class_;

        // Stack pointer in the stack
        i2 stack_ptr;

        // Program counter
        u4 program_ctr;

        Frame() {
            stack_ptr = -1;
            program_ctr = 0;
            class_ = NULL;
            stack = NULL;
            method = NULL;
        }
};

#endif