#include "ng_render.h"
#include <glad/glad.h>
#include <stddef.h>

void ng_render_setviewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}
void ng_render_set_clear_colorf(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0f);
}
void ng_render_set_clear_color8(unsigned char r, unsigned char g, unsigned char b)
{
	glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}
void ng_render_set_clear_depth(float depth)
{
	glClearDepth(depth);
}
void ng_render_clear_frame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// Vertex & Index buffer objects
static ng_vbo ng_create_vbo_()
{
	GLuint vao, vbo;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	return (ng_vbo){ vao, vbo };
}
static ng_ibo ng_create_ibo_()
{
	GLuint ibo;

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	return (ng_ibo) { ibo };
}

void ng_render_destroy_vbo(ng_vbo vbo)
{
	glDeleteBuffers(1, &vbo.vbo);
	glDeleteVertexArrays(1, &vbo.vao);
}
void ng_render_destroy_ibo(ng_ibo ibo)
{
	glDeleteBuffers(1, &ibo.ibo);
}

// Static vbo & ibo
ng_vbo ng_render_create_vbo(ng_vtxbuf* buf)
{
	ng_vbo o = ng_create_vbo_();
	glBufferData(GL_ARRAY_BUFFER, buf->used * sizeof(ng_vertex), buf->buf, GL_STATIC_DRAW);
	return o;
}
ng_ibo ng_render_create_ibo(ng_idxbuf* buf)
{
	ng_ibo o = ng_create_ibo_();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * buf->used, buf->buf, GL_STATIC_DRAW);
	return o;
}

// Dynamic vbo & ibo
ng_vbo ng_render_create_vbo_dynamic(ng_vtxbuf* buf)
{
	ng_vbo o = ng_create_vbo_();
	glBufferData(GL_ARRAY_BUFFER, buf->used * sizeof(ng_vertex), buf->buf, GL_DYNAMIC_DRAW);
	return o;
}
ng_ibo ng_render_create_ibo_dynamic(ng_idxbuf* buf)
{
	ng_ibo o = ng_create_ibo_();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * buf->used, buf->buf, GL_STATIC_DRAW);
	return o;
}
void ng_render_fill_vbo_dynamic(ng_vbo vbo, ng_vtxbuf* buf)
{
	glBindVertexArray(vbo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.vbo);
	glBufferData(GL_ARRAY_BUFFER, buf->used * sizeof(ng_vertex), buf->buf, GL_DYNAMIC_DRAW);
}
void ng_render_fill_ibo_dynamic(ng_ibo ibo, ng_idxbuf* buf)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * buf->used, buf->buf, GL_DYNAMIC_DRAW);
}

void ng_render_draw_mesh(ng_vbo vbo, ng_ibo ibo, unsigned int start, unsigned int count)
{
	glBindVertexArray(vbo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		0,                              // position 0
		3,                              // three floats
		GL_FLOAT,                       // elements are floats
		GL_FALSE,                       //
		sizeof(ng_vertex),              // stride
		(void*)offsetof(ng_vertex, pos) // position within stride
	);
	glVertexAttribPointer(
		1,                                // position 1
		3,                                // three floats
		GL_FLOAT,                         // elements are floats
		GL_FALSE,                         //
		sizeof(ng_vertex),                // stride
		(void*)offsetof(ng_vertex, color) // position within stride
	);
	glVertexAttribPointer(
		2,                             // position 2
		2,                             // two floats
		GL_FLOAT,                      // elements are floats
		GL_FALSE,                      //
		sizeof(ng_vertex),             // stride
		(void*)offsetof(ng_vertex, uv) // position within stride
	);
	glVertexAttribPointer(
		3,                                // position 3
		1,                                // one short
		GL_FLOAT,                         // elements are shorts
		GL_FALSE,                         //
		sizeof(ng_vertex),                // stride
		(void*)offsetof(ng_vertex, glyph) // position within stride
	);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.ibo);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (void*)(start * sizeof(unsigned short)));


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}
