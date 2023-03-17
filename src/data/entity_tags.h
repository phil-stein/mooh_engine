#ifndef ENTITY_TAGS_H
#define ENTITY_TAGS_H

#include "global/global.h"

// @NOTE: put your tags here
//        for entity_t.tags_flag

// @NOTE: tags defined are tmp
typedef enum entity_tags_flag
{
  TAG_PLAYER   = FLAG(0),
  TAG_CUBE_DYN = FLAG(1), 
  TAG_UP_FORCE = FLAG(2),

}entity_tags_flag;



#endif // ENTITY_TAGS_H 
