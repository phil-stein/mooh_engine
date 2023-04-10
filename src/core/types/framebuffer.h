#ifndef CORE_FRAMEBUFFER_H
#define CORE_FRAMEBUFFER_H

#include "global/global.h"
#include "math/math_inc.h" 
#include "GLAD/glad.h"

typedef enum framebuffer_type
{
	FRAMEBUFFER_RGB				       = GL_RGB,
	FRAMEBUFFER_RGB16F			     = GL_RGBA16F, // @TODO: check if GL_RGB16F works as well
	FRAMEBUFFER_SINGLE_CHANNEL	 = GL_RED,
	FRAMEBUFFER_SINGLE_CHANNEL_F = GL_R16F,
	FRAMEBUFFER_DEPTH			       = GL_DEPTH_COMPONENT,  // @UNSURE: not used i think, shadowmap ?
  FRAMEBUFFER_DEFERRED         = 0x9999,
	
}framebuffer_type;
// @DOC: get the channel amoount in framebuffer_t.buffer, -1 is unknown / error
#define FRAMEBUFFER_TYPE_TO_CHANNEL_NR(type)  ( (type) == FRAMEBUFFER_RGB              ?  3 :   \
                                                (type) == FRAMEBUFFER_RGB16F           ?  4 :   \
                                                (type) == FRAMEBUFFER_SINGLE_CHANNEL   ?  1 :   \
                                                (type) == FRAMEBUFFER_SINGLE_CHANNEL_F ?  1 :   \
                                                (type) == FRAMEBUFFER_DEPTH            ? -1 :   \
                                                (type) == FRAMEBUFFER_DEFERRED         ?  4 :   \
                                                -1 )

// @DOC: get the GL_TYPE in framebuffer_t.buffer, -1 is unknown / error
#define FRAMEBUFFER_TYPE_TO_GL_DATA_TYPE(type)  ( (type) == FRAMEBUFFER_RGB              ?  GL_UNSIGNED_BYTE :   \
                                                  (type) == FRAMEBUFFER_RGB16F           ?  GL_UNSIGNED_BYTE :   \
                                                  (type) == FRAMEBUFFER_SINGLE_CHANNEL   ?  GL_UNSIGNED_BYTE :   \
                                                  (type) == FRAMEBUFFER_SINGLE_CHANNEL_F ?  GL_FLOAT :           \
                                                  (type) == FRAMEBUFFER_DEPTH            ? -1 :                  \
                                                  (type) == FRAMEBUFFER_DEFERRED         ?  GL_UNSIGNED_BYTE :   \
                                                  -1 )
// @DOC: get the vairable type in framebuffer_t.buffer, i.e. u8, f32, -1 is unknown / error
#define FRAMEBUFFER_TYPE_TO_DATA_TYPE(type)  ( (type) == FRAMEBUFFER_RGB              ? u8  :   \
                                               (type) == FRAMEBUFFER_RGB16F           ? u8  :   \
                                               (type) == FRAMEBUFFER_SINGLE_CHANNEL   ? u8  :   \
                                               (type) == FRAMEBUFFER_SINGLE_CHANNEL_F ? f32 :   \
                                               (type) == FRAMEBUFFER_DEPTH            ? -1  :   \
                                               (type) == FRAMEBUFFER_DEFERRED         ? u8  :   \
                                               -1 )


// @DOC: types are already opengl types just deferred isnt
#define FRAMEBUFFER_TYPE_TO_GL_TYPE(type)  ( (type) == FRAMEBUFFER_DEFERRED ?  GL_RGBA : (type))

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


// @DOC: creates a framebuffer as specified in "fb"
//       ! use this to make framebuffers not any of the other functions, usually
//       when "width" & "height" are 0, they will be set automatically
//       when "size_divisor" is 0 it will be set to 1, doing nothing
//       when "samples" is 0 or less it will be set to 4
//       !!! setting "width" & "height" only affects "FRAMEBUFFER_DEPTH" 
//       !!! only "FRAMEBUFFER_RGB" and "FRAMEBUFFER_RGB16F" can be set to use multisampling via"is_msaa"
//       returns succes
bool framebuffer_create(framebuffer_t* fb);

// @DOC: set framebuffer to be the one used for future actions
void framebuffer_bind(framebuffer_t* fb);
// @DOC: set framebuffer to be the one used for future actions
void framebuffer_bind_fbo(u32 fbo);
// @DOC: deactivate framebuffer
void framebuffer_unbind();
// @DOC: delete framebuffer and attachments
void framebuffer_delete(framebuffer_t* fb);

// @DOC: get the value at pixel[x, y] for a FRAMEBUFFER_RGBAF framebuffer
void framebuffer_get_rgbaf_value(framebuffer_t* fb, u32 buffer, int x, int y, vec4 out);
  // @TODO: get to work
