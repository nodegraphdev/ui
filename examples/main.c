#include <stdio.h>
#include <string.h>
#include <ngui/ngui.h>
#include <ngui/ng_painter.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


nvec2i test_size(nvec2i max, void *data)
{
	return max;
}

void test_container_paint(nvec2i size, void *data)
{
	// printf("Painting test_container: %d,%d, %d children\n", size.x, size.y, ng_num_children());

	for (int i = 0; i < ng_num_children(); i++)
	{
		ng_child_paint(i, ng_child_size(i, size));
	}

	// printf("End test_container\n");
}

struct test_text_data
{
	char *prefix, *text;
	int width;
};

nvec2i test_text_size(nvec2i max, void *d)
{
	struct test_text_data *data = d;

	return (nvec2i){data->width, max.y};
}

void test_text_paint(nvec2i size, void *d)
{
	struct test_text_data *data = d;
	// printf("Painting text: %s%s\n", data->prefix, data->text);

	ng_paintctx *ctx = ng_painter_create();

	ng_painter_set_color(ctx, 0, 0, 0);
	ng_painter_rect(ctx, data->width, data->width, 7, 7);
	ng_painter_set_color(ctx, 255, 255, 255);
	ng_painter_rect(ctx, data->width+1, data->width+1, 5, 5);

	ng_paintdraw *draw = ng_painter_build(ctx);
	ng_painter_destroy(ctx);

	ng_painter_draw_at(draw, (nvec3){0, 0, 0});
	ng_painter_draw_destroy(draw);
}

void test_container()
{
	ng_begin_container(test_size, test_container_paint, NULL);
}

void test_text(char *text, int width)
{
	struct test_text_data *data = malloc(sizeof(struct test_text_data));
	data->prefix = ng_get_props("test-prefix");
	data->text = text;
	data->width = width;

	ng_add_render_object(test_text_size, test_text_paint, data);
}

void draw_ui()
{
	ng_reset_props();

	test_container();
	{
		ng_prop("test-prefix", "1-");
		test_text("A", 100);
		test_text("B", 80);

		ng_flex_begin();
		{
			ng_prop("test-prefix", "2-");
			ng_prop("flex", 1);
			test_text("C", 70);
			ng_prop("flex", 0);
			test_text("D", 60);
		}
		ng_end();

		ng_prop("test-prefix", "3-");
		test_text("E", 40);
	}
	ng_end();
}

int main()
{
	ng_init();
	printf("Hello!");

	ng_register_prop("test-prefix", NG_PROP_STRING, 0);


	/* ng_paintctx*	ng_init();
	printf("Hello!");

	ng_register_prop("test-prefix", NG_PROP_STRING);

 ctx = ng_painter_create(); */
	/* ng_painter_setcolor(ctx, 0.5, 0.5, 0.5); */
	/* ng_painter_rect(ctx, 0, 0, 100, 100); */
	/* ng_painter_setcolor(ctx, 0.75, 0.75, 0.75); */
	/* ng_painter_rect(ctx, 20, 20, 100, 100); */

	/* ng_paintdraw* draw = ng_painter_build(ctx); */

	while (ng_open())
	{
		ng_begin_frame();

		draw_ui();
		ng_commit();

		ng_end_frame();
	}

	ng_shutdown();

	return 0;
}
