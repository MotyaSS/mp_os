#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fibonacci_heap.h"

void fib_node_destruct(fib_node* node)
{
	if (node == NULL)
	{
		return;
	}

	if (node->left != NULL)
	{
		node->left->right = NULL;
	}
	
	fib_node_destruct(node->child);
	fib_node_destruct(node->right);

	request_destruct(node->req);
	free_all(2, node->req, node);
}

status_code fib_node_copy(fib_node** node_dest, const fib_node* node_src, const fib_node* barrier)
{
	if (node_dest == NULL)
	{
		return NULL_ARG;
	}
	
	if (node_src == NULL || node_src == barrier)
	{
		*node_dest = NULL;
		return OK;
	}
	
	*node_dest = (fib_node*) malloc(sizeof(fib_node));
	if (*node_dest == NULL)
	{
		return BAD_ALLOC;
	}
	
	(*node_dest)->rank = node_src->rank;
	(*node_dest)->req = (request*) malloc(sizeof(request));
	if ((*node_dest)->req == NULL)
	{
		free(*node_dest);
		return BAD_ALLOC;
	}
	
	status_code code = request_copy((*node_dest)->req, node_src->req);
	if (code)
	{
		free((*node_dest)->req);
		free(*node_dest);
		return code;
	}
	
	barrier = barrier == NULL ? node_src : barrier;
	
	code = code ? code : fib_node_copy(&(*node_dest)->child, node_src->child, NULL);
	code = code ? code : fib_node_copy(&(*node_dest)->right, node_src->right, barrier);
	
	if (code)
	{
		fib_node_destruct(*node_dest);
		return code;
	}
	
	if ((*node_dest)->right != NULL)
	{
		(*node_dest)->left = (*node_dest)->right->left;
		(*node_dest)->left->right = *node_dest;
		(*node_dest)->right->left = *node_dest;
	}
	else
	{
		(*node_dest)->left = (*node_dest);
	}
	
	return OK;
}


status_code fib_heap_construct(fib_heap* fh, int (*compare)(const request*, const request*))
{
	if (fh == NULL)
	{
		return NULL_ARG;
	}

	fh->head = NULL;
	fh->compare = compare;
	fh->size = 0;

	return OK;
}

status_code fib_heap_set_null(fib_heap* fh)
{
	if (fh == NULL)
	{
		return NULL_ARG;
	}

	fh->head = NULL;
	fh->compare = NULL;
	fh->size = 0;

	return OK;
}

status_code fib_heap_destruct(fib_heap* fh)
{
	if (fh == NULL)
	{
		return OK;
	}

	fib_node_destruct(fh->head);
	fib_heap_set_null(fh);

	return OK;
}

status_code fib_heap_copy(fib_heap* fh_dest, const fib_heap* fh_src)
{
	if (fh_dest == NULL || fh_src == NULL)
	{
		return NULL_ARG;
	}

	status_code status = fib_heap_construct(fh_dest, fh_src->compare);
	status = status ? status : fib_node_copy(&(fh_dest->head), fh_src->head, NULL);
	if (status != OK)
	{
		fib_heap_destruct(fh_dest);
		return status;
	}
	
	fh_dest->size = fh_src->size;

	return OK;
}

status_code fib_heap_meld(fib_heap* fh_res, fib_heap* fh_l, fib_heap* fh_r)
{
	if (fh_res == NULL || fh_l == NULL || fh_r == NULL)
	{
		return NULL_ARG;
	}
	
	if (fh_l == fh_r || fh_l->compare != fh_r->compare)
	{
		return INVALID_INPUT;
	}
	
	if (fh_l->head == NULL || fh_r->head == NULL)
	{
		fh_res->head = fh_l->head == NULL ? fh_r->head : fh_l->head;
		fib_heap_set_null(fh_l);
		fib_heap_set_null(fh_r);
		return OK;
	}
	
	fib_heap fh_tmp;
	
	fh_tmp.size = fh_l->size + fh_r->size;
	fh_tmp.compare = fh_l->compare;
	
	if (fh_l->head == NULL || fh_r->head == NULL)
	{
		fh_tmp.head = fh_l->head == NULL ? fh_r->head : fh_l->head;
	}
	else
	{
		fh_l->head->left->right = fh_r->head;
		fh_l->head->left = fh_r->head->left;
		fh_r->head->left = fh_l->head->left;
		fh_r->head->left->right = fh_l->head;
		
		fh_tmp.head = fh_l->compare(fh_l->head->req, fh_r->head->req) <= 0 ? fh_l->head : fh_r->head;
	}
	
	fh_l->size = fh_r->size = 0;
	fh_l->head = fh_r->head = NULL;
	*fh_res = fh_tmp;
	
	return OK;
}

status_code fib_heap_copy_meld(fib_heap* fh_res, const fib_heap* fh_l, const fib_heap* fh_r)
{
	if (fh_res == NULL || fh_l == NULL || fh_r == NULL)
	{
		return NULL_ARG;
	}
	
	if (fh_res == fh_l || fh_res == fh_r)
	{
		return INVALID_INPUT;
	}
	
	status_code code = OK;
    fib_heap fh_lc, fh_rc;
        
    fib_heap_set_null(&fh_lc);
    fib_heap_set_null(&fh_rc);
    fib_heap_set_null(fh_res);
    
    code = code ? code : fib_heap_copy(&fh_lc, fh_l);
    code = code ? code : fib_heap_copy(&fh_rc, fh_r);
    code = code ? code : fib_heap_meld(fh_res, &fh_lc, &fh_rc);
    
    fib_heap_destruct(&fh_lc);
    fib_heap_destruct(&fh_rc);
    
    if (code)
    {
        fib_heap_destruct(fh_res);
        return code;
    }
    
    return OK;
}

