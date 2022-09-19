/*
  Arpie.C - Arpeggiated Echo effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Arpeggio Echo effect by Transmogrifox

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
#include "Arpie.h"

Arpie::Arpie(double sample_rate, uint32_t intermediate_bufsize) :
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
    Ppattern(),
    Pharms(3),
    Psubdiv(1),
    dl(),
    dr(),
    delay(),
    lrdelay(0),
    kl(),
    kr(),
    rvkl(),
    rvkr(),
    rvfl(),
    rvfr(),
    maxx_delay(),
    fade(),
    harmonic(1),
    envcnt(0),
    invattack(),
    subdiv(Psubdiv + 1),
    pattern(NULL),
    panning(),
    lrcross(),
    fb(),
    hidamp(),
    reverse(),
    ldelay(NULL),
    rdelay(NULL),
    oldl(),
    oldr(),
    Srate_Attack_Coeff(1.0f / (fSAMPLE_RATE * ATTACK)),
    envattack(),
    envswell()
{
    uint32_t SAMPLE_RATE = sample_rate;

    //default values
    invattack = SAMPLE_RATE / 15;
    envattack = 1.0f / (float) invattack;
    maxx_delay = SAMPLE_RATE * MAX_DELAY;
    fade = SAMPLE_RATE / 10; //200ms fade time available

    ldelay = new float[maxx_delay];
    rdelay = new float[maxx_delay];
    pattern = new int[MAXHARMS];

    setpreset(Ppreset);
    setpattern(0);
    cleanup();
}

Arpie::~Arpie()
{
    delete[] ldelay;
    delete[] rdelay;
    delete[] pattern;
}

/*
 * Cleanup the effect
 */
void
Arpie::cleanup()
{
    memset(ldelay, 0, sizeof (float)* maxx_delay);
    memset(rdelay, 0, sizeof (float)* maxx_delay);
    oldl = 0.0;
    oldr = 0.0;
    rvkl = 0;
    rvkr = 0;
    rvfl = 0;
    rvfr = 0;
    kl = 0;
    kr = 0;
    harmonic = 1;
}

void
Arpie::set_random_parameters()
{
    for(int i = 0; i < C_ARPIE_PARAMETERS; i++)
    {
        switch(i)
        {
            case Arpie_DryWet:
            case Arpie_Pan:
            case Arpie_LR_Delay:
            case Arpie_LR_Cross:
            case Arpie_Feedback:
            case Arpie_Damp:
            case Arpie_ArpeWD:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Arpie_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Arpie_Subdivision:
            case Arpie_Pattern:
            {
                int value = (int) (RND * 6);
                changepar (i, value);
            }
            break;

            case Arpie_Harm:
            {
                int value = (int) (RND * 8);
                changepar (i, value + 1);
            }
            break;
        }
    }
}

#ifdef LV2_SUPPORT
void
Arpie::lv2_update_params(uint32_t period)
{
    PERIOD = period;
}
#endif // LV2

void
Arpie::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_ARPIE_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Arpie_Tempo:
            case Arpie_LR_Delay:
            case Arpie_LR_Cross:
            case Arpie_Feedback:
            case Arpie_Damp:
            case Arpie_ArpeWD:
            case Arpie_Harm:
            case Arpie_Pattern:
            case Arpie_Subdivision:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), arpie_parameters[i * 3 + 1], arpie_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Arpie_Subdivision )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Arpie_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( i )), arpie_parameters[i * 3 + 1], arpie_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Arpie_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
            
            // Offset
            case Arpie_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, arpie_parameters[i * 3 + 1], arpie_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Arpie_Pan ) - 64);
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
Arpie::initdelays()
{
    kl = 0;
    kr = 0;

    dl = delay - lrdelay;

    if (dl < 1)
        dl = 1;
    
    dr = delay + lrdelay;
    
    if (dr < 1)
        dr = 1;
    
    if (dl > maxx_delay)
    {
        dl = maxx_delay;
        dr = maxx_delay - 2 * lrdelay;
    }
    
    if (dr > maxx_delay)
    {
        dr = maxx_delay;
        dl = maxx_delay - 2 * lrdelay;
    }

    rvkl = 0;
    rvkr = 0;
    Srate_Attack_Coeff = 15.0f / (dl + dr); // Set swell time to 1/10th of average delay time
    fade = (dl + dr) / 5;

    for (int i = dl; i < maxx_delay; i++)
    {
        ldelay[i] = 0.0;
    }
    
    for (int i = dr; i < maxx_delay; i++)
    {
        rdelay[i] = 0.0;
    }
    
    oldl = 0.0;
    oldr = 0.0;
}

