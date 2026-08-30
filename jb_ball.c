#include "jb_ball.h"

/* JumpyBall.exe Track_DrawFrame 0x0001ed64..0x0001edd0 and 0x0001eb24..0x0001ebbc:
   __itod(g_layoutMode), __muld 1.5, __addd(g_viewBottom), __subd 110.0. */
static double BaseY(const jb_ball_state *st)
{
    return (double)st->layout_mode * JB_BALL_LAYOUT_MUL +
           (double)st->view_bottom - JB_BALL_Y_BIAS;
}

/* JumpyBall.exe Track_DrawFrame 0x0001eacc __stod(g_ballY) then __ged 0x0001eaf8
   against 0.0, branching over the whole shadow block to 0x0001ecb8. */
static void DrawShadow(const jb_ball_state *st)
{
    double y;

    if (!((double)st->ball_y >= 0.0))
        return;

    /* JumpyBall.exe Track_DrawFrame 0x0001ebc0..0x0001ec88: __addd(velZ*0.5),
       __addd(ballY*6.0), __addd(40.0), __dtoi. */
    y = BaseY(st) + (double)st->ball_vel_z * JB_BALL_VELZ_MUL +
        (double)st->ball_y * JB_SHADOW_Y_MUL + JB_SHADOW_Y_BIAS;

    /* JumpyBall.exe Track_DrawFrame 0x0001eb18..0x0001ecb4: [sp,#0x8..0x18] all
       zero, so the tint and the source origin are 0. */
    Blit_Glyph(st->screen, st->view_center_x - JB_SHADOW_X_OFS, (int)y,
               JB_SHADOW_W, JB_SHADOW_H, st->spr_shadow, 0, 0, 0, 0, 0);
}

/* JumpyBall.exe Track_DrawFrame 0x0001ecb8..0x0001ef10. */
void Ball_DrawFrame(const jb_ball_state *st)
{
    double y;
    int    frame, src_x;

    DrawShadow(st);

    /* JumpyBall.exe Track_DrawFrame 0x0001ecc0..0x0001ed1c: __itod(g_ballSpin),
       __muld 0.03125, __dtoi, mul 0x46, then __rt_sdiv(0x276, .) whose remainder
       is stored at [sp,#0x14] as the source x. */
    frame = (int)((double)st->ball_spin * JB_BALL_SPIN_RATE);
    src_x = (frame * JB_BALL_CELL) % (JB_BALL_FRAMES * JB_BALL_CELL);

    /* JumpyBall.exe Track_DrawFrame 0x0001ede4..0x0001eee4: __itos(g_layoutMode)
       widened by __stod, __subd from 66.0, __muld by g_ballY, __subd from the
       base, then __addd(velZ*0.5) and __dtoi. */
    y = BaseY(st) -
        (JB_BALL_Y_SCALE - (double)(float)st->layout_mode) * (double)st->ball_y +
        (double)st->ball_vel_z * JB_BALL_VELZ_MUL;

    Blit_Glyph(st->screen, st->view_center_x - JB_BALL_X_OFS, (int)y,
               JB_BALL_DRAW_SIZE, JB_BALL_DRAW_SIZE, st->spr_ball_frames,
               st->hud_r, st->hud_g, st->hud_b, src_x, 0);
}
