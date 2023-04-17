#ifndef OPERATION_H
#define OPERATION_H

#include "global/global.h"
#include "math/math_inc.h"

typedef enum 
{
  // @UNSURE:
  OP_GUI_WIN_OPEN,
  OP_GUI_WIN_CLOSE,
  OP_GUI_WIN_MINIMIZE,
  OP_GUI_WIN_MAXIMIZE,
  OP_GUI_WIN_MOVE,      

  OP_ENTITY_MOVE,
  OP_ENTITY_ROTATE,
  OP_ENTITY_SCALE,
  OP_ENTITY_REMOVE,         // @TODO: @UNSURE:
  OP_ENTITY_ADD,
  OP_ENTITY_CHILD_ADD,      // @TODO: 
  OP_ENTITY_CHILD_REMOVE,   // @TODO: 

  // @TODO:
  OP_MATERIAL_ROUGHNESS,
  OP_MATERIAL_METALLIC,
  OP_MATERIAL_TILE_SCL,
  OP_MATERIAL_TILE_BY_SCL,
  OP_MATERIAL_TILE,
  OP_MATERIAL_ADD,
  OP_MATERIAL_REMOVE,

  // @TODO:
  OP_PHYS_MASS,
  OP_PHYS_DRAG,
  OP_PHYS_FRICTION,
  OP_PHYS_POSITION,
  OP_PHYS_VELOCITY,
  OP_PHYS_FORCE,
  // @TODO: aabb, sphere
  OP_PHYS_ADD,
  OP_PHYS_REMOVE,

  // @TODO: 
  OP_POINT_LIGHT_POSITION,
  OP_POINT_LIGHT_COLOR,
  OP_POINT_LIGHT_INTENSITY,
  OP_POINT_LIGHT_ADD,
  OP_POINT_LIGHT_REMOVE,  
  
  // @TODO:
  OP_DIR_LIGHT_DIRECTION,
  OP_DIR_LIGHT_COLOR,
  OP_DIR_LIGHT_INTENSITY,
  OP_DIR_LIGHT_ADD,
  OP_DIR_LIGHT_REMOVE,

}operation_type;

