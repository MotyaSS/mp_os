#ifndef _LEFTIST_HEAP_H_
#define _LEFTIST_HEAP_H_

#include "../utility.h"

typedef struct leftist_node
{
    request* req;
    int distance;
    struct leftist_node* left;
    struct leftist_node* right;
} leftist_node, lft_node;

typedef struct Leftist_heap
{
    leftist_node* head;
    unsigned int size;
    int (*compare)(const request*, const request*);
} Leftist_heap, lft_heap;

status_code lft_heap_set_null(lft_heap* lft);
status_code lft_heap_construct(lft_heap* lft, int (*compare)(const request*, const request*));
status_code lft_heap_copy(lft_heap* lft_dest, const lft_heap* lft_src);
status_code lft_heap_destruct(lft_heap* lft);

status_code lft_heap_meld(lft_heap* lft_res, lft_heap* lft_l, lft_heap* lft_r);
status_code lft_heap_copy_meld(lft_heap* lft_res, const lft_heap* lft_l, const lft_heap* lft_r);

status_code lft_heap_size(const lft_heap* lft, size_t* size);
status_code lft_heap_top(const lft_heap* lft, request** req);
status_code lft_heap_pop(lft_heap* lft, request** req);
status_code lft_heap_insert(lft_heap* lft, request* req);

#endif // _LEFTIST_HEAP_H_