/*
  ZynAddSubFX - a software synthesizer

  Looper.C - Looper effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Looper effect by Transmogrifox

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Looper.h"

Looper::Looper(float size, double samplerate, uint32_t intermediate_bufsize) :
    Effect(size, samplerate, intermediate_bufsize),
    Pplay(),
    Pstop(1),
    looper_bar(),
    looper_qua(),
    progstate(),
    ticker(samplerate, intermediate_bufsize),
    fSAMPLE_RATE(float(samplerate)),
    SAMPLE_RATE(samplerate),
    PERIOD(intermediate_bufsize),
    Pvolume(),
    Precord(),
    Pclear(1),
    Preverse(),
    Pfade1(),
    Pfade2(),
    PT1(1),
    PT2(),
    Pautoplay(),
    Prec1(),
    Prec2(),
    Plink(),
    Ptempo(120),
    Pbar(2),
    Pmetro(),
    Pms(),
    kl(),
    kl2(),
    rvkl(),
    rvkl2(),
    maxx_delay(lrintf(fSAMPLE_RATE * size)),
    fade((int) SAMPLE_RATE / 2),            // 1/2 SR fade time available
    dl(),
    dl2(),
    first_time1(1),
    first_time2(1),
    rplaystate(),
    barlen(1),
    looper_ts(1),
    ldelay(NULL),
    rdelay(NULL),
    t2ldelay(NULL),
    t2rdelay(NULL),
    oldl(),
    oldr(),
    Srate_Attack_Coeff(1.0f / (fSAMPLE_RATE * ATTACK)),
    track1gain(),
    track2gain(),
    fade1(),
    fade2(),
    pregain1(),
    pregain2(),
    mvol()
{
    //default values
    settempo(120);
    setbar(2);
    ticker.cleanup();

    ldelay = new float[maxx_delay];
    rdelay = new float[maxx_delay];
    t2ldelay = new float[maxx_delay];
    t2rdelay = new float[maxx_delay];

    Looper::setpreset(Ppreset);
    Looper::cleanup();
}

Looper::~Looper()
{
    delete[] ldelay;
    delete[] rdelay;
    delete[] t2ldelay;
    delete[] t2rdelay;
}

/*
 * Cleanup the effect
 */
void
Looper::cleanuppt1()
{
    for (int i = 0; i < maxx_delay; i++)
    {
        ldelay[i] = 0.0f;
        rdelay[i] = 0.0f;
    }
}

void
Looper::cleanuppt2()
{
    for (int i = 0; i < maxx_delay; i++)
    {
        t2ldelay[i] = 0.0f;
        t2rdelay[i] = 0.0f;
    }
}

void Looper::cleanup()
{
    cleanuppt1();
    cleanuppt2();
}

/*
 * Initialize the delays
 */
void
Looper::initdelays()
{
    if (PT1)
    {
        kl = 0;
        ticker.cleanup();
        dl = maxx_delay;
        rvkl = dl - 1;
        cleanuppt1();
    }

    if (PT2)
    {
        kl2 = 0;
        ticker.cleanup();
        dl2 = maxx_delay;
        rvkl2 = dl2 - 1;
        cleanuppt2();
    }
    
    Srate_Attack_Coeff = 90.0f / (dl + dl2); // Set swell time
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Looper::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    ticker.lv2_update_params(period);
    ticker.cleanup();
    cleanup();
}
#endif

/*
 * Effect output
 */
