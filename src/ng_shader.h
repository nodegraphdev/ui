#pragma once

typedef void ng_shader;
enum 
{
    NG_SHADERPARAM_MODEL,
    NG_SHADERPARAM_VIEW,
    NG_SHADERPARAM_PROJECTION,
    //NG_SHADERPARAM_BLUR,

    NG_SHADERPARAM_COUNT
};

ng_shader* ng_shader_create(const char* vs, const char* fs);
void ng_shader_destroy(ng_shader* shader);

void ng_shader_bind(ng_shader* shader);
void ng_shader_set(int uniform, void* data);
