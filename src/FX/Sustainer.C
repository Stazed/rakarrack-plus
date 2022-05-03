/*
  Rakarrack Guitar FX

  Sustainer.C - Simple compressor/sustainer effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License
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
#include "Sustainer.h"

Sustainer::Sustainer(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    Pvolume(64),
    Psustain(64),
    timer(),
    hold((int) (sample_rate * 0.0125)),     //  12.5ms
    level(0.5f),
    fsustain(0.5f),
    input(),
    tmpgain(),
    prls(),
    compeak(),
    compg(),
    compenv(),
    oldcompenv(),
    calpha(),
    cbeta(),
    cthresh(0.25f),
    cratio(0.25f),
    cpthresh()
{
    float cSAMPLE_RATE = 1 / sample_rate;

    float tmp = 0.01f;      //  10 ms decay time on peak detectorS
    prls = 1.0f - (cSAMPLE_RATE / (cSAMPLE_RATE + tmp));

    tmp = 0.05f;            //  50 ms att/rel on compressor
    calpha = cSAMPLE_RATE / (cSAMPLE_RATE + tmp);
    cbeta = 1.0f - calpha;

    setpreset(Ppreset);

    cleanup();
}

Sustainer::~Sustainer()
{

}

/*
 * Cleanup the effect
 */
void
Sustainer::cleanup()
{
    compeak = 0.0f;
    compenv = 0.0f;
    oldcompenv = 0.0f;
    cpthresh = cthresh; //dynamic threshold
}

#ifdef LV2_SUPPORT
void
Sustainer::lv2_update_params(uint32_t period)
{
    PERIOD = period;
}
#endif // LV2

void
Sustainer::set_random_parameters()
{
    for(int i = 0; i < C_SUSTAIN_PARAMETERS; i++)
    {
        switch(i)
        {
            case Sustain_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Sustain_Sustain:
            {
                int value = (int) (RND * 127);
                changepar (i, value + 1);
            }
            break;
        }
    }
}

void
Sustainer::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SUSTAIN_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Sustain_Gain:
            case Sustain_Sustain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), sus_parameters[i * 3 + 1], sus_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Sustain_Sustain )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
        }
    }
}

/*
 * Effect output
 */
void
Sustainer::out(float * efxoutl, float * efxoutr)
{
    for (unsigned int i = 0; i < PERIOD; i++)
    { //apply compression to auxresampled
        float auxtempl = input * efxoutl[i];
        float auxtempr = input * efxoutr[i];
        float auxcombi = 0.5f * (auxtempl + auxtempr);
        
        if (fabs(auxcombi) > compeak)
        {
            compeak = fabs(auxcombi); //First do peak detection on the signal
            timer = 0;
        }
        
        if (timer > hold)
        {
            compeak *= prls;
            timer--;
        }
        
        timer++;
        compenv = cbeta * oldcompenv + calpha * compeak; //Next average into envelope follower
        oldcompenv = compenv;

        if (compenv > cpthresh)
        { //if envelope of signal exceeds thresh, then compress
            compg = cpthresh + cpthresh * (compenv - cpthresh) / compenv;
            cpthresh = cthresh + cratio * (compg - cpthresh); //cpthresh changes dynamically
            tmpgain = compg / compenv;
        }
        else
        {
            tmpgain = 1.0f;
        }

        if (compenv < cpthresh) cpthresh = compenv;
        if (cpthresh < cthresh) cpthresh = cthresh;

        efxoutl[i] = auxtempl * tmpgain * level;
        efxoutr[i] = auxtempr * tmpgain * level;
    }
    //End compression
}

void
Sustainer::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */
void
Sustainer::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SUSTAIN_PARAMETERS;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Moderate
        {79, 54},
        //Extreme
        {16, 127},
        //Mild
        {120, 15}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_SUSTAINER, npreset - NUM_PRESETS + 1, pdata);
        
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
Sustainer::changepar(int npar, int value)
{
    switch (npar)
    {
    case Sustain_Gain:
        Pvolume = value;
        level = dB2rap(-30.0f * (1.0f - ((float) Pvolume / 127.0f)));
        break;
    case Sustain_Sustain:
        Psustain = value;
        fsustain = (float) Psustain / 127.0f;
        cratio = 1.25f - fsustain;
        input = dB2rap(42.0f * fsustain - 6.0f);
        cthresh = 0.25 + fsustain;
        break;
    default:
        return;
    }
}

int
Sustainer::getpar(int npar)
{
    switch (npar)
    {
    case Sustain_Gain:
        return (Pvolume);

    case Sustain_Sustain:
        return (Psustain);

    }
    return (0); //in case of bogus parameter number
}

