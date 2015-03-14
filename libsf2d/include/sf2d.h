#ifndef SF2D_H
#define SF2D_H

#include <3ds.h>

#define RGBA8(r, g, b, a) ((((r)&0xFF)<<24) | (((g)&0xFF)<<16) | (((b)&0xFF)<<8) | (((a)&0xFF)<<0))

int sf2d_init();
int sf2d_fini();

void sf2d_start_frame();
void sf2d_end_frame();

void sf2d_set_clear_color(u32 color);

#endif
