#ifndef _MAP_H_
#define _MAP_H_

#include "../utility.h"
#include "../department.h"

typedef enum map_base_t
{
    MB_HASHSET,
    MB_ARR,
    MB_BST,
    MB_TRIE,
} map_base_t;

// Map contains POINTERS to departments, so department cannot be freed in case its erasing, BUT destruct frees all contained data
typedef struct Map
{
    void* ds;
    status_code (*set_null)(void* ds);
    // Returns NULL_ARG (1) if ds is NULL
    // Otherwise, returns OK (0)
    
    status_code (*construct)(void* ds, size_t (*calc_hash)(const char*));
    // Returns NULL_ARG (1) if one of the arguments is NULL
    // Returns BAD_ALLOC (18) if memory allocation error occurred
    // Otherwise, returns OK (0)
    
    status_code (*destruct)(void* ds);
    // returns OK (0) (?)
    
    status_code (*contains)(void* ds, const char* key, int* is_contained);
    // Returns NULL_ARG (1) if one of the arguments is NULL
    // Otherwise, returns OK (0)
    
    status_code (*get)(void* ds, const char* key, Department** dep);
    // Returns NULL_ARG (1) if one of the arguments is NULL
    // Returns BAD_ACCESS (19) if key is not contained in the map
    // Otherwise, returns OK (0)
    
    status_code (*insert)(void* ds, const char* key, const Department* dep);
    // Returns NULL_ARG (1) if one of the arguments is NULL
    // Returns BAD_ALLOC (18) if memory allocation error occurred
    // Returns BAD_ACCESS (19) if key is already contained in the map
    // Otherwise, returns OK (0)
    
    status_code (*erase)(void* ds, const char* key);
    // Returns NULL_ARG (1) if one of the arguments is NULL
    // Returns BAD_ACCESS (19) if key is not contained in the map
    // Otherwise, returns OK (0)
} Map;

status_code map_set_null(Map* map);
status_code map_construct(Map* map, map_base_t base, size_t (*calc_hash)(const char*));
status_code map_destruct(Map* map);

status_code map_contains(Map* map, const char* key, int* is_contained);
status_code map_get(Map* map, const char* key, Department** dep);
status_code map_insert(Map* map, const char* key, Department* dep);
status_code map_erase(Map* map, const char* key);

#endif