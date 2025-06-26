/*
  ZynAddSubFX - a software synthesizer

  EQ.C - EQ effect
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
#include "EQ.h"

EQ::EQ(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(64),
    interpbuf(NULL),
    filter()
{
    outvolume = 0.7f;
    EQ::initialize();
    EQ::cleanup();
};

EQ::~EQ()
{
    clear_initialize();
};

/*
 * Cleanup the effect
 */
void
EQ::cleanup()
{
    for (int i = 0; i < MAX_EQ_BANDS; i++)
    {
        filter[i].l->cleanup();
        filter[i].r->cleanup();
    }
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
EQ::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
}
#endif // LV2

void
EQ::init_filters()
{
    interpbuf = new float[PERIOD];

    for (int i = 0; i < MAX_EQ_BANDS; i++)
    {
        filter[i].Ptype = 0;
        filter[i].Pfreq = 64;
        filter[i].Pgain = 64;
        filter[i].Pq = 64;
        filter[i].Pstages = 0;
        filter[i].l = new AnalogFilter(6, 1000.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
        filter[i].r = new AnalogFilter(6, 1000.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    }
}

void
EQ::initialize()
{
    init_filters();

    for (int i = 0; i <= 45; i += 5)
    {
        change_parameters(i + 10, 7);
        change_parameters(i + 14, 0);
    }

    change_parameters(11, 31);
    change_parameters(16, 63);
    change_parameters(21, 125);
    change_parameters(26, 250);
    change_parameters(31, 500);
    change_parameters(36, 1000);
    change_parameters(41, 2000);
    change_parameters(46, 4000);
    change_parameters(51, 8000);
    change_parameters(56, 16000);
}

void
EQ::clear_initialize()
{
    for (int i = 0; i < MAX_EQ_BANDS; i++)
    {
        delete filter[i].l;
        delete filter[i].r;
    }
    
    delete[] interpbuf;
}

/*
 * Effect output
 */
void
EQ::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    for (unsigned i = 0; i < MAX_EQ_BANDS; i++)
    {
        if (filter[i].Ptype == 0)
            continue;
        
        filter[i].l->filterout(efxoutl, PERIOD);
        filter[i].r->filterout(efxoutr, PERIOD);
    }

    for (unsigned i = 0; i < PERIOD; i++)
    {
        efxoutl[i] = efxoutl[i] * outvolume;
        efxoutr[i] = efxoutr[i] * outvolume;
        
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
EQ::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */
void
EQ::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = powf(0.005f, (1.0f - (float) _Pvolume / 127.0f)) * 10.0f;
}

void
EQ::set_random_parameters()
{
    for(int i = 0; i < C_EQ_PARAMETERS; i++)
    {
        switch(i)
        {
            case EQ_31_HZ:
            case EQ_63_HZ:
            case EQ_125_HZ:
            case EQ_250_HZ:
            case EQ_500_HZ:
            case EQ_1_KHZ:
            case EQ_2_KHZ:
            case EQ_4_KHZ:
            case EQ_8_KHZ:
            case EQ_16_KHZ:
            case EQ_Gain:
            case EQ_Q:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;
        }
    }
}

void
EQ::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 10;
    for(int i = 0; i < C_EQ_PARAMETERS; i++)
    {
        switch(param_case_offset)
        {
            case EQ_Gain:   // 0
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, 1, getpar( EQ_Gain ) - 64, eq_parameters[0 * 3 + 1], eq_parameters[0 * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar(EQ_Gain) - 64 );
                    s_buf += ":";
                }
                
                param_case_offset = EQ_Q; // set for EQ_Q
            }
            break;

            case EQ_Q:      // 1
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, 2, getpar( EQ_Q ) - 64, eq_parameters[1 * 3 + 1], eq_parameters[1 * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar(EQ_Q) - 64 );
                    s_buf += ":";
                }
    
                param_case_offset = EQ_31_HZ;   // set for EQ_31_HZ
            }
            break;
            
            case EQ_31_HZ:  // = 2
            case EQ_63_HZ:
            case EQ_125_HZ:
            case EQ_250_HZ:
            case EQ_500_HZ:
            case EQ_1_KHZ:
            case EQ_2_KHZ:
            case EQ_4_KHZ:
            case EQ_8_KHZ:
            case EQ_16_KHZ: // 9
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, param_case_offset + 2, getpar( param_case_offset ) - 64, eq_parameters[(param_case_offset + 2) * 3 + 1], eq_parameters[(param_case_offset + 2) * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar(param_case_offset) - 64 );

                    if ( param_case_offset !=  EQ_16_KHZ)   // last one no need for delimiter
                        s_buf += ":";
                }

                param_case_offset++;     // next parameter
            }
            break;
        }
    }
}

/**
 * This changes the Q for all ten EQ1 bands. Not used by Cabinet or Parametric.
 * 
 * @param npar
 *      The EQ_Q + 2 index >> 13 in this case.
 * 
 * @param value
 *      The slider value, or MIDI control value Q adjustment for each band. 
 */
void
EQ::changepar_Q(int npar, int value)
{
    for (int i = 0; i < 10; i++)
    {
        change_parameters(((i * 5) + npar), value);
    }
}

