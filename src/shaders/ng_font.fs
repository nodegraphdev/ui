#version 330 core
precision lowp float;

in vec3 v_color;
in vec2 v_uv;

// Font glyph data
flat in int v_contour_count;
flat in int v_contours;
flat in int v_curves;


out vec4 o_fragColor;

uniform isampler2D u_tex;



ivec2 g_dims;
int getShort(int index)
{	
	return texelFetch(u_tex, ivec2(mod(index, g_dims.x), floor(index / g_dims.x)), 0).r;
}
vec2 getVec2(int index)
{
	return vec2 ( getShort(index), getShort(index + 1)) / 1000.0;
}



vec2 curve(vec2 p1, vec2 p2, vec2 p3, float t)
{
	return pow(1.0 - t, 2.0) * p1 + 2.0 * t * (1.0 - t) * p2 + t * t * p3;
}

float saturate(float v)
{
	return clamp(v, 0.0, 1.0);
}


void main()
{
	
	if(v_contour_count == 0)
		discard;
	
	
	
	g_dims = ivec2(textureSize(u_tex, 0));
	
	
	
	vec2 o = v_uv;
	
	float m = 100.0;
	float alpha = 0.0;
	
	int coff = v_curves;
	for (int k = 0; k < v_contour_count; k++)
	{
	
		int curve_count = getShort(v_contours + k);
	
		
		for(int i = 0; i < curve_count; i++)
		{
			vec2 p1 = getVec2(coff);
			coff += 2;
			vec2 p2 = getVec2(coff);
			coff += 2;
			vec2 p3 = getVec2(coff);
			coff += 2;
			
			
			p1 -= o;
			p2 -= o;
			p3 -= o;
			
			
			float a = p1.y - 2 * p2.y + p3.y;
			float b = p1.y - p2.y;
			float c = p1.y;

			float t1 = 0.0;
			float t2 = 0.0;
			
			if(a < 0.0001 && a > -0.0001)
			{
				t1 = t2 = c / 2 * b;
			}
			else
			{
				float g = b*b-a*c;
				if(g<0.0) g = 0.0;
				g = sqrt(g);
				
				t1 = (b-g)/a;
				t2 = (b+g)/a;
					
			}
			
			
			
			
			int shift = ((p1.y > 0) ? 2 : 0) + ((p2.y > 0) ? 4 : 0) + ((p3.y > 0) ? 8 : 0);
			int bittable = 0x2E74;
			int r = bittable >> shift;
			
			
			if((r & 1) != 0 && (t1 >= 0 && t1 < 1.0))
			{
				vec2 c = curve(p1, p2, p3, t1);
				if(o.x >= 0.5)
				{					
					if(c.x >= 0.0)
					{
						alpha += saturate(0.5 + m * c.x);
						//alpha += 1.0;
					}
				}
				else
				{
					if(c.x < 0.0)
					{
						alpha -= saturate(0.5 - m * c.x);
						//alpha -= saturate(-m * (c.x + 0.5 / m));
						
						//alpha -= saturate(-m * c.x - 0.5);
						//alpha -= 1.0;
						
					}
				}
			}
			if((r & 2) != 0 && (t2 >= 0 && t2 < 1.0))
			{
				vec2 c = curve(p1, p2, p3, t2);
				if(o.x >= 0.5)
				{
					if(c.x >= 0.0)
					{
						alpha -= saturate(0.5 + m * c.x);
					
						//alpha -= saturate(m * c.x + 0.5);
						//alpha -= c.x =0.5 / m;
						//alpha -= 1.0;
					}
				}
				else
				{
					if(c.x < 0.0)
					{
						alpha += saturate(0.5 - m * c.x);
						//alpha += 1.0;
					}
				}
				
			}
			
		}
		
	}
	
//	o_fragColor = vec4(1.0, v_uv.x, v_uv.y, alpha);
	
	
	vec2 dims = textureSize(u_tex, 0);
	ivec2 pos = ivec2(v_uv * dims);
	
	vec4 hh = texelFetch(u_tex, pos, 0);
	
	
	
	//o_fragColor = vec4(hh.r / 800.0, 0.0, alpha,1.0);
	o_fragColor = vec4(v_color.xyz, alpha);

}