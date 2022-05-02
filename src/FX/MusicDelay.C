/*
  ZynAddSubFX - a software synthesizer

  Echo.C - Echo effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

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
#include "MusicDelay.h"

MusicDelay::MusicDelay(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(50),
    Ppanning1(64),
    Ppanning2(64),
    Pgain1(64),
    Pgain2(64),
    Pdelay1(60),
    Pdelay2(60),
    Plrdelay(100),
    Plrcross(100),
    Pfb1(40),
    Pfb2(40),
    Phidamp(60),
    Ptempo(100),
    maxx_delay(sample_rate * MAX_DELAY),
    dl1(maxx_delay - 1),
    dr1(maxx_delay - 1),
    dl2(maxx_delay - 1),
    dr2(maxx_delay - 1),
    delay1(),
    delay2(),
    lrdelay(),
    kl1(),
    kr1(),
    kl2(),
    kr2(),
    panning1(),
    panning2(),
    lrcross(),
    fb1(),
    fb2(),
    hidamp(),
    gain1(),
    gain2(),
    ldelay1(NULL),
    rdelay1(NULL),
    ldelay2(NULL),
    rdelay2(NULL),
    oldl1(),
    oldr1(),
    oldl2(),
    oldr2()
{
    ldelay1 = new float[maxx_delay];
    rdelay1 = new float[maxx_delay];
    ldelay2 = new float[maxx_delay];
    rdelay2 = new float[maxx_delay];

    setpreset(Ppreset);
    cleanup();
}

MusicDelay::~MusicDelay()
{
    delete[] ldelay1;
    delete[] rdelay1;
    delete[] ldelay2;
    delete[] rdelay2;
}

/*
 * Cleanup the effect
 */
void
MusicDelay::cleanup()
{
    for (int i = 0; i < dl1; i++)
        ldelay1[i] = 0.0;
    
    for (int i = 0; i < dr1; i++)
        rdelay1[i] = 0.0;
    
    for (int i = 0; i < dl2; i++)
        ldelay2[i] = 0.0;
    
    for (int i = 0; i < dr2; i++)
        rdelay2[i] = 0.0;

    oldl1 = 0.0;
    oldr1 = 0.0;
    oldl2 = 0.0;
    oldr2 = 0.0;
}

#ifdef LV2_SUPPORT
void
MusicDelay::lv2_update_params(uint32_t period)
{
    PERIOD = period;
}
#endif // LV2

void
MusicDelay::set_random_parameters()
{
    for(int i = 0; i < C_MUSIC_PARAMETERS; i++)
    {
        switch(i)
        {
            case Music_Delay_1:
            case Music_Delay_2:
            {
                int value = (int) (RND * 6);
                changepar (i, value + 1);
            }
            break;

            case Music_Del_Offset:
            {
                int value = (int) (RND * 7);
                changepar (i, value + 1);
            }
            break;

            case Music_DryWet:
            case Music_LR_Cross:
            case Music_Pan_1:
            case Music_Pan_2:
            case Music_Feedback_1:
            case Music_Damp:
            case Music_Feedback_2:
            case Music_Gain_1:
            case Music_Gain_2:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Music_Tempo:
            {
                int value = (int) (RND * 470);
                changepar (i, value + 10);
            }
            break;
        }
    }
}

void
MusicDelay::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_MUSIC_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Music_Delay_1:
            case Music_Del_Offset:
            case Music_LR_Cross:
            case Music_Feedback_1:
            case Music_Damp:
            case Music_Delay_2:
            case Music_Feedback_2:
            case Music_Tempo:
            case Music_Gain_1:
            case Music_Gain_2:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), mdel_parameters[i * 3 + 1], mdel_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Music_Gain_2)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Music_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Music_DryWet )), mdel_parameters[i * 3 + 1], mdel_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Music_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Music_Pan_1:
            case Music_Pan_2:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, mdel_parameters[i * 3 + 1], mdel_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ) - 64);
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

/*
 * Initialize the delays
 */
