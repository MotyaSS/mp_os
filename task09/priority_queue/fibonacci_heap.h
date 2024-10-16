#ifndef _FIBONACCI_HEAP_H_
#define _FIBONACCI_HEAP_H_

#include "../utility.h"

typedef struct fib_node {
	request* req;
	unsigned int rank;

    struct fib_node *child;
    struct fib_node *left;
	struct fib_node *right;
} fib_node;

typedef struct Fibonacci_heap
{
	fib_node* head;
	size_t size;
	int (*compare)(const request*, const request*);
} Fibonacci_heap, fib_heap;


status_code fib_heap_set_null(fib_heap* heap);
status_code fib_heap_construct(fib_heap* heap, int (*compare)(const request*, const request*));
status_code fib_heap_copy(fib_heap* dest, const fib_heap* src);
status_code fib_heap_destruct(fib_heap* heap);

status_code fib_heap_meld(fib_heap* res, fib_heap* heap_l, fib_heap* heap_r);
status_code fib_heap_copy_meld(fib_heap* res, const fib_heap* heap_l, const fib_heap* heap_r);

status_code fib_heap_size(const fib_heap* heap, size_t* size);
status_code fib_heap_top(const fib_heap* heap, request** req);
status_code fib_heap_pop(fib_heap* heap, request** req);
status_code fib_heap_insert(fib_heap* heap, request* req);

#endif // _FIBONACCI_HEAP_H_