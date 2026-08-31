#ifndef JB_KEYCONFIG_H
#define JB_KEYCONFIG_H

#include "jb_gfx.h"

typedef struct {
    const jb_surface *screen;
    const jb_sprite  *panel;

    unsigned key;

    int view_w;
    int view_h;
    int view_center_x;

    int active;
    int step;
} jb_keyconfig;

void KeyConfig_Load(void);

/* jumpyball JumpyBall.exe WndProc 0x0001fd2c, the settings screen index 2
   branch: it stores -10 into g_keyLeft, g_keyRight, g_keyUp, g_keyDown,
   g_keyJump and g_keyMenu, clears g_keyConfigStep and sets g_inKeyConfig. */
void KeyConfig_Begin(jb_keyconfig *kc);

/* jumpyball JumpyBall.exe WndProc 0x0001fd2c rejects a code that already
   equals one of the six bindings without advancing g_keyConfigStep, assigns
   step 0..5 to left, right, up, down, jump, menu, and clears g_inKeyConfig
   once g_keyConfigStep reaches 6. */
int KeyConfig_KeyDown(jb_keyconfig *kc, int code);

/* jumpyball JumpyBall.exe KeyConfig_Tick 0x0001f900. */
void KeyConfig_DrawFrame(const jb_keyconfig *kc);

#endif /* JB_KEYCONFIG_H */
