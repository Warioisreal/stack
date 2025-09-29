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

typedef struct VerifyCallData {
    const char* file_name = nullptr;
    int line_number       = 0;
    const char* func_name = nullptr;
} call_data_t;

#define STACK_VERIFY(st, str) BEGIN { \
                                        call_data_t call_info = {__FILE__, __LINE__, __PRETTY_FUNCTION__}; \
                                        stack_error_t err = CheckStackIntegrity(st); \
                                        if (err != stack_error_t::OK) { \
                                            StackDump(st, err, &call_info, str); \
                                            return CheckStackIntegrity(st); \
                                        } \
                                    } END


stack_error_t CheckStackIntegrity(stack_type* stack);
void StackDump(stack_type* stack, stack_error_t error, call_data_t* call_info, const char* message);
size_t CalculateStructHash(stack_type* stack);
size_t CalculateDataHash(stack_type* stack);

#endif // _DEFENDER_SYSTEM_H_
