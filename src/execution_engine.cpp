#include "../include/execution_engine.h"

u2 ExecutionEngine::get_u2(char* ptr) {
    return (u2)((u1)(ptr)[0]<< 8 & 0x0000FF00 
    | (u1)(ptr)[1]);
}

u4 ExecutionEngine::get_u4(char* ptr) {
    return (u4)( (u4)((u1)(ptr)[0])<<24 & 0xFF000000 
    | (u4)((u1)(ptr)[1])<<16 & 0x00FF0000
    | (u4)((u1)(ptr)[2])<<8 & 0x0000FF00
    | (u4)((u1)(ptr)[3]) & 0x000000FF);
}

i2 ExecutionEngine::get_i2(char* ptr) {
    return (i2)((ptr[0] << 8) | ptr[1]);
}

u4 ExecutionEngine::execute(Frame* frame) {

    u1* byte_code = frame->method->code->code;

    while (1) {
        switch (byte_code[frame->program_ctr]) {
            // Push constant 0 on stack
            case nop:
                frame->program_ctr++;
                break;
            case iconst_0:
            case iconst_1:
            case iconst_2:
            case iconst_3:
            case iconst_4:
            case iconst_5: {
                    u4 val = ((u1)(byte_code[frame->program_ctr]) - (u1)iconst_0);
                    frame->push_i(val);
                    frame->program_ctr++;
                }
                break;

            case istore_0:
            case istore_1:
            case istore_2:
            case istore_3: {
                    u1 idx = byte_code[frame->program_ctr] - istore_0;
                    frame->stack[idx].int_value = frame->pop_i();
                    frame->program_ctr++;
                }
                break;
            case istore: {
                    u1 idx = byte_code[frame->program_ctr+1];
                    frame->stack[idx].int_value = frame->pop_i();
                    frame->program_ctr += 2;
                }
                break;
            case iload_0:
            case iload_1:
            case iload_2:
            case iload_3: {
                    u1 idx = byte_code[frame->program_ctr] - iload_0;
                    frame->push_i(frame->stack[idx].int_value);
                    frame->program_ctr++; 
                }
                break;
            case imul: {I_OP(*)} break;
            case idiv: {I_OP(/)} break;
            case iadd: {I_OP(+)} break;
            case isub: {I_OP(-)} break;
            case bipush: {
                    u1 byte = byte_code[frame->program_ctr+1];
                    frame->push_i(byte);
                    frame->program_ctr += 2;
                }
                break;
            case sipush: {
                    i2 short_val = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    frame->push_s(short_val);
                    frame->program_ctr += 3;
                }
                break;
            case ldc: {
                    u1 idx = byte_code[frame->program_ctr+1];
                    Variable const_ = load_constant(frame->class_, idx);
                    frame->push_v(const_);
                    frame->program_ctr += 2;
                }
                break;
            case invokestatic: {
                    u2 idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    execute_invokestatic(frame, idx);
                    frame->program_ctr += 3;
                }
                break;

            case invokespecial: {
                    // Top of stack is an object reference
                    Object obj = frame->pop_ref();
                    u2 idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    execute_invokespecial(frame, obj, idx);
                    frame->program_ctr += 3;
                }
                break;
            case putstatic: {
                    u2 idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    Variable value = frame->pop_v();
                    store_in_static_field(value, frame, idx);
                    frame->program_ctr += 3;
                }
                break;

            case if_icmpge: {
                    int second = frame->pop_i();
                      int first = frame->pop_i();
                      i2 addr = get_i2((char*)&byte_code[frame->program_ctr+1]);
                       if (first >= second) {
                            frame->program_ctr += addr;                        } else { \
                            frame->program_ctr += 3;
                        }
                }
                break;
            case if_icmpgt: {IF_ICMP(>)}
                break;
            case if_icmple: {IF_ICMP(<=)}
                break;
            case if_icmplt: {IF_ICMP(<)}
                break;
            case if_icmpne: {IF_ICMP(!=)}
                break;
            case iinc: {
                    u1 var_idx = byte_code[frame->program_ctr+1];
                    char amount = byte_code[frame->program_ctr+2];

                    frame->stack[var_idx].int_value = ((int)(frame->stack[var_idx].int_value) + amount);
                    frame->program_ctr += 3;
                }
                break;
            case goto_: {
                    i2 addr = get_i2((char*)&byte_code[frame->program_ctr+1]);
                    frame->program_ctr += addr;
                }
                break;

            case new_: {
                    u2 class_idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    Variable var = create_new_object(frame, class_idx);
                    frame->push_v(var);
                    frame->program_ctr += 3;
                }
                break;
            case dup: {
                    Variable v = frame->pop_v();
                    frame->push_v(v);
                    frame->push_v(v);
                    frame->program_ctr++;
                }
                break;
            case getstatic: {
                    u2 idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    Variable field_val = get_from_static_field(frame, idx);
                    frame->push_v(field_val);
                    frame->program_ctr += 3;
                }
                break;
            case ireturn: {
                    return frame->pop_i();
                }
                break;
            case aload_0:
            case aload_1:
            case aload_2:
            case aload_3: {
                    u2 idx = byte_code[frame->program_ctr] - aload_0;
                    frame->push_ref(frame->stack[idx].object);
                    frame->program_ctr += 1;
                }
                break;
            case astore_0:
            case astore_1:
            case astore_2:
            case astore_3: {
                    u2 idx = byte_code[frame->program_ctr] - astore_0;
                    frame->stack[idx].object = frame->pop_ref();
                    frame->program_ctr += 1;
                }
                break;
            case putfield: {
                    // Index of field
                    u2 idx = get_u2((char*)&byte_code[frame->program_ctr+1]);

                    // Variable is on top
                    Variable var = frame->pop_v();
                    Object obj = frame->pop_ref();
                    execute_putfield(frame, obj, var, idx);

                    frame->program_ctr += 3;
                }
                break;
            case getfield: {
                    u2 idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    Object obj = frame->pop_ref();
                    Variable var = execute_getfield(frame, obj, idx);
                    frame->push_v(var);
                    frame->program_ctr += 3;
                }
                break;
            case newarray: {
                    int size = frame->pop_i();
                    u1 type = byte_code[frame->program_ctr+1];
                    frame->push_ref(execute_newarray(frame, size, type));

                    frame->program_ctr += 2;
                }
                break;
            case anewarray: {
                    // Size of new array is pushed on stack
                    int size = frame->pop_i();
                    u2 obj_type_idx = get_u2((char*)&byte_code[frame->program_ctr+1]);
                    frame->program_ctr += 3;
                }
                break;
            case iastore: {
                    int val = frame->pop_i();
                    int idx = frame->pop_i();

                    // Reference to array
                    Object a_ref = frame->pop_ref();


                    Variable* elements = (Variable*)m_object_heap.derference_object(a_ref);
                    elements[idx+2].int_value = val;

                    m_object_heap.debug_print_primitive_array(a_ref);


                    frame->program_ctr += 1;
                }
                break;
            case iaload: {
                    int idx = frame->pop_i();
                    Object a_ref = frame->pop_ref();



                    Variable* elements = (Variable*)m_object_heap.derference_object(a_ref);
                    frame->push_i(elements[idx+2].int_value);

                    frame->program_ctr += 1;
                }
                break;
            case arraylength: {
                    Object a_ref = frame->pop_ref();

                    Variable* elements = (Variable*)m_object_heap.derference_object(a_ref);
                    
                    // Push size onto stack
                    frame->push_i(elements[1].int_value);

                    frame->program_ctr += 1;
                }
                break;
            case  return_:
                std::cout << "Top of stack " << frame->stack[frame->stack_ptr-1].int_value << "\n";
                return 0;
            default:
                std::cerr << "Unknown bytecode instruction\n";
        }

    }

    return 0;
}


