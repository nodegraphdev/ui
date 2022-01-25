#include <stdio.h>
#include <string.h>
#include <ngui/ngui.h>

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

void test_text_paint(nvec2i size, void *data)
{
	printf("Painting text: %s\n", data);
}

void test_container()
{
	ng_begin_container(test_size, test_container_paint, NULL);
}

void test_text(char *text)
{
	ng_add_render_object(test_size, test_text_paint, (void *) strdup(text));
}


int main()
{
	ng_init();
	printf("Hello!");

	test_container();
	{
		test_text("A");
		test_text("B");

		test_container();
		{
			test_text("C");
			test_text("D");
		}
		ng_end();

		test_text("E");
	}
	ng_end();

	ng_commit();

	return 0;
}
