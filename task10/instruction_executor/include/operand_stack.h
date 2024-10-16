#ifndef _OPERAND_STACK_H_
#define _OPERAND_STACK_H_

#include <utility.h>

typedef struct operand_stack_node
{
    //char* operand;
    uint32_t operand;
    struct operand_stack_node* next;
} operand_stack_node;

typedef struct
{
    operand_stack_node* top;
    ull size;
} Operand_stack;

status_code operand_stack_set_null(Operand_stack* st);
status_code operand_stack_destruct(Operand_stack* st);

status_code operand_stack_top(Operand_stack* st, uint32_t* operand);
status_code operand_stack_is_empty(Operand_stack* st, int* is_empty);
status_code operand_stack_size(Operand_stack* st, ull* size);
status_code operand_stack_push(Operand_stack* st, uint32_t operand);
status_code operand_stack_pop(Operand_stack* st);

#endif // _OPERAND_STACK_H_