Variable ExecutionEngine::load_constant(JavaClass* class_, u1 index) {
    Variable v {};
    v.ptrValue = NULL;

    char* elem = (char*)class_->constant_pool[index];

    switch (elem[0]) {
        case CP_Type::CONSTANT_Integer: {
            u1 first = elem[1];
            u1 second = elem[2];
            u1 third = elem[3];
            u1 fourth = elem[4];
            v.int_value = (((u4) first) << 24) | (((u4) second) << 16) | (((u4) third) << 8) | (((u4) fourth));
        }
        break;
    
        default:
            break;
    }

    return v;
}

Variable ExecutionEngine::get_from_static_field(Frame* frame, u2 idx) {
    char* pool_ptr = (char*)frame->class_->constant_pool[idx];

    if (pool_ptr[0] != CONSTANT_Fieldref) {
        std::cerr << "Element in constant pool is not of type CONSTANT_Fieldref\n";
        std::exit(1);
    }

    u2 class_idx = get_u2(&pool_ptr[1]);
    u2 name_type_idx = get_u2(&pool_ptr[3]);

    // Class
    pool_ptr = (char*)frame->class_->constant_pool[class_idx];

    if (pool_ptr[0] != CONSTANT_Class) {
        std::cerr << "No CONSTANT_Class\n";
        std::exit(1);
    }

    std::string c_name;

    u2 class_name_idx = get_u2(&pool_ptr[1]);
    frame->class_->string_from_constant_pool(class_name_idx, c_name);

    // Name and type
    pool_ptr = (char*)frame->class_->constant_pool[name_type_idx];

    if (pool_ptr[0] != CONSTANT_NameAndType) {
        std::cerr << "No NameAndType\n";
        std::exit(1);
    }

    u2 name_idx = get_u2(&pool_ptr[1]);
    u2 descr_idx = get_u2(&pool_ptr[3]);

    std::string f_name;
    std::string f_descr;

    frame->class_->string_from_constant_pool(name_idx, f_name);
    frame->class_->string_from_constant_pool(descr_idx, f_descr);
    
    CP_Type type = get_constant_elem_type(f_descr);

    JavaClass* virt_class = m_class_heap->get_class(c_name);
    Variable* val = virt_class->get_static_value(f_name);

    return *val;
}

