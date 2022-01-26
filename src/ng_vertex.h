#pragma once
#include "ng_math.h"


// Vertex format
typedef struct ng_vertex
{
	nvec3 pos;
	nvec3 color;
	nvec2 uv;
} ng_vertex;



// Vertex buffer //

typedef struct ng_vtxbuf
{
    ng_vertex* buf;
    unsigned int capacity;
    unsigned int used;
} ng_vtxbuf;

void ng_vtxbuf_alloc(ng_vtxbuf* b, unsigned int capacity);
void ng_vtxbuf_free(ng_vtxbuf* b);

// Resizes the buffer to fit as many elements as needed
// Don't use this constantly!
void ng_vtxbuf_fit(ng_vtxbuf* b, unsigned int count);

// Pushes elements to the back of the buffer if there's space
// We could just pass back a pointer at the current pos, but this should be safer
void ng_vtxbuf_push(ng_vtxbuf* b, ng_vertex* ele);
void ng_vtxbuf_push_many(ng_vtxbuf* b, ng_vertex* arr, unsigned int count);

// Resets the head back to start. Next push will be at element zero
void ng_vtxbuf_reset(ng_vtxbuf* b);

void ng_vtxbuf_view(ng_vertex* arr, unsigned int size, ng_vtxbuf* out);




// Index buffer //

typedef struct ng_idxbuf
{
    unsigned short* buf;
    unsigned int capacity;
    unsigned int used;
} ng_idxbuf;

void ng_idxbuf_alloc(ng_idxbuf* b, unsigned int capacity);
void ng_idxbuf_free(ng_idxbuf* b);

// Resizes the buffer to fit as many elements as needed
// Don't use this constantly!
void ng_idxbuf_fit(ng_idxbuf* b, unsigned int count);

// Pushes elements to the back of the buffer if there's space
// We could just pass back a pointer at the current pos, but this should be safer
void ng_idxbuf_push(ng_idxbuf* b, unsigned short ele);
void ng_idxbuf_push_many(ng_idxbuf* b, unsigned short* arr, unsigned int count);
void ng_idxbuf_push_tri(ng_idxbuf* out, unsigned short start);
void ng_idxbuf_push_quad(ng_idxbuf* out, unsigned short start);

// Resets the head back to start. Next push will be at element zero
void ng_idxbuf_reset(ng_idxbuf* b);

void ng_idxbuf_view(unsigned short* arr, unsigned int size, ng_idxbuf* out);



// Mesh assembling //

// Allocates and fills an index buffer based on a vertex buffer and a pattern
void ng_idxbuf_fill_tris(ng_idxbuf* out, const ng_vtxbuf* vb);
void ng_idxbuf_fill_quads(ng_idxbuf* out, const ng_vtxbuf* vb);
void ng_idxbuf_fill_convexpolygon(ng_idxbuf* out, const ng_vtxbuf* vb);

