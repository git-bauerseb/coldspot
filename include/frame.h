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

        /*
            Stack pointer to the current frame stack.
            Points to the next position in the stack, that is
            stack[stack_ptr] == FREE
        */
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

        /* Push (arbitrary) variable onto stack. */
        void push_v(Variable var);
        Variable pop_v();

        /* Push integer onto stack. */
        void push_i(int value);
        /* Pop integer value from stack */
        int pop_i();

        /* Push short value onto stack. */
        void push_s(short value);
        short pop_s();

        /* Push reference to object onto stack. */
        void push_ref(Object obj);
        Object pop_ref();
};

#endif