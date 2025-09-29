#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "stack_func.h"

#define CHECK_ERROR(error) if (error != stack_error_t::OK) { return 1; }

#define INIT(name) .stack_info = {__FILE__, __LINE__, __PRETTY_FUNCTION__, #name}

#define CREATE_STACK(name) stack_type name = {INIT(name)}


int main(void) {
    stack_elem_t value = POISON;
    CREATE_STACK(st1);
    StackCtor(&st1, 5);
    PrintStack(&st1);
    StackPush(&st1, 10);
    PrintStack(&st1);
    StackPop(&st1, &value);
    PRINT_STACK_ELEMENT(BASE, value);
    PrintStack(&st1);
    StackDtor(&st1);

    return 0;
}
