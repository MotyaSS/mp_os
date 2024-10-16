#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "binary_heap.h"
#include "leftist_heap.h"
#include "skew_heap.h"
#include "binomial_heap.h"
#include "fibonacci_heap.h"
#include "cartesian_tree.h"

status_code p_queue_set_null(p_queue* pq)
{
    if (pq == NULL)
    {
        return NULL_ARG;
    }
    pq->ds_size = 0;
    pq->ds = NULL;
    pq->set_null = NULL;
    pq->construct = NULL;
    pq->copy = NULL;
    pq->destruct = NULL;
    pq->meld = NULL;
    pq->copy_meld = NULL;
    pq->size = NULL;
    pq->top = NULL;
    pq->pop = NULL;
    pq->insert = NULL;
    return OK;
}

status_code p_queue_construct(p_queue* pq, pq_base_t base, int (*compare)(const request*, const request*))
{
    if (pq == NULL)
    {
        return NULL_ARG;
    }
    
    pq->base = base;
    pq->ds = NULL;
    
    switch (base)
    {
        case PQB_BINARY: // omg rename functions
        {
            pq->ds_size = sizeof(Binary_heap);
            pq->ds = malloc(sizeof(Binary_heap));
            
            pq->set_null 	= (status_code (*)(void*)) 											bin_heap_set_null;
            pq->construct 	= (status_code (*)(void*, int (*)(const request*, const request*))) bin_heap_construct;
            pq->copy 		= (status_code (*)(void*, const void*)) 							bin_heap_copy;
            pq->destruct 	= (status_code (*)(void*)) 											bin_heap_destruct;
            pq->meld 		= (status_code (*)(void*, void*, void*)) 							bin_heap_meld;
            pq->copy_meld 	= (status_code (*)(void*, const void*, const void*))                bin_heap_copy_meld;
            pq->size 		= (status_code (*)(void*, size_t*))                                 bin_heap_size;
            pq->top 		= (status_code (*)(void*, request**))                               bin_heap_top;
            pq->pop 		= (status_code (*)(void*, request**))                               bin_heap_pop;
            pq->insert 		= (status_code (*)(void*, const request*))                          bin_heap_insert;
            break;
        }
        case PQB_LEFTIST:
        {
            pq->ds_size = sizeof(Leftist_heap);
            pq->ds = malloc(sizeof(Leftist_heap));
            
            pq->set_null 	= (status_code (*)(void*)) 											lft_heap_set_null;
            pq->construct 	= (status_code (*)(void*, int (*)(const request*, const request*))) lft_heap_construct;
            pq->copy 		= (status_code (*)(void*, const void*)) 							lft_heap_copy;
            pq->destruct 	= (status_code (*)(void*)) 											lft_heap_destruct;
            pq->meld 		= (status_code (*)(void*, void*, void*)) 							lft_heap_meld;
            pq->copy_meld 	= (status_code (*)(void*, const void*, const void*))                lft_heap_copy_meld;
            pq->size 		= (status_code (*)(void*, size_t*))                                 lft_heap_size;
            pq->top 		= (status_code (*)(void*, request**))                               lft_heap_top;
            pq->pop 		= (status_code (*)(void*, request**))                               lft_heap_pop;
            pq->insert 		= (status_code (*)(void*, const request*))                          lft_heap_insert;
            break;
        }
        case PQB_SKEW:
        {
            pq->ds_size = sizeof(Skew_heap);
            pq->ds = malloc(sizeof(Skew_heap));
            
            pq->set_null 	= (status_code (*)(void*)) 											skw_heap_set_null;
            pq->construct 	= (status_code (*)(void*, int (*)(const request*, const request*))) skw_heap_construct;
            pq->copy 		= (status_code (*)(void*, const void*)) 							skw_heap_copy;
            pq->destruct 	= (status_code (*)(void*)) 											skw_heap_destruct;
            pq->meld 		= (status_code (*)(void*, void*, void*)) 							skw_heap_meld;
            pq->copy_meld 	= (status_code (*)(void*, const void*, const void*))                skw_heap_copy_meld;
            pq->size 		= (status_code (*)(void*, size_t*))                                 skw_heap_size;
            pq->top 		= (status_code (*)(void*, request**))                               skw_heap_top;
            pq->pop 		= (status_code (*)(void*, request**))                               skw_heap_pop;
            pq->insert 		= (status_code (*)(void*, const request*))                          skw_heap_insert;
            break;
        }
        case PQB_BINOM:
        {
            pq->ds_size = sizeof(Binomial_heap);
            pq->ds = malloc(sizeof(Binomial_heap));
            
            pq->set_null    = (status_code (*)(void*))                                          bm_heap_set_null;
            pq->construct   = (status_code (*)(void*, int (*)(const request*, const request*))) bm_heap_construct;
            pq->copy        = (status_code (*)(void*, const void*))                             bm_heap_copy;
            pq->destruct    = (status_code (*)(void*))                                          bm_heap_destruct;
            pq->meld        = (status_code (*)(void*, void*, void*))                            bm_heap_meld;
            pq->copy_meld   = (status_code (*)(void*, const void*, const void*))                bm_heap_copy_meld;
            pq->size        = (status_code (*)(void*, size_t*))                                 bm_heap_size;
            pq->top         = (status_code (*)(void*, request**))                               bm_heap_top;
            pq->pop         = (status_code (*)(void*, request**))                               bm_heap_pop;
            pq->insert      = (status_code (*)(void*, const request*))                          bm_heap_insert;
            break;
        }
        case PQB_FIB:
        {
            pq->ds_size = sizeof(Fibonacci_heap);
            pq->ds = malloc(sizeof(Fibonacci_heap));
            
            pq->set_null    = (status_code (*)(void*))                                          fib_heap_set_null;
            pq->construct   = (status_code (*)(void*, int (*)(const request*, const request*))) fib_heap_construct;
            pq->copy        = (status_code (*)(void*, const void*))                             fib_heap_copy;
            pq->destruct    = (status_code (*)(void*))                                          fib_heap_destruct;
            pq->meld        = (status_code (*)(void*, void*, void*))                            fib_heap_meld;
            pq->copy_meld   = (status_code (*)(void*, const void*, const void*))                fib_heap_copy_meld;
            pq->size        = (status_code (*)(void*, size_t*))                                 fib_heap_size;
            pq->top         = (status_code (*)(void*, request**))                               fib_heap_top;
            pq->pop         = (status_code (*)(void*, request**))                               fib_heap_pop;
            pq->insert      = (status_code (*)(void*, const request*))                          fib_heap_insert;
            break;
        }
        case PQB_TREAP:
        {
            pq->ds_size = sizeof(Cartesian_tree);
            pq->ds = malloc(sizeof(Cartesian_tree));
            
            pq->set_null    = (status_code (*)(void*))                                          treap_set_null;
            pq->construct   = (status_code (*)(void*, int (*)(const request*, const request*))) treap_construct;
            pq->copy        = (status_code (*)(void*, const void*))                             treap_copy;
            pq->destruct    = (status_code (*)(void*))                                          treap_destruct;
            pq->meld        = (status_code (*)(void*, void*, void*))                            treap_meld;
            pq->copy_meld   = (status_code (*)(void*, const void*, const void*))                treap_copy_meld;
            pq->size        = (status_code (*)(void*, size_t*))                                 treap_size;
            pq->top         = (status_code (*)(void*, request**))                               treap_top;
            pq->pop         = (status_code (*)(void*, request**))                               treap_pop;
            pq->insert      = (status_code (*)(void*, const request*))                          treap_insert;
            break;
        }
    }
    if (pq->ds == NULL)
    {
        p_queue_set_null(pq);
        return BAD_ALLOC;
    }
    
    pq->set_null(pq->ds);
    status_code code = pq->construct(pq->ds, compare);
    
    if (code)
    {
        free(pq->ds);
        p_queue_set_null(pq);
        return code;
    }
    
    return OK;
}

