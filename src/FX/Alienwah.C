/*
  ZynAddSubFX - a software synthesizer

  Alienwah.C - "AlienWah" effect
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
#include "Alienwah.h"
#include <stdio.h>

Alienwah::Alienwah(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    Pvolume(),
    Ppanning(),
    Pdepth(),
    Pfb(),
    Plrcross(),
    Pdelay(),
    Pphase(),
    oldk(0),
    oldpdelay(0),
    lfo(NULL),
    panning(0.0f),
    fb(0.0f),
    depth(0.0f),
    lrcross(0.0f),
    phase(0.0f),
    oldl(),
    oldr(),
    oldclfol(),
    oldclfor()
{
    lfo = new EffectLFO(sample_rate);

    setpreset(Ppreset);
    cleanup();
    
    oldclfol.a = fb;
    oldclfol.b = 0.0;
    oldclfor.a = fb;
    oldclfor.b = 0.0;
};

Alienwah::~Alienwah()
{
    delete lfo;
};

/*
 * Apply the effect
 */
void
Alienwah::out(float * efxoutl, float * efxoutr)
{
    float lfol, lfor;
    COMPLEXTYPE clfol, clfor, out, tmp;

    lfo->effectlfoout(&lfol, &lfor);
    lfol *= depth * D_PI;
    lfor *= depth * D_PI;
    clfol.a = cosf(lfol + phase) * fb;
    clfol.b = sinf(lfol + phase) * fb;
    clfor.a = cosf(lfor + phase) * fb;
    clfor.b = sinf(lfor + phase) * fb;

    bool have_nans = false;

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float x = (float) i / fPERIOD;
        float x1 = 1.0f - x;
        //left
        tmp.a = clfol.a * x + oldclfol.a * x1;
        tmp.b = clfol.b * x + oldclfol.b * x1;

        out.a = tmp.a * oldl[oldk].a - tmp.b * oldl[oldk].b
                + (1.0f - fabsf(fb)) * efxoutl[i] * (1.0f - panning);
        out.a += DENORMAL_GUARD;
        out.b = tmp.a * oldl[oldk].b + tmp.b * oldl[oldk].a;
        out.b += DENORMAL_GUARD;
        oldl[oldk].a = out.a;
        oldl[oldk].b = out.b;
        float l = out.a * 10.0f * (fb + 0.1f);

        //right
        tmp.a = clfor.a * x + oldclfor.a * x1;
        tmp.b = clfor.b * x + oldclfor.b * x1;

        out.a = tmp.a * oldr[oldk].a - tmp.b * oldr[oldk].b
                + (1.0f - fabsf(fb)) * efxoutr[i] * panning;
        out.a += DENORMAL_GUARD;
        out.b = tmp.a * oldr[oldk].b + tmp.b * oldr[oldk].a;
        out.b += DENORMAL_GUARD;
        oldr[oldk].a = out.a;
        oldr[oldk].b = out.b;
        float r = out.a * 10.0f * (fb + 0.1f);


        if (++oldk >= Pdelay)
            oldk = 0;
        //LRcross
        efxoutl[i] = l * (1.0f - lrcross) + r * lrcross;
        efxoutr[i] = r * (1.0f - lrcross) + l * lrcross;
        
        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    };

    oldclfol.a = clfol.a;
    oldclfol.b = clfol.b;
    oldclfor.a = clfor.a;
    oldclfor.b = clfor.b;
    
    if(have_nans)
        cleanup();

}

/*
 * Cleanup the effect
 */
void
Alienwah::cleanup()
{
    for (int i = oldpdelay; i < MAX_ALIENWAH_DELAY; i++)
    {
        oldl[i].a = 0.0f;
        oldl[i].b = 0.0f;
        oldr[i].a = 0.0f;
        oldr[i].b = 0.0f;

    };
    oldk = 0;
}

#ifdef LV2_SUPPORT
void
Alienwah::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    fPERIOD = period;
    lfo->updateparams(period);
}
#endif // LV2

