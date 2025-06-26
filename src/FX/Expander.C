
/*
  rakarrack - a guitar effects software

 Expander.C  -  Noise Gate Effect

  Copyright (C) 2010 Ryan Billing & Josep Andreu
  Author: Ryan Billing & Josep Andreu
  Adapted from swh-plugins Noise Gate by Steve Harris

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

#include <math.h>
#include "Expander.h"

Expander::Expander(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    Pthreshold(),
    Pattack(),
    Pdecay(),
    Pshape(),
    Plpf(),
    Phpf(),
    Plevel(),
    efollower(),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    sgain(),
    sfactor(),
    tfactor(),
    tlevel(),
    a_rate(),
    d_rate(),
    env(),
    oldgain(),
    gain(),
    fs(sample_rate),
    level(),
    interpbuf(NULL),
    lpfl(NULL),
    lpfr(NULL),
    hpfl(NULL),
    hpfr(NULL)
{
    initialize();
    Expander::setpreset(0);
}

Expander::~Expander()
{
    clear_initialize();
}

void
Expander::cleanup()
{
    lpfl->cleanup();
    hpfl->cleanup();
    lpfr->cleanup();
    hpfr->cleanup();
    oldgain = env = 0.0f;
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Expander::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    setlpf(Plpf);
    sethpf(Phpf);
}
#endif // LV2

void
Expander::set_random_parameters()
{
    for(int i = 0; i < C_EXPANDER_PARAMETERS; i++)
    {
        switch(i)
        {
            case Expander_Threshold:
            {
                int value = (int) (RND * 80);
                changepar (i, value - 80);
            }
            break;

            case Expander_Shape:
            {
                int value = (int) (RND * 49);
                changepar (i, value + 1);
            }
            break;

            case Expander_Attack:
            {
                int value = (int) (RND * 4999);
                changepar (i, value + 1);
            }
            break;

            case Expander_Release:
            {
                int value = (int) (RND * 990);
                changepar (i, value + 10);
            }
            break;

            case Expander_LPF:
            {
                int value = (int) (RND * 25980);
                changepar (i, value + 20);
            }
            break;

            case Expander_HPF:
            {
                int value = (int) (RND * 19980);
                changepar (i, value + 20);
            }
            break;

            case Expander_Gain:
            {
                int value = (int) (RND * 126);
                changepar (i, value + 1);
            }
            break;
        }
    }
}

void
Expander::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_EXPANDER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Expander_Threshold:
            case Expander_Shape:
            case Expander_Attack:
            case Expander_Release:
            case Expander_LPF:
            case Expander_HPF:
            case Expander_Gain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), expand_parameters[i * 3 + 1], expand_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Expander_Gain)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Expander::initialize()
{
    interpbuf = new float[PERIOD];
    lpfl = new AnalogFilter(2, 22000, 1, 0, fSAMPLE_RATE, interpbuf);
    lpfr = new AnalogFilter(2, 22000, 1, 0, fSAMPLE_RATE, interpbuf);
    hpfl = new AnalogFilter(3, 20, 1, 0, fSAMPLE_RATE, interpbuf);
    hpfr = new AnalogFilter(3, 20, 1, 0, fSAMPLE_RATE, interpbuf);
}

void Expander::clear_initialize()
{
    delete[] interpbuf;
    delete lpfl;
    delete lpfr;
    delete hpfl;
    delete hpfr;
}

void
Expander::setlpf(int value)
{
    Plpf = value;
    float fr = (float) Plpf;
    lpfl->setfreq(fr);
    lpfr->setfreq(fr);
}

void
Expander::sethpf(int value)
{
    Phpf = value;
    float fr = (float) Phpf;
    hpfl->setfreq(fr);
    hpfr->setfreq(fr);
}

void
Expander::out(float *efxoutl, float *efxoutr)
{
    bool have_nans = false;

    lpfl->filterout(efxoutl, PERIOD);
    hpfl->filterout(efxoutl, PERIOD);
    lpfr->filterout(efxoutr, PERIOD);
    hpfr->filterout(efxoutr, PERIOD);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float delta = 0.5f * (fabsf(efxoutl[i]) + fabsf(efxoutr[i])) - env; //envelope follower from Compressor.C
        
        if (delta > 0.0)
        {
            env += a_rate * delta;
        }
        else
        {
            env += d_rate * delta;
        }

        //End envelope power detection

        if (env > tlevel)
            env = tlevel;
        
        float expenv = sgain * (expf(env * sfactor * tfactor) - 1.0f); //Envelope waveshaping

        gain = (1.0f - d_rate) * oldgain + d_rate * expenv;
        oldgain = gain; //smooth it out a little bit

        if (efollower)
        {
            efxoutl[i] = gain;
            efxoutr[i] += gain;
        }
        else
        {
            efxoutl[i] *= gain*level;
            efxoutr[i] *= gain*level;
        }

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();
}

void
Expander::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

void
Expander::setpreset(int npreset)
{
    const int PRESET_SIZE = C_EXPANDER_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {

        //Noise Gate
        {-50, 20, 50, 50, 3134, 76, 1},
        //Boost Gate
        {-55, 30, 50, 50, 1441, 157, 50},
        //Treble swell
        {-30, 9, 950, 25, 6703, 526, 90},
        //Another
        {-30, 2, 20, 120, 3134, 76, 10}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_EXPANDER, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
}

void
Expander::changepar(int npar, int value)
{
    switch (npar)
    {
    case Expander_Threshold:
        Pthreshold = value;
        tfactor = dB2rap(-((float) Pthreshold));
        tlevel = 1.0f / tfactor;
        break;
    case Expander_Shape:
        Pshape = value;
        sfactor = dB2rap((float) Pshape / 2);
        sgain = expf(-sfactor);
        break;
    case Expander_Attack:
        Pattack = value;
        a_rate = 1000.0f / ((float) Pattack * fs);
        break;
    case Expander_Release:
        Pdecay = value;
        d_rate = 1000.0f / ((float) Pdecay * fs);
        break;
    case Expander_LPF:
        setlpf(value);
        break;
    case Expander_HPF:
        sethpf(value);
        break;
    case Expander_Gain: // Out Gain
        Plevel = value;
        level = dB2rap((float) value / 6.0f);
        break;
    default:
        return;
    }
}

int
Expander::getpar(int npar)
{
    switch (npar)
    {
    case Expander_Threshold:
        return (Pthreshold);

    case Expander_Shape:
        return (Pshape);

    case Expander_Attack:
        return (Pattack);

    case Expander_Release:
        return (Pdecay);

    case Expander_LPF:
        return (Plpf);

    case Expander_HPF:
        return (Phpf);

    case Expander_Gain:
        return (Plevel);

    }

    return (0);
}
