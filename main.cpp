#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "stack_func.h"

#define CHECK_ERROR(error) if (error != stack_error_t::OK) { return (int)error; }

#define INIT(name) .stack_info = {__FILE__, __LINE__, __PRETTY_FUNCTION__, #name}

#define CREATE_STACK(name) stack_type name = {INIT(name)}


int main(void) {

    stack_elem_t value = POISON;
    CREATE_STACK(st1);
    CHECK_ERROR(StackCtor(&st1, 5));
    CHECK_ERROR(PrintStack(&st1));
    CHECK_ERROR(StackPush(&st1, 10));
    CHECK_ERROR(StackPush(&st1, 10));
    CHECK_ERROR(StackPush(&st1, 10));
    CHECK_ERROR(StackPush(&st1, 10));
    CHECK_ERROR(PrintStack(&st1));
    CHECK_ERROR(StackPop(&st1, &value));
    CHECK_ERROR(PrintStack(&st1));
    PRINT_STACK_ELEMENT(BASE, value);
    CHECK_ERROR(PrintStack(&st1));
    CHECK_ERROR(StackDtor(&st1));

    return 0;
}
