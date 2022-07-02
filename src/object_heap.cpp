#include "object_heap.h"

ObjectHeap::ObjectHeap() {
    m_object_id = 42;
}

ObjectHeap::~ObjectHeap() {}

Object ObjectHeap::create_object(JavaClass* class_) {
    Object object;
    object.heap_ptr = NULL;
    object.type = 0;

    u2 num_fields = class_->fields_count+1;
    Variable* vars = new Variable[num_fields];

    memset(vars, 0, num_fields * sizeof(Variable));

    // First field points to the class object
    vars[0].ptrValue = (void *)class_;

    object.heap_ptr = (void*)m_object_id;
    m_object_id += 1;

    m_object_map[(void*)object.heap_ptr] = vars;

    return object;
}

void* ObjectHeap::derference_object(Object object) {
    return m_object_map[object.heap_ptr];
}