#include "core/event_sys.h"
#include "core/types/types.h"
#include "core/state.h"
#include "global/global.h"

#include "stb/stb_ds.h"


phys_collision_callback** phys_collision_arr = NULL;
int                       phys_collision_arr_len = 0;
phys_trigger_callback**   phys_trigger_arr = NULL;
int                       phys_trigger_arr_len = 0;

// // void event_sys_trigger_started_frame();          // @UNSURE: before each new frame 
// 
// void event_sys_trigger_finished_asset_loading();  // after all assets have been loaded
// void event_sys_trigger_finished_setup();          // after the ´program starts outputing to the window
// // void event_sys_trigger_finished_frame();         // @UNSURE: after each new frame 
// 
// void event_sys_trigger_entity_added(int id);                     // on entity added to world
// void event_sys_trigger_entity_removed(int id);                   // on entity removed from world
// void event_sys_trigger_entity_parented(int parent, int child);   // on entity being parented

void event_sys_trigger_phys_collision(int id_01, int id_02)
{  
  // P("");
  // P_INT(id_01);
  // P_INT(id_02);
  // P("");

  bool error = false;
  entity_t* e_01 = state_get_entity(id_01, &error); ASSERT(!error);
  entity_t* e_02 = state_get_entity(id_02, &error); ASSERT(!error);

  // check if null
  if (e_01->collision_f) { e_01->collision_f(e_01, e_02); }
  if (e_02->collision_f) { e_02->collision_f(e_02, e_01); }

  for (int i = 0; i < phys_collision_arr_len; ++i)
  {
    phys_collision_arr[i](id_01, id_02);
  }
  // arrfree(phys_collision_arr);
  // phys_collision_arr = NULL;
  // phys_collision_arr_len = 0;
}
void event_sys_trigger_phys_trigger(int id_01, int id_02)       // on two entities colliding, at least one is set to trigger 
{
  bool error = false;
  entity_t* e_01 = state_get_entity(id_01, &error); ASSERT(!error);
  entity_t* e_02 = state_get_entity(id_02, &error); ASSERT(!error);

  // check if null
  if (e_01->trigger_f) { e_01->trigger_f(e_01, e_02); }
  if (e_02->trigger_f) { e_02->trigger_f(e_02, e_01); }

  for (int i = 0; i < phys_trigger_arr_len; ++i)
  {
    phys_trigger_arr[i](id_01, id_02);
  }
  // arrfree(phys_collision_arr);
  // phys_collision_arr = NULL;
  // phys_collision_arr_len = 0;
}

// --- register ---

// // void event_sys_register_started_frame(empty_callback callback);        
// 
// void event_sys_register_finished_asset_loading(empty_callback callback);
// void event_sys_register_finished_setup(empty_callback callback);
// // void event_sys_register_finished_frame(empty_callback callback);        
// 
// void event_sys_register_entity_added(ent_added_callback callback);
// void event_sys_register_entity_removed(ent_removed_callback callback);
// void event_sys_register_entity_parented(ent_parented_callback callback);

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