void
Alienwah::set_random_parameters()
{
    for(int i = 0; i < C_ALIENWAH_PARAMETERS; i++)
    {
        switch(i)
        {
            case Alien_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Alien_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case Alien_DryWet:
            case Alien_Pan:
            case Alien_LFO_Random:
            case Alien_LFO_Stereo:
            case Alien_Depth:
            case Alien_Feedback:
            case Alien_LR_Cross:
            case Alien_Phase:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Alien_Delay:
            {
                int value = (int) (RND * 101);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Alienwah::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_ALIENWAH_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Alien_LFO_Tempo:
            case Alien_LFO_Random:
            case Alien_LFO_Type:
            case Alien_LFO_Stereo:
            case Alien_Depth:
            case Alien_Feedback:
            case Alien_Delay:
            case Alien_LR_Cross:
            case Alien_Phase:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), alien_parameters[i * 3 + 1], alien_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Alien_Phase )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Alien_DryWet: 
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( i )), alien_parameters[i * 3 + 1], alien_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Alien_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
            
            // Offset
            case Alien_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, alien_parameters[i * 3 + 1], alien_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Alien_Pan ) - 64);
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
Alienwah::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Alienwah::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + .5f) / 127.0f;
}

void
Alienwah::setdepth(int _Pdepth)
{
    Pdepth = _Pdepth;
    depth = ((float) _Pdepth / 127.0f);
}

void
Alienwah::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = fabsf(((float) _Pfb - 64.0f) / 64.1f);
    fb = sqrtf(fb);

    if (fb < 0.4f)
        fb = 0.4f;

    if (_Pfb < 64)
        fb = -fb;
}

void
Alienwah::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f;
}

void
Alienwah::setdelay(int _Pdelay)
{
    if (_Pdelay > MAX_ALIENWAH_DELAY)
        _Pdelay = MAX_ALIENWAH_DELAY;

    Pdelay = _Pdelay;

    if (_Pdelay > oldpdelay)
        cleanup();

    oldpdelay = _Pdelay;
}

void
Alienwah::setphase(int _Pphase)
{
    Pphase = _Pphase;
    phase = ((float) _Pphase - 64.0f) / 64.0f * PI;
}

void
Alienwah::setpreset(int npreset)
{
    const int PRESET_SIZE = C_ALIENWAH_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //AlienWah1
        {64, 64, 80, 0, 0, 62, 60, 105, 25, 0, 64},
        //AlienWah2
        {64, 64, 95, 106, 0, 101, 60, 105, 17, 0, 64},
        //AlienWah3
        {64, 64, 55, 0, 1, 100, 112, 105, 31, 0, 42},
        //AlienWah4
        {64, 64, 1, 0, 1, 66, 101, 11, 47, 0, 86}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_ALIENWAH, npreset - NUM_PRESETS + 1, pdata);
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
Alienwah::changepar(int npar, int value)
{
    switch (npar)
    {
    case Alien_DryWet:
        setvolume(value);
        break;
    case Alien_Pan:
        setpanning(value);
        break;
    case Alien_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case Alien_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case Alien_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case Alien_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case Alien_Depth:
        setdepth(value);
        break;
    case Alien_Feedback:
        setfb(value);
        break;
    case Alien_Delay:
        setdelay(value);
        break;
    case Alien_LR_Cross:
        setlrcross(value);
        break;
    case Alien_Phase:
        setphase(value);
        break;
    default:
        return;
    }
}

int
Alienwah::getpar(int npar)
{
    switch (npar)
    {
    case Alien_DryWet:
        return (Pvolume);

    case Alien_Pan:
        return (Ppanning);

    case Alien_LFO_Tempo:
        return (lfo->Pfreq);

    case Alien_LFO_Random:
        return (lfo->Prandomness);

    case Alien_LFO_Type:
        return (lfo->PLFOtype);

    case Alien_LFO_Stereo:
        return (lfo->Pstereo);

    case Alien_Depth:
        return (Pdepth);

    case Alien_Feedback:
        return (Pfb);

    case Alien_Delay:
        return (Pdelay);

    case Alien_LR_Cross:
        return (Plrcross);

    case Alien_Phase:
        return (Pphase);

    }

    return (0);
}
