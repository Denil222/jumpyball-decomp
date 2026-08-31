#ifndef JB_APPASSETS_H
#define JB_APPASSETS_H

#include "jb_gfx.h"

typedef struct {
    jb_sprite backdrop_ice;
    jb_sprite backdrop_desert;
    jb_sprite map1;
    jb_sprite map2;
    jb_sprite map3;
    jb_sprite tex_water_h;
    jb_sprite tex_water_v;
    jb_sprite tex_ice_v;
    jb_sprite tex_sand_h;
    jb_sprite tex_sand_v;
    jb_sprite tex_mesh;
    jb_sprite tex_wood;
    jb_sprite tex_grass;
    jb_sprite tex_grass_h;
    jb_sprite sign_keep_off;
    jb_sprite ball;
    jb_sprite shadow;
    jb_sprite backdrop_menu;
    jb_sprite bar_thin;
    jb_sprite panel;
    jb_sprite title;
    jb_sprite button_narrow;
    jb_sprite button_wide;
    jb_sprite logo_small;
    jb_sprite keyconfig_panel;
} jb_app_assets;

extern jb_app_assets jb_a;

/* jumpyball JumpyBall.exe Game_Init 0x000113bc loads every LoadBitmapW
   resource into its global sprite before the first Screen_Set 0x00013678. */
int AppAssets_Load(const jb_surface *dst);
void AppAssets_Free(void);

#endif /* JB_APPASSETS_H */