status_code p_queue_copy(p_queue* pq_dest, const p_queue* pq_src)
{
    if (pq_dest == NULL || pq_src == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    *pq_dest = *pq_src;
    
    pq_dest->ds = malloc(pq_src->ds_size);
    if (pq_dest-> ds == NULL)
    {
        code = BAD_ALLOC;
    }
    
    code = code ? code : pq_src->copy(pq_dest->ds, pq_src->ds);
    
    if (code)
    {
        p_queue_destruct(pq_dest);
        return code;
    }
    
    return OK;
}

status_code p_queue_destruct(p_queue* pq)
{
    if (pq == NULL)
    {
        return OK;
    }
    if (pq->ds != NULL)
    {
        pq->destruct(pq->ds);
        free(pq->ds);
    }
    p_queue_set_null(pq);
    return OK;
}


status_code p_queue_meld(p_queue* pq_res, p_queue* pq_l, p_queue* pq_r)
{
    if (pq_res == NULL || pq_l == NULL || pq_r == NULL)
    {
        return NULL_ARG;
    }
    if (pq_l->base != pq_r->base)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    p_queue pq_tmp;
    
    if (pq_res == pq_l || pq_res == pq_r)
    {
        pq_tmp = pq_res == pq_l ? *pq_l : *pq_r;
    }
    else
    {
        pq_tmp = *pq_l;
        pq_tmp.ds = malloc(pq_l->ds_size);
        if (pq_tmp.ds == NULL)
        {
            code = BAD_ALLOC;
        }
    }
    
    code = code ? code : pq_l->meld(pq_tmp.ds, pq_l->ds, pq_r->ds);
    
    if (code)
    {
        if (pq_res != pq_l && pq_res != pq_r)
        {
            p_queue_destruct(&pq_tmp);
        }
        return code;
    }
    
    *pq_res = pq_tmp;
    return OK;
}

status_code p_queue_copy_meld(p_queue* pq_res, const p_queue* pq_l, const p_queue* pq_r)
{
    if (pq_res == NULL || pq_l == NULL || pq_r == NULL)
    {
        return NULL_ARG;
    }
    if (pq_l->base != pq_r->base)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    p_queue pq_tmp = *pq_l;
    
    pq_tmp.ds = malloc(pq_l->ds_size);
    if (pq_tmp.ds == NULL)
    {
        code = BAD_ALLOC;
    }	
    
    code = code ? code : pq_l->copy_meld(pq_tmp.ds, pq_l->ds, pq_r->ds);
    if (code)
    {
        p_queue_destruct(&pq_tmp);
        return code;
    }
    
    *pq_res = pq_tmp;
    return OK;
}


status_code p_queue_size(p_queue* pq, size_t* size)
{
    if (pq == NULL || size == NULL)
    {
        return NULL_ARG;
    }
    return pq->size(pq->ds, size);
}

status_code p_queue_top(p_queue* pq, request** req)
{
    if (pq == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    return pq->top(pq->ds, req);
}

status_code p_queue_pop(p_queue* pq, request** req)
{
    if (pq == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    return pq->pop(pq->ds, req);
}

status_code p_queue_insert(p_queue* pq, request* req)
{
    if (pq == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    return pq->insert(pq->ds, req);
}


int compare_request(const request* lhs, const request* rhs)
{
    if (lhs->prior == rhs->prior)
    {
        return strcmp(lhs->time, rhs->time);
    }
    // OR MAYBE rhs->prior - lhs->prior;
    return lhs->prior > rhs->prior ? -1 : 1;
}
