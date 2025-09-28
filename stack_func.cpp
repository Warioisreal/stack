#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"
#include "define_lib.h"

#include "stack_func.h"

static stack_error_t StackError(stack_type* stack);
static void StackDump(stack_type* stack, stack_error_t error, const char* str);
static stack_error_t StackUpCapacity(stack_type* stack);

#define STACK_VERIFY(st, str) BEGIN { \
                                    stack_error_t err = StackError(st); \
                                    if (err != stack_error_t::NORMAL) { \
                                        StackDump(st, err, str); \
                                        return StackError(st); \
                                    } \
                                 } END

//----------------------------------------------------------------------------------

stack_error_t StackCtor(stack_type* stack, const size_t capacity) {

    stack->last_elem_ptr = 0;
    stack->capacity      = capacity;
    stack->data          = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t)) + 1;

    *(stack->data - 1)    = CANARY;
    stack->data[capacity] = CANARY;

    for (size_t pos = 0; pos < capacity; pos++) {
        stack->data[pos] = POISON;
    }

    STACK_VERIFY(stack, "error");

    return stack_error_t::NORMAL;
}

//----------------------------------------------------------------------------------

stack_error_t StackPush(stack_type* stack, const stack_elem_t value) {
    STACK_VERIFY(stack, "error");

    if (stack->last_elem_ptr + 1 == stack->capacity) {
        StackUpCapacity(stack);
    }
    stack->data[stack->last_elem_ptr++] = value;

    STACK_VERIFY(stack, "error");

    return stack_error_t::NORMAL;
}

//----------------------------------------------------------------------------------

stack_error_t StackPop(stack_type* stack, stack_elem_t* value) {
    STACK_VERIFY(stack, "error");

    if (stack->last_elem_ptr > 0) {
        *value = stack->data[--stack->last_elem_ptr];

        stack->data[stack->last_elem_ptr] = POISON;
    } else {
        StackDump(stack, stack_error_t::VOIDPOP, "voidpop");
        return stack_error_t::VOIDPOP;
    }

    STACK_VERIFY(stack, "error");

    return stack_error_t::NORMAL;
}

//----------------------------------------------------------------------------------

stack_error_t StackDtor(stack_type* stack) {
    STACK_VERIFY(stack, "error");

    stack->capacity = 0;
    stack->last_elem_ptr = 0;
    free(stack->data - 1);
    stack->data = nullptr;

    return stack_error_t::NORMAL;
}

//----------------------------------------------------------------------------------

stack_error_t PrintStack(stack_type* stack) {
    STACK_VERIFY(stack, "error");

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        if (pos < stack->last_elem_ptr) {
            PrintColor(BASE, "\t* ");
            printf("[%zu] = ", pos);
            PRINT_STACK_ELEM(GREEN, stack->data[pos]);
        } else
        if (pos == stack->last_elem_ptr) {
            PrintColor(BASE, "\t> ");
            printf("[%zu] = ", pos);
            PRINT_STACK_ELEM(YELLOW, stack->data[pos]);
        } else {
            PrintColor(BASE, "\tp ");
            printf("[%zu] = ", pos);
            PRINT_STACK_ELEM(RED, stack->data[pos]);
        }
    }

    printf("\n");
    return stack_error_t::NORMAL;
}

//----------------------------------------------------------------------------------

static stack_error_t StackError(stack_type* stack) {
    if (stack == nullptr) {
        return stack_error_t::NULLPTR;
    }
    if (stack->capacity == 0) {
        return stack_error_t::ZEROSIZE;
    }
    if (*(stack->data - 1) != CANARY || stack->data[stack->capacity] != CANARY) {
        return stack_error_t::CANARYERR;
    }
    if (stack->last_elem_ptr > stack->capacity) {
        return stack_error_t::OVERSIZE;
    }

    return stack_error_t::NORMAL;
}

//----------------------------------------------------------------------------------

static void StackDump(stack_type* stack, stack_error_t error, const char* str) {
    if (str != nullptr) { PrintColorVar(RED, "%s\n", str); }

    printf("stack(");
    if (error == stack_error_t::NULLPTR) {
        PrintColorVar(RED, "%p", stack);
        printf(")\n");
    } else {
        PrintColorVar(BLUE, "%p", stack);
        printf(")");
        printf("  stack_object_name: ");
        PrintColorVar(YELLOW, "%s", stack->stack_init_data.obj_name);
        printf("  create_in_file: ");
        PrintColorVar(YELLOW, "%s", stack->stack_init_data.file_name);
        printf("  line: ");
        PrintColorVar(YELLOW, "%d", stack->stack_init_data.line_number);
        printf("  create_in_function: ");
        PrintColorVar(YELLOW, "%s", stack->stack_init_data.func_name);
        printf("\n");

        if (error == stack_error_t::OVERSIZE) {
            printf("    last_elem_ptr = ");
            PrintColorVar(RED, "%zu\n", stack->last_elem_ptr);
            printf("    capacity = ");
            PrintColorVar(RED, "%zu\n", stack->capacity);
        } else
        if (error == stack_error_t::ZEROSIZE) {
            printf("    last_elem_ptr = ");
            PrintColorVar(RED, "%zu\n", stack->last_elem_ptr);
        } else {
            printf("    last_elem_ptr = ");
            PrintColorVar(GREEN, "%zu\n", stack->last_elem_ptr);
            printf("    capacity = ");
            PrintColorVar(GREEN, "%zu\n", stack->capacity);

            printf("\n\tstack_data:\n");

            if (error == stack_error_t::CANARYERR && *(stack->data - 1) != CANARY) {
                PrintColor(RED, "\tc ");
                printf("[-1] = ");
                PRINT_STACK_ELEM(RED, *(stack->data - 1));
            } else {
                PrintColor(GREEN, "\tc ");
                printf("[-1] = ");
                PRINT_STACK_ELEM(GREEN, *(stack->data - 1));
            }

            for (size_t pos = 0; pos < stack->capacity; pos++) {
                if (pos <= stack->last_elem_ptr) {
                    PrintColor(BASE, "\t* ");
                    printf("[%zu] = ", pos);
                    PRINT_STACK_ELEM(GREEN, stack->data[pos]);
                } else {
                    PrintColor(BASE, "\tp ");
                    printf("[%zu] = ", pos);
                    PRINT_STACK_ELEM(RED, stack->data[pos]);
                }
            }

            if (error == stack_error_t::CANARYERR && stack->data[stack->capacity] != CANARY) {
                PrintColor(RED, "\tc ");
                printf("[-1] = ");
                PRINT_STACK_ELEM(RED, stack->data[stack->capacity]);
            } else {
                PrintColor(GREEN, "\tc ");
                printf("[-1] = ");
                PRINT_STACK_ELEM(GREEN, stack->data[stack->capacity]);
            }
        }
    }
}

//----------------------------------------------------------------------------------

static stack_error_t StackUpCapacity(stack_type* stack) {
    STACK_VERIFY(stack, "error");

    stack->data[stack->capacity] = POISON;
    stack->capacity *= 2;
    *(&stack->data - 1) = (stack_elem_t*)realloc(stack->data - 1, (stack->capacity + 2) * sizeof(stack_elem_t));
    stack->data[stack->capacity] = CANARY;

    STACK_VERIFY(stack, "error");

    return stack_error_t::NORMAL;
}
