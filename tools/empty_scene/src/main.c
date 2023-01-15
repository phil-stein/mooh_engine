#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global/global.h"
#include "math/math_inc.h"
#include "core/serialization.h"

// ---- vars ----

// ---- func decls ----

int main(void)
{
  P("hello");
  
  
  serialization_load_scene_from_file("cock");
  // serialization_write_empty_scene_to_file();

	return 0;
}



