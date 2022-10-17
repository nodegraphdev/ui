#pragma once

// Texture formats
enum NG_TEXTURE_FORMAT
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
	NG_TEXTURE_FORMAT_RG16UI,
	NG_TEXTURE_FORMAT_R16UI,
	NG_TEXTURE_FORMAT_R16I,

};


// Textures //
typedef void ng_texture;

// Pixels can be NULL for a blank texture
ng_texture* ng_texture_create(enum NG_TEXTURE_FORMAT format, unsigned char* pixels, unsigned int width, unsigned int height);
void ng_texture_destroy(ng_texture* texture);
void ng_texture_bind(ng_texture* texture, int unit);
void ng_texture_get_dimensions(ng_texture* texture, unsigned int* width, unsigned int* height);


// Framebuffers //
typedef void ng_framebuffer;

ng_framebuffer* ng_framebuffer_create(ng_texture* colortexture, int hasdepth);
// Does not destroy the attached color texture!!
void ng_framebuffer_destroy(ng_framebuffer* framebuffer);
ng_texture* ng_framebuffer_color(ng_framebuffer* framebuffer);
void ng_framebuffer_bind(ng_framebuffer* framebuffer);
void ng_framebuffer_unbind();