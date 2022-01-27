#include <ngui.h>
#include "ng_properties.h"
#include "ng_window.h"

#include <stdlib.h>
#include <stdio.h>

enum
{
	NG_CONTAINER,
	NG_LEAF,
};

struct ng_render_base
{
	int type;

	struct ng_container *parent;
	struct ng_render_base *sibling;

	ng_size_func *size;
	ng_paint_func *paint;
	void *data;
};

struct ng_render_object
{
	struct ng_render_base base;
	struct ng_retained_prop props[];
};

struct ng_container
{
	struct ng_render_base base;

	struct ng_render_base *first_child,
		*last_child;

	int num_children;

	struct ng_retained_prop props[];
};

static struct ng_container *ng_current_container = NULL;
static struct ng_container *ng_root_container = NULL;
static const nvec2i nvec2i_zero_ = { 0, 0 };

nvec2i ng_root_container_size(nvec2i max, void *data)
{
	return max;
}

void ng_root_container_paint(nvec2i size, void *data)
{
	if (ng_num_children() > 0)
	{
		nvec2i desired_size = ng_child_size(0, size);
		ng_child_paint(0, desired_size);
	}
}

void ng_render_tree_init_()
{
	struct ng_container *root_container = calloc(1, sizeof(struct ng_container));

	struct ng_render_base *ro = &root_container->base;
	ro->data = NULL;
	ro->paint = ng_root_container_paint;
	ro->size = ng_root_container_size;
	ro->type = NG_CONTAINER;

	ng_current_container = root_container;
	ng_root_container = root_container;
}

int ng_num_children()
{
	return ng_current_container->num_children;
}

static struct ng_render_base *ng_nth_child_(int index)
{
	if (index >= ng_num_children())
		return NULL;

	struct ng_render_base *child = ng_current_container->first_child;

	// This can be optimized later to make sequential access faster
	for (int i = 0; i < index; i++)
	{
		child = child->sibling;
	}

	return child;
}

struct ng_retained_prop *ng_get_render_object_prop_(struct ng_render_base *base, char *key)
{
	if (base->type == NG_CONTAINER)
	{
		return ng_get_retained_prop_(((struct ng_container *)base)->props, key);
	}
	else
	{
		return ng_get_retained_prop_(((struct ng_render_object *)base)->props, key);
	}
}

int ng_child_get_propi(int index, char *key)
{
	struct ng_retained_prop *prop = ng_get_render_object_prop_(ng_nth_child_(index), key);
	if (prop)
		return prop->ival;
	else
		return 0;
}

double ng_child_get_propf(int index, char *key)
{
	struct ng_retained_prop *prop = ng_get_render_object_prop_(ng_nth_child_(index), key);
	if (prop)
		return prop->dval;
	else
		return 0;
}

char *ng_child_get_props(int index, char *key)
{

	struct ng_retained_prop *prop = ng_get_render_object_prop_(ng_nth_child_(index), key);
	if (prop)
		return prop->sval;
	else
		return "";
}

void *ng_child_get_propp(int index, char *key)
{
	struct ng_retained_prop *prop = ng_get_render_object_prop_(ng_nth_child_(index), key);
	if (prop)
		return prop->pval;
	else
		return 0;
}

nvec2i ng_child_size(int index, nvec2i max)
{
	struct ng_render_base *child = ng_nth_child_(index);

	if (!child)
		return nvec2i_zero_;

	struct ng_container *prev_container = ng_current_container;

	if (child->type == NG_CONTAINER)
	{
		ng_current_container = (struct ng_container *)child;
	}

	nvec2i size = child->size(max, child->data);

	if (child->type == NG_CONTAINER)
	{
		ng_current_container = prev_container;
	}

	return size;
}

void ng_child_paint(int index, nvec2i size)
{
	struct ng_render_base *child = ng_nth_child_(index);

	if (!child)
		return;

	struct ng_container *prev_container = ng_current_container;

	// TODO: don't repeat yourself
	if (child->type == NG_CONTAINER)
	{
		ng_current_container = (struct ng_container *)child;
	}

	child->paint(size, child->data);

	if (child->type == NG_CONTAINER)
	{
		ng_current_container = prev_container;
	}
}

void ng_init_render_object_(struct ng_render_base *object,
							ng_paint_func *paint, ng_size_func *size,
							void *data)
{
	object->data = data;
	object->paint = paint;
	object->size = size;
	object->parent = ng_current_container;
	object->type = NG_LEAF;

	if (ng_num_children())
	{
		ng_current_container->last_child->sibling = object;
		ng_current_container->last_child = object;
	}
	else
	{
		ng_current_container->first_child = object;
		ng_current_container->last_child = object;
	}

	ng_current_container->num_children++;
}

void ng_add_render_object(ng_size_func *size, ng_paint_func *paint, void *data)
{
	struct ng_render_object *object = calloc(1, sizeof(struct ng_render_object) +
		sizeof(struct ng_retained_prop) * ng_num_retained_props_());

	ng_init_render_object_(&object->base, paint, size, data);
	ng_populate_retained_(object->props);
}

void ng_begin_container(ng_size_func *size, ng_paint_func *paint, void *data)
{
	struct ng_container *container = calloc(1, sizeof(struct ng_container) +
		sizeof(struct ng_retained_prop) * ng_num_retained_props_());

	ng_init_render_object_(&container->base, paint, size, data);
	container->base.type = NG_CONTAINER;
	ng_populate_retained_(container->props);

	ng_current_container = container;
}

void ng_end()
{
	if (ng_current_container->base.parent)
	{
		ng_current_container = ng_current_container->base.parent;
	}
	else
	{
		fprintf(stderr, "ng_end:error: container has no parent\n");
	}
}

static void ng_free_render_object_(struct ng_render_base *ro);

static void ng_free_container_(struct ng_container *c)
{
	struct ng_render_base *child = c->first_child,
		*next;

	for (int i = 0; i < c->num_children; i++)
	{
		next = child->sibling;
		ng_free_render_object_(child);
		child = next;
	}
}

static void ng_free_render_object_(struct ng_render_base *ro)
{
	if (ro->type == NG_CONTAINER)
		ng_free_container_((struct ng_container *)ro);

	if (ro->data)
		free(ro->data);

	free(ro);
}

void ng_commit()
{
	ng_current_container = ng_root_container;

	nvec2i size = ng_window_size_();
	void *data = ng_current_container->base.data;

	size = ng_current_container->base.size(size, data);
	ng_current_container->base.paint(size, data);

	ng_free_container_(ng_current_container);
	ng_current_container->first_child = ng_current_container->last_child = NULL;
	ng_current_container->num_children = 0;
	ng_current_container->base.data = NULL;
}
