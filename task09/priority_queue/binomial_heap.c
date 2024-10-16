#include <stdlib.h>
#include <string.h>

#include "binomial_heap.h"

void bmh_node_destruct(bmh_node* node)
{
    if (node == NULL)
    {
        return;
    }
    
    bmh_node_destruct(node->son);
    bmh_node_destruct(node->brother);
    request_destruct(node->req);
    free_all(2, node->req, node);
}

status_code bmh_node_copy(bmh_node** node_dest, const bmh_node* node_src)
{
    if (node_dest == NULL)
    {
        return NULL_ARG;
    }
    if (node_src == NULL)
    {
        *node_dest = NULL;
        return OK;
    }
    
    *node_dest = NULL;
    bmh_node* tmp_node = (bmh_node*) malloc(sizeof(bmh_node));
    if (tmp_node == NULL)
    {
        return BAD_ALLOC;
    }
    
    tmp_node->req = (request*) malloc(sizeof(request));
    if (tmp_node->req == NULL)
    {
        free(tmp_node);
        return BAD_ALLOC;
    }
    
    status_code code = request_copy(tmp_node->req, node_src->req);
    if (code)
    {
        free(tmp_node);
        return code;
    }
    
    tmp_node->son = NULL;
    tmp_node->brother = NULL;
    tmp_node->rank = node_src->rank;
    
    code = code ? code : bmh_node_copy(&tmp_node->son, node_src->son);
    code = code ? code : bmh_node_copy(&tmp_node->brother, node_src->brother);
    if (code)
    {
        bmh_node_destruct(tmp_node);
        return code;
    }
    
    *node_dest = tmp_node;
    
    return OK;
}

status_code bm_heap_set_null(bm_heap* bmh)
{
    if (bmh == NULL)
    {
        return NULL_ARG;
    }
    
    bmh->head = NULL;
    bmh->size = 0;
    bmh->compare = NULL;
    
    return OK;
}

status_code bm_heap_construct(bm_heap* bmh, int (*compare)(const request*, const request*))
{
    if (bmh == NULL)
    {
        return NULL_ARG;
    }
    
    bmh->head = NULL;
    bmh->size = 0;
    bmh->compare = compare;
    
    return OK;
}

status_code bm_heap_copy(bm_heap* bmh_dest, const bm_heap* bmh_src)
{
    if (bmh_dest == NULL || bmh_src == NULL)
    {
        return NULL_ARG;
    }
    
    bmh_dest->size = bmh_src->size;
    bmh_dest->compare = bmh_src->compare;
    
    status_code code = bmh_node_copy(&bmh_dest->head, bmh_src->head);
    if (code)
    {
        bm_heap_destruct(bmh_dest);
        return code;
    }
    
    return OK;
}

status_code bm_heap_destruct(bm_heap* bmh)
{
    if (bmh == NULL)
    {
        return OK;
    }
    
    bmh_node_destruct(bmh->head);
    bm_heap_set_null(bmh);
    
    return OK;
}