void
MusicDelay::initdelays()
{
    kl1 = 0;
    kr1 = 0;

    if (delay1 >= maxx_delay) delay1 = maxx_delay - 1;
    if (delay2 >= maxx_delay) delay2 = maxx_delay - 1;

    dl1 = delay1;
    
    if (dl1 < 1)
        dl1 = 1;
    
    dr1 = delay1;
    
    if (dr1 < 1)
        dr1 = 1;
    
    kl2 = 0;
    kr2 = 0;
    dl2 = delay2 + lrdelay;
    
    if (dl2 < 1)
        dl2 = 1;
    
    dr2 = delay2 + lrdelay;
    
    if (dr2 < 1)
        dr2 = 1;

    for (int i = dl1; i < maxx_delay; i++)
        ldelay1[i] = 0.0;
    
    for (int i = dl2; i < maxx_delay; i++)
        ldelay2[i] = 0.0;

    for (int i = dr1; i < maxx_delay; i++)
        rdelay1[i] = 0.0;
    
    for (int i = dr2; i < maxx_delay; i++)
        rdelay2[i] = 0.0;

    cleanup();
}

/*
 * Effect output
 */
void
MusicDelay::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;
    for (unsigned int i = 0; i < PERIOD; i++)
    {
        float ldl1 = ldelay1[kl1];
        float rdl1 = rdelay1[kr1];
        float l1 = ldl1 * (1.0f - lrcross) + rdl1 * lrcross;
        float r1 = rdl1 * (1.0f - lrcross) + ldl1 * lrcross;
        ldl1 = l1;
        rdl1 = r1;

        float ldl2 = ldelay2[kl2];
        float rdl2 = rdelay2[kr2];
        float l2 = ldl2 * (1.0f - lrcross) + rdl2 * lrcross;
        float r2 = rdl2 * (1.0f - lrcross) + ldl2 * lrcross;
        ldl2 = l2;
        rdl2 = r2;

        ldl1 = efxoutl[i] * gain1 * (1.0f - panning1) - ldl1 * fb1;
        rdl1 = efxoutr[i] * gain1 * panning1 - rdl1 * fb1;

        ldl2 = efxoutl[i] * gain2 * (1.0f - panning2) - ldl2 * fb2;
        rdl2 = efxoutr[i] * gain2 * panning2 - rdl2 * fb2;

        efxoutl[i] = (ldl1 + ldl2) * 2.0f;
        efxoutr[i] = (rdl1 + rdl2) * 2.0f;
        
        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }

        //LowPass Filter
        ldelay1[kl1] = ldl1 = ldl1 * hidamp + oldl1 * (1.0f - hidamp);
        rdelay1[kr1] = rdl1 = rdl1 * hidamp + oldr1 * (1.0f - hidamp);
        oldl1 = ldl1;
        oldr1 = rdl1;

        ldelay2[kl2] = ldl2 = ldl2 * hidamp + oldl2 * (1.0f - hidamp);
        rdelay2[kr2] = rdl2 = rdl2 * hidamp + oldr2 * (1.0f - hidamp);
        oldl2 = ldl2;
        oldr2 = rdl2;

        if (++kl1 >= dl1)
            kl1 = 0;
        
        if (++kr1 >= dr1)
            kr1 = 0;

        if (++kl2 >= dl2)
            kl2 = 0;
        
        if (++kr2 >= dr2)
            kr2 = 0;
    }

    if(have_nans)
        cleanup();
}

/*
 * Parameter control
 */
void
MusicDelay::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
MusicDelay::setpanning(int num, int _Ppanning)
{
    switch (num)
    {
    case 1:
        Ppanning1 = _Ppanning;
        panning1 = ((float) Ppanning1 + 0.5f) / 127.0f;
        break;
    case 2:
        Ppanning2 = _Ppanning;
        panning2 = ((float) Ppanning2 + 0.5f) / 127.0f;
        break;
    }
}

