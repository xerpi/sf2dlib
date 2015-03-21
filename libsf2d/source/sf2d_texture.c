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
