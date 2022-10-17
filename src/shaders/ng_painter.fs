#version 330 core
precision lowp float;
in vec4 v_color;
in vec2 v_uv;
out vec4 o_fragColor;
void main() {
    o_fragColor = vec4(v_color.xyz, 1.0);// * (1.0 - 0.5 * pow(v_uv.y - 0.5, 2));
}; 