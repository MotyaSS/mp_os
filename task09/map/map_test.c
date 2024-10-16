#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "../utility.h"
#include "map.h"

#include "trie.h"

int main()
{
    clock_t timer;
    for (map_base_t base = MB_HASHSET; base <= MB_TRIE; ++base)
    {
        //if (base == MB_HASHSET) continue;
        //if (base == MB_ARR) continue;
        //if (base == MB_BST) continue;
        //if (base == MB_TRIE) continue;
        
        timer = clock();
        
        Map map;
        Department* dep;
        Department* dep1;
        char* dep_name;
        char* key;
        int is_contained;
        
        map_set_null(&map);
        
        // Empty map
        {
            assert(map_destruct(&map) == OK);
            assert(map_construct(&map, base, calc_default_str_hash) == OK);
            assert(map_destruct(&map) == OK);
            assert(map_destruct(&map) == OK);
        }
        // test 1
        {
            assert(map_construct(&map, base, calc_default_str_hash) == OK);
            
            key = (char*) malloc(sizeof(char) * 3);
            assert(key != NULL);
            key[2] = '\0';
            
            for (size_t i = 0; i < 10; ++i)
            {
                dep = (Department*) malloc(sizeof(Department));
                dep1 = NULL;
                assert(dep != NULL);
                assert(department_construct(dep, "123", i, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
                
                key[0] = '0' + (i % 10);
                key[1] = '0' + ((i*5 + 3) % 10);
                
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(map_insert(&map, key, dep) == OK);
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map_get(&map, key, &dep1) == OK);
                assert(dep == dep1);
            }
            
            for (size_t i = 0; i < 10; ++i)
            {
                key[0] = '0' + (i % 10);
                key[1] = '0' + ((i*5 + 3) % 10);
                
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map_get(&map, key, &dep) == OK);
                assert(dep->staff_size == i);
                
                assert(map_erase(&map, key) == OK);
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(department_destruct(dep) == OK);
                free(dep);
            }
            
            free(key);
            assert(map_destruct(&map) == OK);
        }
        // test 2
        {
            assert(map_construct(&map, base, calc_default_str_hash) == OK);
            
            key = (char*) malloc(sizeof(char) * 11);
            assert(key != NULL);
            key[10] = '\0';
            
            for (size_t i = 0; i < 1000; ++i)
            {
                dep_name = (char*) malloc(sizeof(char) * 4);
                assert(dep_name != NULL);
                sprintf(dep_name, "%llu", (ull) i);
                dep = (Department*) malloc(sizeof(Department));
                dep1 = NULL;
                assert(dep != NULL);
                assert(department_construct(dep, dep_name, 1, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
                free(dep_name);
                dep_name = NULL;
                
                key[0] = '0' + ((7*i + 17) % 3);
                key[1] = '0' + ((17*i + 13) % 5);
                key[2] = '0' + ((i*(i+3) + 2) % 7);
                key[3] = '0' + ((i*(i+2)*(i+5) + 17) % 10);
                key[4] = 'A' + ((3*i + 3) % 26);
                key[5] = 'A' + ((17*i + 5) % 26);
                key[6] = 'A' + ((53*i + 7) % 26);
                key[7] = 'A' + ((97*i + 2) % 26);
                key[8] = 'A' + ((137*i + 1) % 23);
                key[9] = 'A' + ((177*i + 55) % 17);
                
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(map_insert(&map, key, dep) == OK);
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map_get(&map, key, &dep1) == OK);
                assert(dep == dep1);
            }
            
            for (size_t i = 0; i < 900; ++i)
            {
                key[0] = '0' + ((7*i + 17) % 3);
                key[1] = '0' + ((17*i + 13) % 5);
                key[2] = '0' + ((i*(i+3) + 2) % 7);
                key[3] = '0' + ((i*(i+2)*(i+5) + 17) % 10);
                key[4] = 'A' + ((3*i + 3) % 26);
                key[5] = 'A' + ((17*i + 5) % 26);
                key[6] = 'A' + ((53*i + 7) % 26);
                key[7] = 'A' + ((97*i + 2) % 26);
                key[8] = 'A' + ((137*i + 1) % 23);
                key[9] = 'A' + ((177*i + 55) % 17);
                
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 1);
                assert(map_get(&map, key, &dep) == OK);
                assert(atoi(dep->name) == i);
                
                assert(map_erase(&map, key) == OK);
                assert(map_contains(&map, key, &is_contained) == OK);
                assert(is_contained == 0);
                assert(department_destruct(dep) == OK);
                free(dep);
            }
            
            free(key);
            assert(map_destruct(&map) == OK);
        }
        // test 3
        {
            assert(map_construct(&map, base, calc_default_str_hash) == OK);
            
            key = (char*) malloc(sizeof(char) * 2);
            assert(key != NULL);
            strcpy(key, "A");
            
            dep = (Department*) malloc(sizeof(Department));
            assert(dep != NULL);
            assert(department_construct(dep, "123", 2, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
            assert(map_insert(&map, key, dep) == OK);
            
            dep = (Department*) malloc(sizeof(Department));
            assert(dep != NULL);
            assert(department_construct(dep, "123", 2, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
            assert(map_insert(&map, key, dep) == BAD_ACCESS);
            assert(department_destruct(dep) == OK);
            free(dep);
            
            for (size_t i = 1; i < 20; ++i)
            {
                key[0] = 'A' + i;
                dep = (Department*) malloc(sizeof(Department));
                assert(dep != NULL);
                assert(department_construct(dep, "123", 2, PQB_BINOM, 2.0, 1e-9, 1, 1, compare_request) == OK);
                assert(map_insert(&map, key, dep) == OK);
            }
            
            free(key);
            assert(map_destruct(&map) == OK);
        }
        
        assert(map_destruct(&map) == OK);
        
        if (base == MB_HASHSET) printf("%-40s %0.1lfms\n", "Hash set passed the tests", (double)(clock() - timer) / CLOCKS_PER_SEC * 1000);
        if (base == MB_ARR) 	printf("%-40s %0.1lfms\n", "Dynamic array passed the tests", (double)(clock() - timer) / CLOCKS_PER_SEC * 1000);
        if (base == MB_BST) 	printf("%-40s %0.1lfms\n", "Binary search tree passed the tests", (double)(clock() - timer) / CLOCKS_PER_SEC * 1000);
        if (base == MB_TRIE) 	printf("%-40s %0.1lfms\n", "Trie passed the tests", (double)(clock() - timer) / CLOCKS_PER_SEC * 1000);
    }
    printf("All tests have been passed\n");
}