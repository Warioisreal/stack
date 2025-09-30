#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "defender_system.h"
#include "stack_func.h"

static stack_error_t StackIncreaseCapacity(stack_type* stack);
static stack_error_t StackShrinkCapacity(stack_type* stack);


stack_error_t StackCtor(stack_type* stack, const size_t capacity) {

    #ifdef DEBUG
    stack->left_canary  = STRUCT_CANARY_DEFAULT; // struct canary
    #endif
    stack->size         = 0;
    stack->capacity     = capacity;

    #ifdef DEBUG
    stack_elem_t* buf_data = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t)) + 1;
    #else
    stack_elem_t* buf_data = (stack_elem_t*)calloc(capacity, sizeof(stack_elem_t));
    #endif
    if (buf_data == nullptr) {
        GET_INFO(call_info);
        stack->error = stack_error_t::CALLOC_FAILED;
        StackDump(stack, &call_info, "calloc failed");
        return stack_error_t::CALLOC_FAILED;
    }

    stack->data         = buf_data;

    #ifdef DEBUG
    *(stack->data - 1)    = CANARY_DEFAULT;
    stack->data[capacity] = CANARY_DEFAULT;
    #endif

    for (size_t pos = 0; pos < capacity; pos++) {
        stack->data[pos] = POISON;
    }

    #ifdef DEBUG
    stack->data_hash    = CalculateDataHash(stack);
    stack->right_canary = STRUCT_CANARY_DEFAULT; // struct canary
    stack->struct_hash  = CalculateStructHash(stack);
    #endif

    STACK_VERIFY(stack, "stack construction error");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t StackPush(stack_type* stack, const stack_elem_t value) {
    STACK_VERIFY(stack, "error before push");

    if (stack->size + 1 == stack->capacity) {
        StackIncreaseCapacity(stack);
    }
    stack->data[stack->size++] = value;

    #ifdef DEBUG
    stack->data_hash   = CalculateDataHash(stack);
    stack->struct_hash = CalculateStructHash(stack);
    #endif

    STACK_VERIFY(stack, "error after push");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t StackPop(stack_type* stack, stack_elem_t* value) {
    STACK_VERIFY(stack, "error before pop");

    if (stack->size > 0) {
        *value = stack->data[--stack->size];

        stack->data[stack->size] = POISON;

        #ifdef DEBUG
        stack->data_hash   = CalculateDataHash(stack);
        stack->struct_hash = CalculateStructHash(stack);
        #endif

        if (stack->size == stack->capacity / 4 && stack->size > 1) {
            StackShrinkCapacity(stack);
        }
    } else {
        GET_INFO(call_info);
        stack->error = stack_error_t::POP_EMPTY_STACK;
        StackDump(stack, &call_info, "pop from empty stack");
        return stack_error_t::POP_EMPTY_STACK;
    }

    STACK_VERIFY(stack, "error after pop");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t StackDtor(stack_type* stack) {
    STACK_VERIFY(stack, "error before destruction");

    #ifdef DEBUG
    stack->left_canary   = 0;
    #endif
    stack->size          = 0;
    stack->capacity      = 0;

    #ifdef DEBUG
    free(stack->data - 1);
    #else
    free(stack->data);
    #endif
    stack->data          = nullptr;

    #ifdef DEBUG
    stack->data_hash     = 0;
    stack->right_canary  = 0;
    stack->struct_hash   = 0;
    #endif

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

stack_error_t PrintStack(stack_type* stack) {
    STACK_VERIFY(stack, "error before print");

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        if (pos < stack->size) {
            PrintColorVar(BASE, "    *[%zu] = ", pos);
            PRINT_STACK_ELEMENT(GREEN, stack->data[pos]);
        } else if (pos == stack->size) {
            PrintColorVar(BASE, "    >[%zu] = ", pos);
            PRINT_STACK_ELEMENT(YELLOW, stack->data[pos]);
        } else {
            PrintColorVar(GREY, "    p[%zu] = ", pos);
            PRINT_STACK_ELEMENT(GREY, stack->data[pos]);
        }
    }

    printf("\n");
    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

static stack_error_t StackIncreaseCapacity(stack_type* stack) {
    STACK_VERIFY(stack, "error before realloc");

    stack->capacity *= 2;

    #ifdef DEBUG
    stack_elem_t* new_data = (stack_elem_t*)realloc(stack->data - 1, (stack->capacity + 2) * sizeof(stack_elem_t));
    #else
    stack_elem_t* new_data = (stack_elem_t*)realloc(stack->data, stack->capacity * sizeof(stack_elem_t));
    #endif

    if (new_data == nullptr) {
        GET_INFO(call_info);
        stack->error = stack_error_t::REALLOC_FAILED;
        StackDump(stack, &call_info, "realloc failed");
        return stack_error_t::REALLOC_FAILED;
    }

    #ifdef DEBUG
    stack->data                  = new_data + 1;
    #else
    stack->data                  = new_data;
    #endif

    for (size_t pos = stack->size; pos < stack->capacity; pos++) {
        stack->data[pos] = POISON;
    }

    #ifdef DEBUG
    stack->data[stack->capacity] = CANARY_DEFAULT;
    stack->data_hash             = CalculateDataHash(stack);
    stack->struct_hash           = CalculateStructHash(stack);
    #endif

    STACK_VERIFY(stack, "error after realloc");

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

static stack_error_t StackShrinkCapacity(stack_type* stack) {
    STACK_VERIFY(stack, "error before realloc");

    stack->capacity /= 2;

    #ifdef DEBUG
    stack_elem_t* new_data = (stack_elem_t*)realloc(stack->data - 1, (stack->capacity + 2) * sizeof(stack_elem_t));
    #else
    stack_elem_t* new_data = (stack_elem_t*)realloc(stack->data, stack->capacity * sizeof(stack_elem_t));
    #endif

    if (new_data == nullptr) {
        GET_INFO(call_info);
        stack->error = stack_error_t::REALLOC_FAILED;
        StackDump(stack, &call_info, "realloc failed");
        return stack_error_t::REALLOC_FAILED;
    }

    #ifdef DEBUG
    stack->data                  = new_data + 1;
    #else
    stack->data                  = new_data;
    #endif

    for (size_t pos = stack->size; pos < stack->capacity; pos++) {
        stack->data[pos] = POISON;
    }

    #ifdef DEBUG
    stack->data[stack->capacity] = CANARY_DEFAULT;
    stack->data_hash             = CalculateDataHash(stack);
    stack->struct_hash           = CalculateStructHash(stack);
    #endif

    STACK_VERIFY(stack, "error after realloc");

    return stack_error_t::OK;
}
