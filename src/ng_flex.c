#include <ngui.h>
#include <stdlib.h>
#include <string.h>

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

}

static void ng_flex_paint_(nvec2i size, void *data)
{

}

void ng_flex_begin()
{
	struct ng_flex_data *data = calloc(1, sizeof(struct ng_flex_data));

	if (strcmp(ng_get_props("flex-direction"), "column") == 0)
		data->flex_direction = NG_FLEX_COLUMN;
	else
		data->flex_direction = NG_FLEX_ROW;
}
