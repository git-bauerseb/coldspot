#include "class_heap.h"
#include "frame.h"
#include "execution_engine.h"

int main(int argc, char** argv) {

    ClassHeap heap{};
    heap.add_class("Test.class");

    JavaClass* class_ = heap.get_class("Test.class");

    Frame frame{};
    frame.class_ = class_;

    // For now, reserve 20 spots on the stack
    frame.stack = new Variable[20];

    // Get index of main method
    int idx = class_->get_method_index("main", "()V");

    if (idx < 0) {
        std::cerr << "No main() method specified\n";
        return 1;
    }

    frame.method = &class_->methods[idx];

    // Reserve space for local variables
    frame.stack_ptr = class_->methods[idx].code->max_locals;

    ExecutionEngine engine{};
    engine.execute(&frame);

    delete[] frame.stack;

    return 0;
}