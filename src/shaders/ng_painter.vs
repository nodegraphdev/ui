#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_uv;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
out vec3 v_color;
out vec2 v_uv;
out vec4 v_worldpos;
void main(){
    v_color=a_color;
    v_uv=a_uv;
    gl_Position = u_projection * u_view * u_model * vec4(a_pos.xyz, 1.0);
};