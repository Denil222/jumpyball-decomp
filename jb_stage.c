#include "jb_stage.h"

#include "jb_consts.h"

/* JumpyBall.exe Level_Begin 0x0001376c at 0x000138a4: g_theme = g_menuIndex,
   forced 0 while g_altTrackMode 0x00064ab4 is positive, then 2 -> 4 and
   3 -> 1, so levels 0..2 reach themes 0, 1 and 4. */
static int Stage_Theme(int level)
{
    int theme = level;

    if (theme == 2)
        theme = 4;
    if (theme == 3)
        theme = 1;
    return theme;
}

/* JumpyBall.exe Level_Begin 0x0001376c LoadBitmapW arguments: theme 0 -> 0x141,
   1 -> 0x13c, 2 -> 0x142, 3 -> 0x144, 4 -> 0x141. */
int Stage_BackdropRes(int theme)
{
    if (theme == 1)
        return JB_RES_BACKDROP_DESERT;
    if (theme == 2)
        return JB_RES_BACKDROP_FOREST;
    if (theme == 3)
        return JB_RES_BACKDROP_SKY;
    return JB_RES_BACKDROP_ICE;
}

void Stage_Begin(jb_stage *st, int level, unsigned ticks)
{
    if (level < 0)
        level = 0;
    if (level >= JB_LEVEL_N)
        level = JB_LEVEL_N - 1;

    st->level        = level;
    st->theme        = Stage_Theme(level);
    st->backdrop_res = Stage_BackdropRes(st->theme);

    st->hud_r = st->hud_g = st->hud_b = (st->theme == 0) ? 0 : JB_HUD_BRIGHT;

    st->time_sec    = -1;
    st->time_min    = 0;
    st->start_ticks = ticks;
    st->anim_ms     = 0;
}

void Stage_Step(jb_stage *st, unsigned ticks)
{
    st->anim_ms = ticks - st->start_ticks;
    if (st->anim_ms <= 1000u)
        return;
    st->time_sec++;
    if (st->time_sec == 0x3c) {
        st->time_sec = 0;
        st->time_min++;
    }
    st->start_ticks = ticks;
}

int Stage_Frozen(const jb_stage *st)
{
    return st->time_sec < 0;
}

int Stage_Cleared(const jb_stage *st, int cam_row)
{
    (void)st;
    return cam_row > JB_LEVEL_LAST_ROW;
}
