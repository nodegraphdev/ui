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

int ng_init();
void ng_shutdown();
int ng_open();
void ng_begin_frame();
void ng_end_frame();


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
nvec2i ng_size_child(int index, nvec2i max);
void ng_paint_child(int index, nvec2i size);

/* Properties API */
void ng_register_prop(char *key, int type);
void ng_prop(char *key, ...);
void ng_props(char *key, ...);

int ng_get_propi(char *key);
double ng_get_propf(char *key);
char *ng_get_props(char *key);
void *ng_get_propp(char *key);

void ng_reset_props();

// TODO: implement
// void ng_save_props();
// void ng_restore_props();
