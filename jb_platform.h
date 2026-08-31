/* Host layer for the port.  It stands in for JumpyBall.exe's GAPI path
   (Gfx_CreateBackBuffer 0x00021698, Gfx_Present 0x000126fc) and is the only
   file allowed to know about SDL. */
#ifndef JB_PLATFORM_H
#define JB_PLATFORM_H

#include "jb_gfx.h"

enum {
    JB_KEY_LEFT,
    JB_KEY_RIGHT,
    JB_KEY_UP,
    JB_KEY_DOWN,
    JB_KEY_JUMP,
    JB_KEY_MENU,
    JB_KEY_COUNT
};

int Platform_Init(int w, int h, int scale, const char *title);
void Platform_Shutdown(void);

/* The 16bpp surface every blitter in jb_gfx.h draws into. */
jb_surface *Platform_BackBuffer(void);

void Platform_Present(void);

/* Returns 0 once the window has been closed. */
int Platform_PollEvents(void);

int Platform_KeyDown(int key);

/* jumpyball JumpyBall.exe WndProc 0x0001fd2c compares the incoming VK code
   against g_keyLeft, g_keyRight, g_keyUp, g_keyDown, g_keyJump and g_keyMenu;
   the key-config wizard in the same function writes those six slots. */
#define JB_KEY_UNBOUND 0

int Platform_KeyBinding(int key);
void Platform_SetKeyBinding(int key, int code);

/* jumpyball JumpyBall.exe WndProc 0x0001fd2c binds on WM_KEYDOWN. */
int Platform_NextRawKey(void);
void Platform_FlushRawKeys(void);

unsigned Platform_Ticks(void);

void Platform_Delay(unsigned ms);

/* JumpyBall.exe Game_Init 0x000113bc calls hssSpeaker::volumeSounds with 0x20
   and hssSound::volume with 0x20 and 0x40. */
#define JB_VOLUME_MAX 128
#define JB_SOUND_SLOTS 4
#define JB_MASTER_VOLUME_MAX 0x20
#define JB_MUSIC_VOLUME_MAX 0x40

int Platform_AudioInit(void);
int Platform_SoundLoad(int slot, const char *path, int volume);
void Platform_SoundPlay(int slot);
void Platform_SoundMasterVolume(int volume);
int Platform_MusicPlay(const char *path, int volume, int loop);
void Platform_MusicStop(void);
void Platform_MusicMasterVolume(int volume);
void Platform_AudioShutdown(void);

/* Directory the running executable lives in, with a trailing separator. */
/* Directory the running executable lives in, with a trailing separator. */
const char *Platform_BasePath(void);

void Platform_ShowError(const char *title, const char *text);

const char *Platform_LastError(void);

#endif /* JB_PLATFORM_H */
