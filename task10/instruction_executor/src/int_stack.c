#include <stdlib.h>
#include <string.h>

#include <../include/int_stack.h>

status_code int_stack_set_null(Int_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    st->top = NULL;
    return OK;
}

status_code int_stack_destruct(Int_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    int_stack_node* cur = st->top;
    while (cur != NULL)
    {
        int_stack_node* nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    
    return OK;
}


status_code int_stack_top(Int_stack* st, int* val)
{
    if (st == NULL || val == NULL)
    {
        return NULL_ARG;
    }
    
    if (st->top == NULL)
    {
        return BAD_ACCESS;
    }
    
    *val = st->top->val;
    return OK;
}

status_code int_stack_is_empty(Int_stack* st, int* is_empty)
{
    if (st == NULL || is_empty == NULL)
    {
        return NULL_ARG;
    }
    *is_empty = st->top == NULL;
    return OK;
}

status_code int_stack_push(Int_stack* st, int val)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    int_stack_node* node = (int_stack_node*) malloc(sizeof(int_stack_node));
    if (node == NULL)
    {
        return BAD_ALLOC;
    }
    
    node->val = val;
    node->next = st->top;
    st->top = node;
    
    return OK;
}

status_code int_stack_pop(Int_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    if (st->top == NULL)
    {
        return BAD_ACCESS;
    }
    
    int_stack_node* tmp = st->top;
    st->top = st->top->next;
    
    free(tmp);
    
    return OK;
}
