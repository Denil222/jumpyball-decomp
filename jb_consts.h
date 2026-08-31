/* All values decoded out of JumpyBall.exe (imagebase 0x00010000) with
   scripts/peek.py; each line names the VA and the symbol it came from. */
#ifndef JB_CONSTS_H
#define JB_CONSTS_H

#define JB_VIEW_W        240   /* JumpyBall.exe 0x00026260 g_viewW */
#define JB_VIEW_H        320   /* JumpyBall.exe 0x00026264 g_viewH */
#define JB_VIEW_CENTER_X 120   /* JumpyBall.exe 0x00026268 g_viewCenterX */
#define JB_VIEW_BOTTOM   320   /* JumpyBall.exe 0x0002625c g_viewBottom */

/* JumpyBall.exe Game_Init 0x000113bc, g_screenW == 0xb0 branch */
#define JB_SMALL_VIEW_W        176
#define JB_SMALL_VIEW_H        220
#define JB_SMALL_VIEW_CENTER_X 88
#define JB_LAYOUT_240x320      0     /* JumpyBall.exe 0x00064938 g_layoutMode */
#define JB_LAYOUT_176x220      0x14

#define JB_MAP_COLS   16    /* JumpyBall.exe 0x0002628c g_mapCols */
#define JB_MAP_ROWS   1024  /* JumpyBall.exe 0x00026274 g_mapRows */
#define JB_GRID_ROWS  1020  /* JumpyBall.exe Level_LoadTileMap 0x0001261c, bound 0x3fc */
/* JumpyBall.exe Track_DrawFrame 0x0001e75c indexes g_tileGrid 0x00031178 at
   (g_camRow + row) * g_mapCols for 17 rows, and WndProc 0x00020920 lets
   g_camRow reach 0x3fe before it ends the run; the rows past 0x3fc are the
   zeroed .bss tail of the 0x400-row array. */
#define JB_GRID_ALLOC_ROWS (JB_GRID_ROWS + JB_VISIBLE_ROWS + 2)
#define JB_ROW_PIXELS 10    /* JumpyBall.exe 0x000262bc kRowPixels */
/* JumpyBall.exe Game_Init 0x000113bc overwrites the 0x00026290 initialiser 100 with
   (int)(140.0 - g_layoutMode*(5/17) + DAT_00064a60); DAT_00064a60 is never written. */
#define JB_TILE_SIZE       140
#define JB_SMALL_TILE_SIZE 134
#define JB_FAR_TILE_SIZE   4   /* JumpyBall.exe Track_DrawFrame 0x0001dd54 */
#define JB_PROJ_SPAN       172 /* JumpyBall.exe 0x0006155c kProjSpan = proj(16) */
#define JB_TEX_ANIM_PERIOD 250 /* JumpyBall.exe 0x0006495c g_animMs, remainder vs 125 */
#define JB_TEX_ANIM_HALF   125
#define JB_TEX_VSTEP_STRIDE 300 /* JumpyBall.exe 0x00035238 g_texVStep row stride */
#define JB_NEAR_ROW_EXTRA  300  /* JumpyBall.exe TrackRow_DrawIce 0x00016e98, row 0 */
#define JB_KEYED_ROW_FIRST 10   /* rowIndex >= 10 switches to Blit_Keyed */
/* JumpyBall.exe TrackRow_DrawGrass 0x00019370 "cmp r0,#0x64" */
#define JB_GRASS_KEYED_ROW_FIRST 100
#define JB_GLOW_TINT   250  /* JumpyBall.exe TrackRow_DrawSky 0x00016c9c "mov r5,#0xfa" */
#define JB_GLOW_SPREAD 3.0  /* JumpyBall.exe TrackRow_DrawSky 0x00016c60 0x4008000000000000 */

#define JB_ACCEL_X       7.0f   /* JumpyBall.exe 0x000262ac kAccelX */
#define JB_JUMP_IMPULSE  10.0f  /* JumpyBall.exe 0x000262b4 kJumpImpulse */
#define JB_GRAVITY       20.0f  /* JumpyBall.exe 0x00026434 kGravity */
#define JB_RESPAWN_Y     5.0f   /* JumpyBall.exe Player_Respawn 0x00012f18 */
#define JB_DEATH_VEL_Y  -11.0f  /* JumpyBall.exe WndProc 0x0001fd2c */
#define JB_DEATH_FLOOR  -20.0f  /* JumpyBall.exe WndProc 0x0001fd2c */
#define JB_BALL_X_CLAMP  16.0f  /* JumpyBall.exe WndProc 0x0001fd2c */

#define JB_VISIBLE_ROWS 17     /* JumpyBall.exe 0x00026358 kVisibleRows */
#define JB_PROJ_SCALE   163.0f /* JumpyBall.exe 0x00026438 kProjScale */
#define JB_PROJ_NUM     70.0f  /* JumpyBall.exe 0x0002643c kProjNum */
#define JB_PROJ_DEN_A   60.0f  /* JumpyBall.exe 0x00026440 kProjDenA */
#define JB_PROJ_DEN_B   100.0f /* JumpyBall.exe 0x00026444 kProjDenB */

#define JB_PARALLAX 0.08f /* JumpyBall.exe Game_DrawFrame 0x0001f37c */

/* JumpyBall.exe Track_DrawFrame 0x0001dd54, Blit_Glyph 0x000239ac call for
   g_sprBallFrames 0x00061c78 at 0x0001ef10: r3 = 0x45, [sp,#0x0] = 0x45, and
   __rt_sdiv 0x0001ed1c takes (0x276, frame * 0x46). */
#define JB_BALL_FRAMES     9
#define JB_BALL_CELL       70    /* 0x46 */
#define JB_BALL_DRAW_SIZE  69    /* 0x45 */

