#pragma once

#include "ng_math.h"

typedef nvec2i ng_size_func(nvec2i max, void *data);
typedef void ng_paint_func(nvec2i size, void *data);

enum ng_prop_type
{
    NG_PROP_UNDEF,
    NG_PROP_INT,
    NG_PROP_DOUBLE,
    NG_PROP_POINTER,
    NG_PROP_STRING
};

enum ng_prop_flag
{
	NG_PROP_IMPLICIT_RETAIN = 1
};

int ng_init();
void ng_shutdown();
int ng_open();
void ng_begin_frame();
void ng_end_frame();


/* These functions set the name/id/class of the next render
 * object/container to be created. By default the name/id/class are
 * all empty strings "". ng_with_name is special in that it also hooks
 * into ng_css, so once you call this, any calls to ng_get_propx
 * will work with properties set in CSS. */
void ng_with_name(char *name);
void ng_with_id(char *id);
void ng_with_class(char *class);

/* Data must be freeable with free() */
/* TODO: destructor function */
void ng_add_render_object(ng_size_func *size, ng_paint_func *paint, void *data);
void ng_begin_container(ng_size_func *size, ng_paint_func *paint, void *data);
void ng_end();
/* Call this at the end of each frame to actually lay out and paint
 * the UI */
void ng_commit();

/* Container API */
/* these functions must only be called during the size or paint
 * functions of a container. */
int ng_num_children();
nvec2i ng_child_size(int index, nvec2i max);
void ng_child_paint(int index, nvec2i size);

/* these functions get retained properties from a child */
int ng_child_get_propi(int index, char *key);
double ng_child_get_propf(int index, char *key);
char *ng_child_get_props(int index, char *key);
void *ng_child_get_propp(int index, char *key);

/* Properties API */
void ng_register_prop(char *key, int type, int flags);
void ng_prop(char *key, ...);
void ng_props(char *key, ...);

int ng_get_propi(char *key);
double ng_get_propf(char *key);
char *ng_get_props(char *key);
void *ng_get_propp(char *key);

void ng_reset_props();

void ng_save_props();
void ng_restore_props();


/* Containers */
void ng_flex_begin();
