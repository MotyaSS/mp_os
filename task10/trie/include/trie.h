#ifndef _TRIE_H_
#define _TRIE_H_

#include <stdint.h>

#include <utility.h>

#define TRIE_ALPABET_SIZE 63

typedef struct trie_node
{
    struct trie_node* parent;
    struct trie_node* children[TRIE_ALPABET_SIZE];
    uint32_t value;
    int is_initialized;
} trie_node;

typedef struct Trie
{
    trie_node* root;
} Trie;

typedef struct trie_key_val
{
	char* key;
	uint32_t value;
} trie_key_val;

status_code trie_set_null(Trie* trie);
status_code trie_construct(Trie* trie);
status_code trie_destruct(Trie* trie);

status_code trie_contains(const Trie* trie, const char* key, int* is_contained);
status_code trie_insert(Trie* trie, const char* key, uint32_t value);
status_code trie_erase(Trie* trie, const char* key);
status_code trie_get(const Trie* trie, const char* key, uint32_t* value);
status_code trie_set(Trie* trie, const char* key, uint32_t value);

status_code trie_get_key_vals(const Trie* trie, ull* cnt, trie_key_val** key_vals);

#endif // _TRIE_H_