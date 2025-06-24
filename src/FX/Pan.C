/*
  rakarrack - a guitar effects software

 pan.C  -  Auto/Pan -  Extra Stereo
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

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


#include "Pan.h"

Pan::Pan(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    Pvolume(),
    Ppanning(),
    Pextra(),
    PAutoPan(),
    PextraON(),
    dvalue(),
    cdvalue(),
    sdvalue(),
    panning(),
    mul(),
    lfol(),
    lfor(),
    ll(),
    lr(),
    lfo(NULL)
{
    lfo = new EffectLFO(sample_rate);
    Pan::setpreset(Ppreset);

    lfo->effectlfoout(&lfol, &lfor);

    Pan::cleanup();
}

Pan::~Pan()
{
    delete lfo;
}

void
Pan::cleanup()
{
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Pan::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    fPERIOD = period;
    lfo->updateparams(period);
}
#endif // LV2

void
Pan::set_random_parameters()
{
    for(int i = 0; i < C_PAN_PARAMETERS; i++)
    {
        switch(i)
        {
            case Pan_DryWet:
            case Pan_Pan:
            case Pan_LFO_Random:
            case Pan_LFO_Stereo:
            case Pan_Ex_St_Amt:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Pan_LFO_Type:
            {
                int value = (int) (RND * LFO_NUM_TYPES);
                changepar (i, value);
            }
            break;

            case Pan_LFO_Tempo:
            {
                int value = (int) (RND * LFO_FREQ_MAX);
                changepar (i, value + 1);
            }
            break;

            case Pan_AutoPan:
            case Pan_Enable_Extra:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Pan::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_PAN_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Pan_LFO_Tempo:
            case Pan_LFO_Random:
            case Pan_LFO_Type:
            case Pan_LFO_Stereo:
            case Pan_Ex_St_Amt:
            case Pan_AutoPan:
            case Pan_Enable_Extra:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), pan_parameters[i * 3 + 1], pan_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Pan_Enable_Extra)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Pan_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Pan_DryWet )), pan_parameters[i * 3 + 1], pan_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Pan_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Pan_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Pan_Pan ) - 64, pan_parameters[i * 3 + 1], pan_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Pan_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Pan::out(float *efxoutl, float *efxoutr)
{
    if (PextraON)
    {
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            float avg = (efxoutl[i] + efxoutr[i]) * .5f;

            float ldiff = efxoutl[i] - avg;
            float rdiff = efxoutr[i] - avg;

            float tmp = avg + ldiff * mul;
            efxoutl[i] = tmp*cdvalue;

            tmp = avg + rdiff * mul;
            efxoutr[i] = tmp*sdvalue;
        }
    }

    if (PAutoPan)
    {
        ll = lfol;
        lr = lfor;
        lfo->effectlfoout(&lfol, &lfor);
        
        float coeff_PERIOD = 1.0 / fPERIOD;
        
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            float fi = (float) i;
            float P_i = (float) (PERIOD - i);

            float pp = (ll * P_i + lfol * fi) * coeff_PERIOD;

            efxoutl[i] *= pp * panning;

            pp = (lr * P_i + lfor * fi) * coeff_PERIOD;

            efxoutr[i] *= pp * (1.0f - panning);
        }
    }
}

void
Pan::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Pan::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning) / 127.0f;
    dvalue = panning*M_PI_2;
    cdvalue = cosf(dvalue);
    sdvalue = sinf(dvalue);
}

void
Pan::setextra(int _Pextra)
{
    Pextra = _Pextra;
    mul = 4.0f * (float) _Pextra / 127.0f;
}

void
Pan::setpreset(int npreset)
{
    const int PRESET_SIZE = C_PAN_PARAMETERS;
    const int NUM_PRESETS = 2;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //AutoPan
        {64, 64, 26, 0, 0, 0, 0, 1, 0},
        //Extra Stereo
        {64, 64, 80, 0, 0, 0, 10, 0, 1}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_PAN, npreset - NUM_PRESETS + 1, pdata);
        
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
Pan::changepar(int npar, int value)
{
    switch (npar)
    {
    case Pan_DryWet:
        setvolume(value);
        break;
    case Pan_Pan:
        setpanning(value);
        break;
    case Pan_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case Pan_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case Pan_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case Pan_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case Pan_Ex_St_Amt:
        setextra(value);
        break;
    case Pan_AutoPan:
        PAutoPan = value;
        break;
    case Pan_Enable_Extra:
        PextraON = value;
        break;
    default:
        return;
    }
}

int
Pan::getpar(int npar)
{
    switch (npar)
    {
    case Pan_DryWet:
        return (Pvolume);

    case Pan_Pan:
        return (Ppanning);

    case Pan_LFO_Tempo:
        return (lfo->Pfreq);

    case Pan_LFO_Random:
        return (lfo->Prandomness);

    case Pan_LFO_Type:
        return (lfo->PLFOtype);

    case Pan_LFO_Stereo:
        return (lfo->Pstereo);

    case Pan_Ex_St_Amt:
        return (Pextra);

    case Pan_AutoPan:
        return (PAutoPan);

    case Pan_Enable_Extra:
        return (PextraON);

    }

    return (0);
}
