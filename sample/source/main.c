#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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

int main()
{
	// Set the random seed based on the time
	srand(time(NULL));

	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));


	sf2d_texture *tex1 = sf2d_create_texture_mem_RGBA8(dice_img.pixel_data, dice_img.width, dice_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_texture *tex2 = sf2d_create_texture_mem_RGBA8(citra_img.pixel_data, citra_img.width, citra_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);

	float rad = 0.0f;
	u16 touch_x = 320/2;
	u16 touch_y = 240/2;
	touchPosition touch;
	circlePosition circle;
	u32 held;

	while (aptMainLoop()) {

		hidScanInput();
		hidCircleRead(&circle);
		held = hidKeysHeld();

		if (held & KEY_START) {
			break;
		} else if (held & KEY_TOUCH) {
			hidTouchRead(&touch);
			touch_x = touch.px;
			touch_y = touch.py;
		} else if (held & (KEY_L | KEY_R)) {
			sf2d_set_clear_color(RGBA8(rand()%255, rand()%255, rand()%255, 255));
		}

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_rectangle_rotate(260, 20, 40, 40, RGBA8(0xFF, 0xFF, 0x00, 0xFF), -2.0f*rad);
			sf2d_draw_rectangle(20, 60, 40, 40, RGBA8(0xFF, 0x00, 0x00, 0xFF));
			sf2d_draw_rectangle(5, 5, 30, 30, RGBA8(0x00, 0xFF, 0xFF, 0xFF));
			sf2d_draw_texture_rotate(tex1, 400/2 + circle.dx, 240/2 - circle.dy, rad);
		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_rectangle_rotate(190, 160, 70, 60, RGBA8(0xFF, 0xFF, 0xFF, 0xFF), 3.0f*rad);
			sf2d_draw_rectangle(30, 100, 40, 60, RGBA8(0xFF, 0x00, 0xFF, 0xFF));
			sf2d_draw_texture_rotate(tex2, touch_x, touch_y, -rad);
			sf2d_draw_rectangle(160-15 + cosf(rad)*50.0f, 120-15 + sinf(rad)*50.0f, 30, 30, RGBA8(0x00, 0xFF, 0xFF, 0xFF));
			sf2d_draw_fill_circle(40, 40, 35, RGBA8(0x00, 0xFF, 0x00, 0xFF));
		sf2d_end_frame();

		rad += 0.2f;

		sf2d_swapbuffers();
	}

	sf2d_free_texture(tex1);
	sf2d_free_texture(tex2);

	sf2d_fini();
	return 0;
}
