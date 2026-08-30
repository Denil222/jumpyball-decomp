#ifndef JB_LEVEL_H
#define JB_LEVEL_H

#include "jb_consts.h"
#include "jb_gfx.h"

/* JumpyBall.exe Level_LoadTileMap 0x0001261c writes 0x10 bytes per row
   ("add r7,r7,#0x10") for 0x3fc rows to g_tileGrid 0x00031178. */
extern unsigned char jb_tile_grid[JB_GRID_ALLOC_ROWS * JB_MAP_COLS];

/* JumpyBall.exe g_checkpointY 0x000611e8 and g_checkpointX 0x000612b0 lie 0xc8
   bytes apart, and Level_LoadTileMap 0x000126a4 indexes both from 1. */
#define JB_CHECKPOINT_N 50

extern int jb_checkpoint_x[JB_CHECKPOINT_N];
extern int jb_checkpoint_y[JB_CHECKPOINT_N];

/* JumpyBall.exe Level_LoadTileMap 0x0001261c reads g_levelMap1 0x00061c74 at
   0x00012634, g_levelMap2 0x00061d10 at 0x0001264c and g_levelMap3 0x00061d14
   at 0x00012658, and takes the source row stride from g_levelMap1 for all three
   at 0x000126c0 "ldr r0,[r9,#0x54]; ldr r3,[r0,#0x4]". */
void Level_LoadTileMap(const jb_surface *screen, int level,
                       const jb_sprite *map1, const jb_sprite *map2,
                       const jb_sprite *map3);

#endif /* JB_LEVEL_H */
