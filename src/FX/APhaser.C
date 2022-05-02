/*

  APhaser.C  - Approximate digital model of an analog JFET phaser.
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
#include "APhaser.h"
#include <stdio.h>
#define PHASER_LFO_SHAPE 2
#define ONE_  0.99999f        // To prevent LFO ever reaching 1.0 for filter stability purposes
#define ZERO_ 0.00001f        // Same idea as above.

Analog_Phaser::Analog_Phaser(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    Pvolume(),
    Pdistortion(),
    Pwidth(),
    Pfb(),
    Poffset(),
    Pstages(),
    Poutsub(),
    Phyper(),
    Pdepth(),
    Pbarber(),
    barber(0),          //Deactivate barber pole phasing by default
    distortion(),
    fb(),
    width(),
    offsetpct(),
    fbl(),
    fbr(),
    depth(),
    lxn1(NULL),
    lyn1(NULL),
    rxn1(NULL),
    ryn1(NULL),
    offset(NULL),
    oldlgain(),
    oldrgain(),
    rdiff(),
    ldiff(),
    invperiod(1.0f / fPERIOD),
    mis(1.0f),
    Rmin(625.0f),       // 2N5457 typical on resistance at Vgs = 0
    Rmax(22000.0f),     // Resistor parallel to FET
    Rmx(Rmin / Rmax),
    Rconst(1.0f + Rmx), // Handle parallel resistor relationship
    C(0.00000005f),     // 50 nF
    CFs(),
    lfo(NULL)
{
    float fSAMPLE_RATE = sample_rate;

    lxn1 = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES);

    lyn1 = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES);

    rxn1 = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES);

    ryn1 = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES);

    offset = (float *) malloc(sizeof (float)* MAX_PHASER_STAGES); //model mismatch between JFET devices
    offset[0] = -0.2509303f;
    offset[1] = 0.9408924f;
    offset[2] = 0.998f;
    offset[3] = -0.3486182f;
    offset[4] = -0.2762545f;
    offset[5] = -0.5215785f;
    offset[6] = 0.2509303f;
    offset[7] = -0.9408924f;
    offset[8] = -0.998f;
    offset[9] = 0.3486182f;
    offset[10] = 0.2762545f;
    offset[11] = 0.5215785f;

    CFs = 2.0f * fSAMPLE_RATE*C;

    lfo = new EffectLFO(sample_rate);

    setpreset(Ppreset); //this will get done before out is run
    cleanup();
};

Analog_Phaser::~Analog_Phaser()
{
    delete lfo;
    free(lxn1);
    free(lyn1);
    free(rxn1);
    free(ryn1);
    free(offset);
};

/*
 * Effect output
 */
