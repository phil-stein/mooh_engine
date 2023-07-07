#include "data/test_comp.h"
#include "data/local_data.h"
#include "data/entity_tags.h"
#include "data/entity_template.h"
#include "core/core_data.h"
#include "core/input.h"
#include "core/camera.h"
#include "core/state.h"
#include "core/window.h"
#include "core/io/assetm.h"
#include "core/io/save_sys.h"
#include "core/debug/debug_draw.h"
#include "core/debug/debug_timer.h"
#include "math/math_inc.h"
#include "mui/mui.h"

#include "stb/stb_ds.h"

static core_data_t* core_data = NULL;


vec3 start_pos = { 0, 0, 0 }; // starting position of player char

#define AMMO_MAX 30



void player_camera(entity_t* this, f32 dt);
void player_ui(entity_t* this);

void player_init(entity_t* this)
{
  core_data = core_data_get();
  vec3_copy(this->pos, start_pos);
  input_center_cursor_pos(0, 0);
  input_set_cursor_visible(false);
}
void player_update(entity_t* this, f32 dt)
{
  player_ui(this);

  // - physics test -

  //  @NOTE: moving object with physics
  f32 speed      = 500.0f * dt;
  f32 jump_force = 600.0f * 80.0f * dt;
  if (input_get_key_down(KEY_LEFT_SHIFT))
  { speed *= 4.0f; }

  vec3 front;
  vec3 back;
  vec3 left;
  vec3 right;
  mat4_get_directions(this->model, front, back, left, right);
  // vec3 test;
  // vec3_mul_f(front, 2.0f, test);
  // vec3_add(this->pos, test, test);
  // debug_draw_line_register(this->pos, test, RGB_F(1, 0, 1));

  // shoot ball

  if (input_get_key_pressed(KEY_ENTER))
  {
    // static int projectile_id = -1;
    // if (projectile_id >= 0) { state_entity_remove(projectile_id); }
    vec3 projectile_pos, projectile_force;
    vec3_mul_f(front, 2.0f, projectile_pos);
    vec3_add(this->pos, projectile_pos, projectile_pos);
    projectile_pos[1] += 2.0f;
    int projectile_id = state_entity_add_from_template(projectile_pos, VEC3(0), VEC3(0.2f), ENTITY_TEMPLATE_PROJECTILE);
    
    entity_t* projectile = state_entity_get(projectile_id);
    vec3_mul_f(front, 2000.0f, projectile_force);
    ENTITY_SET_FORCE(projectile, projectile_force);
  }
  
  vec3_mul_f(front, speed, front);
  vec3_mul_f(back, speed, back);
  vec3_mul_f(left, speed, left);
  vec3_mul_f(right, speed, right);

  if (input_get_key_down(KEY_LEFT_ARROW)  || input_get_key_down(KEY_A))
  { ENTITY_FORCE(this, left); }
	if (input_get_key_down(KEY_RIGHT_ARROW) || input_get_key_down(KEY_D))
  { ENTITY_FORCE(this, right); }
	if (input_get_key_down(KEY_UP_ARROW)    || input_get_key_down(KEY_W))
  { ENTITY_FORCE(this, front); }
	if (input_get_key_down(KEY_DOWN_ARROW)  || input_get_key_down(KEY_S))
  { ENTITY_FORCE(this, back); }
	
  if (this->is_grounded && input_get_key_pressed(KEY_SPACE))
  { ENTITY_FORCE_Y(this, jump_force); }
  
  // @NOTE: reset when falling down
  if (this->pos[1] < -2.0f)
  { 
    P_INFO("player fell, reloading scene");
    P_VEC3(this->pos);

    save_sys_load_scene_from_state_buffer();
    // ENTITY_SET_POS(this, start_pos); 

    // // @TMP: reseting local data
    // local_data_test_t data = { .var0 = 0, .act = false, .var1 = 0.5f, .vector = { 0, 0, 0} };
    // local_data_arr[0] = data;
    // entity_local_data_key_t data_key =  { .arr_idx = 0, .type_id = local_data_id };
    // this->local_data[0] =  data_key; 
  }

  // @NOTE: set camera orientation
  vec3 cam_pos; 
  vec3_copy(core_data->cam.pos, cam_pos); // camera_get_pos(cam_pos);
  vec3 orientation = { 0.0f, -0.3f, -1.0f };
  vec3 target;
  vec3_add(this->pos, VEC3_Y(3), target); // so not looking at feet
  vec3_sub(target, cam_pos, orientation);
  vec3_normalize(orientation, orientation);
  camera_set_front(orientation);

  // -- mouse control --
  f32 x_offset = input_get_mouse_delta_x();
  f32 y_offset = input_get_mouse_delta_y();
  
  input_center_cursor_pos(); 
  input_set_cursor_visible(false);
  
  f32 mouse_sensitivity = 0.5f;
  x_offset *= mouse_sensitivity;
  y_offset *= mouse_sensitivity;

  ENTITY_ROTATE_Y(this, -x_offset * 0.75f);

  // @NOTE: rotate cam around player x-axis
  // - get player model
  // - rotate by x degree on x-axis
  // - get forward vector
  // - use vec to position cam
  
  static f32 camera_rot = 0.0f;
  camera_rot = CLAMP(camera_rot + y_offset, -80, 10);
  vec3 pos, rot;
  vec3_copy(this->pos, pos);
  vec3_copy(this->rot, rot);
  rot[0] -= camera_rot;

  
  mat4 player_model;
  mat4_make_model(pos, rot, VEC3(1), player_model);
  
  vec3_copy(VEC3(0), cam_pos);
  cam_pos[2] -= 10;  // offset to the back
  mat4 cam_model;
  mat4_make_model(cam_pos, VEC3(0), VEC3(1), cam_model);
  mat4_mul(player_model, cam_model, cam_model);

  // re-assign front & back as the player_model is different from the this->model
  mat4_get_forward(player_model, front);
  vec3_mul_f(front, -1.0f, back);

  vec3_mul_f(back, 10, pos);
  vec3_add(pos, this->pos, pos);
  
  // debug_draw_sphere_register(pos, 0.35f, RGB_F(0, 1, 1));
  vec3_copy(core_data->cam.pos, cam_pos); // camera_get_pos(cam_pos);
  vec3 dif;
  vec3_sub(pos, cam_pos, dif);
  // @NOTE: speed of cam-smoothing framerate dependent, but gets yittery when * dt
  vec3_mul_f(dif, 0.1f, dif); // CLAMP(8.0f * dt, 0.08f, 0.2f)
  // P_F32(8.0f * dt);
  // P_F32(CLAMP(8.0f * dt, 0.08f, 0.2f));
  vec3_add(cam_pos, dif, cam_pos);
  camera_set_pos(cam_pos);

}
void player_on_collision(entity_t* this, entity_t* col)
{
  // PF("player_on_collision: %d\n", col->id);
}
void player_on_trigger(entity_t* this, entity_t* trig)
{
  // PF("player_on_trigger: %d\n", trig->id);

  // check if trigger has tag
  if (HAS_FLAG(trig->tags_flag, TAG_UP_FORCE))
  {
    ENTITY_FORCE_Y(this, 50.0f);
  }
}

