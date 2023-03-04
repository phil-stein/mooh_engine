#include "core/types/texture.h"
#include "math/math_inc.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define STB_IMAGE_IMPLEMENTATION  // only define once
#include "stb/stb_image.h"
#pragma GCC diagnostic pop

#include "GLAD/glad.h"

void texture_free(texture_t* t)
{
	glDeleteTextures(1, &t->handle);
}
void texture_free_handle(u32 handle)
{
	glDeleteTextures(1, &handle);
}

void texture_load_pixels(const char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bool flip_vertical) 
{
    int width, height;

    // OpenGL has texture coordinates with (0, 0) on bottom
    stbi_set_flip_vertically_on_load(flip_vertical);
    u8* image = stbi_load(path, &width, &height, channel_num, STBI_rgb_alpha);
    assert(image != NULL);

    *pixels_out = malloc((double)(width * height * 4));
    assert(*pixels_out != NULL);
    memcpy(*pixels_out, image, (double)(width * height * 4));
    assert(*pixels_out != NULL);
    *width_out = width;
    *height_out = height;

    stbi_image_free(image);
}

u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num, bool srgb)
{
  u32 handle;

  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  // no interpolation
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0);

  assert(channel_num >= 1);
  assert(channel_num != 2);
  assert(channel_num <= 4);
  int gl_internal_format = 0;
  int gl_format          = 0;
  switch (channel_num)
  {
    case 1:
      gl_internal_format = srgb ? GL_SRGB8 : GL_R8;
      gl_format = GL_RED;
      break;
    case 3:
      gl_internal_format = srgb ? GL_SRGB : GL_RGB;
      gl_format = GL_RGB;
      break;
    case 4:
      gl_internal_format = srgb ? GL_SRGB_ALPHA : GL_RGBA;
      gl_format = GL_RGBA;
      break;
  }
  assert(gl_format != 0);

  glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, pixels);
  
  // must be called after glTexImage2D
  glGenerateMipmap(GL_TEXTURE_2D);
  // not sure this does anything ???
  // float max = 4.0f;
  // glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
  // float amount = MIN(4.0f, max);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
 
  return handle;
}

texture_t texture_create_from_path(const char* file_path, bool flip_vertical)
{
    u8* pixels;
    size_t width, height;
    int channel_num = 0;
    texture_load_pixels(file_path, &pixels, &width, &height, &channel_num, flip_vertical);
    u32 handle = texture_create_from_pixels(pixels, width, height, channel_num, false);
    free(pixels);

    texture_t tex;
    tex.handle = handle;
    tex.width = width;
    tex.height = height;
    tex.path = (char*)file_path;

    printf("loaded texture from '%s', handle: '%d'\n", file_path, handle);

    return tex;
}

u32 texture_load_cubemap(char* right, char* left, char* bottom, char* top, char* front, char* back)
{
  u32 cube_map;
  glGenTextures(1, &cube_map);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);

  char* cube_map_paths[6];
  cube_map_paths[0] = right;
  cube_map_paths[1] = left;
  cube_map_paths[2] = bottom;
  cube_map_paths[3] = top;
  cube_map_paths[4] = front;
  cube_map_paths[5] = back;


  int width, height, channel_num = 0;
  for (u32 i = 0; i < 6; ++i)
  {
    stbi_set_flip_vertically_on_load(true);
    u8* data = stbi_load(cube_map_paths[i], &width, &height, &channel_num, 0);
    P_STR(cube_map_paths[i]);
    P_INT(channel_num);
    P_INT(width);
    P_INT(height);
    int gl_internal_format = 0;
    int gl_format          = 0;
    switch (channel_num)
    {
        case 1:
            gl_internal_format = GL_SRGB8; // GL_R8
            gl_format = GL_RED;
            break;
        case 3:
            gl_internal_format = GL_SRGB; // gl_RGB
            gl_format = GL_RGB;
            break;
        case 4:
            gl_internal_format = GL_SRGB_ALPHA; // GL_RGBA
            gl_format = GL_RGBA;
            break;
    }
    assert(gl_format != 0);
    if (data)
    {
      // we can increment the first param like this because the enum linearly increments
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_internal_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, data );
      stbi_image_free(data);
    }
    else
    {
      printf("Cubemap tex failed to load at path: %s\n", cube_map_paths[i]);
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return cube_map;
}


