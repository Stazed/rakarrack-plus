/*
  ZynAddSubFX - a software synthesizer

  Chorus.C - Chorus and Flange effects
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
#include "Chorus.h"
#include <stdio.h>
#include "FPreset.h"

Chorus::Chorus(double sample_rate, uint32_t intermediate_bufsize) :
    Ppreset(0),
    outvolume(0.5f),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    lfo(NULL),
    Pvolume(),
    Ppanning(),
    Pdepth(),
    Pdelay(),
    Pfb(),
    Plrcross(),
    Pflangemode(0),
    Poutsub(),
    maxdelay(lrintf(MAX_CHORUS_DELAY / 1000.0 * (int) sample_rate)),
    dlk(),
    drk(),
    dlhi(),
    dlhi2(),
    awesome_mode(0),
    depth(),
    delay(),
    fb(),
    lrcross(),
    panning(),
    oldr(),
    oldl(),
    dl1(),
    dl2(),
    dr1(),
    dr2(),
    lfol(),
    lfor(),
    delayl(NULL),
    delayr(NULL),
    dllo(),
    mdel(),
    Fpre(NULL)
{
    delayl = new float[maxdelay];
    delayr = new float[maxdelay];
    lfo = new EffectLFO(sample_rate);

    float tmp = 0.08f;
    ldelay = new delayline(tmp, 2, sample_rate);
    rdelay = new delayline(tmp, 2, sample_rate);
    ldelay -> set_averaging(0.005f);
    rdelay -> set_averaging(0.005f);
    ldelay->set_mix(0.5f);
    rdelay->set_mix(0.5f);

    lfo->effectlfoout(&lfol, &lfor);
    dl2 = getdelay(lfol);
    dr2 = getdelay(lfor);
    setpreset(Ppreset); // must be set after call to getdelay
    cleanup();
}

Chorus::~Chorus()
{
    delete[] delayl;
    delete[] delayr;
    delete ldelay;
    delete rdelay;
    delete lfo;
}

/*
 * get the delay value in samples; xlfo is the current lfo value
 */
float Chorus::getdelay(float xlfo)
{
    float result = 0;
    if (Pflangemode == 0)
    {
        result = (delay + xlfo * depth) * fSAMPLE_RATE;
    }
    else
    {
        result = 0;
    }

    //check if it is too big delay(caused by erroneous setdelay() and setdepth()
    if ((result + 0.5) >= maxdelay)
    {
        fprintf(stderr, "%s",
                "WARNING: Chorus.C::getdelay(..) too big delay (see setdelay and setdepth funcs.)\n");
        printf("%f %d\n", result, maxdelay);
        result = (float) maxdelay - 1.0f;
    }

    return (result);
}

/*
 * Apply the effect
 */
void
Chorus::out(float * efxoutl, float * efxoutr)
{

    dl1 = dl2;
    dr1 = dr2;
    lfo->effectlfoout(&lfol, &lfor);

    if (awesome_mode)
    { //use interpolated delay line for better sound
        float tmpsub;

        dl2 = delay + lfol * depth;
        dr2 = delay + lfor * depth;
        if (Poutsub != 0)
        {
            tmpsub = -1.0f;
        }
        else
        {
            tmpsub = 1.0f;
        }

        for (unsigned i = 0; i < PERIOD; i++)
        {
            //Left
            mdel = (dl1 * (float) (PERIOD - i) + dl2 * (float) i) / fPERIOD;
            float tmp = efxoutl[i] + oldl*fb;
            efxoutl[i] = tmpsub * ldelay->delay(tmp, mdel, 0, 1, 0);
            oldl = efxoutl[i];

            //Right
            mdel = (dr1 * (float) (PERIOD - i) + dr2 * (float) i) / fPERIOD;
            tmp = efxoutr[i] + oldr*fb;
            efxoutr[i] = tmpsub * rdelay->delay(tmp, mdel, 0, 1, 0);
            oldr = efxoutr[i];
        }
    }
    else
    {
        dl2 = getdelay(lfol);
        dr2 = getdelay(lfor);
        for (unsigned i = 0; i < PERIOD; i++)
        {
            float inl = efxoutl[i];
            float inr = efxoutr[i];
            //LRcross
            float l = inl;
            float r = inr;
            inl = l * (1.0f - lrcross) + r * lrcross;
            inr = r * (1.0f - lrcross) + l * lrcross;

            //Left channel

            //compute the delay in samples using linear interpolation between the lfo delays
            mdel = (dl1 * (float) (PERIOD - i) + dl2 * (float) i) / fPERIOD;
            
            if (++dlk >= maxdelay)
                dlk = 0;
            
            float tmp = (float) dlk - mdel + (float) maxdelay * 2.0f; //where should I get the sample from

            F2I(tmp, dlhi);
            dlhi %= maxdelay;

            dlhi2 = (dlhi - 1 + maxdelay) % maxdelay;
            dllo = 1.0f - fmodf(tmp, 1.0f);
            efxoutl[i] = delayl[dlhi2] * dllo + delayl[dlhi] * (1.0f - dllo);
            delayl[dlk] = inl + efxoutl[i] * fb;

            //Right channel

            //compute the delay in samples using linear interpolation between the lfo delays
            mdel = (dr1 * (float) (PERIOD - i) + dr2 * (float) i) / fPERIOD;
            
            if (++drk >= maxdelay)
                drk = 0;
            
            tmp = (float) drk - mdel + (float) maxdelay * 2.0f; //where should I get the sample from

            F2I(tmp, dlhi);
            dlhi %= maxdelay;

            dlhi2 = (dlhi - 1 + maxdelay) % maxdelay;
            dllo = 1.0f - fmodf(tmp, 1.0f);
            efxoutr[i] = delayr[dlhi2] * dllo + delayr[dlhi] * (1.0f - dllo);
            delayr[dlk] = inr + efxoutr[i] * fb;
        }

        if (Poutsub != 0)
        {
            for (unsigned i = 0; i < PERIOD; i++)
            {
                efxoutl[i] *= -1.0f;
                efxoutr[i] *= -1.0f;
            }
        }

        for (unsigned i = 0; i < PERIOD; i++)
        {
            efxoutl[i] *= (1.0f - panning);
            efxoutr[i] *= panning;
        }
    } //end awesome_mode test
}

