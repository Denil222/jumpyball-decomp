#include "jb_consts.h"
#include "jb_trackrow.h"

/* JumpyBall.exe TrackRow_DrawIceAlt 0x00019e28: one ice band of width+1 with no
   per-column loop (no __rt_sdiv) and no animation phase (no __rt_udiv), and
   0x00019f3c "subgt r3,r6,#0x8" against 0x0001a1ac "sub r3,r10,#0x9". */
void TrackRow_DrawIceAlt(const jb_trackrow_ctx *ctx, const jb_track_row *row)
{
    int   height   = row->y_near - row->y_far;
    float inv      = 1.0f / (float)height;
    float dx_left  = (float)(row->x_near_left - row->x_far_left) * inv;
    float dx_right = (float)(row->x_near_right - row->x_far_right) * inv;
    float x_left   = (float)row->x_far_left;
    float x_right  = (float)row->x_far_right;
    int   y_end    = row->y_near;
    int   blend    = 0;
    int   y;

    if (row->row == 0)
        y_end += JB_NEAR_ROW_EXTRA;
    if (row->row > 5)
        blend = (row->row - 8) * 5;

    for (y = row->y_far; y < y_end; y++) {
        int width = (int)(x_right - x_left);
        int ty    = y - ctx->layout_mode;

        if (ty < ctx->view_bottom + 1) {
            int step = jb_tex_vstep[(height + 2) * JB_TEX_VSTEP_STRIDE
                                    + (y - row->y_far)];

            if (row->row < JB_KEYED_ROW_FIRST) {
                Blit_TileH(ctx->screen, (int)x_left, ty, width + 1, step,
                           ctx->tex_ice_v, JB_NO_KEY);
                Blit_NoKey(ctx->screen, 0, ty, (int)x_left, 1,
                           ctx->tex_water_h, (int)(400.0f - x_left), step);
                Blit_NoKey(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           ctx->tex_water_h, 0, step);
            } else {
                int vlin = (row->row - 9) * 5 + (y - row->y_far);

                Blit_TileH_Ofs(ctx->screen, (int)x_left, ty, width + 1, step,
                               ctx->tex_ice_v, JB_NO_KEY, blend);
                Blit_Keyed(ctx->screen, 0, ty, (int)x_left, 1,
                           ctx->tex_water_h, blend, JB_NO_KEY,
                           (int)(400.0f - x_left), vlin);
                Blit_Keyed(ctx->screen, (int)x_right, ty,
                           (int)((float)ctx->view_w - x_right + 1.0f), 1,
                           ctx->tex_water_h, blend, JB_NO_KEY, 0, vlin);
            }
        }

        x_left  += dx_left;
        x_right += dx_right;

        /* JumpyBall.exe TrackRow_DrawIceAlt 0x0001a368 __lts(xLeft, 0.0) and
           0x0001a3bc __gts(xRight + 1.0, __itos(g_viewW)) reach
           0x0001a3c4 "ldrne r4,[sp,#0x58]". */
        if (row->row == 0 && ty > ctx->view_bottom && x_left < 0.0f
            && x_right + 1.0f > (float)ctx->view_w)
            y = y_end;
    }
}
