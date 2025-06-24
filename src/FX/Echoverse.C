/*
  ZynAddSubFX - a software synthesizer

  Echoverse.C - Echo effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Echo effect by Transmogrifox

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

// Renamed from RBEcho.C on 4/19/2020 by stazed

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Echoverse.h"     // Echoverse

Echoverse::Echoverse(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    Pvolume(50),
    Ppanning(64),
    Pdelay(60),
    Plrdelay(100),
    Plrcross(100),
    Pfb(40),
    Phidamp(60),
    Preverse(),
    Psubdiv(1),
    Pes(),
    maxx_delay(1 + sample_rate * MAX_DELAY),
    delay(),
    lrdelay(),
    ltime(),
    rtime(),
    fdelay(),
    subdiv(1.0f),
    pes(),
    pingpong(),
    ipingpong(1.0f),
    rvl(),
    rvr(),
    rpanning(),
    lpanning(),
    lrcross(),
    fb(),
    hidamp(),
    reverse(),
    ireverse(),
    lfeedback(),
    rfeedback(),
    oldl(),
    oldr(),
    Srate_Attack_Coeff(1.0f / (sample_rate * ATTACK)),
    ldelay(NULL),
    rdelay(NULL)
{
    ldelay = new delayline(2.0f, 3, sample_rate);
    rdelay = new delayline(2.0f, 3, sample_rate);

    Echoverse::setpreset(Ppreset);
    Echoverse::cleanup();
}

Echoverse::~Echoverse()
{
    delete ldelay;
    delete rdelay;
}

/*
 * Cleanup the effect
 */
void
Echoverse::cleanup()
{
    ldelay->cleanup();
    rdelay->cleanup();
    ldelay->set_averaging(0.25f);
    rdelay->set_averaging(0.25f);
    oldl = 0.0;
    oldr = 0.0;
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Echoverse::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
}
#endif // LV2

void
Echoverse::set_random_parameters()
{
    for(int i = 0; i < C_ECHOVERSE_PARAMETERS; i++)
    {
        switch(i)
        {
            case Echoverse_DryWet:
            case Echoverse_Pan:
            case Echoverse_LR_Delay:
            case Echoverse_Feedback:
            case Echoverse_Damp:
            case Echoverse_Reverse:
            case Echoverse_Ext_Stereo:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Echoverse_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Echoverse_Angle:
            {
                int value = (int) (RND * 129);
                changepar (i, value);
            }
            break;

            case Echoverse_Subdivision:
            {
                int value = (int) (RND * 6);
                changepar (i, value);
            }
            break;

        }
    }
}

void
Echoverse::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_ECHOVERSE_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Echoverse_Tempo:
            case Echoverse_LR_Delay:
            case Echoverse_Feedback:
            case Echoverse_Damp:
            case Echoverse_Reverse:
            case Echoverse_Subdivision:
            case Echoverse_Ext_Stereo:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), echoverse_parameters[i * 3 + 1], echoverse_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Echoverse_Ext_Stereo )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Echoverse_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Echoverse_DryWet )), echoverse_parameters[i * 3 + 1], echoverse_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Echoverse_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Echoverse_Pan:
            case Echoverse_Angle:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, echoverse_parameters[i * 3 + 1], echoverse_parameters[i * 3 + 2]);
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
Echoverse::initdelays()
{
    oldl = 0.0;
    oldr = 0.0;

    if (Plrdelay > 0)
    {
        ltime = delay + lrdelay;
        rtime = delay - lrdelay;
    }
    else
    {
        ltime = delay - lrdelay;
        rtime = delay + lrdelay;
    }

    if (ltime > 2.0f) ltime = 2.0f;
    if (ltime < 0.01f) ltime = 0.01f;

    if (rtime > 2.0f) rtime = 2.0f;
    if (rtime < 0.01f) rtime = 0.01f;
}

/*
 * Effect output
 */
void
Echoverse::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        //LowPass Filter
        float ldl = lfeedback * hidamp + oldl * (1.0f - hidamp);
        float rdl = rfeedback * hidamp + oldr * (1.0f - hidamp);
        oldl = ldl + DENORMAL_GUARD;
        oldr = rdl + DENORMAL_GUARD;

        ldl = ldelay->delay_simple((ldl + efxoutl[i]), delay, 0, 1, 0);
        rdl = rdelay->delay_simple((rdl + efxoutr[i]), delay, 0, 1, 0);

        if (Preverse)
        {
            rvl = ldelay->delay_simple(oldl, delay, 1, 0, 1) * ldelay->envelope();
            rvr = rdelay->delay_simple(oldr, delay, 1, 0, 1) * rdelay->envelope();
            ldl = ireverse * ldl + reverse*rvl;
            rdl = ireverse * rdl + reverse*rvr;
        }

        //lfeedback = lpanning * fb * ldl;
        //rfeedback = rpanning * fb * rdl;
        lfeedback = rpanning * fb * ldl;
        rfeedback = lpanning * fb * rdl;

        if (Pes)
        {
            ldl *= cosf(lrcross);
            rdl *= sinf(lrcross);

            float avg = (ldl + rdl) * 0.5f;
            float ldiff = ldl - avg;
            float rdiff = rdl - avg;

            float tmp = avg + ldiff * pes;
            ldl = 0.5 * tmp;

            tmp = avg + rdiff * pes;
            rdl = 0.5f * tmp;
        }
        //efxoutl[i] = (ipingpong*ldl + pingpong *ldelay->delay_simple(0.0f, ltime, 2, 0, 0)) * lpanning;
        //efxoutr[i] = (ipingpong*rdl + pingpong *rdelay->delay_simple(0.0f, rtime, 2, 0, 0)) * rpanning;
        efxoutl[i] = (ipingpong * ldl + pingpong * ldelay->delay_simple(0.0f, ltime, 2, 0, 0)) * rpanning;
        efxoutr[i] = (ipingpong * rdl + pingpong * rdelay->delay_simple(0.0f, rtime, 2, 0, 0)) * lpanning;

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();
}