void
Analog_Phaser::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    float lfol, lfor;
    lfo->effectlfoout(&lfol, &lfor);

    float lmod = lfol * width + depth;
    float rmod = lfor * width + depth;

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

    if (Phyper != 0)
    {
        lmod *= lmod; //Triangle wave squared is approximately sin on bottom, tri on top
        rmod *= rmod; //Result is exponential sweep more akin to filter in synth with exponential generator circuitry.
    };

    lmod = sqrtf(1.0f - lmod); //gl,gr is Vp - Vgs. Typical FET drain-source resistance follows constant/[1-sqrt(Vp - Vgs)]
    rmod = sqrtf(1.0f - rmod);

    rdiff = (rmod - oldrgain) * invperiod;
    ldiff = (lmod - oldlgain) * invperiod;

    float gl = oldlgain;
    float gr = oldrgain;

    oldlgain = lmod;
    oldrgain = rmod;

    for (unsigned int i = 0; i < PERIOD; i++)
    {

        gl += ldiff; // Linear interpolation between LFO samples
        gr += rdiff;

        float lxn = efxoutl[i];
        float rxn = efxoutr[i];


        if (barber)
        {
            gl = fmodf((gl + 0.25f), ONE_);
            gr = fmodf((gr + 0.25f), ONE_);
        };


        //Left channel
        
        float hpfl = 0.0;
        for (int j = 0; j < Pstages; j++)
        {
            //Phasing routine
            mis = 1.0f + offsetpct * offset[j];
            float d = (1.0f + 2.0f * (0.25f + gl) * hpfl * hpfl * distortion) * mis; //This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.
            Rconst = 1.0f + mis*Rmx;
            float bl = (Rconst - gl) / (d * Rmin); // This is 1/R. R is being modulated to control filter fc.
            float lgain = (CFs - bl) / (CFs + bl);

            lyn1[j] = lgain * (lxn + lyn1[j]) - lxn1[j];
            lyn1[j] += DENORMAL_GUARD;
            hpfl = lyn1[j] + (1.0f - lgain) * lxn1[j]; //high pass filter -- Distortion depends on the high-pass part of the AP stage.

            lxn1[j] = lxn;
            lxn = lyn1[j];
            if (j == 1) lxn += fbl; //Insert feedback after first phase stage
        };

        //Right channel
        float hpfr = 0.0;
        for (int j = 0; j < Pstages; j++)
        {
            //Phasing routine
            mis = 1.0f + offsetpct * offset[j];
            float d = (1.0f + 2.0f * (0.25f + gr) * hpfr * hpfr * distortion) * mis; // distortion
            Rconst = 1.0f + mis*Rmx;
            float br = (Rconst - gr) / (d * Rmin);
            float rgain = (CFs - br) / (CFs + br);

            ryn1[j] = rgain * (rxn + ryn1[j]) - rxn1[j];
            ryn1[j] += DENORMAL_GUARD;
            hpfr = ryn1[j] + (1.0f - rgain) * rxn1[j]; //high pass filter

            rxn1[j] = rxn;
            rxn = ryn1[j];
            if (j == 1) rxn += fbr; //Insert feedback after first phase stage
        };

        fbl = lxn * fb;
        fbr = rxn * fb;
        efxoutl[i] = lxn;
        efxoutr[i] = rxn;

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }

    };

    if(have_nans)
        cleanup();

    if (Poutsub != 0)
    {
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            efxoutl[i] *= -1.0f;
            efxoutr[i] *= -1.0f;
        };
    }

};

/*
 * Cleanup the effect
 */
void
Analog_Phaser::cleanup()
{
    fbl = 0.0;
    fbr = 0.0;
    oldlgain = 0.0;
    oldrgain = 0.0;
    for (int i = 0; i < Pstages; i++)
    {
        lxn1[i] = 0.0;

        lyn1[i] = 0.0;

        rxn1[i] = 0.0;

        ryn1[i] = 0.0;

    };
};

void
Analog_Phaser::set_random_parameters()
{
    for(int i = 0; i < C_APHASER_PARAMETERS; i++)
    {
        switch(i)
        {
            case APhase_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case APhase_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case APhase_DryWet:
            case APhase_Distortion:
            case APhase_LFO_Random:
            case APhase_LFO_Stereo:
            case APhase_Width:
            case APhase_Mismatch:
            case APhase_Depth:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case APhase_Feedback:
            {
                int value = (int) (RND * 129);
                changepar (i, value);
            }
            break;

            case APhase_Stages:
            {
                int value = (int) (RND * 12);
                changepar (i, value + 1);
            }
            break;

            case APhase_Subtract:
            case APhase_Hyper:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;
        }
    }
}

#ifdef LV2_SUPPORT
void
Analog_Phaser::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    fPERIOD = period;
    lfo->updateparams(period);
    invperiod = 1.0f / fPERIOD;
}
#endif // LV2

void
Analog_Phaser::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_APHASER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case APhase_Distortion:
            case APhase_LFO_Tempo:
            case APhase_LFO_Random:
            case APhase_LFO_Type:
            case APhase_LFO_Stereo:
            case APhase_Width:
            case APhase_Stages:
            case APhase_Mismatch:
            case APhase_Subtract:
            case APhase_Depth:
            case APhase_Hyper:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), aphase_parameters[i * 3 + 1], aphase_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  APhase_Hyper)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case APhase_DryWet: 
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( i )), aphase_parameters[i * 3 + 1], aphase_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( APhase_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case APhase_Feedback:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, aphase_parameters[i * 3 + 1], aphase_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( APhase_Feedback ) - 64);
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
Analog_Phaser::setwidth(int _Pwidth)
{
    Pwidth = _Pwidth;
    width = ((float) _Pwidth / 127.0f);
};

