#pragma once

// Unrolled Linked Stack
//
// - Node size grows as elements are added

typedef struct ng_ustack_node
{
	struct ng_ustack_node* prev;
	struct ng_ustack_node* next;
	unsigned short capacity;
	unsigned short used;

	// type[capacity] elements;
} ng_ustack_node;

typedef struct ng_ustack
{
	ng_ustack_node* first;
	ng_ustack_node* last;

	unsigned int elementsize;
	unsigned int count;
} ng_ustack;

void ng_ustack_alloc(ng_ustack* stack, unsigned int elementsize);
void ng_ustack_free(ng_ustack* stack);

// Returns the element once coppied in
// Pushes and pops to the back
void* ng_ustack_push(ng_ustack* stack, void* element);
void ng_ustack_pop(ng_ustack* stack);


// Returns the element at the index within the stack
void* ng_ustack_at(ng_ustack* stack, unsigned int index);

// Returns the element at the index within the node
void* ng_ustack_node_at(ng_ustack* stack, ng_ustack_node* node, unsigned int index);

// Returns first and last elements of the stack
void* ng_ustack_first(ng_ustack* stack);
void* ng_ustack_last(ng_ustack* stack);