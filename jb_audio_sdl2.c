#include "jb_platform.h"
#include "jb_mod.h"

#include <SDL.h>
#include <stdlib.h>

#define JB_VOICES 4

typedef struct {
    Uint8   *data;
    Uint32   len;
    int      volume;
    int      from_wav;
} jb_sound;

typedef struct {
    int    slot;
    Uint32 pos;
} jb_voice;

static jb_sound          jb_sounds[JB_SOUND_SLOTS];
static jb_voice          jb_voices[JB_VOICES];
static SDL_AudioDeviceID jb_audio_dev;
static SDL_AudioSpec     jb_audio_spec;

void Platform_AudioPause(int pause)
{
    if (jb_audio_dev != 0)
        SDL_PauseAudioDevice(jb_audio_dev, pause);
}

/* JumpyBall.exe Game_Init 0x000113bc calls
   hssSpeaker::volumeSounds(&g_hssSpeaker, 0x20), and Volume_Step 0x00012b54
   keeps that setting below 0x21. */
static int jb_master_vol = JB_MASTER_VOLUME_MAX;

void Platform_SoundMasterVolume(int volume)
{
    jb_master_vol = volume;
}

static void SDLCALL MixVoices(void *user, Uint8 *out, int len)
{
    int i;

    (void)user;
    Mod_Render((short *)out, len / 2);

    for (i = 0; i < JB_VOICES; i++) {
        const jb_sound *snd;
        Uint32          left;
        Uint32          n;

        if (jb_voices[i].slot < 0)
            continue;
        snd  = &jb_sounds[jb_voices[i].slot];
        left = snd->len - jb_voices[i].pos;
        n    = (left < (Uint32)len) ? left : (Uint32)len;
        SDL_MixAudioFormat(out, snd->data + jb_voices[i].pos, jb_audio_spec.format,
                           n, snd->volume * jb_master_vol / JB_MASTER_VOLUME_MAX);
        jb_voices[i].pos += n;
        if (jb_voices[i].pos >= snd->len)
            jb_voices[i].slot = -1;
    }
}

/* JumpyBall.exe Game_Init 0x000113bc opens the speaker with hssSpeaker::open
   0x5622, 0x10, false, 1, 4. */
int Platform_AudioInit(void)
{
    SDL_AudioSpec want;
    int           i;

    for (i = 0; i < JB_VOICES; i++)
        jb_voices[i].slot = -1;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return 0;

    SDL_zero(want);
    want.freq     = 22050;
    want.format   = AUDIO_S16LSB;
    want.channels = 1;
    want.samples  = 512;
    want.callback = MixVoices;

    jb_audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &jb_audio_spec, 0);
    if (jb_audio_dev == 0)
        return 0;
    Mod_Init(jb_audio_spec.freq);
    SDL_PauseAudioDevice(jb_audio_dev, 0);
    return 1;
}

/* jumpyball JumpyBall.exe Music_LoadForContext 0x0001db68 */
int Platform_MusicPlay(const char *path, int volume, int loop)
{
    SDL_RWops     *rw;
    unsigned char *buf;
    Sint64         size;
    int            ok;

    if (jb_audio_dev == 0)
        return 0;
    rw = SDL_RWFromFile(path, "rb");
    if (rw == NULL)
        return 0;
    size = SDL_RWsize(rw);
    if (size <= 0 || size > 0x400000) {
        SDL_RWclose(rw);
        return 0;
    }
    buf = (unsigned char *)malloc((size_t)size);
    if (buf == NULL) {
        SDL_RWclose(rw);
        return 0;
    }
    if (SDL_RWread(rw, buf, 1, (size_t)size) != (size_t)size) {
        SDL_RWclose(rw);
        free(buf);
        return 0;
    }
    SDL_RWclose(rw);

    SDL_LockAudioDevice(jb_audio_dev);
    ok = Mod_Play(buf, (int)size, volume, loop);
    SDL_UnlockAudioDevice(jb_audio_dev);
    return ok;
}

/* jumpyball hss.dll hssSpeaker::stopMusics 0x001042f0 */
void Platform_MusicStop(void)
{
    if (jb_audio_dev == 0) {
        Mod_Stop();
        return;
    }
    SDL_LockAudioDevice(jb_audio_dev);
    Mod_Stop();
    SDL_UnlockAudioDevice(jb_audio_dev);
}

/* jumpyball hss.dll hssSpeaker::volumeMusics 0x0010567c */
void Platform_MusicMasterVolume(int volume)
{
    if (jb_audio_dev == 0) {
        Mod_MasterVolume(volume);
        return;
    }
    SDL_LockAudioDevice(jb_audio_dev);
    Mod_MasterVolume(volume);
    SDL_UnlockAudioDevice(jb_audio_dev);
}

int Platform_SoundLoad(int slot, const char *path, int volume)
{
    SDL_AudioSpec spec;
    SDL_AudioCVT  cvt;
    Uint8        *buf;
    Uint32        len;

    if (jb_audio_dev == 0 || slot < 0 || slot >= JB_SOUND_SLOTS)
        return 0;
    if (SDL_LoadWAV(path, &spec, &buf, &len) == NULL)
        return 0;
    if (SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
                          jb_audio_spec.format, jb_audio_spec.channels,
                          jb_audio_spec.freq) < 0) {
        SDL_FreeWAV(buf);
        return 0;
    }

    if (cvt.needed) {
        cvt.len = (int)len;
        cvt.buf = (Uint8 *)SDL_malloc((size_t)cvt.len * (size_t)cvt.len_mult);
        if (cvt.buf == NULL) {
            SDL_FreeWAV(buf);
            return 0;
        }
        SDL_memcpy(cvt.buf, buf, (size_t)len);
        SDL_FreeWAV(buf);
        if (SDL_ConvertAudio(&cvt) < 0) {
            SDL_free(cvt.buf);
            return 0;
        }
        jb_sounds[slot].data     = cvt.buf;
        jb_sounds[slot].len      = (Uint32)cvt.len_cvt;
        jb_sounds[slot].from_wav = 0;
    } else {
        jb_sounds[slot].data     = buf;
        jb_sounds[slot].len      = len;
        jb_sounds[slot].from_wav = 1;
    }
    jb_sounds[slot].volume = volume;
    return 1;
}

/* JumpyBall.exe WndProc 0x0001fd2c plays g_sndBounce on the g_ballY < 0.0
   landing branch and g_sndMenuClick on the menu up/down/left branches, both
   through hssSpeaker::playSound. */
void Platform_SoundPlay(int slot)
{
    int i;

    if (jb_audio_dev == 0 || slot < 0 || slot >= JB_SOUND_SLOTS ||
        jb_sounds[slot].data == NULL)
        return;

    SDL_LockAudioDevice(jb_audio_dev);
    for (i = 0; i < JB_VOICES; i++) {
        if (jb_voices[i].slot < 0) {
            jb_voices[i].slot = slot;
            jb_voices[i].pos  = 0;
            break;
        }
    }
    SDL_UnlockAudioDevice(jb_audio_dev);
}

void Platform_AudioShutdown(void)
{
    int i;

    if (jb_audio_dev != 0) {
        SDL_CloseAudioDevice(jb_audio_dev);
        jb_audio_dev = 0;
    }
    Mod_Stop();
    for (i = 0; i < JB_SOUND_SLOTS; i++) {
        if (jb_sounds[i].data == NULL)
            continue;
        if (jb_sounds[i].from_wav)
            SDL_FreeWAV(jb_sounds[i].data);
        else
            SDL_free(jb_sounds[i].data);
        jb_sounds[i].data = NULL;
    }
}