/*
 * Cleanup the effect
 */
void
Chorus::cleanup()
{
    for (int i = 0; i < maxdelay; i++)
    {
        delayl[i] = 0.0;
        delayr[i] = 0.0;
    }
}

#ifdef LV2_SUPPORT
void
Chorus::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    fPERIOD = period;
    lfo->updateparams(period);
}
#endif // 0

/*
 * Parameter control
 */
void
Chorus::setdepth(int Pdepth)
{
    this->Pdepth = Pdepth;
    depth = (powf(8.0f, ((float) Pdepth / 127.0f) * 2.0f) - 1.0f) / 1000.0f; //seconds
}

void
Chorus::setdelay(int Pdelay)
{
    this->Pdelay = Pdelay;
    delay = (powf(10.0f, ((float) Pdelay / 127.0f) * 2.0f) - 1.0f) / 1000.0f; //seconds
}

void
Chorus::setfb(int Pfb)
{
    this->Pfb = Pfb;
    fb = ((float) Pfb - 64.0f) / 64.1f;
}

void
Chorus::setvolume(int Pvolume)
{
    this->Pvolume = Pvolume;
    
    if (awesome_mode)
    { //use interpolated delay line for better sound
        ldelay->set_mix(((float) Pvolume / 128.0f));
        rdelay->set_mix(((float) Pvolume / 128.0f));
    }
    
    outvolume = (float) Pvolume / 127.0f;
}

void
Chorus::setpanning(int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((float) Ppanning + .5f) / 127.0f;
}

void
Chorus::setlrcross(int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (float) Plrcross / 127.0f;
}

void
Chorus::setpreset(int npreset)
{
    const int PRESET_SIZE = C_CHORUS_PARAMETERS;
    const int NUM_PRESETS = 5;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Chorus1
        {64, 64, 33, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0},
        //Chorus2
        {64, 64, 19, 0, 0, 98, 56, 90, 64, 19, 0, 0, 1},
        //Chorus3
        {64, 64, 7, 0, 1, 42, 97, 95, 90, 127, 0, 0, 0},
        //Celeste1
        {64, 64, 1, 0, 0, 42, 115, 18, 90, 127, 0, 0, 0},
        //Celeste2
        {64, 64, 7, 117, 0, 50, 115, 9, 31, 127, 0, 1, 0}
    };

    // (npreset > 4) means user defined (Insert) presets for Chorus
    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_CHORUS, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else    // No user defined presets so just set it
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
    
    Ppreset = npreset;
}

void
Chorus::changepar(int npar, int value)
{
    switch (npar)
    {
    case Chorus_DryWet:
        setvolume(value);
        break;
    case Chorus_Pan:
        setpanning(value);
        break;
    case Chorus_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case Chorus_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case Chorus_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case Chorus_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case Chorus_Depth:
        setdepth(value);
        break;
    case Chorus_Delay:
        setdelay(value);
        break;
    case Chorus_Feedback:
        setfb(value);
        break;
    case Chorus_LR_Cross:
        setlrcross(value);
        break;
    case Chorus_SKIP_Flange_10:
        if (value > 1)
            value = 1;
        Pflangemode = value;
        break;
    case Chorus_Subtract:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case Chorus_Intense:
        awesome_mode = value;
        if (awesome_mode)
        {
            ldelay->set_mix(((float) Pvolume / 128.0f));
            rdelay->set_mix(((float) Pvolume / 128.0f));
        }
        outvolume = (float) Pvolume / 127.0f;
        break;
    }
}

int
Chorus::getpar(int npar)
{
    switch (npar)
    {
    case Chorus_DryWet:
        return (Pvolume);
        break;
    case Chorus_Pan:
        return (Ppanning);
        break;
    case Chorus_LFO_Tempo:
        return (lfo->Pfreq);
        break;
    case Chorus_LFO_Random:
        return (lfo->Prandomness);
        break;
    case Chorus_LFO_Type:
        return (lfo->PLFOtype);
        break;
    case Chorus_LFO_Stereo:
        return (lfo->Pstereo);
        break;
    case Chorus_Depth:
        return (Pdepth);
        break;
    case Chorus_Delay:
        return (Pdelay);
        break;
    case Chorus_Feedback:
        return (Pfb);
        break;
    case Chorus_LR_Cross:
        return (Plrcross);
        break;
    case Chorus_SKIP_Flange_10:
        return (Pflangemode);
        break;
    case Chorus_Subtract:
        return (Poutsub);
        break;
    case Chorus_Intense:
        return (awesome_mode);
        break;
    default:
        return (0);
    }
}
