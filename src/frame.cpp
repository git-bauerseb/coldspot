#include "frame.h"

void Frame::push_i(int value) {
    stack[stack_ptr].int_value = value;
    stack_ptr++;
}

void Frame::push_v(Variable var) {
    stack[stack_ptr] = var;
    stack_ptr++;
}

void Frame::push_s(short value) {
    stack[stack_ptr].short_value = value;
    stack_ptr++;
}

void Frame::push_ref(Object obj) {
    stack[stack_ptr].object = obj;
    stack_ptr++;
}


int Frame::pop_i() {
    stack_ptr--;
    int ret_value = stack[stack_ptr].int_value;
    return ret_value;
}

short Frame::pop_s() {
    stack_ptr--;
    short ret_value = stack[stack_ptr].short_value;
    return ret_value;
}

Object Frame::pop_ref() {
    stack_ptr--;
    Object obj = stack[stack_ptr].object;
    return obj;
}

Variable Frame::pop_v() {
    stack_ptr--;
    Variable v = stack[stack_ptr];
    return v;
}