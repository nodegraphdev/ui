#include "ng_painter.h"
#include "ng_vertex.h"
#include "ng_ustack.h"
#include "ng_render.h"
#include "ng_shader.h"

#include <stdlib.h>
#include <string.h>

enum
{
	NG_PAINT_BRUSH_SOLID,
	NG_PAINT_BRUSH_GRADIENT,
};
enum
{
	NG_PAINTCMD_NONE,
	NG_PAINTCMD_TRI,
	NG_PAINTCMD_RECT,
	NG_PAINTCMD_QUAD,
};

// Current visual state of the next draw
typedef struct ng_paintbrush
{
	int mode;
	union
	{
		nvec3 color;
		struct
		{
			nvec2i origin, delta;
			nvec3 color0, color1;
		};
	};
} ng_paintbrush;

// Geometry and shape of the next draw
typedef struct ng_paintcmd
{
	int type;
	ng_paintbrush brush;
	union 
	{
		nvec2i quad[4];
		nvec2i rect[2];
		nvec2i tri[3];
	};
} ng_paintcmd;

// In progress geometry. Same as ng_paintctx 
typedef struct ng_paintdata
{
	unsigned int vertexestimate;
	unsigned int indexestimate;

	ng_paintbrush brush;

	ng_ustack cmds;
} ng_paintdata;

// Complete painter. Ready to be drawn
typedef struct ng_paintdrawdata
{
	ng_vbo vbo;
	ng_ibo ibo;

	// TODO: make these into arrays once we have drawcalls
	ng_shader* shader;
	unsigned int start;
	unsigned int count;
} ng_paintdrawdata;


// TODO: These two need to get moved elsewhere 
// Default shaders for the painter
static const char* ng_painter_vertexshader =
"#version 330 core\n"
"layout(location = 0) in vec3 a_pos;"
"layout(location = 1) in vec3 a_color;"
"layout(location = 2) in vec2 a_uv;"
"uniform mat4 u_model;"
"uniform mat4 u_view;"
"uniform mat4 u_projection;"
"out vec3 v_color;"
"out vec4 v_worldpos;"
"void main(){"
"    v_color=a_color;"
"    gl_Position = u_projection * u_view * u_model * vec4(a_pos.xyz, 1.0);"
"}";
static const char* ng_painter_fragmentshader =
"#version 330 core\n"
"precision lowp float;"
"in vec3 v_color;"
"out vec4 o_fragColor;"
"void main() {"
"    o_fragColor = vec4(v_color.xyz, 1.0);"
"}";


static ng_shader* ng_painter_default_shader = 0;


static void ng_painter_submit_(ng_paintdata* data, ng_paintcmd* cmd)
{
	ng_paintcmd* p = ng_ustack_push(&data->cmds, cmd);
	p->brush = data->brush;
}


// Internal 
void ng_painter_init_()
{
	ng_painter_default_shader = ng_shader_create(ng_painter_vertexshader, ng_painter_fragmentshader);
}
void ng_painter_shutdown_()
{
	ng_shader_destroy(ng_painter_default_shader);
}


// Painter
ng_paintctx* ng_painter_create()
{
	ng_paintdata* data = malloc(sizeof(ng_paintdata));
	data->vertexestimate = 0;
	data->indexestimate = 0;

	memset(&data->brush, 0, sizeof(ng_paintbrush));

	ng_ustack_alloc(&data->cmds, sizeof(ng_paintcmd));

	return data;
}
void ng_painter_destroy(ng_paintctx* ctx)
{
	ng_paintdata* data = ctx;
	ng_ustack_free(&data->cmds);
	free(data);
}