void
MusicDelay::setdelay(int num, int _Pdelay)
{
    switch (num)
    {
    case 1:
        Pdelay1 = _Pdelay;
        break;
    case 2:
        Pdelay2 = _Pdelay;
        break;
    case 3:
        Plrdelay = _Pdelay;
    }
    
    float ntem = 60.0f / (float) Ptempo;
    float coef = 0.0;

    delay1 = lrintf((ntem / (float) Pdelay1) * fSAMPLE_RATE);

    if (Plrdelay != 0)
        coef = ntem / (float) Plrdelay;
    else
        coef = 0.0;

    delay2 = lrintf((coef + (ntem / (float) Pdelay2)) * fSAMPLE_RATE);

    initdelays();
}

void
MusicDelay::setgain(int num, int _PGain)
{
    switch (num)
    {
    case 1:
        Pgain1 = _PGain;
        gain1 = (float) Pgain1 / 127.0f;
        break;
    case 2:
        Pgain2 = _PGain;
        gain2 = (float) Pgain2 / 127.0f;
        break;
    }
}

void
MusicDelay::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f * 1.0f;
}

void
MusicDelay::setfb(int num, int _Pfb)
{
    switch (num)
    {
    case 1:
        Pfb1 = _Pfb;
        fb1 = (float) Pfb1 / 127.0f;
        break;
    case 2:
        Pfb2 = _Pfb;
        fb2 = (float) Pfb2 / 127.0f;
        break;
    }
}

void
MusicDelay::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = 1.0f - (float) _Phidamp / 127.0f;
}

void
MusicDelay::settempo(int _Ptempo)
{
    float coef = 0.0;

    Ptempo = _Ptempo;
    float ntem = 60.0f / (float) _Ptempo;


    delay1 = lrintf((ntem / (float) Pdelay1) * fSAMPLE_RATE);
    
    if (Plrdelay != 0)
        coef = ntem / (float) Plrdelay;
    else
        coef = 0;
    
    delay2 = lrintf((coef + (ntem / (float) Pdelay2)) * fSAMPLE_RATE);

    initdelays();
}

void
MusicDelay::setpreset(int npreset)
{
    const int PRESET_SIZE = C_MUSIC_PARAMETERS;
    const int NUM_PRESETS = 2;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Echo 1
        {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75},
        //Echo 2
        {67, 0, 3, 7, 0, 59, 0, 127, 6, 69, 60, 127, 127}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_MUSICAL_DELAY, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = (int) npreset;
}

void
MusicDelay::changepar(int npar, int value)
{
    switch (npar)
    {
    case Music_DryWet:
        setvolume(value);
        break;
    case Music_Pan_1:
        setpanning(1, value);
        break;
    case Music_Delay_1:
        setdelay(1, value);
        break;
    case Music_Del_Offset:
        setdelay(3, value);
        break;
    case Music_LR_Cross:
        setlrcross(value);
        break;
    case Music_Feedback_1:
        setfb(1, value);
        break;
    case Music_Damp:
        sethidamp(value);
        break;
    case Music_Pan_2:
        setpanning(2, value);
        break;
    case Music_Delay_2:
        setdelay(2, value);
        break;
    case Music_Feedback_2:
        setfb(2, value);
        break;
    case Music_Tempo:
        settempo(value);
        break;
    case Music_Gain_1:
        setgain(1, value);
        break;
    case Music_Gain_2:
        setgain(2, value);
        break;
    default:
        return;
    }
}

int
MusicDelay::getpar(int npar)
{
    switch (npar)
    {
    case Music_DryWet:
        return (Pvolume);

    case Music_Pan_1:
        return (Ppanning1);

    case Music_Delay_1:
        return (Pdelay1);

    case Music_Del_Offset:
        return (Plrdelay);

    case Music_LR_Cross:
        return (Plrcross);

    case Music_Feedback_1:
        return (Pfb1);

    case Music_Damp:
        return (Phidamp);

    case Music_Pan_2:
        return (Ppanning2);

    case Music_Delay_2:
        return (Pdelay2);

    case Music_Feedback_2:
        return (Pfb2);

    case Music_Tempo:
        return (Ptempo);

    case Music_Gain_1:
        return (Pgain1);

    case Music_Gain_2:
        return (Pgain2);

    }
    return (0); //in case of bogus parameter number
}