status_code fib_heap_size(const fib_heap* fh, size_t* size)
{
	if (fh == NULL)
	{
		return NULL_ARG;
	}

	*size = fh->size;

	return OK;
}

status_code fib_node_merge(fib_heap* fh, fib_node* node_1, fib_node* node_2, fib_node** res)
{
	if (fh == NULL || node_1 == NULL || node_2 == NULL || res == NULL)
	{
		return NULL_ARG;
	}
	
	if (node_1 == node_2 || node_1->rank != node_2->rank)
	{
		return INVALID_INPUT;
	}
	
	if (fh->compare(node_1->req, node_2->req) > 0)
	{
		fib_node* tmp = node_1;
		node_1 = node_2;
		node_2 = tmp;
	}
	
	// hang node_2 on node_1
	if (node_1->child != NULL)
	{
		node_2->right = node_1->child;
		node_2->left = node_1->child->left;
		node_2->left->right = node_2;
		node_2->right->left = node_2;
	}
	else
	{
		node_2->left = node_2;
		node_2->right = node_2;
	}
	
	node_1->child = node_2;
	node_1->rank++;
	
	*res = node_1;

	return OK;
}

status_code fib_heap_consolidate(fib_heap* fh)
{
	if (fh == NULL)
	{
		return NULL_ARG;
	}
	
	if (fh->head == NULL || fh->head->right == fh->head)
	{
		return OK;
	}

	status_code code = OK;
	fib_node* cur = fh->head;
	fib_node* nxt = fh->head->right;
	size_t size = log2(1.0 * fh->size) + 3;

	fib_node** arr = (fib_node**) calloc(size, sizeof(fib_node*));
	if (arr == NULL)
	{
		return BAD_ALLOC;
	}
	
	fh->head->left->right = NULL;
	
	while (!code && cur != NULL)
	{
		nxt = cur->right;
		
		if (cur->right == cur)
		{
			cur->right = NULL;
		}
		
		while (!code && arr[cur->rank] != NULL)
		{
			fib_node* node_tmp = arr[cur->rank];
			arr[cur->rank] = NULL;
			code = fib_node_merge(fh, cur, node_tmp, &cur);
			
			if (code)
			{
				arr[size - 1] = cur;
			}
		}
		
		arr[cur->rank] = cur;
		cur = nxt;
	}
	
	fib_node* top = NULL;
	fib_node* start = NULL;
	fib_node* prev = NULL;
	
	for (size_t i = 0; i < size; ++i)
	{
		if (arr[i] == NULL)
		{
			continue;
		}
		
		start = start != NULL ? start : arr[i];
		top = top != NULL ? top : arr[i];
		
		if (fh->compare(arr[i]->req, top->req) < 0)
		{
			top = arr[i];
		}
		
		if (prev != NULL)
		{
			prev->right = arr[i];
		}
		
		arr[i]->left = prev;
		prev = arr[i];
	}
	
	prev->right = start;
	start->left = prev;
	fh->head = top;
	free(arr);
	
	return code;
}

status_code fib_heap_top(const fib_heap* fh, request** req)
{
	if (fh == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	
	if (fh->head == NULL)
	{
		req = NULL;
		return OK;
	}

	*req = fh->head->req;

	return OK;
}

status_code fib_heap_pop(fib_heap* fh, request** req)
{
	if (fh == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	
	if (fh->head == NULL)
	{
		*req = NULL;
		return OK;
	}
	
	fib_node* top = fh->head;
	*req = fh->head->req;
	
	if (fh->head->right == fh->head && fh->head->child == NULL)
	{
		fh->head = NULL;
	}
	else if (fh->head->right == fh->head)
	{
		fh->head = fh->head->child;
	}
	else if (fh->head->child == NULL)
	{
		fh->head->left->right = fh->head->right;
		fh->head->right->left = fh->head->left;
		fh->head = fh->head->left;
	}
	else
	{
		fh->head->left->right = fh->head->child;
		fh->head->right->left = fh->head->child->left;
		fh->head->left->right->left = fh->head->left;
		fh->head->right->left->right = fh->head->right;
		fh->head = fh->head->left;
	}
	
	free(top);
	fh->size--;
	
	return fib_heap_consolidate(fh);
}

status_code fib_heap_insert(fib_heap* fh, request* req)
{
	if (fh == NULL || req == NULL)
	{
		return NULL_ARG;
	}
	
	fib_node* node = (fib_node*)malloc(sizeof(fib_node));
	if (node == NULL)
	{
		return BAD_ALLOC;
	}
	
	node->req = req;
	node->child = NULL;
	node->rank = 0;
	
	if (fh->head == NULL)
	{
		node->left = node->right = node;
		fh->head = node;
	}
	else
	{	
		node->left = fh->head->left;
		node->right = fh->head;
		node->left->right = node;
		node->right->left = node;

		if (fh->compare(node->req, fh->head->req) < 0)
		{
			fh->head = node;
		}
	}
	
	fh->size++;
	
	return OK;
}
