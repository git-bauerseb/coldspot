#include "execution_engine.h"

u4 ExecutionEngine::execute(Frame* frame) {

    u1* byte_code = frame->method->code->code;

    while (1) {

        /*
        std::cout << "STACK CONTENT\n";
        for (int i = 0; i < 5; i++) {
            std::cout << frame->stack[i].int_value << "\n";
        }

        std::cout << "\n";
        */

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
                    frame->stack_ptr++;
                    u4 val = ((u1)(byte_code[frame->program_ctr]) - (u1)iconst_0);
                    frame->stack[frame->stack_ptr].int_value = val;
                    frame->program_ctr++;
                }
                break;

            case istore_0:
            case istore_1:
            case istore_2:
            case istore_3: {
                    u1 idx = byte_code[frame->program_ctr] - istore_0;
                    frame->stack[idx].int_value = frame->stack[frame->stack_ptr].int_value;
                    frame->stack_ptr--;
                    frame->program_ctr++;
                }
                break;
            case istore: {
                    u1 idx = byte_code[frame->program_ctr+1];
                    frame->stack[idx].int_value = frame->stack[frame->stack_ptr].int_value;
                    frame->stack_ptr--;
                    frame->program_ctr += 2;
                }
                break;
            case iload_0:
            case iload_1:
            case iload_2:
            case iload_3: {
                    u1 idx = byte_code[frame->program_ctr] - iload_0;
                    frame->stack_ptr++;
                    frame->stack[frame->stack_ptr] = frame->stack[idx];
                    frame->program_ctr++; 
                }
                break;
            case imul: {
                    u4 second = frame->stack[frame->stack_ptr].int_value;
                    u4 first = frame->stack[frame->stack_ptr-1].int_value;
                    frame->stack[frame->stack_ptr-1].int_value = first * second;
                    frame->program_ctr++;
                    frame->stack_ptr--;
                }
                break;

            case iadd: {
                    u4 second = frame->stack[frame->stack_ptr].int_value;
                    u4 first = frame->stack[frame->stack_ptr-1].int_value;
                    frame->stack[frame->stack_ptr-1].int_value = first + second;
                    frame->program_ctr++;
                    frame->stack_ptr--;
                }
                break;
            case isub: {
                    u4 second = frame->stack[frame->stack_ptr].int_value;
                    u4 first = frame->stack[frame->stack_ptr-1].int_value;
                    frame->stack[frame->stack_ptr-1].int_value = first - second;
                    frame->program_ctr++;
                    frame->stack_ptr--;
                }
                break;
            case bipush: {
                    u1 byte = byte_code[frame->program_ctr+1];
                    frame->stack_ptr++;
                    frame->stack[frame->stack_ptr].int_value = byte;
                    frame->program_ctr += 2;
                }
                break;
            case  return_:
                std::cout << "Top of stack " << frame->stack[frame->stack_ptr+1].int_value << "\n";
                return 0;
            default:
                std::cerr << "Unknown bytecode instruction\n";
        }

    }

    return 0;
}