#include <stdlib.h>
#include "sf2d.h"
#include "sf2d_private.h"

sf2d_texture *sf2d_create_texture(int width, int height, GPU_TEXCOLOR pixel_format, sf2d_place place)
{
	int pow2_w = next_pow2(width);
	int pow2_h = next_pow2(height);
	int data_size;

	switch (pixel_format) {
	case GPU_RGBA8:
	default:
		data_size = pow2_w * pow2_h * 4;
		break;
	case GPU_RGB8:
		data_size = pow2_w * pow2_h * 3;
		break;
	case GPU_RGBA5551:
	case GPU_RGB565:
		data_size = pow2_w * pow2_h * 2;
		break;
	}

	sf2d_texture *texture;

	if (place == SF2D_PLACE_RAM) {
		// If there's not enough linear heap space, return
		if (linearSpaceFree() < data_size) {
			return NULL;
		}
		texture = malloc(sizeof(*texture));
		texture->data = linearMemAlign(data_size, 0x80);

	} else if (place == SF2D_PLACE_VRAM) {
		// If there's not enough VRAM heap space, return
		if (vramSpaceFree() < data_size) {
			return NULL;
		}
		texture = malloc(sizeof(*texture));
		texture->data = vramMemAlign(data_size, 0x80);

	} else {
		//wot?
		return NULL;
	}

	texture->place = place;
	texture->pixel_format = pixel_format;
	texture->width = width;
	texture->height = height;
	texture->pow2_w = pow2_w;
	texture->pow2_h = pow2_h;
	texture->data_size = data_size;

	return texture;
}

void sf2d_free_texture(sf2d_texture *texture)
{
	if (texture) {
		if (texture->place == SF2D_PLACE_RAM) {
			linearFree(texture->data);
		} else if (texture->place == SF2D_PLACE_VRAM) {
			vramFree(texture->data);
		}
		free(texture);
	}
}

void sf2d_draw_texture(const sf2d_texture *texture, int x, int y)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_malloc(4 * sizeof(sf2d_vertex_pos_tex));

	int w = texture->width;
	int h = texture->height;

	vertices[0].position = (sf2d_vector_3f){(float)x,   (float)y,   0.5f};
	vertices[1].position = (sf2d_vector_3f){(float)x+w, (float)y,   0.5f};
	vertices[2].position = (sf2d_vector_3f){(float)x,   (float)y+h, 0.5f};
	vertices[3].position = (sf2d_vector_3f){(float)x+w, (float)y+h, 0.5f};

	vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
	vertices[1].texcoord = (sf2d_vector_2f){1.0f, 0.0f};
	vertices[2].texcoord = (sf2d_vector_2f){0.0f, 1.0f};
	vertices[3].texcoord = (sf2d_vector_2f){1.0f, 1.0f};

	GPU_SetTextureEnable(GPU_TEXUNIT0);

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_MODULATE, GPU_MODULATE,
		0xFFFFFFFF
	);

	GPU_SetTexture(
		GPU_TEXUNIT0,
		(u32 *)osConvertVirtToPhys((u32)texture->data),
		texture->width,
		texture->height,
		GPU_TEXTURE_MAG_FILTER(GPU_NEAREST) | GPU_TEXTURE_MIN_FILTER(GPU_NEAREST),
		texture->pixel_format
	);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys((u32)vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 4);
}

static const u8 tile_order[] = {
	0,1,8,9,2,3,10,11,16,17,24,25,18,19,26,27,4,5,
	12,13,6,7,14,15,20,21,28,29,22,23,30,31,32,33,
	40,41,34,35,42,43,48,49,56,57,50,51,58,59,36,
	37,44,45,38,39,46,47,52,53,60,61,54,55,62,63
};

//Stolen from smealum's portal3DS
void texture_tile32(const u32 *src, u32 *dst, int width, int height)
{
	if (!src || !dst) return;

	int i, j, k, l = 0;
	for (j = 0; j < height; j+=8) {
		for (i = 0; i < width; i+=8) {
			for (k = 0; k < 8*8; k++) {
				int x = i + tile_order[k]%8;
				int y = j + (tile_order[k] - (x-i))/8;
				u32 v = src[x + (height-1-y)*width];
				dst[l++] = __builtin_bswap32(v);
			}
		}
	}
}