//@DOC: get the pixels out of a buffer in a framebuffer_t.buffer
//      ! returned buffer needs to be freed
//      ! doesnt work for msaa framebuffers
u8* frambuffer_write_pixels_to_buffer(framebuffer_t* fb, u32* buffer_len);
  // @TODO: get to work
// @DOC: get the pixels out of a buffer in a framebuffer, direct without framebuffer_t
//      ! returned buffer needs to be freed
//      ! doesnt work for msaa framebuffers
//      fbo:          framebuffer object registered with opengl
//      width:        width of framebuffer
//      height:       height of framebuffer
//      channel_nr:   r: 1, rgb: 3, rgba: 4
//      gl_type:      GL_RED, GL_RGB, GL_RGBA, etc.
//      gl_data_type: GL_UNSIGNED_BYTE, GL_FLOAT
//      buffer_len:   gets set to the length of returned buffer
u8* frambuffer_write_pixels_to_buffer_fbo(u32 fbo, u32 width, u32 height, u32 channel_nr, u32 gl_type, u32 gl_data_type, u32* buffer_len);

// @DOC: changes the framebuffers size to the one of the window
void framebuffer_resize_to_window(framebuffer_t* fb);
// @DOC: write a multisampled framebuffer(fb_msaa) into a regular buffer(fb)
//       you need to do this and use the regular buffer (fb) for displaying to screen
void framebuffer_blit_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb);
// @DOC: write a multisampled framebuffer(fbo_msaa) into a regular buffer(fbo)
//       you need to do this and use the regular buffer (fbo) for displaying to screen
void framebuffer_blit_multisampled_fbo(u32 fbo_msaa, u32 fbo);
// @DOC: transfers pixel data from all buffer attachments from 'fb_msaa' to 'fb'
//       you need to do this and use the regular buffer (fbo) for displaying to screen
void framebuffer_blit_gbuffer_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb);


// internal functions ---------------------------------------------------------------------------------------


// @DOC: create a basic RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
//       ! used internally
//       tex_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
void framebuffer_create_rgb(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// @DOC: create a hdr RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
//       RGB16F: 16bit float per red, green and blue channel
//       ! used internally
//       tex_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
void framebuffer_create_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// @DOC: create a multisampled RGB framebuffer with depth(24bits) and stencil(8bits) buffer attached
//       multisampled: does anti-aliasing
//       ! used internally
//       tex_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
//       samples:      amount of anti-aliasing samples
void framebuffer_create_multisampled(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);
// @DOC: create a hdr multisampled RGB16F framebuffer with depth(24bits) and stencil(8bits) buffer attached
//       multisampled: does anti-aliasing
//       RGB16F: 16bit float per red, green and blue channel
//       ! used internally
//       tex_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
//       samples:      amount of anti-aliasing samples
void framebuffer_create_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);
// @DOC: create a purely depth buffer framebuffer, f.e. used for shadow-mapping
//       does not use screen size to set size
//       ! used internally
//       tex_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       width:        buffer width
//       height:       buffer height
void framebuffer_create_shadowmap(u32* tex_buffer, u32* fbo, int width, int height);
// @DOC: create a R16F framebuffer, only has a red channel and uses 16bit float instead 8bit unsigned integer
//       ! used internally
//       tex_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
void framebuffer_create_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// create a RED framebuffer, only has a red channel and uses 8bit unsigned integer
void framebuffer_create_single_channel(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);

// @DOC: create a framebuffer with 4 texture buffers for position, normal, material and color, used in deferred rendering
//       ! used internally
//       pos_buffer:   pointer to id for opengl texture buffer
//       norm_buffer:  pointer to id for opengl texture buffer
//       mat_buffer:   pointer to id for opengl texture buffer
//       col_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
void framebuffer_create_gbuffer(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height);
// @DOC: create a musltisampled framebuffer with 4 texture buffers for position, normal, material and color, used in deferred rendering
//       multisampled: does anti-aliasing
//       ! used internally
//       pos_buffer:   pointer to id for opengl texture buffer
//       norm_buffer:  pointer to id for opengl texture buffer
//       mat_buffer:   pointer to id for opengl texture buffer
//       col_buffer:   pointer to id for opengl texture buffer
//       fbo:          pointer to id for opengl frame-buffer object
//       rbo:          pointer to id for opengl render-buffer object
//       size_divisor: divide the current screen resolution to use as res for framebuffer
//       width:        gets set to buffer width
//       height:       gets set to buffer height
//       samples:      amount of anti-aliasing samples
void framebuffer_create_gbuffer_multisampled(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples);

#endif
