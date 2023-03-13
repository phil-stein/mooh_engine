#ifndef INPUT_H
#define INPUT_H

#include "../global/global.h"

// @DOC: "input_state" maps directly to glfws key definitions
typedef enum input_state { STATE_RELEASED, STATE_PRESS } input_state;

// @DOC: modifications to key presses, f.e. holding control while pressing s
typedef enum modifier_type
{
  MOD_SHIFT          = FLAG(0), 
  MOD_CONTROL        = FLAG(1), 
  MOD_ALT            = FLAG(2), 
  MOD_SUPER          = FLAG(3), 
  MOD_CAPS_LOCK      = FLAG(4), 
  MOD_NUM_LOCK       = FLAG(5), 

}modifier_type;

// @DOC: maps directly to glfws key definitions
typedef enum
{
    KEY_UNKNOWN = -1,
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47, KEY_SUB = 47,
    KEY_ALPHA0 = 48,
    KEY_ALPHA1 = 49,
    KEY_ALPHA2 = 50,
    KEY_ALPHA3 = 51,
    KEY_ALPHA4 = 52,
    KEY_ALPHA5 = 53,
    KEY_ALPHA6 = 54,
    KEY_ALPHA7 = 55,
    KEY_ALPHA8 = 56,
    KEY_ALPHA9 = 57,
    KEY_SEMICOLON = 59,
    KEY_EQUAL = 61,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_LEFT_BRACKET = 91,
    KEY_BACKSLASH = 92,
    KEY_RIGHT_BRACKET = 93, KEY_ADD = 93,
    KEY_GRAVE_ACCENT = 96,
    KEY_WORLD1 = 161,
    KEY_WORLD2 = 162,
    KEY_ESCAPE = 256,
    KEY_ENTER = 257,
    KEY_TAB = 258,
    KEY_BACKSPACE = 259,
    KEY_INSERT = 260,
    KEY_DELETE = 261,
    KEY_RIGHT_ARROW = 262,
    KEY_LEFT_ARROW = 263,
    KEY_DOWN_ARROW = 264,
    KEY_UP_ARROW = 265,
    KEY_PAGE_UP = 266,
    KEY_PAGE_DOWN = 267,
    KEY_HOME = 268,
    KEY_END = 269,
    KEY_CAPS_LOCK = 280,
    KEY_SCROLL_LOCK = 281,
    KEY_NUM_LOCK = 282,
    KEY_PRINT = 283,
    KEY_PAUSE = 284,
    KEY_F1 = 290,
    KEY_F2 = 291,
    KEY_F3 = 292,
    KEY_F4 = 293,
    KEY_F5 = 294,
    KEY_F6 = 295,
    KEY_F7 = 296,
    KEY_F8 = 297,
    KEY_F9 = 298,
    KEY_F10 = 299,
    KEY_F11 = 300,
    KEY_F12 = 301,
    KEY_F13 = 302,
    KEY_F14 = 303,
    KEY_F15 = 304,
    KEY_F16 = 305,
    KEY_F17 = 306,
    KEY_F18 = 307,
    KEY_F19 = 308,
    KEY_F20 = 309,
    KEY_F21 = 310,
    KEY_F22 = 311,
    KEY_F23 = 312,
    KEY_F24 = 313,
    KEY_F25 = 314,
    KEY_NUMPAD0 = 320,
    KEY_NUMPAD1 = 321,
    KEY_NUMPAD2 = 322,
    KEY_NUMPAD3 = 323,
    KEY_NUMPAD4 = 324,
    KEY_NUMPAD5 = 325,
    KEY_NUMPAD6 = 326,
    KEY_NUMPAD7 = 327,
    KEY_NUMPAD8 = 328,
    KEY_NUMPAD9 = 329,
    KEY_NUMPAD_DECIMAL = 330,
    KEY_NUMPAD_DIVIDE = 331,
    KEY_NUMPAD_MULTIPLY = 332,
    KEY_NUMPAD_SUBTRACT = 333,
    KEY_NUMPAD_ADD = 334,
    KEY_NUMPAD_ENTER = 335,
    KEY_NUMPAD_EQUAL = 336,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,

    /// <summary> The key with the Mac, Windows, etc. Symbol. </summary>
    KEY_LEFT_SUPER = 343,
    /// <summary> 
    /// The key with the Mac, Windows, etc. Symbol. 
    /// <para>Same as <see cref="LeftSuper" />.</para>
    /// </summary>
    KEY_LEFT_WIN_MAC_SYMBOL = KEY_LEFT_SUPER,

    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,

    /// <summary> The key with the Mac, Windows, etc. Symbol. </summary>
    KEY_RIGHT_SUPER = 347,
    /// <summary> 
    /// The key with the Mac, Windows, etc. Symbol. 
    /// <para>Same as <see cref="RightSuper" />.</para>
    /// </summary>
    KEY_RIGHT_WIN_MAC_SYMBOL = KEY_RIGHT_SUPER,

    KEY_MENU = 348
}key_type;

