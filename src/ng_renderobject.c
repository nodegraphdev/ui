#include <ngui.h>

#include <stdlib.h>
#include <stdio.h>

enum
{
	NG_CONTAINER,
	NG_LEAF,
};

struct ng_render_object
{
	int type;

	struct ng_container *parent;
	struct ng_render_object	*sibling;

	ng_size_func *size;
	ng_paint_func *paint;
	void *data;
};

struct ng_container
{
	struct ng_render_object render_object;

	struct ng_render_object *first_child,
		*last_child;

	int num_children;
};

static struct ng_render_object *ng_last_render_object = NULL;
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
		nvec2i desired_size = ng_size_child(0, size);
		ng_paint_child(0, desired_size);
	}
}

void ng_render_tree_init_()
{
	struct ng_container *root_container = calloc(1, sizeof(struct ng_container));

	struct ng_render_object *ro = &root_container->render_object;
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

static struct ng_render_object *ng_nth_child_(int index)
{
	if (index >= ng_num_children())
		return NULL;

	struct ng_render_object *child = ng_current_container->first_child;

	// This can be optimized later to make sequential access faster
	for (int i = 0; i < index; i++)
	{
		child = child->sibling;
	}

	return child;
}

nvec2i ng_size_child(int index, nvec2i max)
{
	struct ng_render_object *child = ng_nth_child_(index);

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

void ng_paint_child(int index, nvec2i size)
{
	struct ng_render_object *child = ng_nth_child_(index);

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

void ng_init_render_object_(struct ng_render_object *object,
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
	struct ng_render_object *object = calloc(1, sizeof(struct ng_render_object));

	ng_init_render_object_(object, paint, size, data);
}

void ng_begin_container(ng_size_func *size, ng_paint_func *paint, void *data)
{
	struct ng_container *container = calloc(1, sizeof(struct ng_container));

	ng_init_render_object_(&container->render_object, paint, size, data);
	container->render_object.type = NG_CONTAINER;

	ng_current_container = container;
}

void ng_end()
{
	if (ng_current_container->render_object.parent)
	{
		ng_current_container = ng_current_container->render_object.parent;
	}
	else
	{
		fprintf(stderr, "ng_end:error: container has no parent\n");
	}
}

static void ng_free_render_object_(struct ng_render_object *ro);

static void ng_free_container_(struct ng_container *c)
{
	struct ng_render_object *child = c->first_child,
		*next;

	for (int i = 0; i < c->num_children; i++)
	{
		next = child->sibling;
		ng_free_render_object_(child);
		child = next;
	}
}

static void ng_free_render_object_(struct ng_render_object *ro)
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

	// TODO: hard coded
	nvec2i size = {640, 480};
	void *data = ng_current_container->render_object.data;

	size = ng_current_container->render_object.size(size, data);
	ng_current_container->render_object.paint(size, data);

	ng_free_container_(ng_current_container);
}
