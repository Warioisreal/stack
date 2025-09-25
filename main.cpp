#include <stdio.h>

#include "color_lib.h"

#include "stack_func.h"

#define CHECK_ERROR(error) if (error != stack_error_t::NORMAL) { return 1; }

#define INIT(name) .stack_init_data = {__FILE__, __LINE__, __func__, #name}

#define MAKE_STACK(name) stack_type name = {INIT(name)}


int main(void) {

    bool DEBUG = true;

    MAKE_STACK(st1);

    stack_elem_t value = POISON;

    CHECK_ERROR(StackCtor(&st1, 5));

    CHECK_ERROR(StackPush(&st1, 10));

    CHECK_ERROR(StackPush(&st1, 20));

    CHECK_ERROR(StackPush(&st1, 30));

    CHECK_ERROR(StackPop(&st1, &value));

    PRINT_STACK_ELEM(BASE, value);

    CHECK_ERROR(StackDtor(&st1));

    return 0;
}