void
Looper::out(float * efxoutl, float * efxoutr)
{
    float ticktock[PERIOD];
    bool play_metro = false;
    
    if ((Pmetro) && (Pplay) && (!Pstop))
    {
        ticker.metronomeout(ticktock, PERIOD);
        play_metro = true;
    }

    for (unsigned i = 0; i < PERIOD; i++)
    {
        if ((Pplay) && (!Pstop))
        {
            if (Precord)
            {
                if ((Prec1) && (PT1))
                {
                    ldelay[kl] += pregain1 * efxoutl[i];
                    rdelay[kl] += pregain1 * efxoutr[i];
                }
                if ((Prec2) && (PT2))
                {
                    t2ldelay[kl2] += pregain2 * efxoutl[i];
                    t2rdelay[kl2] += pregain2 * efxoutr[i];
                }
            }

            if (!Pclear)
            {
                if (++kl >= dl)
                {
                    kl = 0;
                }
                
                rvkl = dl - 1 - kl;
                
                if (rvkl < 0 )
                {
                    rvkl = 0;
                }
                
                if (rvkl >= maxx_delay)
                {
                    rvkl = maxx_delay -1;
                }
                
                if (++kl2 >= dl2)
                {
                    kl2 = 0;
                }
                
                rvkl2 = dl2 - 1 - kl2;
                
                if (rvkl2 < 0)
                {
                    rvkl2 = 0;
                }
                
                if (rvkl2 >= maxx_delay)
                {
                    rvkl2 = maxx_delay -1;
                }
                
                if ((Plink) || (PT1))
                {
                    timeposition(kl);
                }
                else
                {
                    timeposition(kl2);
                }
            }

            if (Preverse)
            {
                float lswell = (float) (abs(kl - rvkl)) * Srate_Attack_Coeff;
                
                if (lswell <= PI)
                {
                    lswell = 0.5f * (1.0f - cosf(lswell)); //Clickless transition
                    efxoutl[i] = (fade1 * ldelay[rvkl] + fade2 * t2ldelay[rvkl2]) * lswell; //Volume ducking near zero crossing.
                }
                else
                {
                    efxoutl[i] = fade1 * ldelay[rvkl] + fade2 * t2ldelay[rvkl2];
                }

                float rswell = (float) (abs(kl - rvkl)) * Srate_Attack_Coeff;
                
                if (rswell <= PI)
                {
                    rswell = 0.5f * (1.0f - cosf(rswell)); //Clickless transition
                    efxoutr[i] = (fade1 * rdelay[rvkl] + fade2 * t2rdelay[rvkl2]) * rswell; //Volume ducking near zero crossing.
                }
                else
                {
                    efxoutr[i] = fade1 * rdelay[rvkl] + fade2 * t2rdelay[rvkl2];
                }
            }
            else
            {
                efxoutl[i] = fade1 * ldelay[kl] + fade2 * t2ldelay[kl2];
                efxoutr[i] = fade1 * rdelay[kl] + fade2 * t2rdelay[kl2];
            }
        }
        else
        {
            efxoutl[i] = 0.0f;
            efxoutr[i] = 0.0f;
        }

        if (play_metro)
        {
            efxoutl[i] += ticktock[i] * mvol; //if you want to hear the metronome in Looper
            efxoutr[i] += ticktock[i] * mvol;
        }
    }
}

/*
 * Parameter control
 */

void
Looper::setvolume(int volume)
{
    Pvolume = volume;
    outvolume = (float) volume / 127.0f;
}

void Looper::setmvol(int value)
{
    mvol = 2.0f * (float) value / 100.0f;
}

void Looper::settempo(int value)
{
    Ptempo = value;
    setbar(Pbar);
    ticker.set_tempo(value);
}

int Looper::set_len(int value)
{
    int x = value / barlen;

    int a1 = abs(value - (x * barlen));
    int a2 = abs(value - ((x + 1) * barlen));

    if (a2 < a1)
    {
        return (((x + 1) * barlen) + 1);
    }

    return (x * barlen + 1);
}

int Looper::cal_len(int value)
{
    int lenbar = lrintf((float) value * 60.0 / (float) Ptempo * fSAMPLE_RATE);

    looper_ts = lenbar / value;
//    printf("looper_ts = %d: lenbar = %d: value = %d: barlen = %d: looper_bar = %d: looper_qua = %d\n",
//            looper_ts, lenbar, value, barlen, looper_bar, looper_qua);
    
    return (lenbar);
    // return(lrintf((float)value*60.0/(float)Ptempo*fSAMPLE_RATE));
}