void ng_painter_set_color(ng_paintctx* ctx, float r, float g, float b)
{
	ng_paintdata* data = ctx;
	data->brush.mode = NG_PAINT_BRUSH_SOLID;
	data->brush.color = (nvec3){r,g,b};
}
void ng_painter_set_gradient(ng_paintctx* ctx, nvec2i p0, nvec2i p1, nvec3 color0, nvec3 color1)
{
	ng_paintdata* data = ctx;
	data->brush.mode = NG_PAINT_BRUSH_GRADIENT;
	data->brush.origin = p0;
	data->brush.delta = ng_subv2i(p1, p0);
	data->brush.color0 = color0;
	data->brush.color1 = color1;


}


// Painting commands //
void ng_painter_rect(ng_paintctx* ctx, int x, int y, int width, int height)
{
	ng_paintdata* data = ctx;
	data->vertexestimate += 4;
	data->indexestimate  += 6;

	ng_paintcmd cmd;
	cmd.type = NG_PAINTCMD_RECT;
	cmd.rect[0] = (nvec2i){ x, y };
	cmd.rect[1] = (nvec2i){ width, height };
	ng_painter_submit_(data, &cmd);
}
void ng_painter_quad(ng_paintctx* ctx, nvec2i p0, nvec2i p1, nvec2i p2, nvec2i p3)
{
	ng_paintdata* data = ctx;
	data->vertexestimate += 4;
	data->indexestimate += 6;

	ng_paintcmd cmd;
	cmd.type = NG_PAINTCMD_QUAD;
	cmd.quad[0] = p0;
	cmd.quad[1] = p1;
	cmd.quad[2] = p2;
	cmd.quad[3] = p3;
	ng_painter_submit_(data, &cmd);
}
void ng_painter_tri(ng_paintctx* ctx, nvec2i p0, nvec2i p1, nvec2i p2)
{
	ng_paintdata* data = ctx;
	data->vertexestimate += 3;
	data->indexestimate += 3;

	ng_paintcmd cmd;
	cmd.type = NG_PAINTCMD_TRI;
	cmd.tri[0] = p0;
	cmd.tri[1] = p1;
	cmd.tri[2] = p2;
	ng_painter_submit_(data, &cmd);
}


// Geometry Building //
static nvec3 ng_brush_vtx_color_(ng_paintbrush* brush, nvec2i position)
{
	nvec3 color = { 0,0,0 };
	switch (brush->mode)
	{
	case NG_PAINT_BRUSH_SOLID:
	{
		color = brush->color;
		break;
	}
	case NG_PAINT_BRUSH_GRADIENT:
	{
		float bias = ng_dotv2i(ng_subv2i(position, brush->origin), brush->delta) / (float)ng_dotv2i(brush->delta, brush->delta);
		if (bias < 0.0f)
			bias = 0.0f;
		else if (bias > 1.0f)
			bias = 1.0f;

		color = ng_addv3(ng_scalev3(brush->color0, 1.0f - bias), ng_scalev3(brush->color1, bias));
		break;
	}

	default:
		break;
	}

	return color;
}

static void ng_brush_paint_vertices_(ng_paintbrush* brush, ng_vertex* v, int count)
{
	for (int i = 0; i < count; i++)
	{
		nvec3 p = v[i].pos;
		v[i].color = ng_brush_vtx_color_(brush, (nvec2i){ p.x, p.y });
	}
}