#define MAX_AMMO 30
int ammo = MAX_AMMO;

void player_ui(entity_t* this)
{
  TIMER_START("ui");

  texture_t* circle_tex = assetm_get_texture("#internal/circle.png", false);
  texture_t* weapon_tex = assetm_get_texture("_icons/kriss_vector_01.png", false);
  
  // -- circle & ammo --
  { 
    mui_img_tint(VEC2_XY(-0.8f, -0.8f),   VEC2(0.80f), circle_tex, VEC3(0.75f));
    mui_img_tint(VEC2_XY(-0.8f, -0.8f),   VEC2(0.65f), circle_tex, VEC3(0.55f));

    mui_img_tint(VEC2_XY(-0.8f, -0.8f),   VEC2(0.45f), weapon_tex, VEC3(1.00f));

    mui_img_tint(VEC2_XY(-0.72f, -0.72f), VEC2(0.50f), circle_tex, VEC3(0.35f));

    char txt[64];
    SPRINTF(64, txt, "%d|%d", ammo, AMMO_MAX);
    mui_text(VEC2_XY(-0.70f, -0.65f), txt, MUI_CENTER | MUI_UP);
  }
  // -- inventory --
  {
    mui_group_t group;
    mui_group_t items;
    // MUI_GROUP_T_INIT(&group, VEC2_XY(0.0f, -0.8f), VEC2_XY(1.75f, 0.5f), 0.0f, MUI_DYNAMIC | MUI_CENTER | MUI_ROW, false);
    // MUI_GROUP_T_INIT(&items, VEC2_XY(0.0f, -0.8f), VEC2_XY(1.75f, 0.5f), 0.0f, MUI_DYNAMIC | MUI_CENTER | MUI_ROW, false);
    // 
    // MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 1.00f, 1.00f, 1.00f));
    // MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 0.75f, 0.75f, 0.75f));
    // MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 0.50f, 0.50f, 0.50f));
    // MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 0.25f, 0.25f, 0.25f));
    // 
    // mui_group(&group);
    {
      MUI_GROUP_T_INIT(&group, VEC2_XY(0.0f, -0.8f), VEC2_XY(1.75f, 0.5f), 0.0f, MUI_STATIC | MUI_CENTER | MUI_ROW, false);
      
      MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 1.00f, 1.00f, 1.00f));
      MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 0.75f, 0.75f, 0.75f));
      MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 0.50f, 0.50f, 0.50f));
      MUI_GROUP_T_ADD(&group, MUI_OBJ_T_IMG_GROUP(circle_tex, 0.25f, 0.25f, 0.25f));
      
      mui_group(&group);
    }
    {
      MUI_GROUP_T_INIT(&items, VEC2_XY(0.0f, -0.8f), VEC2_XY(1.75f, 0.5f), 0.0f, MUI_STATIC | MUI_CENTER | MUI_ROW, false);
      mui_obj_t* slot_1, *slot_2, *slot_3, *slot_4;
      MUI_GROUP_T_ADD_PTR(&items, MUI_OBJ_T_IMG(0.0f, 0.0f, 0.55f, 0.55f, weapon_tex, 1.00f, 1.00f, 1.00f), slot_1);
      MUI_GROUP_T_ADD_PTR(&items, MUI_OBJ_T_IMG(0.0f, 0.0f, 0.55f, 0.55f, weapon_tex, 0.75f, 0.75f, 0.75f), slot_2);
      MUI_GROUP_T_ADD_PTR(&items, MUI_OBJ_T_IMG(0.0f, 0.0f, 0.55f, 0.55f, weapon_tex, 0.50f, 0.50f, 0.50f), slot_3);
      MUI_GROUP_T_ADD_PTR(&items, MUI_OBJ_T_IMG(0.0f, 0.0f, 0.55f, 0.55f, weapon_tex, 0.25f, 0.25f, 0.25f), slot_4);
      
      static int active_slot = 1;
      slot_1->active = active_slot == 1;
      slot_2->active = active_slot == 2;
      slot_3->active = active_slot == 3;
      slot_4->active = active_slot == 4;

      if (input_get_key_pressed(KEY_ALPHA1) || input_get_key_pressed(KEY_NUMPAD1)) { active_slot = 1; }
      if (input_get_key_pressed(KEY_ALPHA2) || input_get_key_pressed(KEY_NUMPAD2)) { active_slot = 2; }
      if (input_get_key_pressed(KEY_ALPHA3) || input_get_key_pressed(KEY_NUMPAD3)) { active_slot = 3; }
      if (input_get_key_pressed(KEY_ALPHA4) || input_get_key_pressed(KEY_NUMPAD4)) { active_slot = 4; }

      
      mui_group(&items);
    }
  }


  TIMER_STOP_PRINT(); // ui

  // // -- group --
  // {
  //   mui_group_t group = MUI_GROUP_T_INIT(0.5f, 0.5f, 0.2f, 0.1f, 0.0f, MUI_CENTER);
  //   vec3_copy(VEC3(0.1f), group.bg_color);
  //   group.bg_visible = true;
  //  
  //   mui_obj_t obj0 = MUI_OBJ_T_INIT_QUAD_GROUP(1.0f, 1.0f, 1.0f); 
  //   MUI_GROUP_T_ADD(&group, &obj0);
  //   // MUI_GROUP_T_ADD(&group, MUI_OBJ_T_INIT_QUAD_GROUP(VEC3(0.75f));
  //   // MUI_GROUP_T_ADD(&group, MUI_OBJ_T_INIT_QUAD_GROUP(VEC3(0.50f));

  //   mui_group(&group);
  // }
}

