#include <stdlib.h>
#include <string.h>

#include <../include/operand_stack.h>

status_code operand_stack_set_null(Operand_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    st->top = NULL;
    st->size = 0;
    return OK;
}

status_code operand_stack_destruct(Operand_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    operand_stack_node* cur = st->top;
    while (cur != NULL)
    {
        operand_stack_node* nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    
    st->top = NULL;
    st->size = 0;
    
    return OK;
}


status_code operand_stack_top(Operand_stack* st, uint32_t* operand)
{
    if (st == NULL || operand == NULL)
    {
        return NULL_ARG;
    }
    
    if (st->top == NULL)
    {
        return BAD_ACCESS;
    }
    
    *operand = st->top->operand;
    return OK;
}

status_code operand_stack_is_empty(Operand_stack* st, int* is_empty)
{
    if (st == NULL || is_empty == NULL)
    {
        return NULL_ARG;
    }
    *is_empty = st->top == NULL;
    return OK;
}

status_code operand_stack_size(Operand_stack* st, ull* size)
{
    if (st == NULL || size == NULL)
    {
        return NULL_ARG;
    }
    *size = st->size;
    return OK;
}

status_code operand_stack_push(Operand_stack* st, uint32_t operand)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    operand_stack_node* node = (operand_stack_node*) malloc(sizeof(operand_stack_node));
    if (node == NULL)
    {
        return BAD_ALLOC;
    }
    
    node->operand = operand;
    node->next = st->top;
    st->top = node;
    ++st->size;
    
    return OK;
}

// status_code operand_stack_push_copy(Operand_stack* st, const char* operand)
// {
//     if (st == NULL || operand == NULL)
//     {
//         return NULL_ARG;
//     }
    
//     operand_stack_node* node = (operand_stack_node*) malloc(sizeof(operand_stack_node));
//     if (node == NULL)
//     {
//         return BAD_ALLOC;
//     }
    
//     node->operand = (char*) malloc(sizeof(char) * (strlen(operand) + 1));
//     if (node->operand == NULL)
//     {
//         free(node);
//         return BAD_ALLOC;
//     }
    
//     strcpy(node->operand, operand);
//     node->next = st->top;
//     st->top = node;
//     ++st->size;
    
//     return OK;
// }

status_code operand_stack_pop(Operand_stack* st)
{
    if (st == NULL)
    {
        return NULL_ARG;
    }
    
    if (st->top == NULL)
    {
        return BAD_ACCESS;
    }
    
    operand_stack_node* tmp = st->top;
    st->top = st->top->next;
    --st->size;
    
    free(tmp);
    
    return OK;
}
