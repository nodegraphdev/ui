#include "ng_shader.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


// GL uniform types as enums
// This makes dealing with uniforms nicer

enum
{
    NG_UNIFORM_FLOAT1,
    NG_UNIFORM_FLOAT2,
    NG_UNIFORM_FLOAT3,
    NG_UNIFORM_FLOAT4,

    NG_UNIFORM_INT1,
    NG_UNIFORM_INT2,
    NG_UNIFORM_INT3,
    NG_UNIFORM_INT4,

    NG_UNIFORM_MAT2,
    NG_UNIFORM_MAT3,
    NG_UNIFORM_MAT4,
};
const unsigned int ng_uniform_type_size[] =
{
    /*float1*/ sizeof(float) * 1,
    /*float2*/ sizeof(float) * 2,
    /*float3*/ sizeof(float) * 3,
    /*float4*/ sizeof(float) * 4,

    /*int1*/   sizeof(int)   * 1,
    /*int2*/   sizeof(int)   * 2,
    /*int3*/   sizeof(int)   * 3,
    /*int4*/   sizeof(int)   * 4,

    /*mat2*/   sizeof(float) * 2 * 2,
    /*mat3*/   sizeof(float) * 3 * 3,
    /*mat4*/   sizeof(float) * 4 * 4,
};


typedef struct ng_parameter
{
    int type;
    const char* name;
    void* data;
    int size;
} ng_parameter;

// This should be the same as NG_SHADERPARAM
#define PARAMETER(type, name) {NG_UNIFORM_ ## type, "u_" #name, 0},
static ng_parameter ng_shader_params[NG_SHADERPARAM_COUNT] = {
    PARAMETER(MAT4, model)
    PARAMETER(MAT4, view)
    PARAMETER(MAT4, projection)
};
#undef PARAMETER

typedef struct ng_shaderdata
{
    GLuint program;

    GLint uniforms[NG_SHADERPARAM_COUNT];
} ng_shaderdata;


static void* ng_shader_param_data;
static ng_shaderdata* ng_shader_current;


// Internal functions //

static void ng_check_error_(GLuint shader)
{
    int infoLogLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        char vertexError[1024];
        if (infoLogLength > 1024)
            infoLogLength = 1024;
        glGetShaderInfoLog(shader, infoLogLength, NULL, &vertexError[0]);
        printf("[shader] Error:\n%s", vertexError);
    }
}
static GLuint ng_compile_shader_(GLenum shaderType, const char* src)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    ng_check_error_(shader);
    return shader;
}
static void ng_apply_parameter_(int idx)
{
    if(!ng_shader_current) return;

    GLint uniform = ng_shader_current->uniforms[idx];
    if (uniform < 0) return;

    void* mem = ng_shader_params[idx].data;
    switch (ng_shader_params[idx].type)
    {
    case NG_UNIFORM_FLOAT1: glUniform1fv(uniform, 1, mem); break;
    case NG_UNIFORM_FLOAT2: glUniform2fv(uniform, 1, mem); break;
    case NG_UNIFORM_FLOAT3: glUniform3fv(uniform, 1, mem); break;
    case NG_UNIFORM_FLOAT4: glUniform4fv(uniform, 1, mem); break;
    
    case NG_UNIFORM_INT1: glUniform1iv(uniform, 1, mem); break;
    case NG_UNIFORM_INT2: glUniform2iv(uniform, 1, mem); break;
    case NG_UNIFORM_INT3: glUniform3iv(uniform, 1, mem); break;
    case NG_UNIFORM_INT4: glUniform4iv(uniform, 1, mem); break;

    case NG_UNIFORM_MAT2: glUniformMatrix2fv(uniform, 1, 0, mem); break;
    case NG_UNIFORM_MAT3: glUniformMatrix3fv(uniform, 1, 0, mem); break;
    case NG_UNIFORM_MAT4: glUniformMatrix4fv(uniform, 1, 0, mem); break;
    }
}
static void ng_apply_all_parameters_()
{
    for (int i = 0; i < NG_SHADERPARAM_COUNT; i++)
        ng_apply_parameter_(i);
}



// Public facing functions //

void ng_shader_init()
{
    ng_shader_current = 0;

    // Prep uniform data

    // Create a block for the uni data to live in
    int sz = 0;
    for (int i = 0; i < NG_SHADERPARAM_COUNT; i++)
        sz += ng_uniform_type_size[ng_shader_params[i].type];
    ng_shader_param_data = calloc(1, sz);

    // Assign uni data pointers
    char* unidata = (char*)ng_shader_param_data;
    for (int i = 0; i < NG_SHADERPARAM_COUNT; i++)
    {
        ng_shader_params[i].data = unidata;
        unidata += ng_uniform_type_size[ng_shader_params[i].type];
    }

}

ng_shader* ng_shader_create(const char* vs, const char* fs)
{
    ng_shaderdata* data = malloc(sizeof(ng_shaderdata));

    // Compile vertex and fragment shaders
    GLuint vertexShader = ng_compile_shader_(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = ng_compile_shader_(GL_FRAGMENT_SHADER, fs);

    // Attach them to the program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Setup the vertex attributes
    glBindAttribLocation(program, 0, "a_pos");
    glBindAttribLocation(program, 1, "a_color");
    glBindAttribLocation(program, 2, "a_uv");
    glBindAttribLocation(program, 3, "a_glyph");

    // Link!
    glLinkProgram(program);
    data->program = program;

    // Get all uniforms for this shader
    for (int i = 0; i < NG_SHADERPARAM_COUNT; i++)
        data->uniforms[i] = glGetUniformLocation(program, ng_shader_params[i].name);

    // Clean up the scraps
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return data;
}
void ng_shader_destroy(ng_shader* shader)
{
    ng_shaderdata* data = shader;

    glDeleteProgram(data->program);
    free(shader);
}

void ng_shader_bind(ng_shader* shader)
{
    ng_shaderdata* data = shader;

    ng_shader_current = data;

    glUseProgram(data->program);
    ng_apply_all_parameters_();
}
void ng_shader_set(int parameter, void* data)
{
    assert(parameter >= 0 && parameter < NG_SHADERPARAM_COUNT);

    ng_parameter* uni = &ng_shader_params[parameter];

    // Copy it in so we can apply it elsewhere later
    memcpy(uni->data, data, ng_uniform_type_size[uni->type]);

    // Apply it
    ng_apply_parameter_(parameter);
}

 