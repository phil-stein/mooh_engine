#include "mui/mui.h"
#include "core/window.h"
#include "core/io/assetm.h"
#include "core/types/shader.h"
#include "core/core_data.h"
#include "text/text_inc.h"

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

static core_data_t* core_data = NULL;

void mui_init()
{
  core_data = core_data_get();
  // char* _cwd = _getcwd(NULL, 0);
  // ASSERT(_cwd != NULL);
  // strcpy(cwd, _cwd);
  // P_STR(cwd);
  
  // ---- text init ---
 
  char* path_relative = "fonts/JetBrainsMonoNL-Regular.ttf";
  // char* path = "JetBrains Mono NL Regular Nerd Font Complete Mono Windows Compatible.ttf"; // dont work
  // char* path = "fonts/Raleway-Bold.ttf"; // dont work
  // char* path = "fonts/Roboto-Bold.ttf"; // dont work
  char path_0[256];
  SPRINTF(256, path_0, "%s%s", core_data->asset_path, path_relative);
  P_STR(path_0);

  text_load_font(path_0, font_size + FONT_X_SIZE_DIF, &font_x);
  text_load_font(path_0, font_size + FONT_S_SIZE_DIF, &font_s);
  text_load_font(path_0, font_size + FONT_M_SIZE_DIF, &font_m);
  text_load_font(path_0, font_size + FONT_L_SIZE_DIF, &font_l);

  font_main     = &font_m;
    
  char path_1[256];
  SPRINTF(256, path_0, "%s%s", core_data->asset_path, "shaders/text/text.vert");
  SPRINTF(256, path_1, "%s%s", core_data->asset_path, "shaders/text/text.frag");
  P_STR(path_0);
  P_STR(path_1);
  bool err = false;
  // u32 text_shader = shader_create(path_0, path_1, "text_shader", &err);
  shader_t _text_shader = shader_create_from_file(path_0, path_1, NULL, "text_shader");
  u32       text_shader = _text_shader.handle;
  // u32 text_shader = shader_create("_assets/shaders/text.vert", 
	// 				                        "_assets/shaders/text.frag", 
	// 				                        "text_shader", &err);
  ASSERT(!err);
  // u32 img_shader  = shader_create(ASSET_PATH"shaders/text.vert", 
	// 				                        ASSET_PATH"shaders/text.frag", 
	// 				                        "text_shader", &err);
  u32 blank_tex   = assetm_get_texture("#internal/blank.png", true)->handle;
    
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

void mui_update()
{
  int w, h;
  window_get_size(&w, &h);

  #define STATUS_MAX 512
  int status[STATUS_MAX] = { ':', ')' };
  int _status[STATUS_MAX];
  static int status_pos = 0;
  int _status_pos = status_pos;
  for (int i = 0; i < status_pos; ++i) { _status[i] = status[i]; }
  { 
    _status[_status_pos++] = ' '; _status[_status_pos++] = '[';
    _status[_status_pos++] = 'c'; _status[_status_pos++] = 'm';
    _status[_status_pos++] = 'd'; _status[_status_pos++] = ']';
  }
  vec2 status_pos_v = { (2*w) - font_main->gw * (_status_pos +4), -h * 2 + font_main->gh * 2 };
  status_pos_v[0] -= font_main->gw * 1;
  status_pos_v[1] -= font_main->gh * 0.55f;
  // text_draw_quad(status_pos, VEC2_XY(font_main->gw * (_status_pos +4), font_main->gh * 2), RGB_F(0.2f, 0.2f, 0.25f));
  status_pos_v[0] += font_main->gw * 1;
  status_pos_v[1] += font_main->gh * 0.55f;
  text_draw_line(status_pos_v, _status, _status_pos, font_main);
}


