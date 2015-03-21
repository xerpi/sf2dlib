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
	sf2d_vertex_pos_tex *vertices = sf2d_pool_alloc(4 * sizeof(sf2d_vertex_pos_tex));

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

	/*
	GPU_SetTexture(
		GPU_TEXUNIT unit,
		u32* data,
		u16 width,
		u16 height,
		u32 param,
		GPU_TEXCOLOR colorType
	);*/

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
		0xFF,
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_SetTextureEnable(GPU_TEXUNIT0);
	GPU_DrawArray(GPU_TRIANGLE_STRIP, 4);
}
