#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "editor/gui/gui.h"
#include "editor/gui/gui_style.h"
#include "editor/app.h"
#include "editor/operation.h"
#include "editor/stylesheet.h"
#include "core/core_data.h"
#include "core/window.h"
#include "core/state.h"
#include "core/input.h"
#include "core/io/assetm.h"
#include "core/io/save_sys.h"
#include "core/renderer/renderer_extra.h"

// @NOTE: tmp
#include "phys/phys_world.h"
#include "phys/phys_debug_draw.h"

void gui_properties_win(ui_context* ctx, ui_rect win_rect, const u32 win_flags, bool is_selected)
{
  // window test --------------------------------------------------------------------------------------
  // int w, h;
  // window_get_size(&w, &h);
  
  core_data_t* core_data = core_data_get();;
  app_data_t*  app_data  = app_data_get();;


  if (is_selected && nk_begin(ctx, "properties", win_rect, win_flags)) 
  {
    // -- properties --

    int id = app_data->selected_id;
    if (id >= 0)  // entities
    {
      entity_t* e     = state_entity_get(id);
      int world_len = 0; int dead_len = 0;
      state_entity_get_arr(&world_len, &dead_len);
      
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_labelf(ctx, NK_TEXT_LEFT, "id: %d, table_idx: %d", id, e->template_idx);
      nk_labelf(ctx, NK_TEXT_LEFT, "parent: %d, #children: %d", e->parent, e->children_len);
      nk_layout_row_dynamic(ctx, 25, 2);
      static int parent_id = 0;
      nk_property_int(ctx, "parent id", 0, &parent_id, world_len -1, 1, 0.1f);
      if (nk_button_label(ctx, "set parent") && parent_id >= 0 && parent_id < world_len)
      {
        bool error = false;
        state_entity_get_err(parent_id, &error);  // just checking if exists // @TODO: isnt there a state func for that
        if (!error)
        {
          state_entity_add_child_remove_parent_id(parent_id, id, true);
          operation_t op = OPERATION_T_ENTITY_CHILD_ADD(parent_id, id, e->parent);
          operation_register(&op);
          
          GUI_INFO_STR_SET(app_data, "parented: %d -> %d", e->id, e->parent);
        }
        else { ERR("parent id not valid: %d", parent_id); }
      }

      nk_layout_row_dynamic(ctx, 25, 1);
      if (nk_button_label(ctx, "remove parent") && e->parent >= 0)
      {
        GUI_INFO_STR_SET(app_data, "unparented: %d -> %d", e->id, e->parent);
        state_entity_remove_child_id(e->parent, e->id, true);
      }

      // @TODO: @UNSURE: display children

      nk_layout_row_dynamic(ctx, 25, 2);
      nk_labelf(ctx, NK_TEXT_LEFT, "init:   %s", STR_BOOL(e->init_f != NULL));
      nk_labelf(ctx, NK_TEXT_LEFT, "update: %s", STR_BOOL(e->update_f != NULL));
      nk_layout_row_dynamic(ctx, 25, 1);
      nk_labelf(ctx, NK_TEXT_LEFT, "is_moved: %s", STR_BOOL(e->is_moved));
      

      nk_layout_row_dynamic(ctx, 30, 2);
      if (nk_button_label(ctx, "remove"))
      {
        operation_t op = OPERATION_T_ENTITY_REMOVE(app_data->selected_id);
        operation_register(&op);

        state_entity_remove_id(app_data->selected_id);
        app_data->selected_id = -1;
      }
      if (nk_button_label(ctx, "duplicate"))
      {
        int id = state_entity_duplicate_id(app_data->selected_id, VEC3_XYZ(2, 0, 0));
        app_data->selected_id = id;
      }
      nk_layout_row_dynamic(ctx, 30, 1);

      nk_layout_row_dynamic(ctx, 30, 2);
      static char struct_name_buffer[64] = "\0";
      nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD | NK_EDIT_AUTO_SELECT, struct_name_buffer, sizeof(struct_name_buffer), nk_filter_ascii);
      bool btn_act = strlen(struct_name_buffer) > 0;
      gui_style_set_button_color(ctx, btn_act);
      if (nk_button_symbol_label(ctx, btn_act ? NK_SYMBOL_NONE : NK_SYMBOL_X, "save as structure", NK_TEXT_ALIGN_RIGHT))
      {
        save_sys_write_structure_to_file(struct_name_buffer, e->id);
      }
      gui_style_set_button_color(ctx, true);
      
      nk_layout_row_dynamic(ctx, 30, 1);

      // -- components --

      if (nk_tree_push(ctx, NK_TREE_TAB, "transform", NK_MINIMIZED))
      {
        gui_properties_transform(ctx, e, e->pos, e->rot, e->scl, &e->is_moved);
        nk_tree_pop(ctx);
      }
      
      if (e->mat >= 0 && e->mesh >= 0 && nk_tree_push(ctx, NK_TREE_TAB, "material", NK_MINIMIZED))
      {
        material_t* mat = assetm_get_material_by_idx(e->mat);
        gui_properties_material(ctx, win_rect, mat, e->mat);
        nk_tree_pop(ctx);
      }
      
      if (e->mesh >= 0 && nk_tree_push(ctx, NK_TREE_TAB, "mesh", NK_MINIMIZED))
      {
        mesh_t* m = assetm_get_mesh_by_idx(e->mesh);
        gui_properties_mesh(ctx, m, e->mesh, e->template_idx);
        nk_tree_pop(ctx);
      }

      if (e->point_light_idx >= 0 && nk_tree_push(ctx, NK_TREE_TAB, "point light", NK_MINIMIZED))
      {
        bool error = false;
        point_light_t* p = state_point_light_get(e->point_light_idx, &error);
        // gui_properties_point_light(e->pos, VEC3(1), 1);
        gui_properties_point_light(ctx, p, e->point_light_idx);
        nk_tree_pop(ctx);
      }
      else 
      {
        if (nk_button_label(ctx, "add pointlight"))
        { state_point_light_add(VEC3(0), VEC3(1), 1.0f, e->id); }
      }

      const entity_template_t* def = entity_template_get(e->template_idx);
      if (def->phys_flag != 0 && nk_tree_push(ctx, NK_TREE_TAB, "physics", NK_MINIMIZED))
      {
        gui_properties_physics(ctx, def, e);
        nk_tree_pop(ctx);
      }

    } 
    else if (id <= ID_BUFFER_TERRAIN_0)  // terrain
    {
      int idx = ID_BUFFER_TERRAIN_TO_CHUNK_IDX(id);
      nk_labelf(ctx, NK_TEXT_LEFT, "[terrain]");
      nk_labelf(ctx, NK_TEXT_LEFT, "-> id: %d, idx: %d", id, idx);

      terrain_chunk_t* chunk = &core_data->terrain_chunks[idx];
      
      if (chunk->loaded && nk_tree_push(ctx, NK_TREE_TAB, "edit", NK_MINIMIZED))
      {
        // nk_selectable_label(ctx, "edit", NK_TEXT_CENTERED, &app_data->terrain_edit_act); 
       
        const char* names[]  = { "none", "sculpt", "level", "smooth", "paint" };
        const int names_len = 5;
        app_data->terrain_edit_type = nk_combo(ctx, names, names_len, app_data->terrain_edit_type, 25, nk_vec2(200, 200));

        nk_property_float(ctx, "strength", 0.0f, &app_data->terrain_edit_strength, 10.0f, 0.1f, 0.01f);
        nk_property_float(ctx, "radius", 0.0f, &app_data->terrain_edit_radius, 20.0f, 0.1f, 0.01f);
        if (app_data->terrain_edit_type == TERRAIN_EDIT_PAINT)
        {
          nk_property_int(ctx, "material", 0, &app_data->terrain_edit_paint_material, core_data->terrain_materials_len, 1, 0.01f);
        }
        nk_tree_pop(ctx);
      }

      if (chunk->loaded && nk_tree_push(ctx, NK_TREE_TAB, "chunk", NK_MINIMIZED))
      {
        nk_labelf(ctx, NK_TEXT_LEFT, "pos [ %.2f, %.2f, %.2f ]", chunk->pos[0], chunk->pos[1], chunk->pos[2]);
        nk_labelf(ctx, NK_TEXT_LEFT, "strips_num: %d, verts_per_strip: %d", chunk->strips_num, chunk->verts_per_strip);
      
        nk_tree_pop(ctx);
      }
      
      if (chunk->loaded && nk_tree_push(ctx, NK_TREE_TAB, "layout", NK_MINIMIZED))
      {
        nk_property_int(ctx, "draw dist", 1, (int*)&core_data->terrain_draw_dist, 20, 1, 0.01f);
        nk_property_int(ctx, "cull dist", core_data->terrain_draw_dist +1, (int*)&core_data->terrain_cull_dist, 25, 1, 0.01f);
      
        nk_spacing(ctx, 1);
        nk_label(ctx, "add chunk", NK_TEXT_LEFT);

        static ivec2 pos = { 0, 0 };
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_property_int(ctx, "x", -1000, &pos[0], 1000, 1, 0.01f);
        nk_property_int(ctx, "z", -1000, &pos[1], 1000, 1, 0.01f);
        
        nk_layout_row_dynamic(ctx, 25, 2);
        if (nk_button_label(ctx, "add chunk"))
        { 
          terrain_add_chunk(pos); 
        }

        nk_tree_pop(ctx);
      }
      
       if (nk_tree_push(ctx, NK_TREE_TAB, "materials", NK_MINIMIZED))
      {
        char buf[32];
        for (u32 i = 0;  i < core_data->terrain_materials_len; ++i)
        {
          sprintf(buf, "materials[%d]", i);
          if (nk_tree_push(ctx, NK_TREE_TAB, buf, NK_MINIMIZED))
          {
            material_t* mat = assetm_get_material_by_idx(core_data->terrain_materials[i]);
            gui_properties_material(ctx, win_rect, mat, core_data->terrain_materials[i]);
            nk_tree_pop(ctx);
          }
        }
        nk_tree_pop(ctx);
      }
    }
    else
    { nk_labelf(ctx, NK_TEXT_LEFT, "no entity selected"); }
  }
  if (is_selected)
  { nk_end(ctx); }
}
void gui_properties_transform(ui_context* ctx, entity_t* e, vec3 pos, vec3 rot, vec3 scl, bool* has_moved)
{

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "position", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  vec3 pos_old; vec3_copy(pos, pos_old);
  nk_property_float(ctx, "p.x", -2048.0f, &pos[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.y", -2048.0f, &pos[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.z", -2048.0f, &pos[2], 2048.0f, 0.1f, 0.01f);
  if (!vec3_equal(pos_old, pos)) { *has_moved = true; }
  vec3 g_pos;
  mat4_get_pos(e->model, g_pos);
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_TEXT_LEFT, "global pos: x: %.2f, y: %.2f, z: %.2f", g_pos[0], g_pos[1], g_pos[2]);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "rotation", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  vec3 rot_old; vec3_copy(rot, rot_old);
  nk_property_float(ctx, "r.x", -2048.0f, &rot[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "r.y", -2048.0f, &rot[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "r.z", -2048.0f, &rot[2], 2048.0f, 0.1f, 0.01f);
  if (!vec3_equal(rot_old, rot)) { *has_moved = true; }

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "scale", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  vec3 scl_old; vec3_copy(scl, scl_old);
  nk_property_float(ctx, "s.x", -2048.0f, &scl[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "s.y", -2048.0f, &scl[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "s.z", -2048.0f, &scl[2], 2048.0f, 0.1f, 0.01f);
  if (!vec3_equal(scl_old, scl)) { *has_moved = true; }
}
void gui_properties_material(ui_context* ctx, ui_rect win_rect, material_t* mat, int idx)
{
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_LEFT, "idx: %d", idx);

  const int size = win_rect.w / 2 - 20;
  nk_layout_row_static(ctx, size, size, 2);
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->albedo)->handle));
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->normal)->handle));
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->roughness)->handle));
  nk_image(ctx, nk_image_id(assetm_get_texture_by_idx(mat->metallic)->handle));

  nk_layout_row_dynamic(ctx, 30, 1);
  gui_color_selector(mat->tint);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_property_float(ctx, "roughness", 0.0f, &mat->roughness_f, 1.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "metallic",  0.0f, &mat->metallic_f,  1.0f, 0.1f, 0.01f);

  nk_spacing(ctx, 1);
  nk_checkbox_label(ctx, "tile by scale", &mat->tile_by_scl);
  nk_property_float(ctx, "tile scale",  -10.0f, &mat->tile_scl,  10.0f, 0.1f, 0.01f);
  nk_layout_row_dynamic(ctx, 30, 2);
  nk_property_float(ctx, "tile.x",  -10.0f, &mat->tile[0],  10.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "tile.y",  -10.0f, &mat->tile[1],  10.0f, 0.1f, 0.01f);
  nk_layout_row_dynamic(ctx, 30, 1);

  nk_spacing(ctx, 1);
  if (nk_button_label(ctx, "print template"))
  {
    ASSERT(mat->template_idx > 0);
#ifndef _MSC_VER
    material_template_t m = (material_template_t)( *material_template_get(mat->template_idx) );
#else
    material_template_t m = *material_template_get(mat->template_idx);
#endif
    vec3_copy(mat->tint, m.tint);
    m.roughn_f    = mat->roughness_f;
    m.metall_f    = mat->metallic_f;
    m.tile_scl    = mat->tile_scl;
    m.tile_by_scl = mat->tile_by_scl;
    vec2_copy(mat->tile, m.tile);
    const char* str = material_template_generate_string(&m);
    PF("%s", str);
  }
}
void gui_properties_mesh(ui_context* ctx, mesh_t* m, int idx, int entity_template_idx)
{  
  const entity_template_t* tmplt = entity_template_get(entity_template_idx);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_LEFT, "name:    %s", tmplt->mesh);
  nk_labelf(ctx, NK_LEFT, "f32 per vert: %d", m->floats_per_vert);
  nk_labelf(ctx, NK_LEFT, "verts:   %d", m->verts_count);
  nk_labelf(ctx, NK_LEFT, "indices: %d", m->indices_count);
  
}
void gui_properties_point_light(ui_context* ctx, point_light_t* p, int idx)
{
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_labelf(ctx, NK_LEFT, "id: %d, arr idx: %d", p->id, idx);
  
  nk_label(ctx, "position offset", NK_TEXT_LEFT);
  nk_layout_row_dynamic(ctx, 30, 3);
  nk_property_float(ctx, "p.x", -2048.0f, &p->offset[0], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.y", -2048.0f, &p->offset[1], 2048.0f, 0.1f, 0.01f);
  nk_property_float(ctx, "p.z", -2048.0f, &p->offset[2], 2048.0f, 0.1f, 0.01f);
  // nk_labelf(ctx, NK_LEFT, "p.x %.2f", (*p->pos_ptr)[0]);
  // nk_labelf(ctx, NK_LEFT, "p.y %.2f", (*p->pos_ptr)[1]);
  // nk_labelf(ctx, NK_LEFT, "p.z %.2f", (*p->pos_ptr)[2]);

  gui_color_selector(p->color);
  
  nk_property_float(ctx, "intensity", -2.0f, &p->intensity, 2048.0f, 0.1f, 0.01f);
  
}
void gui_properties_physics(ui_context* ctx, const entity_template_t* def, entity_t* e)
{
  // @NOTE: tmp
  u32 arr_len = 0;
  phys_obj_t* arr = phys_get_obj_arr(&arr_len);
  phys_obj_t* obj = NULL;
  for (u32 i = 0; i < arr_len; ++i)
  {
    if (arr[i].entity_idx == e->id) { obj = &arr[i]; }
  }
  if (obj)
  {
    nk_layout_row_dynamic(ctx, 30, 1);
    if (HAS_FLAG(def->phys_flag, ENTITY_HAS_RIGIDBODY))
    {
      nk_labelf(ctx, NK_TEXT_LEFT, " -- rigidbody --");
      
      nk_property_float(ctx, "mass", 0.1f, &obj->rb.mass, 4096.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "drag", 0.0001f, &obj->rb.drag, 2.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "friction", 0.0001f, &obj->rb.friction, 2.0f, 0.1f, 0.01f); 

      nk_labelf(ctx, NK_TEXT_LEFT, "pos: %f, %f, %f", obj->pos[0], obj->pos[1], obj->pos[2]);
      nk_labelf(ctx, NK_TEXT_LEFT, "vel: %f, %f, %f", obj->rb.velocity[0], obj->rb.velocity[1], obj->rb.velocity[2]);
      // nk_labelf(ctx, NK_TEXT_LEFT, "f:   %f, %f, %f", obj->rb.force[0], obj->rb.force[1], obj->rb.force[2]);
      
      nk_property_float(ctx, "force.x", -2048.0f, &obj->rb.force[0], 2048.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "force.y", -2048.0f, &obj->rb.force[1], 2048.0f, 0.1f, 0.01f); 
      nk_property_float(ctx, "force.z", -2048.0f, &obj->rb.force[2], 2048.0f, 0.1f, 0.01f);

      nk_labelf(ctx, NK_TEXT_LEFT, "is_grounded: %s", STR_BOOL(obj->collider.is_grounded));

    }
    if (HAS_FLAG(def->phys_flag, ENTITY_HAS_SPHERE))
    {
      nk_labelf(ctx, NK_TEXT_LEFT, " -- sphere --");
      nk_labelf(ctx, NK_TEXT_LEFT, "radius: %.2f", def->radius);
      
      nk_labelf(ctx, NK_TEXT_LEFT, "is trigger: %s", STR_BOOL(def->is_trigger));
      
      phys_debug_draw_sphere_collider(obj);
    }
    if (HAS_FLAG(def->phys_flag, ENTITY_HAS_BOX))
    {
      nk_labelf(ctx, NK_TEXT_LEFT, " -- box --");
      nk_labelf(ctx, NK_TEXT_LEFT, "[aabb]   x: %.2f y: %.2f, z: %.2f", def->aabb_size[0], def->aabb_size[1], def->aabb_size[2]);
      nk_labelf(ctx, NK_TEXT_LEFT, "[offset] x: %.2f y: %.2f, z: %.2f", obj->collider.offset[0], obj->collider.offset[1], obj->collider.offset[2]);
      
      nk_labelf(ctx, NK_TEXT_LEFT, "is trigger: %s", STR_BOOL(def->is_trigger));
    
      // doesnt make much sense, just use obb
      // if (nk_button_label(ctx, "rotate box y"))
      // {
      //   phys_rotate_box_y(e->id);
      // }

      phys_debug_draw_box_collider(obj);

    }
  }

}


