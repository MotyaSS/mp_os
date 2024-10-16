#ifndef _BINARY_SEARCH_TREE_H_
#define _BINARY_SEARCH_TREE_H_

#include "../utility.h"
#include "../department.h"

typedef struct bst_node
{
    char* key;
    struct bst_node* left;
    struct bst_node* right;
    Department* dep;
} bst_node;

typedef struct Binary_search_tree
{
    bst_node* root;
} Binary_search_tree, bst;


status_code bst_set_null(bst* tree);
status_code bst_construct(bst* tree, size_t (*unused)(const char*));
status_code bst_destruct(bst* tree);

status_code bst_contains(bst* tree, const char* key, int* is_contained);
status_code bst_get(bst* tree, const char* key, Department** dep);
status_code bst_insert(bst* tree, const char* key, Department* dep);
status_code bst_erase(bst* tree, const char* key);

void print_tree(bst_node *node, int tab);

#endif // _BINARY_SEARCH_TREE_H_