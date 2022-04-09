#ifndef TEXTURE_H
#define TEXTURE_H

#include "global/global.h"


typedef struct texture_t
{
	u32 handle;
	int width;
	int height;
	char* path;
}texture_t;


void texture_load_pixels(const char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bool flip_vertical);

u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num, bool srgb);

texture_t texture_create_from_path(const char* file_path, bool flip_vertical);

u32 texture_load_cubemap(char* right, char* left, char* bottom, char* top, char* front, char* back);

#endif
