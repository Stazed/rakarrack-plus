/*
  ZynAddSubFX - a software synthesizer

  Distorsion.C - Distorsion effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu & Ryan Billing

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
#include "FPreset.h"

Distorsion::Distorsion(int wave_res, int wave_upq,
                       int wave_dnq, double samplerate,
                       uint32_t intermediate_bufsize) :
    Ppreset(),
    octoutl(NULL),
    octoutr(NULL),
    outvolume(0.5f),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(samplerate),
    WAVE_RES(wave_res),
    WAVE_UPQ(wave_upq),
    WAVE_DNQ(wave_dnq),
    Pvolume(50),
    Ppanning(),
    Plrcross(40),
    Pdrive(90),
    Plevel(64),
    Ptype(),
    Pnegate(),
    Plpf(127),
    Phpf(),
    Pstereo(),
    Pprefiltering(),
    Poctave(),
    panning(),
    lrcross(),
    octave_memoryl(-1.0),
    togglel(1.0),
    octave_memoryr(-1.0),
    toggler(1.0),
    octmix(),
    lpfl(NULL),
    lpfr(NULL),
    hpfl(NULL),
    hpfr(NULL),
    blockDCl(NULL),
    blockDCr(NULL),
    DCl(NULL),
    DCr(NULL),
    dwshapel(NULL),
    dwshaper(NULL),
    Fpre(NULL),
    interpbuf(NULL)
{
    initialize();

    setpreset(0, Ppreset);
    cleanup();
}

Distorsion::~Distorsion()
{
    clear_initialize();
}

/*
 * Cleanup the effect
 */
void
Distorsion::cleanup()
{
    lpfl->cleanup();
    hpfl->cleanup();
    lpfr->cleanup();
    hpfr->cleanup();
    blockDCr->cleanup();
    blockDCl->cleanup();
    DCl->cleanup();
    DCr->cleanup();
}

std::vector<int>
Distorsion::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_DIST_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Distorsion::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_DIST_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#ifdef LV2_SUPPORT
void
Distorsion::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        clear_initialize();
        initialize();
    }
    else
    {
        PERIOD = period;
    }
}
#endif // LV2

/*
 * Apply the filters
 */

void
Distorsion::applyfilters(float * efxoutl, float * efxoutr)
{
    lpfl->filterout(efxoutl, PERIOD);
    hpfl->filterout(efxoutl, PERIOD);

    if (Pstereo != 0)
    {
        //stereo
        lpfr->filterout(efxoutr, PERIOD);
        hpfr->filterout(efxoutr, PERIOD);
    }
}

void
Distorsion::initialize()
{
    octoutl = (float *) malloc(sizeof (float) * PERIOD);
    octoutr = (float *) malloc(sizeof (float) * PERIOD);
    
    for (unsigned i = 0; i < PERIOD; i++)
    {
        octoutl[i] = octoutr[i] = 0;
    }

    interpbuf = new float[PERIOD];
    lpfl = new AnalogFilter(2, 22000, 1, 0, fSAMPLE_RATE, interpbuf);
    lpfr = new AnalogFilter(2, 22000, 1, 0, fSAMPLE_RATE, interpbuf);
    hpfl = new AnalogFilter(3, 20, 1, 0, fSAMPLE_RATE, interpbuf);
    hpfr = new AnalogFilter(3, 20, 1, 0, fSAMPLE_RATE, interpbuf);
    blockDCl = new AnalogFilter(2, 440.0f, 1, 0, fSAMPLE_RATE, interpbuf);
    blockDCr = new AnalogFilter(2, 440.0f, 1, 0, fSAMPLE_RATE, interpbuf);
    blockDCl->setfreq(75.0f);
    blockDCr->setfreq(75.0f);
    DCl = new AnalogFilter(3, 30, 1, 0, fSAMPLE_RATE, interpbuf);
    DCr = new AnalogFilter(3, 30, 1, 0, fSAMPLE_RATE, interpbuf);
    DCl->setfreq(30.0f);
    DCr->setfreq(30.0f);

    dwshapel = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    dwshaper = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
}

