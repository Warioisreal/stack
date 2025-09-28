#ifndef _STACK_FUNC_H_
#define _STACK_FUNC_H_

#include "color_lib.h"

//#define INIT()
#define PRINT_VAR_NAME(var) printf("%s\n", #var)

typedef int stack_elem_t;

#define PRINT_STACK_ELEM(color, var) PrintColorVar(color, "%d\n", var)

const stack_elem_t POISON = 0;

const stack_elem_t CANARY = 0xbecaca;

typedef struct StructInitData {
    const char* file_name = nullptr;
    int line_number       = 0;
    const char* func_name = nullptr;
    const char* obj_name  = nullptr;
} init_data_t;

typedef struct Stack {
    stack_elem_t* data   = nullptr;
    size_t last_elem_ptr = 0;
    size_t capacity      = 0;
    init_data_t stack_init_data = {};
} stack_type;

typedef enum class Errors : char {
    NORMAL    = 0,
    NULLPTR   = 1,
    OVERSIZE  = 2,
    ZEROSIZE  = 3,
    CANARYERR = 4,
    VOIDPOP   = 5,
    SIZEERROR = 6
} stack_error_t;


stack_error_t StackCtor(stack_type* stack, const size_t capacity);
stack_error_t StackPush(stack_type* stack, const stack_elem_t value);
stack_error_t StackPop(stack_type* stack, stack_elem_t* value);
stack_error_t StackDtor(stack_type* stack);

#endif // _STACK_FUNC_H_
