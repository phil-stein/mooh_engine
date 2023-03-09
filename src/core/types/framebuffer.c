#include "core/types/framebuffer.h"
#include "core/window.h"

#include "GLAD/glad.h"



void framebuffer_create_rgb(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

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
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo); // &rbo

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("-!!!-> ERROR_CREATING_FRAMEBUFFER");
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);


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
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_multisampled(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	// generate texture
	glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, w, h, GL_TRUE); // 4 samples
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_multisampled_hdr(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	// generate texture
	glGenTextures(1, tex_buffer);
	// glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE); // 4 samples
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_shadowmap(u32* tex_buffer, u32* fbo, int width, int height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	framebuffer_bind_fbo(*fbo);

	// gen texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	f32 border_col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_col);

	framebuffer_bind_fbo(*fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *tex_buffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// unbind the framebuffer, opengl now renders to the default buffer again
  framebuffer_unbind();
}

void framebuffer_create_single_channel(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
	// create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL); // GL_UNSIGNED_BYTE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo); // &rbo

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_single_channel_f(u32* tex_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
  // create framebuffer object
	glGenFramebuffers(1, fbo);
	// set fbo to be the active framebuffer to be modified
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

	// generate texture
	glGenTextures(1, tex_buffer);
	glBindTexture(GL_TEXTURE_2D, *tex_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, NULL); // GL_UNSIGNED_BYTE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_buffer, 0);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// free memory
	// glDeleteFramebuffers(1, &fbo);
}

void framebuffer_create_gbuffer(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height)
{
  glGenFramebuffers(1, fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;


  // - color buffer
  glGenTextures(1, col_buffer);
  glBindTexture(GL_TEXTURE_2D, *col_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *col_buffer, 0);
 
  // - material buffer
  glGenTextures(1, mat_buffer);
  glBindTexture(GL_TEXTURE_2D, *mat_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *mat_buffer, 0);
 
  // - normal color buffer
  glGenTextures(1, norm_buffer);
  glBindTexture(GL_TEXTURE_2D, *norm_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *norm_buffer, 0);

  // - position color buffer
  glGenTextures(1, pos_buffer);
  glBindTexture(GL_TEXTURE_2D, *pos_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, *pos_buffer, 0);

  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(4, attachments);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "-!!!-> ERROR_CREATING_FRAMEBUFFER\n");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_create_gbuffer_multisampled(u32* pos_buffer, u32* norm_buffer, u32* mat_buffer, u32* col_buffer, u32* fbo, u32* rbo, f32 size_divisor, int* width, int* height, int samples)
{
  glGenFramebuffers(1, fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	int w, h; window_get_size(&w, &h);
	// scale the resolution 
	w /= size_divisor;
	h /= size_divisor;
	*width = w;
	*height = h;

  // - color buffer
	glGenTextures(1, col_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *col_buffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *col_buffer, 0);
 
  // - material buffer
	glGenTextures(1, mat_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *mat_buffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, *mat_buffer, 0);
 
  // - normal color buffer
  glGenTextures(1, norm_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *norm_buffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, *norm_buffer, 0);
 

  // - position color buffer
  glGenTextures(1, pos_buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *pos_buffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D_MULTISAMPLE, *pos_buffer, 0);
  
  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(4, attachments);

	// create render buffer object
	glGenRenderbuffers(1, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, *rbo);  // &rbo
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach render buffer object to the depth and stencil buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);  // &rbo

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ERR("creating deferred msaa framebuffer");
		assert(0);
	}

	// unbind the framebuffer, opengl now renders to the default buffer again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
}
void framebuffer_bind_fbo(u32 fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void framebuffer_unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_delete(framebuffer_t* fb)
{
  glDeleteTextures(1, &fb->buffer);
  if (fb->type == FRAMEBUFFER_DEFERRED)
  {
    glDeleteTextures(1, &fb->buffer02);
    glDeleteTextures(1, &fb->buffer03);
    glDeleteTextures(1, &fb->buffer04);
  }
  if (fb->type != FRAMEBUFFER_SINGLE_CHANNEL)
  {
    glDeleteRenderbuffers(1, &fb->rbo);
  }
  glDeleteFramebuffers(1, &fb->fbo);
}

void framebuffer_get_rgbaf_value(framebuffer_t* fb, u32 buffer, int x, int y, vec4 out)
{
  int w = 0;
  int h = 0;
  window_get_size(&w, &h);
  y = h - y; // invert as buffer is rendered upside down

  glGetError(); // clear any previous errors
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT0 + buffer);
  glReadPixels((int)x, (int)y, 1, 1, GL_RGBA, GL_FLOAT, out);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
 
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  framebuffer_unbind();
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
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void framebuffer_blit_multisampled_fbo(u32 fbo_msaa, u32 fbo)
{
	int w, h;
	window_get_size(&w, &h);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void framebuffer_blit_gbuffer_multisampled(framebuffer_t* fb_msaa, framebuffer_t* fb)
{
	int w, h;
	// window_get_size(&w, &h);
	w = fb->width;
	h = fb->height;
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	// glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT1);
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT2);
  glDrawBuffer(GL_COLOR_ATTACHMENT2);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
 
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_msaa->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT3);
  glDrawBuffer(GL_COLOR_ATTACHMENT3);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
   
  // reset state
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

}
