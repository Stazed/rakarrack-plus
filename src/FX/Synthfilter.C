/*

  Synthfilter.C  - Approximate digital model of an analog JFET phaser.
  Analog modeling implemented by Ryan Billing aka Transmogrifox.
  November, 2009

  Credit to:
  ///////////////////
  ZynAddSubFX - a software synthesizer

  Phaser.C - Phaser effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

  DSP analog modeling theory & practice largely influenced by various CCRMA publications, particularly works by Julius O. Smith.
  ////////////////////


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
#include "Synthfilter.h"
#include <stdio.h>

#define ONE_  0.99999f        // To prevent LFO ever reaching 1.0 for filter stability purposes
#define ZERO_ 0.00001f        // Same idea as above.

Synthfilter::Synthfilter(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    Pvolume(),
    Pdistortion(),
    Pwidth(),
    Pfb(),
    Plpstages(4),
    Phpstages(2),
    Poutsub(),
    Pdepth(),
    Penvelope(),
    Pattack(),
    Prelease(),
    Pbandwidth(),
    delta(1.0 / sample_rate),
    distortion(),
    fb(),
    width(),
    env(),
    envdelta(),
    sns(),
    att(delta * 5.0f),          // 200ms
    rls(delta * 5.0f),          // 200ms
    fbl(),
    fbr(),
    depth(),
    bandgain(),
    lyn1(NULL),
    ryn1(NULL),
    lx1hp(NULL),
    ly1hp(NULL),
    rx1hp(NULL),
    ry1hp(NULL),
    oldlgain(),
    oldrgain(),
    inv_period(1.f / (float) PERIOD),
    Rmin(185.0f),               // 2N5457 typical on resistance at Vgs = 0
    Rmax(22000.0f),             // Resistor
    C(0.00000005f),             // 50 nF
    Clp(0.00000005f),
    Chp(0.00000005f),
    lfo(NULL)
{
    lyn1 = new float[MAX_SFILTER_STAGES];
    ryn1 = new float[MAX_SFILTER_STAGES];
    lx1hp = new float[MAX_SFILTER_STAGES];
    rx1hp = new float[MAX_SFILTER_STAGES];
    ly1hp = new float[MAX_SFILTER_STAGES];
    ry1hp = new float[MAX_SFILTER_STAGES];

    lfo = new EffectLFO(sample_rate);

    setpreset(Ppreset);
    cleanup();
}

Synthfilter::~Synthfilter()
{
    delete[] lyn1;
    delete[] ryn1;
    delete[] lx1hp;
    delete[] rx1hp;
    delete[] ly1hp;
    delete[] ry1hp;
    delete lfo;
}

/*
 * Effect output
 */
