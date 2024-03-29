#include "ng_vertex.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Vertex buffer //

void ng_vtxbuf_alloc(ng_vtxbuf* b, unsigned int capacity)
{
    b->capacity = capacity;
    b->used = 0;
    b->buf = (ng_vertex*)malloc(sizeof(ng_vertex) * capacity);
}

void ng_vtxbuf_free(ng_vtxbuf* b)
{
    if (b->buf)
        free(b->buf);

    b->capacity = 0;
    b->used = 0;
    b->buf = 0;
}

void ng_vtxbuf_fit(ng_vtxbuf* b, unsigned int count)
{
    if (b->capacity >= count)
        return;

    // Reallocate and copy in the old contents. Slow!
    ng_vertex* n = malloc(count * sizeof(ng_vertex));
    memcpy(n, b->buf, b->capacity * sizeof(ng_vertex));
    free(b->buf);

    b->buf = n;
    b->capacity = count;
}

void ng_vtxbuf_push(ng_vtxbuf* b, ng_vertex* ele)
{
    // Are we full?
    if (b->used >= b->capacity)
    {
        assert(0);
        return;
    }

    b->buf[b->used] = *ele;
    b->used++;
}

void ng_vtxbuf_push_many(ng_vtxbuf* b, ng_vertex* arr, unsigned int count)
{
    // Are we full?
    if (b->used >= b->capacity)
    {
        assert(!count);
        return;
    }

    // Cap our copy to prevent overfilling
    if (b->used + count > b->capacity)
        count = b->capacity - b->used;

    memcpy(b->buf + b->used, arr, count * sizeof(ng_vertex));
    b->used += count;
}

void ng_vtxbuf_reset(ng_vtxbuf* b)
{
    b->used = 0;
}

void ng_vtxbuf_view(ng_vertex* arr, unsigned int size, ng_vtxbuf* out)
{
    unsigned int count = size / sizeof(ng_vertex);
    out->buf = arr;
    out->capacity = count;
    out->used = count;
}



// Index buffer //

void ng_idxbuf_alloc(ng_idxbuf* b, unsigned int capacity)
{
    b->capacity = capacity;
    b->used = 0;
    b->buf = (unsigned short*)malloc(sizeof(unsigned short) * capacity);
}

void ng_idxbuf_free(ng_idxbuf* b)
{
    if (b->buf)
        free(b->buf);

    b->capacity = 0;
    b->used = 0;
    b->buf = 0;
}

void ng_idxbuf_fit(ng_idxbuf* b, unsigned int count)
{
    if (b->capacity >= count)
        return;

    // Reallocate and copy in the old contents. Slow!
    unsigned short* n = malloc(count * sizeof(unsigned short));
    memcpy(n, b->buf, b->capacity * sizeof(unsigned short));
    free(b->buf);

    b->buf = n;
    b->capacity = count;
}

void ng_idxbuf_push(ng_idxbuf* b, unsigned short ele)
{
    // Are we full?
    if (b->used >= b->capacity)
    {
        assert(0);
        return;
    }

    b->buf[b->used] = ele;
    b->used++;
}

void ng_idxbuf_push_many(ng_idxbuf* b, unsigned short* arr, unsigned int count)
{
    // Are we full?
    if (b->used >= b->capacity)
    {
        assert(!count);
        return;
    }

    // Cap our copy to prevent overfilling
    if (b->used + count > b->capacity)
        count = b->capacity - b->used;

    memcpy(b->buf + b->used, arr, count * sizeof(unsigned short));
    b->used += count;
}

void ng_idxbuf_reset(ng_idxbuf* b)
{
    b->used = 0;
}

void ng_idxbuf_view(unsigned short* arr, unsigned int size, ng_idxbuf* out)
{
    unsigned int count = size / sizeof(unsigned short);
    out->buf = arr;
    out->capacity = count;
    out->used = count;
}



// Mesh assembling //


void ng_idxbuf_push_tri(ng_idxbuf* b, unsigned short start)
{
    unsigned short a[] = { 
        start + 0, 
        start + 1, 
        start + 2 
    };
    ng_idxbuf_push_many(b, &a[0], 3);
}
void ng_idxbuf_push_quad(ng_idxbuf* b, unsigned short start)
{
    unsigned short a[] = { 
        start + 0,
        start + 1,
        start + 2,
    
        start + 2,
        start + 3,
        start + 0
    };
    ng_idxbuf_push_many(b, &a[0], 6);
}


void ng_idxbuf_push_convexpoly(ng_idxbuf* b, unsigned short start, unsigned short count)
{
    int idxCount = (count - 2) * 3;

    unsigned short* f = b->buf + b->used;
    b->used += idxCount;

    for (unsigned int i = 1; i < count - 1; i++)
    {
        f[0] = start;
        f[1] = start + i;
        f[2] = start + i + 1;
        f += 3;
    }
}
