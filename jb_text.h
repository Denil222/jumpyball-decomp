#ifndef JB_TEXT_H
#define JB_TEXT_H

#include "jb_gfx.h"

/* JumpyBall.exe Font_Load 0x0001f4d8 keeps its argument in g_fontSize and loads
   one of two sheets: 1 takes BITMAP 254 and scans row 0 for 0xff00ff marker
   pairs, 2 takes BITMAP 332 and scans for columns that are black over the full
   sheet height.  Both fill g_glyphWidth 0x00027298 and g_glyphX 0x00027698. */
int Font_Load(const jb_surface *dst);

/* JumpyBall.exe Screen_Set 0x00013678 calls Font_Load(1) and Player_Respawn
   0x00012f18 calls Font_Load(2). */
void Font_Select(const jb_surface *dst, int size);

void Font_Free(void);

/* JumpyBall.exe Text_DrawCentered 0x00012878. */
void Text_DrawCentered(const jb_surface *dst, int x, int y, const char *s,
                       int align, int r, int g, int b);

/* JumpyBall.exe Text_DrawNumber 0x000129b4. */
void Text_DrawNumber(const jb_surface *dst, int x, int y, int value,
                     int align, int digits, int r, int g, int b);

#endif /* JB_TEXT_H */