/*
 * Effect output
 */
void
Arpie::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float ldl = ldelay[kl];
        float rdl = rdelay[kr];
        float l = ldl * (1.0f - lrcross) + rdl * lrcross;
        float r = rdl * (1.0f - lrcross) + ldl * lrcross;
        ldl = l;
        rdl = r;


        ldl = efxoutl[i] * (1.0f - panning) - ldl * fb;
        rdl = efxoutr[i] * panning - rdl * fb;

        if (reverse > 0.0)
        {
            float lswell = (float) (abs(kl - rvkl)) * Srate_Attack_Coeff;
            envswell = (1.0f - cosf(PI * envcnt * envattack));
            
            if (envswell > 1.0f)
                envswell = 1.0f;
            
            if (lswell <= PI)
            {
                lswell = 0.5f * (1.0f - cosf(lswell)); //Clickless transition
                efxoutl[i] = envswell * (reverse * (ldelay[rvkl] * lswell + ldelay[rvfl] * (1.0f - lswell)) + (ldl * (1 - reverse))); //Volume ducking near zero crossing.
            }
            else
            {
                efxoutl[i] = ((ldelay[rvkl] * reverse) + (ldl * (1 - reverse))) * envswell;
            }

            float rswell = (float) (abs(kr - rvkr)) * Srate_Attack_Coeff;
            
            if (rswell <= PI)
            {
                rswell = 0.5f * (1.0f - cosf(rswell)); //Clickless transition
                efxoutr[i] = envswell * (reverse * (rdelay[rvkr] * rswell + rdelay[rvfr] * (1.0f - rswell)) + (rdl * (1 - reverse))); //Volume ducking near zero crossing.
            }
            else
            {
                efxoutr[i] = envswell * ((rdelay[rvkr] * reverse) + (rdl * (1 - reverse)));
            }
        }
        else
        {
            efxoutl[i] = ldl;
            efxoutr[i] = rdl;
        }
        
        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }

        //LowPass Filter
        if(kl >= maxx_delay)
        {
            kl = 0;
        }
        
        if(kr >= maxx_delay)
        {
            kr = 0;
        }
        
        ldelay[kl] = ldl = ldl * hidamp + oldl * (1.0f - hidamp);
        rdelay[kr] = rdl = rdl * hidamp + oldr * (1.0f - hidamp);
        oldl = ldl + DENORMAL_GUARD;
        oldr = rdl + DENORMAL_GUARD;

        if (++envcnt >= invattack)
            envcnt = invattack;

        if (kl > (dl - invattack))
            envcnt -= 2;

        if (envcnt < 0)
            envcnt = 0;

        if (++kl >= dl)
        {
            kl = 0;
            envcnt = 0;

            if (++harmonic >= Pharms)
                harmonic = 0;
        }
        
        if (++kr >= dr)
            kr = 0;
        
        rvkl += pattern[harmonic];
        
        if (rvkl >= (dl))
            rvkl = rvkl % (dl);
        
        rvkr += pattern[harmonic];
        
        if (rvkr >= (dr))
            rvkr = rvkr % (dr);

        rvfl = rvkl + fade;
        
        if (rvfl >= (dl))
            rvfl = rvfl % (dl);
        
        rvfr = rvkr + fade;
        
        if (rvfr >= (dr))
            rvfr = rvfr % (dr);
    }
    
    if(have_nans)
        cleanup();
}

/*
 * Parameter control
 */
void
Arpie::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
    
    if (_Pvolume == 0)
        cleanup();
}

void
Arpie::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + 0.5f) / 127.0f;
}

void
Arpie::setreverse(int _Preverse)
{
    Preverse = _Preverse;
    reverse = (float) _Preverse / 127.0f;
}

void
Arpie::setdelay(int _Pdelay)
{
    Pdelay = _Pdelay;
    
    if (_Pdelay < 30)
        _Pdelay = 30;
    
    if (_Pdelay > 600)
        _Pdelay = 600; //100ms .. 2 sec constraint
    
    delay = 1 + lrintf((60.0f / ((float) (subdiv * _Pdelay))) * fSAMPLE_RATE); //quarter notes
    initdelays();

};

void
Arpie::setlrdelay(int _Plrdelay)
{
    float tmp;
    Plrdelay = _Plrdelay;
    tmp =  (powf(2.0, fabsf((float) _Plrdelay - 64.0f) / 64.0f * 9.0f) -
            1.0f) / 1000.0f * fSAMPLE_RATE;
    
    if (_Plrdelay < 64.0)
        tmp = -tmp;
    
    lrdelay = lrintf(tmp);
    initdelays();
}

