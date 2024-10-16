#include "trie.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

int char_to_int(char ch)
{
    if (isdigit(ch))
    {
        return ch - '0';
    }
    if (isalpha(ch))
    {
        return toupper(ch) - 'A' + 10;
    }
    return -1;
}

int char_to_ind(char ch)
{
    return char_to_int(ch);
}

char ind_to_char(int ind)
{
    if (ind >= 0 && ind <= 9)
    {
        return '0' + ind;
    }
    if (ind > 9 && ind < 36)
    {
        return 'a' + ind - 10;
    }
    return '\0';
}


status_code trie_node_construct(trie_node** node)
{
    if (node == NULL)
    {
        return NULL_ARG;
    }
    *node = (trie_node*) malloc(sizeof(trie_node));
    if (*node == NULL)
    {
        return BAD_ALLOC;
    }
    for (ull i = 0; i < TRIE_ALPABET_SIZE; ++i)
    {
        (*node)->children[i] = NULL;
    }
    (*node)->parent = NULL;
    (*node)->dep = NULL;
    return OK;
}

status_code trie_node_destruct(trie_node** node)
{
    if (node == NULL)
    {
        return NULL_ARG;
    }
    if (*node == NULL)
    {
        return OK;
    }
    
    for (ull i = 0; i < TRIE_ALPABET_SIZE; ++i)
    {
        trie_node_destruct(&(*node)->children[i]);
    }
    department_destruct((*node)->dep);
    free_all(2, (*node)->dep, *node);
    *node = NULL;
    return OK;
}

status_code trie_set_null(Trie* trie)
{
    if (trie == NULL)
    {
        return NULL_ARG;
    }
    trie->root = NULL;
    return OK;
}

status_code trie_construct(Trie* trie)
{
    if (trie == NULL)
    {
        return NULL_ARG;
    }
    status_code err_code = trie_node_construct(&trie->root);
    if (err_code)
    {
        return err_code;
    }
    return OK;
}

status_code trie_destruct(Trie* trie)
{
    if (trie == NULL)
    {
        return OK;
    }
    status_code err_code = OK;
    if (trie->root != NULL)
    {
        err_code = trie_node_destruct(&trie->root);
    }
    trie->root = NULL;
    return err_code;
}


status_code trie_search_node(const Trie* trie, const char* key, trie_node** node)
{
    if (trie == NULL || key == NULL || node == NULL)
    {
        return NULL_ARG;
    }
    if (trie->root == NULL)
    {
        return INVALID_INPUT;
    }
    *node = NULL;
    trie_node* cur = trie->root;
    for (ull i = 0; key[i]; ++i)
    {
        if (cur == NULL)
        {
            *node = NULL;
            return OK;
        }
        int ind = char_to_ind(key[i]);
        if (ind == -1)
        {
            return INVALID_INPUT;
        }
        cur = cur->children[ind];
    }
    *node = cur;
    return OK;
}

status_code trie_create_node(const Trie* trie, const char* key, trie_node** node)
{
    if (trie == NULL || key == NULL)
    {
        return NULL_ARG;
    }
    if (trie->root == NULL)
    {
        return INVALID_INPUT;
    }
    status_code err_code = OK;
    trie_node* cur = trie->root;
    trie_node* first_new = NULL;
    for (ull i = 0; key[i] && !err_code; ++i)
    {
        int ind = char_to_ind(key[i]);
        if (ind == -1)
        {
            err_code = INVALID_INPUT;
        }
        if (cur->children[ind] == NULL)
        {
            err_code = trie_node_construct(&cur->children[ind]);
            first_new = first_new == NULL ? cur->children[ind] : first_new;
            if (!err_code)
            {
                cur->children[ind]->parent = cur;
            }
        }
        if (!err_code)
        {
            cur = cur->children[ind];
        }
    }
    if (err_code)
    {
        trie_node_destruct(&first_new);
        cur = NULL;
    }
    if (node != NULL)
    {
        *node = cur;
    }
    return err_code;
}

status_code trie_clean_branch(const Trie* trie, trie_node* node, trie_node* cleaned_child_node)
{
    if (trie == NULL || node == NULL)
    {
        return NULL_ARG;
    }
    int child_flag = 0;
    for (ull i = 0; i < TRIE_ALPABET_SIZE; ++i)
    {
        if (cleaned_child_node != NULL && node->children[i] == cleaned_child_node)
        {
            node->children[i] = NULL;
        }
        if (node->children[i] != NULL)
        {
            child_flag = 1;
        }
    }
    if (child_flag || trie->root == node || node->dep != NULL)
    {
        return OK;
    }
    trie_node* parent = node->parent;
    trie_node* tmp = node;
    trie_node_destruct(&node);
    return trie_clean_branch(trie, parent, tmp);
}


status_code trie_contains(const Trie* trie, const char* key, int* is_contained)
{
    if (trie == NULL || key == NULL || is_contained == NULL)
    {
        return NULL_ARG;
    }
    trie_node* node = NULL;
    status_code err_code = trie_search_node(trie, key, &node);
    if (err_code)
    {
        return err_code;
    }
    *is_contained = node != NULL && node->dep != NULL;
    return OK;
}

status_code trie_get(const Trie* trie, const char* key, Department** dep)
{
    if (trie == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    trie_node* node = NULL;
    status_code err_code = trie_search_node(trie, key, &node);
    if (err_code)
    {
        return err_code;
    }
    if (node == NULL)
    {
        return BAD_ACCESS;
    }
    *dep = node->dep;
    return OK;
}

status_code trie_insert(Trie* trie, const char* key, Department* dep)
{
    if (trie == NULL || key == NULL)
    {
        return NULL_ARG;
    }
    trie_node* node = NULL;
    status_code err_code = trie_create_node(trie, key, &node);
    if (err_code)
    {
        return err_code;
    }
    if (node->dep != NULL)
    {
        return BAD_ACCESS;
    }
    node->dep = dep;
    if (node->dep == NULL)
    {
        return trie_clean_branch(trie, node, NULL);
    }
    return OK;
}

status_code trie_erase(Trie* trie, const char* key)
{
    if (trie == NULL || key == NULL)
    {
        return NULL_ARG;
    }
    trie_node* node = NULL;
    status_code err_code = trie_search_node(trie, key, &node);
    if (err_code)
    {
        return err_code;
    }
    if (node == NULL)
    {
        return BAD_ACCESS;
    }
    node->dep = NULL;
    return trie_clean_branch(trie, node, NULL);
}
