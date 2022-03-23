#pragma once


typedef void ng_font;

void ng_font_init();

void ng_font_test();

ng_font* ng_font_load(const char* path);
void ng_font_destroy(ng_font* font);


void ng_text_draw(const char* text, ng_font* font);
