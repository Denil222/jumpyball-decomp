#include "jb_consts.h"
#include "jb_trackrow.h"

/* JumpyBall.exe TrackRow_DrawIce 0x00017060 / TrackRow_DrawDesert 0x00015d48
   __rt_udiv(0xfa, g_animMs), then "cmp r1,#0x7d" on the remainder at
   0x00017068 / 0x00015d50. */
static int AnimPhase(int anim_ms)
{
    return ((unsigned)anim_ms % JB_TEX_ANIM_PERIOD) > JB_TEX_ANIM_HALF;
}

static void DrawTiledRow(const jb_trackrow_ctx *ctx, const jb_track_row *row,
                         const jb_sprite *tex_h, const jb_sprite *tex_v,
                         const jb_sprite *tex_tile)
{
    int   height   = row->y_near - row->y_far;
    float inv      = 1.0f / (float)height;
    float dx_left  = (float)(row->x_near_left - row->x_far_left) * inv;
    float dx_right = (float)(row->x_near_right - row->x_far_right) * inv;
    float x_left   = (float)row->x_far_left;
    float x_right  = (float)row->x_far_right;
    int   anim     = AnimPhase(ctx->anim_ms);
    int   y_end    = row->y_near;
    int   blend    = 0;
    int   y;

    /* JumpyBall.exe TrackRow_DrawIce 0x00016f9c / TrackRow_DrawDesert 0x00015c84
       "addeq r3,r9,#0x12c", then "movgt r2,#0x5" at 0x00016fb0 / 0x00015c98. */
    if (row->row == 0)
        y_end += JB_NEAR_ROW_EXTRA;
    if (row->row > 5)
        blend = (row->row - 9) * 5;

    for (y = row->y_far; y < y_end; y++) {
        int width = (int)(x_right - x_left);
        int ty    = y - ctx->layout_mode;

        if (ty < ctx->view_bottom + 1) {
            int step = jb_tex_vstep[(height + 2) * JB_TEX_VSTEP_STRIDE
                                    + (y - row->y_far)];
            int vlin = blend + (y - row->y_far);
            int c, acc;

            if (row->row < JB_KEYED_ROW_FIRST) {
                int sy = anim ? 100 - step : step;

                Blit_NoKey(ctx->screen, 0, ty, ctx->view_w, 1, tex_h, 0, sy);
                Blit_NoKey(ctx->screen, 0, ty, (int)x_left, 1, tex_h,
                           (int)(400.0f - x_left), sy);
                Blit_NoKey(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           tex_h, 0, sy);
            } else {
                int sy = anim ? 100 - vlin : vlin;

                Blit_Keyed(ctx->screen, 0, ty, (int)x_left, 1, tex_h, blend,
                           JB_NO_KEY, (int)(400.0f - x_left), sy);
                Blit_Keyed(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           tex_h, blend, JB_NO_KEY, 0, sy);
            }

            /* JumpyBall.exe TrackRow_DrawIce 0x00017634 / TrackRow_DrawDesert
               0x0001631c: __rt_sdiv(g_mapCols, k) and
               __rt_sdiv(g_mapCols, k + width) with k stepped by width. */
            for (c = 0, acc = 0; c < ctx->map_cols; c++, acc += width) {
                int x0 = acc / ctx->map_cols;
                int x1 = (acc + width) / ctx->map_cols;

                if (row->tiles[c] == 0) {
                    int sy = (anim ? 100 : 0) + step;

                    if (row->row < JB_KEYED_ROW_FIRST)
                        Blit_TileH(ctx->screen, x0 + (int)x_left, ty,
                                   (x1 - x0) + 1, sy, tex_v, JB_NO_KEY);
                    else
                        Blit_TileH_Ofs(ctx->screen, x0 + (int)x_left, ty,
                                       (x1 - x0) + 1, sy, tex_v, JB_NO_KEY,
                                       blend);
                } else if (row->row < JB_KEYED_ROW_FIRST) {
                    Blit_TileH(ctx->screen, x0 + (int)x_left - 1, ty,
                               (x1 - x0) + 2, step, tex_tile, JB_NO_KEY);
                } else {
                    Blit_TileH_Ofs(ctx->screen, x0 + (int)x_left, ty,
                                   (x1 - x0) + 1, step, tex_tile, JB_NO_KEY,
                                   blend);
                }
            }
        }

        x_left  += dx_left;
        x_right += dx_right;

        /* JumpyBall.exe TrackRow_DrawIce 0x00017a54 / TrackRow_DrawDesert
           0x0001673c __lts(xLeft, 0.0) and 0x00017aa8 / 0x00016790
           __gts(xRight + 1.0, __itos(g_viewW)) end row 0 early. */
        if (row->row == 0 && ty > ctx->view_bottom && x_left < 0.0f
            && x_right + 1.0f > (float)ctx->view_w)
            break;
    }
}

/* JumpyBall.exe TrackRow_DrawIce 0x00016e98 blits g_texWaterH 0x00061c8c,
   g_texWaterV 0x00061c90 and g_texIceV 0x00061c80. */
void TrackRow_DrawIce(const jb_trackrow_ctx *ctx, const jb_track_row *row)
{
    DrawTiledRow(ctx, row, ctx->tex_water_h, ctx->tex_water_v, ctx->tex_ice_v);
}

/* JumpyBall.exe TrackRow_DrawDesert 0x00015b80 blits g_texSandH 0x00061c98,
   g_texSandV 0x00061c94 and g_texMesh 0x00061c9c. */
void TrackRow_DrawDesert(const jb_trackrow_ctx *ctx, const jb_track_row *row)
{
    DrawTiledRow(ctx, row, ctx->tex_sand_h, ctx->tex_sand_v, ctx->tex_mesh);
}