status_code bm_heap_meld(bm_heap* bmh_res, bm_heap* bmh_l, bm_heap* bmh_r)
{
    if (bmh_res == NULL || bmh_l == NULL || bmh_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (bmh_l == bmh_r || bmh_l->compare != bmh_r->compare)
    {
        return INVALID_INPUT;
    }
    
    bm_heap bmh_tmp;
    bmh_node* iter_l = bmh_l->head;
    bmh_node* iter_r = bmh_r->head;
    
    if (iter_l == NULL && iter_r == NULL)
    {
        bmh_res->head = NULL;
        bmh_res->size = 0;
        bmh_res->compare = bmh_l->compare;
        return OK;
    }
    
    if (iter_l != NULL && (iter_r == NULL || iter_l->rank < iter_r->rank))
    {
        bmh_tmp.head = iter_l;
        iter_l = iter_l->brother;
    }
    else
    {
        bmh_tmp.head = iter_r;
        iter_r = iter_r->brother;
    }
    
    bmh_node* iter_res = bmh_tmp.head;
    while (iter_l != NULL && iter_r != NULL)
    {
        if (iter_l->rank < iter_r->rank)
        {
            iter_res->brother = iter_l;
            iter_l = iter_l->brother;
        }
        else
        {
            iter_res->brother = iter_r;
            iter_r = iter_r->brother;
        }
        iter_res = iter_res->brother;
    }
    
    while (iter_l != NULL)
    {
        iter_res->brother = iter_l;
        iter_l = iter_l->brother;
        iter_res = iter_res->brother;
    }
    
    while (iter_r != NULL)
    {
        iter_res->brother = iter_r;
        iter_r = iter_r->brother;
        iter_res = iter_res->brother;
    }
    
    
    bmh_node* prev_node = NULL;
    bmh_node* cur_node = bmh_tmp.head;
    bmh_node* nxt_node = bmh_tmp.head->brother;
    
    while (nxt_node != NULL)
    {
        bmh_node* tmp_node = nxt_node->brother;
        if (cur_node->rank == nxt_node->rank && (tmp_node == NULL || tmp_node->rank != nxt_node->rank))
        {
            if (bmh_l->compare(cur_node->req, nxt_node->req) <= 0)
            {
                nxt_node->brother = cur_node->son;
                cur_node->son = nxt_node;
            }
            else
            {
                cur_node->brother = nxt_node->son;
                nxt_node->son = cur_node;
                
                if (cur_node == bmh_tmp.head)
                {
                    bmh_tmp.head = nxt_node;
                }
                cur_node = nxt_node;
            }
            if (prev_node != NULL)
            {
                prev_node->brother = cur_node;
            }
            cur_node->rank++;
            cur_node->brother = tmp_node;
            nxt_node = tmp_node;
        }
        else
        {
            prev_node = cur_node;
            cur_node = nxt_node;
            nxt_node = nxt_node->brother;
        }
    }
    
    size_t size = bmh_l->size + bmh_r->size;
    bmh_l->head = bmh_r->head = NULL;
    bmh_l->size = bmh_r->size = 0;
    bmh_res->head = bmh_tmp.head;
    bmh_res->size = size;
    bmh_res->compare = bmh_l->compare;
    
    return OK;
}

status_code bm_heap_copy_meld(bm_heap* bmh_res, const bm_heap* bmh_l, const bm_heap* bmh_r)
{
    if (bmh_res == NULL)
    {
        return NULL_ARG;
    }
    
    if (bmh_res == bmh_l || bmh_res == bmh_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    bm_heap bmh_lc, bmh_rc;
        
    bm_heap_set_null(&bmh_lc);
    bm_heap_set_null(&bmh_rc);
    bm_heap_set_null(bmh_res);
    
    code = code ? code : bm_heap_copy(&bmh_lc, bmh_l);
    code = code ? code : bm_heap_copy(&bmh_rc, bmh_r);
    code = code ? code : bm_heap_meld(bmh_res, &bmh_lc, &bmh_rc);
    
    bm_heap_destruct(&bmh_lc);
    bm_heap_destruct(&bmh_rc);
    
    if (code)
    {
        bm_heap_destruct(bmh_res);
        return code;
    }
    
    return OK;
}

status_code bm_heap_size(const bm_heap* bmh, size_t* size)
{
    if (bmh == NULL)
    {
        return NULL_ARG;
    }
    *size = bmh->size;
    return OK;
}

status_code bm_heap_find(const bm_heap* bmh, bmh_node** prev, bmh_node** node)
{
    if (node == NULL)
    {
        return NULL_ARG;
    }
    if (bmh->head == NULL)
    {
        *node = NULL;
        return OK;
    }
    
    bmh_node* prev_node = NULL;
    bmh_node* target_node = bmh->head;
    bmh_node* cur_node = bmh->head;
    
    while (cur_node->brother != NULL)
    {
        if (bmh->compare(cur_node->brother->req, target_node->req) < 0)
        {
            prev_node = cur_node;
            target_node = cur_node->brother;
        }
        cur_node = cur_node->brother;
    }
    
    if (prev != NULL)
    {
        *prev = prev_node;
    }
    *node = target_node;
    
    return OK;
}

status_code bm_heap_top(const bm_heap* bmh, request** req)
{
    if (bmh == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    bmh_node* node = NULL;
    bm_heap_find(bmh, NULL, &node);
    if (node == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    *req = node->req;
    
    return OK;
}

status_code bm_heap_pop(bm_heap* bmh, request** req)
{
    if (bmh == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    bmh_node* prev = NULL;
    bmh_node* node = NULL;
    bm_heap_find(bmh, &prev, &node);
    if (node == NULL)
    {
        if (req != NULL)
        {
            *req = NULL;
        }
        return OK;
    }
    
    ll tmp_size = 0;
    status_code code = bpow_safely(2, node->rank, &tmp_size);
    size_t node_size = (size_t) tmp_size;
    if (code) 
    {
        return code;
    }
    
    if (req != NULL)
    {
        *req = node->req;
    }
    if (prev != NULL)
    {
        prev->brother = node->brother;
    }
    if (node == bmh->head)
    {
        bmh->head = node->brother;
    }
    
    bm_heap bmh_add;
    bmh_add.head = node->son;
    bmh_add.compare = bmh->compare;
    free(node);
    
    prev = NULL;
    node = bmh_add.head;
    while (node != NULL)
    {
        bmh_node* nxt = node->brother;
        node->brother = prev;
        prev = node;
        node = nxt;
    }
    bmh_add.head = prev;
    
    bmh->size -= node_size;
    bmh_add.size = node_size - 1;
    bm_heap_meld(bmh, bmh, &bmh_add);
    
    return OK;
}

status_code bm_heap_insert(bm_heap* bmh, request* req)
{
    if (bmh == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    bmh_node* node = (bmh_node*) malloc(sizeof(bmh_node));
    if (node == NULL)
    {
        return BAD_ALLOC;
    }
    
    node->req = req;
    node->son = NULL;
    node->brother = NULL;
    node->rank = 0;
    
    bm_heap bmh_add;
    bmh_add.head = node;
    bmh_add.size = 1;
    bmh_add.compare = bmh->compare;
    bm_heap_meld(bmh, bmh, &bmh_add);
    
    return OK;
}
