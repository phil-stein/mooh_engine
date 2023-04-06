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
#include "editor/app.h"

#include "core/window.h"
#include "core/io/file_io.h"
#include "core/io/save_sys.h"


static app_data_t* app_data = NULL;
static ui_context* ctx      = NULL;

void gui_struct_browser_display_struct(const char* path, const char* name)
{ 
  ASSERT(app_data != NULL);
  ASSERT(ctx != NULL);
  // nk_labelf(ctx, NK_TEXT_LEFT, "path: %s", path);
  // nk_labelf(ctx, NK_TEXT_LEFT, "name: %s", name);
  
  char name_no_ext[64];
  strcpy(name_no_ext, name);
  name_no_ext[strlen(name) - strlen(".struct")] = '\0'; // castrate extension
  if(nk_button_label(ctx, name_no_ext))
  {
    int id = save_sys_load_structure_from_file(name_no_ext);
    app_data->selected_id = id;
    // P_INT(id);
    // P_INT(app_data->selected_id);
  }
}

void gui_struct_browser_win(ui_context* _ctx, ui_rect win_rect, const u32 win_flags)
{
  app_data = app_data_get();
  ctx = _ctx;
  
  if (nk_begin(ctx, "struct_browser", win_rect, win_flags)) 
  {
    nk_layout_row_dynamic(ctx, 20, 1);

    file_io_search_dir_for_file_type(ASSET_PATH"structures", ".struct", gui_struct_browser_display_struct);

  }
  nk_end(ctx);
}