// @DOC: maps directly to glfws key definitions
typedef enum
{
    MOUSE_BUTTON1 = 0,  // Same as Left
    MOUSE_BUTTON2 = 1,  // Same as Right
    MOUSE_BUTTON3 = 2,  // Same as Middle
    MOUSE_BUTTON4 = 3,
    MOUSE_BUTTON5 = 4,
    MOUSE_BUTTON6 = 5,
    MOUSE_BUTTON7 = 6,
    MOUSE_BUTTON8 = 7,
    MOUSE_LEFT   = MOUSE_BUTTON1, // Same as Button1
    MOUSE_RIGHT  = MOUSE_BUTTON2, // Same as Button2
    MOUSE_MIDDLE = MOUSE_BUTTON3  // Same as Button3
}mouse_btn_type;

// @DOC: initializes the input system, call this before any other calls to input
void input_init();
// @DOC: updatees the key states, etc. call every frame
void input_update();


// @DOC: returns the state of the key [KEY_PRESS, KEY_RELEASE]
input_state input_get_key_state(key_type _key);

// @DOC: returns true if the key is pressed, false it it is released
bool input_get_key_down(key_type _key);

// @DOC: returns true if the key is released, false it it is pressed
bool input_get_key_released(key_type _key);

// @DOC: check whether a key is pressed and not held
//       returns true the first frame the Key is held/pressed
bool input_get_key_pressed(key_type _key);

// @NOTE: neat idea, but kinda unnecessary
// checks multiple keys down with or logic opreator
// e.g. input_get_keys_down_or(2, KEY_A, KEY_B);
// count: amount of KEY_... enums
bool input_get_keys_down_or(u32 count, ...);

// @DOC: sets the key states for the last frame
//       window is type GLFWwindow*
//       !used internally
void input_key_callback(void* window, key_type _key, int scancode, input_state state, int mods);

// @DOC: gets the state of the key last frame 
bool input_get_last_key_state(key_type _key);


// @DOC: returns the state of the button [KEY_PRESS, KEY_RELEASED]
input_state input_get_mouse_state(mouse_btn_type btn);
// @DOC: returns true if the mouse-button is pressed, false it it is released
bool input_get_mouse_down(mouse_btn_type btn);
// @DOC: returns true if the mouse-button is released, false it it is pressed
bool input_get_mouse_released(mouse_btn_type btn);
// @DOC: check whether a mouse-button is pressed and not held
//       returns true the first frame the button is held/pressed
bool input_get_mouse_pressed(mouse_btn_type btn);
// @DOC: gets the state of the mouse-button last frame 
bool input_get_last_mouse_state(mouse_btn_type btn);
// @DOC: window is type GLFWwindow*
void input_mouse_callback(void* window, mouse_btn_type button, input_state state, int mods);


// @DOC: get the mouse's x position
f64 input_get_mouse_x();
// @DOC: get the mouse's y position
f64 input_get_mouse_y();
// @DOC: get the mouse's position
void input_get_mouse_pos(f64* x, f64* y);
// @DOC: get the distance the mouse moved on the x axis last frame
f64 input_get_mouse_delta_x();
// @DOC: get the distance the mouse moved on the y axis last frame
f64 input_get_mouse_delta_y();
// @DOC: get the distance the mouse moved on the x and y axis last frame
void input_get_mouse_delta(f64* x, f64* y);

// @DOC: puts the cursor in the middle of the window
void input_center_cursor_pos();
// @DOC: set if the cursor is visible
void input_set_cursor_visible_dbg(bool visible, const char* file, const int line);
#define input_set_cursor_visible(visible) input_set_cursor_visible_dbg((visible), __FILE__, __LINE__)

// @DOC: glfw callback used internally
//       window is type GLFWwindow*
//       !used internally
void input_mouse_pos_callback(void* window, double xpos, double ypos);

// @DOC: 
//       !used internally
void input_scroll_callback(void* window, double xpos, double ypos);

#endif
