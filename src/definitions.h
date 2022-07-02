#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <cstdint>

#define u4 std::uint32_t
#define u2 std::uint16_t
#define u1 std::uint8_t

#define i2 std::int16_t
#define i4 std::int32_t

#define f4 float

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

#endif