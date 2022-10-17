#include "ng_font.h"
#include "ng_vertex.h"
#include "ng_render.h"
#include "ng_shader.h"
#include "ng_texture.h"
#include "ng_painter.h"
#include "ng_ustack.h"
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftcolor.h>

static FT_Library ng_freetype_lib;

static ng_vbo ng_testvbo;
static ng_ibo ng_testibo;

ng_shader* ng_font_shader;

ng_texture* ng_font_texture;

extern const unsigned char ng_font_vs[];
extern const unsigned char ng_font_fs[];

struct ng_font_glyph
{
	ng_ustack /*unsigned short*/ contours;
	ng_ustack /*nvec2s*/         curves;

	short width;
	short height;

	nvec4 vertex_color;
};

struct ng_font_shader_data_glyph_header
{
	short width;
	short contour_count;

	// These two are stored immediately after this struct
	// unsigned short* contours;
	// nvec2s* curves;
};

struct ng_font_shader_data
{
	short units_per_em;
	// Maps up ascii codes to offsets in shorts in the image
	// (-1) means "Don't draw" or "Blank glyph"
	short character_offsets[256];

	// After this struct is an array of ng_font_shader_data_glyph_header and their data

};

void aaa();

void ng_font_init()
{

	FT_Error error = FT_Init_FreeType(&ng_freetype_lib);
	if (error)
	{
		printf("error!!");
	}

	ng_font_load("seguiemj.ttf");
	
	aaa();
}

void ng_font_destroy(ng_font* font)
{

}

struct ng_halfedge
{
	struct ng_halfedge* next;
	struct ng_halfedge* pair;
	nvec2s* vertex;
};

#if 0

static void ng_font_glyph_hull(nvec2s* points, int point_count)
{

	// Find the points with the min and max x coordinates
	nvec2s* minx = points + 0;
	nvec2s* maxx = points + 0;

	for (int i = 1; i < point_count; i++)
	{
		nvec2s* p = points + i;

		if (p->x == minx->x && p->y < minx->y)
			minx = p;
		else if (p->x < minx->x)
			minx = p;

		if (p->x == maxx->x && p->y > maxx->y)
			maxx = p;
		else if (p->x > maxx->x)
			maxx = p;
	}


	// Top fills front to back
	nvec2s* top = malloc(sizeof(nvec2s) * point_count);
	
	// Bottom fills back to front
	nvec2s* bottom = top + point_count - 1;
	
	// Top count is positive
	int top_count = 0;

	// Bottom count is negative
	int bottom_count = 0;


	nvec2s* topfar = maxx;
	nvec2s* bottomfar = minx;
	float topfardist = 0;
	float bottomfardist = 0;



	nvec2s delta = ng_subv2s(*maxx, *minx);
	
	nvec2 minx_f = { minx->x, minx->y };

	// Normalizing makes this have a magnitude of 1 and lets us skip a lot of divisions
    nvec2 errvec = ng_normalizev2((nvec2){delta.y,-delta.x});

	for (int i = 0; i < point_count; i++)
	{
		nvec2s* p = points + i;
		if (p == minx || p == maxx) continue;
	
		nvec2 o = ng_subv2((nvec2){ p->x, p->y }, minx_f);
		int dist = ng_dotv2(errvec, o);


		if (dist > 0)
		{
			top[top_count] = *p;
			if (dist > topfardist)
			{
				topfardist = dist;
				topfar = &top[top_count];
			}
			top_count++;
		}
		else
		{
			bottom[bottom_count] = *p;
			if (dist < bottomfardist)
			{
				bottomfardist = dist;
				bottomfar = &bottom[bottom_count];
			}
			bottom_count--;
		}
	}

	/*
	// Top Left
	nvec2s *topleftfar = topfar, *toprightfar = topfar;
	float topleftfardist = 0, toprightfardist = 0;
	int toplefti = 0;
	int toprighti = 0;
	nvec2s* topleft = malloc(sizeof(nvec2s) * topi); // Fills front to back
	nvec2s* topright = top + topi - 1; // Fills back to front 

	nvec2s deltaleft = ng_subv2s(*topfar, *minx);
	nvec2s deltaright = ng_subv2s(*topfar, *maxx);
	for (int i = 0; i < topi; i++)
	{
		nvec2s* p = top + i;

		nvec2s o = ng_subv2s(*p, *minx);
		float dist = o.y - ng_scalev2s(deltaleft, ng_dotv2s(deltaleft, o) / (float)ng_dotv2s(deltaleft, deltaleft)).y;

		if (dist > 0)
		{
			topleft[toplefti] = *p;
			if (dist > topleftfardist)
			{
				topleftfardist = dist;
				topleftfar = &topleft[toplefti];
			}
			toplefti++;
		}
		else
		{
			o = ng_subv2s(*p, *maxx);
			dist = o.y - ng_scalev2s(deltaright, ng_dotv2s(deltaright, o) / (float)ng_dotv2s(deltaright, deltaright)).y;

			if (dist > 0)
			{
				topright[toprighti] = *p;
				if (dist > toprightfardist)
				{
					toprightfardist = dist;
					toprightfar = &topright[toprighti];
				}
				toprighti--;
			}
		}
	}

	*/
	printf("A=[");
	for (int i = 0; i < top_count; i++)
	{
		printf("(%d, %d)", top[i].x, top[i].y);
		if (i + 1 != top_count) printf(",");

	}
	printf("]\n");


	printf("B=[");
	for (int i = bottom_count; i <= 0; i++)
	{
		printf("(%d, %d)", bottom[i].x, bottom[i].y);
		if (i != 0) printf(",");

	}
	printf("]\n");

	/*
	printf("C=[");
	for (int i = 0; i < bottomi; i++)
	{
		printf("(%d, %d)", bottom[i].x, bottom[i].y);
		if (i + 1 != bottomi) printf(",");
	}
	printf("]\n");
	*/

	printf("L(p_1, p_2,t) = (1-t)*p_1 + t*p_2\n");
	printf("L((%d, %d), (%d, %d), t)\n", minx->x, minx->y, maxx->x, maxx->y);

	printf("L((%d, %d), (%d, %d), t)\n", minx->x, minx->y, topfar->x, topfar->y);
	printf("L((%d, %d), (%d, %d), t)\n", maxx->x, maxx->y, topfar->x, topfar->y);

	printf("L((%d, %d), (%d, %d), t)\n", minx->x, minx->y, bottomfar->x, bottomfar->y);
	printf("L((%d, %d), (%d, %d), t)\n", maxx->x, maxx->y, bottomfar->x, bottomfar->y);


}
#endif




