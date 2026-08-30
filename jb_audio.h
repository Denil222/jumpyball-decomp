#ifndef JB_AUDIO_H
#define JB_AUDIO_H

/* JumpyBall.exe Game_Init 0x000113bc loads "\Sounds\sblam.wav" into
   g_sndBounce and "\Sounds\smenu.wav" into g_sndMenuClick. */
enum {
    JB_SND_BOUNCE,
    JB_SND_MENU,
    JB_SND_COUNT
};

void Audio_Init(void);

void Audio_Play(int snd);

/* JumpyBall.exe Music_LoadForContext 0x0001db68 picks "\Musics\mainmenu.tkm"
   at volume 0x14 when g_appMode is 1, otherwise rand() % 2 + 1 selects
   "\Musics\music1.tkm" at 0x28 or "\Musics\music2.tkm" at 0x38; all three loop. */
enum {
    JB_MUS_MENU,
    JB_MUS_GAME
};

void Audio_MusicPlay(int ctx);

void Audio_MusicStop(void);

/* JumpyBall.exe Volume_Step 0x00012b54 adds step*2 to
   hssSpeaker::volumeSounds while the sum is below 0x21, and step*4 to
   hssSpeaker::volumeMusics while that sum is below 0x41; both comparisons are
   unsigned, so a step below zero stops at zero. */
void Audio_VolumeStep(int step);

/* JumpyBall.exe Menu_DrawFrame 0x0001a7d0 at 0x0001cf70 blits the bar with
   width hssSpeaker::volumeSounds * 2. */
int Audio_Volume(void);

#endif /* JB_AUDIO_H */
