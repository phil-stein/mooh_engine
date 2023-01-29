#include "global/global.h"


// empty_callback in global.h
typedef void (ent_added_callback)(int id);
typedef void (ent_removed_callback)(int id);
typedef void (ent_parented_callback)(int parent, int child);
typedef void (phys_collision_callback)(int id_01, int id_02);
typedef void (phys_trigger_callback)(int id_01, int id_02);



// --- trigger ---

// void event_sys_trigger_started_frame();          // @UNSURE: before each new frame 

void event_sys_trigger_finished_asset_loading();  // after all assets have been loaded
void event_sys_trigger_finished_setup();          // after the ´program starts outputing to the window
// void event_sys_trigger_finished_frame();         // @UNSURE: after each new frame 

void event_sys_trigger_entity_added(int id);                     // on entity added to world
void event_sys_trigger_entity_removed(int id);                   // on entity removed from world
void event_sys_trigger_entity_parented(int parent, int child);   // on entity being parented

void event_sys_trigger_phys_collision(int id_01, int id_02);     // on two entities colliding 
void event_sys_trigger_phys_trigger(int id_01, int id_02);       // on two entities colliding, at least one is set to trigger 


// --- register ---

// void event_sys_register_started_frame(empty_callback callback);        

void event_sys_register_finished_asset_loading(empty_callback callback);
void event_sys_register_finished_setup(empty_callback callback);
// void event_sys_register_finished_frame(empty_callback callback);        

void event_sys_register_entity_added(ent_added_callback callback);
void event_sys_register_entity_removed(ent_removed_callback callback);
void event_sys_register_entity_parented(ent_parented_callback callback);

void event_sys_register_phys_collision(phys_collision_callback* callback);     
void event_sys_register_phys_trigger(phys_trigger_callback callback);       


