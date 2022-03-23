#pragma once


enum NG_GLINFO_DUMP 
{
	NG_GLINFO_DUMP_ADAPTER,
	NG_GLINFO_DUMP_CONTEXT,
	NG_GLINFO_DUMP_EXTENSIONS,
	NG_GLINFO_DUMP_ALL = 255
};
void ng_glinfo_dump(enum NG_GLINFO_DUMP info_level);


// Must be called BEFORE the gl context is created
void ng_glinfo_debug_enable();

// Must be called AFTER the gl context is created
void ng_glinfo_debug_attach_logger();

