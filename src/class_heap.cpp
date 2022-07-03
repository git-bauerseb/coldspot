#include "../include/class_heap.h"

ClassHeap::ClassHeap() {}
ClassHeap::~ClassHeap() {
    for (auto& elem : class_map) {
        delete elem.second;
    }
}

void ClassHeap::add_class(std::string path) {
    ClassReader reader {path};

    JavaClass* class_ = new JavaClass(reader.get_bytecode());
    bool c_parse = class_->parse_class(); 

    if (!c_parse) {
        std::cerr << "Could not parse class file " << path << "\n";
        return;
    }

    class_map.insert({strip_ending(path), class_});
}

JavaClass* ClassHeap::get_class(std::string name) {
    return class_map[name];
}

std::string ClassHeap::strip_ending(std::string path) {
    
    int to = 0;
    
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == '.') {
            to = i;
            break;
        }
    }

    return path.substr(0, to);
}

JavaClass* ClassHeap::get_class_with_method(std::string& name, std::string& descr) {
    for(auto& p : class_map) {
        int idx = p.second->get_method_index(name, descr);
    
        if (idx >= 0) {
            return p.second;
        }
    }

    return nullptr;
}