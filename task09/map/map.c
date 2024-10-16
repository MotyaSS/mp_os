#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "hash_set.h"
#include "dynamic_array.h"
#include "binary_search_tree.h"
#include "trie.h"

status_code map_set_null(Map* map)
{
    if (map == NULL)
    {
        return NULL_ARG;
    }
    map->ds = NULL;
    map->set_null = NULL;
    map->construct = NULL;
    map->destruct = NULL;
    map->contains = NULL;
    map->get = NULL;
    map->insert = NULL;
    map->erase = NULL;
    return OK;
}

status_code map_construct(Map* map, map_base_t base, size_t (*calc_hash)(const char*))
{
    if (map == NULL || calc_hash == NULL)
    {
        return NULL_ARG;
    }
    
    map->ds = NULL;
    
    switch (base)
    {
        case MB_HASHSET:
        {
            map->ds = malloc(sizeof(Hash_set));
            
            map->set_null           = (status_code (*)(void*))                                      hset_set_null;
            map->construct          = (status_code (*)(void*, size_t (*)(const char*)))             hset_construct;
            map->destruct           = (status_code (*)(void*))                                      hset_destruct;
            map->contains           = (status_code (*)(void*, const char*, int* is_contained))      hset_contains;
            map->get                = (status_code (*)(void*, const char*, Department** dep))       hset_get;
            map->insert             = (status_code (*)(void*, const char*, const Department* dep))  hset_insert;
            map->erase              = (status_code (*)(void*, const char*))                         hset_erase;
            break;
        }
        case MB_ARR:
        {
            map->ds = malloc(sizeof(Dynamic_array));
            
            map->set_null           = (status_code (*)(void*))                                      arr_set_null;
            map->construct          = (status_code (*)(void*, size_t (*)(const char*)))             arr_construct;
            map->destruct           = (status_code (*)(void*))                                      arr_destruct;
            map->contains           = (status_code (*)(void*, const char*, int* is_contained))      arr_contains;
            map->get                = (status_code (*)(void*, const char*, Department** dep))       arr_get;
            map->insert             = (status_code (*)(void*, const char*, const Department* dep))  arr_insert;
            map->erase              = (status_code (*)(void*, const char*))                         arr_erase;
            break;
        }
        case MB_BST:
        {
            map->ds = malloc(sizeof(Binary_search_tree));
            
            map->set_null           = (status_code (*)(void*))                                      bst_set_null;
            map->construct          = (status_code (*)(void*, size_t (*)(const char*)))             bst_construct;
            map->destruct           = (status_code (*)(void*))                                      bst_destruct;
            map->contains           = (status_code (*)(void*, const char*, int* is_contained))      bst_contains;
            map->get                = (status_code (*)(void*, const char*, Department** dep))       bst_get;
            map->insert             = (status_code (*)(void*, const char*, const Department* dep))  bst_insert;
            map->erase              = (status_code (*)(void*, const char*))                         bst_erase;
            break;
        }
        case MB_TRIE:
        {
            map->ds = malloc(sizeof(Trie));
            
            map->set_null           = (status_code (*)(void*))                                      trie_set_null;
            map->construct          = (status_code (*)(void*, size_t (*)(const char*)))             trie_construct;
            map->destruct           = (status_code (*)(void*))                                      trie_destruct;
            map->contains           = (status_code (*)(void*, const char*, int* is_contained))      trie_contains;
            map->get                = (status_code (*)(void*, const char*, Department** dep))       trie_get;
            map->insert             = (status_code (*)(void*, const char*, const Department* dep))  trie_insert;
            map->erase              = (status_code (*)(void*, const char*))                         trie_erase;
            break;
        }
    }
    
    map->set_null(map->ds);
    status_code code = map->construct(map->ds, calc_hash);
    
    if (code)
    {
        free(map->ds);
        map_set_null(map);
        return code;
    }
    
    return OK;
}

status_code map_destruct(Map* map)
{
    if (map == NULL)
    {
        return OK;
    }
    if (map->ds != NULL)
    {
        map->destruct(map->ds);
        free(map->ds);
    }
    map_set_null(map);
    return OK;
}


status_code map_contains(Map* map, const char* key, int* is_contained)
{
    if (map == NULL || key == NULL || is_contained == NULL)
    {
        return NULL_ARG;
    }
    return map->contains(map->ds, key, is_contained);
}

status_code map_get(Map* map, const char* key, Department** dep)
{
    if (map == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    return map->get(map->ds, key, dep);
}

status_code map_insert(Map* map, const char* key, Department* dep)
{
    if (map == NULL || key == NULL || dep == NULL)
    {
        return NULL_ARG;
    }
    return map->insert(map->ds, key, dep);
}

status_code map_erase(Map* map, const char* key)
{
    if (map == NULL || key == NULL)
    {
        return NULL_ARG;
    }
    return map->erase(map->ds, key);
}
