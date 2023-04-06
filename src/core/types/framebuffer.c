#include "core/types/framebuffer.h"
#include "core/window.h"
#include "core/debug/debug_opengl.h"

#include "GLAD/glad.h"



void framebuffer_create_rgb(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	_glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; 
  window_get_size(&w, &h);
  if (*width <= 0 || *height <= 0)
  {
	  // scale the resolution 
	  w /= size_divisor;
	  h /= size_divisor;
	  *width = w;
	  *height = h;
  }
  else 
  {
    w = *width;
    h = *height;
  }

  // generate texture
	_glGenTextures(1, tex_buffer);
	_glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	
	_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo); // &rbo

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	_glBindFramebuffer(GL_FRAMEBUFFER, *fbo);


	int w, h; 
  window_get_size(&w, &h);
  if (*width || *height <= 0)
  {
	  // scale the resolution 
	  w /= size_divisor;
	  h /= size_divisor;
	  *width = w;
	  *height = h;
  }
  else 
  {
    w = *width;
    h = *height;
  }

	// generate texture
	_glGenTextures(1, tex_buffer);
	_glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_multisampled(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
	// create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	_glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	// generate texture
	_glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, w, h, GL_TRUE); // 4 samples
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	_glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
	// create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	_glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	// generate texture
	_glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE); // 4 samples
	_glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	_glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_shadowmap(u32* tex_buffer, u32* fbo, int width, int height)
{
	// create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	framebuffer_bind_fbo(*fbo);

	// gen texture
	_glGenTextures(1, tex_buffer);
	_glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	_glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	f32 border_col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_col);

	framebuffer_bind_fbo(*fbo);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *tex_buffer, 0);
	_glDrawBuffer(GL_NONE);
	_glReadBuffer(GL_NONE);

	// unbind the framebuffer, opengl now renders to the default buffer again
  framebuffer_unbind();
}