void ExecutionEngine::store_in_static_field(Variable value, Frame* frame, u2 idx) {
    char* pool_ptr = (char*)frame->class_->constant_pool[idx];

    if (pool_ptr[0] != CONSTANT_Fieldref) {
        std::cerr << "Element in constant pool is not of type CONSTANT_Fieldref\n";
        std::exit(1);
    }

    u2 class_idx = get_u2(&pool_ptr[1]);
    u2 name_type_idx = get_u2(&pool_ptr[3]);

    // Class
    pool_ptr = (char*)frame->class_->constant_pool[class_idx];

    if (pool_ptr[0] != CONSTANT_Class) {
        std::cerr << "No CONSTANT_Class\n";
        std::exit(1);
    }

    std::string c_name;

    u2 class_name_idx = get_u2(&pool_ptr[1]);
    frame->class_->string_from_constant_pool(class_name_idx, c_name);

    // Name and type
    pool_ptr = (char*)frame->class_->constant_pool[name_type_idx];

    if (pool_ptr[0] != CONSTANT_NameAndType) {
        std::cerr << "No NameAndType\n";
        std::exit(1);
    }

    u2 name_idx = get_u2(&pool_ptr[1]);
    u2 descr_idx = get_u2(&pool_ptr[3]);

    std::string f_name;
    std::string f_descr;

    frame->class_->string_from_constant_pool(name_idx, f_name);
    frame->class_->string_from_constant_pool(descr_idx, f_descr);
    
    CP_Type type = get_constant_elem_type(f_descr);

    JavaClass* virt_class = m_class_heap->get_class(c_name);
    virt_class->set_static_value(f_name, value, type);

}

