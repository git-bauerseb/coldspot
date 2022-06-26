#include "class_heap.h"

ClassHeap::ClassHeap() {}
ClassHeap::~ClassHeap() {
    for (auto& elem : class_map) {
        delete elem.second;
    }
}

void ClassHeap::add_class(std::string path) {
    ClassReader reader{path};

    JavaClass* class_ = new JavaClass();
    bool c_parse = class_->parse_class(reader); 

    if (!c_parse) {
        std::cerr << "Could not parse class file " << path << "\n";
        return;
    }

    class_map.insert({path, class_});
}

JavaClass* ClassHeap::get_class(std::string name) {
    return class_map[name];
}