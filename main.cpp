#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "stack_func.h"

#include "stack_attack.h"

#define CHECK_ERROR(error) if (error != stack_error_t::OK) { return (int)error; }

#ifdef DEBUG
#define INIT(name) .stack_info = {__FILE__, __LINE__, __PRETTY_FUNCTION__, #name}
#define CREATE_STACK(name) stack_type name = {INIT(name)}
#else
#define CREATE_STACK(name) stack_type name = {}
#endif

void DemoAttacks(void);


int main(void) {
    #ifndef DEBUG
    printf("=== NORMAL OPERATION DEMO ===\n");

    stack_elem_t value = POISON;
    CREATE_STACK(st1);

    CHECK_ERROR(StackCtor(&st1, 5));
    CHECK_ERROR(PrintStack(&st1));

    CHECK_ERROR(StackPush(&st1, 10));
    CHECK_ERROR(StackPush(&st1, 20));
    CHECK_ERROR(StackPush(&st1, 30));

    CHECK_ERROR(PrintStack(&st1));

    CHECK_ERROR(StackPop(&st1, &value));
    printf("Popped value: ");
    PRINT_STACK_ELEMENT(BASE, value);

    CHECK_ERROR(StackDtor(&st1));
    printf("Normal operation completed successfully.\n\n");
    #endif

    DemoAttacks();

    return 0;
}

//----------------------------------------------------------------------------------

void DemoAttacks(void) {
    #ifdef DEBUG
    printf("=== DEBUG ATTACKS DEMO ===\n");

    CREATE_STACK(victim);

    if (StackCtor(&victim, 5) != stack_error_t::OK) {
        PrintColor(CYAN, "Failed to create stack for attacks.\n");
        return;
    }

    // Заполняем стек немного данными
    StackPush(&victim, 1);
    StackPush(&victim, 2);
    StackPush(&victim, 3);

    PrintColor(CYAN, "Stack before attack:\n");
    PrintStack(&victim);


    // Атака 1: Повреждение канареек
    // AttackCorruptStructCanaries(&victim);

    // Атака 2: Повреждение хешей
    // AttackCorruptHashes(&victim);

    // Атака 3.1: Установка недопустимых значений ёмкости
    AttackCreateSizeInconsistency1(&victim);

    // Атака 3.2: Установка размера больше ёмкости
    // AttackCreateSizeInconsistency2(&victim);

    // Атака 4: Повреждение канареек данных
    // AttackCorruptDataCanaries(&victim);

    // Атака 5: Комбинированная (самая надежная)
    // AttackCombinedDestruction(&victim);

    // Атака 6: Тихая смерть
    // AttackSilentDeath(&victim);

    PrintColor(CYAN, "Stack after attack - attempting operation...\n");

    // Попытка операции, которая должна вызвать падение
    StackPush(&victim, 666);

    // Если мы дошли сюда, атака не сработала как ожидалось
    PrintColor(CYAN, "Attack failed - program is still alive.\n");
    StackDtor(&victim);

    #else
    printf("Attacks are only available in DEBUG mode.\n");
    #endif
}