void ExecutionEngine::execute_invokespecial(Frame* current, Object obj, u2 method_idx) {
    //Dereference first field of object which is a pointer to a class
    // Variable* fields = (Variable*)m_object_heap.derference_object(obj);
    char* pool_ptr = (char*)current->class_->constant_pool[method_idx];

    if (pool_ptr[0] != CP_Type::CONSTANT_Methodref) {
        std::cerr << "Cannot call instance method with non method type\n";
        std::exit(1);
    }

    u2 class_idx = get_u2(&pool_ptr[1]);
    u2 name_type_idx = get_u2(&pool_ptr[3]);

    pool_ptr = (char*)current->class_->constant_pool[class_idx];

    if (pool_ptr[0] != CP_Type::CONSTANT_Class) {
        std::cout << "Element in constant pool is not of type CONSTANT_Class\n";
        std::exit(1);
    }

    u2 class_name_idx = get_u2(&pool_ptr[1]);

    std::string class_name;
    current->class_->string_from_constant_pool(class_name_idx, class_name);

    JavaClass* class_;

    // If the member of object is called, return from execution, because
    // no defintion of object is currently provided
    if (!class_name.compare("java/lang/Object")) {
        return;
    } else {
        class_ = m_class_heap->get_class(class_name);
    }


    pool_ptr = (char*)current->class_->constant_pool[name_type_idx];

    if (pool_ptr[0] != CP_Type::CONSTANT_NameAndType) {
        std::cout << "Element in constant pool is not of type CONSTANT_NameAndType\n";
        std::exit(1);
    }

    u2 method_name_idx = get_u2(&pool_ptr[1]);
    u2 descriptor_idx = get_u2(&pool_ptr[3]);

    std::string method_name;
    std::string method_descr;

    // Get strings from current class
    current->class_->string_from_constant_pool(method_name_idx, method_name);
    current->class_->string_from_constant_pool(descriptor_idx, method_descr);

    u2 parameters = get_method_parameter(method_descr) + 1;
    bool returning = is_returning(method_descr);

    // Setup new frame
    Frame* frame = new Frame();
    frame->class_ = class_;
    frame->stack = new Variable[20];

    // Set current object
    Object cur_obj = m_object;
    m_object = obj;

    // Get Method
    u2 idx_ = current->class_->get_method_index(method_name, method_descr);
    frame->method = &class_->methods[idx_];

    // Reserve space for locals + arguments
    u2 reserve = current->class_->methods[idx_].code->max_locals + parameters;
    frame->stack_ptr = reserve;

    // Push arguments onto stack
    for (int i = 0; i < parameters; i++) {
        frame->stack[i] = current->stack[current->stack_ptr-i];
    }

    // Execute method
    u4 ret_value = this->execute(frame);

    // Restore original object
    m_object = cur_obj;

    delete[] frame->stack;
    delete frame;

    if (ret_value) {
        current->stack[current->stack_ptr].int_value = ret_value;
    }
}

void ExecutionEngine::execute_invokestatic(Frame* current, u2 idx) {
    JavaClass* c_class = current->class_;
    char* pool_ptr = (char*)c_class->constant_pool[idx];

    CONSTANT_Methodref_info* m_ref = (CONSTANT_Methodref_info*)pool_ptr;

    if (pool_ptr[0] != CP_Type::CONSTANT_Methodref) {
        std::cout << "Element in constant pool is not of type CONSTANT_Methodref for static invocation\n";
        std::exit(1);
    }

    u2 class_idx = get_u2(&pool_ptr[1]);
    u2 name_idx = get_u2(&pool_ptr[3]);

    // Get class
    pool_ptr = (char*)c_class->constant_pool[class_idx];

    if (pool_ptr[0] != CP_Type::CONSTANT_Class) {
        std::cout << "Element in constant pool is not of type CONSTANT_Class\n";
        std::exit(1);
    }

    // Get class name
    u2 class_name_idx = get_u2(&pool_ptr[1]);

    std::string class_name;
    c_class->string_from_constant_pool(class_name_idx, class_name);
    // std::cout << class_name_idx << "\n";

    // Get NameAndType
    pool_ptr = (char*)c_class->constant_pool[name_idx];

    if (pool_ptr[0] != CP_Type::CONSTANT_NameAndType) {
        std::cout << "Element in constant pool is not of type CONSTANT_NameAndType\n";
        std::exit(1);
    }

    name_idx = get_u2(&pool_ptr[1]);
    u2 descr_idx = get_u2(&pool_ptr[3]);

    std::string m_name, m_descr;
    c_class->string_from_constant_pool(name_idx, m_name);
    c_class->string_from_constant_pool(descr_idx, m_descr);

    u2 parameters = get_method_parameter(m_descr);
    bool returns = is_returning(m_descr);

    // Virtual class in which the called method resides
    JavaClass* v_class = m_class_heap->get_class(class_name);

    Frame* frame = new Frame();
    frame->class_ = v_class;
    frame->stack = new Variable[20];

    u2 idx_ = v_class->get_method_index(m_name, m_descr);
    frame->method = &v_class->methods[idx_];

    // Reserve space for locals + arguments
    u2 max_locals = v_class->methods[idx_].code->max_locals;
    u2 reserve = max_locals;
    frame->stack_ptr = 0;

    // Push arguments onto stack of new frame
    for (int i = 0; i < parameters; i++) {
        frame->push_v(current->pop_v());
    }

    u4 ret_value = this->execute(frame);

    delete[] frame->stack;
    delete[] frame;

    if (ret_value) {
        current->push_i(ret_value);
    }
}

