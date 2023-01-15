#include "global/global.h"
#include "math/math_inc.h"
#include "core/serialization.h"
#include "core/file_io.h"

#define STB_DS_IMPLEMENTATION // only define once
#include "stb/stb_ds.h"

#include <ctype.h>  // isspace(), etc.

// @NOTE: in mooh defined in makefile 
#define ASSET_PATH "../../assets"

#define ARG_MAX_LEN 56

// @NOTE: undef to hide  
#define BIN_P(a) PF(a)

// -- func decls --
void write_empty_scene_to_file(const char* name);

int main(int argc, char** args)
{
  // @NOTE: PF because always need to be printed
  if (argc <= 1) { PF("no argument given\n -scene_empty \"name\"\n"); return 1; }

  for (u32 i = 0; i < argc; ++i)
  {
    if (args[i][0] == '-')
    {
      if (args[i][1]  == 's' &&
          args[i][2]  == 'c' &&
          args[i][3]  == 'e' &&
          args[i][4]  == 'n' &&
          args[i][5]  == 'e' &&
          args[i][6]  == '_' &&
          args[i][7]  == 'e' &&
          args[i][8]  == 'm' &&
          args[i][9]  == 't' &&
          args[i][10] == 'y' )
      {
        // skip whitespace
        u32 len = strlen(args[i]);
        u32 j = 11;
        while (j < len && isspace(args[i][j])) { j++; }
        if (args[i][j] == '"')
        {
          j++; // skip '"'
          char name[ARG_MAX_LEN];
          u32  name_pos = 0;
          while (j < len && name_pos < ARG_MAX_LEN && args[i][j] != '"')
          {
            name[name_pos++] = args[i][j++];
          }
          name[++name_pos] = '\0';  // terminate string

          write_empty_scene_to_file(name); //("-_-_new_-_-");
        }
      }
    }
  }


	return 0;
}

void write_empty_scene_to_file(const char* name)
{
  u8* buffer = NULL;

  // serialization_serialize_scene(&buffer);
  serialization_serialize_u32(&buffer, 0); // dir_lights_len
  serialization_serialize_u32(&buffer, 0); // point_lights_len
  serialization_serialize_u32(&buffer, 0); // world_len
  
  BIN_P("[serialization] serialized empty scene");

  char path[128];
  sprintf(path, ASSET_PATH"%s", name);
  file_write(path, (const char*)buffer, (int)arrlen(buffer));

  arrfree(buffer);
}