/**
 *  This is the default preset for regular equalizer - NOT Parametric or Cabinet.
 *  Parametric and Cabinet will override this function.
 * 
 * @param npreset
 *      The preset number to set.
 */
void
EQ::setpreset(int npreset)
{
    const int PRESET_SIZE = C_EQ_PARAMETERS;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
        //Pop
        {78, 71, 64, 64, 64, 64, 64, 64, 71, 80, 64, 42},
        //Jazz
        {71, 68, 64, 64, 64, 64, 64, 64, 66, 69, 64, 40}
    };

    if (npreset >= NUM_PRESETS)
    {
        Fpre->ReadPreset(EFX_EQ, npreset - NUM_PRESETS + 1, pdata);
        for (int n = 0; n < 10; n++)
        {
            change_parameters(n * 5 + 12, pdata[n]);
        }

        change_parameters(0, pdata[EQ_Gain]);
        changepar_Q(EQ_Q + 2, pdata[EQ_Q]);

    }
    else
    {
        for (int n = 0; n < 10; n++)
        {
            change_parameters(n * 5 + 12, presets[npreset][n]);
        }
        
        change_parameters(0, presets[npreset][EQ_Gain]); 
        changepar_Q(EQ_Q + 2, presets[npreset][EQ_Q]);
    }
}

void
EQ::change_parameters (int npar, int value)
{
    switch (npar)
    {
    case 0:
        setvolume(value);
        break;
    }
    
    if (npar < 10)
        return;

    int nb = (npar - 10) / 5; //number of the band (filter)
    
    if (nb >= MAX_EQ_BANDS)
        return;
    
    int bp = npar % 5; //band paramenter

    float tmp;
    switch (bp)
    {
    case 0:
        if (value > 9)
            value = 0; //has to be changed if more filters will be added
        filter[nb].Ptype = value;
        if (value != 0)
        {
            filter[nb].l->settype(value - 1);
            filter[nb].r->settype(value - 1);
        }
        break;
    case 1:
        filter[nb].Pfreq = value;
        tmp = (float) value;
        filter[nb].l->setfreq(tmp);
        filter[nb].r->setfreq(tmp);
        break;
    case 2:
        filter[nb].Pgain = value;
        tmp = 30.0f * ((float) value - 64.0f) / 64.0f;
        filter[nb].l->setgain(tmp);
        filter[nb].r->setgain(tmp);
        break;
    case 3:
        filter[nb].Pq = value;
        tmp = powf(30.0f, ((float) value - 64.0f) / 64.0f);
        filter[nb].l->setq(tmp);
        filter[nb].r->setq(tmp);
        break;
    case 4:
        if (value >= MAX_FILTER_STAGES)
            value = MAX_FILTER_STAGES - 1;
        filter[nb].Pstages = value;
        filter[nb].l->setstages(value);
        filter[nb].r->setstages(value);
        break;
    }
}

int
EQ::get_parameters (int npar)
{
    switch (npar)
    {
    case 0:
        return (Pvolume);

    }

    if (npar < 10)
        return (0);

    int nb = (npar - 10) / 5; //number of the band (filter)
    
    if (nb >= MAX_EQ_BANDS)
        return (0);
    
    int bp = npar % 5; //band paramenter
    switch (bp)
    {
    case 0:
        return (filter[nb].Ptype);

    case 1:
        return (filter[nb].Pfreq);

    case 2:
        return (filter[nb].Pgain);

    case 3:
        return (filter[nb].Pq);

    case 4:
        return (filter[nb].Pstages);

    }

    return (0); //in case of bogus parameter number
}

void
EQ::changepar(int npar, int value)
{
    switch(npar)
    {
    case EQ_Gain:
        change_parameters(0, value);
        break;
    case EQ_Q:
        changepar_Q(EQ_Q + 2, value);
        break;
    case EQ_31_HZ:
    case EQ_63_HZ:
    case EQ_125_HZ:
    case EQ_250_HZ:
    case EQ_500_HZ:
    case EQ_1_KHZ:
    case EQ_2_KHZ:
    case EQ_4_KHZ:
    case EQ_8_KHZ:
    case EQ_16_KHZ:
        change_parameters(npar * 5 + 12, value);
        break;
    default:
        return;
    }
}

int
EQ::getpar(int npar)
{
    switch(npar)
    {
    case EQ_Gain:
        return get_parameters(0);

    case EQ_Q:
        return get_parameters(EQ_Q + 2);

    case EQ_31_HZ:
    case EQ_63_HZ:
    case EQ_125_HZ:
    case EQ_250_HZ:
    case EQ_500_HZ:
    case EQ_1_KHZ:
    case EQ_2_KHZ:
    case EQ_4_KHZ:
    case EQ_8_KHZ:
    case EQ_16_KHZ:
        return get_parameters(npar * 5 + 12);

    }
    
    return 0;
}

/**
 *  This is never called. Probably leftover from zynaddsubfx.
 * 
 * @param freq
 * 
 * @return 
 */
float EQ::getfreqresponse(float freq)
{
    float resp = 1.0f;

    for (int i = 0; i < MAX_EQ_BANDS; i++)
    {
        if (filter[i].Ptype == 0)
            continue;
        
        resp *= filter[i].l->H(freq);
    }
    return (rap2dB(resp * outvolume));
}
