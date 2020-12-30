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
    PERIOD(intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    Ppreset(),
    outvolume(0.5f),
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
    Fpre(NULL),
    lfo(NULL)
{
    lfo = new EffectLFO(sample_rate);
    setpreset(Ppreset);

    lfo->effectlfoout(&lfol, &lfor);

    cleanup();
}

Pan::~Pan()
{
    delete lfo;
}

void
Pan::cleanup()
{
}

#ifdef LV2_SUPPORT
void
Pan::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    fPERIOD = period;
    lfo->updateparams(period);
}
#endif // LV2

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
Pan::setvolume(int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (float) Pvolume / 127.0f;
}

void
Pan::setpanning(int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((float) Ppanning) / 127.0f;
    dvalue = panning*M_PI_2;
    cdvalue = cosf(dvalue);
    sdvalue = sinf(dvalue);
}

void
Pan::setextra(int Pextra)
{
    this->Pextra = Pextra;
    mul = 4.0f * (float) Pextra / 127.0f;
}

void
Pan::setpreset(int npreset)
{
    const int PRESET_SIZE = 9;
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
        Fpre->ReadPreset(13, npreset - NUM_PRESETS + 1, pdata);
        
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
    }
}

int
Pan::getpar(int npar)
{
    switch (npar)
    {
    case Pan_DryWet:
        return (Pvolume);
        break;
    case Pan_Pan:
        return (Ppanning);
        break;
    case Pan_LFO_Tempo:
        return (lfo->Pfreq);
        break;
    case Pan_LFO_Random:
        return (lfo->Prandomness);
        break;
    case Pan_LFO_Type:
        return (lfo->PLFOtype);
        break;
    case Pan_LFO_Stereo:
        return (lfo->Pstereo);
        break;
    case Pan_Ex_St_Amt:
        return (Pextra);
        break;
    case Pan_AutoPan:
        return (PAutoPan);
        break;
    case Pan_Enable_Extra:
        return (PextraON);
        break;
    default:
        return (0);
    }
}
