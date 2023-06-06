#ifndef EDITOR_SAVE_H
#define EDITOR_SAVE_H


#define EDITOR_SAVE_NAME_START  editor_info__v
#define EDITOR_SAVE_VERSION     0_1
#define EDITOR_SAVE_EXTENSION   ".editor_info"
#define EDITOR_SAVE_NAME_X      PASTE_2(EDITOR_SAVE_NAME_START, EDITOR_SAVE_VERSION)
#define EDITOR_SAVE_NAME        EXPAND_TO_STR(EDITOR_SAVE_NAME_X)


void editor_save_init();

void editor_save_info();
void editor_load_info();

#endif // EDITOR_SAVE_H
