/*
  ZynAddSubFX - a software synthesizer

  Echo.C - Echo effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Echo effect by Transmogrifox
  Echo Direct patch From: Arnout Engelen <arnouten@bzzt.net>
  delayline utility by Transmogrifox

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
#include "Echo.h"

Echo::Echo(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    Pvolume(50),
    Ppanning(64),
    Pdelay(60),
    Plrdelay(100),
    Plrcross(100),
    Pfb(40),
    Phidamp(60),
    Preverse(),
    Pdirect(),
    dl(),
    dr(),
    delay(),
    lrdelay(),
    kl(),
    kr(),
    rvkl(),
    rvkr(),
    rvfl(),
    rvfr(),
    maxx_delay(sample_rate * MAX_DELAY),
    fade(sample_rate / 5),               //  1/5 SR fade time available
    ldelay(NULL),
    rdelay(NULL),
    ltime(),
    rtime(),
    oldl(),
    oldr(),
    panning(),
    lrcross(),
    fb(),
    hidamp(),
    reverse(),
    ireverse(),
    Srate_Attack_Coeff(1.0f / (sample_rate * ATTACK))
{
    ldelay = new delayline(2.0f, 1, sample_rate);
    rdelay = new delayline(2.0f, 1, sample_rate);

    Echo::setpreset(Ppreset);
    Echo::cleanup();
}

Echo::~Echo()
{
    delete ldelay;
    delete rdelay;
}

/*
 * Cleanup the effect
 */
void
Echo::cleanup()
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
Echo::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
}
#endif // LV2

void
Echo::set_random_parameters()
{
    for(int i = 0; i < C_ECHO_PARAMETERS; i++)
    {
        switch(i)
        {
            case Echo_DryWet:
            case Echo_Pan:
            case Echo_LR_Delay:
            case Echo_LR_Cross:
            case Echo_Feedback:
            case Echo_Damp:
            case Echo_Reverse:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Echo_Delay:
            {
                int value = (int) (RND * 1980);
                changepar (i, value + 20);
            }
            break;

            case Echo_Direct:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Echo::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_ECHO_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Echo_Delay:
            case Echo_LR_Delay:
            case Echo_LR_Cross:
            case Echo_Feedback:
            case Echo_Damp:
            case Echo_Reverse:
            case Echo_Direct:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), echo_parameters[i * 3 + 1], echo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Echo_Direct )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Echo_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Echo_DryWet )), echo_parameters[i * 3 + 1], echo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Echo_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Echo_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Echo_Pan ) - 64, echo_parameters[i * 3 + 1], echo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Echo_Pan ) - 64);
                    s_buf += ":";
                }
            }
        }
    }
}

/*
 * Initialize the delays
 */
void
Echo::initdelays()
{
    oldl = 0.0;
    oldr = 0.0;
    ltime = delay + lrdelay;
    rtime = delay - lrdelay;

    if (ltime > 2.0f) ltime = 2.0f;
    if (ltime < 0.01f) ltime = 0.01f;

    if (rtime > 2.0f) rtime = 2.0f;
    if (rtime < 0.01f) rtime = 0.01f;
}

/*
 * Effect output
 */
void
Echo::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float ldl = ldelay->delay_simple(oldl, ltime, 0, 1, 0);
        float rdl = rdelay->delay_simple(oldr, rtime, 0, 1, 0);

        if (Preverse)
        {
            float rvl = ldelay->delay_simple(oldl, ltime, 1, 0, 1) * ldelay->envelope();
            float rvr = rdelay->delay_simple(oldr, rtime, 1, 0, 1) * rdelay->envelope();
            ldl = ireverse * ldl + reverse*rvl;
            rdl = ireverse * rdl + reverse*rvr;
        }

        float l = ldl * (1.0f - lrcross) + rdl * lrcross;
        float r = rdl * (1.0f - lrcross) + ldl * lrcross;
        ldl = l;
        rdl = r;

        float ldlout = -ldl*fb;
        float rdlout = -rdl*fb;
        
        if (!Pdirect)
        {
            l = ldl = efxoutl[i] * (1.0f - panning) + ldlout;
            r = rdl = efxoutr[i] * panning + rdlout;
        }
        else
        {
            ldl = efxoutl[i] * (1.0f - panning) + ldlout;
            rdl = efxoutr[i] * panning + rdlout;
        }

        efxoutl[i] = l;
        efxoutr[i] = r;

        //LowPass Filter
        oldl = ldl * hidamp + oldl * (1.0f - hidamp);
        oldr = rdl * hidamp + oldr * (1.0f - hidamp);
        oldl += DENORMAL_GUARD;
        oldr += DENORMAL_GUARD;

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
Echo::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Echo::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + 0.5f) / 127.0f;
}

