#include <stdio.h>

#include "color_lib.h"

#include "stack_func.h"

#define CHECK_ERROR(error) if (error != stack_error_t::NORMAL) { return 1; }

#define INIT(name) .stack_init_data = {__FILE__, __LINE__, __PRETTY_FUNCTION__, #name}

#define MAKE_STACK(name) stack_type name = {INIT(name)}


int main(void) {

    bool DEBUG = true;

    MAKE_STACK(st1);

    stack_elem_t value = POISON;

    CHECK_ERROR(StackCtor(&st1, 5));

    PrintStack(&st1);

    CHECK_ERROR(StackPush(&st1, 10));

    PrintStack(&st1);

    CHECK_ERROR(StackPush(&st1, 20));

    PrintStack(&st1);

    CHECK_ERROR(StackPush(&st1, 30));

    PrintStack(&st1);

    CHECK_ERROR(StackPush(&st1, 40));

    PrintStack(&st1);

    CHECK_ERROR(StackPush(&st1, 50));

    PrintStack(&st1);

    CHECK_ERROR(StackPop(&st1, &value));

    PrintStack(&st1);

    PRINT_STACK_ELEM(BASE, value);

    CHECK_ERROR(StackDtor(&st1));

    return 0;
}