static struct ng_font_glyph ng_font_glyph_load_outline(FT_GlyphSlot glyph)
{
	ng_ustack curves;
	ng_ustack_alloc(&curves, sizeof(nvec2s));

	ng_ustack contours;
	ng_ustack_alloc(&contours, sizeof(unsigned short));

	FT_Outline* outline = &glyph->outline;
	nvec2s p1, p2, p3;


	/*
	if (face->glyph->format == FT_GLYPH_FORMAT_NONE)
		puts("\tFT_GLYPH_FORMAT_NONE");
	*/
	//if (glyph->format == FT_GLYPH_FORMAT_COMPOSITE)
	//	puts("\tFT_GLYPH_FORMAT_COMPOSITE");
	/*
	if (face->glyph->format == FT_GLYPH_FORMAT_BITMAP)
		puts("\tFT_GLYPH_FORMAT_BITMAP");
	if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
		puts("\tFT_GLYPH_FORMAT_OUTLINE");
	if (face->glyph->format == FT_GLYPH_FORMAT_PLOTTER)
		puts("\tFT_GLYPH_FORMAT_PLOTTER");
	*/
	

	
	// TODO: Quick Hull the points list!
	// TODO: Stop sending so much extra data!
	
	int start = 0;
	for (int i = 0; i < outline->n_contours; i++)
	{
		int contcurcount = curves.count;

		int j = start;
		
		assert(outline->tags[j] & 1);

		int first = j;
		FT_Vector* p = &outline->points[j];
		p1.x = p->x;
		p1.y = p->y;
		j++;

		int lastwascontrol = 0;
		for (; j <= outline->contours[i]; j++)
		{
			if (!(outline->tags[j] & 1))
			{
				// Off the curve, got a control point
				p = &outline->points[j];
				if (!lastwascontrol)
				{
					// Last point wasn't a control, so we can squeeze right in
					p2.x = p->x;
					p2.y = p->y;
				}
				else
				{
					// The last point was also a control point, so 
					// we need to break up this curve into smaller parts for our shader

					nvec2s c;
					c.x = p->x;
					c.y = p->y;

					p3 = ng_scalev2s(ng_addv2s(p2, c), 0.5f);

					// Emit curve
					ng_ustack_push(&curves, &p1);
					ng_ustack_push(&curves, &p2);
					ng_ustack_push(&curves, &p3);

					// Our last point is now our first, and we can use this control point for our second
					p1 = p3;
					p2 = c;

				}
				lastwascontrol = 1;
			}
			else
			{
				// On the curve

				p = &outline->points[j];
				p3.x = p->x;
				p3.y = p->y;

				if (!lastwascontrol)
				{
					// Straight line
					p2 = ng_scalev2s(ng_addv2s(p1, p3), 0.5);


					// Emit partial curve
					//ng_ustack_push(&curves, &p2);
				}


				ng_ustack_push(&curves, &p1);
				ng_ustack_push(&curves, &p2);
				ng_ustack_push(&curves, &p3);


				p1 = p3;



				lastwascontrol = 0;
			}

		}

		// First point is our p3 now
		p = &outline->points[first];
		p3.x = p->x;
		p3.y = p->y;

		if (!lastwascontrol)
		{
			p2 = ng_scalev2s(ng_addv2s(p1, p3), 0.5);

			// Emit partial curve
			//ng_ustack_push(&curves, &p2);
		}
		ng_ustack_push(&curves, &p1);
		ng_ustack_push(&curves, &p2);
		ng_ustack_push(&curves, &p3);

		// End
		contcurcount = (curves.count - contcurcount) / 3;

		ng_ustack_push(&contours, &contcurcount);


		start = outline->contours[i] + 1;
	}