/*
 * Parameter control
 */
void
Echoverse::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Echoverse::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    lpanning = ((float) _Ppanning) / 64.0f;
    rpanning = 2.0f - lpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f / (lpanning + 1.0f);
    rpanning = 1.0f - 1.0f / (rpanning + 1.0f);
    lpanning *= 1.1f;
    rpanning *= 1.1f;
}

void
Echoverse::setreverse(int _Preverse)
{
    Preverse = _Preverse;
    reverse = (float) _Preverse / 127.0f;
    ireverse = 1.0f - reverse;
}

void
Echoverse::setdelay(int _Pdelay)
{
    Pdelay = _Pdelay;
    fdelay = 60.0f / ((float) _Pdelay);
    if (fdelay < 0.01f)
    {
        fdelay = 0.01f;
    }
    if (fdelay > (float) MAX_DELAY)
    {
        fdelay = (float) MAX_DELAY; //Constrains 10ms ... MAX_DELAY
    }
    delay = subdiv * fdelay;
    initdelays();
}

void
Echoverse::setlrdelay(int _Plrdelay)
{
    float tmp;
    Plrdelay = _Plrdelay;
    lrdelay = delay * fabs(((float) _Plrdelay - 64.0f) / 65.0f);

    tmp = fabs(((float) _Plrdelay - 64.0f) / 32.0f);
    pingpong = 1.0f - 1.0f / (5.0f * tmp * tmp + 1.0f);
    pingpong *= 1.05159f;
    ipingpong = 1.0f - pingpong;
    initdelays();
}

void
Echoverse::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = D_PI * (float) _Plrcross / 128.0f;
}

void
Echoverse::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = (float) _Pfb / 128.0f;
}

void
Echoverse::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = f_exp(-D_PI * 500.0f * ((float) _Phidamp) / fSAMPLE_RATE);
}

void
Echoverse::setpreset(int npreset)
{
    const int PRESET_SIZE = C_ECHOVERSE_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Echo 1
        {64, 64, 90, 64, 64, 64, 64, 0, 1, 96},
        //Echo 2
        {64, 64, 90, 64, 64, 64, 64, 0, 2, 96},
        //Echo 3
        {64, 64, 90, 64, 64, 111, 127, 77, 3, 96},
        //Echo 4
        {67, 64, 128, 64, 64, 59, 0, 0, 1, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_ECHOVERSE, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
    
    Ppreset = npreset;
}

void
Echoverse::changepar(int npar, int value)
{
    switch (npar)
    {
    case Echoverse_DryWet:
        setvolume(value);
        break;
    case Echoverse_Pan:
        setpanning(value);
        break;
    case Echoverse_Tempo:
        setdelay(value);
        break;
    case Echoverse_LR_Delay:
        setlrdelay(value);
        break;
    case Echoverse_Angle:
        setlrcross(value);
        break;
    case Echoverse_Feedback:
        setfb(value);
        break;
    case Echoverse_Damp:
        sethidamp(value);
        break;
    case Echoverse_Reverse:
        setreverse(value);
        break;
    case Echoverse_Subdivision:
        Psubdiv = value;
        subdiv = 1.0f / ((float) (value + 1));
        delay = subdiv * fdelay;
        initdelays();
        break;
    case Echoverse_Ext_Stereo:
        Pes = value;
        pes = 8.0f * (float) Pes / 127.0f;
        break;
    default:
        return;
    }
}

int
Echoverse::getpar(int npar)
{
    switch (npar)
    {
    case Echoverse_DryWet:
        return (Pvolume);

    case Echoverse_Pan:
        return (Ppanning);

    case Echoverse_Tempo:
        return (Pdelay);

    case Echoverse_LR_Delay:
        return (Plrdelay);

    case Echoverse_Angle:
        return (Plrcross);

    case Echoverse_Feedback:
        return (Pfb);

    case Echoverse_Damp:
        return (Phidamp);

    case Echoverse_Reverse:
        return (Preverse);

    case Echoverse_Subdivision:
        return (Psubdiv);

    case Echoverse_Ext_Stereo:
        return (Pes);

    }
    return (0); //in case of bogus parameter number
}
