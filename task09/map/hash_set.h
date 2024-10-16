#ifndef _HASH_SET_H_
#define _HASH_SET_H_

#include "../utility.h"
#include "../department.h"

#define HASH_SET_ALPHABET_SIZE 36
#define MIN_HS_SIZE 127
#define MAX_HS_SIZE (1ULL << 58)

typedef struct hs_node
{
	char* key;
	Department* dep;
	struct hs_node* prev;
	struct hs_node* next;
} hs_node;

typedef struct Hash_set
{
	ull (*calc_hash)(const char*);
	size_t hs_size;
	size_t* chain_sizes;
	hs_node** chains;
	size_t max_chain_size;
	size_t max_chain_cnt;
	size_t min_chain_size;
	size_t min_chain_cnt;
} Hash_set, hset;

status_code hset_set_null(hset* hs);
status_code hset_construct(hset* hs, ull (*calc_hash)(const char*));
status_code hset_destruct(hset* hs);

status_code hset_contains(hset* hs, const char* key, int* is_contained);
status_code hset_get(hset* hs, const char* key, Department** dep);
status_code hset_insert(hset* hs, const char* key, Department* dep);
status_code hset_erase(hset* hs, const char* key);

#endif // _HASH_SET_H_