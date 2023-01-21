#ifndef TEST_COMP_H
#define TEST_COMP_H

#include "global/global.h"
#include "core/types/types.h"

void player_init(entity_t* this);
void player_update(entity_t* this, f32 dt);
void player_on_collision(entity_t* this, entity_t* col);
void player_on_trigger(entity_t* this, entity_t* trig);

void entity_init(entity_t* this);
void entity_update(entity_t* this, f32 dt);

#endif
