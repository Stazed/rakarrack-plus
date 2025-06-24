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

Chorus::Chorus(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
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
    mdel()
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
    Chorus::setpreset(Ppreset); // must be set after call to getdelay
    Chorus::cleanup();
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
    bool have_nans = false;

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

            if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
            {
                efxoutl[i] = efxoutr[i] = 0.0;
                have_nans = true;
            }
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

            if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
            {
                efxoutl[i] = efxoutr[i] = 0.0;
                have_nans = true;
            }
        }
    } //end awesome_mode test

    if(have_nans)
        cleanup();
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

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Chorus::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    fPERIOD = period;
    lfo->updateparams(period);
}
#endif // 0

void
Chorus::set_random_parameters()
{   
    for(int i = 0; i < C_CHORUS_PARAMETERS; i++)
    {
        switch(i)
        {
            case Chorus_LFO_Tempo:
            {
                int value = (int) (RND * LFO_FREQ_MAX);
                changepar (i, value + 1);
            }
            break;

            case Chorus_LFO_Type:
            {
                int value = (int) (RND * LFO_NUM_TYPES);
                changepar (i, value);
            }
            break;

            case Chorus_DryWet:
            case Chorus_Pan:
            case Chorus_LFO_Random:
            case Chorus_LFO_Stereo:
            case Chorus_Depth:
            case Chorus_Delay:
            case Chorus_Feedback:
            case Chorus_LR_Cross:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Chorus_Subtract:
            case Chorus_Intense:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Chorus_SKIP_Flange_10:
                break;
        }
    }
}

void
Chorus::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    
    for(int i = 0; i < (C_CHORUS_PARAMETERS - 1); i++)  // -1 skip Chorus_SKIP_Flange_10
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Chorus_LFO_Tempo:
            case Chorus_LFO_Random:
            case Chorus_LFO_Type:
            case Chorus_LFO_Stereo:
            case Chorus_Depth:
            case Chorus_Delay:
            case Chorus_Feedback:
            case Chorus_Subtract:
            case Chorus_Intense:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), chorus_parameters[i * 3 + 1], chorus_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Chorus_Intense )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Chorus_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Chorus_DryWet )), chorus_parameters[i * 3 + 1], chorus_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Chorus_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
            
            // Offset
            case Chorus_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Chorus_Pan ) - 64, chorus_parameters[i * 3 + 1], chorus_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Chorus_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;
            
            // Skip after this one
            case Chorus_LR_Cross:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Chorus_LR_Cross ), chorus_parameters[i * 3 + 1], chorus_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Chorus_LR_Cross ) );
                    s_buf += ":";
                }

                // increment for skipped parameter
                param_case_offset++;
            }
            break;
        }
        param_case_offset++;
    }
}

/*
 * Parameter control
 */
void
Chorus::setdepth(int _Pdepth)
{
    Pdepth = _Pdepth;
    depth = (powf(8.0f, ((float) _Pdepth / 127.0f) * 2.0f) - 1.0f) / 1000.0f; //seconds
}

void
Chorus::setdelay(int _Pdelay)
{
    Pdelay = _Pdelay;
    delay = (powf(10.0f, ((float) _Pdelay / 127.0f) * 2.0f) - 1.0f) / 1000.0f; //seconds
}

void
Chorus::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = ((float) _Pfb - 64.0f) / 64.1f;
}

void
Chorus::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    
    if (awesome_mode)
    { //use interpolated delay line for better sound
        ldelay->set_mix(((float) _Pvolume / 128.0f));
        rdelay->set_mix(((float) _Pvolume / 128.0f));
    }
    
    outvolume = (float) _Pvolume / 127.0f;
}

void
Chorus::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + .5f) / 127.0f;
}

void
Chorus::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f;
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

    // (npreset > NUM_PRESETS - 1) means user defined (Insert) presets for Chorus
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
    default:
        return;
    }
}

int
Chorus::getpar(int npar)
{
    switch (npar)
    {
    case Chorus_DryWet:
        return (Pvolume);

    case Chorus_Pan:
        return (Ppanning);

    case Chorus_LFO_Tempo:
        return (lfo->Pfreq);

    case Chorus_LFO_Random:
        return (lfo->Prandomness);

    case Chorus_LFO_Type:
        return (lfo->PLFOtype);

    case Chorus_LFO_Stereo:
        return (lfo->Pstereo);

    case Chorus_Depth:
        return (Pdepth);

    case Chorus_Delay:
        return (Pdelay);

    case Chorus_Feedback:
        return (Pfb);

    case Chorus_LR_Cross:
        return (Plrcross);

    case Chorus_SKIP_Flange_10:
        return (Pflangemode);

    case Chorus_Subtract:
        return (Poutsub);

    case Chorus_Intense:
        return (awesome_mode);

    }

    return (0);
}
