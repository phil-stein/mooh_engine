#include "global/global.h"
#include "math/math_inc.h"
#include "core/file_io.h"
#include "serialization/serialization.h"

#define STB_DS_IMPLEMENTATION // only define once
#include "stb/stb_ds.h"

#include <ctype.h>  // isspace(), etc.

// @NOTE: in mooh defined in makefile 
#define ASSET_PATH "../../assets"

#define ARG_MAX_LEN 56

// @NOTE: undef to hide  
#define BIN_P(a) P(a)
#define BIN_PF(a, ...) PF(a, __VA_ARGS__)

// -- func decls --
void write_empty_scene_to_file(const char* name);

int main(int argc, char** args)
{
  // @NOTE: PF because always need to be printed
  if (argc <= 1) { PF("no argument given\n -scene_empty \"name\"\n"); return 1; }

  for (u32 i = 1; i < argc; ++i)
  {
    if (args[i][0] == '-')
    {
      // for test
      // P("has -");
      // if (args[i][1]  == 's') { P("has s"); }
      // if (args[i][2]  == 'c') { P("has c"); } 
      // if (args[i][3]  == 'e') { P("has e"); } 
      // if (args[i][4]  == 'n') { P("has n"); } 
      // if (args[i][5]  == 'e') { P("has e"); } 
      // if (args[i][6]  == '_') { P("has _"); } 
      // if (args[i][7]  == 'e') { P("has e"); } 
      // if (args[i][8]  == 'm') { P("has m"); } 
      // if (args[i][9]  == 'm') { P("has p"); } 
      // if (args[i][10] == 't') { P("has t"); } 
      // if (args[i][11] == 'y') { P("has y"); }

      if (args[i][1]  == 's' &&
          args[i][2]  == 'c' &&
          args[i][3]  == 'e' &&
          args[i][4]  == 'n' &&
          args[i][5]  == 'e' &&
          args[i][6]  == '_' &&
          args[i][7]  == 'e' &&
          args[i][8]  == 'm' &&  
          args[i][9]  == 'p' &&  
          args[i][10] == 't' &&
          args[i][11] == 'y' )
      {
        i++;
        BIN_PF("[bin] name: %s\n", args[i]);
        write_empty_scene_to_file(args[i]); //("-_-_new_-_-");
        break;
      }
    }
  }


	return 0;
}

// used to be after if (args[i][1] == 's', etc.)
      // {
      //   P("correct arg");
      //   // skip whitespace
      //   u32 len = strlen(args[i]);
      //   u32 j = 11;
      //   while (j < len && isspace(args[i][j])) { j++; }
      //   if (args[i][j] == '"')
      //   {
      //     j++; // skip '"'
      //     char name[ARG_MAX_LEN];
      //     u32  name_pos = 0;
      //     while (j < len && name_pos < ARG_MAX_LEN && args[i][j] != '"')
      //     {
      //       name[name_pos++] = args[i][j++];
      //     }
      //     name[++name_pos] = '\0';  // terminate string
      //     
      //     P_STR(name);
      //     write_empty_scene_to_file(name); //("-_-_new_-_-");
      //     break;
      //   }
      // }

void write_empty_scene_to_file(const char* name)
{
  u8* buffer = NULL;

  // serialization_serialize_scene(&buffer);
  serialization_serialize_u32(&buffer, 0); // world_len
  serialization_serialize_u32(&buffer, 0); // point_lights_len
  
  // BIN_P("[serialization] serialized empty scene");

  // check if 'name' includes file ending
  int len = strlen(name);
  bool has_ending = name[len -6] == '.' && 
                    name[len -5] == 's' &&  
                    name[len -4] == 'c' &&  
                    name[len -3] == 'e' &&  
                    name[len -2] == 'n' &&  
                    name[len -1] == 'e';

  char path[128];
  sprintf(path, ASSET_PATH"/%s%s", name, has_ending ? "" : ".scene");
  BIN_PF("[bin] wrote empty scene to \"%s\"\n", path);
  file_write(path, (const char*)buffer, (int)arrlen(buffer));

  arrfree(buffer);
}


