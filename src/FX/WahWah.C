/*
  ZynAddSubFX - a software synthesizer

  WahWah.C - "WahWah" effect and others
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

// Renamed from DynamicFilter.C on 4/19/2020 by stazed

#include <math.h>
#include "WahWah.h"
#include <stdio.h>

WahWah::WahWah(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(),
    Ppanning(),
    Pdepth(),
    Pampsns(),
    Pampsnsinv(),
    Pampsmooth(),
    Pmode(),
    panning(),
    depth(),
    ampsns(),
    ampsmooth(),
    ms1(),
    ms2(),
    ms3(),
    ms4(),
    lfo(NULL),
    filterl(NULL),
    filterr(NULL),
    filterpars(NULL)
{
    lfo = new EffectLFO(sample_rate);
    filterpars = new FilterParams(0, 64, 64, sample_rate, PERIOD);
    setpreset(Ppreset);
    cleanup();
}

WahWah::~WahWah()
{
    delete lfo;
    delete filterpars;
    delete filterl;
    delete filterr;
}

/*
 * Apply the effect
 */
void
WahWah::out(float * efxoutl, float * efxoutr)
{
    if (filterpars->changed)
    {
        filterpars->changed = false;
        cleanup();
        return;
    }

    bool have_nans = false;

    float lfol, lfor;   // initialize o.k.
    lfo->effectlfoout(&lfol, &lfor);
    lfol *= depth * 5.0f;
    lfor *= depth * 5.0f;
    float freq = filterpars->getfreq();
    float q = filterpars->getq();

    for (uint32_t i = 0; i < PERIOD; i++)
    {
        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }

        float x = (fabsf(efxoutl[i]) + fabsf(efxoutr[i])) * 0.5f;
        ms1 = ms1 * (1.0f - ampsmooth) + x * ampsmooth + 1e-10f;
    }

    if(have_nans)
    {
        cleanup();
        return;
    }

    float ampsmooth2 = powf(ampsmooth, 0.2f) * 0.3f;
    ms2 = ms2 * (1.0f - ampsmooth2) + ms1 * ampsmooth2;
    ms3 = ms3 * (1.0f - ampsmooth2) + ms2 * ampsmooth2;
    ms4 = ms4 * (1.0f - ampsmooth2) + ms3 * ampsmooth2;
    float rms = (sqrtf(ms4)) * ampsns;

    float frl = filterl->getrealfreq(freq + lfol + rms);
    float frr = filterr->getrealfreq(freq + lfor + rms);

    filterl->setfreq_and_q(frl, q);
    filterr->setfreq_and_q(frr, q);

    filterl->filterout(efxoutl, PERIOD);
    filterr->filterout(efxoutr, PERIOD);

    //panning
    for (uint32_t i = 0; i < PERIOD; i++)
    {
        efxoutl[i] *= (1.0f - panning);
        efxoutr[i] *= panning;
    }
}

/*
 * Cleanup the effect
 */
void
WahWah::cleanup()
{
    reinitfilter();
    ms1 = 0.0;
    ms2 = 0.0;
    ms3 = 0.0;
    ms4 = 0.0;
}

#ifdef LV2_SUPPORT
void
WahWah::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        delete filterpars;
        filterpars = new FilterParams(0, 64, 64, fSAMPLE_RATE, PERIOD);
        reinitfilter();
    }
    else
    {
        PERIOD = period;
    }
    
    lfo->updateparams(period);
}
#endif // LV2

void
WahWah::set_random_parameters()
{
    for(int i = 0; i < C_WAHWAH_PARAMETERS; i++)
    {
        switch(i)
        {
            case WahWah_DryWet:
            case WahWah_Pan:
            case WahWah_LFO_Random:
            case WahWah_LFO_Stereo:
            case WahWah_Depth:
            case WahWah_Sense:
            case WahWah_Smooth:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case WahWah_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case WahWah_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case WahWah_ASI:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case WahWah_Mode:
            {
                int value = (int) (RND * 5);
                changepar (i, value);
            }
            break;
        }
    }
}

