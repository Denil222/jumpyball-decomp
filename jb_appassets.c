#include "jb_appassets.h"

#include "jb_assets.h"
#include "jb_bmp.h"
#include "jb_consts.h"
#include "jb_platform.h"
#include "jb_text.h"

#include <stdio.h>

#define JB_MSG_MAX 2048

jb_app_assets jb_a;

static const struct {
    int        res;
    jb_sprite *spr;
} jb_asset_table[] = {
    { JB_RES_BACKDROP_ICE,    &jb_a.backdrop_ice },
    { JB_RES_BACKDROP_DESERT, &jb_a.backdrop_desert },
    { JB_RES_LEVEL_MAP_1,     &jb_a.map1 },
    { JB_RES_LEVEL_MAP_2,     &jb_a.map2 },
    { JB_RES_LEVEL_MAP_3,     &jb_a.map3 },
    { JB_RES_TEX_WATER_H,     &jb_a.tex_water_h },
    { JB_RES_TEX_WATER_V,     &jb_a.tex_water_v },
    { JB_RES_TEX_ICE_V,       &jb_a.tex_ice_v },
    { JB_RES_TEX_SAND_H,      &jb_a.tex_sand_h },
    { JB_RES_TEX_SAND_V,      &jb_a.tex_sand_v },
    { JB_RES_TEX_MESH,        &jb_a.tex_mesh },
    { JB_RES_TEX_WOOD,        &jb_a.tex_wood },
    { JB_RES_TEX_GRASS,       &jb_a.tex_grass },
    { JB_RES_TEX_GRASS_H,     &jb_a.tex_grass_h },
    { JB_RES_SIGN_KEEP_OFF,   &jb_a.sign_keep_off },
    { JB_RES_BALL_FRAMES,     &jb_a.ball },
    { JB_RES_SHADOW,          &jb_a.shadow },
    { JB_RES_BACKDROP_MENU,   &jb_a.backdrop_menu },
    { JB_RES_BAR_THIN,        &jb_a.bar_thin },
    { JB_RES_PANEL_GRADIENT,  &jb_a.panel },
    { JB_RES_TITLE,           &jb_a.title },
    { JB_RES_BUTTON_NARROW,   &jb_a.button_narrow },
    { JB_RES_BUTTON_WIDE,     &jb_a.button_wide },
    { JB_RES_LOGO_POCKETNEW_SMALL, &jb_a.logo_small },
    { JB_RES_KEYCONFIG_PANEL, &jb_a.keyconfig_panel }
};

#define JB_ASSET_N (int)(sizeof jb_asset_table / sizeof jb_asset_table[0])

int AppAssets_Load(const jb_surface *dst)
{
    int i;

    for (i = 0; i < JB_ASSET_N; i++) {
        const char *path = Assets_Bitmap(jb_asset_table[i].res);

        if (!Bmp_LoadSprite(dst, path, jb_asset_table[i].spr)) {
            char msg[JB_MSG_MAX];

            snprintf(msg, sizeof msg,
                     "Bitmap missing or unreadable:\n\n%s\n\n"
                     "Copy the whole BITMAP folder next to jumpyball.exe.",
                     path);
            Platform_ShowError("JumpyBall", msg);
            return 0;
        }
    }
    return 1;
}

void AppAssets_Free(void)
{
    int i;

    for (i = 0; i < JB_ASSET_N; i++)
        Bmp_FreeSprite(jb_asset_table[i].spr);
    Font_Free();
}
