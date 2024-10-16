#include <string.h>

#include "binary_heap.h"

status_code bin_heap_sift_down(bin_heap* bh, size_t i)
{
    if (bh == NULL)
    {
        return NULL_ARG;
    }
    
    while (2 * i + 1 < bh->size)
    {
        int left_idx = 2 * i + 1;
        int right_idx = 2 * i + 2;
        int j = left_idx;
        if (right_idx < bh->size && (bh->compare(bh->elems[right_idx], bh->elems[left_idx]) < 0))
        {
            j = right_idx;
        }
        if (bh->compare(bh->elems[i], bh->elems[j]) < 0)
        {
            break;
        }
        request* tmp = bh->elems[i];
        bh->elems[i] = bh->elems[j];
        bh->elems[j] = tmp;
        i = j;
    }
    
    return OK;
}

status_code bin_heap_sift_up(bin_heap* bh)
{
    if (bh == NULL)
    {
        return NULL_ARG;
    }
    
    unsigned i = bh->size;
    while (i > 0 && (bh->compare(bh->elems[(i-1) / 2], bh->elems[i]) > 0))
    {
        request* tmp = bh->elems[i];
        bh->elems[i] = bh->elems[(i - 1) / 2];
        bh->elems[(i - 1) / 2] = tmp;
        i = (i - 1) / 2;
    }
    return OK;
}

status_code bin_heap_set_null(bin_heap* bh)
{
    if (bh == NULL)
    {
        return NULL_ARG;
    }
    
    bh->capacity = 0;
    bh->size = 0;
    bh->elems = NULL;
    bh->compare = NULL;
    
    return OK;
}

status_code bin_heap_construct(bin_heap* bh, int (*compare)(const request*, const request*))
{
    if (bh == NULL)
    {
        return NULL_ARG;
    }
    
    bh->elems = (request**) calloc(2, sizeof(request*));
    if (bh->elems == NULL)
    {
        bin_heap_destruct(bh);
        return BAD_ALLOC;
    }
    
    bh->compare = compare;
    bh->capacity = 2;
    bh->size = 0;
    
    return OK;
}

status_code bin_heap_copy(bin_heap* bh_dest, const bin_heap* bh_src)
{
    if (bh_dest == NULL || bh_src == NULL)
    {
        return NULL_ARG;
    }
    
    bh_dest->elems = (request**) calloc(bh_src->capacity, sizeof(request*));
    if (bh_dest->elems == NULL)
    {
        bin_heap_destruct(bh_dest);
        return BAD_ALLOC;
    }
    
    bh_dest->size = 0;
    for (size_t i = 0; i < bh_src->size; ++i)
    {
        bh_dest->elems[i] = (request*) malloc(sizeof(request));
        if (bh_dest->elems[i] == NULL)
        {
            bin_heap_destruct(bh_dest);
            return BAD_ALLOC;
        }
        
        status_code st = request_copy(bh_dest->elems[i], bh_src->elems[i]);
        if (st)
        {
            free(bh_dest->elems[i]);
            bin_heap_destruct(bh_dest);
            return st;
        }
        bh_dest->size++;
    }
    
    bh_dest->size = bh_src->size;
    bh_dest->capacity = bh_src->capacity;
    bh_dest->compare = bh_src->compare;
    
    return OK;
}

status_code bin_heap_destruct(bin_heap* bh)
{
    if (bh == NULL)
    {
        return NULL_ARG;
    }
    
    for (size_t i = 0; i < bh->size; ++i)
    {
        request_destruct(bh->elems[i]);
        free(bh->elems[i]);
    }
    free(bh->elems);
    bin_heap_set_null(bh);
    
    return OK;
}

