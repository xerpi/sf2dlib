#include <string.h>
#include <3ds.h>
#include <sf2d.h>

/*      ______________________
	   |                      |
	   |                      |
	   |                      |
	   |                      |
	   |                      |
	   |______________________| ^
	                            | x
	                       <-----
	                         y
*/

static const sf2d_vertex_pos_col triangle_mesh[] =
{
	//{y, x, z}, {r, g, b, a}
	{{240.0f+60.0f, 120.0f,       0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{240.0f-60.0f, 120.0f+60.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{240.0f-60.0f, 120.0f-60.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};

static void *triangle_data = NULL;
static void draw_triangle();

int main()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0x40));

	triangle_data = linearAlloc(sizeof(triangle_mesh));
	memcpy(triangle_data, triangle_mesh, sizeof(triangle_mesh));

	sf2d_texture *tex = sf2d_create_texture(64, 64, GPU_RGBA8, SF2D_PLACE_VRAM);
	//Fill texture
	int i, j;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			((u32 *)tex->data)[j + 64*i] = RGBA8(0, 255, 255, 100);
		}
	}

	while (aptMainLoop()) {
		sf2d_start_frame();
		hidScanInput();

		draw_triangle();
		
		sf2d_draw_rectangle(200, 20, 40, 40, RGBA8(0xFF, 0xFF, 0x00, 0xFF));
		sf2d_draw_rectangle(20, 20, 40, 40, RGBA8(0xFF, 0x00, 0x00, 0xFF));
		sf2d_draw_rectangle(300, 150, 80, 60, RGBA8(0x00, 0x00, 0xFF, 0xFF));
		sf2d_draw_rectangle(30, 100, 40, 60, RGBA8(0xFF, 0x00, 0xFF, 0xFF));

		sf2d_draw_texture(tex, 180, 100);

		if (hidKeysDown() & KEY_START) break;

		sf2d_end_frame();
	}
	
	linearFree(triangle_data);
	sf2d_free_texture(tex);
	
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
