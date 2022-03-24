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

static FT_Library ng_freetype_lib;

static ng_vbo ng_testvbo;
static ng_ibo ng_testibo;

ng_shader* ng_font_shader;

ng_texture* ng_font_texture;

extern const unsigned char ng_font_vs[];
extern const unsigned char ng_font_fs[];

struct dy_font_glyph
{
	ng_ustack /*nvec2s*/         contours;
	ng_ustack /*unsigned short*/ curves;

	short width;
	short height;
};

struct dy_font_shader_data_glyph_header
{
	short width;
	short contour_count;

	// These two are stored immediately after this struct
	// unsigned short* contours;
	// nvec2s* curves;
};

struct dy_font_shader_data
{
	short units_per_em;
	// Maps up ascii codes to offsets in shorts in the image
	// (-1) means "Don't draw" or "Blank glyph"
	short character_offsets[128];

	// After this struct is an array of dy_font_shader_data_glyph_header and their data

};

void aaa();

void ng_font_init()
{

	FT_Error error = FT_Init_FreeType(&ng_freetype_lib);
	if (error)
	{
		printf("error!!");
	}

	ng_font_load("Ubuntu-R.ttf");
	
	aaa();
}

void ng_font_destroy(ng_font* font)
{

}

static struct dy_font_glyph ng_font_demand_glyph(FT_Face face, char character);

#define glyph_count 128

ng_font* ng_font_load(const char* path)
{
	// Load the font out of Freetype
	FT_Face face;
	FT_Error error = FT_New_Face(ng_freetype_lib, path, 0, &face);


	assert(error == 0);
	
	unsigned int byte_count = 0;


	struct dy_font_glyph glyphs[glyph_count];

	// Load all of ascii for now
	for (int i = 0; i < glyph_count; i++)
	{
		struct dy_font_glyph* g = &glyphs[i];
		
		*g = ng_font_demand_glyph(face, i);
	
		if (g->contours.count > 0 && g->curves.count > 0)
		{
			byte_count += sizeof(struct dy_font_shader_data_glyph_header);
			
			byte_count += ng_ustack_size(&g->contours);
			byte_count += ng_ustack_size(&g->curves);
		}
	}

	// Allocate space for the texture
	byte_count += sizeof(struct dy_font_shader_data);

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
	struct dy_font_shader_data* sd = cur;
	cur = sd + 1;

	// Scaling factor
	sd->units_per_em = face->units_per_EM;

	for (int i = 0; i < glyph_count; i++)
	{
		struct dy_font_glyph* glyph = &glyphs[i];

		if (glyph->contours.count == 0 || glyph->curves.count == 0)
		{
			sd->character_offsets[i] = -1;
			continue;
		}

		struct dy_font_shader_data_glyph_header* gh = cur;
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

#undef glyph_count



static struct dy_font_glyph ng_font_demand_glyph(FT_Face face, char character)
{

	ng_ustack curves;
	ng_ustack_alloc(&curves, sizeof(nvec2s));

	ng_ustack contours;
	ng_ustack_alloc(&contours, sizeof(unsigned short));



	FT_Error lcerr = FT_Load_Char(face, character, FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP);
	FT_Outline* outline = &face->glyph->outline;
	nvec2s p1, p2, p3;

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

	struct dy_font_glyph glyph;
	glyph.contours = contours;
	glyph.curves = curves;
	glyph.width = face->glyph->metrics.width;
	glyph.height = face->glyph->metrics.height;
	return glyph;
}


int charcount = 0;
void aaa() {



	charcount = 128;

	ng_font_shader = ng_shader_create(ng_font_vs, ng_font_fs);
	

	ng_vtxbuf vb;
	ng_idxbuf ib;
	ng_vtxbuf_alloc(&vb, 4 * charcount);
	ng_idxbuf_alloc(&ib, 6 * charcount);

	ng_vertex a[4 * 128];
	
	for (int i = 0; i < 128; i++)
	{
		nvec2 pos = { i % 16, i / 16 };
		int d = i * 4;

		a[d + 0] = (ng_vertex){ {pos.x+0, pos.y+0,0},{1,1,1},{0.0,  0.8}, i};
		a[d + 1] = (ng_vertex){ {pos.x+1, pos.y+0,0},{1,1,1},{1.0,  0.8}, i};
		a[d + 2] = (ng_vertex){ {pos.x+1, pos.y+1,0},{1,1,1},{1.0, -0.2}, i};
		a[d + 3] = (ng_vertex){ {pos.x+0, pos.y+1,0},{1,1,1},{0.0, -0.2}, i};
	}


	ng_vtxbuf_push_many(&vb, &a[0], 4 * charcount);

	for(int i = 0; i < charcount; i++)
		ng_idxbuf_push_quad(&ib, i*4);

	ng_testvbo = ng_render_create_vbo(&vb);
	ng_testibo = ng_render_create_ibo(&ib);

	ng_vtxbuf_free(&vb);
	ng_idxbuf_free(&ib);


}

extern ng_shader* ng_painter_default_shader;
void ng_font_test()
{

	float scl = 120.0;
	//float scl = 10 + fabs(cos(clock() / (float)CLOCKS_PER_SEC)) * 90.0;
	nmat4x4 model = 
	{{scl,  0,  0,0},
	 {  0,scl,  0,0},
	 {  0,  0,  1,0},
	 { 0, 0,200,1}};

	ng_shader_set(NG_SHADERPARAM_MODEL, &model);


	ng_shader_bind(ng_font_shader);
	ng_texture_bind(ng_font_texture);
	ng_render_draw_mesh(ng_testvbo, ng_testibo, 0, 6 * charcount);
}
