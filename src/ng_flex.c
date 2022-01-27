#include <ngui.h>
#include <ng_painter.h>
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
	int cross_axis_start;
	nvec2i min_size;
	int flex;
	int width;
};

struct ng_flex_data
{
	int flex_direction;
	struct ng_flex_child *children;
};

static int ng_flex_width_per_flex_(int width, int line_width, int row_flex_total)
{
	int remaining_width = width - line_width;

	int width_per_flex = 0;
	if (row_flex_total)
		width_per_flex = (int)((double)remaining_width / (double)row_flex_total);

	return width_per_flex;
}

static nvec2i ng_flex_size_(nvec2i max, void *d)
{
	struct ng_flex_data *data = d;

	int width = max.x;
	int line_width = 0;
	int cursor_y = 0;
	int max_height = 0;
	int row_flex_total = 0;
	int line_starts_at = 0;

	data->children = calloc(ng_num_children(), sizeof(struct ng_flex_child));

	// TODO: respect flex-direction

	int i;

	for (i = 0; i < ng_num_children(); i++)
	{
		nvec2i child_size = ng_child_size(i, (nvec2i){0, 0});

		if (child_size.y > max_height)
			max_height = child_size.y;

		int flex =  ng_child_get_propi(i, "flex");
		// printf("flex %d, total %d\n", flex, row_flex_total);

		row_flex_total += flex;

		data->children[i].cross_axis_start = cursor_y;
		data->children[i].min_size = child_size;
		data->children[i].flex = flex;
		data->children[i].width = child_size.x;

		// If this doesn't fit on one line
		if (line_width + child_size.x > width)
		{
			cursor_y += max_height;
			max_height = 0;


			// If there's already stuff on this line, move to next line. Otherwise,
			// the widget has no choice but to fit in this line
			if (line_width > 0)
			{
				// Turns out this element actually isn't on this line
				row_flex_total -= flex;
				int width_per_flex = ng_flex_width_per_flex_(width, line_width, row_flex_total);

				for (int j = line_starts_at; j <= i; j++)
				{
					data->children[j].width += data->children[j].flex * width_per_flex;
				}

				// And then layout this item a second time (now on a new line)
				i--;
			}
			else
			{
				// too bad, must fit here anyway
				data->children[i].width = width;
			}

			row_flex_total = 0;
			line_width = 0;
			line_starts_at = i + 1;
		}
		else
		{
			line_width += child_size.x;
		}
	}

	if (line_width > 0)
	{
		int width_per_flex = ng_flex_width_per_flex_(width, line_width, row_flex_total);

		// TODO: factor out?

		for (int j = line_starts_at; j < i; j++)
		{
			data->children[j].width += data->children[j].flex * width_per_flex;
		}
	}

	return (nvec2i){width, cursor_y + max_height};
}

static void ng_flex_paint_(nvec2i size, void *d)
{
	struct ng_flex_data *data = d;

	int current_y = 0;
	int current_x = 0;

	for (int i = 0; i < ng_num_children(); i++)
	{
		struct ng_flex_child *child = &data->children[i];

		if (child->cross_axis_start != current_y)
		{
			current_y = child->cross_axis_start;
			current_x = 0;
		}

		ng_painter_push_origin((nvec2i){current_x, current_y});
		ng_child_paint(i, (nvec2i){child->width, child->min_size.y});
		ng_painter_pop_origin();

		current_x += child->width;
	}

	free(data->children);
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
