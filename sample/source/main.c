#include <string.h>
#include <3ds.h>
#include <sf2d.h>

typedef struct {
	float x, y, z;
} vector_3f;

typedef struct {
	float r, g, b, a;
} vector_4f;

typedef struct {
	vector_3f position;
	vector_4f color;
} vertex_pos_col;

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

static const vertex_pos_col triangle_mesh[] =
{
	//{y, x, z}, {r, g, b, a}
	{{240.0f+60.0f, 120.0f,       0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{240.0f-60.0f, 120.0f+60.0f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{240.0f-60.0f, 120.0f-60.0f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};

int main()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0x40));

	u32 *p = linearAlloc(sizeof(triangle_mesh));
	memcpy(p, triangle_mesh, sizeof(triangle_mesh));

	while (aptMainLoop()) {
		sf2d_start_frame();
		hidScanInput();

		GPU_SetAttributeBuffers(
			2, // number of attributes
			(u32*)osConvertVirtToPhys((u32)p),
			GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_FLOAT), // (float){x, y, z}, (float){r, g, b, a}
			0xFF,
			0x10,
			1, //number of buffers
			(u32[]){0x0}, // buffer offsets (placeholders)
			(u64[]){0x10}, // attribute permutations for each buffer
			(u8[]){2} // number of attributes for each buffer
		);
		
		GPU_DrawArray(GPU_TRIANGLES, sizeof(triangle_mesh)/sizeof(triangle_mesh[0]));

		if (hidKeysDown() & KEY_START) break;

		sf2d_end_frame();
	}
	
	linearFree(p);
	
	sf2d_fini();
	return 0;
}
