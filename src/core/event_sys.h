#ifndef CORE_EVENT_SYS_H
#define CORE_EVENT_SYS_H

#include "global/global.h"


// empty_callback in global.h
typedef void (play_state_callback)(bool state);
typedef void (ent_added_callback)(int id);
typedef void (ent_removed_callback)(int id);
typedef void (ent_parented_callback)(int parent, int child);
typedef void (ent_parent_rm_callback)(int parent, int child);
typedef void (phys_collision_callback)(int id_01, int id_02);
typedef void (phys_trigger_callback)(int id_01, int id_02);



// --- trigger ---

// @TODO: 
// void event_sys_trigger_started_frame();          // @UNSURE: before each new frame 

// @TODO:
// void event_sys_trigger_finished_asset_loading();  // after all assets have been loaded
// void event_sys_trigger_finished_setup();          // after the ´program starts outputing to the window
// void event_sys_trigger_finished_frame();         // @UNSURE: after each new frame 

// @DOC: calls all functions registered under event_sys_register_play_state
//       ! used internally
//       state: true -> play, false -> pause
void event_sys_trigger_play_state(bool state);

// @DOC: calls all functions registered under event_sys_register_entity_added
//       ! used internally
//       id: id of the added entity
void event_sys_trigger_entity_added(int id);                     // on entity added to world
// @DOC: calls all functions registered under event_sys_register_entity_removed
//       ! used internally
//       id: id of the removed entity
void event_sys_trigger_entity_removed(int id);                   // on entity removed from world
// @DOC: calls all functions registered under event_sys_register_entity_parented
//       ! used internally
//       parent: id of the new parent
//       child:  id of the new child
void event_sys_trigger_entity_parented(int parent, int child);   // on entity being parented
// @DOC: calls all functions registered under event_sys_register_entity_parent_removed
//       ! used internally
//       parent: id of the new parent
//       child:  id of the new child
void event_sys_trigger_entity_parent_removed(int parent, int child);   // on entity having its parent removed

// @DOC: calls all functions registered under event_sys_register_phys_collision
//       ! used internally
//       id_01: id of the first ent
//       id_02: id of the other ent
void event_sys_trigger_phys_collision(int id_01, int id_02);     // on two entities colliding 
// @DOC: calls all functions registered under event_sys_register_phys_trigger
//       ! used internally
//       id_01: id of the first ent
//       id_02: id of the other ent
void event_sys_trigger_phys_trigger(int id_01, int id_02);       // on two entities colliding, at least one is set to trigger 


// --- register ---

// @TODO:
// void event_sys_register_started_frame(empty_callback callback);   // @UNSURE:

// @TODO:
// void event_sys_register_finished_asset_loading(empty_callback callback);
// void event_sys_register_finished_setup(empty_callback callback);
// // void event_sys_register_finished_frame(empty_callback callback); // @UNSURE: 

// @DOC: register a function to be called when the play state is changed, play / pause 
//       callback: function pointer to the func
void event_sys_register_play_state(play_state_callback callback);

  // @DOC: register a function to be called when an entity is added 
//       callback: function pointer to the func
void event_sys_register_entity_added(ent_added_callback callback);
// @DOC: register a function to be called when an entity is removed
//       callback: function pointer to the func
void event_sys_register_entity_removed(ent_removed_callback callback);
// @DOC: register a function to be called when an entity is parented to another
//       callback: function pointer to the func
void event_sys_register_entity_parented(ent_parented_callback callback);
// @DOC: register a function to be called when an entity has its parent removed
//       callback: function pointer to the func
void event_sys_register_entity_parent_removed(ent_parent_rm_callback callback);

// @DOC: register a function to be called when two entites collide and both arent set to trigger
//       callback: function pointer to the func
void event_sys_register_phys_collision(phys_collision_callback* callback);     
// @DOC: register a function to be called when two entites collider, and at least on i set to trigger
//       callback: function pointer to the func
void event_sys_register_phys_trigger(phys_trigger_callback callback);       


#endif
