#include <stdio.h>
#include <string.h>
#include <ngui/ngui.h>
#include <ngui/ng_painter.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TEST_WIDTH 250
#define TEST_HEIGHT 50


nvec2i test_size(nvec2i max, void *data)
{
	return max;
}

void test_container_paint(nvec2i size, void *data)
{
	for (int i = 0; i < ng_num_children(); i++)
	{
		ng_child_paint(i, ng_child_size(i, size));
	}
}

nvec2i test_widget_size(nvec2i max, void *d)
{
	return (nvec2i){TEST_WIDTH, TEST_HEIGHT};
}

void test_widget_paint(nvec2i size, void *d)
{
	ng_paintctx *ctx = ng_painter_create();

	ng_painter_set_color(ctx, 0, 0, 0);
	ng_painter_rect(ctx, 0, 0, size.x, size.y);

	ng_painter_set_gradient(ctx, (nvec2i){0, 0}, size,
							(nvec3){0, 0, 1},
							(nvec3){0, 1, 0});
	ng_painter_rect(ctx, 5, 5, size.x-10, size.y-10);

	ng_paintdraw *draw = ng_painter_build(ctx);
	ng_painter_destroy(ctx);

	ng_painter_draw(draw);
	ng_painter_draw_destroy(draw);
}

void test_container()
{
	ng_begin_container(test_size, test_container_paint, NULL);
}

void test_widget()
{
	ng_add_render_object(test_widget_size, test_widget_paint, NULL);
}

void draw_ui()
{
	ng_reset_props();

	ng_prop("flex-direction", "column");
	ng_flex_begin();
	{
		test_widget();
		test_widget();
		test_widget();

		ng_prop("flex", 1);
		test_widget();
		ng_prop("flex", 0);
		test_widget();
		test_widget();

		ng_prop("flex", 2);
		test_widget();
	}
	ng_end();
}

int main()
{
	ng_init();
	printf("Hello!");

	ng_register_prop("test-prefix", NG_PROP_STRING, 0);


	int w = 400;
	int h = 80;
	int b = 10;
	ng_paintctx* ctx = ng_painter_create(); 

	// Top left border
	ng_painter_set_gradient(ctx, (nvec2i){b,b}, (nvec2i){0,h}, (nvec3){ 0.75f, 0.75f, 0.75f }, (nvec3){ 0.25f, 0.25f, 0.25f });
	ng_painter_quad(ctx, (nvec2i){0,0}, (nvec2i){0,h}, (nvec2i){b,h-b}, (nvec2i){b,b});
	ng_painter_set_gradient(ctx, (nvec2i){0,0}, (nvec2i){0,b}, (nvec3){ 0.85f, 0.85f, 0.85f }, (nvec3){ 0.65f, 0.65f, 0.65f });
	ng_painter_quad(ctx, (nvec2i){0,0}, (nvec2i){w,0}, (nvec2i){w-b,b}, (nvec2i){b,b});
	
	// Bottom right border
	ng_painter_set_gradient(ctx, (nvec2i){w-b,b}, (nvec2i){w,h}, (nvec3){ 0.75f, 0.75f, 0.75f }, (nvec3){ 0.25f, 0.25f, 0.25f });
	ng_painter_quad(ctx, (nvec2i){w,h}, (nvec2i){w,0}, (nvec2i){w-b,b}, (nvec2i){w-b,h-b});
	ng_painter_set_gradient(ctx, (nvec2i){0,h-b}, (nvec2i){0,h}, (nvec3){ 0.35f, 0.35f, 0.35f }, (nvec3){ 0.15f, 0.15f, 0.15f });
	ng_painter_quad(ctx, (nvec2i){w,h}, (nvec2i){0,h}, (nvec2i){b,h-b}, (nvec2i){w-b,h-b});

	// Content background
	ng_painter_set_gradient(ctx, (nvec2i){0,0}, (nvec2i){0,h/2}, (nvec3){ 0.65f, 0.65f, 0.65f }, (nvec3){ 0.55f, 0.55f, 0.55f });
	ng_painter_rect(ctx, b, b, w-b*2, h/2-b);
	ng_painter_set_gradient(ctx, (nvec2i){0,h/2}, (nvec2i){0,h}, (nvec3){ 0.55f, 0.55f, 0.55f }, (nvec3){ 0.35f, 0.35f, 0.35f });
	ng_painter_rect(ctx, b, h/2, w-b*2, h/2-b);

	ng_paintdraw* draw = ng_painter_build(ctx);
	ng_painter_destroy(ctx);


	ctx = ng_painter_create();
	ng_painter_set_color(ctx, 0, 255, 255);
	ng_painter_set_gradient(ctx, (nvec2i) { 0, 0 }, (nvec2i) { 40, 40 }, (nvec3) { 0, 0, 1 }, (nvec3) { 0, 1, 0 });
	ng_painter_rect(ctx, 0, 0, 40, 40);

	ng_paintdraw* rect = ng_painter_build(ctx);
	ng_painter_destroy(ctx);

	while (ng_open())
	{
		ng_begin_frame();

		draw_ui();
		ng_commit();

		// ng_painter_draw_at(draw, (nvec3) {60, 90, 0});

//		for (int i = 0; i < 10; i++)
//		{
//			ng_painter_push_origin((nvec2i) {20, 20});
//			ng_painter_draw(rect);
//		}
//		for (int i = 0; i < 10; i++)
//			ng_painter_pop_origin();

		ng_end_frame();
	}

	ng_painter_draw_destroy(draw);
	ng_shutdown();

	return 0;
}
