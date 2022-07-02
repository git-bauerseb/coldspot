#ifndef OBJECT_HEAP_H
#define OBJECT_HEAP_H

#include <map>
#include <string.h>

#include "definitions.h"
#include "class_file.h"




class ObjectHeap {
    public:
        ObjectHeap();
        ~ObjectHeap();

        Object create_object(JavaClass* class_);
        void* derference_object(Object object);


    private:
        std::map<void*, void*> m_object_map;
        int m_object_id;
};

#endif