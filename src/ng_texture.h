#pragma once

enum
{
	// Depth, Stencil, Component size
	NG_TEXTURE_FORMAT_D8,
	NG_TEXTURE_FORMAT_DS8,

	// Red, Green, Blue, Alpha, Component size
	NG_TEXTURE_FORMAT_RGBA8,
	NG_TEXTURE_FORMAT_RGB8,
	NG_TEXTURE_FORMAT_RG8,
	NG_TEXTURE_FORMAT_R8,
	NG_TEXTURE_FORMAT_BGRA8,
	NG_TEXTURE_FORMAT_BGR8,

};

typedef void ng_texture;

void ng_texture_destroy(ng_texture* texture);

ng_texture* ng_texture_from_bytes(int format, unsigned char* pixels, unsigned int width, unsigned int height);
void ng_texture_bind(ng_texture* texture);

void ng_texture_get_dimensions(ng_texture* texture, unsigned int* width, unsigned int* height);