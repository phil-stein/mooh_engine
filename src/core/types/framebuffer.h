#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "global/global.h"
#include "math/math_inc.h" 
#include "GLAD/glad.h"

typedef enum framebuffer_type
{
	FRAMEBUFFER_RGB				       = GL_RGB,
	FRAMEBUFFER_RGB16F			     = GL_RGBA16F, // check if GL_RGB16F works as well
	FRAMEBUFFER_SINGLE_CHANNEL	 = GL_RED,
	FRAMEBUFFER_SINGLE_CHANNEL_F = GL_R16F,
	FRAMEBUFFER_DEPTH			       = GL_DEPTH_COMPONENT,
  FRAMEBUFFER_DEFERRED         = 0x9999,
	
}framebuffer_type;
typedef struct framebuffer_t
{
	u32 buffer;
	u32 buffer02;
	u32 buffer03;
	u32 buffer04;
	u32 fbo;
	u32 rbo;

	framebuffer_type type;

	// use either not both
	int size_divisor;
	int width, height;

	bool is_msaa;
	int  samples;

}framebuffer_t;


// create a basic RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
void framebuffer_create_rgb(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a hdr RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
// RGB16F: 16bit float per red, green and blue channel
void framebuffer_create_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a multisampled RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
// multisampled: does anti-aliasing
void framebuffer_create_multisampled(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);
// create a hdr multisampled RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
// multisampled: does anti-aliasing
// RGB16F: 16bit float per red, green and blue channel
void framebuffer_create_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);
// create a purely depth buffer framebuffer, f.e. used for shadow-mapping
void framebuffer_create_shadowmap(u32* tex_buffer, u32* fbo, int width, int height);
// create a R16F framebuffer, only has a red channel and uses 16bit float instead 8bit unsigned integer
void framebuffer_create_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a RED framebuffer, only has a red channel and uses 8bit unsigned integer
void framebuffer_create_single_channel(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);

// create a framebuffer with 3 texture buffers for position normal and color
void framebuffer_create_gbuffer(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a musltisampled framebuffer with 3 texture buffers for position normal and color
void framebuffer_create_gbuffer_multisampled(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);

// creates a framebuffer as specified in "fb"
// when "width" & "height" are 0, they will be set automatically
// when "size_divisor" is 0 it will be set to 1, doing nothing
// when "samples" is 0 or less it will be set to 4
// !!! setting "width" & "height" only affects "FRAMEBUFFER_DEPTH" 
// !!! only "FRAMEBUFFER_RGB" and "FRAMEBUFFER_RGB16F" can be set to use multisampling via"is_msaa"
bool framebuffer_create(framebuffer_t* fb);

// set framebuffer to be the one used for future actions
void framebuffer_bind(framebuffer_t* fb);
// set framebuffer to be the one used for future actions
void framebuffer_bind_fbo(u32 fbo);
// deactivate framebuffer
void framebuffer_unbind();
// delete framebuffer and attachments
void framebuffer_delete(framebuffer_t* fb);

// get the value at pixel[x, y] for a FRAMEBUFFER_RGBAF framebuffer
void framebuffer_get_rgbaf_value(framebuffer_t* fb, u32 buffer, int x, int y, vec4 out);

// changes the framebuffers size to the one of the window
void framebuffer_resize_to_window(framebuffer_t* fb);
// write a multisampled framebuffer(fb_msaa) into a regular buffer(fb)
// you need to do this and use the regular buffer (fb) for displaying to screen
void framebuffer_blit_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb);
// write a multisampled framebuffer(fbo_msaa) into a regular buffer(fbo)
// you need to do this and use the regular buffer (fbo) for displaying to screen
void framebuffer_blit_multisampled_fbo(u32 fbo_msaa, u32 fbo);
// transfers pixel data from all buffer attachments from 'fb_msaa' to 'fb'
// you need to do this and use the regular buffer (fbo) for displaying to screen
void framebuffer_blit_gbuffer_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb);

#endif
