#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cartesian_tree.h"

int treap_node_construct(treap_node** node, request* req)
{
    if (node == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    *node = (treap_node*) malloc(sizeof(treap_node));
    if (*node == NULL)
    {
        return BAD_ALLOC;
    }

    (*node)->req = req;
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->priority = rand();

    return OK;
}

void treap_node_destruct(treap_node* node)
{
    if (node == NULL)
    {
        return;
    }
    
    treap_node_destruct(node->left);
    treap_node_destruct(node->right);
    request_destruct(node->req);
    free_all(2, node->req, node);
}

status_code treap_node_copy(treap_node** node_dest, treap_node* node_src)
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
    treap_node* tmp_node = (treap_node*) malloc(sizeof(treap_node));
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
    
    tmp_node->priority = node_src->priority;
    tmp_node->left = NULL;
    tmp_node->right = NULL;
    
    code = code ? code : treap_node_copy(&tmp_node->left, node_src->left);
    code = code ? code : treap_node_copy(&tmp_node->right, node_src->right);
    if (code)
    {
        treap_node_destruct(tmp_node);
        return OK;
    }
    
    *node_dest = tmp_node;
    
    return OK;
}

treap_node* treap_node_merge(treap_node* t1, treap_node* t2)
{
    if (t1 == NULL && t2 == NULL)
    {
        return NULL;
    }
    
    if (t1 == NULL)
    {
        return t2;
    }
    if (t2 == NULL)
    {
        return t1;
    }

    if (t1->priority > t2->priority)
    {
        t1->right = treap_node_merge(t1->right, t2);
        return t1;
    }
    else
    {
        t2->left = treap_node_merge(t1, t2->left);
        return t2;
    }
}

void treap_node_split(treap_node* t, int (*compare)(const request*, const request*), 
                        request* req, treap_node** t1, treap_node** t2) {
    if (compare == NULL || req == NULL || t1 == NULL || t2 == NULL)
    {
        return;
    }

    if (t == NULL)
    {
        *t1 = NULL;
        *t2 = NULL;
        return;
    }

    if (compare(t->req, req) < 0)
    {
        treap_node_split(t->right, compare, req, &(t->right), t2);
        *t1 = t;
    }
    else
    {
        treap_node_split(t->left, compare, req, t1, &(t->left));
        *t2 = t;
    }
}

treap_node* treap_node_meld(treap_node* t1, treap_node* t2, int (*compare)(const request*, const request*))
{
    if ((t1 == NULL && t2 == NULL) || compare == NULL)
    {
        return NULL;
    }
    if (t1 == NULL)
    {
        return t2;
    }
    if (t2 == NULL)
    {
        return t1;
    }
    
    treap_node* less = NULL;
    treap_node* greater = NULL;
    
    t1 = treap_node_meld(t1, t2->left, compare);
    t1 = treap_node_meld(t1, t2->right, compare);
    t2->left = t2->right = NULL;
    
    treap_node_split(t1, compare, t2->req, &less, &greater);
    less = treap_node_merge(less, t2);
    t1 = treap_node_merge(less, greater);
    
    return t1;
}


status_code treap_set_null(Treap* treap)
{
    if (treap == NULL)
    {
        return NULL_ARG;
    }
    
    treap->root = NULL;
    treap->size = 0;
    treap->compare = NULL;
    
    return OK;
}

status_code treap_construct(Treap* treap, int (*compare)(const request*, const request*))
{
    if (treap == NULL || compare == NULL)
    {
        return NULL_ARG;
    }
    
    treap->root = NULL;
    treap->size = 0;
    treap->compare = compare;
    
    return OK;
}

status_code treap_copy(Treap* treap_dest, const Treap* treap_src)
{
    if (treap_dest == NULL || treap_src == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = treap_node_copy(&treap_dest->root, treap_src->root);
    if (code)
    {
        return code;
    }
    
    treap_dest->size = treap_src->size;
    treap_dest->compare = treap_src->compare;
    
    return OK;
}

status_code treap_destruct(Treap* treap)
{
    if (treap == NULL)
    {
        return OK;
    }
    
    treap_node_destruct(treap->root);
    treap_set_null(treap);
    
    return OK;
}


status_code treap_meld(Treap* treap_res, Treap* treap_l, Treap* treap_r)
{
    if (treap_res == NULL || treap_l == NULL || treap_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (treap_l == treap_r || treap_l->compare != treap_r->compare)
    {
        return INVALID_INPUT;
    }
    
    Treap treap_tmp;
    
    treap_tmp.root = treap_node_meld(treap_l->root, treap_r->root, treap_l->compare);
    treap_tmp.size = treap_l->size + treap_r->size;
    treap_tmp.compare = treap_l->compare;
    
    treap_l->root = treap_r->root = NULL;
    treap_l->size = treap_r->size = 0;
    *treap_res = treap_tmp;
    
    return OK;
}

status_code treap_copy_meld(Treap* treap_res, const Treap* treap_l, const Treap* treap_r)
{
    if (treap_res == NULL || treap_l == NULL || treap_r == NULL)
    {
        return NULL_ARG;
    }
    
    if (treap_res == treap_l || treap_res == treap_l)
    {
        return INVALID_INPUT;
    }
    
    status_code code = OK;
    Treap treap_lc, treap_rc;
        
    treap_set_null(&treap_lc);
    treap_set_null(&treap_rc);
    treap_set_null(treap_res);
    
    code = code ? code : treap_copy(&treap_lc, treap_l);
    code = code ? code : treap_copy(&treap_rc, treap_r);
    code = code ? code : treap_meld(treap_res, &treap_lc, &treap_rc);
    
    treap_destruct(&treap_lc);
    treap_destruct(&treap_rc);
    
    if (code)
    {
        treap_destruct(treap_res);
        return code;
    }
    
    return OK;
}


status_code treap_size(const Treap* treap, size_t* size)
{
    if (treap == NULL || size == NULL)
    {
        return NULL_ARG;
    }
    *size = treap->size;
    return OK;
}

status_code treap_top(const Treap* treap, request** req)
{
    if (treap == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    treap_node* cur = treap->root;
    
    if (cur == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    while (cur->left != NULL)
    {
        cur = cur->left;
    }

    *req = cur->req;

    return OK;
}

status_code treap_pop(Treap* treap, request** req)
{
    if (treap == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    treap_node* prev = NULL;
    treap_node* cur = treap->root;
    
    if (cur == NULL)
    {
        *req = NULL;
        return OK;
    }
    
    while (cur->left != NULL) {
        prev = cur;
        cur = cur->left;
    }
    
    if (prev != NULL)
    {
        prev->left = cur->right;
    }
    else
    {
        treap->root = treap->root->right;
    }
    
    *req = cur->req;
    free(cur);
    treap->size--;
    
    return OK;
}

status_code treap_insert(Treap* treap, request* req)
{
    if (treap == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    
    treap_node* node = NULL;
    
    status_code code = treap_node_construct(&node, req);
    if (code)
    {
        return code;
    }

    treap_node* less = NULL;
    treap_node* greater = NULL;
    
    treap_node_split(treap->root, treap->compare, req, &less, &greater);
    less = treap_node_merge(less, node);
    treap->root = treap_node_merge(less, greater);
    treap->size++;

    return OK;
}
