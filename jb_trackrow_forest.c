#include <stdlib.h>

#include "jb_consts.h"
#include "jb_trackrow.h"

/* JumpyBall.exe TrackRow_DrawForest 0x00017ad4: integer __rt_sdiv column
   stepping like the ice/desert core, the horizontal strips drawn AFTER the
   columns, no full-width strip, and the wood path at 0x00017cf0 indexing
   g_texVStep with (height + 1) * 300 where every other path adds one more. */
void TrackRow_DrawForest(const jb_trackrow_ctx *ctx, const jb_track_row *row)
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

    /* JumpyBall.exe TrackRow_DrawForest 0x00017be4 "addeq r3,r9,#0x12c" and
       0x00017bf8 "subgt r3,r11,#0x8". */
    if (row->row == 0)
        y_end += JB_NEAR_ROW_EXTRA;
    if (row->row > 5)
        blend = (row->row - 8) * 5;

    for (y = row->y_far; y < y_end; y++) {
        int width = (int)(x_right - x_left);
        int ty    = y - ctx->layout_mode;
        int c, acc;

        span = (float)width / (float)ctx->map_cols;

        if (ty < ctx->view_bottom + 1) {
            int step  = jb_tex_vstep[(height + 2) * JB_TEX_VSTEP_STRIDE
                                     + (y - row->y_far)];
            int wstep = jb_tex_vstep[(height + 1) * JB_TEX_VSTEP_STRIDE
                                     + (y - row->y_far)];
            /* JumpyBall.exe TrackRow_DrawForest 0x00018168 "sub r3,r0,#0x9". */
            int vlin  = (row->row - 9) * 5 + (y - row->y_far);

            /* JumpyBall.exe TrackRow_DrawForest 0x00017cbc "ldrb r3,[r11,r0]"
               against g_rowTiles 0x000613b8, 0x00017f7c "add r11,r11,r3". */
            for (c = 0, acc = 0; c < ctx->map_cols; c++, acc += width) {
                int x0 = acc / ctx->map_cols;
                int x1 = (acc + width) / ctx->map_cols;

                if (row->tiles[c] != 0)
                    Blit_TileH(ctx->screen, x0 + (int)x_left, ty,
                               (x1 - x0) + 1, wstep, ctx->tex_wood, JB_NO_KEY);
                else if (row->row < JB_KEYED_ROW_FIRST)
                    Blit_TileH(ctx->screen, x0 + (int)x_left, ty,
                               (x1 - x0) + 1, step, ctx->tex_forest_v,
                               JB_NO_KEY);
                else
                    Blit_TileH_Ofs(ctx->screen, x0 + (int)x_left, ty,
                                   (x1 - x0) + 1, step, ctx->tex_forest_v,
                                   JB_NO_KEY, blend);
            }

            /* JumpyBall.exe TrackRow_DrawForest 0x00018034 / 0x00018130 /
               0x000181fc / 0x000182f4, all after the column loop. */
            if (row->row < JB_KEYED_ROW_FIRST) {
                Blit_NoKey(ctx->screen, 0, ty, (int)x_left, 1,
                           ctx->tex_forest_h, (int)(400.0f - x_left), step);
                Blit_NoKey(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           ctx->tex_forest_h, 0, step);
            } else {
                Blit_Keyed(ctx->screen, 0, ty, (int)x_left, 1,
                           ctx->tex_forest_h, blend, JB_NO_KEY,
                           (int)(400.0f - x_left), vlin);
                Blit_Keyed(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           ctx->tex_forest_h, blend, JB_NO_KEY, 0, vlin);
            }
        }

        x_left  += dx_left;
        x_right += dx_right;

        /* JumpyBall.exe TrackRow_DrawForest 0x00018370 __lts(xLeft, 0.0) and
           0x000183c4 __gts(xRight + 1.0, __itos(g_viewW)) reach
           0x000183d0 "ldrne r8,[sp,#0x74]". */
        if (row->row == 0 && ty > ctx->view_bottom && x_left < 0.0f
            && x_right + 1.0f > (float)ctx->view_w)
            y = y_end;
    }

    /* JumpyBall.exe TrackRow_DrawForest 0x000183f0 srand(g_camRow + row), then
       0x00018484 / 0x000189e4 __lts(__subs(__itos(y - g_layoutMode),
       __muls(span, 4.0)), __itos(g_viewBottom + 1)). */
    srand(ctx->cam_row + row->row);

    if ((float)(y - ctx->layout_mode) - span * 4.0f
        < (float)(ctx->view_bottom + 1)) {
        /* JumpyBall.exe TrackRow_DrawForest 0x000184f8 __muld 0x4010000000000000
           and 0x00018560 __muld 0x4008000000000000. */
        int    h    = (int)((double)span * 4.0 + 1.0);
        int    w    = (int)((double)span * 3.0 + 1.0);
        double base = (double)y - (double)span * 4.0 + 1.0;
        int    tty  = (int)(base - (double)ctx->layout_mode);
        int    i;

        for (i = 0; i < 4; i++) {
            /* JumpyBall.exe TrackRow_DrawForest 0x00018734 __rt_sdiv(6, rand())
               takes the remainder in r1. */
            int r6 = rand() % 6;
            int x;

            if ((i & 1) == 0)
                x = (int)((double)(x_left - (float)r6 * span)
                          - (double)span * 3.0);
            else
                x = (int)((float)r6 * span + x_right);

            /* JumpyBall.exe TrackRow_DrawForest 0x000186b8 Blit_TileHV against
               0x00018c68 Blit_TileHV_Ofs. */
            if (row->row < JB_KEYED_ROW_FIRST)
                Blit_TileHV(ctx->screen, x, tty, w, h, ctx->spr_tree,
                            ctx->sign_key);
            else
                Blit_TileHV_Ofs(ctx->screen, x, tty, w, h, ctx->spr_tree,
                                ctx->sign_key, blend);
        }
    }

    /* JumpyBall.exe TrackRow_DrawForest 0x00018fa8 __lts(__subs(__itos(y -
       g_layoutMode), span), __itos(g_viewBottom + 1)), 0x00018fc0 "add r3,r0,r3;
       cmp r3,#0x4", 0x00019030 __muld against 0x4000000000000000 and 0x00019110
       __muld against 0x4014000000000000. */
    if ((float)(y - ctx->layout_mode) - span < (float)(ctx->view_bottom + 1)
        && ctx->cam_row + row->row == 4)
        Blit_TileHV(ctx->screen,
                    (int)((double)span * 5.0 + (double)x_left),
                    (int)((float)y - span + 1.0f - (float)ctx->layout_mode),
                    (int)((double)span * 2.0 + 1.0),
                    (int)(span + 1.0f),
                    ctx->spr_sign_keep_off, ctx->sign_key);
}
