/*
  ZynAddSubFX - a software synthesizer

  Phaser.C - Phaser effect
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

#include <math.h>
#include "Phaser.h"
#include <stdio.h>
#include "../EFX_common/FPreset.h"
#define PHASER_LFO_SHAPE 2

Phaser::Phaser(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    Pvolume(),
    Ppanning(),
    Pdepth(),
    Pfb(),
    Plrcross(),
    Pstages(),
    Poutsub(),
    Pphase(),
    panning(),
    fb(),
    depth(),
    lrcross(),
    fbl(),
    fbr(),
    phase(),
    oldl(NULL),
    oldr(NULL),
    oldlgain(),
    oldrgain(),
    lfo(NULL)
{
    oldl = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES * 2);
    oldr = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES * 2);

    lfo = new EffectLFO(sample_rate);

    setpreset(Ppreset);
    cleanup();
}

Phaser::~Phaser()
{
    free(oldl);
    free(oldr);
    delete lfo;
}

/*
 * Effect output
 */
void
Phaser::out(float * efxoutl, float * efxoutr)
{
    float lfol, lfor; // initialized o.k.
    lfo->effectlfoout(&lfol, &lfor);

    float lgain = lfol;
    float rgain = lfor;
    lgain = (expf(lgain * PHASER_LFO_SHAPE) - 1.0f) / (expf(PHASER_LFO_SHAPE) - 1.0f);
    rgain = (expf(rgain * PHASER_LFO_SHAPE) - 1.0f) / (expf(PHASER_LFO_SHAPE) - 1.0f);

    lgain = 1.0f - phase * (1.0f - depth) - (1.0f - phase) * lgain * depth;
    rgain = 1.0f - phase * (1.0f - depth) - (1.0f - phase) * rgain * depth;

    if (lgain > 1.0)
    {
        lgain = 1.0f;
    }
    else if (lgain < 0.0)
    {
        lgain = 0.0f;
    }

    if (rgain > 1.0)
    {
        rgain = 1.0f;
    }
    else if (rgain < 0.0)
    {
        rgain = 0.0f;
    }

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        float x = (float) i / fPERIOD;
        float x1 = 1.0f - x;
        float gl = lgain * x + oldlgain * x1;
        float gr = rgain * x + oldrgain * x1;
        float inl = efxoutl[i] * panning + fbl;
        float inr = efxoutr[i] * (1.0f - panning) + fbr;

        //Left channel
        for (int j = 0; j < Pstages * 2; j++)
        {
            //Phasing routine
            float tmp = oldl[j] + DENORMAL_GUARD;
            oldl[j] = gl * tmp + inl;
            inl = tmp - gl * oldl[j];
        }
        //Right channel
        for (int j = 0; j < Pstages * 2; j++)
        {
            //Phasing routine
            float tmp = oldr[j] + DENORMAL_GUARD;
            oldr[j] = (gr * tmp) + inr;
            inr = tmp - (gr * oldr[j]);
        }
        //Left/Right crossing
        float l = inl;
        float r = inr;
        inl = l * (1.0f - lrcross) + r * lrcross;
        inr = r * (1.0f - lrcross) + l * lrcross;

        fbl = inl * fb;
        fbr = inr * fb;
        efxoutl[i] = inl;
        efxoutr[i] = inr;
    }

    oldlgain = lgain;
    oldrgain = rgain;

    if (Poutsub != 0)
    {
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            efxoutl[i] *= -1.0f;
            efxoutr[i] *= -1.0f;
        }
    }
}

/*
 * Cleanup the effect
 */
void
Phaser::cleanup()
{
    fbl = 0.0;
    fbr = 0.0;
    oldlgain = 0.0;
    oldrgain = 0.0;
    
    for (int i = 0; i < Pstages * 2; i++)
    {
        oldl[i] = 0.0;
        oldr[i] = 0.0;
    }
}

#ifdef LV2_SUPPORT
void
Phaser::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    fPERIOD = period;
    lfo->updateparams(period);
}
#endif // LV2

