#ifndef _TRIE_H_
#define _TRIE_H_

#include "../utility.h"
#include "../department.h"

#define TRIE_ALPABET_SIZE 36

typedef struct trie_node
{
    struct trie_node* parent;
    struct trie_node* children[TRIE_ALPABET_SIZE];
    Department* dep;
} trie_node;

typedef struct Trie
{
    trie_node* root;
} Trie;

typedef pair_str_department trie_key_val;

status_code trie_set_null(Trie* trie);
status_code trie_construct(Trie* trie);
status_code trie_destruct(Trie* trie);

status_code trie_contains(const Trie* trie, const char* key, int* is_contained);
status_code trie_get(const Trie* trie, const char* key, Department** dep);
status_code trie_insert(Trie* trie, const char* key, Department* dep);
status_code trie_erase(Trie* trie, const char* key);

#endif // _TRIE_H_