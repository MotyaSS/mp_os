#ifndef _INT_STACK_H_
#define _INT_STACK_H_

#include <utility.h>

typedef struct int_stack_node
{
    int val;
    struct int_stack_node* next;
} int_stack_node;

typedef struct
{
    int_stack_node* top;
} Int_stack;

status_code int_stack_set_null(Int_stack* st);
status_code int_stack_destruct(Int_stack* st);

status_code int_stack_top(Int_stack* st, int* val);
status_code int_stack_is_empty(Int_stack* st, int* is_empty);
status_code int_stack_push(Int_stack* st, int val);
status_code int_stack_pop(Int_stack* st);

#endif // _INT_STACK_H_