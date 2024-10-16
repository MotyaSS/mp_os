#ifndef _BINARY_HEAP_H_
#define _BINARY_HEAP_H_

#include <stdlib.h>

#include "../utility.h"

typedef struct Binary_heap
{
    size_t size;
    size_t capacity;
    request** elems;
    int (*compare)(const request*, const request*);
} Binary_heap, bin_heap;

status_code bin_heap_set_null(bin_heap* bh);
status_code bin_heap_construct(bin_heap* bh, int (*compare)(const request*, const request*));
status_code bin_heap_copy(bin_heap* bh_dest, const bin_heap* bh_src);
status_code bin_heap_destruct(bin_heap* bh);

status_code bin_heap_meld(bin_heap* bh, bin_heap* bh_l, bin_heap* bh_r);
status_code bin_heap_copy_meld(bin_heap* bh, const bin_heap* bh_l, const bin_heap* bh_r);

status_code bin_heap_size(const bin_heap* bh, size_t* size);
status_code bin_heap_top(const bin_heap* bh, request** req);
status_code bin_heap_pop(bin_heap* bh, request** req);
status_code bin_heap_insert(bin_heap* bh, request* req);

#endif //_BINARY_HEAP_H_