#include "jb_mod_priv.h"

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void VolSlide(mod_track *t)
{
    t->vol += t->volslide;
    if (t->vol < 0) t->vol = 0;
    if (t->vol > 0x40) t->vol = 0x40;
    t->mixvol = (t->vol * jb_M.volbase) >> 6;
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void RowLoop(int ch, mod_track *t)
{
    int b = jb_M.loopcnt[ch], p = t->parlo, hi = 0xf0;

    if ((b & 0xf0) == 0) {
        if (p == 0) {
            jb_M.loopstart[ch] = (unsigned char)jb_M.cellofs;
            return;
        }
    } else {
        if (p == 0) return;
        p = (b & 0xf) - 1;
    }
    if (p == 0) {
        hi = 0;
    } else {
        jb_M.jumporder = (unsigned int)jb_M.orderidx;
        jb_M.jumpcell  = jb_M.loopstart[ch];
    }
    jb_M.loopcnt[ch] = (unsigned char)(p | hi);
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void RowEffectE(int ch, mod_track *t, int cellperiod)
{
    switch (t->parhi) {
    case 0x6:
        RowLoop(ch, t);
        break;
    case 0xa:
        t->vol += t->parlo;
        if (t->vol > 0x40) t->vol = 0x40;
        t->mixvol = (t->vol * jb_M.volbase) >> 6;
        break;
    case 0xb:
        t->vol -= t->parlo;
        if (t->vol < 0) t->vol = 0;
        t->mixvol = (t->vol * jb_M.volbase) >> 6;
        break;
    case 0xc:
        t->notecut = t->parlo;
        break;
    case 0xd:
        t->notedelayperiod = cellperiod;
        t->notedelay       = t->parlo - 1;
        break;
    case 0xe:
        jb_M.patdelay = t->parlo;
        break;
    }
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void RowEffectHi(int ch, mod_track *t, int cellperiod)
{
    int param = t->param;

    switch (t->effect) {
    case 0xb:
        if (!jb_M.loop && (unsigned int)param <= (unsigned int)jb_M.orderidx) {
            jb_M.jumporder = (unsigned int)jb_M.songlen;
        } else {
            jb_M.jumporder = (unsigned int)param;
            jb_M.jumpcell  = 0;
        }
        break;
    case 0xc:
        t->vol    = param;
        t->mixvol = (param * jb_M.volbase) >> 6;
        break;
    case 0xd:
        if ((unsigned int)(jb_M.orderidx + 1) < (unsigned int)jb_M.songlen) {
            jb_M.jumporder = (unsigned int)(jb_M.orderidx + 1);
            jb_M.jumpcell  = (unsigned int)(t->parlo + t->parhi * 10) * 4;
        } else if (!jb_M.loop) {
            jb_M.jumporder = (unsigned int)jb_M.songlen;
        } else {
            jb_M.jumporder = (unsigned int)jb_M.restart;
            jb_M.jumpcell  = (unsigned int)(t->parlo + t->parhi * 10) * 4;
        }
        break;
    case 0xe:
        RowEffectE(ch, t, cellperiod);
        break;
    case 0xf:
        if (param > 0x20) {
            jb_M.tickhz = (param * 2) / 5;
            Mod_SetTempo();
        } else if (param != 0) {
            jb_M.speed = param;
        }
        break;
    }
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void RowEffect(int ch, mod_track *t, int cellperiod)
{
    int param = t->param;

    switch (t->effect) {
    case 0x0:
        if (param != 0) {
            t->arpstep[0] = t->step;
            t->arpphase   = 0;
            t->arpstep[1] = Mod_Step(Mod_Period(t->parhi + t->noteidx));
            t->arpstep[2] = Mod_Step(Mod_Period(t->parlo + t->noteidx));
        }
        break;
    case 0x1:
    case 0x2:
        t->portabase = t->period;
        break;
    case 0x3:
        if (cellperiod == 0) {
            if (t->portatarget == 0) t->portatarget = Mod_Period(t->noteidx);
        } else {
            t->portatarget = Mod_Period(jb_M.smp[t->sample].finetune * 0x54 +
                                        jb_M.note[cellperiod]);
        }
        if (param != 0) t->portaspeed = param;
        break;
    case 0x4:
        if (param != 0) {
            t->vibspeed = t->parhi;
            t->vibpos   = 0;
            t->vibdepth = t->parlo;
        }
        break;
    case 0x5:
    case 0x6:
    case 0xa:
        if (param == 0)         t->volslide = 0;
        else if (t->parhi == 0) t->volslide = -t->parlo;
        else                    t->volslide = t->parhi;
        break;
    case 0x9:
        if (param != 0)
            t->sampleofs = (unsigned int)(t->parlo + t->parhi * 0x10) * 0x1000000u;
        t->pos = t->sampleofs;
        break;
    default:
        RowEffectHi(ch, t, cellperiod);
        break;
    }
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
void Mod_ProcessRow(void)
{
    int ch;

    for (ch = 0; ch < jb_M.chans; ch++) {
        mod_track   *t = &jb_M.tr[ch];
        unsigned int w = Mod_Cell(ch);
        int sample     = (int)((w & 0xf0) | ((w & 0xffffff) >> 20));
        int cellperiod = (int)(((w & 0xffff) >> 8) | ((w & 0xf) << 8));

        t->param  = (int)(w >> 24);
        t->effect = (int)((w & 0xfffff) >> 16);
        t->parhi  = (int)(w >> 28);
        t->parlo  = t->param & 0xf;

        if (sample != 0) {
            t->sample = sample;
            t->vol    = jb_M.smp[sample].vol;
            t->mixvol = (t->vol * jb_M.volbase) >> 6;
        }
        if (cellperiod != 0 && t->effect != 3 && t->effect != 5 &&
            t->parhi + t->effect * 0x10 != 0xed) {
            t->pos     = 0;
            t->noteidx = jb_M.smp[t->sample].finetune * 0x54 +
                         jb_M.note[cellperiod];
            t->period  = Mod_Period(t->noteidx);
        }
        if (t->sample != 0) t->step = Mod_Step(t->period);
        RowEffect(ch, t, cellperiod);
    }
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
void Mod_AdvanceRow(void)
{
    if (jb_M.jumporder == MOD_NOJUMP) {
        jb_M.cellofs += jb_M.chans;
        if (jb_M.cellofs >= jb_M.chans * 0x40) {
            jb_M.cellofs = 0;
            jb_M.orderidx++;
            if (jb_M.orderidx >= jb_M.songlen) {
                if (!jb_M.loop) {
                    jb_M.playing = 0;
                    return;
                }
                jb_M.orderidx = jb_M.restart;
            }
            jb_M.patno = jb_M.file[MOD_ORDER_OFS + jb_M.orderidx];
        }
        return;
    }
    if (jb_M.jumporder >= (unsigned int)jb_M.songlen) {
        jb_M.playing = 0;
        return;
    }
    jb_M.orderidx  = (int)jb_M.jumporder;
    jb_M.patno     = jb_M.file[MOD_ORDER_OFS + jb_M.orderidx];
    jb_M.cellofs   = (int)jb_M.jumpcell;
    jb_M.jumporder = MOD_NOJUMP;
    if (jb_M.cellofs >= jb_M.chans * 0x40) jb_M.cellofs = 0;
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void TickEffectE(mod_track *t)
{
    if (t->parhi == 0xc) {
        if (t->notecut != 0) t->notecut--;
        if (t->notecut == 0) t->mixvol = 0;
    } else if (t->parhi == 0xd) {
        if (t->notedelay == -1) {
            t->notedelay = (int)MOD_NOJUMP;
            t->period    = t->notedelayperiod;
            t->step      = Mod_Step(t->notedelayperiod);
        } else if (t->notedelay != (int)MOD_NOJUMP) {
            t->notedelay--;
        }
    }
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void TickPorta(mod_track *t)
{
    unsigned int tgt = (unsigned int)t->portatarget, np;

    if (tgt < (unsigned int)t->period) {
        np        = (unsigned int)(t->period - t->portaspeed);
        t->period = (int)np;
        if (np < tgt || (int)np < 0) t->period = (int)tgt;
    }
    if ((unsigned int)t->period < tgt) {
        np        = (unsigned int)(t->portaspeed + t->period);
        t->period = (int)np;
        if (tgt < np) t->period = (int)tgt;
    }
    t->step = Mod_Step(t->period);
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
static void TickVibrato(mod_track *t)
{
    int vp = t->vibpos, per;

    if (vp < 0x20)
        per = t->period + ((jb_mod_vibrato[vp] * t->vibdepth) >> 7);
    else
        per = t->period - ((jb_mod_vibrato[vp - 0x20] * t->vibdepth) >> 7);
    vp += t->vibspeed;
    t->step = Mod_Step(per);
    if (vp > 0x3f) vp -= 0x40;
    t->vibpos = vp;
}

/* jumpyball hss.dll hssSpeaker::updateModSFX 0x00104370 */
void Mod_TickEffects(void)
{
    int ch;

    for (ch = 0; ch <= jb_M.chans; ch++) {
        mod_track *t = &jb_M.tr[ch];

        switch (t->effect) {
        case 0x0:
            if (t->param != 0) {
                if (++t->arpphase == 3) t->arpphase = 0;
                t->step = t->arpstep[t->arpphase];
            }
            break;
        case 0x1:
            t->portabase -= t->param;
            if (t->portabase < 0xd) t->portabase = 0xd;
            t->period = t->portabase;
            t->step   = Mod_Step(t->period);
            break;
        case 0x2:
            t->portabase += t->param;
            if ((unsigned int)t->portabase > MOD_MAXPERIOD)
                t->portabase = MOD_MAXPERIOD;
            t->period = t->portabase;
            t->step   = Mod_Step(t->period);
            break;
        case 0x3:
            TickPorta(t);
            break;
        case 0x5:
            TickPorta(t);
            VolSlide(t);
            break;
        case 0x4:
            TickVibrato(t);
            break;
        case 0x6:
            TickVibrato(t);
            VolSlide(t);
            break;
        case 0xa:
            VolSlide(t);
            break;
        case 0xe:
            TickEffectE(t);
            break;
        }
    }
}
