#ifndef JB_MENU_H
#define JB_MENU_H

#include "jb_gfx.h"

/* JumpyBall.exe Menu_DrawFrame 0x0001a7d0 and WndProc 0x0001fd2c, the
   g_appMode 0x00064a24 == 1 branch. */
#define JB_SCREEN_MAIN     0
#define JB_SCREEN_LEVELS   1
#define JB_SCREEN_ABOUT    2
#define JB_SCREEN_SETTINGS 4

typedef struct {
    const jb_surface *screen;

    const jb_sprite *backdrop;      /* g_backdropMenu 0x00061b48 */
    const jb_sprite *bar_thin;      /* g_sprBarThin */
    const jb_sprite *panel;         /* g_sprPanelGradient */
    const jb_sprite *title;         /* g_sprTitleJumpyBall */
    const jb_sprite *button_narrow; /* g_sprButtonNarrow */
    const jb_sprite *button_wide;   /* g_sprButtonWide */
    const jb_sprite *logo_small;    /* g_sprLogoPocketNewSmall */

    unsigned key; /* DAT_00064b18 */

    int view_w;
    int view_h;
    int view_center_x;
    int layout_mode;

    int screen_id;    /* g_screen 0x00064a20 */
    int index;        /* g_menuIndex 0x00064a28 */
    int max_unlocked; /* g_maxLevelUnlocked */
    int auto_jump;    /* g_autoJump 0x00026430 */
} jb_menu;

/* JumpyBall.exe WndProc 0x0001fd2c menu branch: the confirm keys leave through
   Level_Begin 0x0001376c, App_Quit or Screen_Set 0x00013678. */
enum {
    JB_MENU_NONE,
    JB_MENU_PLAY,
    JB_MENU_QUIT
};

/* JumpyBall.exe Screen_Set 0x00013678 stores a to g_screen and b to
   g_menuIndex, and calls Font_Load 0x0001f4d8 with 1. */
void Menu_ScreenSet(jb_menu *m, int screen_id, int index);

/* JumpyBall.exe Menu_DrawFrame 0x0001a7d0. */
void Menu_DrawFrame(const jb_menu *m);

/* JumpyBall.exe WndProc 0x0001fd2c, the g_appMode == 1 key branch. */
int Menu_KeyDown(jb_menu *m, int key);

#endif /* JB_MENU_H */