void
Arpie::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f * 1.0f;
}

void
Arpie::setfb(int _Pfb)
{
    Pfb = _Pfb;
    fb = (float) _Pfb / 128.0f;
}

void
Arpie::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = 0.5f - (float) _Phidamp / 254.0f;
}

void
Arpie::setpattern(int _Ppattern)
{
    Ppattern = _Ppattern;

    const int PATTERN_SIZE = MAXHARMS;
    const int NUM_PATTERNS = 7;
    int setpatterns[NUM_PATTERNS][PATTERN_SIZE] = {
        {2, 3, 4, 5, 6, 7, 8, 9},
        {9, 8, 7, 6, 5, 4, 3, 2},
        {2, 4, 3, 5, 4, 6, 5, 7},
        {2, 2, 4, 3, 6, 2, 5, 3},
        {3, 2, 4, 3, 5, 4, 6, 5},
        {4, 3, 2, 7, 5, 3, 4, 2},
        {2, 3, 4, 5, 6, 7, 8, 9}
    };


    if (_Ppattern >= NUM_PATTERNS)
        _Ppattern = NUM_PATTERNS - 1;
    for (int ii = 0; ii < PATTERN_SIZE; ii++)
        pattern[ii] = setpatterns[_Ppattern][ii];
}

void
Arpie::setpreset(int npreset)
{
    const int PRESET_SIZE = C_ARPIE_PARAMETERS;
    const int NUM_PRESETS = 10;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        // Tremolo
        {0, 64, 126, 64, 30, 59, 0, 127, 1, 0, 3},
        //Arpie 1
        {67, 64, 35, 64, 30, 59, 0, 127, 4, 0, 0},
        //Arpie 2
        {67, 64, 21, 64, 30, 59, 0, 64, 4, 0, 0},
        //Arpie 3
        {67, 75, 60, 64, 30, 59, 10, 0, 4, 0, 0},
        //Simple Arpie
        {67, 60, 44, 64, 30, 0, 0, 0, 4, 0, 0},
        //Canyon
        {67, 60, 102, 50, 30, 82, 48, 0, 4, 0, 0},
        //Panning Arpie 1
        {67, 64, 44, 17, 0, 82, 24, 0, 4, 0, 0},
        //Panning Arpie 2
        {81, 60, 46, 118, 100, 68, 18, 0, 4, 0, 0},
        //Panning Arpie 3
        {81, 60, 26, 100, 127, 67, 36, 0, 4, 0, 0},
        //Feedback Arpie
        {62, 64, 28, 64, 100, 90, 55, 0, 4, 0, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_ARPIE, npreset - NUM_PRESETS + 1, pdata);
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
Arpie::changepar(int npar, int value)
{
    switch (npar)
    {
    case Arpie_DryWet:
        setvolume(value);
        break;
    case Arpie_Pan:
        setpanning(value);
        break;
    case Arpie_Tempo:
        setdelay(value);
        break;
    case Arpie_LR_Delay:
        setlrdelay(value);
        break;
    case Arpie_LR_Cross:
        setlrcross(value);
        break;
    case Arpie_Feedback:
        setfb(value);
        break;
    case Arpie_Damp:
        sethidamp(value);
        break;
    case Arpie_ArpeWD:
        setreverse(value);
        break;
    case Arpie_Harm:
        Pharms = value;
        if ((Pharms < 2) && (Pharms >= MAXHARMS))
        {
            Pharms = 2;
        }
        break;
    case Arpie_Pattern:
        setpattern(value);
        break;
    case Arpie_Subdivision:
        Psubdiv = value;
        subdiv = Psubdiv + 1;
        setdelay(Pdelay);
        break;
    default:
        return;
    }
}

int
Arpie::getpar(int npar)
{
    switch (npar)
    {
    case Arpie_DryWet:
        return (Pvolume);

    case Arpie_Pan:
        return (Ppanning);

    case Arpie_Tempo:
        return (Pdelay);

    case Arpie_LR_Delay:
        return (Plrdelay);

    case Arpie_LR_Cross:
        return (Plrcross);

    case Arpie_Feedback:
        return (Pfb);

    case Arpie_Damp:
        return (Phidamp);

    case Arpie_ArpeWD:
        return (Preverse);

    case Arpie_Harm:
        return (Pharms);

    case Arpie_Pattern:
        return (Ppattern);

    case Arpie_Subdivision:
        return (Psubdiv);

    }
    return (0); //in case of bogus parameter number
}
