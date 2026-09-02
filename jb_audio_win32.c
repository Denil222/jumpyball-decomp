/* waveOut host audio for the native Win32 / Windows CE backend, the
   counterpart of jb_audio_sdl2.c.  The device is opened with the format
   hss.dll asked for - JumpyBall.exe Game_Init 0x000113bc calls
   hssSpeaker::open with 0x5622 (22050), 0x10 bits, mono, 4 voices - and a
   feeder thread refills the wave buffers with Mod_Render plus the running
   sound voices, which is what hssSpeaker::updateModSFX 0x00104370 did. */
#include "jb_platform.h"
#include "jb_mod.h"

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>

#define JB_RATE    22050
#define JB_VOICES  4
#define JB_BUFFERS 4

/* A handheld needs the longer buffer to survive a scheduling hiccup; the
   desktop keeps the 512-frame period SDL_OpenAudioDevice asks for. */
#ifdef JB_WINCE
#define JB_FRAMES 1024
#else
#define JB_FRAMES 512
#endif

typedef struct {
    short   *data;
    unsigned frames;
    int      volume;
} jb_sound;

typedef struct {
    int      slot;
    unsigned pos;
} jb_voice;

static jb_sound  jb_sounds[JB_SOUND_SLOTS];
static jb_voice  jb_voices[JB_VOICES];

static HWAVEOUT        jb_wave;
static WAVEHDR         jb_hdr[JB_BUFFERS];
static short          *jb_bufs[JB_BUFFERS];
static HANDLE          jb_thread;
static HANDLE          jb_event;
static CRITICAL_SECTION jb_lock;
static int             jb_lock_ready;
static volatile int    jb_feeding;

/* JumpyBall.exe Game_Init 0x000113bc calls
   hssSpeaker::volumeSounds(&g_hssSpeaker, 0x20), and Volume_Step 0x00012b54
   keeps that setting below 0x21. */
static int jb_master_vol = JB_MASTER_VOLUME_MAX;

void Platform_SoundMasterVolume(int volume)
{
    jb_master_vol = volume;
}

static void Lock(void)
{
    if (jb_lock_ready)
        EnterCriticalSection(&jb_lock);
}

static void Unlock(void)
{
    if (jb_lock_ready)
        LeaveCriticalSection(&jb_lock);
}

/* SDL_MixAudioFormat scales by volume/SDL_MIX_MAXVOLUME and saturates; the
   volumes jb_audio.c hands out are on that same 0..128 scale. */
static void MixVoice(short *out, const short *src, unsigned n, int vol)
{
    unsigned i;

    for (i = 0; i < n; i++) {
        int v = out[i] + (int)src[i] * vol / JB_VOLUME_MAX;

        if (v > 32767)
            v = 32767;
        else if (v < -32768)
            v = -32768;
        out[i] = (short)v;
    }
}

static void FillBuffer(short *out, int frames)
{
    int i;

    Lock();
    Mod_Render(out, frames);
    for (i = 0; i < JB_VOICES; i++) {
        const jb_sound *snd;
        unsigned        left, n;

        if (jb_voices[i].slot < 0)
            continue;
        snd  = &jb_sounds[jb_voices[i].slot];
        left = snd->frames - jb_voices[i].pos;
        n    = (left < (unsigned)frames) ? left : (unsigned)frames;
        MixVoice(out, snd->data + jb_voices[i].pos, n,
                 snd->volume * jb_master_vol / JB_MASTER_VOLUME_MAX);
        jb_voices[i].pos += n;
        if (jb_voices[i].pos >= snd->frames)
            jb_voices[i].slot = -1;
    }
    Unlock();
}

/* waveOutWrite may not be called from a waveOut callback, so the refill runs
   on its own thread.  The wait has a timeout as well as the event because not
   every Windows CE waveOut driver signals CALLBACK_EVENT reliably. */
static DWORD WINAPI FeedThread(LPVOID arg)
{
    (void)arg;
    while (jb_feeding) {
        int i;

        WaitForSingleObject(jb_event, 10);
        for (i = 0; i < JB_BUFFERS && jb_feeding; i++) {
            if ((jb_hdr[i].dwFlags & WHDR_DONE) == 0)
                continue;
            FillBuffer(jb_bufs[i], JB_FRAMES);
            jb_hdr[i].dwFlags &= ~WHDR_DONE;
            if (waveOutWrite(jb_wave, &jb_hdr[i], sizeof jb_hdr[i]) != MMSYSERR_NOERROR)
                jb_hdr[i].dwFlags |= WHDR_DONE;
        }
    }
    return 0;
}