ng_paintdraw* ng_painter_build(ng_paintctx* ctx)
{
	ng_paintdata* data = ctx;

	ng_vtxbuf vb;
	ng_idxbuf ib;
	ng_vtxbuf_alloc(&vb, data->vertexestimate);
	ng_idxbuf_alloc(&ib, data->indexestimate);

	// TODO: Need a way to split up draw calls at some point!

	// Draw order is based on depth
	// Increase the z for each cmd
	float z = 0;

	// Loop all cmds
	for (ng_ustack_node* n = data->cmds.first; n; n = n->next)
		for(int i = 0; i < n->used; i++)
		{
			ng_paintcmd* cmd = ng_ustack_node_at(&data->cmds, n, i);
			ng_paintbrush* brush = &cmd->brush;

			// Build vbo & ibo based on type
			switch (cmd->type)
			{
			case NG_PAINTCMD_TRI:
			{
				unsigned int i = vb.used;
				ng_idxbuf_push_tri(&ib, i);

				ng_vertex a[] = {
					{{cmd->tri[0].x, cmd->tri[0].y, z}, {0,0,0}, {0,0}},
					{{cmd->tri[1].x, cmd->tri[1].y, z}, {0,0,0}, {0,0}},
					{{cmd->tri[2].x, cmd->tri[2].y, z}, {0,0,0}, {0,0}},
				};
				ng_brush_paint_vertices_(brush, &a[0], 4);

				ng_vtxbuf_push_many(&vb, &a[0], 3);
				break;
			}
			case NG_PAINTCMD_RECT:
			{
				unsigned int i = vb.used;
				ng_idxbuf_push_quad(&ib, i);

				ng_vertex a[] = {
					{{cmd->rect[0].x,                  cmd->rect[0].y,                  z}, {0,0,0}, {0,0}},
					{{cmd->rect[0].x + cmd->rect[1].x, cmd->rect[0].y,                  z}, {0,0,0}, {1,0}},
					{{cmd->rect[0].x + cmd->rect[1].x, cmd->rect[0].y + cmd->rect[1].y, z}, {0,0,0}, {1,1}},
					{{cmd->rect[0].x,                  cmd->rect[0].y + cmd->rect[1].y, z}, {0,0,0}, {0,1}},
				};
				ng_brush_paint_vertices_(brush, &a[0], 4);

				ng_vtxbuf_push_many(&vb, &a[0], 4);
				break;
			}
			case NG_PAINTCMD_QUAD:
			{
				unsigned int i = vb.used;
				ng_idxbuf_push_quad(&ib, i);

				ng_vertex a[] = {
					{{cmd->quad[0].x, cmd->quad[0].y, z}, {0,0,0}, {0,0}},
					{{cmd->quad[1].x, cmd->quad[1].y, z}, {0,0,0}, {1,0}},
					{{cmd->quad[2].x, cmd->quad[2].y, z}, {0,0,0}, {1,1}},
					{{cmd->quad[3].x, cmd->quad[3].y, z}, {0,0,0}, {0,1}},
				};
				ng_brush_paint_vertices_(brush, &a[0], 4);

				ng_vtxbuf_push_many(&vb, &a[0], 4);
				break;
			}
			}

			// Increase z for each cmd
			z += 0.1f;
		}


	// Data should all be in place now. Generate the renderables 
	ng_vbo vbo = ng_render_create_vbo(&vb);
	ng_ibo ibo = ng_render_create_ibo(&ib);

	ng_paintdrawdata* draw = malloc(sizeof(ng_paintdrawdata));
	draw->vbo = vbo;
	draw->ibo = ibo;
	draw->start = 0;
	draw->count = ib.used;
	draw->shader = ng_painter_default_shader;

	ng_vtxbuf_free(&vb);
	ng_idxbuf_free(&ib);

	return draw;
}


// Paint drawing //

void ng_painter_draw_destroy(ng_paintdraw* draw)
{
	ng_paintdrawdata* data = draw;

	ng_render_destroy_ibo(data->ibo);
	ng_render_destroy_vbo(data->vbo);
	free(draw);
}

void ng_painter_draw(ng_paintdraw* draw)
{
	ng_paintdrawdata* data = draw;

	ng_shader_bind(data->shader);
	ng_render_draw_mesh(data->vbo, data->ibo, data->start, data->count);
}

void ng_painter_draw_at(ng_paintdraw* draw, nvec3 origin)
{
	// This should get glommed up into some kind of transform helper later on
	nmat4x4 model;
	ng_identity4x4(&model);
	model.d = (nvec4){ origin.x, origin.y, origin.z, 1.0f };
	ng_shader_set(NG_SHADERPARAM_MODEL, &model);

	ng_painter_draw(draw);
}
