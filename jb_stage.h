#ifndef JB_STAGE_H
#define JB_STAGE_H

/* JumpyBall.exe Level_Begin 0x0001376c resets the run and picks the theme, the
   backdrop bitmap and the HUD colour; WndProc 0x00020920 ends the run. */

/* JumpyBall.exe Level_LoadTileMap 0x0001261c tests level 0, 1 and >1 against
   g_levelMap1 0x00061c74, g_levelMap2 0x00061cb0 and g_levelMap3 0x00061cbc. */
#define JB_LEVEL_N 3

/* JumpyBall.exe WndProc 0x00020920 "cmp r0,#0x3fe" with bgt, then Sleep(1000)
   before Screen_Set 0x00013678. */
#define JB_LEVEL_LAST_ROW       0x3fe
#define JB_LEVEL_CLEAR_PAUSE_MS 1000

typedef struct {
    int level;        /* g_menuIndex 0x00064a28 */
    int theme;        /* g_theme 0x00064944 */
    int backdrop_res;

    /* JumpyBall.exe Level_Begin 0x0001376c stores 0 to g_hudR 0x00026280,
       g_hudG 0x00026284 and g_hudB 0x00026288 while g_theme is 0, else 0xfa. */
    int hud_r;
    int hud_g;
    int hud_b;

    /* JumpyBall.exe Level_Begin 0x0001376c stores -1 to g_timeSec 0x00064a44
       and 0 to g_timeMin 0x00064a48. */
    int      time_sec;
    int      time_min;
    unsigned start_ticks; /* g_levelStartTicks 0x00064960 */
    unsigned anim_ms;     /* g_animMs 0x00064968 */

    /* JumpyBall.exe WndProc 0x00020920 raises g_maxLevelUnlocked to
       g_menuIndex + 1 once g_camRow passes 0x3fe. */
    int max_unlocked;
} jb_stage;

/* JumpyBall.exe Level_Begin 0x0001376c, the stores at 0x00013780..0x000138f0. */
void Stage_Begin(jb_stage *st, int level, unsigned ticks);

/* JumpyBall.exe WndProc 0x00020a30 takes g_animMs as GetTickCount() -
   g_levelStartTicks and rolls the clock at 1000 ms and 0x3c. */
void Stage_Step(jb_stage *st, unsigned ticks);

/* JumpyBall.exe WndProc 0x00020934 "cmp r0,#0x0" with blt jumps straight to
   LAB_00020940 Game_DrawFrame, skipping input, physics and the camera. */
int Stage_Frozen(const jb_stage *st);

/* JumpyBall.exe WndProc 0x00020920 "cmp r0,#0x3fe" with bgt. */
int Stage_Cleared(const jb_stage *st, int cam_row);

/* JumpyBall.exe Level_Begin 0x0001376c LoadBitmapW argument per g_theme. */
int Stage_BackdropRes(int theme);

#endif /* JB_STAGE_H */
