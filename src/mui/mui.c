#include "mui/mui.h"
#include "core/window.h"
#include "core/io/assetm.h"
#include "core/types/shader.h"
#include "core/core_data.h"
#include "core/renderer/renderer_direct.h"
#include "text/text_inc.h"

#include "stb/stb_ds.h"

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

typedef enum 
{ 
  MUI_OBJ_TEXT = 1, 
  MUI_OBJ_QUAD = 2, 
  MUI_OBJ_IMG  = 3

}mui_obj_type;

#define MUI_OBJ_TEXT_MAX 32
typedef struct
{
  mui_obj_type type;

  vec2 pos;
  int text[MUI_OBJ_TEXT_MAX];
  int text_len;
  text_orientation orientation;
  
  // vec2 pos;
  vec2 scl;
  rgbf color;
  texture_t* tex;

}mui_obj_t;

mui_obj_t* obj_arr = NULL;
u32        obj_arr_len = 0;

void mui_init()
{
  core_data = core_data_get();
  // char* _cwd = _getcwd(NULL, 0);
  // ASSERT(_cwd != NULL);
  // strcpy(cwd, _cwd);
  // P_STR(cwd);
  
  // ---- text init ---
 
  char path_0[ASSET_PATH_MAX + 64];
  SPRINTF(ASSET_PATH_MAX + 64, path_0, "%s%s", core_data->asset_path, "fonts/JetBrainsMonoNL-Regular.ttf");

  text_load_font(path_0, font_size + FONT_X_SIZE_DIF, &font_x);
  text_load_font(path_0, font_size + FONT_S_SIZE_DIF, &font_s);
  text_load_font(path_0, font_size + FONT_M_SIZE_DIF, &font_m);
  text_load_font(path_0, font_size + FONT_L_SIZE_DIF, &font_l);

  font_main     = &font_m;
    
  char path_1[ASSET_PATH_MAX + 64];
  SPRINTF(ASSET_PATH_MAX + 64, path_0, "%s%s", core_data->asset_path, "shaders/text/text.vert");
  SPRINTF(ASSET_PATH_MAX + 64, path_1, "%s%s", core_data->asset_path, "shaders/text/text.frag");
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

  // int w, h;
  // window_get_size(&w, &h);
  // P_INT(w);
  // P_INT(h);
  // float dpi_x, dpi_y;
  // window_get_monitor_dpi(&dpi_x, &dpi_y);
  // P_F32(dpi_x);
  // P_F32(dpi_y);
}

void mui_update()
{
  P_U32(obj_arr_len);
  for (u32 i = 0; i < obj_arr_len; ++i)
  {
    mui_obj_t* o = &obj_arr[i];
    
    if (o->type == MUI_OBJ_QUAD)
    { renderer_direct_draw_quad(VEC2(0), 10.0f, o->pos, o->scl, o->color); } 
    else if (o->type == MUI_OBJ_IMG)
    { renderer_direct_draw_quad_textured(VEC2(0), 10.0f, o->pos, o->scl, o->tex, o->color); }
    else if (o->type == MUI_OBJ_TEXT)
    { text_draw_line(o->pos, o->text, o->text_len, font_main); }
  }
  if (obj_arr_len > 0)
  {
    arrfree(obj_arr);
    obj_arr_len = 0;
  }
}

