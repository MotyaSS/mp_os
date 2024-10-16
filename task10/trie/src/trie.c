#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <../include/trie.h>

int char_to_int(char ch)
{
    if (isdigit(ch))
    {
        return ch - '0';
    }
    if (isupper(ch))
    {
        return ch - 'A' + 10;
    }
    if (islower(ch))
    {
        return ch - 'a' + 36;
    }
    if (ch == '_')
    {
        return 62;
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
    if (ind > 9 && ind <= 35)
    {
        return 'A' + ind - 10;
    }
    if (ind > 35 && ind <= 61)
    {
        return 'a' + ind - 36;
    }
    if (ind == 62)
    {
        return '_';
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
    (*node)->is_initialized = 0;
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
    free(*node);
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
    if (child_flag || trie->root == node || node->is_initialized)
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
    *is_contained = node != NULL && node->is_initialized;
    return OK;
}

status_code trie_insert(Trie* trie, const char* key, uint32_t value)
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
    if (node->is_initialized)
    {
        return BAD_ACCESS;
    }
    node->value = value;
    node->is_initialized = 1;
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
    if (node == NULL || !node->is_initialized)
    {
        return BAD_ACCESS;
    }
    node->is_initialized = 0;
    return trie_clean_branch(trie, node, NULL);
}

status_code trie_get(const Trie* trie, const char* key, uint32_t* value)
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
    if (node == NULL || !node->is_initialized)
    {
        return BAD_ACCESS;
    }
    *value = node->value;
    return OK;
}

status_code trie_set(Trie* trie, const char* key, uint32_t value)
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
    if (node == NULL || !node->is_initialized)
    {
        return BAD_ACCESS;
    }
    node->value = value;
    return OK;
}


status_code trie_get_key_vals(const Trie* trie, ull* cnt, trie_key_val** key_vals)
{
	if (trie == NULL || cnt == NULL || key_vals == NULL)
	{
		return NULL_ARG;
	}
	if (trie->root == NULL)
	{
		return INVALID_INPUT;
	}
	
	ull kv_cnt = 0;
	ull kv_size = 2;
	trie_key_val* key_vals_tmp = (trie_key_val*) malloc(sizeof(trie_key_val) * 2);
	if (key_vals_tmp == NULL)
	{
		return BAD_ALLOC;
	}
	ull str_iter = 0;
	ull str_size = 2;
	char* str = malloc(sizeof(char) * 2);
	if (str == NULL)
	{
		free(key_vals_tmp);
		return BAD_ALLOC;
	}
	
	status_code err_code = OK;
	trie_node* cur_node = trie->root;
    
    // Process root node
    if (cur_node->is_initialized)
    {
        char* str_copy = (char*) malloc(sizeof(char));
        err_code = str_copy != NULL ? OK : BAD_ALLOC;
        if (str_copy != NULL)
        {
            strcpy(str_copy, "");
        }
        if (!err_code)
        {
            key_vals_tmp[kv_cnt].key = str_copy;
            key_vals_tmp[kv_cnt++].value = cur_node->value;
        }
        else
        {
            free(str_copy);
        }
    }
    
    // Process other nodes
	int ind = 0;
	int run_flag = 1;
    
	while (!err_code && run_flag)
	{
		// Select node to process
		while (cur_node->children[ind] == NULL && ind < TRIE_ALPABET_SIZE)
		{
			++ind;
		}
		// No node to process, back to parent
		if (ind >= TRIE_ALPABET_SIZE)
		{
			if (cur_node != trie->root)
			{
				ind = char_to_ind(str[--str_iter]) + 1;
				str[str_iter] = '\0';
				cur_node = cur_node->parent;
			}
			else
			{
				run_flag = 0;
			}
		}
		// Go process node
		else
		{
			// Update buffer string, ind and cur_node
			if (str_iter + 1 == str_size)
			{
				str_size *= 2;
				char* tmp = (char*) realloc(str, sizeof(char) * str_size);
				err_code = tmp != NULL ? OK : BAD_ALLOC;
				str = tmp != NULL ? tmp : str;
			}
			if (!err_code)
			{
				str[str_iter++] = ind_to_char(ind);
				str[str_iter] = '\0';
			}
			cur_node = cur_node->children[ind];
			ind = 0;
			
			// Process node
			if (!err_code && cur_node->is_initialized)
			{
				char* str_copy = (char*) malloc(sizeof(char) * (str_iter + 1));
				err_code = str_copy != NULL ? OK : BAD_ALLOC;
				if (str_copy != NULL)
				{
					strcpy(str_copy, str);
				}
				if (!err_code && kv_cnt == kv_size)
				{
					kv_size *= 2;
					trie_key_val* tmp = (trie_key_val*) realloc(key_vals_tmp, sizeof(trie_key_val) * kv_size);
					err_code = tmp != NULL ? OK : BAD_ALLOC;
					key_vals_tmp = tmp != NULL ? tmp : key_vals_tmp;
				}
				if (!err_code)
				{
					key_vals_tmp[kv_cnt].key = str_copy;
					key_vals_tmp[kv_cnt++].value = cur_node->value;
				}
				else
				{
					free(str_copy);
				}
			}
		}
	} // while end
	free(str);
	if (err_code)
	{
		for (ull i = 0; i < kv_cnt; ++i)
		{
			free(key_vals_tmp[i].key);
		}
		free(key_vals_tmp);
		return err_code;
	}
	*cnt = kv_cnt;
	*key_vals = key_vals_tmp;
	return OK;
}
