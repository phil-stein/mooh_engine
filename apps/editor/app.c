#include "editor/app.h"
#include "editor/gui/gui.h"
#include "editor/gizmo.h"
#include "editor/terrain_edit.h"
#include "editor/stylesheet.h"
#include "editor/operation.h"
#include "editor/editor_save.h"
#include "core/program.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/renderer/renderer.h"
#include "core/renderer/renderer_extra.h"
#include "core/camera.h"
#include "core/io/file_io.h"
#include "core/io/assetm.h"
#include "core/io/save_sys.h"
#include "core/io/asset_io.h"
#include "core/state.h"
#include "core/event_sys.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "core/terrain.h"
#include "data/entity_template.h"
#include "phys/phys_world.h"
#include "mui/mui.h"   // @TMP:

#include "stb/stb_ds.h"


// bool app_data.wireframe_act = false;
// float app_data.mouse_sensitivity = 0.125f;
// int app_data.selected_id = -1; // -1 = not selected

app_data_t app_data = APP_DATA_INIT(); 


static core_data_t* core_data = NULL;

void move_cam_by_keys();
void rotate_cam_by_mouse();


int main(void)
{
  
  // @TODO: @UNSURE: these results are sus
  vec3 a = VEC3_INIT(1);
  vec3 b = VEC3_INIT(2);
  vec3 c; 
  vec3_lerp(a, b, 0.5f, c);
  P_VEC3(c);
  P_VEC3(VEC3_LERP(a, b, 0.5f));
  
  vec3_lerp_f(0, 1, 0.5f, c);
  P_VEC3(c);
  P_VEC3(VEC3_LERP_F(0, 1, 0.5f));
  
  P_LINE();
  
  program_start(1600, 900, "editor", WINDOW_MIN, app_init, app_update, ASSET_PATH);  // WINDOW_FULL
  
  return 0;
}

void app_init()
{
  core_data = core_data_get();
  
  // -- scene --
  const char scene_name[] =  "test.scene";
  // const char scene_name[] =  "empty.scene";
  // save_sys_write_scene_to_file(scene_name);
  TIMER_FUNC_STATIC(save_sys_load_scene_from_file(scene_name));
  // in game will be done by camera-controller
  // input_center_cursor_pos();
  camera_set_pos(VEC3_XYZ(0.0f,   6.0f,  10.0f));
  camera_set_front(VEC3_XYZ(0.0f,  -0.15f, -1.0f));
  
  event_sys_register_entity_removed(app_entity_removed_callback);

  TIMER_FUNC_STATIC(gui_init());
  editor_save_init();
  
  // -- terrain --
  TIMER_FUNC_STATIC(save_sys_load_terrain_from_file("test.terrain"));
  TIMER_FUNC_STATIC(terrain_create(25));



  // -- terrain --
  // vec2* info = malloc(sizeof(vec2) * TERRAIN_LAYOUT_VERT_INFO_LEN(core_data));
  // for (int i = 0; i < TERRAIN_LAYOUT_VERT_INFO_LEN(core_data); ++i)
  // { info[i][0] = 0.0f; info[i][1] = 0.0f; }
  // // { height[i] = (u8)(rand_f32() * 256.0f); }

  // const int size = 10;
  // for (int x = -(size/2); x < (size/2); ++x)
  // {
  //   for (int z = -(size/2); z < (size/2); ++z)
  //   {
  //     terrain_layout_t layout = { .pos = { x, z }, .vert_info = info  };
  //     arrput(core_data->terrain_layout, layout);
  //   }
  // }
  // terrain_layout_t layout = { .pos = { 0, 0 }, .vert_info = info  };
  // arrput(core_data->terrain_layout, layout);

  // // core_data->terrain_layout_len = arrlen(core_data->terrain_layout);
  // // core_data->terrain_layout_len = 0;
  // terrain_add_chunk(IVEC2(0));
  // P_INT(core_data->terrain_layout_len);
  // TIMER_FUNC_STATIC(terrain_create(25));
  // core_data->terrain_scl = 100;
  
}

