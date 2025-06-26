/*
  Shuffle.C - Distorsion effect

  ZynAddSubFX - a software synthesizer
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Distorsion.h"
#include "Shuffle.h"

/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */

Shuffle::Shuffle(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    inputl(NULL),
    inputr(NULL),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(50),
    PvolL(),
    PvolML(),
    PvolMH(),
    PvolH(),
    PQ(),
    E(),
    Cross1(),
    Cross2(),
    Cross3(),
    Cross4(),
    tmp(),
    volL(),
    volML(),
    volMH(),
    volH(),
    lr(NULL),
    hr(NULL),
    mlr(NULL),
    mhr(NULL),
    interpbuf(NULL)
{
    initialize();

    Shuffle::setpreset(Ppreset);
    Shuffle::cleanup();
}

Shuffle::~Shuffle()
{
    clear_initialize();
}

/*
 * Cleanup the effect
 */
void
Shuffle::cleanup()
{
    lr->cleanup();
    hr->cleanup();
    mlr->cleanup();
    mhr->cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Shuffle::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    setCross1(Cross1);
    setCross2(Cross2);
    setCross3(Cross3);
    setCross4(Cross4);
    setGainL(getpar(Shuffle_Gain_L));
    setGainML(getpar(Shuffle_Gain_ML));
    setGainMH(getpar(Shuffle_Gain_MH));
    setGainH(getpar(Shuffle_Gain_H));
    set_q(PQ);
}
#endif // LV2

void
Shuffle::set_random_parameters()
{
    for(int i = 0; i < C_SHUFFLE_PARAMETERS; i++)
    {
        switch(i)
        {
            case Shuffle_Gain_L:
            case Shuffle_Gain_ML:
            case Shuffle_Gain_MH:
            case Shuffle_Gain_H:
            case Shuffle_Width:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Shuffle_Freq_L:
            {
                int value = (int) (RND * 980);
                changepar (i, value + 20);
            }
            break;

            case Shuffle_Freq_ML:
            {
                int value = (int) (RND * 3600);
                changepar (i, value + 400);
            }
            break;

            case Shuffle_Freq_MH:
            {
                int value = (int) (RND * 6800);
                changepar (i, value + 1200);
            }
            break;

            case Shuffle_Freq_H:
            {
                int value = (int) (RND * 20000);
                changepar (i, value + 6000);
            }
            break;

            case Shuffle_F_Band:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Shuffle_DryWet:
            {
                int value = (int) (RND * 128);
                changepar (i, Dry_Wet(value));
            }
            break;
        }
    }
}

void
Shuffle::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SHUFFLE_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Shuffle_Gain_L:
            case Shuffle_Gain_ML:
            case Shuffle_Gain_MH:
            case Shuffle_Gain_H:
            case Shuffle_Freq_L:
            case Shuffle_Freq_ML:
            case Shuffle_Freq_MH:
            case Shuffle_Freq_H:
            case Shuffle_Width:
            case Shuffle_F_Band:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), shuffle_parameters[i * 3 + 1], shuffle_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Shuffle_F_Band )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Shuffle_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Shuffle_DryWet )), shuffle_parameters[i * 3 + 1], shuffle_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Shuffle_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Shuffle::initialize()
{
    inputl = (float *) malloc(sizeof (float) * PERIOD);
    inputr = (float *) malloc(sizeof (float) * PERIOD);

    interpbuf = new float[PERIOD];
    lr = new AnalogFilter(6, 300.0f, .3f, 0, fSAMPLE_RATE, interpbuf);
    hr = new AnalogFilter(6, 8000.0f, .3f, 0, fSAMPLE_RATE, interpbuf);
    mlr = new AnalogFilter(6, 1200.0f, .3f, 0, fSAMPLE_RATE, interpbuf);
    mhr = new AnalogFilter(6, 2400.0f, .3f, 0, fSAMPLE_RATE, interpbuf);
}

void
Shuffle::clear_initialize()
{
    free(inputl);
    free(inputr);
    delete[] interpbuf;
    delete lr;
    delete hr;
    delete mlr;
    delete mhr;
}

/*
 * Effect output
 */
