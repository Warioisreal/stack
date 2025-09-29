#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "defender_system.h"
#include "stack_func.h"

static stack_error_t StackUpCapacity(stack_type* stack);


stack_error_t StackCtor(stack_type* stack, const size_t capacity) {

    stack->left_canary   = STRUCT_CANARY_DEFAULT; // struct canary
    stack->size = 0;
    stack->capacity      = capacity;
    stack->data          = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t)) + 1;

    *(stack->data - 1)    = CANARY_DEFAULT;
    stack->data[capacity] = CANARY_DEFAULT;

    for (size_t pos = 0; pos < capacity; pos++) {
        stack->data[pos] = POISON;
    }

    stack->data_hash    = CalculateDataHash(stack);
    stack->right_canary = STRUCT_CANARY_DEFAULT; // struct canary
    stack->struct_hash  = CalculateStructHash(stack);

    STACK_VERIFY(stack, "stack construction error");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t StackPush(stack_type* stack, const stack_elem_t value) {
    STACK_VERIFY(stack, "error before push");

    if (stack->size + 1 == stack->capacity) {
        StackUpCapacity(stack);
    }
    stack->data[stack->size++] = value;

    stack->data_hash   = CalculateDataHash(stack);
    stack->struct_hash = CalculateStructHash(stack);

    STACK_VERIFY(stack, "error after push");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t StackPop(stack_type* stack, stack_elem_t* value) {
    STACK_VERIFY(stack, "error before pop");

    if (stack->size > 0) {
        *value = stack->data[--stack->size];

        stack->data[stack->size] = POISON;

        stack->data_hash   = CalculateDataHash(stack);
        stack->struct_hash = CalculateStructHash(stack);
    } else {
        StackDump(stack, stack_error_t::POP_EMPTY_STACK, "pop from empty stack");
        return stack_error_t::POP_EMPTY_STACK;
    }

    STACK_VERIFY(stack, "error after pop");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t StackDtor(stack_type* stack) {
    STACK_VERIFY(stack, "error before destruction");

    stack->left_canary   = 0;
    stack->size          = 0;
    stack->capacity      = 0;

    free(stack->data - 1);
    stack->data          = nullptr;

    stack->data_hash     = 0;
    stack->right_canary  = 0;
    stack->struct_hash   = 0;

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t PrintStack(stack_type* stack) {
    STACK_VERIFY(stack, "error before print");

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        if (pos < stack->size) {
            PrintColor(BASE, "\t* ");
            printf("[%zu] = ", pos);
            PRINT_STACK_ELEMENT(GREEN, stack->data[pos]);
        } else
        if (pos == stack->size) {
            PrintColor(BASE, "\t> ");
            printf("[%zu] = ", pos);
            PRINT_STACK_ELEMENT(YELLOW, stack->data[pos]);
        } else {
            PrintColor(BASE, "\tp ");
            printf("[%zu] = ", pos);
            PRINT_STACK_ELEMENT(RED, stack->data[pos]);
        }
    }

    printf("\n");
    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

static stack_error_t StackUpCapacity(stack_type* stack) {
    STACK_VERIFY(stack, "error before realloc");

    stack->capacity *= 2;

    stack_elem_t* new_data = (stack_elem_t*)realloc(stack->data - 1, (stack->capacity + 2) * sizeof(stack_elem_t));

    if (new_data == nullptr) {
        StackDump(stack, stack_error_t::REALLOC_FAILED, "realloc failed");
        return stack_error_t::REALLOC_FAILED;
    }

    stack->data                  = new_data + 1;

    for (size_t pos = stack->size; pos < stack->capacity; pos++) {
        stack->data[pos] = POISON;
    }

    stack->data[stack->capacity] = CANARY_DEFAULT;
    stack->data_hash             = CalculateDataHash(stack);
    stack->struct_hash           = CalculateStructHash(stack);

    STACK_VERIFY(stack, "error after realloc");

    return stack_error_t::OK;
}