	struct ng_font_glyph glyphdata;
	glyphdata.contours = contours;
	glyphdata.curves = curves;
	glyphdata.width = glyph->metrics.width;
	glyphdata.height = glyph->metrics.height;
	return glyphdata;
}

ng_ustack s_emoji;
short s_emwidth = 0;

void ng_font_glyph_load_color(FT_Face face, FT_UInt glyph_index)
{
	s_emwidth = face->units_per_EM;

	ng_ustack_alloc(&s_emoji, sizeof(struct ng_font_glyph));
	FT_Palette_Data pallet_data;
	FT_Palette_Data_Get(face, &pallet_data);

	FT_Color* palette;
	FT_LayerIterator  iterator;

	FT_Bool  have_layers;
	FT_UInt  layer_glyph_index;
	FT_UInt  layer_color_index;


	FT_Error error = FT_Palette_Select(face, 0, &palette);
	if (error)
		palette = NULL;

	iterator.p = NULL;
	have_layers = FT_Get_Color_Glyph_Layer(face,
		glyph_index,
		&layer_glyph_index,
		&layer_color_index,
		&iterator);

	int z = 0;
	if (palette && have_layers)
	{
		do
		{
			FT_Color  layer_color;

			if (layer_color_index == 0xFFFF)
				layer_color = (FT_Color){ 255, 255, 255, 255 };
			else
				layer_color = palette[layer_color_index];

			// Load and render glyph `layer_glyph_index', then
			// blend resulting pixmap (using color `layer_color')
			// with previously created pixmaps.

			FT_Error lcerr = FT_Load_Glyph(face, layer_glyph_index, FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP | FT_LOAD_IGNORE_TRANSFORM);
			
			struct ng_font_glyph gd = ng_font_glyph_load_outline(face->glyph);
			gd.vertex_color = (nvec4){ layer_color.red / 255.0f, layer_color.green / 255.0f, layer_color.blue / 255.0f, layer_color.alpha / 255.0f, };
			ng_ustack_push(&s_emoji, &gd);


		} while (FT_Get_Color_Glyph_Layer(face,
			glyph_index,
			&layer_glyph_index,
			&layer_color_index,
			&iterator));

	}

}


