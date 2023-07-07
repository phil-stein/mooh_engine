#include "mui/mui.h"
#include "core/window.h"
#include "core/io/assetm.h"
#include "core/types/shader.h"
#include "core/core_data.h"
#include "core/renderer/renderer_direct.h"
#include "core/debug/debug_opengl.h"
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
  // @NOTE: no blendd-type specified because same as renderer.c
  _glEnable(GL_BLEND);
  _glDisable(GL_CULL_FACE);
  _glDisable(GL_DEPTH_TEST);
  
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

  _glEnable(GL_CULL_FACE);
  _glEnable(GL_DEPTH_TEST);
}

void mui_text(vec2 pos, char* text, mui_orientation_type orientation)
{
  int len = strlen(text);
  ERR_CHECK(len < MUI_OBJ_TEXT_MAX, "text too long for buffer size");
 
  // P_TEXT_ORIENTATION(orientation);
  ERR_CHECK(!((HAS_FLAG(orientation, MUI_UP)     && HAS_FLAG(orientation, MUI_MIDDLE)) ||
              (HAS_FLAG(orientation, MUI_UP)     && HAS_FLAG(orientation, MUI_DOWN))   ||
              (HAS_FLAG(orientation, MUI_MIDDLE) && HAS_FLAG(orientation, MUI_DOWN))),
              "can only have one of MUI_UP, MUI_MIDDLE or MUI_DOWN");
  ERR_CHECK(!((HAS_FLAG(orientation, MUI_LEFT)   && HAS_FLAG(orientation, MUI_CENTER)) ||
              (HAS_FLAG(orientation, MUI_LEFT)   && HAS_FLAG(orientation, MUI_RIGHT))  ||
              (HAS_FLAG(orientation, MUI_CENTER) && HAS_FLAG(orientation, MUI_RIGHT))),
              "can only have one of MUI_LEFT, MUI_CENTER or MUI_RIGHT");

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
  if (HAS_FLAG(orientation, MUI_RIGHT))
  { pos[0] -= font_main->gw * len; }
  else if (HAS_FLAG(orientation, MUI_CENTER)) 
  { pos[0] -= font_main->gw * len * 0.5f; }

  // if no flag
  if(!HAS_FLAG(orientation, MUI_UP) && !HAS_FLAG(orientation, MUI_MIDDLE) && 
     !HAS_FLAG(orientation, MUI_DOWN))
  { orientation |= MUI_UP; }

  // if (HAS_FLAG(orientation, TEXT_UP)) {}
  if (HAS_FLAG(orientation, MUI_DOWN))
  { pos[1] -= font_main->gh; }
  else if (HAS_FLAG(orientation, MUI_MIDDLE))
  { pos[1] -= font_main->gh * 0.5f; }

  vec2_copy(pos, o.pos);

  // text_draw_line(pos, text_buffer, len, font_main);
  arrput(obj_arr, o);
  obj_arr_len++;
}

void mui_img_complx(vec2 pos, vec2 scl, texture_t* tex, rgbf tint, bool scale_by_ratio)
{
  mui_obj_t obj = MUI_OBJ_T_INIT_IMG(pos[0], pos[1], scl[0], scl[1], tex, tint[0], tint[1], tint[2]);
  // obj.type = MUI_OBJ_IMG;
  // vec2_copy(pos, obj.pos);
  // vec2_copy(scl, obj.scl);
  // vec3_copy(tint, obj.color);
  // obj.tex = tex;
  
  mui_img_obj(&obj, scale_by_ratio);
}

void mui_img_obj(mui_obj_t* obj, bool scale_by_ratio)
{
  // renderer_direct_draw_quad_textured(VEC2(0), 10.0f, VEC2_XY(0, 0), VEC2(1), tex, RGB_F(0, 1, 1)); 
 
  if (!obj->active) { return; }

  // orinetation & scaling 
  int w, h;
  window_get_size(&w, &h);
  f32 r_wh = ((f32)w / h);
  
  obj->pos[0] *= -1.0f;
  obj->pos[0] *= r_wh * 4.0f;
  obj->pos[1] *= 4.0f;
  
  // obj.scl[0] *= (scale_by_ratio ? r_wh : 1.0f);
  if (scale_by_ratio)
  { obj->scl[0] *= r_wh; }
  else
  { obj->scl[0] = obj->scl[1]; }
  
  
  r_wh = ((f32)obj->tex->width / obj->tex->height);
  obj->scl[0] *= r_wh;

  // flip, otherwise upside down 
  vec2_negate(obj->scl, obj->scl); 

  arrput(obj_arr, *obj);
  obj_arr_len++;
}
int mui_quad(vec2 pos, vec2 scl, rgbf color)
{ 
  mui_obj_t obj;
  obj.type = MUI_OBJ_QUAD;
  vec2_copy(pos, obj.pos);
  vec2_copy(scl, obj.scl);
  vec3_copy(color, obj.color);
 
  // orinetation & scaling 
  // @TODO: 
  bool scale_by_ratio = false; 
  int w, h;
  window_get_size(&w, &h);
  f32 r_wh = ((f32)w / h);
  
  obj.pos[0] *= -1.0f;
  obj.pos[0] *= r_wh * 4.0f;
  obj.pos[1] *= 4.0f;
  
  obj.scl[0] *= (scale_by_ratio ? r_wh : 1.0f);
  
  // r_wh = ((f32)tex->width / tex->height);
  // obj.scl[0] *= r_wh; 
  
  arrput(obj_arr, obj);
  obj_arr_len++;
  return obj_arr_len -1;
}

