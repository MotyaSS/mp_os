#ifndef _OPERATOR_STACK_H_
#define _OPERATOR_STACK_H_

#include <utility.h>

typedef struct operator_stack_node
{
    char oper;
    struct operator_stack_node* next;
} operator_stack_node;

typedef struct
{
    operator_stack_node* top;
    ull size;
} Operator_stack;

status_code operator_stack_set_null(Operator_stack* st);
status_code operator_stack_destruct(Operator_stack* st);

status_code operator_stack_top(Operator_stack* st, char* oper);
status_code operator_stack_is_empty(Operator_stack* st, int* is_empty);
status_code operator_stack_size(Operator_stack* st, ull* size);
status_code operator_stack_push(Operator_stack* st, char oper);
status_code operator_stack_pop(Operator_stack* st);

#endif // _OPERATOR_STACK_H_