void
Synthfilter::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    float lfol, lfor;
    lfo->effectlfoout(&lfol, &lfor);
    
    float lmod = lfol * width + depth + env*sns;
    float rmod = lfor * width + depth + env*sns;

    if (lmod > ONE_)
    {
        lmod = ONE_;
    }
    else if (lmod < ZERO_)
    {
        lmod = ZERO_;
    }
    
    if (rmod > ONE_)
    {
        rmod = ONE_;
    }
    else if (rmod < ZERO_)
    {
        rmod = ZERO_;
    }

    lmod = 1.0f - lmod;
    rmod = 1.0f - rmod;
    lmod *= lmod;
    rmod *= rmod;

    float xl = (lmod - oldlgain) * inv_period;
    float xr = (rmod - oldrgain) * inv_period;
    float gl = oldlgain; // Linear interpolation between LFO samples
    float gr = oldrgain;

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float lxn = bandgain * efxoutl[i];
        float rxn = bandgain * efxoutr[i]; //extra gain

        gl += xl;
        gr += xr;   //  linear interpolation of LFO

        //  Envelope detection
        envdelta = (fabsf(efxoutl[i]) + fabsf(efxoutr[i])) - env;   //   envelope follower from Compressor.C
        
        if (delta > 0.0)
        {
            env += att * envdelta;
        }
        else
        {
            env += rls * envdelta;
        }

        //  End envelope power detection
        
        if (Plpstages < 1)
        {
            lxn += fbl;
            rxn += fbr;
        }

        //  Left channel Low Pass Filter
        for (int j = 0; j < Plpstages; j++)
        {
            float d = 1.0f + fabs(lxn) * distortion; // gain decreases as signal amplitude increases.

            //  low pass filter:  alpha*x[n] + (1-alpha)*y[n-1]
            //  alpha = lgain = dt/(RC + dt)
            float lgain = delta / ((Rmax * gl * d + Rmin) * Clp + delta);
            lyn1[j] = lgain * lxn + (1.0f - lgain) * lyn1[j];
            lyn1[j] += DENORMAL_GUARD;
            lxn = lyn1[j];
            
            if (j == 0) lxn += fbl; //  Insert feedback after first filter stage
        }

        //  Left channel High Pass Filter
        for (int j = 0; j < Phpstages; j++)
        {
            //  high pass filter:  alpha*(y[n-1] + x[n] - x[n-1]) // alpha = lgain = RC/(RC + dt)
            float lgain = (Rmax * gl + Rmin) * Chp / ((Rmax * gl + Rmin) * Chp + delta);
            ly1hp[j] = lgain * (lxn + ly1hp[j] - lx1hp[j]);

            ly1hp[j] += DENORMAL_GUARD;
            lx1hp[j] = lxn;
            lxn = ly1hp[j];
        }

        //  Right channel Low Pass Filter
        for (int j = 0; j < Plpstages; j++)
        {
            float d = 1.0f + fabs(rxn) * distortion; //   This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.

            float rgain = delta / ((Rmax * gr * d + Rmin) * Clp + delta);
            ryn1[j] = rgain * rxn + (1.0f - rgain) * ryn1[j];
            ryn1[j] += DENORMAL_GUARD;
            rxn = ryn1[j];
            
            if (j == 0) rxn += fbr; //  Insert feedback after first filter stage
        }

        //  Right channel High Pass Filter
        for (int j = 0; j < Phpstages; j++)
        {
            //  high pass filter:  alpha*(y[n-1] + x[n] - x[n-1]) // alpha = rgain = RC/(RC + dt)
            float rgain = (Rmax * gr + Rmin) * Chp / ((Rmax * gr + Rmin) * Chp + delta);
            ry1hp[j] = rgain * (rxn + ry1hp[j] - rx1hp[j]);

            ry1hp[j] += DENORMAL_GUARD;
            rx1hp[j] = rxn;
            rxn = ry1hp[j];
        }

        fbl = lxn * fb;
        fbr = rxn * fb;

        efxoutl[i] = lxn;
        efxoutr[i] = rxn;

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();

    oldlgain = lmod;
    oldrgain = rmod;

    if (Poutsub != 0)
    {
        for (unsigned i = 0; i < PERIOD; i++)
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
Synthfilter::cleanup()
{
    fbl = 0.0f;
    fbr = 0.0f;
    oldlgain = 0.0f;
    oldrgain = 0.0f;
    env = 0.0f;
    envdelta = 0.0f;
    
    for (int i = 0; i < MAX_SFILTER_STAGES; i++)
    {
        lyn1[i] = 0.0f;
        ryn1[i] = 0.0f;

        ly1hp[i] = 0.0f;
        lx1hp[i] = 0.0f;
        ry1hp[i] = 0.0f;
        rx1hp[i] = 0.0f;
    }
}

#ifdef LV2_SUPPORT
void
Synthfilter::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    lfo->updateparams(period);
    inv_period = 1.f / (float) period;
}
#endif // LV2

void
Synthfilter::set_random_parameters()
{
    for(int i = 0; i < C_SYNTHFILTER_PARAMETERS; i++)
    {
        switch(i)
        {
            case Synthfilter_DryWet:
            case Synthfilter_Distort:
            case Synthfilter_LFO_Random:
            case Synthfilter_LFO_Stereo:
            case Synthfilter_Width:
            case Synthfilter_Depth:
            case Synthfilter_Offset:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Synthfilter_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Synthfilter_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case Synthfilter_Feedback:
            case Synthfilter_Env_Sens:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Synthfilter_LPF_Stages:
            case Synthfilter_HPF_Stages:
            {
                int value = (int) (RND * 13);
                changepar (i, value);
            }
            break;

            case Synthfilter_Subtract:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Synthfilter_Attack:
            {
                int value = (int) (RND * 995);
                changepar (i, value + 5);
            }
            break;

            case Synthfilter_Release:
            {
                int value = (int) (RND * 495);
                changepar (i, value + 5);
            }
            break;
        }
    }
}

void
Synthfilter::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SYNTHFILTER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Synthfilter_Distort:
            case Synthfilter_LFO_Tempo:
            case Synthfilter_LFO_Random:
            case Synthfilter_LFO_Type:
            case Synthfilter_LFO_Stereo:
            case Synthfilter_Width:
            case Synthfilter_Feedback:
            case Synthfilter_LPF_Stages:
            case Synthfilter_HPF_Stages:
            case Synthfilter_Subtract:
            case Synthfilter_Depth:
            case Synthfilter_Env_Sens:
            case Synthfilter_Attack:
            case Synthfilter_Release:
            case Synthfilter_Offset:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), synth_parameters[i * 3 + 1], synth_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Synthfilter_Offset )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Synthfilter_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Synthfilter_DryWet )), synth_parameters[i * 3 + 1], synth_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Synthfilter_DryWet )) );
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
Synthfilter::setwidth(int _Pwidth)
{
    Pwidth = _Pwidth;
    width = ((float) _Pwidth / 127.0f);
}

void
Synthfilter::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = (float) _Pfb;
    
    if (fb < 0.0f)
    {
        fb /= 18.0f;
    }
    else if (fb > 0.0f)
    {
        fb /= 65.0f;
    }
    
    if (Plpstages <= 2)
    {
        fb *= 0.3; //keep filter stable when phase shift is small
    }
}

