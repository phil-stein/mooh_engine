#include "core/event_sys.h"
#include "core/types/types.h"
#include "core/state.h"
#include "global/global.h"

#include "stb/stb_ds.h"

// empty_callback** custom_arr = NULL;
// int              custom_arr_len = 0;

play_state_callback**     play_state_arr = NULL;
int                       play_state_arr_len = 0;

phys_collision_callback** phys_collision_arr = NULL;
int                       phys_collision_arr_len = 0;
phys_trigger_callback**   phys_trigger_arr = NULL;
int                       phys_trigger_arr_len = 0;

ent_added_callback**     ent_added_arr = NULL;
int                      ent_added_arr_len = 0;
ent_removed_callback**   ent_removed_arr = NULL;
int                      ent_removed_arr_len = 0;
ent_parented_callback**  ent_parented_arr = NULL;
int                      ent_parented_arr_len = 0;
ent_parent_rm_callback** ent_parent_rm_arr = NULL;
int                      ent_parent_rm_arr_len = 0;



// // void event_sys_trigger_started_frame();          // @UNSURE: before each new frame 
// 
// void event_sys_trigger_finished_asset_loading();  // after all assets have been loaded
// void event_sys_trigger_finished_setup();          // after the ´program starts outputing to the window
// // void event_sys_trigger_finished_frame();         // @UNSURE: after each new frame 

void event_sys_trigger_play_state(bool state)                     // on entity added to world
{
  for (int i = 0; i < play_state_arr_len; ++i)
  {
    play_state_arr[i](state);
  }
}

void event_sys_trigger_entity_added(int id)                     // on entity added to world
{
  for (int i = 0; i < ent_added_arr_len; ++i)
  {
    ent_added_arr[i](id);
  }
}
void event_sys_trigger_entity_removed(int id)                   // on entity removed from world
{
  for (int i = 0; i < ent_removed_arr_len; ++i)
  {
    ent_removed_arr[i](id);
  }
}
void event_sys_trigger_entity_parented(int parent, int child)   // on entity being parented
{
  for (int i = 0; i < ent_parented_arr_len; ++i)
  {
    ent_parented_arr[i](parent, child);
  }
}
void event_sys_trigger_entity_parent_removed(int parent, int child)   // on entity having its parent removed
{
  for (int i = 0; i < ent_parent_rm_arr_len; ++i)
  {
    ent_parent_rm_arr[i](parent, child);
  }
}

void event_sys_trigger_phys_collision(int id_01, int id_02)
{  
  entity_t* e_01 = state_entity_get(id_01);
  entity_t* e_02 = state_entity_get(id_02);

  // check if null
  if (e_01->collision_f) { e_01->collision_f(e_01, e_02); }
  if (e_02->collision_f) { e_02->collision_f(e_02, e_01); }

  for (int i = 0; i < phys_collision_arr_len; ++i)
  {
    phys_collision_arr[i](id_01, id_02);
  }
}
void event_sys_trigger_phys_trigger(int id_01, int id_02)       // on two entities colliding, at least one is set to trigger 
{
  entity_t* e_01 = state_entity_get(id_01);
  entity_t* e_02 = state_entity_get(id_02);

  // check if null
  if (e_01->trigger_f) { e_01->trigger_f(e_01, e_02); }
  if (e_02->trigger_f) { e_02->trigger_f(e_02, e_01); }

  for (int i = 0; i < phys_trigger_arr_len; ++i)
  {
    phys_trigger_arr[i](id_01, id_02);
  }
}

// --- register ---

// // void event_sys_register_started_frame(empty_callback callback);        
// 
// void event_sys_register_finished_asset_loading(empty_callback callback);
// void event_sys_register_finished_setup(empty_callback callback);
// // void event_sys_register_finished_frame(empty_callback callback);        
// 

void event_sys_register_play_state(play_state_callback callback)
{
  arrput(play_state_arr, callback);
  play_state_arr_len++;
}

void event_sys_register_entity_added(ent_added_callback callback)
{
  arrput(ent_added_arr, callback);
  ent_added_arr_len++;
}
void event_sys_register_entity_removed(ent_removed_callback callback)
{
  arrput(ent_removed_arr, callback);
  ent_removed_arr_len++;
}
void event_sys_register_entity_parented(ent_parented_callback callback)
{
  arrput(ent_parented_arr, callback);
  ent_parented_arr_len++;
}
void event_sys_register_entity_parent_removed(ent_parent_rm_callback callback)
{
  arrput(ent_parent_rm_arr, callback);
  ent_parent_rm_arr_len++;
}

void event_sys_register_phys_collision(phys_collision_callback* callback)
{
  arrput(phys_collision_arr, callback);
  phys_collision_arr_len++;
}
void event_sys_register_phys_trigger(phys_trigger_callback callback)       
{
  arrput(phys_trigger_arr, callback);
  phys_trigger_arr_len++;
}


