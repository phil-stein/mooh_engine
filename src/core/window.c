#include "core/window.h"
#include "core/core_data.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"
#include "stb/stb_ds.h"

// -- vars --
static core_data_t* core_data;
int monitor_w = 0;
int monitor_h = 0;
char window_title[WINDOW_TITLE_MAX];
bool is_maximized = true;
window_type win_type;

// framebuffer ptr array to be resized on window size change
framebuffer_t** resize_buffers = NULL;
int			        resize_buffers_len = 0;

// -- func decls --
void error_callback(int error, const char* description);
void resize_callback(void* window, int width, int height);
void maximize_callback(void* window, int maximized);


// intis glfw & glad, also creates the window
// returns: <stddef.h> return_code
bool window_create(const int width, const int height, const char* title, window_type type)
{
  strcpy(window_title, title);
  
  core_data = core_data_get();

	// enable error logging for glfw
	glfwSetErrorCallback(error_callback);
	
  // Initialise GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		fprintf(stderr, "Failed to initialize GLFW !!!\n");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // @TODO: implement this, page 439 in learnopengl

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  core_data->monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(core_data->monitor);
  monitor_w = mode->width;
  monitor_h = mode->height;

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  // open a window and create its opengl context
	if (type == WINDOW_FULL)
  {
      core_data->window = glfwCreateWindow(mode->width, mode->height, title, core_data->monitor, NULL);
  }
  else
  {
      core_data->window = glfwCreateWindow(width, height, title, NULL, NULL);
  }
  win_type = type;

	if (core_data->window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return false;
	}

	// make the window's context current
	glfwMakeContextCurrent(core_data->window);

	// initialize glad, load the opengl bindings
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		return false;
	}

	// tell opengl the size of our window
	glViewport(0, 0, width, height);

	// maximize window
	if (type == WINDOW_MAX)
	{
		glfwMaximizeWindow(core_data->window);
	}

	// set the resize callback
	glfwSetFramebufferSizeCallback(core_data->window, (GLFWframebuffersizefun)resize_callback);
  // @NOTE: causes inability to restore maximized after fullscreen, also framebuffers crash when minimizing to system tray
  // glfwSetWindowMaximizeCallback(core_data->window,  (GLFWwindowmaximizefun)maximize_callback); 

	glfwSetWindowAttrib(core_data->window, GLFW_FOCUS_ON_SHOW, true);
	// glfwSetWindowAttrib(window, GLFW_AUTO_ICONIFY, true);
	glfwRequestWindowAttention(core_data->window);

	return true; // all good :)
}

// glfw error func
void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW-Error: %s\n", description);
}

// window resize callback
// resizes the "glViewport" according to the resized window
// window is type GLFWwindow*
void resize_callback(void* window, int width, int height)
{
	glViewport(0, 0, width, height);
	for (int i = 0; i < resize_buffers_len; ++i)
	{
		if (resize_buffers[i] == NULL) { continue; }
    framebuffer_resize_to_window(resize_buffers[i]);
	}
}

void maximize_callback(void* window, int maximized)
{
  win_type = maximized ? WINDOW_MAX : WINDOW_MIN; // : win_type;
}

window_type window_get_mode()
{
  return win_type;
}
void window_get_size(int* w, int* h)
{
	glfwGetWindowSize(core_data->window, w, h);
}
void window_get_monitor_size_cm(float* w, float* h)
{
  int w_mm, h_mm; // millimeter
  glfwGetMonitorPhysicalSize(core_data->monitor, &w_mm, &h_mm);
  *w = (float)w_mm * 0.1f;
  *h = (float)h_mm * 0.1f;
}
void window_get_monitor_dpi(float* w, float* h)
{
  window_get_monitor_size_cm(w, h);
  *w *= 0.39370079; // cm to inch
  *h *= 0.39370079; // cm to inch
  // int w_px, h_px;
	// glfwGetWindowSize(window, &w_px, &h_px);
  *w = monitor_w / *w; // pixels per inch
  *h = monitor_h / *h; // pixels per inch
}
char* window_get_title() 
{
	return window_title;
}

bool window_get_should_close()
{
	return (bool)glfwWindowShouldClose(core_data->window);
}

void window_close()
{
	glfwSetWindowShouldClose(core_data->window, GLFW_TRUE);
}
void window_set_mode(window_type type)
{
  if (type == WINDOW_MIN)
  {
    glfwRestoreWindow(core_data->window);
  }
  else if (type == WINDOW_MAX)
  { 
		glfwMaximizeWindow(core_data->window);
  }
  else if (type == WINDOW_FULL)
  {
    const GLFWvidmode* mode = glfwGetVideoMode(core_data->monitor);
    glfwSetWindowMonitor(core_data->window, core_data->monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
  }
  win_type = type;
}
void window_set_title(const char* title)
{
	glfwSetWindowTitle(core_data->window, title);
	// window_title = (char*)title;
  strcpy(window_title, title);
}


// framebuffer size update --------------------
void window_set_texturebuffer_to_update_to_screen_size(framebuffer_t* fb)
{
	arrput(resize_buffers, fb);
	resize_buffers_len++;
}

