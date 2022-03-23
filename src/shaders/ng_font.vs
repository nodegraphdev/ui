#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in float a_glyph;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform isampler2D u_tex;

out vec3 v_color;
out vec4 v_worldpos;
out vec2 v_uv;

flat out int v_contour_count;
flat out int v_contours;
flat out int v_curves;


ivec2 g_dims = ivec2(1,1);
int getShort(int index)
{
	
	return texelFetch(u_tex, ivec2(mod(index, g_dims.x), (index / g_dims.x)), 0).r;
}

void main()
{

    v_color=a_color;
	v_uv=a_uv;
    gl_Position = u_projection * u_view * u_model * vec4(a_pos.xyz, 1.0);
	
	
	g_dims = ivec2(textureSize(u_tex, 0));
	
	
	int header_pos = getShort(int(a_glyph));
	if(header_pos < 0)
	{
		// No data
		v_contour_count = 0;
		v_contours = 0;
		v_curves = 0;
	}
	else
	{
		v_contour_count = getShort(header_pos + 0);
		v_contours = header_pos + 1;
		v_curves = header_pos + 1 + v_contour_count;
	}
	
}
