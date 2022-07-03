#ifndef CLASS_HEAP_H
#define CLASS_HEAP_H

#include <iostream>
#include <map>
#include <string>

#include "class_file.h"
#include "class_reader.h"

class ClassHeap {
    public:
        ClassHeap();
        ~ClassHeap();

        void add_class(std::string name);
        JavaClass* get_class(std::string name);

        JavaClass* get_class_with_method(std::string& name, std::string& descr);

    private:

        std::string strip_ending(std::string path);
        std::map<std::string, JavaClass*> class_map;
};

#endif