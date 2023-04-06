#ifndef GUI_STYLE
#define GUI_STYLE

#include "global/global.h"
// #include "editor/gui.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"

#define COLOR_BUTTON_ACTIVE_ENABLED   nk_rgba(175, 175, 175, 255)
#define COLOR_BUTTON_HOVER_ENABLED    nk_rgba(175, 175, 175, 255)
#define COLOR_BUTTON_NORMAL_ENABLED   nk_rgba(175, 175, 175, 255)
#define COLOR_BUTTON_ACTIVE_DISABLED  nk_rgba(125, 125, 125, 255)
#define COLOR_BUTTON_HOVER_DISABLED   nk_rgba(125, 125, 125, 255)
#define COLOR_BUTTON_NORMAL_DISABLED  nk_rgba(125, 125, 125, 255)

INLINE void gui_style_set_button_color(struct nk_context* ctx, bool active)
{
  ctx->style.button.text_active = active ? COLOR_BUTTON_ACTIVE_ENABLED : COLOR_BUTTON_ACTIVE_DISABLED;
  ctx->style.button.text_hover  = active ? COLOR_BUTTON_HOVER_ENABLED  : COLOR_BUTTON_HOVER_DISABLED;
  ctx->style.button.text_normal = active ? COLOR_BUTTON_NORMAL_ENABLED : COLOR_BUTTON_NORMAL_DISABLED;
}

#endif
