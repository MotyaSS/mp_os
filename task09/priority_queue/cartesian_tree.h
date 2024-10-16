#ifndef _CARTESIAN_TREE_H_
#define _CARTESIAN_TREE_H_

#include "../utility.h"

typedef struct treap_node
{
    request* req;
    unsigned priority;
    struct treap_node* left;
    struct treap_node* right;
} treap_node;

typedef struct Cartesian_tree
{
    treap_node* root;
    size_t size;
    int (*compare)(const request*, const request*);
} Cartesian_tree, Treap;


status_code treap_set_null(Treap* treap);
status_code treap_construct(Treap* treap, int (*compare)(const request*, const request*));
status_code treap_copy(Treap* treap_dest, const Treap* treap_src);
status_code treap_destruct(Treap* treap);

status_code treap_meld(Treap* treap_res, Treap* treap_l, Treap* treap_r);
status_code treap_copy_meld(Treap* treap_res, const Treap* treap_l, const Treap* treap_r);

status_code treap_size(const Treap* treap, size_t* size);
status_code treap_top(const Treap* treap, request** req);
status_code treap_pop(Treap* treap, request** req);
status_code treap_insert(Treap* treap, request* req);

#endif // _CARTESIAN_TREE_H_