#include <stdio.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "defender_system.h"

static size_t djb2(size_t hash, size_t field);
static void DumpStackInfo(stack_type* stack, stack_error_t error, call_data_t* call_info);
static void DumpStackFields(stack_type* stack, stack_error_t error);
static void DumpStackData(stack_type* stack);


stack_error_t CheckStackIntegrity(stack_type* stack) {

    // check stack pointer
    if (stack == nullptr) { return stack_error_t::NULL_POINTER; }

    // check struct canary
    if (stack->left_canary != STRUCT_CANARY_DEFAULT ||
        stack->right_canary != STRUCT_CANARY_DEFAULT) { return stack_error_t::STRUCT_CANARY_CORRUPT; }

    // check stack capacity
    if (stack->capacity == 0) { return stack_error_t::ZERO_CAPACITY; }

    // check data pointer
    if (stack->data == nullptr) { return stack_error_t::DATA_NULL_POINTER; }

    // check struct size
    if (stack->size > stack->capacity) { return stack_error_t::SIZE_OVER_CAPACITY; }

    // check data canary
    if (*(stack->data - 1) != CANARY_DEFAULT ||
        stack->data[stack->capacity] != CANARY_DEFAULT) { return stack_error_t::DATA_CANARY_CORRUPT; }

    // calculating current struct_hash and data_hash
    size_t current_struct_hash = CalculateStructHash(stack);
    size_t current_data_hash = CalculateDataHash(stack);

    // check struct_hash
    if (current_struct_hash != stack->struct_hash) { return stack_error_t::HASH_MISMATCH; }

    // check data_hash
    if (current_data_hash != stack->data_hash) { return stack_error_t::HASH_MISMATCH; }

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------

void StackDump(stack_type* stack, stack_error_t error, call_data_t* call_info, const char* message) {
    if (message != nullptr) { PrintColorVar(RED, "%s\n", message); }

    DumpStackInfo(stack, error, call_info);

    DumpStackFields(stack, error);

    DumpStackData(stack);
}

//----------------------------------------------------------------------------------

size_t CalculateStructHash(stack_type* stack) {
    if (stack == nullptr) return 0;

    size_t struct_hash = HASH_SEED;

    // left struct canary
    struct_hash = djb2(struct_hash, stack->left_canary);

    // stack size
    struct_hash = djb2(struct_hash, stack->size);

    // stack capacity
    struct_hash = djb2(struct_hash, stack->capacity);

    // data pointer
    struct_hash = djb2(struct_hash, (size_t)stack->data);

    // right struct canary
    struct_hash = djb2(struct_hash, stack->right_canary);

    return struct_hash;
}

//----------------------------------------------------------------------------------

size_t CalculateDataHash(stack_type* stack) {
    if (stack == nullptr) return 0;

    size_t data_hash = HASH_SEED;

    // left data canary
    data_hash = djb2(data_hash, (size_t)(*(stack->data - 1)));

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        data_hash = djb2(data_hash, (size_t)stack->data[pos]);
    }

    // right data canary
    data_hash = djb2(data_hash, (size_t)stack->data[stack->capacity]);

    return data_hash;
}

//----------------------------------------------------------------------------------

static size_t djb2(size_t hash, size_t field) {
    return ((hash << 5) + hash) + field;
}

//----------------------------------------------------------------------------------

static void DumpStackInfo(stack_type* stack, stack_error_t error, call_data_t* call_info) {
    printf("\nerror_in_file: ");
    PrintColorVar(YELLOW, "%s", call_info->file_name);
    printf("  line: ");
    PrintColorVar(YELLOW, "%d", call_info->line_number);
    printf("  function: ");
    PrintColorVar(YELLOW, "%s", call_info->func_name);
    printf("\n\n");

    printf("stack(");
    if (error == stack_error_t::NULL_POINTER) {
        PrintColorVar(RED, "%p", stack);
        printf(")\n");
        return;
    } else {
        PrintColorVar(BLUE, "%p", stack);
        printf(")");
        printf("  stack_object_name: ");
        PrintColorVar(YELLOW, "%s", stack->stack_info.obj_name);
        printf("  create_in_file: ");
        PrintColorVar(YELLOW, "%s", stack->stack_info.file_name);
        printf("  line: ");
        PrintColorVar(YELLOW, "%d", stack->stack_info.line_number);
        printf("  function: ");
        PrintColorVar(YELLOW, "%s", stack->stack_info.func_name);
        printf("\n");
    }
}

//----------------------------------------------------------------------------------