status_code bin_heap_meld(bin_heap* bh, bin_heap* bh_l, bin_heap* bh_r)
{
    if (bh == NULL || bh_l == NULL || bh_r == NULL)
    {
        return NULL_ARG;
    }
    if (bh_l == bh_r || bh_l->compare != bh_r->compare)
    {
        return INVALID_INPUT;
    }
    
    bin_heap bh_tmp;
    size_t max_capacity = bh_l->capacity > bh_r->capacity ? bh_l->capacity : bh_r->capacity;
    
    if (bh_l->size + bh_r->size > max_capacity)
    {
        max_capacity *= 2;
    }
    
    bh_tmp.size = bh_l->size + bh_r->size;
    bh_tmp.capacity = max_capacity;
    bh_tmp.elems = (request**) malloc(sizeof(request*) * max_capacity);
    bh_tmp.compare = bh_l->compare;
    
    if (bh_tmp.elems == NULL)
    {
        return BAD_ALLOC;
    }
    
    for (size_t i = 0; i < bh_l->size; ++i)
    {
        bh_tmp.elems[i] = bh_l->elems[i];
    }
    for (size_t i = 0; i < bh_r->size; ++i)
    {
        bh_tmp.elems[i + bh_l->size] = bh_r->elems[i];
    }
    
    for (size_t i = 0; i <= bh_tmp.size / 2; ++i)
    {
        bin_heap_sift_down(&bh_tmp, bh_tmp.size / 2 - i);
    }
    
    bh_l->size = 0;
    bh_r->size = 0;
    
    if (bh == bh_l)
    {
        free(bh_r->elems);
    }
    if (bh == bh_r)
    {
        free(bh_r->elems);
    }
    
    *bh = bh_tmp;
    
    return OK;
}

status_code bin_heap_copy_meld(bin_heap* bh, const bin_heap* bh_l, const bin_heap* bh_r)
{
    if (bh == NULL || bh_l == NULL || bh_r == NULL)
    {
        return NULL_ARG;
    }
    
    bin_heap bh_lc, bh_rc, bh_res;
    bin_heap_set_null(&bh_lc);
    bin_heap_set_null(&bh_rc);
    bin_heap_set_null(&bh_res);
    
    status_code st = bin_heap_copy(&bh_lc, bh_l);
    st = st ? st : bin_heap_copy(&bh_rc, bh_r);
    st = st ? st : bin_heap_meld(&bh_res, &bh_lc, &bh_rc);
    
    bin_heap_destruct(&bh_rc);
    bin_heap_destruct(&bh_lc);
    
    if (st)
    {
        bin_heap_destruct(&bh_res);
        return st;
    }
    
    *bh = bh_res;
    
    return OK;
}

status_code bin_heap_size(const bin_heap* bh, size_t* size)
{
    if (size == NULL)
    {
        return NULL_ARG;
    }
    
    *size = bh->size;
    
    return OK;
}

status_code bin_heap_top(const bin_heap* bh, request** req)
{
    if (bh == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    if (bh->size == 0)
    {
        *req = NULL;
        return OK;
    }
    
    *req = bh->elems[0];
    
    return OK;
}

status_code bin_heap_pop(bin_heap* bh, request** req)
{
    if (bh == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    if (bh->size == 0)
    {
        *req = NULL;
        return OK;
    }
    
    *req = bh->elems[0];
    
    bh->elems[0] = bh->elems[bh->size-1];
    bh->size--;
    bin_heap_sift_down(bh, 0);
    
    return OK;
}

status_code bin_heap_insert(bin_heap* bh, request* req)
{
    if (bh == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    if (bh->elems == NULL)
    {
        return UNINITIALIZED_USAGE;
    }
    
    if (bh->size == bh->capacity)
    {
        request** tmp = realloc(bh->elems, sizeof(request*) * (bh->capacity * 2));
        if (tmp == NULL)
        {
            return BAD_ALLOC;
        }
        bh->capacity *= 2;
        bh->elems = tmp;
    }
    
    bh->elems[bh->size] = req;
    bin_heap_sift_up(bh);
    bh->size++;
    
    return OK;
}
