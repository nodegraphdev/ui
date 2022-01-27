#include <ngui.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum
{
	NG_FLEX_ROW,
	NG_FLEX_COLUMN,
};

struct ng_flex_child
{
	int flex;
};

struct ng_flex_data
{
	int flex_direction;
};

static nvec2i ng_flex_size_(nvec2i max, void *data)
{
	return max;
}

static void ng_flex_paint_(nvec2i size, void *data)
{
	for (int i = 0; i < ng_num_children(); i++)
	{
		printf("flex = %d\n", ng_child_get_propi(i, "flex"));
		ng_child_paint(i, ng_child_size(i, size));
	}
}

void ng_flex_begin()
{
	struct ng_flex_data *data = calloc(1, sizeof(struct ng_flex_data));

	if (strcmp(ng_get_props("flex-direction"), "column") == 0)
		data->flex_direction = NG_FLEX_COLUMN;
	else
		data->flex_direction = NG_FLEX_ROW;

	ng_begin_container(ng_flex_size_, ng_flex_paint_, data);
}