void app_update()
{
  // - mui test -
  int w, h;
  window_get_size(&w, &h);
  
  // mui_text_l(VEC3_XY(0, 0), "(0, 0)");
  // mui_text_l(VEC2_XY(10, 10), "(10, 10)");
  // mui_text_l(VEC2_XY(100, 100), "(100, 100)");
  // mui_text_l(VEC2_XY(1000, 1000), "(1000, 1000)");
  // mui_text_l(VEC2_XY(w / 2, h / 2), "(x, x)");
  
  // mui_text(VEC2_XY(w -10, h -50), "(x, x)", TEXT_RIGHT | TEXT_UP);
  // {
  //   mui_text(VEC2_XY(w/2, h/2), "(right|up)",     TEXT_RIGHT | TEXT_UP);
  //   mui_text(VEC2_XY(w/2, h/2), "(right|middle)", TEXT_RIGHT | TEXT_MIDDLE);
  //   mui_text(VEC2_XY(w/2, h/2), "(right|down)",   TEXT_RIGHT | TEXT_DOWN);
  //   h += 150;
  //   mui_text(VEC2_XY(w/2, h/2), "(left|up)",     TEXT_LEFT | TEXT_UP);
  //   mui_text(VEC2_XY(w/2, h/2), "(left|middle)", TEXT_LEFT | TEXT_MIDDLE);
  //   mui_text(VEC2_XY(w/2, h/2), "(left|down)",   TEXT_LEFT | TEXT_DOWN);
  //   h += 150;
  //   mui_text(VEC2_XY(w/2, h/2), "(center|up)",     TEXT_CENTER | TEXT_UP);
  //   mui_text(VEC2_XY(w/2, h/2), "(center|middle)", TEXT_CENTER | TEXT_MIDDLE);
  //   mui_text(VEC2_XY(w/2, h/2), "(center|down)",   TEXT_CENTER | TEXT_DOWN);
  // }
  // text_draw_img( VEC2_XY(w/2, h/2), VEC2(10), u32 tex, rgbf tint);
  // mui_quad(VEC2_XY(0, 0),     VEC2(100), RGB_F(1, 1, 1));
  // mui_quad(VEC2_XY(100, 100), VEC2(100), RGB_F(1, 1, 0));
  mui_quad(VEC2_XY(w/2, h/2), VEC2(100), RGB_F(1, 0, 1));
  // mui_quad(VEC2_XY(w,   h),   VEC2(100), RGB_F(1, 0, 1));
  // mui_quad(VEC2_XY(0, 0),     VEC2(4),   RGB_F(1, 1, 1));
  // mui_quad(VEC2_XY(10, 10),   VEC2(4),   RGB_F(1, 1, 1));
  
  // {
  //   int w = 100; 
  //   int h = 100; 
  //   int ww, wh;
  //   vec2 pos, size;
  //   window_get_size(&ww, &wh);
  //   size[0] = w * 2;
  //   size[1] = h * 2;
  //   pos[0]  = ww - (size[0] / 2);
  //   pos[1]  = wh + (size[1] / 2);
  //   mui_quad(pos, size, RGB_F_RGB(0.75f));  
  // }

  // toggle wireframe, esc to quit, etc.
  programm_app_default_logic(core_data);

  // @TODO: this shows infront of gizmos
  //        also move to gizmo.c
  // draw lights
  if (!core_data_is_play())
  {
    int world_len = 0;
    int world_dead_len = 0;
    entity_t* world = state_entity_get_arr(&world_len, &world_dead_len);
    vec3 pos;
    for (int i = 0; i < world_len; ++i)
    {
      if (world[i].point_light_idx >= 0) 
      {
        bool error = false;
        point_light_t* p = state_point_light_get(world[i].point_light_idx, &error); ASSERT(!error);
        vec3_add(world[i].pos, p->offset, pos);
        debug_draw_mesh_register(pos, GIZMO_POINT_LIGHT_ROT, GIZMO_POINT_LIGHT_SCL, p->color, assetm_get_mesh_idx(GIZMO_POINT_LIGHT_MESH)); 
      }
    }
  }

  mat4 model, display_model;
  vec3 pos;
  GIZMO_MODEL_POS(&app_data, model, display_model, pos);
  TIMER_FUNC(renderer_extra_draw_scene_mouse_pick(display_model)); 
  TIMER_FUNC(gui_update());
  if(!core_data_is_play()) { TIMER_FUNC(gizmo_update()); }
  TIMER_FUNC(terrain_edit_update());

  // @NOTE: sync selected with outline
  core_data->outline_id = app_data.selected_id;

  // save map & terrain
  if (input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_S) && !core_data_is_play())
  { 
    save_sys_write_scene_to_file(SCENE_FILE_NAME); 
    save_sys_write_terrain_to_file(TERRAIN_FILE_NAME); 

    GUI_INFO_STR_SET(&app_data, "saved");
  }

  // undo operation
  if (input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_Z) && !core_data_is_play())
  { operation_reverse(); }
  
  // stop / pause pla-mode
  if (input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_SPACE))
  {
    if (core_data_is_play()) { core_data_pause(); }
    else                     { core_data_play();  }
  }
  
    // @TODO: flickers first frame
  static bool start = true;
  if (!app_data.mouse_over_ui && input_get_mouse_down(KEY_MOUSE_MOVE_START))
  {
    app_data.switch_gizmos_act = false;
    if (start)
    { 
      input_center_cursor_pos(); 
      start = false;
    }
    else
    {
      input_set_cursor_visible(false);
      rotate_cam_by_mouse(); 
      move_cam_by_keys();
    }
  }
  else
  { 
    app_data.switch_gizmos_act = true;
    start = true;
    input_set_cursor_visible(true); 
  }

  // snapping enabled when holding ctrl
  app_data.gizmo_snapping = (app_data.selected_id >= 0 && input_get_key_down(KEY_GIZMO_SNAPPING));
  
    // duplicate with 'ctrl + d'
  if (app_data.selected_id >= 0 && input_get_key_down(KEY_LEFT_CONTROL) && input_get_key_pressed(KEY_D))
  {
    int id = state_entity_duplicate_id(app_data.selected_id, VEC3_XYZ(2, 0, 0));
    app_data.selected_id = id;
  }

}

