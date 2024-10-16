#include <stdlib.h>
#include <string.h>

#include "leftist_heap.h"

void lft_node_destruct(lft_node* node)
{
    if (node == NULL)
    {
        return;
    }
        
    lft_node_destruct(node->left);
    lft_node_destruct(node->right);
    
    request_destruct(node->req);
    free_all(2, node->req, node);
}

status_code lft_node_copy(lft_node** result, const lft_node* src)
{
    if (result == NULL)
    {
        return NULL_ARG;
    }
    
    if (src == NULL)
    {
        *result = NULL;
        return OK;
    }
    
    lft_node* node = (lft_node*) malloc(sizeof(lft_node));
    if (node == NULL)
    {
        return BAD_ALLOC;
    }
    
    node->req = (request*) malloc(sizeof(request));
    if (node->req == NULL)
    {
        free(node);
        return BAD_ALLOC;
    }
    
    status_code code = request_copy(node->req, src->req);
    if (code)
    {
        free_all(2, node->req, node);
        return BAD_ALLOC;
    }
    
    node->distance = src->distance;
    node->left = node->right = NULL;
    
    code = code ? code : lft_node_copy(&node->right, src->right);
    code = code ? code : lft_node_copy(&node->left, src->left);
    if (code)
    {
        lft_node_destruct(node);
        return code;
    }
    
    *result = node;
    
    return OK;
}

lft_node* lft_node_merge(lft_node* a, lft_node* b, int (*compare)(const request*, const request*))
{
    if (compare == NULL || (a == NULL && b == NULL))
    {
        return NULL;
    }
    
    if (a == NULL)
    {
        return b;
    }
    if (b == NULL)
    {
        return a;
    }
    
    if (compare(a->req, b->req) > 0)
    {
        lft_node* temp = a;
        a = b;
        b = temp;
    }
    
    a->right = lft_node_merge(a->right, b, compare);
    
    if (a->right != NULL && (a->left == NULL || a->left->distance < a->right->distance))
    {
        lft_node* temp = a->left;
        a->left = a->right;
        a->right = temp;
    }
    
    a->distance = a->right == NULL ? 0 : a->right->distance + 1;

    return a;
}


status_code lft_heap_set_null(lft_heap* lft)
{
    if (lft == NULL)
    {
        return NULL_ARG;
    }
    
    lft->head = NULL;
    lft->size = 0;
    lft->compare = NULL;
    
    return OK;
}

status_code lft_heap_construct(lft_heap* lft, int (*compare)(const request*, const request*))
{
    if (lft == NULL || compare == NULL)
    {
        return NULL_ARG;
    }
    
    lft_heap_set_null(lft);
    lft->compare = compare;
    
    return OK;
}

status_code lft_heap_copy(lft_heap* lft_dest, const lft_heap* lft_src)
{
    if (lft_dest == NULL || lft_src == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = lft_node_copy(&lft_dest->head, lft_src->head);
    if (code)
    {
        lft_heap_destruct(lft_dest);
        return code;
    }
    
    lft_dest->size = lft_src->size;
    lft_dest->compare = lft_src->compare;
    
    return OK;
}

status_code lft_heap_destruct(lft_heap* lft)
{
    if (lft == NULL)
    {
        return OK;
    }
    
    lft_node_destruct(lft->head);
    lft_heap_set_null(lft);
    
    return OK;
}


status_code lft_heap_meld(lft_heap* lft_res, lft_heap* lft_l, lft_heap* lft_r)
{
    if (lft_res == NULL || lft_l == NULL || lft_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (lft_l == lft_r || lft_l->compare != lft_r->compare)
    {
        return INVALID_INPUT;
    }
    
    lft_heap lft_tmp;
    
    lft_tmp.head = lft_node_merge(lft_r->head, lft_l->head, lft_l->compare);
    lft_tmp.size = lft_l->size + lft_r->size;
    lft_tmp.compare = lft_l->compare;
    
    lft_l->head = lft_r->head = NULL;
    lft_l->size = lft_r->size = 0;
    
    *lft_res = lft_tmp;
    
    return OK;
}

status_code lft_heap_copy_meld(lft_heap* lft_res, const lft_heap* lft_l, const lft_heap* lft_r)
{
    if (lft_res == NULL || lft_l == NULL || lft_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (lft_res == lft_l || lft_res == lft_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    lft_heap lft_lc, lft_rc;
        
    lft_heap_set_null(&lft_lc);
    lft_heap_set_null(&lft_rc);
    lft_heap_set_null(lft_res);
    
    code = code ? code : lft_heap_copy(&lft_lc, lft_l);
    code = code ? code : lft_heap_copy(&lft_rc, lft_r);
    code = code ? code : lft_heap_meld(lft_res, &lft_lc, &lft_rc);
    
    lft_heap_destruct(&lft_lc);
    lft_heap_destruct(&lft_rc);
    
    if (code)
    {
        lft_heap_destruct(lft_res);
        return code;
    }
    
    return OK;
}


status_code lft_heap_size(const lft_heap* lft, size_t* size)
{
    if (lft == NULL)
    {
        return NULL_ARG;
    }
    (*size) = lft->size;
    return OK;
}

status_code lft_heap_top(const lft_heap* lft, request** req)
{
    if (lft == NULL)
    {
        return NULL_ARG;
    }
    
    if (lft->head == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    *req = lft->head->req;
    
    return OK;
}

status_code lft_heap_pop(lft_heap* lft, request** req)
{
    if (lft == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    if (lft->head == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    lft_node* old_head= lft->head;
    *req = lft->head->req;
    lft->head = lft_node_merge(lft->head->right, lft->head->left, lft->compare);
    
    free(old_head);
    lft->size--;
    
    return OK;
}

status_code lft_heap_insert(lft_heap* lft, request* req)
{
    if (lft == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    lft_node* new_node = (lft_node*) malloc(sizeof(lft_node));
    if (new_node == NULL)
    {
        return BAD_ALLOC;
    }

    new_node->distance = 0;
    new_node->req = req;
    new_node->right = NULL;
    new_node->left = NULL;
    
    lft_heap lft_add;
    lft_add.head = new_node;
    lft_add.size = 1;
    lft_add.compare = lft->compare;
    
    return lft_heap_meld(lft, lft, &lft_add);
}
