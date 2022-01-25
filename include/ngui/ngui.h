#pragma once

#include "ng_math.h"

typedef nvec2i ng_size_func(nvec2i max, void *data);
typedef void ng_paint_func(nvec2i size, void *data);

int ng_init();

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