// --- projectile ---
typedef struct {u32 key; u32 value; } hm_entry_t;
typedef struct
{
  f32 time_alive;
  int target_id;
}local_data_test_t;
local_data_test_t data_default = { .time_alive = 0.0f, .target_id = -1 };

LOCAL_DATA_DECL(local_data_test_t, local_data);

const f32 time_alive_max = 1.0f;
const f32 start_scl      = 0.5f; 

void projectile_init(entity_t* this)
{
  LOCAL_DATA_INIT(local_data_test_t, local_data, data_default, data);


  vec3_copy_f(start_scl, this->scl);

  // pick random target
  int world_len, world_dead_len;
  entity_t* world = state_entity_get_arr(&world_len, &world_dead_len);
  data->target_id = -1;
  for (u32 i = 0; i < 10; ++i)  // 10 tries to find alive target
  {
    int idx = rand_int_range(0, world_len);
    if (!world[idx].is_dead) { data->target_id = world[idx].id; break; } 
  }
  P_INT(data->target_id);
}
void projectile_update(entity_t* this, f32 dt)
{
  LOCAL_DATA_GET(local_data_test_t, local_data, data);

  data->time_alive += dt;
  if (data->time_alive >= time_alive_max)
  {
    state_entity_remove(this);
    return;
  }

  if (data->target_id >= 0)
  {
    bool err = false;
    entity_t* e = state_entity_get_err(data->target_id, &err);
    if (!err)
    {
      vec3 pos;
      vec3_lerp(this->pos, e->pos, data->time_alive / time_alive_max, pos);
      ENTITY_SET_POS(this, pos);
    }
  }

  // lerp size to 0
  vec3 scl;
  vec3_lerp_f(start_scl, 0.0f, data->time_alive / time_alive_max, scl);
  ENTITY_SET_SCL(this, scl);
}
void projectile_cleanup(entity_t* this)
{
  LOCAL_DATA_DEL(local_data);
}

void entity_init(entity_t* this)
{
  // P("-- entity init --");
}
void entity_update(entity_t* this, f32 dt)
{
  const f32 speed = 10.0f * dt;
  // this->rot[2] += speed;
  ENTITY_ROTATE_Y(this, speed);

  // f32 r = (f32)sin(time_get_total_t());
  // f32 g = (f32)cos(time_get_total_t());
  // f32 b = (f32)sin(time_get_total_t());
  // r = (r + 1) * 0.5f;
  // g = (g + 1) * 0.5f;
  // b = (b + 1) * 0.5f;
  // 
  // vec3_copy(RGB_F(r, g, b), this->tint);

  // this->pos[1] = r * 0.8f +4;

	// if (input_get_key_down(KEY_LEFT_ARROW))
  // { this->pos[0] -= speed; }
	// if (input_get_key_down(KEY_RIGHT_ARROW))
  // { this->pos[0] += speed; }
	// if (input_get_key_down(KEY_UP_ARROW))
  // { this->pos[2] -= speed; }
	// if (input_get_key_down(KEY_DOWN_ARROW))
  // { this->pos[2] += speed; }

}
