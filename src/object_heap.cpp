#include "../include/object_heap.h"

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

Object ObjectHeap::create_primitive_array(int size, u1 type) {
    /*
        First two entries are reserved.

        arr[0]: type of array elements
        arr[1]: length of array
    */
    Variable* arr = new Variable[size+2];
    Object obj;
    obj.heap_ptr = 0;
    obj.type = type;

    // Initialize array to zero
    memset(arr, 0, sizeof(Variable)*(size+1));

    obj.heap_ptr = (void*)m_object_id;
    m_object_id += 1;

    arr[0].int_value = type;
    arr[1].int_value = size;
    m_object_map[(void*)obj.heap_ptr] = arr;

    return obj;
}

void ObjectHeap::debug_print_primitive_array(Object obj) {
    Variable* elements = (Variable*)derference_object(obj);

    std::cout << "Debug::Array\n";

    switch (elements[0].int_value) {
        std::cout << "\tType: ";
        case CP_Type::CONSTANT_Integer: {
                std::cout << "INT\n";
            }
            break;
        default:
            std::cout << "ERR Type\n";
            break;
    }

    std::cout << "\tSize " << elements[1].int_value << "\n";
    std::cout << "\tElem ";

    for (int i = 0; i < elements[1].int_value; i++) {
        std::cout << elements[2+i].int_value << " ";
    }

    std::cout << "\n";
}