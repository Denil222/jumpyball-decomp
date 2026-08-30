#include "jb_audio.h"

#include "jb_assets.h"
#include "jb_platform.h"

#include <stdlib.h>

/* JumpyBall.exe Game_Init 0x000113bc calls hssSound::volume with 0x20 for
   sblam.wav and 0x40 for smenu.wav, and hssSound::loop with false for both. */
static const struct {
    const char *name;
    int         volume;
} jb_snd_table[JB_SND_COUNT] = {
    { "sblam.wav", 0x20 },
    { "smenu.wav", 0x40 }
};

/* JumpyBall.exe Game_Init 0x000113bc passes 0x20 to
   hssSpeaker::volumeSounds. */
static int jb_volume = JB_MASTER_VOLUME_MAX;

static int jb_music_vol = JB_MUSIC_VOLUME_MAX;

/* JumpyBall.exe Music_LoadForContext 0x0001db68 */
static const struct {
    const char *name;
    int         volume;
} jb_mus_table[2] = {
    { "music1.tkm", 0x28 },
    { "music2.tkm", 0x38 }
};

void Audio_Init(void)
{
    int i;

    if (!Platform_AudioInit())
        return;

    for (i = 0; i < JB_SND_COUNT; i++)
        Platform_SoundLoad(i, Assets_Sound(jb_snd_table[i].name),
                           jb_snd_table[i].volume * JB_VOLUME_MAX / 0x40);
}

void Audio_Play(int snd)
{
    if (snd < 0 || snd >= JB_SND_COUNT)
        return;
    Platform_SoundPlay(snd);
}

void Audio_VolumeStep(int step)
{
    /* JumpyBall.exe Volume_Step 0x00012b54 keeps hssSpeaker::volumeSounds
       below 0x21 and hssSpeaker::volumeMusics below 0x41, and compares
       unsigned, so a negative step stops at 0. */
    if ((unsigned)(step * 2 + jb_volume) < 0x21u)
        jb_volume += step * 2;
    if ((unsigned)(step * 4 + jb_music_vol) < 0x41u)
        jb_music_vol += step * 4;
    Platform_SoundMasterVolume(jb_volume);
    Platform_MusicMasterVolume(jb_music_vol);
}

int Audio_Volume(void)
{
    return jb_volume;
}

/* JumpyBall.exe Music_LoadForContext 0x0001db68 */
void Audio_MusicPlay(int ctx)
{
    int pick;

    if (ctx == JB_MUS_MENU) {
        Platform_MusicPlay(Assets_Music("mainmenu.tkm"), 0x14, 1);
        return;
    }
    pick = rand() % 2;
    Platform_MusicPlay(Assets_Music(jb_mus_table[pick].name),
                       jb_mus_table[pick].volume, 1);
}

void Audio_MusicStop(void)
{
    Platform_MusicStop();
}
