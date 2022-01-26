#include <stdio.h>
#include <string.h>
#include <ngui/ngui.h>
#include <ngui/ng_painter.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


nvec2i test_size(nvec2i min, void *data)
{
	return min;
}

void test_container_paint(nvec2i size, void *data)
{
	printf("Painting test_container: %d,%d, %d children\n", size.x, size.y, ng_num_children());

	for (int i = 0; i < ng_num_children(); i++)
	{
		ng_paint_child(i, ng_size_child(i, size));
	}
	printf("End test_container\n");
}

struct test_text_data
{
	char *prefix, *text;
};

void test_text_paint(nvec2i size, void *d)
{
	struct test_text_data *data = d;
	printf("Painting text: %s%s\n", data->prefix, data->text);
}

void test_container()
{
	ng_begin_container(test_size, test_container_paint, NULL);
}

void test_text(char *text)
{
	struct test_text_data *data = malloc(sizeof(struct test_text_data));
	data->prefix = ng_get_props("test-prefix");
	data->text = text;

	ng_add_render_object(test_size, test_text_paint, data);
}


int main()
{
	ng_init();
	printf("Hello!");

	ng_register_prop("test-prefix", NG_PROP_STRING);

	ng_reset_props();

	test_container();
	{
		ng_prop("test-prefix", "1-");
		test_text("A");
		test_text("B");

		test_container();
		{
			ng_prop("test-prefix", "2-");
			test_text("C");
			test_text("D");
		}
		ng_end();

		ng_prop("test-prefix", "3-");
		test_text("E");
	}
	ng_end();

	ng_commit();


	ng_paintctx* ctx = ng_painter_create();
	ng_painter_setcolor(ctx, 0.5, 0.5, 0.5);
	ng_painter_rect(ctx, 0, 0, 100, 100);
	ng_painter_setcolor(ctx, 0.75, 0.75, 0.75);
	ng_painter_rect(ctx, 20, 20, 100, 100);

	ng_paintdraw* draw = ng_painter_build(ctx);


	while (ng_open())
	{
		ng_begin_frame();

		
		ng_painter_draw_at(draw, (nvec3){0,0,0});

		ng_painter_draw_at(draw, (nvec3){ 600.0f * (0.5 + 0.5 * cos(clock() / (float)CLOCKS_PER_SEC)), 200.0f, 0.0f });

		ng_end_frame();
	}

	ng_shutdown();

	return 0;
}