void
Distorsion::clear_initialize()
{
    free(octoutl);
    free(octoutr);
    delete lpfl;
    delete lpfr;
    delete hpfl;
    delete hpfr;
    delete blockDCl;
    delete blockDCr;
    delete DCl;
    delete DCr;
    delete dwshapel;
    delete dwshaper;
    delete[] interpbuf;
}

/*
 * Effect output
 */
void
Distorsion::out(float * efxoutl, float * efxoutr)
{
    float inputvol = powf(5.0f, ((float) Pdrive - 32.0f) / 127.0f);
    
    if (Pnegate != 0)
        inputvol *= -1.0f;

    if (Pstereo != 0)
    {
        //Stereo
        for (unsigned i = 0; i < PERIOD; i++)
        {
            efxoutl[i] = efxoutl[i] * inputvol * 2.0f;
            efxoutr[i] = efxoutr[i] * inputvol * 2.0f;
        }
    }
    else
    {
        for (unsigned i = 0; i < PERIOD; i++)
        {
            efxoutl[i] = (efxoutl[i] + efxoutr[i]) * inputvol;
        }
    }

    if (Pprefiltering != 0)
        applyfilters(efxoutl, efxoutr);

    //no optimised, yet (no look table)

    dwshapel->waveshapesmps(PERIOD, efxoutl, Ptype, Pdrive, 1);
    
    if (Pstereo != 0)
        dwshaper->waveshapesmps(PERIOD, efxoutr, Ptype, Pdrive, 1);

    if (Pprefiltering == 0)
        applyfilters(efxoutl, efxoutr);

    if (Pstereo == 0) memcpy(efxoutr, efxoutl, PERIOD * sizeof (float));

    if (octmix > 0.01f)
    {
        for (unsigned i = 0; i < PERIOD; i++)
        {
            float lout = efxoutl[i];
            float rout = efxoutr[i];

            if ((octave_memoryl < 0.0f) && (lout > 0.0f))
                togglel *= -1.0f;

            octave_memoryl = lout;

            if ((octave_memoryr < 0.0f) && (rout > 0.0f))
                toggler *= -1.0f;

            octave_memoryr = rout;

            octoutl[i] = lout * togglel;
            octoutr[i] = rout * toggler;
        }

        blockDCr->filterout(octoutr, PERIOD);
        blockDCl->filterout(octoutl, PERIOD);
    }

    float level = dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float lout = efxoutl[i];
        float rout = efxoutr[i];

        float l = lout * (1.0f - lrcross) + rout * lrcross;
        float r = rout * (1.0f - lrcross) + lout * lrcross;

        if (octmix > 0.01f)
        {
            lout = l * (1.0f - octmix) + octoutl[i] * octmix;
            rout = r * (1.0f - octmix) + octoutr[i] * octmix;
        }
        else
        {
            lout = l;
            rout = r;
        }

        efxoutl[i] = lout * 2.0f * level * (1.0f - panning);
        efxoutr[i] = rout * 2.0f * level * panning;
    }

    DCr->filterout(efxoutr, PERIOD);
    DCl->filterout(efxoutl, PERIOD);
}

/*
 * Parameter control
 */
void
Distorsion::setvolume(int Pvolume)
{
    this->Pvolume = Pvolume;

    outvolume = (float) Pvolume / 127.0f;
    
    if (Pvolume == 0)
        cleanup();
}

void
Distorsion::setpanning(int Ppanning)
{
    this->Ppanning = Ppanning;
    panning = ((float) Ppanning + 0.5f) / 127.0f;
}

void
Distorsion::setlrcross(int Plrcross)
{
    this->Plrcross = Plrcross;
    lrcross = (float) Plrcross / 127.0f * 1.0f;
}

void
Distorsion::setlpf(int value)
{
    Plpf = value;
    float fr = (float) Plpf;
    lpfl->setfreq(fr);
    lpfr->setfreq(fr);
}

