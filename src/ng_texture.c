#include "ng_texture.h"
#include <stdlib.h>
#include <cassert>
#include <glad/glad.h>

typedef struct ng_texture_data
{
    GLuint id;

    unsigned int width;
    unsigned int height;

} ng_texture_data;

typedef struct ng_framebuffer_data
{
    GLuint framebuffer;
    GLuint renderbuffer;
    char hasrenderbuffer; // Would be nice to axe this
    
    ng_texture_data* color;

} ng_framebuffer_data;


// Maps to NG_TEXTURE_FORMAT
static int ng_texture_format_lookup[][3] = {

  // Internal,           Layout, Size
    {GL_DEPTH_COMPONENT, GL_RED, GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_D8
    {GL_DEPTH_STENCIL,   GL_RG,  GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_DS8

  // Internal, Layout,  Size
    {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_RGBA8
    {GL_RGB8,  GL_RGB,  GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_RGB8
    {GL_RG8,   GL_RG,   GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_RG8
    {GL_R8,    GL_RED,  GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_R8
    {GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_BGRA8
    {GL_RGB8,  GL_BGR,  GL_UNSIGNED_BYTE}, // NG_TEXTURE_FORMAT_BGR8
};

void ng_texture_destroy(ng_texture* texture)
{
    ng_texture_data* data = texture;

    glDeleteTextures(1, &data->id);

    free(data);
}

ng_texture* ng_texture_create(enum NG_TEXTURE_FORMAT format, unsigned char* pixels, unsigned int width, unsigned int height)
{
    // Create the texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Look up the GL format
    int* fmt = &ng_texture_format_lookup[format];

    // Put the image in the texture
    glTexImage2D(GL_TEXTURE_2D, 0, fmt[0], width, height, 0, fmt[1], fmt[2], pixels);


    ng_texture_data* data = malloc(sizeof(ng_texture_data));
    data->id = texture;
    data->width = width;
    data->height = height;
    return (ng_texture*)data;
}

void ng_texture_bind(ng_texture* texture)
{
    ng_texture_data* data = texture;

    glBindTexture(GL_TEXTURE_2D, data->id);
}

void ng_texture_get_dimensions(ng_texture* texture, unsigned int* width, unsigned int* height)
{
    ng_texture_data* data = texture;

    if (width)
        *width = data->width;

    if (height)
        *height = data->height;
}




// Framebuffer //


ng_framebuffer* ng_framebuffer_create(ng_texture* colortexture, int hasdepth)
{
    ng_texture_data* txdata = colortexture;

    // Create the framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // Attach the texture to the fb
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txdata->id, 0);

    GLuint rbo = 0;
    if (hasdepth)
    {
        // Create the depthbuffer
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, txdata->width, txdata->height);
        
        // Attach it
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    ng_framebuffer_data* fbdata = malloc(sizeof(ng_framebuffer_data));
    fbdata->framebuffer = framebuffer;
    fbdata->renderbuffer = rbo;
    fbdata->hasrenderbuffer = hasdepth;
    fbdata->color = colortexture;
    return (ng_framebuffer*)fbdata;
}


void ng_framebuffer_destroy(ng_framebuffer* framebuffer)
{
    ng_framebuffer_data* data = framebuffer;
    
    if (data->hasrenderbuffer)
    {
        glDeleteRenderbuffers(1, &data->renderbuffer);
    }
    
    glDeleteFramebuffers(1, &data->framebuffer);
    
    // Leave the color texture as is 
    
    free(data);
}

ng_texture* ng_framebuffer_color(ng_framebuffer* framebuffer)
{
    ng_framebuffer_data* data = framebuffer;
    return data->color;
}


void ng_framebuffer_bind(ng_framebuffer* framebuffer)
{
    ng_framebuffer_data* data = framebuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, data->framebuffer);
}
void ng_framebuffer_unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}