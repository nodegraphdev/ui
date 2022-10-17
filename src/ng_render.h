#pragma once

#include "ng_vertex.h"

// Vertex and Index buffer objects
typedef struct { unsigned int vao, vbo; } ng_vbo;
typedef struct { unsigned int ibo; } ng_ibo;


// Drawing functions
void ng_render_set_clear_colorf(float r, float g, float b);
void ng_render_set_clear_color8(unsigned char r, unsigned char g, unsigned char b);
void ng_render_set_clear_depth(float depth);
void ng_render_clear_frame();


// View 
void ng_render_viewport_push(int x, int y, int w, int h);
void ng_render_viewport_pop();
void ng_render_viewport_pop_all();


// Vertex & Index buffer objects
void ng_render_destroy_vbo(ng_vbo vbo);
void ng_render_destroy_ibo(ng_ibo ibo);

// Static vbo & ibo
ng_vbo ng_render_create_vbo(ng_vtxbuf* buf);
ng_ibo ng_render_create_ibo(ng_idxbuf* buf);

// Dynamic vbo & ibo
ng_ibo ng_render_create_ibo_dynamic(ng_idxbuf* buf);
ng_vbo ng_render_create_vbo_dynamic(ng_vtxbuf* buf);
void ng_render_fill_vbo_dynamic(ng_vbo vbo, ng_vtxbuf* buf);
void ng_render_fill_ibo_dynamic(ng_ibo ibo, ng_idxbuf* buf);

void ng_render_draw_mesh(ng_vbo vbo, ng_ibo ibo, unsigned int start, unsigned int count);
