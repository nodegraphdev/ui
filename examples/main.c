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

	while (ng_open())
	{
		ng_begin_frame();

		//draw_ui();
		ng_commit();


		ng_end_frame();
	}

	ng_shutdown();

	return 0;
}
