#include <string.h>
#include <3ds.h>
#include <sf2d/sf2d.h>

int main()
{
	sf2d_init();

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		// Your code goes here

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Example rendering code that displays a white pixel
		// Please note that the 3DS screens are sideways (thus 240x400 and 240x320)
		u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		memset(fb, 0, 240*400*3);
		fb[3*(10+10*240)] = 0xFF;
		fb[3*(10+10*240)+1] = 0xFF;
		fb[3*(10+10*240)+2] = 0xFF;

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	sf2d_fini();
	return 0;
}