void framebuffer_create_single_channel(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	_glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	_glGenTextures(1, tex_buffer);
	_glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	_glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL); // GL_UNSIGNED_BYTE
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo); // &rbo

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
  // create framebuffer object
	_glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	_glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	_glGenTextures(1, tex_buffer);
	_glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	_glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, NULL); // GL_UNSIGNED_BYTE
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach it to currently bound framebuffer object
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_gbuffer(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
  _glGenFramebuffers(1, fbo);
  _glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;


  // - color buffer
  _glGenTextures(1, col_buffer);
  _glBindTexture(GL_TEXTURE_2D, *col_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *col_buffer, 0);
 
  // - material buffer
  _glGenTextures(1, mat_buffer);
  _glBindTexture(GL_TEXTURE_2D, *mat_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *mat_buffer, 0);
 
  // - normal color buffer
  _glGenTextures(1, norm_buffer);
  _glBindTexture(GL_TEXTURE_2D, *norm_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *norm_buffer, 0);

  // - position color buffer
  _glGenTextures(1, pos_buffer);
  _glBindTexture(GL_TEXTURE_2D, *pos_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, *pos_buffer, 0);

  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  _glDrawBuffers(4, attachments);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
    ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER\n");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_create_gbuffer_multisampled(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
  _glGenFramebuffers(1, fbo);
  _glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

  // - color buffer
	_glGenTextures(1, col_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *col_buffer);
	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *col_buffer, 0);
 
  // - material buffer
	_glGenTextures(1, mat_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *mat_buffer);
	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, *mat_buffer, 0);
 
  // - normal color buffer
  _glGenTextures(1, norm_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *norm_buffer);
	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, *norm_buffer, 0);
 

  // - position color buffer
  _glGenTextures(1, pos_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *pos_buffer);
	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// _glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D_MULTISAMPLE, *pos_buffer, 0);
  
  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  _glDrawBuffers(4, attachments);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	_glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
    P_INT(samples);
    P_INT(w);
    P_INT(h);
		ERR("creating deferred msaa framebuffer\n");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void framebuffer_create_gbuffer_multisampled_02(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
  _glGenFramebuffers(1, fbo);
  _glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

  // - color buffer
	_glGenTextures(1, col_buffer);
	_glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *col_buffer);
	_glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *col_buffer, 0);
 
  // - material buffer
  _glGenTextures(1, mat_buffer);
  _glBindTexture(GL_TEXTURE_2D, *mat_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *mat_buffer, 0);
 
  // - normal color buffer
  _glGenTextures(1, norm_buffer);
  _glBindTexture(GL_TEXTURE_2D, *norm_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *norm_buffer, 0);

  // - position color buffer
  _glGenTextures(1, pos_buffer);
  _glBindTexture(GL_TEXTURE_2D, *pos_buffer);
  _glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  _glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, *pos_buffer, 0);
  
  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  _glDrawBuffers(4, attachments);

	// create render buffer object
	_glGenRenderbuffers(1, rbo);
	_glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	_glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	_glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach render buffer object to the depth and stencil buffer
	_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

  FRAMEBUFFER_ERR_CHECK("creating deferred msaa framebuffer");

	// unbind the framebuffer, opengl now renders to the default buffer again
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool framebuffer_create(framebuffer_t* fb)
{
	fb->size_divisor = fb->size_divisor <= 0 ? 1 : fb->size_divisor;

	if (fb->is_msaa) // multisampled
	{
		if (fb->type == FRAMEBUFFER_RGB)
		{
			fb->samples = fb->samples <= 0 ? 4 : fb->samples;
			framebuffer_create_multisampled(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height, fb->samples);
			return true;
		}
		else if (fb->type == FRAMEBUFFER_RGB16F)
		{
			fb->samples = fb->samples <= 0 ? 4 : fb->samples;
			framebuffer_create_multisampled_hdr(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height, fb->samples);
			return true;
		}
    else if (fb->type == FRAMEBUFFER_DEFERRED)
    {
      P_INFO("gbuffer msaa");
      framebuffer_create_gbuffer_multisampled(&fb->buffer04, &fb->buffer03, &fb->buffer02, &fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height, fb->samples);
      return true;
    }
    else
		{
			printf("[ERROR] Tried to create a MSAA Framebuffer of illegal type.\n");
		}
	}
	else 
	{
		if (fb->type == FRAMEBUFFER_RGB)
		{
			framebuffer_create_rgb(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
			return true;
		}
		else if (fb->type == FRAMEBUFFER_RGB16F)
		{
			framebuffer_create_hdr(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
			return true;
		}
		else if (fb->type == FRAMEBUFFER_SINGLE_CHANNEL)
		{
			framebuffer_create_single_channel(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
      return true;
		}
		else if (fb->type == FRAMEBUFFER_SINGLE_CHANNEL_F)
		{
			framebuffer_create_single_channel_f(&fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
			return true;
		}
		else if (fb->type == FRAMEBUFFER_SINGLE_CHANNEL)
		{
			if (fb->width == 0 || fb->height == 0) 
			{ window_get_size(&fb->width, &fb->height); }
			framebuffer_create_shadowmap(&fb->buffer, &fb->fbo, fb->width, fb->height); //  &fb->rbo, 
			return true;
		}
    else if (fb->type == FRAMEBUFFER_DEFERRED)
    {
      framebuffer_create_gbuffer(&fb->buffer04, &fb->buffer03, &fb->buffer02, &fb->buffer, &fb->fbo, &fb->rbo, fb->size_divisor, &fb->width, &fb->height);
      return true;
    }
		else
		{
			printf("[ERROR] Tried to create a Framebuffer of unknown type.\n");
		}

	}
	return false;
}

void framebuffer_bind(framebuffer_t* fb)
{
	_glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
}
void framebuffer_bind_fbo(u32 fbo)
{
	_glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void framebuffer_unbind()
{
	_glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_delete(framebuffer_t* fb)
{
  _glDeleteTextures(1, &fb->buffer);
  if (fb->type == FRAMEBUFFER_DEFERRED)
  {
    _glDeleteTextures(1, &fb->buffer02);
    _glDeleteTextures(1, &fb->buffer03);
    _glDeleteTextures(1, &fb->buffer04);
  }
  if (fb->type != FRAMEBUFFER_SINGLE_CHANNEL)
  {
    _glDeleteRenderbuffers(1, &fb->rbo);
  }
  _glDeleteFramebuffers(1, &fb->fbo);
}

void framebuffer_get_rgbaf_value(framebuffer_t* fb, u32 buffer, int x, int y, vec4 out)
{
  int w = 0;
  int h = 0;
  window_get_size(&w, &h);
  y = h - y; // invert as buffer is rendered upside down

  _glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT0 + buffer);
  _glReadPixels((int)x, (int)y, 1, 1, GL_RGBA, GL_FLOAT, out);
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
 
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  framebuffer_unbind();
}

// @TODO: get to work
u8* frambuffer_write_pixels_to_buffer(framebuffer_t* fb, u32* buffer_len)
{
  ASSERT(!fb->is_msaa);
  u32 channel_nr = FRAMEBUFFER_TYPE_TO_CHANNEL_NR(fb->type);
  ASSERT(channel_nr == 1 || channel_nr == 2 || channel_nr == 3 || channel_nr == 4);
  *buffer_len = fb->width * fb->height * channel_nr;
  u8* buffer;
  MALLOC(buffer, *buffer_len);
  
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  _glReadPixels((int)0, (int)0, fb->width, fb->height, FRAMEBUFFER_TYPE_TO_GL_TYPE(fb->type), FRAMEBUFFER_TYPE_TO_GL_DATA_TYPE(fb->type), buffer);
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  framebuffer_unbind();

  return buffer;
}
// @TODO: get to work
u8* frambuffer_write_pixels_to_buffer_fbo(u32 fbo, u32 width, u32 height, u32 channel_nr, u32 gl_type, u32 gl_data_type, u32* buffer_len)
{
  ASSERT(width > 0 && height > 0);
  ASSERT(channel_nr == 1 || channel_nr == 2 || channel_nr == 3 || channel_nr == 4);
  *buffer_len = width * height * channel_nr;
  u8* buffer;
  MALLOC(buffer, *buffer_len);
  
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  _glReadPixels(0, 0, width, height, gl_type, gl_data_type, buffer);
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  framebuffer_unbind();

  return buffer;
}

void framebuffer_resize_to_window(framebuffer_t* fb)
{
	// glBindTexture(GL_TEXTURE_2D, fb->buffer);

	int w, h; window_get_size(&w, &h);
	if (fb->size_divisor > 1)
	{
		w /= fb->size_divisor;
		h /= fb->size_divisor;
	}
	fb->width  = w;
	fb->height = h;

  framebuffer_delete(fb);
  framebuffer_create(fb);
}

void framebuffer_blit_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb)
{
	int w, h;
	// window_get_size(&w, &h);
	w = fb->width;
	h = fb->height;
	_glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
	_glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void framebuffer_blit_multisampled_fbo(u32 fbo_msaa, u32 fbo)
{
	int w, h;
	window_get_size(&w, &h);
	_glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	_glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void framebuffer_blit_gbuffer_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb)
{
  framebuffer_blit_multisampled(fb_msaa, fb);

  return;
	int w, h;
	// window_get_size(&w, &h);
	w = fb->width;
	h = fb->height;
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  _glDrawBuffer(GL_COLOR_ATTACHMENT0);
	_glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	// glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	_glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT1);
  _glDrawBuffer(GL_COLOR_ATTACHMENT1);
  _glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	_glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT2);
  _glDrawBuffer(GL_COLOR_ATTACHMENT2);
	_glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
 
  _glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  _glReadBuffer(GL_COLOR_ATTACHMENT3);
  _glDrawBuffer(GL_COLOR_ATTACHMENT3);
	_glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
   
  // reset state
  _glReadBuffer(GL_COLOR_ATTACHMENT0);
  _glDrawBuffer(GL_COLOR_ATTACHMENT0);

}