/**
 *  This function calculates the time signature that is displayed in the box to the right
 *  of the metronome check box. It works in a limited way that sometimes appears broken.
 *  The value sent is the range of the PERIOD for each cycle. Since this range may be less
 *  than the full bar of the time signature when the tempo is slow, it may only show a single
 *  beat. If the tempo is fast, it can show multiple bars and beats. It would be more useful
 *  if the bars and beats reflected a time range equivalent to the recording length set in
 *  Settings/Audio/Looper Size Seconds. FIXME.
 * 
 * @param value
 *      The range of PERIOD.
 */
void
Looper::timeposition(int value)
{
//    printf("looper_ts = %d: value = %d: barlen = %d: looper_bar = %d: looper_qua = %d\n",
//            looper_ts, value, barlen, looper_bar, looper_qua);

    looper_bar = value / barlen + 1;
    looper_qua = (value % barlen) / looper_ts + 1;
}

void Looper::setbar(int value)
{
    Pbar = value;
    switch (value)
    {
    case 0:
        barlen = cal_len(2);
        if (!Pms)ticker.set_meter(2);
        break;
    case 1:
        barlen = cal_len(3);
        if (!Pms)ticker.set_meter(3);
        break;
    case 2:
        barlen = cal_len(4);
        if (!Pms)ticker.set_meter(4);
        break;
    case 3:
        barlen = cal_len(5);
        if (!Pms)ticker.set_meter(5);
        break;
    case 4:
        barlen = cal_len(6);
        if (!Pms)ticker.set_meter(6);
        break;
    case 5:
        barlen = cal_len(7);
        if (!Pms)ticker.set_meter(7);
        break;
    case 6:
        barlen = cal_len(9);
        if (!Pms)ticker.set_meter(9);
        break;
    case 7:
        barlen = cal_len(11);
        if (!Pms)ticker.set_meter(11);
        break;
    }
}

void Looper::setfade()
{
    fade1 = (float) (Pfade1) / 64.0f;
    fade2 = (float) (Pfade2) / 64.0f;
    pregain1 = 1.015f / (fade1 + 0.015f); //this is so material being recorded mixes at the same level as what you hear
    pregain2 = 1.015f / (fade2 + 0.015f); //magic number is ~1/66, keeps record gain from getting too high
    fade1 *= track1gain;
    fade2 *= track2gain;
}

