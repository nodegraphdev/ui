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
	int perp_axis_start;
	nvec2i min_size;
	int flex;
	int par_size;
};

struct ng_flex_data
{
	int flex_direction;
	struct ng_flex_child *children;
};

static int ng_flex_par_axis_(struct ng_flex_data *data, nvec2i size)
{
	if (data->flex_direction == NG_FLEX_ROW)
		return size.x;
	else
		return size.y;
}

static int ng_flex_perp_axis_(struct ng_flex_data *data, nvec2i size)
{
	if (data->flex_direction == NG_FLEX_ROW)
		return size.y;
	else
		return size.x;
}

static int ng_flex_width_per_flex_(int size_par, int cursor_par, int row_flex_total)
{
	int remaining_width = size_par - cursor_par;

	int width_per_flex = 0;
	if (row_flex_total)
		width_per_flex = (int)((double)remaining_width / (double)row_flex_total);

	return width_per_flex;
}

static nvec2i ng_flex_size_(nvec2i max, void *d)
{
	struct ng_flex_data *data = d;

	// The max size parallel to the flex-direction
	int size_par = ng_flex_par_axis_(data, max);

	// "Cursor" position parallel to the flex-direction
	int cursor_par = 0;
	// "Cursor" position perpendicular to the flex-direction
	int cursor_perp = 0;
	// Max size of this row on the axis perpendicular to the flex-direction
	int max_perp = 0;
	// The sum of all "flex" properties in the current "row"
	int row_flex_total = 0;
	// The index of the first child on the current row
	int row_starts_at = 0;

	data->children = calloc(ng_num_children(), sizeof(struct ng_flex_child));

	// Child index
	int i;

	for (i = 0; i < ng_num_children(); i++)
	{
		nvec2i child_size = ng_child_size(i, (nvec2i){0, 0});

		if (ng_flex_perp_axis_(data, child_size) > max_perp)
			max_perp = ng_flex_perp_axis_(data, child_size);

		int flex = ng_child_get_propi(i, "flex");
		// printf("flex %d, total %d\n", flex, row_flex_total);

		row_flex_total += flex;

		data->children[i].perp_axis_start = cursor_perp;
		data->children[i].min_size = child_size;
		data->children[i].flex = flex;
		data->children[i].par_size = ng_flex_par_axis_(data, child_size);

		// If this doesn't fit on one line
		if (cursor_par + ng_flex_par_axis_(data, child_size) > size_par)
		{
			cursor_perp += max_perp;
			max_perp = 0;

			// If there's already stuff on this line, move to next line. Otherwise,
			// the widget has no choice but to fit in this line
			if (cursor_par > 0)
			{
				// Turns out this element actually isn't on this line
				row_flex_total -= flex;
				int width_per_flex = ng_flex_width_per_flex_(size_par, cursor_par, row_flex_total);

				for (int j = row_starts_at; j <= i; j++)
				{
					data->children[j].par_size += data->children[j].flex * width_per_flex;
				}

				// And then layout this item a second time (now on a new line)
				i--;
			}
			else
			{
				// too bad, must fit here anyway
				data->children[i].par_size = size_par;
			}

			row_flex_total = 0;
			cursor_par = 0;
			row_starts_at = i + 1;
		}
		else
		{
			cursor_par += ng_flex_par_axis_(data, child_size);
		}
	}

	if (cursor_par > 0)
	{
		int width_per_flex = ng_flex_width_per_flex_(size_par, cursor_par, row_flex_total);

		// TODO: factor out?

		for (int j = row_starts_at; j < i; j++)
		{
			data->children[j].par_size += data->children[j].flex * width_per_flex;
		}
	}

	return (nvec2i){size_par, cursor_perp + max_perp};
}

static void ng_flex_paint_(nvec2i size, void *d)
{
	struct ng_flex_data *data = d;

	int current_perp = 0;
	int current_par = 0;

	for (int i = 0; i < ng_num_children(); i++)
	{
		struct ng_flex_child *child = &data->children[i];

		if (child->perp_axis_start != current_perp)
		{
			current_perp = child->perp_axis_start;
			current_par = 0;
		}

		if (data->flex_direction == NG_FLEX_ROW)
		{
			ng_painter_push_origin((nvec2i) {current_par, current_perp});
			ng_child_paint(i, (nvec2i){child->par_size, child->min_size.y});
		}
		else
		{
			ng_painter_push_origin((nvec2i){current_perp, current_par});
			ng_child_paint(i, (nvec2i){child->min_size.x, child->par_size});
		}

		ng_painter_pop_origin();

		current_par += child->par_size;
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
