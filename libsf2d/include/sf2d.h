#ifndef SF2D_H
#define SF2D_H

#include <3ds.h>

// Defines

#define RGBA8(r, g, b, a) ((((r)&0xFF)<<24) | (((g)&0xFF)<<16) | (((b)&0xFF)<<8) | (((a)&0xFF)<<0))

// Structs
typedef struct {
	float x, y, z;
} sf2d_vector_3f;

typedef struct {
	float r, g, b, a;
} sf2d_vector_4f;

typedef struct {
	sf2d_vector_3f position;
	sf2d_vector_4f color;
} sf2d_vertex_pos_col;

// Basic functions

int sf2d_init();
int sf2d_fini();

void sf2d_start_frame();
void sf2d_end_frame();

void sf2d_set_clear_color(u32 color);

// Draw functions
void sf2d_draw_rectangle(int x, int y, int w, int h, u32 color);

#endif
