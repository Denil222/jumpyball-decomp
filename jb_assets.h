#ifndef JB_ASSETS_H
#define JB_ASSETS_H

int Assets_Init(void);

const char *Assets_Root(void);
const char *Assets_FailureText(void);

/* JumpyBall.exe Game_Init 0x000113bc LoadBitmapW resource ids. */
const char *Assets_Bitmap(int res);

/* JumpyBall.exe 0x00061dc0 "\Sounds\sblam.wav", 0x00061e38 "\Sounds\smenu.wav". */
const char *Assets_Sound(const char *name);

/* JumpyBall.exe Music_LoadForContext 0x0001db68 "\Musics\mainmenu.tkm". */
const char *Assets_Music(const char *name);

#endif /* JB_ASSETS_H */
