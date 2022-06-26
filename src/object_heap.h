#ifndef OBJECT_HEAP_H
#define OBJECT_HEAP_H

#include <map>

#include "definitions.h"

struct Object {
    void* heap_ptr;
    u1 type;
};

union Variable {
    u1 char_value;
    u2 short_value;
    u4 int_value;
    f4 float_value;

    void* ptrValue;
    Object object;
};

class ObjectHeap {
    public:
        ObjectHeap();
        ~ObjectHeap();

    private:
        std::map<void*, void*> object_map;
};

#endif