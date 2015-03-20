#include <string.h>
#include <3ds.h>
#include <sf2d/sf2d.h>

typedef struct {
	float x, y, z, w;
} vector_4f;

typedef struct {
	unsigned char r, g, b, a;
} vector_4ub;

typedef struct {
	vector_4f position;
	vector_4ub color;
} vertex_pos_col;

static const vertex_pos_col triangle_mesh[] __attribute__((aligned(128)))= {
	{{-0.5f, -0.5f, +0.5f, 1.0f}, {0xFF, 0xFF, 0x00, 0x00}},
	{{+0.5f, -0.5f, +0.5f, 1.0f}, {0x00, 0xFF, 0xFF, 0x00}},
	{{+0.5f, +0.5f, +0.5f, 1.0f}, {0xFF, 0xFF, 0x00, 0x00}}
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
	sf2d_set_clear_color(RGBA8(0xFF, 0x00, 0x00, 0xFF));
	
	int x = 0;

	while (aptMainLoop()) {
		sf2d_start_frame();
		hidScanInput();

		GPU_SetAttributeBuffers(
			2, // number of attributes
			(u32*)osConvertVirtToPhys((u32)triangle_mesh),
			GPU_ATTRIBFMT(0, 4, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_UNSIGNED_BYTE), // (float){x, y, z, w}, (uchar){r, g, b, a}
			0xFF,
			0x10,
			2, //number of buffers
			(u32[]){0x0, 0x0}, // buffer offsets (placeholders)
			(u64[]){0x0, 0x1}, // attribute permutations for each buffer
			(u8[]){1, 1} // number of attributes for each buffer
		);
		
		/*
		GPU_SetAttributeBuffers(u8 totalAttributes,
			u32* baseAddress,
			u64 attributeFormats,
			u16 attributeMask,
			u64 attributePermutation,
			u8 numBuffers,
			u32 bufferOffsets[],
			u64 bufferPermutations[],
			u8 bufferNumAttributes[]);
		*/
		
		GPU_DrawArrayDirectly(GPU_TRIANGLES, (u8 *)triangle_mesh, sizeof(triangle_mesh)/sizeof(triangle_mesh[0]));
		
		if (hidKeysDown() & KEY_START) break;

		sf2d_end_frame();
	}

	sf2d_fini();
	return 0;
}
