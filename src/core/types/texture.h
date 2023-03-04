#ifndef TEXTURE_H
#define TEXTURE_H

#include "global/global.h"


typedef struct texture_t
{
	u32 handle; // handle used for opengl calls
	int width;  // width of tex in pixels
	int height; // height of tex in pixels
  // @OPTIMIZATION: @UNSURE: remove this
	char* path; // path to texture file
}texture_t;

// @DOC: free the memory of a texture
//       t: texture to be freed
//       @UNSURE: texture_t.path
void texture_free(texture_t* t);
// @DOC: free the memory of a texture
//       handle: handle of texture 
//       @UNSURE: texture_t.path
void texture_free_handle(u32 handle);

// @DOC: load the pixels of an image file
//       path:          path to file
//       pixels_out:    array of pixels for output
//       width_out:     with of texture for output
//       height_out:    height of texture for output
//       channel_num:   single / rgb / rgba 
//       flip_vertical: flip texture vertically on load
void texture_load_pixels(const char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bool flip_vertical);

// @DOC: send pixel data to gpu / opengl
//       pixels:      to be send over
//       width:       width of texture in pixels
//       height:      height of texture in pixels
//       channel_num: single / rgb / rgba 
//       srgb:        whether to load the texture normal or srgb 
u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num, bool srgb);

// @DOC: load texture from given path
//       file_path:     path to texture
//       flip_vertical: flip texture vertically on load
texture_t texture_create_from_path(const char* file_path, bool flip_vertical);

// @DOC: load cubemap from six images
//       returns direct handle for cubemap 
//       right:  path to texture 
//       left:   path to texture 
//       bottom: path to texture 
//       top:    path to texture 
//       front:  path to texture 
//       back:   path to texture 
u32 texture_load_cubemap(char* right, char* left, char* bottom, char* top, char* front, char* back);

#endif