void
Distorsion::sethpf(int value)
{
    Phpf = value;
    float fr = (float) Phpf;

    hpfl->setfreq(fr);
    hpfr->setfreq(fr);
    //Prefiltering of 51 is approx 630 Hz. 50 - 60 generally good for OD pedal.
}

void
Distorsion::setoctave(int Poctave)
{
    this->Poctave = Poctave;
    octmix = (float) (Poctave) / 127.0f;
}

void
Distorsion::setpreset(int dgui, int npreset)
{
    const int PRESET_SIZE = C_DIST_PARAMETERS;
    const int NUM_PRESETS = 6;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Overdrive 1
        {84, 64, 35, 56, 40, 0, 0, 6703, 21, 0, 0, 0, 0},
        //Overdrive 2
        {85, 64, 35, 29, 45, 1, 0, 25040, 21, 0, 0, 0, 0},
        //Distorsion 1
        {0, 64, 0, 87, 14, 6, 0, 3134, 157, 0, 1, 0, 0},
        //Distorsion 2
        {0, 64, 127, 87, 14, 0, 1, 3134, 102, 0, 0, 0, 20},
        //Distorsion 3
        {0, 64, 127, 127, 12, 13, 0, 5078, 56, 0, 1, 0, 0},
        //Guitar Amp
        {84, 64, 35, 63, 50, 2, 0, 824, 21, 0, 0, 0, 0}
    };

    // (npreset > 5) means user defined (Insert) presets for Distorsion
    if ((dgui == 0) && (npreset > 5))       // efx_Distorsion = dgui = 0
    {
        Fpre->ReadPreset(EFX_DISTORTION, npreset - 5, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    // (npreset > 1) means user defined (Insert) presets for Overdrive
    else if ((dgui == 1) && (npreset > 1))  // efx_Overdrive = dgui = 1
    {
        Fpre->ReadPreset(EFX_OVERDRIVE, npreset - 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else    // No user defined presets so just set it
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
    
    Ppreset = npreset;
    cleanup();
}

void
Distorsion::changepar(int npar, int value)
{
    switch (npar)
    {
    case Dist_DryWet:
        setvolume(value);
        break;
    case Dist_Pan:
        setpanning(value);
        break;
    case Dist_LR_Cross:
        setlrcross(value);
        break;
    case Dist_Drive:
        Pdrive = value;
        break;
    case Dist_Level:
        Plevel = value;
        break;
    case Dist_Type:
        Ptype = value;
        break;
    case Dist_Negate:
        if (value > 1)
            value = 1;
        Pnegate = value;
        break;
    case Dist_LPF:
        setlpf(value);
        break;
    case Dist_HPF:
        sethpf(value);
        break;
    case Dist_Stereo:
        if (value > 1)
            value = 1;
        Pstereo = value;
        break;
    case Dist_Prefilter:
        Pprefiltering = value;
        break;
    case Dist_SKIP_11:
        break;
    case Dist_Suboctave:
        setoctave(value);
        break;
    }
}

int
Distorsion::getpar(int npar)
{
    switch (npar)
    {
    case Dist_DryWet:
        return (Pvolume);
        break;
    case Dist_Pan:
        return (Ppanning);
        break;
    case Dist_LR_Cross:
        return (Plrcross);
        break;
    case Dist_Drive:
        return (Pdrive);
        break;
    case Dist_Level:
        return (Plevel);
        break;
    case Dist_Type:
        return (Ptype);
        break;
    case Dist_Negate:
        return (Pnegate);
        break;
    case Dist_LPF:
        return (Plpf);
        break;
    case Dist_HPF:
        return (Phpf);
        break;
    case Dist_Stereo:
        return (Pstereo);
        break;
    case Dist_Prefilter:
        return (Pprefiltering);
        break;
    case Dist_SKIP_11:
        return (0);
        break;
    case Dist_Suboctave:
        return (Poctave);
        break;
    }
    return (0); //in case of bogus parameter number
}



