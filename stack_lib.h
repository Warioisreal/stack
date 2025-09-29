#ifndef _STACK_LIB_H_
#define _STACK_LIB_H_

#include <ctype.h>

typedef int stack_elem_t;

#define PRINT_STACK_ELEMENT(color, var) PrintColorVar(color, "%d\n", var)
#define PRINT_STACK_CANARY(color, var) PrintColorVar(color, "0x%X\n", (unsigned int)var)

const size_t STRUCT_CANARY_DEFAULT = 0xbadf00d;
const stack_elem_t CANARY_DEFAULT  = 0xbadcaca;
const stack_elem_t POISON          = 0;

typedef struct StructInitData {
    const char* file_name = nullptr;
    int line_number       = 0;
    const char* func_name = nullptr;
    const char* obj_name  = nullptr;
} init_data_t;

typedef struct Stack {
    size_t left_canary          = 0;
    size_t size                 = 0;
    size_t capacity             = 0;
    stack_elem_t* data          = nullptr;
    size_t data_hash            = 0;
    init_data_t stack_info = {};
    size_t right_canary         = 0;
    size_t struct_hash          = 0;
} stack_type;

#endif // _STACK_LIB_H_
