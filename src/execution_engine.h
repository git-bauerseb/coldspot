#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include "frame.h"

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

    return_ = 0xb1
};


class ExecutionEngine {
    public:
        ExecutionEngine() {}
        ~ExecutionEngine() {}

        u4 execute(Frame* frame);

    private:
};

#endif