/* JumpyBall.exe Game_Init 0x000113bc opens the speaker with hssSpeaker::open
   0x5622, 0x10, false, 1, 4. */
int Platform_AudioInit(void)
{
    WAVEFORMATEX fmt;
    DWORD        tid;
    int          i;

    for (i = 0; i < JB_VOICES; i++)
        jb_voices[i].slot = -1;

    memset(&fmt, 0, sizeof fmt);
    fmt.wFormatTag      = WAVE_FORMAT_PCM;
    fmt.nChannels       = 1;
    fmt.nSamplesPerSec  = JB_RATE;
    fmt.wBitsPerSample  = 16;
    fmt.nBlockAlign     = (WORD)(fmt.nChannels * fmt.wBitsPerSample / 8);
    fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
    fmt.cbSize          = 0;

    jb_event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (jb_event == NULL)
        return 0;
    if (waveOutOpen(&jb_wave, WAVE_MAPPER, &fmt, (DWORD_PTR)jb_event, 0,
                    CALLBACK_EVENT) != MMSYSERR_NOERROR) {
        CloseHandle(jb_event);
        jb_event = NULL;
        jb_wave  = NULL;
        return 0;
    }

    InitializeCriticalSection(&jb_lock);
    jb_lock_ready = 1;
    Mod_Init(JB_RATE);

    for (i = 0; i < JB_BUFFERS; i++) {
        jb_bufs[i] = (short *)calloc(JB_FRAMES, sizeof(short));
        if (jb_bufs[i] == NULL) {
            Platform_AudioShutdown();
            return 0;
        }
        memset(&jb_hdr[i], 0, sizeof jb_hdr[i]);
        jb_hdr[i].lpData         = (LPSTR)jb_bufs[i];
        jb_hdr[i].dwBufferLength = JB_FRAMES * sizeof(short);
        if (waveOutPrepareHeader(jb_wave, &jb_hdr[i], sizeof jb_hdr[i]) !=
            MMSYSERR_NOERROR) {
            Platform_AudioShutdown();
            return 0;
        }
        if (waveOutWrite(jb_wave, &jb_hdr[i], sizeof jb_hdr[i]) !=
            MMSYSERR_NOERROR) {
            Platform_AudioShutdown();
            return 0;
        }
    }

    jb_feeding = 1;
    jb_thread  = CreateThread(NULL, 0, FeedThread, NULL, 0, &tid);
    if (jb_thread == NULL) {
        Platform_AudioShutdown();
        return 0;
    }
    return 1;
}

void Platform_AudioPause(int pause)
{
    if (jb_wave == NULL)
        return;
    if (pause)
        waveOutPause(jb_wave);
    else
        waveOutRestart(jb_wave);
}

/* jumpyball JumpyBall.exe Music_LoadForContext 0x0001db68 */
int Platform_MusicPlay(const char *path, int volume, int loop)
{
    unsigned char *buf;
    long           size = 0;
    int            ok;

    if (jb_wave == NULL)
        return 0;
    buf = Platform_ReadFile(path, &size);
    if (buf == NULL)
        return 0;
    if (size > 0x400000) {
        free(buf);
        return 0;
    }
    Lock();
    ok = Mod_Play(buf, (int)size, volume, loop);
    Unlock();
    return ok;
}

/* jumpyball hss.dll hssSpeaker::stopMusics 0x001042f0 */
void Platform_MusicStop(void)
{
    Lock();
    Mod_Stop();
    Unlock();
}

/* jumpyball hss.dll hssSpeaker::volumeMusics 0x0010567c */
void Platform_MusicMasterVolume(int volume)
{
    Lock();
    Mod_MasterVolume(volume);
    Unlock();
}

static unsigned Rd16(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

static unsigned Rd32(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8) |
           ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24);
}

/* Sounds/sblam.wav is 11025 Hz mono 16-bit and Sounds/smenu.wav is 11025 Hz
   mono 8-bit, so both need the resample SDL_BuildAudioCVT did. */
