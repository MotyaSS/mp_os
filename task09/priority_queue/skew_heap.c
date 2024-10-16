#include <stdlib.h>
#include <string.h>

#include "skew_heap.h"

void skw_node_destruct(skw_node* node)
{
    if (node == NULL)
    {
        return;
    }
    
    skw_node_destruct(node->right);
    skw_node_destruct(node->left);
    
    request_destruct(node->req);
    free_all(2, node->req, node);
}

status_code skw_node_copy(skw_node** result, const skw_node* src)
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
    
    skw_node* node = (skw_node*) malloc(sizeof(skw_node));
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
    
    node->right = NULL;
    node->left = NULL;
    code = code ? code : skw_node_copy(&node->left, src->left);
    code = code ? code : skw_node_copy(&node->right, src->right);
    if (code)
    {
        skw_node_destruct(node);
        return code;
    }
    
    *result = node;
    
    return OK;
}

skw_node* skw_node_merge(skw_node* a, skw_node* b, int (*compare)(const request*, const request*))
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
        skw_node* temp = a;
        a = b;
        b = temp;
    }
    
    a->left = skw_node_merge(b, a->left, compare);
    
    skw_node* temp = a->left;
    a->left = a->right;
    a->right = temp;
    
    return a;
}


status_code skw_heap_set_null(skw_heap* skw)
{
    if (skw == NULL)
    {
        return NULL_ARG;
    }
    
    skw->head = NULL;
    skw->size = 0;
    skw->compare = NULL;
    
    return OK;
}

status_code skw_heap_construct(skw_heap* skw, int (*compare)(const request*, const request*))
{
    if (skw == NULL || compare == NULL)
    {
        return NULL_ARG;
    }
    
    skw_heap_set_null(skw);
    skw->compare = compare;
    
    return OK;
}

status_code skw_heap_copy(skw_heap* skw_dest, const skw_heap* skw_src)
{
    if (skw_dest == NULL || skw_src == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = skw_node_copy(&skw_dest->head, skw_src->head);
    if (code)
    {
        skw_heap_destruct(skw_dest);
        return code;
    }
    
    skw_dest->size = skw_src->size;
    skw_dest->compare = skw_src->compare;
    
    return OK;
}

status_code skw_heap_destruct(skw_heap* skw)
{
    if (skw == NULL)
    {
        return OK;
    }
    
    skw_node_destruct(skw->head);
    skw_heap_set_null(skw);
    
    return OK;
}


status_code skw_heap_meld(skw_heap* skw_res, skw_heap* skw_l, skw_heap* skw_r)
{
    if (skw_res == NULL || skw_l == NULL || skw_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (skw_l == skw_r || skw_l->compare != skw_r->compare)
    {
        return INVALID_INPUT;
    }
    
    skw_heap skw_tmp;
    
    skw_tmp.head = skw_node_merge(skw_r->head, skw_l->head, skw_l->compare);
    skw_tmp.size = skw_l->size + skw_r->size;
    skw_tmp.compare = skw_l->compare;
    
    skw_l->head = skw_r->head = NULL;
    skw_l->size = skw_r->size = 0;
    
    *skw_res = skw_tmp;
    
    return OK;
}

status_code skw_heap_copy_meld(skw_heap* skw_res, const skw_heap* skw_l, const skw_heap* skw_r)
{
    if (skw_res == NULL || skw_l == NULL || skw_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (skw_res == skw_l || skw_res == skw_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    skw_heap skw_lc, skw_rc;
        
    skw_heap_set_null(&skw_lc);
    skw_heap_set_null(&skw_rc);
    skw_heap_set_null(skw_res);
    
    code = code ? code : skw_heap_copy(&skw_lc, skw_l);
    code = code ? code : skw_heap_copy(&skw_rc, skw_r);
    code = code ? code : skw_heap_meld(skw_res, &skw_lc, &skw_rc);
    
    skw_heap_destruct(&skw_lc);
    skw_heap_destruct(&skw_rc);
    
    if (code)
    {
        skw_heap_destruct(skw_res);
        return code;
    }
    
    return OK;
}

status_code skw_heap_size(const skw_heap* skw, size_t* size)
{
    if (skw == NULL)
    {
        return NULL_ARG;
    }
    (*size) = skw->size;
    return OK;
}

status_code skw_heap_top(const skw_heap* skw, request** req)
{
    if (skw == NULL)
    {
        return NULL_ARG;
    }
    
    if (skw->head == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    *req = skw->head->req;
    
    return OK;
}

status_code skw_heap_pop(skw_heap* skw, request** req)
{
    if (skw == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    if (skw->head == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    skw_node* old_head= skw->head;
    *req = skw->head->req;
    skw->head = skw_node_merge(skw->head->right, skw->head->left, skw->compare);
    
    free(old_head);
    skw->size--;
    
    return OK;
}

status_code skw_heap_insert(skw_heap* skw, request* req)
{
    if (skw == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    
    
    skw_node* new_node = (skw_node*) malloc(sizeof(skw_node));
    if (new_node == NULL)
    {
        return BAD_ALLOC;
    }

    new_node->req = req;
    new_node->right = NULL;
    new_node->left = NULL;
    
    skw_heap skw_add;
    skw_add.head = new_node;
    skw_add.size = 1;
    skw_add.compare = skw->compare;
    
    return skw_heap_meld(skw, skw, &skw_add);
}
