#include "sf2d.h"
#include "shader_vsh_shbin.h"

#define GPU_CMD_SIZE 0x40000

static int sf2d_initialized = 0;
static u32 clear_color = RGBA8(0x00, 0x00, 0x00, 0xFF);
static u32 *gpu_cmd = NULL;
//GPU framebuffer address
static u32 *gpu_fb_addr = (u32 *)0x1F119400;
//GPU depth buffer address
static u32 *gpu_depth_fb_addr = (u32 *)0x1F370800;
//Shader stuff
static DVLB_s *dvlb;
static shaderProgram_s shader;
static u32 projection_desc;
static u32 modelview_desc;
//Matrix
static float ortho_matrix[4*4];

//stolen from staplebutt
static void GPU_SetDummyTexEnv(u8 num)
{
	GPU_SetTexEnv(num,
		GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
		GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_REPLACE,
		GPU_REPLACE,
		0xFFFFFFFF);
}

int sf2d_init()
{
	if (sf2d_initialized) return 0;
	
	gfxInitDefault();
	GPU_Init(NULL);
	gfxSet3D(false);
	
	gpu_cmd = (u32 *)linearAlloc(GPU_CMD_SIZE * 4);
	
	GPU_Reset(NULL, gpu_cmd, GPU_CMD_SIZE);
	
	//Setup the shader
	dvlb = DVLB_ParseFile((u32 *)shader_vsh_shbin, shader_vsh_shbin_size);
	shaderProgramInit(&shader);
	shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);
	
	//Get shader uniform descriptors
	projection_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");
	modelview_desc = shaderInstanceGetUniformLocation(shader.vertexShader, "modelview");
	
	shaderProgramUse(&shader);
	
	//ortho_matrix = initOrtho...

	GPUCMD_Finalize();
	GPUCMD_FlushAndRun(NULL);
	gspWaitForP3D();
	
	sf2d_initialized = 1;
	
	return 1;
}

int sf2d_fini()
{
	if (!sf2d_initialized) return 0;
	
	gfxExit();
	shaderProgramFree(&shader);
	DVLB_Free(dvlb);
	
	sf2d_initialized = 0;
	
	return 1;
}

void sf2d_start_frame()
{
	GPUCMD_SetBufferOffset(0);
	GPU_SetViewport((u32 *)osConvertVirtToPhys((u32)gpu_depth_fb_addr), 
					(u32 *)osConvertVirtToPhys((u32)gpu_fb_addr),
					0, 0, 240*2, 400);
	GPU_DepthMap(-1.0f, 0.0f);
	GPU_SetFaceCulling(GPU_CULL_BACK_CCW);
	GPU_SetStencilTest(false, GPU_ALWAYS, 0x00, 0xFF, 0x00);
	GPU_SetStencilOp(GPU_KEEP, GPU_KEEP, GPU_KEEP);
	GPU_SetBlendingColor(0,0,0,0);
	GPU_SetDepthTestAndWriteMask(true, GPU_GREATER, GPU_WRITE_ALL);	
	GPUCMD_AddMaskedWrite(GPUREG_0062, 0x1, 0);
	GPUCMD_AddWrite(GPUREG_0118, 0);

	GPU_SetAlphaBlending(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);
	GPU_SetAlphaTest(false, GPU_ALWAYS, 0x00);
	
	/*GPU_SetTextureEnable(GPU_TEXUNIT0);

	GPU_SetTexEnv(0,
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVOPERANDS(0,0,0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_MODULATE, GPU_MODULATE,
		0xFFFFFFFF);*/
	GPU_SetDummyTexEnv(0);
	GPU_SetDummyTexEnv(1);
	GPU_SetDummyTexEnv(2);
	GPU_SetDummyTexEnv(3);
	GPU_SetDummyTexEnv(4);
	GPU_SetDummyTexEnv(5);
	
	GPU_SetFloatUniform(GPU_VERTEX_SHADER, projection_desc, (void *)ortho_matrix, 4);
}

void sf2d_end_frame()
{
	GPU_FinishDrawing();
	GPUCMD_Finalize();
	GPUCMD_FlushAndRun(NULL);
	gspWaitForP3D();

	//Draw the screen
	GX_SetDisplayTransfer(NULL, gpu_fb_addr, 0x019001E0, (u32*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 0x019001E0, 0x01001000);
	gspWaitForPPF();

	//Clear the screen
	GX_SetMemoryFill(NULL, gpu_fb_addr, clear_color, &gpu_fb_addr[0x2EE00],
		0x201, gpu_depth_fb_addr, 0x00000000, &gpu_depth_fb_addr[0x2EE00], 0x201);
	gspWaitForPSC0();
	gfxSwapBuffersGpu();

	gspWaitForEvent(GSPEVENT_VBlank0, true);
}

void sf2d_set_clear_color(u32 color)
{
	clear_color = color;
}
