#ifndef JB_BALL_H
#define JB_BALL_H

#include "jb_consts.h"
#include "jb_gfx.h"

/* JumpyBall.exe Track_DrawFrame 0x0001dd54 tail, 0x0001eacc..0x0001ef10. */
typedef struct {
    const jb_surface *screen;          /* g_screen 0x00061b10 */
    const jb_sprite  *spr_shadow;      /* g_sprShadow 0x00061cb8 */
    const jb_sprite  *spr_ball_frames; /* g_sprBallFrames 0x00061c78 */
    int   hud_r;                       /* g_hudR 0x00026280 */
    int   hud_g;                       /* g_hudG 0x00026284 */
    int   hud_b;                       /* g_hudB 0x00026288 */
    int   layout_mode;                 /* g_layoutMode 0x00064938 */
    int   view_bottom;                 /* g_viewBottom 0x0002625c */
    int   view_center_x;               /* g_viewCenterX 0x00026268 */
    float ball_y;                      /* g_ballY 0x00064b10 */
    float ball_vel_z;                  /* g_ballVelZ 0x000649cc */
    int   ball_spin;                   /* g_ballSpin 0x00064994 */
} jb_ball_state;

void Ball_DrawFrame(const jb_ball_state *st);

#endif