/* JumpyBall.exe Track_DrawFrame 0x0001ecdc __muld operand 0x3fa0000000000000. */
#define JB_BALL_SPIN_RATE  0.03125

/* JumpyBall.exe Track_DrawFrame 0x0001eba8 and 0x0001edcc __subd operand
   0x405b8000_00000000, applied after g_layoutMode*1.5 + g_viewBottom. */
#define JB_BALL_Y_BIAS     110.0
#define JB_BALL_LAYOUT_MUL 1.5   /* 0x3ff8000000000000 at 0x0001eb54, 0x0001ed78 */

/* JumpyBall.exe Track_DrawFrame 0x0001ee1c __subd operand
   0x4050800000000000 minus __itos(g_layoutMode) 0x0001edfc. */
#define JB_BALL_Y_SCALE    66.0

/* JumpyBall.exe Track_DrawFrame 0x0001eeb0 and 0x0001ebdc __muld operand
   0x3fe0000000000000 applied to g_ballVelZ 0x000649cc. */
#define JB_BALL_VELZ_MUL   0.5

/* JumpyBall.exe Track_DrawFrame Blit_Glyph 0x0001ecb4 for g_sprShadow
   0x00061cb8: r3 = 0x4a, [sp,#0x0] = 0x30, tint 0,0,0, gated on __ged
   0x0001eaf8 (g_ballY >= 0.0); 0x0001ec38 __muld 0x4018000000000000 and
   0x0001ec80 __addd 0x4044000000000000. */
#define JB_SHADOW_W        74    /* 0x4a */
#define JB_SHADOW_H        48    /* 0x30 */
#define JB_SHADOW_Y_MUL    6.0
#define JB_SHADOW_Y_BIAS   40.0

/* JumpyBall.exe Track_DrawFrame Blit_Glyph x arguments: 0x0001ecac
   sub r1,r3,#0x25 for the shadow and 0x0001ef08 sub r1,r3,#0x23 for the ball. */
#define JB_SHADOW_X_OFS    0x25
#define JB_BALL_X_OFS      0x23

/* JumpyBall.exe Level_Begin 0x0001376c: g_theme = g_menuIndex, then 2 -> 4,
   3 -> 1, forced 0 while g_altTrackMode > 0, so 2 and 3 are unreachable. */
enum jb_theme {
    JB_THEME_ICE    = 0,
    JB_THEME_DESERT = 1,
    JB_THEME_FOREST = 2,
    JB_THEME_SKY    = 3,
    JB_THEME_GRASS  = 4
};

/* JumpyBall.exe LoadBitmapW ids, Game_Init 0x000113bc and Level_Begin 0x0001376c */
#define JB_RES_BACKDROP_MENU   308
#define JB_RES_BACKDROP_ICE    321
#define JB_RES_BACKDROP_DESERT 316
#define JB_RES_BACKDROP_FOREST 322
#define JB_RES_BACKDROP_SKY    324
#define JB_RES_LEVEL_MAP_1     309
#define JB_RES_LEVEL_MAP_2     333
#define JB_RES_LEVEL_MAP_3     334

/* JumpyBall.exe Game_Init 0x000113bc LoadBitmapW ids, in load order from
   0x133 g_texWood to 0x14e g_levelMap3. */
#define JB_RES_FONT          254
/* JumpyBall.exe Font_Load 0x0001f4d8 LoadBitmapW 0x14c on the g_fontSize == 2
   branch. */
#define JB_RES_FONT_LARGE    332
#define JB_RES_TEX_WOOD      307
#define JB_RES_BALL_FRAMES   310
#define JB_RES_TEX_MOSS_H    311
#define JB_RES_TEX_ICE_V     312
#define JB_RES_SIGN_KEEP_OFF 313
#define JB_RES_FIR           314
#define JB_RES_TEX_WATER_H   315
#define JB_RES_TEX_WATER_V   317
#define JB_RES_TEX_SAND_V    318
#define JB_RES_TEX_SAND_H    319
#define JB_RES_TEX_MESH      320
#define JB_RES_TEX_STONE     323
#define JB_RES_TEX_SKY_TILE  325
#define JB_RES_GLOW          326
#define JB_RES_TEX_GRASS     327
#define JB_RES_TREE          328
#define JB_RES_TEX_GRASS_H   329
#define JB_RES_SHADOW        330
#define JB_RES_BALL          331

/* JumpyBall.exe Game_Init 0x000113bc LoadBitmapW ids for the menu chrome, in
   load order at 0x00011400..0x000114f4: g_sprLogoPocketNewSmall 0xfd,
   g_sprBarThin 0xf7, g_sprPanelGradient 0xf8, g_sprTitleJumpyBall 0xf9,
   g_sprButtonNarrow 0xfa, g_sprButtonWide 0xfb. */
#define JB_RES_BAR_THIN             247
#define JB_RES_PANEL_GRADIENT       248
#define JB_RES_TITLE                249
#define JB_RES_BUTTON_NARROW        250
#define JB_RES_BUTTON_WIDE          251
#define JB_RES_LOGO_POCKETNEW_SMALL 253

/* jumpyball JumpyBall.exe KeyConfig_Tick 0x0001f900 blits the sprite
   LoadBitmapW(g_hInstance, 0xcc) loaded. */
#define JB_RES_KEYCONFIG_PANEL 204

#define JB_HUD_BRIGHT 250 /* JumpyBall.exe 0x00026280 g_hudR */

#define JB_MODE_GAME 0 /* JumpyBall.exe 0x00026340 g_appMode */
#define JB_MODE_MENU 1

#endif /* JB_CONSTS_H */