void
Phaser::set_random_parameters()
{
    for(int i = 0; i < C_PHASER_PARAMETERS; i++)
    {
        switch(i)
        {
            case Phaser_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Phaser_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case Phaser_DryWet:
            case Phaser_LFO_Stereo:
            case Phaser_Depth:
            case Phaser_Feedback:
            case Phaser_Phase:
            case Phaser_Pan:
            case Phaser_LFO_Random:
            case Phaser_LR_Cross:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Phaser_Stages:
            {
                int value = (int) (RND * 12);
                changepar (i, value + 1);
            }
            break;

            case Phaser_Subtract:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Phaser::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_PHASER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Phaser_LFO_Tempo:
            case Phaser_LFO_Random:
            case Phaser_LFO_Type:
            case Phaser_LFO_Stereo:
            case Phaser_Depth:
            case Phaser_Feedback:
            case Phaser_Stages:
            case Phaser_Subtract:
            case Phaser_Phase:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), phase_parameters[i * 3 + 1], phase_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Phaser_Phase )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Phaser_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Phaser_DryWet )), phase_parameters[i * 3 + 1], phase_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Phaser_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Phaser_Pan:
            case Phaser_LR_Cross:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, phase_parameters[i * 3 + 1], phase_parameters[i * 3 + 2]);
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
 * Parameter control
 */
void
Phaser::setdepth(int _Pdepth)
{
    Pdepth = _Pdepth;
    depth = ((float) _Pdepth / 127.0f);
}

void
Phaser::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = ((float) _Pfb - 64.0f) / 64.1f;
}

void
Phaser::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Phaser::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + .5f) / 127.0f;
}

void
Phaser::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f;
}

void
Phaser::setstages(int _Pstages)
{
    if (_Pstages > MAX_PHASER_STAGES)
        _Pstages = MAX_PHASER_STAGES;
    
    Pstages = _Pstages;
    cleanup();
}

void
Phaser::setphase(int _Pphase)
{
    Pphase = _Pphase;
    phase = ((float) _Pphase / 127.0f);
}

void
Phaser::setpreset(int npreset)
{
    const int PRESET_SIZE = C_PHASER_PARAMETERS;
    const int NUM_PRESETS = 6;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Phaser1
        {64, 64, 11, 0, 0, 64, 110, 64, 1, 0, 0, 20},
        //Phaser2
        {64, 64, 10, 0, 0, 88, 40, 64, 3, 0, 0, 20},
        //Phaser3
        {64, 64, 8, 0, 0, 66, 68, 107, 2, 0, 0, 20},
        //Phaser4
        {39, 64, 1, 0, 0, 66, 67, 10, 5, 0, 1, 20},
        //Phaser5
        {64, 64, 1, 0, 1, 110, 67, 78, 10, 0, 0, 20},
        //Phaser6
        {64, 64, 31, 100, 0, 58, 37, 78, 3, 0, 0, 20}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_PHASER, npreset - NUM_PRESETS + 1, pdata);
        
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
Phaser::changepar(int npar, int value)
{
    switch (npar)
    {
    case Phaser_DryWet:
        setvolume(value);
        break;
    case Phaser_Pan:
        setpanning(value);
        break;
    case Phaser_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case Phaser_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case Phaser_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case Phaser_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case Phaser_Depth:
        setdepth(value);
        break;
    case Phaser_Feedback:
        setfb(value);
        break;
    case Phaser_Stages:
        setstages(value);
        break;
    case Phaser_LR_Cross:
        setlrcross(value);
        break;
    case Phaser_Subtract:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case Phaser_Phase:
        setphase(value);
        break;
    default:
        return;
    }
}

int
Phaser::getpar(int npar)
{
    switch (npar)
    {
    case Phaser_DryWet:
        return (Pvolume);

    case Phaser_Pan:
        return (Ppanning);

    case Phaser_LFO_Tempo:
        return (lfo->Pfreq);

    case Phaser_LFO_Random:
        return (lfo->Prandomness);

    case Phaser_LFO_Type:
        return (lfo->PLFOtype);

    case Phaser_LFO_Stereo:
        return (lfo->Pstereo); // STDL

    case Phaser_Depth:
        return (Pdepth);

    case Phaser_Feedback:
        return (Pfb);

    case Phaser_Stages:
        return (Pstages);

    case Phaser_LR_Cross:
        return (Plrcross);

    case Phaser_Subtract:
        return (Poutsub);

    case Phaser_Phase:
        return (Pphase);

    }

    return (0);
}
