#include <stdio.h>
#include <string.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

extern const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel;
  unsigned char	 pixel_data[];
} citra_img;

extern const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel;
  unsigned char	 pixel_data[];
} dice_img;

static const sf2d_vertex_pos_col triangle_mesh[] =
{
	{{120.0f,       120.0f-60.0f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{120.0f-60.0f, 120.0f+60.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{120.0f+60.0f, 120.0f+60.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};

static void *triangle_data = NULL;
static void draw_triangle();

int main()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	triangle_data = linearAlloc(sizeof(triangle_mesh));
	memcpy(triangle_data, triangle_mesh, sizeof(triangle_mesh));

	sf2d_texture *tex1 = sf2d_create_texture(dice_img.width, dice_img.height, GPU_RGBA8, SF2D_PLACE_RAM);
	sf2d_fill_texture_from_RGBA8(tex1, dice_img.pixel_data, dice_img.width, dice_img.height);
	sf2d_texture_tile32(tex1);

	sf2d_texture *tex2 = sf2d_create_texture(citra_img.width, citra_img.height, GPU_RGBA8, SF2D_PLACE_RAM);
	sf2d_fill_texture_from_RGBA8(tex2, citra_img.pixel_data, citra_img.width, citra_img.height);
	sf2d_texture_tile32(tex2);

	float rad = 0.0f;

	while (aptMainLoop()) {

		hidScanInput();
		if (hidKeysDown() & KEY_START) break;

		sf2d_start_frame(GFX_TOP, GFX_LEFT);

			draw_triangle();
			sf2d_draw_rectangle_rotate(260, 20, 40, 40, RGBA8(0xFF, 0xFF, 0x00, 0xFF), -2.0f*rad);
			sf2d_draw_rectangle(20, 60, 40, 40, RGBA8(0xFF, 0x00, 0x00, 0xFF));
			sf2d_draw_rectangle(5, 5, 30, 30, RGBA8(0x00, 0xFF, 0xFF, 0xFF));
			sf2d_draw_texture_rotate(tex1, 200-tex1->width/2, 120-tex1->height/2, rad);

		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

			draw_triangle();
			sf2d_draw_rectangle_rotate(190, 160, 70, 60, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), 3.0f*rad);
			sf2d_draw_rectangle(30, 100, 40, 60, RGBA8(0xFF, 0x00, 0xFF, 0xFF));
			sf2d_draw_texture_rotate(tex2, 190, 120-tex2->height/2, -rad);
			sf2d_draw_rectangle(160-15 + cosf(rad)*50.0f, 120-15 + sinf(rad)*50.0f, 30, 30, RGBA8(0x00, 0xFF, 0xFF, 0xFF));

		sf2d_end_frame();

		rad += 0.2f;

		sf2d_swapbuffers();
	}
	
	linearFree(triangle_data);
	sf2d_free_texture(tex1);
	sf2d_free_texture(tex2);
	
	sf2d_fini();
	return 0;
}


void draw_triangle()
{
	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_MODULATE, GPU_MODULATE,
		0xFFFFFFFF
	);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys((u32)triangle_data),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLES, sizeof(triangle_mesh)/sizeof(triangle_mesh[0]));
}
