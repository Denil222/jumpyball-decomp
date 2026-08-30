#include "jb_consts.h"
#include "jb_trackrow.h"

/* JumpyBall.exe TrackRow_DrawGrass 0x00019198: no __rt_sdiv and no __rt_udiv, the
   column pitch is the float 0x00019348 __divs(__itos(width), __itos(g_mapCols)),
   and the row gate is 0x00019370 "cmp r0,#0x64". */
void TrackRow_DrawGrass(const jb_trackrow_ctx *ctx, const jb_track_row *row)
{
    int   height   = row->y_near - row->y_far;
    float inv      = 1.0f / (float)height;
    float dx_left  = (float)(row->x_near_left - row->x_far_left) * inv;
    float dx_right = (float)(row->x_near_right - row->x_far_right) * inv;
    float x_left   = (float)row->x_far_left;
    float x_right  = (float)row->x_far_right;
    int   y_end    = row->y_near;
    int   blend    = 0;
    float span     = 0.0f;
    int   y;

    /* JumpyBall.exe TrackRow_DrawGrass 0x000192a8 "addeq r8,r8,#0x12c" and
       0x000192b4 "subgt r3,r10,#0x8". */
    if (row->row == 0)
        y_end += JB_NEAR_ROW_EXTRA;
    if (row->row > 5)
        blend = (row->row - 8) * 5;

    for (y = row->y_far; y < y_end; y++) {
        int ty = y - ctx->layout_mode;
        int c;

        span = (float)(int)(x_right - x_left) / (float)ctx->map_cols;

        if (ty < ctx->view_bottom + 1) {
            int step = jb_tex_vstep[(height + 2) * JB_TEX_VSTEP_STRIDE
                                    + (y - row->y_far)];

            if (row->row < JB_GRASS_KEYED_ROW_FIRST) {
                Blit_NoKey(ctx->screen, 0, ty, ctx->view_w, 1,
                           ctx->tex_grass_h, 0, step);
                Blit_NoKey(ctx->screen, 0, ty, (int)x_left, 1,
                           ctx->tex_grass_h, (int)(400.0f - x_left), step);
                Blit_NoKey(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           ctx->tex_grass_h, 0, step);
            } else {
                /* JumpyBall.exe TrackRow_DrawGrass 0x00019590 "sub r3,r0,#0x9". */
                int vlin = (row->row - 9) * 5 + (y - row->y_far);

                Blit_Keyed(ctx->screen, 0, ty, (int)x_left, 1,
                           ctx->tex_grass_h, blend, JB_NO_KEY,
                           (int)(400.0f - x_left), vlin);
                Blit_Keyed(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           ctx->tex_grass_h, blend, JB_NO_KEY, 0, vlin);
            }
            /* JumpyBall.exe TrackRow_DrawGrass 0x00019720 "ldrb r3,[r11,r0]"
               against g_rowTiles 0x000613b8. */
            for (c = 0; c < ctx->map_cols; c++) {
                float x_base = (float)c * span + (float)(int)x_left;

                if (row->tiles[c] != 0)
                    Blit_TileH(ctx->screen, (int)(x_base - 1.0f), ty,
                               (int)(span + 2.0f), step, ctx->tex_wood,
                               JB_NO_KEY);
                else if (row->row < JB_GRASS_KEYED_ROW_FIRST)
                    Blit_TileH(ctx->screen, (int)(x_base - 1.0f), ty,
                               (int)(span + 2.0f), step, ctx->tex_grass,
                               JB_NO_KEY);
                else
                    Blit_TileH_Ofs(ctx->screen, (int)x_base, ty,
                                   (int)(span + 1.0f), step, ctx->tex_grass,
                                   JB_NO_KEY, blend);
            }
        }

        x_left  += dx_left;
        x_right += dx_right;

        /* JumpyBall.exe TrackRow_DrawGrass 0x00019b5c __lts(xLeft, 0.0) and
           0x00019bb0 __gts(xRight + 1.0, __itos(g_viewW)) reach
           0x00019bbc "ldrne r5,[sp,#0x68]". */
        if (row->row == 0 && ty > ctx->view_bottom && x_left < 0.0f
            && x_right + 1.0f > (float)ctx->view_w)
            y = y_end;
    }

    /* JumpyBall.exe TrackRow_DrawGrass 0x00019c34 __lts(__subs(__itos(y -
       g_layoutMode), span), __itos(g_viewBottom + 1)), 0x00019c4c "add r3,r0,r10;
       cmp r3,#0x4", 0x00019cbc __muld against 0x4000000000000000 and 0x00019d9c
       __muld against 0x4010000000000000. */
    if ((float)(y - ctx->layout_mode) - span < (float)(ctx->view_bottom + 1)
        && ctx->cam_row + row->row == 4)
        Blit_TileHV(ctx->screen,
                    (int)((double)span * 4.0 + (double)x_left),
                    (int)((float)y - span + 1.0f - (float)ctx->layout_mode),
                    (int)((double)span * 2.0 + 1.0),
                    (int)(span + 1.0f),
                    ctx->spr_sign_keep_off, ctx->sign_key);
}