void
Shuffle::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    for (unsigned int i = 0; i < PERIOD; i++)
    {

        inputl[i] = efxoutl[i] + efxoutr[i];
        inputr[i] = efxoutl[i] - efxoutr[i];
    }

    if (E)
    {
        lr->filterout(inputr, PERIOD);
        mlr->filterout(inputr, PERIOD);
        mhr->filterout(inputr, PERIOD);
        hr->filterout(inputr, PERIOD);
    }
    else
    {
        lr->filterout(inputl, PERIOD);
        mlr->filterout(inputl, PERIOD);
        mhr->filterout(inputl, PERIOD);
        hr->filterout(inputl, PERIOD);
    }

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        efxoutl[i] = (inputl[i] + inputr[i] - efxoutl[i])*.333333f;
        efxoutr[i] = (inputl[i] - inputr[i] - efxoutr[i])*.333333f;

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
Shuffle::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 128.0f;
}

void
Shuffle::setCross1(int value)
{
    Cross1 = value;
    lr->setfreq((float) value);
}

void
Shuffle::setCross2(int value)
{
    Cross2 = value;
    mlr->setfreq((float) value);
}

void
Shuffle::setCross3(int value)
{
    Cross3 = value;
    mhr->setfreq((float) value);
}

void
Shuffle::setCross4(int value)
{
    Cross4 = value;
    hr->setfreq((float) value);
}

void
Shuffle::setGainL(int value)
{
    PvolL = value + 64;
    volL = 30.0f * ((float) PvolL - 64.0f) / 64.0f;
    lr->setgain(volL);
}

void
Shuffle::setGainML(int value)
{
    PvolML = value + 64;
    volML = 30.0f * ((float) PvolML - 64.0f) / 64.0f;
    mlr->setgain(volML);
}

void
Shuffle::setGainMH(int value)
{
    PvolMH = value + 64;
    volMH = 30.0f * ((float) PvolMH - 64.0f) / 64.0f;
    mhr->setgain(volMH);
}

void
Shuffle::setGainH(int value)
{
    PvolH = value + 64;
    volH = 30.0f * ((float) PvolH - 64.0f) / 64.0f;
    hr->setgain(volH);
}

void
Shuffle::set_q(int value)
{
    PQ = value;
    value += 64;
    tmp = powf(30.0f, ((float) value - 64.0f) / 64.0f);
    lr->setq(tmp);
    mlr->setq(tmp);
    mhr->setq(tmp);
    hr->setq(tmp);
}

void
Shuffle::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SHUFFLE_PARAMETERS;
    const int NUM_PRESETS = 5;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Shuffle 1
        {64, 10, 0, 0, 0, 600, 1200, 2000, 6000, -14, 1},
        //Shuffle 2
        {64, 0, 0, 0, 0, 120, 1000, 2400, 8000, -7, 1},
        //Shuffle 3
        {64, 0, 0, 0, 0, 60, 1800, 3700, 12000, 7, 0},
        //Shuffle 4
        {0, 0, 0, 0, 0, 200, 400, 2000, 12000, 0, 0},
        //Remover
        {0, 17, 0, 7, 5, 600, 1200, 2000, 13865, -45, 1}
    };
    
    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_SHUFFLE, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
    
    Ppreset = npreset;
    cleanup();
}

void
Shuffle::changepar(int npar, int value)
{
    switch (npar)
    {
    case Shuffle_DryWet:
        setvolume(value);
        break;
    case Shuffle_Gain_L:
        setGainL(value);
        break;
    case Shuffle_Gain_ML:
        setGainML(value);
        break;
    case Shuffle_Gain_MH:
        setGainMH(value);
        break;
    case Shuffle_Gain_H:
        setGainH(value);
        break;
    case Shuffle_Freq_L:
        setCross1(value);
        break;
    case Shuffle_Freq_ML:
        setCross2(value);
        break;
    case Shuffle_Freq_MH:
        setCross3(value);
        break;
    case Shuffle_Freq_H:
        setCross4(value);
        break;
    case Shuffle_Width:
        set_q(value);
        break;
    case Shuffle_F_Band:
        E = value;
        break;
    default:
        return;
    }
}

int
Shuffle::getpar(int npar)
{
    switch (npar)
    {
    case Shuffle_DryWet:
        return (Pvolume);

    case Shuffle_Gain_L:
        return (PvolL - 64);

    case Shuffle_Gain_ML:
        return (PvolML - 64);

    case Shuffle_Gain_MH:
        return (PvolMH - 64);

    case Shuffle_Gain_H:
        return (PvolH - 64);

    case Shuffle_Freq_L:
        return (Cross1);

    case Shuffle_Freq_ML:
        return (Cross2);

    case Shuffle_Freq_MH:
        return (Cross3);

    case Shuffle_Freq_H:
        return (Cross4);

    case Shuffle_Width:
        return (PQ);

    case Shuffle_F_Band:
        return (E);
    }
    return (0); //in case of bogus parameter number
}