static void DumpStackFields(stack_type* stack, stack_error_t error) {
    switch(error) {
        case stack_error_t::STRUCT_CANARY_CORRUPT:
            printf("  STRUCT_CANARY: ");
            PrintColor(RED, "CORRUPTED\n");
            printf("    left_canary = ");
            if (stack->left_canary != STRUCT_CANARY_DEFAULT) {
                PrintColorVar(RED, "0x%zu", stack->left_canary);
                printf(" (expected: ");
                PrintColorVar(GREEN, "0x%zu", STRUCT_CANARY_DEFAULT);
                printf(")\n");
            } else {
                PrintColorVar(GREEN, "0x%zu\n", stack->left_canary);
            }
            printf("    right_canary = ");
            if (stack->right_canary != STRUCT_CANARY_DEFAULT) {
                PrintColorVar(RED, "0x%zu", stack->right_canary);
                printf(" (expected: ");
                PrintColorVar(GREEN, "0x%zu", STRUCT_CANARY_DEFAULT);
                printf(")\n");
            } else {
                PrintColorVar(GREEN, "0x%zu\n", stack->right_canary);
            }
            return;

        case stack_error_t::SIZE_OVER_CAPACITY:
            printf("  SIZE: ");
            PrintColor(RED, "OVERFLOW\n");
            printf("    size = ");
            PrintColorVar(RED, "%zu", stack->size);
            printf("    capacity = ");
            PrintColorVar(RED, "%zu", stack->capacity);
            printf("\n");
            return;

        case stack_error_t::ZERO_CAPACITY:
            printf("  CAPACITY: ");
            PrintColor(RED, "ZERO\n");
            return;

        case stack_error_t::DATA_NULL_POINTER:
            printf("  DATA: ");
            PrintColor(RED, "NULL_POINTER\n");
            return;

        case stack_error_t::DATA_CANARY_CORRUPT:
            printf("  DATA_CANARY: ");
            PrintColor(RED, "CORRUPTED\n");

            printf("    left_data_canary = ");
            if (*(stack->data - 1) != CANARY_DEFAULT) {
                PRINT_STACK_CANARY(RED, *(stack->data - 1));
                printf(" (expected: ");
                PRINT_STACK_CANARY(GREEN, CANARY_DEFAULT);
                printf(")\n");
            } else {
                PRINT_STACK_CANARY(GREEN, *(stack->data - 1));
            }

            printf("    right_data_canary = ");
            if (stack->data[stack->capacity] != CANARY_DEFAULT) {
                PRINT_STACK_CANARY(RED, stack->data[stack->capacity]);
                printf(" (expected: ");
                PRINT_STACK_CANARY(GREEN, CANARY_DEFAULT);
                printf(")\n");
            } else {
                PRINT_STACK_CANARY(GREEN, stack->data[stack->capacity]);
            }
            break;

        case stack_error_t::HASH_MISMATCH:
            printf("  HASH: ");
            PrintColor(RED, "MISMATCH\n");
            printf("    struct_hash = ");
            PrintColorVar(RED, "%zu", stack->struct_hash);
            printf(" (current: ");
            PrintColorVar(GREEN, "%zu", CalculateStructHash(stack));
            printf(")\n");
            printf("    data_hash = ");
            PrintColorVar(RED, "%zu", stack->data_hash);
            printf(" (current: ");
            PrintColorVar(GREEN, "%zu", CalculateDataHash(stack));
            printf(")\n");
            break;

        case stack_error_t::POP_EMPTY_STACK:
            printf("  OPERATION: ");
            PrintColor(RED, "POP_FROM_EMPTY_STACK\n");
            break;
        case stack_error_t::OK:
        case stack_error_t::NULL_POINTER:
        case stack_error_t::CALLOC_FAILED:
        case stack_error_t::REALLOC_FAILED:
            return;
        default:
            return;
    }
}

//----------------------------------------------------------------------------------

static void DumpStackData(stack_type* stack) {
    printf("  STACK_DATA (%zu/%zu):\n", stack->size, stack->capacity);

    PrintColor(BASE, "    [CANARY_LEFT] = ");
    if (*(stack->data - 1) == CANARY_DEFAULT) {
        PRINT_STACK_ELEMENT(GREEN, *(stack->data - 1));
    } else {
        PRINT_STACK_ELEMENT(RED, *(stack->data - 1));
    }

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        if (pos < stack->size) {
            PrintColorVar(BASE, "    *[%zu] = ", pos);
            PRINT_STACK_ELEMENT(GREEN, stack->data[pos]);
        } else if (pos == stack->size) {
            PrintColorVar(BASE, "    >[%zu] = ", pos);
            PRINT_STACK_ELEMENT(YELLOW, stack->data[pos]);
        } else {
            PrintColorVar(BASE, "    p[%zu] = ", pos);
            PRINT_STACK_ELEMENT(RED, stack->data[pos]);
        }
    }

    PrintColor(BASE, "    [CANARY_RIGHT] = ");
    if (stack->data[stack->capacity] == CANARY_DEFAULT) {
        PRINT_STACK_ELEMENT(GREEN, stack->data[stack->capacity]);
    } else {
        PRINT_STACK_ELEMENT(RED, stack->data[stack->capacity]);
    }
}