void mui_group(mui_group_t* g)
{
  ERR_CHECK(g->objs_len > 0, "mui_group_t has no objects in it.\n");
  ERR_CHECK(HAS_FLAG(g->orientation, MUI_DYNAMIC) || HAS_FLAG(g->orientation, MUI_STATIC),
            "mui_group_t must have either MUI_DYNAMIC or MUI_STATIC");
  ERR_CHECK(!((HAS_FLAG(g->orientation, MUI_LEFT)   && HAS_FLAG(g->orientation, MUI_CENTER)) ||
              (HAS_FLAG(g->orientation, MUI_LEFT)   && HAS_FLAG(g->orientation, MUI_RIGHT))  ||
              (HAS_FLAG(g->orientation, MUI_CENTER) && HAS_FLAG(g->orientation, MUI_RIGHT))),
              "can only have one of MUI_LEFT, MUI_CENTER or MUI_RIGHT");

  vec2 size;
  vec2 pos;
  vec2 pos_step;
  int  wraps = (g->objs_len / g->max_wrap) < 0 ? 1 : (g->objs_len / g->max_wrap);
  wraps += g->objs_len % g->max_wrap; // f.e. for 3 objs 2 max_wrap, would be 1 wrap
  int objs_len_wrap = g->objs_len + (g->objs_len % g->max_wrap);
  
  int w, h;
  window_get_size(&w, &h);
  // P_INT(objs_len_wrap);
  // P_INT(g->max_wrap);
  // P_INT(wraps);

  if (HAS_FLAG(g->orientation, MUI_ROW))
  {
    f32 length = ( (g->scl[0] / objs_len_wrap) * (f32)wraps);
    size[0] =  length - g->margin; 
    size[1] = (g->scl[1]  / (f32)wraps) - g->margin;
    pos_step[0] = length * 0.25f; 
    pos_step[1] = 0.0f;
    vec2_copy(g->pos, pos);
    
    pos[0] -= pos_step[0] * ((objs_len_wrap -1) * 0.5f);
    pos[1] += (wraps -1) * ( size[1] / (f32)wraps ) * 0.5f; 
  }
  else if (HAS_FLAG(g->orientation, MUI_COLUMN))
  {
    size[0] =  g->scl[0] - g->margin;
    size[1] = (g->scl[1] / g->objs_len) - g->margin;
    pos_step[0] = 0.0f;
    pos_step[1] = (g->scl[1] / g->objs_len) * 0.5f;
    vec2_copy(g->pos, pos);
    pos[1] -= pos_step[1] * ((g->objs_len -1) * 0.5f) ;   
  }

  // @UNSURE: if needed
  // // calc half width and move to the left
  // if (HAS_FLAG(g->orientation, MUI_STATIC) && HAS_FLAG(g->orientation, MUI_CENTER))
  // {
  //   // f32 width_avg = 0.0f;
  //   f32 width_total = 0.0f;
  //   for (u32 i = 0; i < g->objs_len; ++i)
  //   { width_total += g->objs[i].scl[0]; }
  //   // width_avg /= g->objs_len;
  //   // pos[0] -= (width_total * 0.5f) * 0.5f * 0.5f;
  // }

  for (u32 i = 0; i < g->objs_len; ++i)
  {
    // -- wrap --
    if (i != 0 && i % g->max_wrap == 0)
    {
      // P("wrap");
      pos[1] -= size[1] * 0.5f;
      pos[0] -= pos_step[0] * g->max_wrap;
    }
    
    // -- draw --
    mui_obj_t* o = &g->objs[i];
    if (o->type == MUI_OBJ_QUAD)
    {
      mui_quad(pos, size, o->color);
    }
    else if (o->type == MUI_OBJ_IMG)
    {
      // mui_img_complx(pos, size, o->tex, o->color,  g->scale_by_ratio);
      
      // vec2_copy(pos,  o->pos);
      // vec2_copy(size, o->scl);
      vec2_add(pos,  o->pos, o->pos);
      vec2_mul(size, o->scl, o->scl);

      mui_img_obj(o, g->scale_by_ratio);
    }
    else 
    { ERR("NOT IMPLEMENTED YET"); }
   
    // account for window width  
    if (HAS_FLAG(g->orientation, MUI_STATIC))
    { 
      f32 ratio_hw = (f32)h / (f32)w;
      pos_step[0] = -1 * o->scl[0] * ratio_hw * 0.5f; 
    }
    vec2_add(pos_step, pos, pos);
  } 
}

