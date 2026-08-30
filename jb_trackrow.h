#ifndef JB_TRACKROW_H
#define JB_TRACKROW_H

#include "jb_gfx.h"
#include "jb_track.h"

/* JumpyBall.exe TrackRow_DrawIce 0x00016e98 reads g_screen 0x00061b10,
   g_texIceV 0x00061c80, g_texWaterH 0x00061c8c, g_texWaterV 0x00061c90,
   g_texVStep 0x00035238, g_animMs 0x0006495c, g_viewW 0x00026260,
   g_viewBottom 0x0002625c, g_layoutMode 0x00064938, g_mapCols 0x0002628c.
   JumpyBall.exe TrackRow_DrawDesert 0x00015b80 reads the same set with
   g_texSandV 0x00061c94, g_texSandH 0x00061c98, g_texMesh 0x00061c9c. */
typedef struct {
    const jb_surface *screen;
    const jb_sprite  *tex_water_h;
    const jb_sprite  *tex_water_v;
    const jb_sprite  *tex_ice_v;
    const jb_sprite  *tex_sand_h;
    const jb_sprite  *tex_sand_v;
    const jb_sprite  *tex_mesh;
    const jb_sprite  *tex_sky_tile;
    const jb_sprite  *spr_glow;
    const jb_sprite  *tex_grass_h;
    const jb_sprite  *tex_grass;
    const jb_sprite  *tex_forest_h;
    const jb_sprite  *tex_forest_v;
    const jb_sprite  *spr_tree;
    const jb_sprite  *tex_wood;
    const jb_sprite  *spr_sign_keep_off;
    unsigned          sign_key;
    int               anim_ms;
    int               view_w;
    int               view_bottom;
    int               layout_mode;
    int               map_cols;
    int               cam_row;
} jb_trackrow_ctx;

/* JumpyBall.exe TrackRow_DrawIce 0x00016e98. */
void TrackRow_DrawIce(const jb_trackrow_ctx *ctx, const jb_track_row *row);

/* JumpyBall.exe TrackRow_DrawDesert 0x00015b80. */
void TrackRow_DrawDesert(const jb_trackrow_ctx *ctx, const jb_track_row *row);

/* JumpyBall.exe TrackRow_DrawSky 0x000167bc reads g_texSkyTile 0x00061ca4,
   g_sprGlow 0x00061ca8, g_camRow 0x000649b8. */
void TrackRow_DrawSky(const jb_trackrow_ctx *ctx, const jb_track_row *row);

/* JumpyBall.exe TrackRow_DrawIceAlt 0x00019e28. */
void TrackRow_DrawIceAlt(const jb_trackrow_ctx *ctx, const jb_track_row *row);

/* JumpyBall.exe TrackRow_DrawGrass 0x00019198 reads g_texWood 0x00061c70,
   g_sprSignKeepOffGrass 0x00061c84, g_texGrass 0x00061cac,
   g_texGrassH 0x00061cb4, DAT_00064b18. */
void TrackRow_DrawGrass(const jb_trackrow_ctx *ctx, const jb_track_row *row);

/* JumpyBall.exe TrackRow_DrawForest 0x00017ad4 reads the sprite table at
   0x00061c20: +0x50 g_texWood, +0x5c 0x00061c7c, +0x64 0x00061c84,
   +0x68 0x00061c88, +0x80 0x00061ca0. */
void TrackRow_DrawForest(const jb_trackrow_ctx *ctx, const jb_track_row *row);

#endif /* JB_TRACKROW_H */
