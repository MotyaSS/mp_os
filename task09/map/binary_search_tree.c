#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binary_search_tree.h"

status_code bst_node_construct(bst_node** node, const char* key, Department* dep)
{
    if (node == NULL)
    {
        return NULL_ARG;
    }
    
    *node = (bst_node*) malloc(sizeof(bst_node));
    if (*node == NULL)
    {
        return BAD_ALLOC;
    }
    
    (*node)->key = (char*) malloc(sizeof(char) * (strlen(key) + 1));
    if ((*node)->key == NULL)
    {
        free(*node);
        return BAD_ALLOC;
    }
    
    strcpy((*node)->key, key);
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->dep = dep;
    
    return OK;
}

status_code bst_node_destruct(bst_node* node)
{
    if (node == NULL)
    {
        return OK;
    }
    
    bst_node_destruct(node->left);
    bst_node_destruct(node->right);
    
    department_destruct(node->dep);
    free_all(3, node->key, node->dep, node);
    
    return OK;
}


status_code bst_set_null(bst* tree)
{
    if (tree == NULL)
    {
        return INVALID_ARG;
    }

    tree->root = NULL;

    return OK;
}

status_code bst_construct(bst* tree, size_t (*unused)(const char*))
{
    if (tree == NULL)
    {
        return NULL_ARG;
    }
    
    tree->root = NULL;
    
    return OK;
}

status_code bst_destruct(bst* tree)
{
    if (tree == NULL)
    {
        return OK;
    }
    
    status_code err_code = OK;
    if (tree->root != NULL)
    {
        err_code = bst_node_destruct(tree->root);
    }
    
    tree->root = NULL;
    
    return err_code;
}


status_code bst_search_node(bst* tree, const char* key, bst_node** node)
{
    if (tree == NULL || key == NULL || node == NULL)
    {
        return NULL_ARG;
    }
    
    if (tree->root == NULL)
    {
        *node = NULL;
        return OK;
    }
    
    bst_node* cur = tree->root;
    int comp = 0;
    
    while (cur != NULL)
    {
        comp = strcmp(key, cur->key);
        if (comp == 0)
        {
            *node = cur;
            return OK;
        }
        else if (comp < 0)
        {
            cur = cur->left;
        }
        else
        {
            cur = cur->right;
        }
    }
    
    *node = NULL;
    return OK;
}

status_code bst_contains(bst* tree, const char* key, int* is_contained)
{
    if (tree == NULL || key == NULL || is_contained == NULL)
    {
        return NULL_ARG;
    }
    
    bst_node* node = NULL;
    bst_search_node(tree, key, &node);
    *is_contained = node == NULL ? 0 : 1;
    
    return OK;
}

status_code bst_get(bst* tree, const char* key, Department** dep)
{
    if (tree == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    
    bst_node* node = NULL;
    bst_search_node(tree, key, &node);
    
    if (node == NULL)
    {
        return BAD_ACCESS;
    }
    
    *dep = node->dep;
    
    return OK;
}

status_code bst_insert(bst* tree, const char* key, Department* dep)
{
    if (tree == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    
    if (tree->root == NULL)
    {
        return bst_node_construct(&tree->root, key, dep);
    }
    
    bst_node* cur = tree->root;
    bst_node* parent = NULL;
    int comp = 0;
    
    while (cur != NULL)
    {
        parent = cur;
        comp = strcmp(key, cur->key);
        
        if (comp == 0)
        {
            return BAD_ACCESS;
        }
        
        cur = comp < 0 ? cur->left : cur->right;
    }
    
    return comp < 0 ? bst_node_construct(&parent->left, key, dep)
                    : bst_node_construct(&parent->right, key, dep);
}

status_code bst_del_node(bst_node** del_node)
{   
    bst_node* node = *del_node;

    if (node == NULL)
    {
        return OK;
    }

    bst_node* new_node;
    if (node->left == NULL && node->right == NULL)
    {
        new_node = NULL;
    }
    else if (node->left == NULL)
    {
        new_node = node->right;
    }
    else if (node->right == NULL)
    {
        new_node = node->left;
    }
    else
    {
        bst_node* prev = node;
        new_node = node->right;
        while (new_node->left != NULL)
        {
            prev = new_node;
            new_node = new_node->left;
        }
        
        new_node->left = node->left;
        prev->left = new_node->right;
        
        if (node->right != new_node)
        {
            new_node->right = node->right;
        }
    }
    
    free(node->key);
    free(node);
    *del_node = new_node;

    return OK;
}

status_code bst_erase(bst* tree, const char* key)
{
    if (tree == NULL || key == NULL)
    {
        return NULL_ARG;
    }

    bst_node* prev = NULL;
    bst_node* cur = tree->root;
    int comp, flag;

    while (cur != NULL)
    {
        if (cur->key == NULL)
        {
            return INVALID_ARG;
        }

        comp = strcmp(key, cur->key);
        if (comp == 0)
        {
            break;
        }
        else if (comp < 0)
        {
            prev = cur;
            cur = cur->left;
            flag = 0;
        }
        else
        {
            prev = cur;
            cur = cur->right;
            flag = 1;
        }
    }

    status_code status = bst_del_node(&cur);
    if (prev == NULL)
    {
        tree->root = cur;
        return status;
    }
    
    if (flag)
    {
        prev->right = cur;
    }
    else
    {
        prev->left = cur;
    }

    return status;
}

void print_tree(bst_node *node, int tab) 
{
    if (node == NULL) 
    {
        return;
    }

    print_tree(node->right, 1 + tab);

    for (int i = 0; i < tab; i++) 
    {
        printf("  ");
    }
    printf("%s\n", node->key);

    print_tree(node->left, 1 + tab);
}