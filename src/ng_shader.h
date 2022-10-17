#pragma once

typedef void ng_shader;
enum 
{
    NG_SHADERPARAM_MODEL,
    NG_SHADERPARAM_VIEW,
    NG_SHADERPARAM_PROJECTION,
    NG_SHADERPARAM_TEXTURECOLOR,
    NG_SHADERPARAM_TEXTUREDATA,

    //NG_SHADERPARAM_BLUR,

    NG_SHADERPARAM_COUNT
};

ng_shader* ng_shader_create(const char* vs, const char* fs);
void ng_shader_destroy(ng_shader* shader);

void ng_shader_bind(ng_shader* shader);
void ng_shader_set(int parameter, void* data);
void* ng_shader_get(int parameter);

void ng_shader_init();