void mui_text(vec2 pos, char* text, text_orientation orientation)
{
  int len = strlen(text);
  ERR_CHECK(len < MUI_OBJ_TEXT_MAX, "text too long for buffer size");
 
  // P_TEXT_ORIENTATION(orientation);
  ERR_CHECK(!((HAS_FLAG(orientation, TEXT_UP)     && HAS_FLAG(orientation, TEXT_MIDDLE)) ||
              (HAS_FLAG(orientation, TEXT_UP)     && HAS_FLAG(orientation, TEXT_DOWN))   ||
              (HAS_FLAG(orientation, TEXT_MIDDLE) && HAS_FLAG(orientation, TEXT_DOWN))),
              "can only have one of TEXT_UP, TEXT_MIDDLE or TEXT_DOWN");
  ERR_CHECK(!((HAS_FLAG(orientation, TEXT_LEFT)   && HAS_FLAG(orientation, TEXT_CENTER)) ||
              (HAS_FLAG(orientation, TEXT_LEFT)   && HAS_FLAG(orientation, TEXT_RIGHT))  ||
              (HAS_FLAG(orientation, TEXT_CENTER) && HAS_FLAG(orientation, TEXT_RIGHT))),
              "can only have one of TEXT_LEFT, TEXT_CENTER or TEXT_RIGHT");

  mui_obj_t o;
  o.type        = MUI_OBJ_TEXT; 
  o.text_len    = len;
  o.orientation = orientation;
  
  // convert to int array
  for (u32 i = 0; i < len; ++i)
  { o.text[i] = (int)text[i]; }

  // adjust height and width
  int w, h;
  window_get_size(&w, &h);
  pos[1] *= -1.0f ;
  // vec2_mul_f(pos, 2, pos);
  // vec2_mul_f(pos, 1.0f, pos);
  vec2_add_f(pos, 1, pos);
  pos[0] *= w;
  pos[1] *= h;

  // flip y 
  pos[1] *= -1.0f ;
  pos[1] -= font_main->gh;
  // pos[1] += font_main->gh;
  
  // if (HAS_FLAG(orientation, TEXT_LEFT)) { }
  if (HAS_FLAG(orientation, TEXT_RIGHT))
  { pos[0] -= font_main->gw * len; }
  else if (HAS_FLAG(orientation, TEXT_CENTER)) 
  { pos[0] -= font_main->gw * len * 0.5f; }

  // if no flag
  if(!HAS_FLAG(orientation, TEXT_UP) && !HAS_FLAG(orientation, TEXT_MIDDLE) && 
     !HAS_FLAG(orientation, TEXT_DOWN))
  { orientation |= TEXT_UP; }

  // if (HAS_FLAG(orientation, TEXT_UP)) {}
  if (HAS_FLAG(orientation, TEXT_DOWN))
  { pos[1] -= font_main->gh; }
  else if (HAS_FLAG(orientation, TEXT_MIDDLE))
  { pos[1] -= font_main->gh * 0.5f; }

  vec2_copy(pos, o.pos);

  // text_draw_line(pos, text_buffer, len, font_main);
  arrput(obj_arr, o);
  obj_arr_len++;
}

int mui_img_complx(vec2 pos, vec2 scl, texture_t* tex, rgbf tint, bool scale_by_ratio)
{
  // renderer_direct_draw_quad_textured(VEC2(0), 10.0f, VEC2_XY(0, 0), VEC2(1), tex, RGB_F(0, 1, 1)); 
  mui_obj_t obj;
  obj.type = MUI_OBJ_IMG;
  vec2_copy(pos, obj.pos);
  vec2_copy(scl, obj.scl);
  vec3_copy(tint, obj.color);
  obj.tex = tex;

  // orinetation & scaling 
  int w, h;
  window_get_size(&w, &h);
  f32 r_wh = ((f32)w / h);
  
  obj.pos[0] *= -1.0f;
  obj.pos[0] *= r_wh * 4.0f;
  obj.pos[1] *= 4.0f;
  
  obj.scl[0] *= (scale_by_ratio ? r_wh : 1.0f);
  

  arrput(obj_arr, obj);
  obj_arr_len++;
  return obj_arr_len -1;
}
int mui_quad(vec2 pos, vec2 scl, rgbf color)
{ 
  // example: 
  // renderer_direct_draw_quad(VEC2(0), 10.0f, VEC2_XY(0, 0), VEC2(1), RGB_F(0, 1, 1)); 
  mui_obj_t obj;
  obj.type = MUI_OBJ_QUAD;
  vec2_copy(pos, obj.pos);
  vec2_copy(scl, obj.scl);
  vec3_copy(color, obj.color);
  
  arrput(obj_arr, obj);
  obj_arr_len++;
  return obj_arr_len -1;
}

// void mui_group(text_orientation orientation, int len, ...)
// {
//   va_list args;
//   va_start(args, len);
//   for (u32 i = 0; i < len; ++i)
//   {
//     int idx = va_arg(args, int); 
//   }
//   va_end(args);
// }

