#include "jb_level.h"

unsigned char jb_tile_grid[JB_GRID_ALLOC_ROWS * JB_MAP_COLS];
int           jb_checkpoint_x[JB_CHECKPOINT_N];
int           jb_checkpoint_y[JB_CHECKPOINT_N];

/* JumpyBall.exe Level_LoadTileMap 0x0001261c. */
void Level_LoadTileMap(const jb_surface *screen, int level,
                       const jb_sprite *map1, const jb_sprite *map2,
                       const jb_sprite *map3)
{
    const uint16_t *src   = 0;
    unsigned char  *dst   = jb_tile_grid;
    int             found = 0;
    int             row, col;

    /* JumpyBall.exe Level_LoadTileMap 0x00012628 "cmp r0,#0x0", 0x00012648
       "cmp r0,#0x1" and 0x00012654 "cmp r0,#0x2" with ldrge. */
    if (level == 0)
        src = map1->pixels;
    if (level == 1)
        src = map2->pixels;
    if (level > 1)
        src = map3->pixels;
    /* JumpyBall.exe Level_LoadTileMap 0x00012644 "ldrne r8,[sp,#0x0]" leaves the
       source pointer uninitialised when none of the three tests matches. */
    if (src == 0)
        return;

    for (row = 0; row < JB_GRID_ROWS; row++) {
        const uint16_t *p = src;

        for (col = 0; col < JB_MAP_COLS; col++, p++) {
            /* JumpyBall.exe Level_LoadTileMap 0x00012670 "mvn r1,#0xff000000"
               then 0x0001268c "movne r3,#0x1". */
            dst[col] = (*p != Color_Pack16If16bpp(screen, 0x00ffffffu));

            /* JumpyBall.exe Level_LoadTileMap 0x00012680 "mov r1,#0xff" then
               0x000126a4 "addeq r10,r10,#0x1" ahead of both stores, with no
               bound on r10. */
            if (*p == Color_Pack16If16bpp(screen, 0x000000ffu)) {
                found++;
                if (found < JB_CHECKPOINT_N) {
                    jb_checkpoint_x[found] = col;
                    jb_checkpoint_y[found] = row;
                }
            }
        }

        dst += JB_MAP_COLS;
        src += map1->w;
    }
}
