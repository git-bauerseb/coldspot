#include "../include/class_heap.h"
#include "../include/frame.h"
#include "../include/execution_engine.h"

void read_in_classfiles(ClassHeap& c_heap, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        c_heap.add_class(argv[i]);
    }
}

int main(int argc, char** argv) {
    ClassHeap heap{};

    read_in_classfiles(heap, argc, argv);

    std::string n_main{"main"};
    std::string n_descr{"()V"};

    JavaClass* class_ = heap.get_class_with_method(n_main, n_descr);

    if (class_ == nullptr) {
        std::cerr << "No class with main() method specified.\n";
        std::exit(1);
    }

    Frame frame{};
    frame.class_ = class_;

    // For now, reserve 20 spots on the stack
    frame.stack = new Variable[20];

    // Get index of main method

    int idx = class_->get_method_index(n_main, n_descr);

    if (idx < 0) {
        std::cerr << "No main() method specified\n";
        return 1;
    }

    frame.method = &class_->methods[idx];

    // Reserve space for local variables
    frame.stack_ptr = class_->methods[idx].code->max_locals;

    ExecutionEngine engine{&heap};
    engine.execute(&frame);

    delete[] frame.stack;

    return 0;
}