void
Looper::setpreset(int npreset)
{
    const int PRESET_SIZE = C_LOOPER_PARAMETERS;
    const int NUM_PRESETS = 2;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Looper 2 seconds
        {64, 0, 1, 0, 1, 0, 64, 1, 0, 1, 64, 1, 0, 0},
        {64, 0, 1, 0, 1, 1, 64, 1, 0, 1, 64, 1, 0, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_LOOPER, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            set_value(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            set_value(n, presets[npreset][n]);
    }
    
    Ppreset = npreset;
}

/**
 * Used by Gui and MIDI control to change parameters. Some of the parameters
 * are toggled, Play, Stop, etc. So this is not used for setting presets and 
 * file, bank setting. The set_value() function is used for presets, etc. since
 * it does not toggle.
 * 
 * @param npar
 *      The case to set.
 * 
 * @param value
 *      The value to set.
 */
void
Looper::changepar(int npar, int value)
{
    switch (npar)
    {
    case Looper_DryWet:
        setvolume(value);
        break;

    case Looper_Play: // Play at current pointer position

        if (Pplay)
        {
            Pplay = 0;
        }
        else
        {
            Pplay = (PT1 || PT2); //prevents accidental record state when neither track is active
            if (!Pstop)
            {
                //if stop was pushed last time, then resume where it left off
                if (PT1)kl = 0;
                if (PT2)kl2 = 0;
                ticker.cleanup();
            }
            Pstop = 0;
        }
        if (Pstop)
        {
            Pstop = 0;
            Pplay = 0;
        }
        Pclear = 0;

        break;

    case Looper_Stop: // stop and reset pointer to 0
        if (Pstop) Pstop = 0;
        else Pstop = 1;
        if (Precord)
        {
            Pstop = 0;
            if ((first_time1 && Prec1) && PT1)
            {
                dl = set_len(kl);
                //printf("dl %d\n",dl);
                first_time1 = 0;
                if (Plink)
                {
                    dl2 = dl;
                }
                kl = 0;
                ticker.cleanup();

            }
            if ((first_time2 && Prec2) && PT2)
            {
                dl2 = set_len(kl2);
                first_time2 = 0;
                if (Plink)
                {
                    dl = dl2;
                }
                kl2 = 0;
                ticker.cleanup();
            }

            Precord = 0;
            Pplay = 0;
        }
        break;
    case Looper_Record: // Record at current position.  If first time (clear = true), then set end of loop, "dl"
        if (Precord)
        {
            if ((first_time1 && Prec1) && PT1)
            {
                dl = set_len(kl);
                first_time1 = 0;
                if (Plink)
                {
                    dl2 = dl;
                }
                kl = 0;
                ticker.cleanup();
            }
            if ((first_time2 && Prec2) && PT2)
            {
                dl2 = set_len(kl2);
                first_time2 = 0;
                if (Plink)
                {
                    dl = dl2;
                }
                kl2 = 0;
                ticker.cleanup();
            }
            Precord = 0;
            Pplay = rplaystate;
            if (Pautoplay) Pplay = 1;
        }
        else
        {
            Precord = 1;
            rplaystate = Pplay;
            if (Pautoplay)
            {
                Pplay = 1;
                if ((first_time1 && Prec1) || (first_time2 && Prec2)) ticker.cleanup();
            }
            else Pplay = 0;
        }
        Pstop = 0;
        Pclear = 0;
        break;
    case Looper_Clear:
        Pclear = 1; // Clear everything and erase the loop
        if (PT1) first_time1 = 1;
        if (PT2) first_time2 = 1;
        if ((PT1) && (PT2)) Pplay = 0;
        Precord = 0;
        if (PT1)kl = 0;
        if (PT2)kl2 = 0;
        ticker.cleanup();
        initdelays();
        break;
    case Looper_Reverse:
    {
        int i = 0;
        if (value) i = 1;
        Preverse = i; // Playback in reverse
        break;
    }
    case Looper_Level_1:
        Pfade1 = value;
        setfade();
        break;
    case Looper_Track_1:
        if (PT1)
        {
            PT1 = 0;
        }
        else
        {
            PT1 = 1;
            if (!Plink)
            {
                if (first_time1)initdelays();
                PT2 = 0; //if they aren't linked in time, then you want to toggle 1&2
                track2gain = 0.0f;
                kl = 0;
                ticker.cleanup();
                setfade();
            }

        }
        track1gain = (float) PT1;
        setfade();
        break;
    case Looper_Track_2:
        if (PT2)
        {
            PT2 = 0;
        }
        else
        {
            PT2 = 1;
            if (!Plink)
            {
                PT1 = 0;
                if (first_time2)initdelays();
                kl2 = 0;
                ticker.cleanup();
                track1gain = 0.0f;
            }

        }
        track2gain = (float) PT2;
        setfade();
        break;
    case Looper_AutoPlay:
    {
        int i = 0;
        if (value) i = 1;
        Pautoplay = i;
        break;
    }
    case Looper_Level_2:
        Pfade2 = value;
        setfade();
        break;
    case Looper_Rec_1:  // Record 1
    {
        int i = 0;
        if (value) i = 1;
        Prec1 = i;
        break;
    }
    case Looper_Rec_2:  // Record 2
    {
        int i = 0;
        if (value) i = 1;
        Prec2 = i;
        break;
    }
    case Looper_Link:   // Link track 1 and track 2
        Plink = value;
        if (Plink)
        {
            if (Prec1) dl2 = dl;
            if (Prec2) dl = dl2; // if both are true, then it is only a redundant assignment
        }
        break;

    case Looper_Tempo:
        settempo(value);
        break;
    case Looper_Bar:     // Time Signature
        setbar(value);
        break;
    case Looper_Metro:   // Play Metronome
        Pmetro = value;
        break;
    case Looper_M_S:     // Set metronome sound
        Pms = value;
        if (Pms == 0) setbar(Pbar);
        if (Pms == 1) ticker.set_meter(1);
        if (Pms == 2) ticker.set_meter(0);
        break;
    default:
        return;
    }
}

int
Looper::getpar(int npar)
{
    switch (npar)
    {
    case Looper_DryWet:
        return (Pvolume);

    case Looper_Play:
        return (Pplay);

    case Looper_Stop:
        return (Pstop);

    case Looper_Record:
        return (Precord);

    case Looper_Clear:
        return (Pclear);

    case Looper_Reverse:
        return (Preverse);

    case Looper_Level_1:
        return (Pfade1);

    case Looper_Track_1:
        return (PT1);

    case Looper_Track_2:
        return (PT2);

    case Looper_AutoPlay:
        return (Pautoplay);

    case Looper_Level_2:
        return (Pfade2);

    case Looper_Rec_1:
        return (Prec1);

    case Looper_Rec_2:
        return (Prec2);

    case Looper_Link:
        return (Plink);

    case Looper_Tempo:
        return (Ptempo);

    case Looper_Bar:
        return (Pbar);

    case Looper_Metro:
        return (Pmetro);

    case Looper_M_S:
        return (Pms);

    }
    return (0); //in case of bogus parameter number
}

void Looper::getstate()
{
    progstate[0] = Pplay;
    progstate[1] = Pstop;
    progstate[2] = Precord;
    progstate[3] = Pclear;
    progstate[4] = PT1;
    progstate[5] = PT2;
}

/**
 * This is for setting the value of the parameters. The changepar() function will toggle the
 * variables for play, stop etc. This is used for setting presets and file, bank loading.
 * 
 * @param npar
 *      The case variable to set.
 * 
 * @param value
 *      The value to set.
 */
void
Looper::set_value(int npar, int value)
{
    switch (npar)
    {
    case Looper_DryWet:
        Pvolume = value;
        outvolume = (float) Pvolume / 127.0f;

        break;
    case Looper_Play:
        Pplay = value;
        break;
    case Looper_Stop:
        Pstop = value;
        break;
    case Looper_Record:
        Precord = value;
        break;
    case Looper_Clear:
        Pclear = 1;
        initdelays();
        break;
    case Looper_Reverse:
        Preverse = value;
        break;
    case Looper_Level_1:
        Pfade1 = value;
        setfade();
        break;
    case Looper_Track_1:
        PT1 = value;
        track1gain = (float) PT1;
        setfade();
        break;
    case Looper_Track_2:
        PT2 = value;
        track2gain = (float) PT2;
        setfade();
        break;
    case Looper_AutoPlay:
        Pautoplay = value;
        break;
    case Looper_Level_2:
        Pfade2 = value;
        setfade();
        break;
    case Looper_Rec_1:
        Prec1 = value;
        break;
    case Looper_Rec_2:
        Prec2 = value;
        break;
    case Looper_Link:
        Plink = value;
        break;
    }

    Pclear = 1; //Clear everything and erase the loop
    if (PT1) first_time1 = 1;
    if (PT2) first_time2 = 1;
    if ((PT1) && (PT2)) Pplay = 0;
    Precord = 0;
    getstate();
}