INLINE const char* operation_type_to_str(operation_type t)
{
  return 
         // -- entity --
         t == OP_ENTITY_MOVE            ? "OP_ENTITY_MOVE"            :
         t == OP_ENTITY_ROTATE          ? "OP_ENTITY_ROTATE"          :
         t == OP_ENTITY_SCALE           ? "OP_ENTITY_SCALE"           :
         t == OP_ENTITY_REMOVE          ? "OP_ENTITY_REMOVE"          :
         t == OP_ENTITY_ADD             ? "OP_ENTITY_ADD"             :
         t == OP_ENTITY_CHILD_ADD       ? "OP_ENTITY_CHILD_ADD"       :
         t == OP_ENTITY_CHILD_REMOVE    ? "OP_ENTITY_CHILD_REMOVE"    :
         // -- phys --
         t == OP_PHYS_MASS              ? "OP_PHYS_MASS"              :
         t == OP_PHYS_DRAG              ? "OP_PHYS_DRAG"              :
         t == OP_PHYS_FRICTION          ? "OP_PHYS_FRICTION"          :
         t == OP_PHYS_POSITION          ? "OP_PHYS_POSITION"          :
         t == OP_PHYS_VELOCITY          ? "OP_PHYS_VELOCITY"          :
         t == OP_PHYS_FORCE             ? "OP_PHYS_FORCE"             :
         t == OP_PHYS_ADD               ? "OP_PHYS_ADD"               :
         t == OP_PHYS_REMOVE            ? "OP_PHYS_REMOVE"            :
         // -- material --
         t == OP_MATERIAL_ROUGHNESS     ? "OP_MATERIAL_ROUGHNESS"     :
         t == OP_MATERIAL_METALLIC      ? "OP_MATERIAL_METALLIC"      :
         t == OP_MATERIAL_TILE_SCL      ? "OP_MATERIAL_TILE_SCL"      :
         t == OP_MATERIAL_TILE_BY_SCL   ? "OP_MATERIAL_TILE_BY_SCL"   :
         t == OP_MATERIAL_TILE          ? "OP_MATERIAL_TILE"          :
         t == OP_MATERIAL_ADD           ? "OP_MATERIAL_ADD"           :
         t == OP_MATERIAL_REMOVE        ? "OP_MATERIAL_REMOVE"        :
         // -- point light --
         t == OP_POINT_LIGHT_POSITION   ? "OP_POINT_LIGHT_POSITION"   :
         t == OP_POINT_LIGHT_COLOR      ? "OP_POINT_LIGHT_COLOR"      :
         t == OP_POINT_LIGHT_INTENSITY  ? "OP_POINT_LIGHT_INTENSITY"  :
         t == OP_POINT_LIGHT_ADD        ? "OP_POINT_LIGHT_ADD"        :
         t == OP_POINT_LIGHT_REMOVE     ? "OP_POINT_LIGHT_REMOVE"     :
         // -- dir light --
         t == OP_DIR_LIGHT_DIRECTION    ? "OP_DIR_LIGHT_DIRECTION"    :
         t == OP_DIR_LIGHT_COLOR        ? "OP_DIR_LIGHT_COLOR"        :
         t == OP_DIR_LIGHT_INTENSITY    ? "OP_DIR_LIGHT_INTENSITY"    :
         t == OP_DIR_LIGHT_ADD          ? "OP_DIR_LIGHT_ADD"          :
         t == OP_DIR_LIGHT_REMOVE       ? "OP_DIR_LIGHT_REMOVE"       :

         "unknown operation_type";
}

typedef struct 
{
  operation_type type;

  // several names for int var for clarity
  union
  {
    int int_val_0;
    int entity_id; 
  };
  union
  {
    int int_val_1;
    int entity_template_idx;
    int child_id; 
  };
  union
  {
    int int_val_2;
    int entity_parent_id; 
    int child_parent_id; 
  };

  // several names for vec var for clarity
  union
  {
    vec3 vec_val_0;
    vec3 pos;
  };
  union
  {
    vec3 vec_val_1;
    vec3 rot;
  };
  union
  {
    vec3 vec_val_2;
    vec3 scl;
  };

}operation_t;

#define P_OPERATION_T(_op)  { PF(" -> type: %s, int_0: %d, int_1: %d, int_2: %d\n",       \
                              operation_type_to_str((_op)->type), (_op)->int_val_0,       \
                                                    (_op)->int_val_1, (_op)->int_val_2);  \
                              PF(" -> "); P_VEC3((_op)->vec_val_0);                       \
                              PF(" -> "); P_VEC3((_op)->vec_val_1);                       \
                              PF(" -> "); P_VEC3((_op)->vec_val_2); }                                     

#define OPERATION_T_ENTITY_ADD(id)      { .type = OP_ENTITY_ADD,       .entity_id = (id) }
#define OPERATION_T_ENTITY_REMOVE(id)   { .type = OP_ENTITY_REMOVE,    .entity_id = (id) }
#define OPERATION_T_ENTITY_CHILD_ADD(_id, _child_id, _child_parent) \
                                        { .type = OP_ENTITY_CHILD_ADD, .entity_id = (_id), .child_id = (_child_id), .child_parent_id = (_child_parent) }

void operation_register(operation_t* op);
void operation_reverse();

void operation_reverse_entity_move(operation_t* op);
void operation_reverse_entity_rotate(operation_t* op);
void operation_reverse_entity_scale(operation_t* op);
void operation_reverse_entity_add(operation_t* op);
void operation_reverse_entity_remove(operation_t* op);
void operation_reverse_entity_child_add(operation_t* op);

#endif // OPERATION_H