void
WahWah::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_WAHWAH_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case WahWah_LFO_Tempo:
            case WahWah_LFO_Random:
            case WahWah_LFO_Type:
            case WahWah_LFO_Stereo:
            case WahWah_Depth:
            case WahWah_Sense:
            case WahWah_ASI:
            case WahWah_Smooth:
            case WahWah_Mode:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), wah_parameters[i * 3 + 1], wah_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  WahWah_Mode )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case WahWah_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( WahWah_DryWet )), wah_parameters[i * 3 + 1], wah_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( WahWah_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case WahWah_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( WahWah_Pan ) - 64, wah_parameters[i * 3 + 1], wah_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( WahWah_Pan ) - 64);
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
WahWah::setdepth(int _Pdepth)
{
    Pdepth = _Pdepth;
    depth = powf(((float) _Pdepth / 127.0f), 2.0f);
}

void
WahWah::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
WahWah::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + .5f) / 127.0f;
}

void
WahWah::setampsns(int _Pampsns)
{
    ampsns = powf((float) _Pampsns / 127.0f, 2.5f) * 10.0f;
    
    if (Pampsnsinv != 0)
        ampsns = -ampsns;
    
    ampsmooth = expf((float) -Pampsmooth / 127.0f * 10.0f) * 0.99f;
    Pampsns = _Pampsns;
}

void
WahWah::reinitfilter()
{
    if (filterl != NULL)
        delete (filterl);
    
    if (filterr != NULL)
        delete (filterr);
    
    filterl = new Filter(filterpars);
    filterr = new Filter(filterpars);
}