static struct ng_font_glyph ng_font_glyph_load(FT_Face face, FT_UInt glyph_index)
{
	FT_Error lcerr = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP | FT_LOAD_IGNORE_TRANSFORM);

	FT_GlyphSlot glyph = face->glyph;


	if (glyph->face->face_flags & FT_FACE_FLAG_COLOR)
	{
		puts("FT_FACE_FLAG_COLOR");
		ng_font_glyph_load_color(face, glyph_index);
	}
}

static void ng_font_character_load(FT_Face face, FT_ULong character_code)
{
	if (character_code > 127 && character_code < 127 + 32) character_code += 0x1F600 - 128;

	FT_UInt glyphindex = FT_Get_Char_Index(face, character_code);
	ng_font_glyph_load(face, glyphindex);
}


#define glyph_count 256
int glyphs_loaded = 0;


ng_font* ng_font_load(const char* path)
{
	// Load the font out of Freetype
	FT_Face face;
	FT_Error error = FT_New_Face(ng_freetype_lib, path, 0, &face);
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);


	assert(error == 0);
	
	unsigned int byte_count = 0;


	struct ng_font_glyph glyphs[glyph_count];


	// Load all of ascii for now
	ng_font_character_load(face, 0x1F914); //0x2728);// 0x1F600);

	glyphs_loaded = s_emoji.count;
	for (int i = 0; i < s_emoji.count; i++)
	{
		struct ng_font_glyph* g = &glyphs[i];
		*g = *(struct ng_font_glyph*)ng_ustack_at(&s_emoji, i);

		if (g->contours.count > 0 && g->curves.count > 0)
		{
			byte_count += sizeof(struct ng_font_shader_data_glyph_header);

			byte_count += ng_ustack_size(&g->contours);
			byte_count += ng_ustack_size(&g->curves);
		}
	}

#if 0
	{
		int hullpointcount = 0;
		for (int i = 0; i < s_emoji.count; i++)
		{
			struct ng_font_glyph* g = ng_ustack_at(&s_emoji, i);
			hullpointcount += g->curves.count;
		}

		nvec2s* hullpoints = malloc(sizeof(nvec2s) * hullpointcount);
		int hh = 0;
		for (int i = 0; i < s_emoji.count; i++)
		{
			struct ng_font_glyph* g = ng_ustack_at(&s_emoji, i);
			
			ng_ustack_pour(&g->curves, hullpoints + hh, sizeof(nvec2s) * (hullpointcount - hh));
			hh += g->curves.count;
		}


		ng_font_glyph_hull(hullpoints, hullpointcount);
	}
#endif

	/*
	for (int i = 0; i < glyph_count; i++)
	{
		struct ng_font_glyph* g = &glyphs[i];
		*g = ng_font_character_load(face, i);
		
	
		if (g->contours.count > 0 && g->curves.count > 0)
		{
			byte_count += sizeof(struct ng_font_shader_data_glyph_header);
			
			byte_count += ng_ustack_size(&g->contours);
			byte_count += ng_ustack_size(&g->curves);
		}
	}
	*/

	// Allocate space for the texture
	byte_count += sizeof(struct ng_font_shader_data);

	// Round it to the nearest power of 2 for the 2 bytes of R16
	unsigned int down =  pow(2, floor(log2(sqrt(byte_count / 2.0))));
	unsigned int up = down * 2;
	
	unsigned int nsz, width, height;

	// Fit it into the smallest rectagle we can
	// TODO: Can we shrink this down into just an equation?
	if ((nsz = down * down * 2) >= byte_count)
	{
		width = down;
		height = down;
	}
	else if((nsz = up * down * 2) >= byte_count)
	{
		width = up;
		height = down;
	}
	else if ((nsz = up * up * 2) >= byte_count)
	{
		width = up;
		height = up;
	}

	printf("%d bytes remaining\n", nsz - byte_count);
	byte_count = nsz;

	char* tex = malloc(byte_count);

	short* texs = tex;

	// Place the data into a texture
	char* cur = tex;
	char* end = tex + byte_count;
	struct ng_font_shader_data* sd = cur;
	cur = sd + 1;

	// Scaling factor
	sd->units_per_em = face->units_per_EM;

	for (int i = 0; i < glyphs_loaded; i++)
	{
		struct ng_font_glyph* glyph = &glyphs[i];

		if (glyph->contours.count == 0 || glyph->curves.count == 0)
		{
			sd->character_offsets[i] = -1;
			continue;
		}

		struct ng_font_shader_data_glyph_header* gh = cur;
		cur = gh + 1;

		sd->character_offsets[i] = ((char*)(gh) - tex) / 2;

		gh->width = glyph->width;
		gh->contour_count = glyph->contours.count;

		cur += ng_ustack_pour(&glyph->contours, cur, end - cur);
		cur += ng_ustack_pour(&glyph->curves,   cur, end - cur);
	}
	assert(cur < end);

	// Fill the remaining space with 0xFE
	memset(cur, 0xFE, end - cur);

	ng_font_texture = ng_texture_create(NG_TEXTURE_FORMAT_R16I, tex, width, height);

	FT_Done_Face(face);


	return 0;
}


