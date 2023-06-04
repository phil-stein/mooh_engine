#include "editor/operation.h"
#include "editor/app.h"
#include "core/state.h"

#include "stb/stb_ds.h"

operation_t* op_arr = NULL;
u32          op_arr_len = 0;

static app_data_t* app_data = NULL;

void operation_register(operation_t* op)
{
  // gather additional data
  switch (op->type)
  {
    case OP_ENTITY_REMOVE:
      {
        entity_t* e = state_entity_get(op->entity_id);
        vec3_copy(e->pos, op->pos);
        vec3_copy(e->rot, op->rot);
        vec3_copy(e->scl, op->scl);
        op->entity_parent_id = e->parent;
        op->entity_template_idx = e->template_idx;
      }
      // make entity_t arr and store in their for all info ?
      break;

    default:
      break;
  }
  
  // add to array
  arrput(op_arr, *op);
  op_arr_len++;

  // debug print
  P("OPERATION REGISTERED:"); 
  P_OPERATION_T(op);
}

void operation_reverse()
{
  if (op_arr_len <= 0)
  { P_ERR("tried operation_reverse(), but no registered operations\n"); }

  operation_t op = arrpop(op_arr);
  op_arr_len--;

  switch (op.type)
  {
    case OP_ENTITY_MOVE:
      operation_reverse_entity_move(&op);
      break;
    case OP_ENTITY_ROTATE:
      operation_reverse_entity_rotate(&op);
      break;
    case OP_ENTITY_SCALE:
      operation_reverse_entity_scale(&op);
      break;
    case OP_ENTITY_ADD:
      operation_reverse_entity_add(&op);
      break;
    case OP_ENTITY_REMOVE:
      operation_reverse_entity_remove(&op);
      break;
    case OP_ENTITY_CHILD_ADD:
      operation_reverse_entity_child_add(&op);
      break;

    default:
      ERR("unknown operation_type in operation_reverse(): %d\n", op.type);
  }
   
  app_data = app_data_get(); 
  // char info_str[GUI_INFO_STR_MAX];
  // SPRINTF(GUI_INFO_STR_MAX, info_str, "undo: %s", operation_type_to_str(op.type));
  GUI_INFO_STR_SET(app_data, "undo: %s", operation_type_to_str(op.type));
}

void operation_reverse_entity_move(operation_t* op)
{
  bool error  = false;
  entity_t* e = state_entity_get_err(op->entity_id, &error);
  if (error)
  {
    P_ERR("failed reversing entity move, because entity: %d, not found\n", op->entity_id);
    return;
  }

  vec3 diff;
  vec3_negate(op->pos, diff);
  ENTITY_MOVE(e, diff);
  
  PF("OPERATION REVERSE:\n");
  P_OPERATION_T(op);
}
void operation_reverse_entity_rotate(operation_t* op)
{
  bool error  = false;
  entity_t* e = state_entity_get_err(op->entity_id, &error);
  if (error)
  {
    P_ERR("failed reversing entity rotate, because entity: %d, not found\n", op->entity_id);
    return;
  }

  vec3 diff;
  vec3_negate(op->rot, diff);
  ENTITY_ROTATE(e, diff);
  
  PF("OPERATION REVERSE:\n");
  P_OPERATION_T(op);
}
void operation_reverse_entity_scale(operation_t* op)
{
  bool error  = false;
  entity_t* e = state_entity_get_err(op->entity_id, &error);
  if (error)
  {
    P_ERR("failed reversing entity scale, because entity: %d, not found\n", op->entity_id);
    return;
  }

  vec3 diff;
  vec3_negate(op->scl, diff);
  ENTITY_SCALE(e, diff);
  
  PF("OPERATION REVERSE:\n");
  P_OPERATION_T(op);
}
void operation_reverse_entity_add(operation_t* op)
{
  state_entity_remove_id(op->entity_id);
  
  PF("OPERATION REVERSE:\n");
  P_OPERATION_T(op);
}

void operation_reverse_entity_remove(operation_t* op)
{
  int id = state_entity_add_from_template(op->pos, op->rot, op->scl, op->entity_template_idx);
  // parent, children, etc.

  app_data = app_data_get();
  app_data->selected_id = id;

  PF("OPERATION REVERSE:\n");
  P_OPERATION_T(op);
}
void operation_reverse_entity_child_add(operation_t* op)
{
  state_entity_remove_child_id(op->entity_id, op->child_id, true);

  if (op->child_parent_id >= 0)
  {
    state_entity_add_child_id(op->child_parent_id, op->child_id, true);
  }

  PF("OPERATION REVERSE:\n");
  P_OPERATION_T(op);
}