void
Analog_Phaser::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = (float) (_Pfb - 64) / 64.2f;
};

void
Analog_Phaser::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
};

void
Analog_Phaser::setdistortion(int _Pdistortion)
{
    Pdistortion = _Pdistortion;
    distortion = (float) _Pdistortion / 127.0f;
};

void
Analog_Phaser::setoffset(int _Poffset)
{
    Poffset = _Poffset;
    offsetpct = (float) _Poffset / 127.0f;
};

void
Analog_Phaser::setstages(int _Pstages)
{

    if (_Pstages >= MAX_PHASER_STAGES)
    {
        _Pstages = MAX_PHASER_STAGES;
    }

    Pstages = _Pstages;

    cleanup();
};

void
Analog_Phaser::setdepth(int _Pdepth)
{
    Pdepth = _Pdepth;
    depth = (float) (_Pdepth - 64) / 127.0f; //Pdepth input should be 0-127.  depth shall range 0-0.5 since we don't need to shift the full spectrum.
};

void
Analog_Phaser::setpreset(int npreset)
{
    const int PRESET_SIZE = C_APHASER_PARAMETERS;
    const int NUM_PRESETS = 6;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Phaser1
        {64, 20, 14, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1},
        //Phaser2
        {64, 20, 14, 5, 1, 64, 110, 40, 6, 10, 0, 70, 1},
        //Phaser3
        {64, 20, 9, 0, 0, 64, 40, 40, 8, 10, 0, 60, 0},
        //Phaser4
        {64, 20, 14, 10, 0, 64, 110, 80, 7, 10, 1, 45, 1},
        //Phaser5
        {25, 20, 240, 10, 0, 64, 25, 16, 8, 100, 0, 25, 0},
        //Phaser6
        {64, 20, 1, 10, 1, 64, 110, 40, 12, 10, 0, 70, 1}
    };

    if (npreset > NUM_PRESETS - 1)
    {

        Fpre->ReadPreset(EFX_ANALOG_PHASER, npreset - NUM_PRESETS + 1, pdata);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = npreset;
};

void
Analog_Phaser::changepar(int npar, int value)
{
    switch (npar)
    {
    case APhase_DryWet:
        setvolume(value);
        break;
    case APhase_Distortion:
        setdistortion(value);
        break;
    case APhase_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case APhase_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case APhase_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        barber = 0;
        if (value == 2) barber = 1;
        break;
    case APhase_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case APhase_Width:
        setwidth(value);
        break;
    case APhase_Feedback:
        setfb(value);
        break;
    case APhase_Stages:
        setstages(value);
        break;
    case APhase_Mismatch:
        setoffset(value);
        break;
    case APhase_Subtract:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case APhase_Depth:
        setdepth(value);
        break;
    case APhase_Hyper:
        if (value > 1)
            value = 1;
        Phyper = value;
        break;
    default:
        return;
    }
}

int
Analog_Phaser::getpar(int npar)
{
    switch (npar)
    {
    case APhase_DryWet:
        return (Pvolume);

    case APhase_Distortion:
        return (Pdistortion);

    case APhase_LFO_Tempo:
        return (lfo->Pfreq);

    case APhase_LFO_Random:
        return (lfo->Prandomness);

    case APhase_LFO_Type:
        return (lfo->PLFOtype);

    case APhase_LFO_Stereo:
        return (lfo->Pstereo);

    case APhase_Width:
        return (Pwidth);

    case APhase_Feedback:
        return (Pfb);

    case APhase_Stages:
        return (Pstages);

    case APhase_Mismatch:
        return (Poffset);

    case APhase_Subtract:
        return (Poutsub);

    case APhase_Depth:
        return (Pdepth);

    case APhase_Hyper:
        return (Phyper);

    }
    
    return (0);
}
