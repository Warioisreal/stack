#ifndef _DEFENDER_SYSTEM_H_
#define _DEFENDER_SYSTEM_H_

#include "define_lib.h"
#include "stack_lib.h"

const size_t HASH_SEED = 5381;

typedef enum class Errors : char {

    OK                    = 0,
    NULL_POINTER          = 1,    // stack == NULL_POINTER
    STRUCT_CANARY_CORRUPT = 2,    // struct canary error
    SIZE_OVER_CAPACITY    = 3,    // size > capacity
    ZERO_CAPACITY         = 4,    // capacity == 0
    DATA_NULL_POINTER     = 5,    // data == NULL_POINTER
    HASH_MISMATCH         = 6,    // hash mismatch
    DATA_CANARY_CORRUPT   = 7,    // data canary error
    POP_EMPTY_STACK       = 8,    // pop from empty stack
    SIZE_CAPACITY_INCONSISTENT = 9,    // size/capacity inconsistency
    REALLOC_FAILED        = 10    // realloc failure
} stack_error_t;

#define STACK_VERIFY(st, str) BEGIN { \
                                    stack_error_t err = CheckStackIntegrity(st); \
                                    if (err != stack_error_t::OK) { \
                                        StackDump(st, err, str); \
                                        return CheckStackIntegrity(st); \
                                    } \
                                 } END


stack_error_t CheckStackIntegrity(stack_type* stack);
void StackDump(stack_type* stack, stack_error_t error, const char* str);
size_t CalculateStructHash(stack_type* stack);
size_t CalculateDataHash(stack_type* stack);

#endif // _DEFENDER_SYSTEM_H_
