#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global/global.h"
#include "math/math_inc.h"


// ---- vars ----

// ---- func decls ----

int main(void)
{
  P("hello");

  vec3 a = { 1, 1, 1 };
  vec3 b = { 2, 2, 2 };
  vec3 result;
  vec3_add(a, b, result);
  P_VEC3(result);

	return 0;
}