void
Synthfilter::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Synthfilter::setdistortion(int _Pdistortion)
{
    Pdistortion = _Pdistortion;
    distortion = (float) _Pdistortion / 127.0f;
}

void
Synthfilter::setdepth(int _Pdepth)
{
    Pdepth = _Pdepth;
    depth = (float) (_Pdepth - 32) / 95.0f; //   Pdepth input should be 0-127. .
}

void
Synthfilter::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SYNTHFILTER_PARAMETERS;
    const int NUM_PRESETS = 8;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Low Pass
        {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0},
        //High Pass
        {0, 20, 14, 0, 1, 64, 110, -40, 0, 6, 0, 32, -32, 500, 100, 0},
        //Band Pass
        {0, 20, 14, 0, 1, 64, 110, -40, 4, 4, 0, 32, -32, 500, 100, 0},
        //Lead Synth
        {0, 89, 31, 0, 1, 95, 38, -16, 1, 2, 1, 114, -32, 92, 215, 29},
        //Water
        {20, 69, 88, 0, 6, 0, 76, -50, 6, 2, 1, 0, 19, 114, 221, 127},
        //Pan Filter
        {0, 20, 100, 0, 5, 127, 127, -64, 2, 0, 0, 57, 0, 340, 288, 110},
        //Multi
        {64, 45, 88, 0, 1, 127, 81, 0, 4, 2, 0, 67, 0, 394, 252, 61},
        //Lazy Wahble
        {0, 0, 80, 0, 0, 62, 50, -30, 2, 1, 0, 100, -30, 80, 100, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_SYNTHFILTER, npreset - NUM_PRESETS + 1, pdata);
        
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
Synthfilter::changepar(int npar, int value)
{
    switch (npar)
    {
    case Synthfilter_DryWet:
        setvolume(value);
        break;
    case Synthfilter_Distort:
        setdistortion(value);
        break;
    case Synthfilter_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case Synthfilter_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case Synthfilter_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case Synthfilter_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case Synthfilter_Width:
        setwidth(value);
        break;
    case Synthfilter_Feedback:
        setfb(value);
        break;
    case Synthfilter_LPF_Stages:
        Plpstages = value;
        if (Plpstages >= MAX_SFILTER_STAGES)
            Plpstages = MAX_SFILTER_STAGES;
        if (Plpstages <= 2) fb = (float) Pfb * 0.25 / 65.0f; // keep filter stable when phase shift is small
        cleanup();
        break;
    case Synthfilter_HPF_Stages:
        Phpstages = value;
        if (Phpstages >= MAX_SFILTER_STAGES)
            Phpstages = MAX_SFILTER_STAGES;
        cleanup();
        break;
    case Synthfilter_Subtract:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case Synthfilter_Depth:
        setdepth(value);
        break;
    case Synthfilter_Env_Sens:
        Penvelope = value;
        sns = (float) Penvelope / 8.0f;
        break;
    case Synthfilter_Attack:
        Pattack = value;
        if (Pattack < 5) Pattack = 5;
        att = delta * 1000.0f / ((float) Pattack);
        break;
    case Synthfilter_Release:
        Prelease = value;
        if (Prelease < 5) Prelease = 5;
        rls = delta * 1000.0f / ((float) Prelease);
        break;
    case Synthfilter_Offset:
        Pbandwidth = value;
        Chp = C * (1.0f + ((float) value) / 64.0f); // C*3
        Clp = C * (1.0f - ((float) value) / 190.0f); // C/3
        break;
    default:
        return;
    }

    if (Phpstages && Plpstages)
    {
        bandgain = powf(((float) (Phpstages * Plpstages + 3)), (1.0f - (float) Pbandwidth / 127.0f));
    }
    else
    {
        bandgain = 1.0f;
    }
}

int
Synthfilter::getpar(int npar)
{
    switch (npar)
    {
    case Synthfilter_DryWet:
        return (Pvolume);

    case Synthfilter_Distort:
        return (Pdistortion);

    case Synthfilter_LFO_Tempo:
        return (lfo->Pfreq);

    case Synthfilter_LFO_Random:
        return (lfo->Prandomness);

    case Synthfilter_LFO_Type:
        return (lfo->PLFOtype);

    case Synthfilter_LFO_Stereo:
        return (lfo->Pstereo);

    case Synthfilter_Width:
        return (Pwidth);

    case Synthfilter_Feedback:
        return (Pfb);

    case Synthfilter_LPF_Stages:
        return (Plpstages);

    case Synthfilter_HPF_Stages:
        return (Phpstages);

    case Synthfilter_Subtract:
        return (Poutsub);

    case Synthfilter_Depth:
        return (Pdepth);

    case Synthfilter_Env_Sens:
        return (Penvelope);

    case Synthfilter_Attack:
        return (Pattack);

    case Synthfilter_Release:
        return (Prelease);

    case Synthfilter_Offset:
        return (Pbandwidth);

    }

    return (0);
}
