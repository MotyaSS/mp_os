#include <stdlib.h>
#include <string.h>

#include <../include/operator_stack.h>

status_code operator_stack_set_null(Operator_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    st->top = NULL;
    st->size = 0;
    return OK;
}

status_code operator_stack_destruct(Operator_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    operator_stack_node* cur = st->top;
    while (cur != NULL)
    {
        operator_stack_node* nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    
    st->top = NULL;
    st->size = 0;
    
    return OK;
}


status_code operator_stack_top(Operator_stack* st, char* oper)
{
    if (st == NULL || oper == NULL)
    {
        return NULL_ARG;
    }
    
    if (st->top == NULL)
    {
        return BAD_ACCESS;
    }
    
    *oper = st->top->oper;
    return OK;
}

status_code operator_stack_is_empty(Operator_stack* st, int* is_empty)
{
    if (st == NULL || is_empty == NULL)
    {
        return NULL_ARG;
    }
    *is_empty = st->top == NULL;
    return OK;
}

status_code operator_stack_size(Operator_stack* st, ull* size)
{
    if (st == NULL || size == NULL)
    {
        return NULL_ARG;
    }
    *size = st->size;
    return OK;
}

status_code operator_stack_push(Operator_stack* st, char oper)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    operator_stack_node* node = (operator_stack_node*) malloc(sizeof(operator_stack_node));
    if (node == NULL)
    {
        return BAD_ALLOC;
    }
    
    node->oper = oper;
    node->next = st->top;
    st->top = node;
    ++st->size;
    
    return OK;
}

status_code operator_stack_pop(Operator_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    if (st->top == NULL)
    {
        return BAD_ACCESS;
    }
    
    operator_stack_node* tmp = st->top;
    st->top = st->top->next;
    --st->size;
    
    free(tmp);
    
    return OK;
}
