#ifdef  EDITOR

#include "game/gui.h"
#include "core/window.h"
#include "core/renderer.h"
#include "core/types/framebuffer.h"

#include "GLAD/glad.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"


#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024


// ---- vars ----
int window_w;
int window_h;

ui_color red = { 255, 0, 0, 255 };

ui_glfw glfw = { 0 };
int width = 0, height = 0;
ui_context* ctx;
ui_colorf bg;

nk_flags window_flags = NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE;


void gui_init()
{
    // ---- nuklear ----
  ctx = nk_glfw3_init(&glfw, window_get(), NK_GLFW3_INSTALL_CALLBACKS);
  /* Load Fonts: if none of these are loaded a default font will be used  */
  /* Load Cursor: if you uncomment cursor loading please hide the cursor */
  {struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);

    struct nk_font *droid = nk_font_atlas_add_from_file(atlas, ASSET_PATH"fonts\\DroidSans.ttf", 18/*16*//*14*/, 0);

    nk_glfw3_font_stash_end(&glfw);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/

    nk_style_set_font(ctx, &droid->handle);
  }
}

void gui_update()
{
  // get window size
  window_get_size(&window_w, &window_h);

  // draw ui
  nk_glfw3_new_frame(&glfw);

  // window test --------------------------------------------------------------------------------------
  int w, h;
  window_get_size(&w, &h);

  // less height because the window bar on top and below
  const f32 w_ratio = 400.0f  / 1920.0f;
  const f32 h_ratio = 1000.0f  / 1020.0f;
  const f32 x_ratio = 10.0f / 1920.0f;
  const f32 y_ratio = 10.0f  / 1020.0f;

  ui_rect frameb_window_rect = nk_rect(x_ratio * w, y_ratio * h, w_ratio * w, h_ratio * h);
  if (nk_begin(ctx, "framebuffers", frameb_window_rect, window_flags | NK_WINDOW_NO_SCROLLBAR)) 
    // cant have these two because the windoews cant be resized otherwise NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE
  {
    u32 color, material, normal, position, lighting, mouse_pick;
    renderer_get_framebuffers(&color, &material, &normal, &position, &lighting, &mouse_pick);

    int w, h;
    window_get_size(&w, &h);
    float ratio = (float)w / (float)h;
    const int size = 190; // 245;
    nk_layout_row_static(ctx, size, size * ratio, 1);
    nk_image(ctx, nk_image_id(color));
    nk_image(ctx, nk_image_id(material));
    nk_image(ctx, nk_image_id(normal));
    nk_image(ctx, nk_image_id(position));
    nk_image(ctx, nk_image_id(lighting));
  }
  nk_end(ctx);

  // --------------------------------------------------------------------------------------------------

  nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void gui_cleanup()
{
  nk_clear(ctx);
}

#endif