static ng_texture* ng_font_whitetexture_ = 0;

void aaa() {


	ng_font_shader = ng_shader_create(ng_font_vs, ng_font_fs);
	

	ng_vtxbuf vb;
	ng_idxbuf ib;
	ng_vtxbuf_alloc(&vb, 4 * glyphs_loaded);
	ng_idxbuf_alloc(&ib, 6 * glyphs_loaded);

	ng_vertex a[4 * glyph_count];
	
	for (int i = 0; i < glyphs_loaded; i++)
	{
		nvec2 pos = { 0, 3 };
		//nvec2 pos = { i % 16, i / 16 };
		int d = i * 4;

		struct ng_font_glyph* g = (struct ng_font_glyph*)ng_ustack_at(&s_emoji, i);
		
		float l = 0;
		float r = 2;
		float t = -1;
		float b = 1;

		

		a[d + 0] = (ng_vertex){ {pos.x+l, pos.y+b,i},g->vertex_color,{l, t}, i};
		a[d + 1] = (ng_vertex){ {pos.x+r, pos.y+b,i},g->vertex_color,{r, t}, i};
		a[d + 2] = (ng_vertex){ {pos.x+r, pos.y+t,i},g->vertex_color,{r, b}, i};
		a[d + 3] = (ng_vertex){ {pos.x+l, pos.y+t,i},g->vertex_color,{l, b}, i};
	}


	ng_vtxbuf_push_many(&vb, &a[0], 4 * glyphs_loaded);

	for(int i = 0; i < glyphs_loaded; i++)
		ng_idxbuf_push_quad(&ib, i*4);

	ng_testvbo = ng_render_create_vbo(&vb);
	ng_testibo = ng_render_create_ibo(&ib);

	ng_vtxbuf_free(&vb);
	ng_idxbuf_free(&ib);


	int white_pixel = 0xFFFFFFFF;
	ng_font_whitetexture_ = ng_texture_create(NG_TEXTURE_FORMAT_RGBA8, &white_pixel, 1, 1);
	
}

extern ng_shader* ng_painter_default_shader;
void ng_font_test()
{


	float scl = 60;
	nmat4x4 model = 
	{{scl,  0,  0,0},
	 {  0,scl,  0,0},
	 {  0,  0,  1,0},
	 { 0, 0,200,1}};

	ng_shader_bind(ng_font_shader);
	
	ng_shader_set(NG_SHADERPARAM_MODEL, &model);


	// Set the data to unit 0 and color to unit 1
	int td = 0;
	int tc = 1;
	ng_shader_set(NG_SHADERPARAM_TEXTUREDATA, &td);
	ng_shader_set(NG_SHADERPARAM_TEXTURECOLOR, &tc);

	// Set unit 0, the data texture, to our data
	ng_texture_bind(ng_font_texture, 0);
	
	// Set unit 1, the color texture, to our texture
	ng_texture_bind(ng_font_whitetexture_, 1);


	ng_render_draw_mesh(ng_testvbo, ng_testibo, 0, 6 * glyph_count);

	float a = 45.0 / 180.0 * 3.14;
	model = (nmat4x4)
	{{scl*cos(a), -scl*sin(a),   0, 0},
	 {scl*sin(a),  scl*cos(a),   0, 0},
	 {         0,           0, scl, 0},
	 {        60,          60, 200, 1}};
	ng_shader_set(NG_SHADERPARAM_MODEL, &model);
	ng_render_draw_mesh(ng_testvbo, ng_testibo, 0, 6 * glyph_count);


}
