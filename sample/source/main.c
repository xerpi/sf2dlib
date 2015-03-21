#include <string.h>
#include <3ds.h>
#include <sf2d/sf2d.h>

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

static const vertex_pos_col triangle_mesh[] __attribute__((aligned(128)))=
{
	//{y, x, z}, {r, g, b, a}
	{{240.0f+60.0f, 120.0f,       0.5f}, {1.0f, 0.0f, 0.0f, 0.0f}},
	{{240.0f-60.0f, 120.0f+60.0f, 0.5f}, {1.0f, 0.0f, 0.0f, 0.0f}},
	{{240.0f-60.0f, 120.0f-60.0f, 0.5f}, {1.0f, 0.0f, 0.0f, 0.0f}},
};

static void GPU_DrawArrayDirectly(GPU_Primitive_t primitive, u8* data, u32 n)
{
	//set attribute buffer address
	GPUCMD_AddSingleParam(0x000F0200, (osConvertVirtToPhys((u32)data))>>3);
	//set primitive type
	GPUCMD_AddSingleParam(0x0002025E, primitive);
	GPUCMD_AddSingleParam(0x0002025F, 0x00000001);
	//index buffer not used for drawArrays but 0x000F0227 still required
	GPUCMD_AddSingleParam(0x000F0227, 0x80000000);
	//pass number of vertices
	GPUCMD_AddSingleParam(0x000F0228, n);

	GPUCMD_AddSingleParam(0x00010253, 0x00000001);

	GPUCMD_AddSingleParam(0x00010245, 0x00000000);
	GPUCMD_AddSingleParam(0x000F022E, 0x00000001);
	GPUCMD_AddSingleParam(0x00010245, 0x00000001);
	GPUCMD_AddSingleParam(0x000F0231, 0x00000001);

	// GPUCMD_AddSingleParam(0x000F0111, 0x00000001); //breaks stuff
}

int main()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0xFF, 0xFF, 0xFF, 0xFF));

	while (aptMainLoop()) {
		sf2d_start_frame();
		hidScanInput();

		GPU_SetAttributeBuffers(
			2, // number of attributes
			(u32*)osConvertVirtToPhys((u32)triangle_mesh),
			GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_FLOAT), // (float){x, y, z}, (float){r, g, b, a}
			0xFF,
			0x10,
			1, //number of buffers
			(u32[]){0x0}, // buffer offsets (placeholders)
			(u64[]){0x10}, // attribute permutations for each buffer
			(u8[]){2} // number of attributes for each buffer
		);
		
		GPU_DrawArrayDirectly(GPU_TRIANGLES, triangle_mesh, sizeof(triangle_mesh)/sizeof(triangle_mesh[0]));
		
		if (hidKeysDown() & KEY_START) break;

		sf2d_end_frame();
	}

	sf2d_fini();
	return 0;
}
