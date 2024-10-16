#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "../utility.h"

typedef enum priority_queue_base_t
{
    PQB_BINARY,
    PQB_LEFTIST,
    PQB_SKEW,
    PQB_BINOM,
    PQB_FIB,
    PQB_TREAP,
} priority_queue_base_t, pq_base_t;

// Priority queue contains POINTERS to requests (NOT A COPIES)
typedef struct Priority_queue
{
    pq_base_t base;
    size_t ds_size;
    void* ds;
    status_code (*set_null)(void* ds);
    status_code (*construct)(void* ds, int (*compare)(const request*, const request*));
    status_code (*copy)(void* ds_dest, const void* ds_src);
    status_code (*destruct)(void* ds);
    status_code (*meld)(void* ds_res, void* ds_l, void* ds_r);
    status_code (*copy_meld)(void* ds_res, const void* ds_l, const void* ds_r);
    status_code (*size)(void* ds, size_t* size);
    status_code (*top)(void* ds, request** req);
    status_code (*pop)(void* ds, request** req);
    status_code (*insert)(void* ds, const request* req);
} Priority_queue, p_queue;

status_code p_queue_set_null(p_queue* pq);
status_code p_queue_construct(p_queue* pq, pq_base_t base, int (*compare)(const request*, const request*));
status_code p_queue_copy(p_queue* pq_dest, const p_queue* pq_src);
status_code p_queue_destruct(p_queue* pq);

status_code p_queue_meld(p_queue* pq_res, p_queue* pq_l, p_queue* pq_r);
status_code p_queue_copy_meld(p_queue* pq_res, const p_queue* pq_l, const p_queue* pq_r);

status_code p_queue_size(p_queue* pq, size_t* size);
status_code p_queue_top(p_queue* pq, request** req);
status_code p_queue_pop(p_queue* pq, request** req);
status_code p_queue_insert(p_queue* pq, request* req);

int compare_request(const request* lhs, const request* rhs);

#endif // _PRIORITY_QUEUE_H_