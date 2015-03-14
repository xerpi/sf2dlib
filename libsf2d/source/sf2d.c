#include "sf2d.h"

#define GPU_CMD_SIZE 0x40000

static int sf2d_initialized = 0;
static u32 *gpu_cmd = NULL;


int sf2d_init()
{
	if (sf2d_initialized) return 0;
	
	gfxInitDefault();
	gfxSet3D(false);
	
	gpu_cmd = (u32 *)linearAlloc(GPU_CMD_SIZE * 4);
	
	GPU_Reset(NULL, gpu_cmd, GPU_CMD_SIZE);
	
	GPUCMD_Finalize();
	GPUCMD_FlushAndRun(NULL);
	gspWaitForP3D();
	
	sf2d_initialized = 1;
	
	return 1;
}

int sf2d_fini()
{
	gspWaitForPSC0();
	gfxExit();
	
	sf2d_initialized = 0;
	
	return 1;
}