static int LoadWav(const char *path, short **out, unsigned *out_frames)
{
    unsigned char *f;
    long           len = 0;
    unsigned       ofs, chunk, size, rate = 0, chans = 0, bits = 0, fmt = 0;
    unsigned       in_frames, i;
    const unsigned char *data = NULL;
    unsigned       data_size = 0;
    short         *pcm;

    f = Platform_ReadFile(path, &len);
    if (f == NULL)
        return 0;
    if (len < 44 || memcmp(f, "RIFF", 4) != 0 || memcmp(f + 8, "WAVE", 4) != 0) {
        free(f);
        return 0;
    }
    ofs = 12;
    while (ofs + 8 <= (unsigned)len) {
        chunk = Rd32(f + ofs + 4);
        if (chunk > (unsigned)len - ofs - 8)
            break;
        if (memcmp(f + ofs, "fmt ", 4) == 0 && chunk >= 16) {
            fmt   = Rd16(f + ofs + 8);
            chans = Rd16(f + ofs + 10);
            rate  = Rd32(f + ofs + 12);
            bits  = Rd16(f + ofs + 22);
        } else if (memcmp(f + ofs, "data", 4) == 0) {
            data      = f + ofs + 8;
            data_size = chunk;
        }
        ofs += 8 + chunk + (chunk & 1u);
    }
    if (fmt != WAVE_FORMAT_PCM || data == NULL || rate == 0 ||
        (chans != 1 && chans != 2) || (bits != 8 && bits != 16)) {
        free(f);
        return 0;
    }

    size      = chans * bits / 8;
    in_frames = data_size / size;
    if (in_frames == 0) {
        free(f);
        return 0;
    }
    /* Nearest-neighbour rate conversion to the 22050 Hz device format. */
    *out_frames = (unsigned)(((double)in_frames * JB_RATE) / (double)rate);
    if (*out_frames == 0)
        *out_frames = 1;
    pcm = (short *)malloc((size_t)*out_frames * sizeof(short));
    if (pcm == NULL) {
        free(f);
        return 0;
    }
    for (i = 0; i < *out_frames; i++) {
        unsigned             src = (unsigned)(((double)i * rate) / JB_RATE);
        const unsigned char *p;
        int                  acc = 0, c;

        if (src >= in_frames)
            src = in_frames - 1;
        p = data + (size_t)src * size;
        for (c = 0; c < (int)chans; c++) {
            if (bits == 8)
                acc += ((int)p[c] - 128) * 256;
            else
                acc += (int)(short)(unsigned short)Rd16(p + c * 2);
        }
        pcm[i] = (short)(acc / (int)chans);
    }
    free(f);
    *out = pcm;
    return 1;
}

int Platform_SoundLoad(int slot, const char *path, int volume)
{
    short   *pcm    = NULL;
    unsigned frames = 0;

    if (jb_wave == NULL || slot < 0 || slot >= JB_SOUND_SLOTS)
        return 0;
    if (!LoadWav(path, &pcm, &frames))
        return 0;
    Lock();
    free(jb_sounds[slot].data);
    jb_sounds[slot].data   = pcm;
    jb_sounds[slot].frames = frames;
    jb_sounds[slot].volume = volume;
    Unlock();
    return 1;
}

/* JumpyBall.exe WndProc 0x0001fd2c plays g_sndBounce on the g_ballY < 0.0
   landing branch and g_sndMenuClick on the menu up/down/left branches, both
   through hssSpeaker::playSound. */
void Platform_SoundPlay(int slot)
{
    int i;

    if (jb_wave == NULL || slot < 0 || slot >= JB_SOUND_SLOTS ||
        jb_sounds[slot].data == NULL)
        return;

    Lock();
    for (i = 0; i < JB_VOICES; i++) {
        if (jb_voices[i].slot < 0) {
            jb_voices[i].slot = slot;
            jb_voices[i].pos  = 0;
            break;
        }
    }
    Unlock();
}

void Platform_AudioShutdown(void)
{
    int i;

    if (jb_thread != NULL) {
        jb_feeding = 0;
        SetEvent(jb_event);
        WaitForSingleObject(jb_thread, 2000);
        CloseHandle(jb_thread);
        jb_thread = NULL;
    }
    jb_feeding = 0;
    if (jb_wave != NULL) {
        waveOutReset(jb_wave);
        for (i = 0; i < JB_BUFFERS; i++) {
            if (jb_hdr[i].dwFlags & WHDR_PREPARED)
                waveOutUnprepareHeader(jb_wave, &jb_hdr[i], sizeof jb_hdr[i]);
        }
        waveOutClose(jb_wave);
        jb_wave = NULL;
    }
    if (jb_event != NULL) {
        CloseHandle(jb_event);
        jb_event = NULL;
    }
    Mod_Stop();
    for (i = 0; i < JB_BUFFERS; i++) {
        free(jb_bufs[i]);
        jb_bufs[i] = NULL;
    }
    for (i = 0; i < JB_SOUND_SLOTS; i++) {
        free(jb_sounds[i].data);
        jb_sounds[i].data = NULL;
    }
    if (jb_lock_ready) {
        DeleteCriticalSection(&jb_lock);
        jb_lock_ready = 0;
    }
}
