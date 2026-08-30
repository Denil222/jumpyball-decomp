#include <stdlib.h>

#include "jb_consts.h"
#include "jb_trackrow.h"

/* JumpyBall.exe TrackRow_DrawSky 0x000167bc: no base or band blits, only the
   per-column tile loop, and only where the tile byte is non-zero
   ("cmp r3,#0x0; beq 0x00016b3c" at 0x00016984). */
void TrackRow_DrawSky(const jb_trackrow_ctx *ctx, const jb_track_row *row)
{
    int    height   = row->y_near - row->y_far;
    float  inv      = 1.0f / (float)height;
    float  dx_left  = (float)(row->x_near_left - row->x_far_left) * inv;
    float  dx_right = (float)(row->x_near_right - row->x_far_right) * inv;
    float  x_left   = (float)row->x_far_left;
    float  x_right  = (float)row->x_far_right;
    int    y_end    = row->y_near;
    int    blend    = 0;
    int    width    = 0;
    int    tile_w, glow_y, y;
    double spread;

    /* JumpyBall.exe TrackRow_DrawSky 0x000168c4 "addeq r3,r10,#0x12c", then
       "movgt r2,#0x5" at 0x000168e0. */
    if (row->row == 0)
        y_end += JB_NEAR_ROW_EXTRA;
    if (row->row > 5)
        blend = (row->row - 9) * 5;

    /* JumpyBall.exe TrackRow_DrawSky 0x000168f0 bl 0x0002451c srand and
       0x000168f4 bl 0x000244d4 rand, whose r0 is overwritten at 0x000168f8. */
    srand((unsigned)(ctx->cam_row + row->row));
    rand();

    for (y = row->y_far; y < y_end; y++) {
        int ty = y - ctx->layout_mode;

        width = (int)(x_right - x_left);

        if (ty < ctx->view_bottom + 1) {
            int step = jb_tex_vstep[(height + 2) * JB_TEX_VSTEP_STRIDE
                                    + (y - row->y_far)];
            int c, acc;

            /* JumpyBall.exe TrackRow_DrawSky 0x000169c0: __rt_sdiv(g_mapCols, k)
               and __rt_sdiv(g_mapCols, k + width) with k stepped by width. */
            for (c = 0, acc = 0; c < ctx->map_cols; c++, acc += width) {
                int x0, x1;

                if (row->tiles[c] == 0)
                    continue;

                x0 = acc / ctx->map_cols;
                x1 = (acc + width) / ctx->map_cols;

                if (row->row < JB_KEYED_ROW_FIRST)
                    Blit_TileH(ctx->screen, x0 + (int)x_left, ty,
                               (x1 - x0) + 1, step, ctx->tex_sky_tile,
                               JB_NO_KEY);
                else
                    Blit_TileH_Ofs(ctx->screen, x0 + (int)x_left, ty,
                                   (x1 - x0) + 1, step, ctx->tex_sky_tile,
                                   JB_NO_KEY, blend);
            }
        }

        x_left  += dx_left;
        x_right += dx_right;

        /* JumpyBall.exe TrackRow_DrawSky 0x00016bbc __lts(xLeft, 0.0) and
           0x00016c10 __gts(xRight + 1.0, __itos(g_viewW)) reach
           0x00016c18 "ldrne r6,[sp,#0x64]", followed by
           0x00016c1c "add r6,r6,#0x1": y is y_end + 1 at the glow blits. */
        if (row->row == 0 && ty > ctx->view_bottom && x_left < 0.0f
            && x_right + 1.0f > (float)ctx->view_w)
            y = y_end;
    }

    /* JumpyBall.exe TrackRow_DrawSky 0x00016c40 __rt_sdiv(g_mapCols, width),
       then __itod(width) and __muld against 0x4008000000000000 at 0x00016c74,
       reused by both blits from [sp,#0x2c]. */
    tile_w = width / ctx->map_cols;
    glow_y = (y - tile_w) - ctx->layout_mode + 1;
    spread = (double)width * JB_GLOW_SPREAD / (double)ctx->map_cols;

    /* JumpyBall.exe TrackRow_DrawSky 0x00016d40 and 0x00016e28 bl 0x00023b08. */
    Blit_TileHV_Glyph(ctx->screen, (int)((double)x_left - spread), glow_y,
                      tile_w, tile_w, ctx->spr_glow, JB_GLOW_TINT,
                      JB_GLOW_TINT, JB_GLOW_TINT);
    Blit_TileHV_Glyph(ctx->screen, (int)(spread + (double)x_right), glow_y,
                      tile_w, tile_w, ctx->spr_glow, JB_GLOW_TINT,
                      JB_GLOW_TINT, JB_GLOW_TINT);
}