void
Echo::setreverse(int _Preverse)
{
    Preverse = _Preverse;
    reverse = (float) _Preverse / 127.0f;
    ireverse = 1.0f - reverse;
}

void
Echo::Tempo2Delay(int value)
{
    Pdelay = 60.0f / (float) value * 1000.0f;
    delay = (float) Pdelay / 1000.0f;

    if ((unsigned int) delay > (MAX_DELAY))
        delay = MAX_DELAY;

    ldelay->set_averaging(10.0f);
    rdelay->set_averaging(10.0f);
    initdelays();
}

void
Echo::setdelay(int _Pdelay)
{
    Pdelay = _Pdelay;
    delay = ((float) _Pdelay) / 1000.0f;
    ldelay->set_averaging(0.05f);
    rdelay->set_averaging(0.05f);
    initdelays();
}

void
Echo::setlrdelay(int _Plrdelay)
{
    Plrdelay = _Plrdelay;
    float tmp =  (powf(2.0, fabsf((float) _Plrdelay - 64.0f) / 64.0f * 9.0f) -
            1.0f) / 1000.0f;
    
    if (_Plrdelay < 64.0)
        tmp = -tmp;
    
    lrdelay = tmp;
    initdelays();
}

void
Echo::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f * 1.0f;
}

void
Echo::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = (float) _Pfb / 128.0f;
}

void
Echo::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = 1.0f - (float) _Phidamp / 127.0f;
}

void
Echo::setdirect(int _Pdirect)
{
    if (_Pdirect > 0)
        _Pdirect = 1;
    
    Pdirect = _Pdirect;
}

void
Echo::setpreset(int npreset)
{
    const int PRESET_SIZE = C_ECHO_PARAMETERS;
    const int NUM_PRESETS = 9;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Echo 1
        {67, 64, 565, 64, 30, 59, 0, 127, 0},
        //Echo 2
        {67, 64, 357, 64, 30, 59, 0, 64, 0},
        //Echo 3
        {67, 75, 955, 64, 30, 59, 10, 0, 0},
        //Simple Echo
        {67, 60, 705, 64, 30, 0, 0, 0, 0},
        //Canyon
        {67, 60, 1610, 50, 30, 82, 48, 0, 0},
        //Panning Echo 1
        {67, 64, 705, 17, 0, 82, 24, 0, 0},
        //Panning Echo 2
        {81, 60, 737, 118, 100, 68, 18, 0, 0},
        //Panning Echo 3
        {81, 60, 472, 100, 127, 67, 36, 0, 0},
        //Feedback Echo
        {62, 64, 456, 64, 100, 90, 55, 0, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_ECHO, npreset - NUM_PRESETS + 1, pdata);
        
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
Echo::changepar(int npar, int value)
{
    switch (npar)
    {
    case Echo_DryWet:
        setvolume(value);
        break;
    case Echo_Pan:
        setpanning(value);
        break;
    case Echo_Delay:
        setdelay(value);
        break;
    case Echo_LR_Delay:
        setlrdelay(value);
        break;
    case Echo_LR_Cross:
        setlrcross(value);
        break;
    case Echo_Feedback:
        setfb(value);
        break;
    case Echo_Damp:
        sethidamp(value);
        break;
    case Echo_Reverse:
        setreverse(value);
        break;
    case Echo_Direct:
        setdirect(value);
        break;
    default:
        return;
    }
}

int
Echo::getpar(int npar)
{
    switch (npar)
    {
    case Echo_DryWet:
        return (Pvolume);

    case Echo_Pan:
        return (Ppanning);

    case Echo_Delay:
        return (Pdelay);

    case Echo_LR_Delay:
        return (Plrdelay);

    case Echo_LR_Cross:
        return (Plrcross);

    case Echo_Feedback:
        return (Pfb);

    case Echo_Damp:
        return (Phidamp);

    case Echo_Reverse:
        return (Preverse);

    case Echo_Direct:
        return (Pdirect);

    };
    
    return (0); //in case of bogus parameter number
}
