#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"

int arr_comparator(const void* e_1, const void* e_2) 
{
    arr_elem* e1 = *(arr_elem**) e_1;
    arr_elem* e2 = *(arr_elem**) e_2;

    return strcmp(e1->key, e2->key);
}

status_code arr_search(Array* arr, const char* key, size_t* res)
{
    if (arr == NULL || key == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    
    if (arr->size == 0)
    {
        return BAD_ACCESS;
    }
    
    size_t l = 0;
    size_t r = arr->size - 1;
    size_t m = 0;
    int cmp = 1;
    
    while (r > l)
    {
        m = (l + r) / 2;
        cmp = strcmp(key, arr->elems[m]->key);
        if (cmp == 0)
        {
            *res = m;
            return OK;
        }
        else if (cmp > 0)
        {
            l = m + 1;
        }
        else
        {
            r = m;
        }
    }
    if (!strcmp(key, arr->elems[l]->key))
    {
        *res = l;
        return OK;
    }

    return BAD_ACCESS;
}


status_code arr_elem_construct(arr_elem** elem, const char* key, Department* dep)
{
    if (key == NULL || dep == NULL)
    {
        return INVALID_ARG;
    }

    *elem = (arr_elem*) malloc(sizeof(arr_elem));
    if (*elem == NULL) 
    {
        return BAD_ALLOC;
    }

    (*elem)->key = (char*) malloc(sizeof(char) * (strlen(key) + 1));
    if ((*elem)->key == NULL) 
    {
        return BAD_ALLOC;
    }

    strcpy((*elem)->key, key);
    (*elem)->dep = dep;

    return OK;
}


status_code arr_set_null(Array* arr)
{
    if (arr == NULL)
    {
        return NULL_ARG;
    }

    arr->size = 0;
    arr->capacity = 0;
    arr->elems = NULL;

    return OK;
}

status_code arr_construct(Array* arr) 
{
    if (arr == NULL)
    {
        return NULL_ARG;
    }
    
    arr->size = 0;
    arr->capacity = 8;

    arr->elems = (arr_elem**) malloc(sizeof(arr_elem*) * arr->capacity);
    if (arr->elems == NULL) 
    {
        return BAD_ALLOC;
    }

    return OK;
}

status_code arr_destruct(Array* arr)
{
    if (arr == NULL)
    {
        return NULL_ARG;
    }
    
    for (int i = 0; i < arr->size; ++i)
    {
        department_destruct(arr->elems[i]->dep);
        free_all(3, arr->elems[i]->key, arr->elems[i]->dep, arr->elems[i]);
    }
    
    free(arr->elems);
    arr_set_null(arr);
    
    return OK;
}


status_code arr_contains(Array* arr, const char* key, int* is_contained)
{
    if (arr == NULL || key == NULL || is_contained == NULL)
    {
        return NULL_ARG;
    }

    size_t ind;
    if (arr_search(arr, key, &ind) == OK)
    {
        *is_contained = 1;
    }
    else
    {
        *is_contained = 0;
    }

    return OK;
}

status_code arr_get(Array* arr, const char* key, Department** dep)
{
    if (arr == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }

    size_t ind;
    status_code code = arr_search(arr, key, &ind);
    if (code)
    {
        return code;
    }

    *dep = arr->elems[ind]->dep;

    return OK;
}

status_code arr_insert(Array* arr, const char* key, Department* dep) 
{
    if (arr == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    
    if (arr->size >= arr->capacity) 
    {
        arr->capacity *= 2;
        arr_elem** tmp;
        
        tmp = realloc(arr->elems, sizeof(arr_elem*) * arr->capacity);
        if (tmp == NULL) 
        {
            free(arr->elems);
            return BAD_ALLOC;
        }
        arr->elems = tmp;
    }
    
    arr_elem* elem;
    status_code code = arr_elem_construct(&elem, key, dep);
    if (code != OK)
    {
        return code;
    }
    
    size_t ind = arr->size;
    int comp = 1;
    
    while (ind > 0 && (comp = strcmp(key, arr->elems[ind - 1]->key)) < 0)
    {
        ind--;
    }
    
    if (comp == 0)
    {
        free_all(2, elem->key, elem);
        return BAD_ACCESS;
    }
    
    for (size_t i = arr->size; i > ind; --i)
    {
        arr->elems[i] = arr->elems[i-1];
    }
    
    arr->elems[ind] = elem;
    arr->size++;

    return OK;
}

status_code arr_erase(Array* arr, const char* key)
{
    if (arr == NULL || key == NULL)
    {
        return NULL_ARG;
    }

    size_t ind;
    status_code code = arr_search(arr, key, &ind);
    if (code)
    {
        return code;
    }
    
    free_all(2, arr->elems[ind]->key, arr->elems[ind]);
    
    for (int i = ind; i < arr->size - 1; ++i)
    {
        arr->elems[i] = arr->elems[i + 1];
    }

    arr->size--;

    return OK;
}
