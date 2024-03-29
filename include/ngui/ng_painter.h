#pragma once

#include "ng_math.h"

typedef void ng_paintctx;
typedef void ng_paintdraw;

ng_paintctx* ng_painter_create();
void ng_painter_destroy(ng_paintctx* ctx);

// Once built, a painter can be drawn many times
ng_paintdraw* ng_painter_build(ng_paintctx* ctx);
void ng_painter_draw(ng_paintdraw* draw);
void ng_painter_draw_at(ng_paintdraw* draw, nvec3 origin);
void ng_painter_draw_destroy(ng_paintdraw* draw);

// Paint brush
void ng_painter_set_color(ng_paintctx* ctx, float r, float g, float b);
void ng_painter_set_gradient(ng_paintctx* ctx, nvec2i p0, nvec2i p1, nvec3 color0, nvec3 color1 );

void ng_painter_rect(ng_paintctx* ctx, int x, int y, int width, int height);
void ng_painter_quad(ng_paintctx* ctx, nvec2i p0, nvec2i p1, nvec2i p2, nvec2i p3);
void ng_painter_tri(ng_paintctx* ctx, nvec2i p0, nvec2i p1, nvec2i p2);
void ng_painter_oval(ng_paintctx* ctx, nvec2i center, float radius_x, float radius_y);
void ng_painter_circle(ng_paintctx* ctx, nvec2i center, float radius);


// Temporary
void ng_painter_push_origin(nvec2i v);
void ng_painter_pop_origin();
void ng_painter_set_origin(nvec3 v);