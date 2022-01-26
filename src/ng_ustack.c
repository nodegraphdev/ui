#include "ng_ustack.h"
#include <stdlib.h>
#include <string.h>

// TODO:
//	Right now this frees nodes on pop when their "used" count is 0
//  It might be good to benchmark this at some point and check if it's worth it to preserve those nodes

// Memory growth pattern
#define NG_USTACK_STARTSIZE 4
unsigned int ng_ustack_nextsize(unsigned int sz)
{
	sz *= 2;
	if (sz > 64)
		return 64;
	return sz;
}


void ng_ustack_alloc(ng_ustack* stack, unsigned int elementsize)
{
	stack->first = 0;
	stack->last = 0;
	stack->count = 0;
	stack->elementsize = elementsize;
}
void ng_ustack_free(ng_ustack* stack)
{
	ng_ustack_node* next = 0;
	for (ng_ustack_node* n = stack->first; n; n = next)
	{
		next = n->next;
		free(n);
	}

	stack->first = 0;
	stack->last = 0;
	stack->count = 0;
	stack->elementsize = 0;
}


void* ng_ustack_push(ng_ustack* stack, void* element)
{
	// Find some space
	ng_ustack_node* node = 0;
	ng_ustack_node* prev = stack->last;

	if (prev && prev->used < prev->capacity)
	{
		// Our prev node is fine to use
		node = prev;
	}
	else
	{
		// Didn't find space! Make a new node

		// Grow from our prev size
		unsigned int nodesize = NG_USTACK_STARTSIZE;
		if (prev)
			nodesize = prev->capacity;
		nodesize = ng_ustack_nextsize(nodesize);

		// Allocate with size for the struct and the data
		node = malloc(sizeof(ng_ustack_node) + nodesize * stack->elementsize);
		node->next = 0;
		node->capacity = nodesize;
		node->used = 0;
		
		// Link it up
		if (prev)
			prev->next = node;
		if (!stack->first)
			stack->first = node;
		node->prev = prev;
		stack->last = node;
	}

	// Copy it in
	void* dest = ng_ustack_node_at(stack, node, node->used);
	memcpy(dest, element, stack->elementsize);
	node->used++;
	stack->count++;

	return dest;
}

void ng_ustack_pop(ng_ustack* stack)
{
	ng_ustack_node* node = stack->last;

	if (!node)
		return;

	if (node->used == 1)
	{
		// Pop the entire node
		
		// Unlink our node
		ng_ustack_node* prev = node->prev;
		if (prev)
		{
			prev->next = 0;
			stack->last = prev;
		}
		else
		{
			// No more nodes
			stack->first = 0;
			stack->last = 0;
		}

		// Bye bye!
		free(node);
	}
	else
	{
		// Just shift down one
		node->used--;
	}

	stack->count--;
}


void* ng_ustack_at(ng_ustack* stack, unsigned int index)
{
	if (index >= stack->count)
		return 0;

	ng_ustack_node* node = 0;
	for (node = stack->first; node; node = node->next)
	{
		if (index < node->used)
			break;
		index -= node->used;
	}

	if (!node || index > node->used)
		return 0;
	return ng_ustack_node_at(stack, node, index);
}

void* ng_ustack_node_at(ng_ustack* stack, ng_ustack_node* node, unsigned int index)
{
	return (char*)(node) + sizeof(ng_ustack_node) + stack->elementsize * index;
}


void* ng_ustack_first(ng_ustack* stack)
{
	ng_ustack_node* node = stack->first;
	if (node && node->used > 0)
		return ng_ustack_node_at(stack, node, 0);
	return 0;
}

void* ng_ustack_last(ng_ustack* stack)
{
	ng_ustack_node* node = stack->last;
	if (node && node->used > 0)
		return ng_ustack_node_at(stack, node, node->used - 1);
	return 0;
}