app_data_t* app_data_get()
{ return &app_data; }

void app_entity_removed_callback(int id)
{
  if (id == app_data.selected_id)
  {
    app_data.selected_id = -1;
  }
}

void move_cam_by_keys()
{
  // -- move the cam --
	float cam_speed = CAM_SPEED * core_data->delta_t;
	if (input_get_key_down(KEY_LEFT_SHIFT))
	{ cam_speed *= CAM_SPEED_SHIFT_MULT; }
	if (input_get_key_down(KEY_MOVE_FORWARD))
	{
		vec3 front; // camera_get_front(front);
		vec3_mul_f(core_data->cam.front, cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_MOVE_BACKWARD))
	{
		vec3 front; // camera_get_front(front);
		vec3_mul_f(core_data->cam.front, -cam_speed, front);
		camera_move(front);
	}
	if (input_get_key_down(KEY_MOVE_LEFT))
	{
		vec3 dist;
		vec3_cross(core_data->cam.front, core_data->cam.up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, -cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_MOVE_RIGHT))
	{
		vec3 dist;
		vec3_cross(core_data->cam.front, core_data->cam.up, dist);
		vec3_normalize(dist, dist);
		vec3_mul_f(dist, cam_speed, dist);
		camera_move(dist);
	}
	if (input_get_key_down(KEY_MOVE_DOWN))
	{
		vec3 up;	// camera_get_up(up);
		vec3_mul_f(core_data->cam.up, -cam_speed, up);
		camera_move(up);
	}
	if (input_get_key_down(KEY_MOVE_UP))
	{
		vec3 up; // camera_get_up(up);
		vec3_mul_f(core_data->cam.up, cam_speed, up);
		camera_move(up);
	}
}

// rotates the camera accoding to the mouse-movement
void rotate_cam_by_mouse()
{
	static bool init = false;
	static f32 pitch, yaw;

	f32 xoffset = input_get_mouse_delta_x();
	f32 yoffset = input_get_mouse_delta_y();

	xoffset *= app_data.mouse_sensitivity;
	yoffset *= app_data.mouse_sensitivity;

	
	yaw += xoffset;
	pitch += yoffset;

	// printf("pitch: %f, yaw: %f\n", pitch, yaw);

	if (pitch > 89.0f)
	{ pitch = 89.0f; }
	if (pitch < -89.0f)
	{ pitch = -89.0f; }

	if (!init)
	{
    vec3 front;
    // camera_get_front(front);
    vec3_copy(core_data->cam.front, front);
		pitch = front[1] * 90; // -30.375f;
		yaw	  =	front[2] * 90; // -90.875;
		init = true;
	}

	vec3 dir;
	f32 yaw_rad = yaw;     m_deg_to_rad(&yaw_rad);
	f32 pitch_rad = pitch; m_deg_to_rad(&pitch_rad);

	dir[0] = (f32)cos(yaw_rad) * (f32)cos(pitch_rad);
	dir[1] = (f32)sin(pitch_rad);
	dir[2] = (f32)sin(yaw_rad) * (f32)cos(pitch_rad);
	camera_set_front(dir);
}

