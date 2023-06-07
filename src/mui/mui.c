#include "mui/mui.h"
#include "core/window.h"
#include "core/io/assetm.h"
#include "core/types/shader.h"
#include "text/text_inc.h""

// #include <ctype.h>
// #include <direct.h>

glyph* g_full = NULL;

#define FONT_SIZE_STEP  1
int  font_size = 14;

font_t font_x = FONT_INIT();
font_t font_s = FONT_INIT();
font_t font_m = FONT_INIT();
font_t font_l = FONT_INIT();
#define FONT_X_SIZE_DIF  -4
#define FONT_S_SIZE_DIF  -2
#define FONT_M_SIZE_DIF   0
#define FONT_L_SIZE_DIF   2

font_t* font_main;

void mui_init()
{
  // char* _cwd = _getcwd(NULL, 0);
  // ASSERT(_cwd != NULL);
  // strcpy(cwd, _cwd);
  // P_STR(cwd);
  
  // ---- text init ---
  
  char* path = "assets/fonts/JetBrainsMonoNL-Regular.ttf";
  // char* path = "JetBrains Mono NL Regular Nerd Font Complete Mono Windows Compatible.ttf"; // dont work
  // char* path = "assets/fonts/Raleway-Bold.ttf"; // dont work
  // char* path = "assets/fonts/Roboto-Bold.ttf"; // dont work
  
  text_load_font(path, font_size + FONT_X_SIZE_DIF, &font_x);
  text_load_font(path, font_size + FONT_S_SIZE_DIF, &font_s);
  text_load_font(path, font_size + FONT_M_SIZE_DIF, &font_m);
  text_load_font(path, font_size + FONT_L_SIZE_DIF, &font_l);

  font_main     = &font_m;
    
  bool err = false;
  u32 text_shader = shader_create("_assets/shaders/text.vert", 
					                        "_assets/shaders/text.frag", 
					                        "text_shader", &err);
  ASSERT(!err);
  // u32 img_shader  = shader_create(ASSET_PATH"shaders/text.vert", 
	// 				                        ASSET_PATH"shaders/text.frag", 
	// 				                        "text_shader", &err);
  u32 blank_tex   = assetm_get_texture("blank.png", true)->handle;
    
  text_draw_init(font_main, 
                 text_shader, text_shader, 
                 blank_tex,
                 window_get_size,
                 shader_use_handle, shader_set_int_handle,
                 shader_set_vec2_handle, shader_set_vec3_handle);

  // ------------------

  g_full = text_get_glyph(U_FULL, font_main);

  // input_register_key_callback(app_key_callback);
  // input_register_utf8_callback(app_utf8_callback);

  int w, h;
  window_get_size(&w, &h);
  P_INT(w);
  P_INT(h);
  float dpi_x, dpi_y;
  window_get_monitor_dpi(&dpi_x, &dpi_y);
  P_F32(dpi_x);
  P_F32(dpi_y);
}