void
WahWah::setpreset(int npreset)
{
    const int PRESET_SIZE = C_WAHWAH_PARAMETERS;
    const int NUM_PRESETS = 5;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //WahWah
        {64, 64, 80, 0, 0, 64, 70, 90, 0, 60, 0},
        //AutoWah
        {64, 64, 70, 0, 0, 80, 70, 0, 0, 60, 1},
        //Sweep
        {64, 64, 30, 0, 0, 50, 80, 0, 0, 60, 2},
        //VocalMorph1
        {64, 64, 80, 0, 0, 64, 70, 64, 0, 60, 3},
        //VocalMorph1
        {64, 64, 50, 0, 0, 96, 64, 0, 0, 60, 4}
    };

    if (npreset > NUM_PRESETS - 1)
    {

        Fpre->ReadPreset(EFX_WAHWAH, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
    //          for (int i=0;i<5;i++){
    //              printf("freq=%d  amp=%d  q=%d\n",filterpars->Pvowels[0].formants[i].freq,filterpars->Pvowels[0].formants[i].amp,filterpars->Pvowels[0].formants[i].q);
    //          };
}

void
WahWah::changepar(int npar, int value)
{
    switch (npar)
    {
    case WahWah_DryWet:
        setvolume(value);
        break;
    case WahWah_Pan:
        setpanning(value);
        break;
    case WahWah_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case WahWah_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case WahWah_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case WahWah_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case WahWah_Depth:
        setdepth(value);
        break;
    case WahWah_Sense:
        setampsns(value);
        break;
    case WahWah_ASI:
        Pampsnsinv = value;
        setampsns(Pampsns);
        break;
    case WahWah_Smooth:
        Pampsmooth = value;
        setampsns(Pampsns);
        break;

    case WahWah_Mode:
    {
        Pmode = value;
        filterpars->defaults();
        switch (Pmode)
        {
        case 0:
            filterpars->Pcategory = 0;
            filterpars->Ptype = 2;
            filterpars->Pfreq = 45;
            filterpars->Pq = 64;
            filterpars->Pstages = 1;
            filterpars->Pgain = 64;
            break;
        case 1:
            filterpars->Pcategory = 2;
            filterpars->Ptype = 0;
            filterpars->Pfreq = 72;
            filterpars->Pq = 64;
            filterpars->Pstages = 0;
            filterpars->Pgain = 64;
            break;
        case 2:
            filterpars->Pcategory = 0;
            filterpars->Ptype = 4;
            filterpars->Pfreq = 64;
            filterpars->Pq = 64;
            filterpars->Pstages = 2;
            filterpars->Pgain = 64;
            break;
        case 3:
            filterpars->Pcategory = 1;
            filterpars->Ptype = 0;
            filterpars->Pfreq = 50;
            filterpars->Pq = 70;
            filterpars->Pstages = 1;
            filterpars->Pgain = 64;

            filterpars->Psequencesize = 2;
            // "I"
            filterpars->Pvowels[0].formants[0].freq = 34;
            filterpars->Pvowels[0].formants[0].amp = 127;
            filterpars->Pvowels[0].formants[0].q = 64;
            filterpars->Pvowels[0].formants[1].freq = 99;
            filterpars->Pvowels[0].formants[1].amp = 122;
            filterpars->Pvowels[0].formants[1].q = 64;
            filterpars->Pvowels[0].formants[2].freq = 108;
            filterpars->Pvowels[0].formants[2].amp = 112;
            filterpars->Pvowels[0].formants[2].q = 64;
            // "A"
            filterpars->Pvowels[1].formants[0].freq = 61;
            filterpars->Pvowels[1].formants[0].amp = 127;
            filterpars->Pvowels[1].formants[0].q = 64;
            filterpars->Pvowels[1].formants[1].freq = 71;
            filterpars->Pvowels[1].formants[1].amp = 121;
            filterpars->Pvowels[1].formants[1].q = 64;
            filterpars->Pvowels[1].formants[2].freq = 99;
            filterpars->Pvowels[1].formants[2].amp = 117;
            filterpars->Pvowels[1].formants[2].q = 64;
            break;
        case 4:
            filterpars->Pcategory = 1;
            filterpars->Ptype = 0;
            filterpars->Pfreq = 64;
            filterpars->Pq = 70;
            filterpars->Pstages = 1;
            filterpars->Pgain = 64;

            filterpars->Psequencesize = 2;
            filterpars->Pnumformants = 2;
            filterpars->Pvowelclearness = 0;

            filterpars->Pvowels[0].formants[0].freq = 70;
            filterpars->Pvowels[0].formants[0].amp = 127;
            filterpars->Pvowels[0].formants[0].q = 64;
            filterpars->Pvowels[0].formants[1].freq = 80;
            filterpars->Pvowels[0].formants[1].amp = 122;
            filterpars->Pvowels[0].formants[1].q = 64;

            filterpars->Pvowels[1].formants[0].freq = 20;
            filterpars->Pvowels[1].formants[0].amp = 127;
            filterpars->Pvowels[1].formants[0].q = 64;
            filterpars->Pvowels[1].formants[1].freq = 100;
            filterpars->Pvowels[1].formants[1].amp = 121;
            filterpars->Pvowels[1].formants[1].q = 64;
            break;
        }   // switch (Pmode)
        /* We do not want to re-initialize the filter here because it deletes the filter. 
         * If it hits just at the wrong moment, then out() will try to use the just deleted filter
         * and crash. So we set changed and check and delete in out() at - if (filterpars->changed).
         */
        filterpars->changed = true;
        break;
    }   // case 10:

    default:
        return;
    }   // switch (npar)
}

int
WahWah::getpar(int npar)
{
    switch (npar)
    {
    case WahWah_DryWet:
        return (Pvolume);

    case WahWah_Pan:
        return (Ppanning);

    case WahWah_LFO_Tempo:
        return (lfo->Pfreq);

    case WahWah_LFO_Random:
        return (lfo->Prandomness);

    case WahWah_LFO_Type:
        return (lfo->PLFOtype);

    case WahWah_LFO_Stereo:
        return (lfo->Pstereo);

    case WahWah_Depth:
        return (Pdepth);

    case WahWah_Sense:
        return (Pampsns);

    case WahWah_ASI:
        return (Pampsnsinv);

    case WahWah_Smooth:
        return (Pampsmooth);

    case WahWah_Mode:
        return (Pmode);

    }

    return (0);
}
