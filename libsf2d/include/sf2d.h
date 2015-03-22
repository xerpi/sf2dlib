#ifndef SF2D_H
#define SF2D_H

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

// Defines

#define RGBA8(r, g, b, a) ((((r)&0xFF)<<24) | (((g)&0xFF)<<16) | (((b)&0xFF)<<8) | (((a)&0xFF)<<0))

// Enums

typedef enum {
	SF2D_PLACE_RAM,
	SF2D_PLACE_VRAM
} sf2d_place;

// Structs

typedef struct {
	float u, v;
} sf2d_vector_2f;

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

typedef struct {
	sf2d_vector_3f position;
	sf2d_vector_2f texcoord;
} sf2d_vertex_pos_tex;

typedef struct {
	sf2d_place place;
	GPU_TEXCOLOR pixel_format;
	int width, height;
	int pow2_w, pow2_h;
	int data_size;
	void *data;
} sf2d_texture;

// Basic functions

int sf2d_init();
int sf2d_fini();

void sf2d_start_frame(gfxScreen_t screen, gfx3dSide_t side);
void sf2d_end_frame();
void sf2d_swapbuffers();

void *sf2d_pool_malloc(u32 size);
void *sf2d_pool_memalign(u32 size, u32 alignment);
void sf2d_pool_reset();

void sf2d_set_clear_color(u32 color);

// Draw functions
void sf2d_draw_line(int x0, int y0, int x1, int y1, u32 color);
void sf2d_draw_rectangle(int x, int y, int w, int h, u32 color);

// Texture
sf2d_texture *sf2d_create_texture(int width, int height, GPU_TEXCOLOR pixel_format, sf2d_place place);
void sf2d_free_texture(sf2d_texture *texture);
void sf2d_bind_texture(const sf2d_texture *texture, GPU_TEXUNIT unit);
void sf2d_draw_texture(const sf2d_texture *texture, int x, int y);
void sf2d_draw_texture_rotate(const sf2d_texture *texture, int x, int y, float rad);
void texture_tile32(const u32 *src, u32 *dst, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