u2 ExecutionEngine::get_method_parameter(std::string& description) {
    u2 i = 1;

    while (description[i] != ')') {
        i++;
    }

    return i-1;
}


bool ExecutionEngine::is_returning(std::string& description) {
    int i = 0;

    // Skip parameters
    while (description[i] != ')') {i++;}

    return description[i+1] != 'V'; 
}


CP_Type ExecutionEngine::get_constant_elem_type(std::string& descr) {
    switch (descr[0]) {
        case 'I':
            return CP_Type::CONSTANT_Integer;
        default:
            std::cerr << "Error, invalid type\n";
            std::exit(1);
    }
}

Variable ExecutionEngine::create_new_object(Frame* frame, u2 class_idx) {

    char* pool_ptr = (char*)frame->class_->constant_pool[class_idx];

    if (pool_ptr[0] != CP_Type::CONSTANT_Class) {
        std::cerr << "Cannot create object from non class type\n";
        std::exit(1);
    }

    u2 name_idx = get_u2(&pool_ptr[1]);

    std::string class_name;
    frame->class_->string_from_constant_pool(name_idx, class_name);

    JavaClass* class_ = m_class_heap->get_class(class_name);

    Object obj = m_object_heap.create_object(class_);

    Variable v;
    v.object = obj;
    return v;
}

void ExecutionEngine::execute_putfield(Frame* frame, Object obj, Variable var, u2 field_idx) {
    Variable* fields = (Variable*)m_object_heap.derference_object(obj);
    JavaClass* class_ = (JavaClass*)fields[0].ptrValue;

    if (class_->magic != 0xCAFEBABE) {
        std::cerr << "Error execute_putfield()\n";
        std::exit(1);
    }

    char* pool_ptr = (char*)frame->class_->constant_pool[field_idx];

    if (pool_ptr[0] != CONSTANT_Fieldref) {
        std::cerr << "Error in execute_putfield(), constant_pool access\n";
        std::exit(1);
    }

    u2 name_type_idx = get_u2(&pool_ptr[3]);
    pool_ptr = (char*)frame->class_->constant_pool[name_type_idx];

    u2 field_name_idx = get_u2(&pool_ptr[1]);

    std::string f_name;
    frame->class_->string_from_constant_pool(field_name_idx, f_name);

    field_idx = class_->get_field_index(f_name);

    fields[field_idx] = var;
}

Variable ExecutionEngine::execute_getfield(Frame* frame, Object obj, u2 field_idx) {
    Variable* fields = (Variable*)m_object_heap.derference_object(obj);
    JavaClass* class_ = (JavaClass*)fields[0].ptrValue;

    if (class_->magic != 0xCAFEBABE) {
        std::cerr << "Error execute_getfield()\n";
        std::exit(1);
    }

    char* pool_ptr = (char*)frame->class_->constant_pool[field_idx];

    if (pool_ptr[0] != CONSTANT_Fieldref) {
        std::cerr << "Error in execute_putfield(), constant_pool access\n";
        std::exit(1);
    }

    u2 name_type_idx = get_u2(&pool_ptr[3]);
    pool_ptr = (char*)frame->class_->constant_pool[name_type_idx];

    u2 field_name_idx = get_u2(&pool_ptr[1]);

    std::string f_name;
    frame->class_->string_from_constant_pool(field_name_idx, f_name);

    field_idx = class_->get_field_index(f_name);

    return fields[field_idx];
}

Object ExecutionEngine::execute_newarray(Frame* frame, int size, u1 type) {
    Object obj = m_object_heap.create_primitive_array(size, type);
    return obj;
}