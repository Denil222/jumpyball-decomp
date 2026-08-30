#include "../jb_mod.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RATE    22050
#define SECONDS 400

static void PutU32(FILE *f, unsigned v)
{
    fputc(v & 0xff, f);
    fputc((v >> 8) & 0xff, f);
    fputc((v >> 16) & 0xff, f);
    fputc((v >> 24) & 0xff, f);
}

static void PutU16(FILE *f, unsigned v)
{
    fputc(v & 0xff, f);
    fputc((v >> 8) & 0xff, f);
}

int main(int argc, char **argv)
{
    FILE          *f;
    long           size;
    unsigned char *buf;
    short         *pcm;
    int            frames = RATE * SECONDS, i, vol;
    long long      sum = 0;
    int            peak = 0, nonzero = 0, last = 0;

    if (argc < 3) {
        printf("usage: modtest <in.tkm> <out.wav> [volume]\n");
        return 2;
    }
    vol = argc > 3 ? (int)strtol(argv[3], 0, 0) : 0x40;

    f = fopen(argv[1], "rb");
    if (f == 0) {
        printf("FAIL open %s\n", argv[1]);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = (unsigned char *)malloc((size_t)size);
    if (buf == 0 || fread(buf, 1, (size_t)size, f) != (size_t)size) {
        printf("FAIL read %s\n", argv[1]);
        return 1;
    }
    fclose(f);

    printf("%s: %ld bytes  magic %.4s  songlen %d  restart %d\n", argv[1], size,
           (const char *)buf + 0x438, buf[0x3b6], buf[0x3b7]);

    Mod_Init(RATE);
    if (!Mod_Play(buf, (int)size, vol, argc > 4 ? (int)strtol(argv[4], 0, 0) : 1)) {
        printf("FAIL Mod_Play\n");
        return 1;
    }

    pcm = (short *)malloc((size_t)frames * 2);
    if (pcm == 0)
        return 1;
    Mod_Render(pcm, frames);

    for (i = 0; i < frames; i++) {
        int v = pcm[i] < 0 ? -pcm[i] : pcm[i];

        sum += (long long)pcm[i] * pcm[i];
        if (v > peak)
            peak = v;
        if (pcm[i] != 0) {
            nonzero++;
            last = i;
        }
    }

    f = fopen(argv[2], "wb");
    if (f == 0) {
        printf("FAIL open %s\n", argv[2]);
        return 1;
    }
    fwrite("RIFF", 1, 4, f);
    PutU32(f, 36 + (unsigned)frames * 2);
    fwrite("WAVEfmt ", 1, 8, f);
    PutU32(f, 16);
    PutU16(f, 1);
    PutU16(f, 1);
    PutU32(f, RATE);
    PutU32(f, RATE * 2);
    PutU16(f, 2);
    PutU16(f, 16);
    fwrite("data", 1, 4, f);
    PutU32(f, (unsigned)frames * 2);
    fwrite(pcm, 2, (size_t)frames, f);
    fclose(f);

    printf("  rendered %d frames (%d s)  peak %d  rms %.1f  nonzero %.2f%%  "
           "last audio at %.2f s\n",
           frames, SECONDS, peak,
           frames ? (double)((long long)(sum / frames)) : 0.0,
           100.0 * nonzero / frames, (double)last / RATE);
    printf("  %s\n", peak > 1000 && nonzero * 10 > frames ? "PASS" : "FAIL");
    return 0;
}
