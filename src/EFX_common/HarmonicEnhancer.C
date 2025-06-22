/*

  HarmonicEnhancer.C  -  Class
  This file is based in the harmonic_gen_1220.c by Steve Harris
  Copyright (C) 2008-2009 Josep Andreu (Holborn)
  Author: Josep Andreu
  Based on Steve Harris LADSPA harmonic.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License
(version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "HarmonicEnhancer.h"

HarmEnhancer::HarmEnhancer(float *Rmag, float hfreq, float lfreq, float gain,
                           double sample_rate, uint32_t intermediate_bufsize) :
    realvol(gain),
    hpffreq(hfreq),
    lpffreq(lfreq),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    HFREQ(hfreq),
    LFREQ(lfreq),
    inputl(NULL),
    inputr(NULL),
    vol(),
#ifdef UNUSED_STUFF
    itm1l(),
    itm1r(),
    otm1l(),
    otm1r(),
#endif
    p(),
    hpfl(NULL),
    hpfr(NULL),
    lpfl(NULL),
    lpfr(NULL),
    interpbuf(NULL),
    limiter(NULL)
{
    initialize();

    set_vol(0, gain);   // sets vol

    limiter = new Limiter(sample_rate, PERIOD);
    limiter->setpreset(1);
    calcula_mag(Rmag);
}

HarmEnhancer::~HarmEnhancer()
{
    clear_initialize();
    delete limiter;
}

void
HarmEnhancer::cleanup()
{
    lpfl->cleanup();
    hpfl->cleanup();
    lpfr->cleanup();
    hpfr->cleanup();
    limiter->cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
HarmEnhancer::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    clear_initialize();
    initialize();
    limiter->lv2_update_params(period);
}
#endif // LV2

void
HarmEnhancer::initialize()
{
    inputl = (float *) malloc(sizeof (float) * PERIOD);
    inputr = (float *) malloc(sizeof (float) * PERIOD);
    unsigned int i = 0;

    for (i = 0; i < PERIOD; i++)
    {
        inputl[i] = inputr[i] = 0;
    }

    interpbuf = new float[PERIOD];
    hpfl = new AnalogFilter(3, HFREQ, 1, 0, fSAMPLE_RATE, interpbuf);
    hpfr = new AnalogFilter(3, HFREQ, 1, 0, fSAMPLE_RATE, interpbuf);
    lpfl = new AnalogFilter(2, LFREQ, 1, 0, fSAMPLE_RATE, interpbuf);
    lpfr = new AnalogFilter(2, LFREQ, 1, 0, fSAMPLE_RATE, interpbuf);
}

void
HarmEnhancer::clear_initialize()
{
    free(inputl);
    free(inputr);
    delete hpfl;
    delete hpfr;
    delete lpfl;
    delete lpfr;
    delete[] interpbuf;
}

void
HarmEnhancer::set_vol(int mode, float gain)
{
    if (!mode)
    {
        vol = gain;
    }
    else
    {
        vol = realvol + gain;
    }

    vol *= 2.0f;
}

void
HarmEnhancer::set_freqh(int mode, float freq)
{
    if (!mode)
    {
        hpffreq = freq;
        freq = 0.0;
    }

    hpfl->setfreq(hpffreq + freq);
    hpfr->setfreq(hpffreq + freq);
}

void
HarmEnhancer::set_freql(int mode, float freq)
{
    if (!mode)
    {
        lpffreq = freq;
        freq = 0.0;
    }

    lpfl->setfreq(lpffreq + freq);
    lpfr->setfreq(lpffreq + freq);
}

/* Calculate Chebychev coefficents from partial magnitudes, adapted from
 * example in Num. Rec. */
void
HarmEnhancer::chebpc(const float c[], float d[])
{

    float sv, dd[HARMONICS];    // initialize o.k.

    for (int j = 0; j < HARMONICS; j++)
    {
        d[j] = dd[j] = 0.0;
    }

    d[0] = c[HARMONICS - 1];

    for (int j = HARMONICS - 2; j >= 1; j--)
    {
        for (int k = HARMONICS - j; k >= 1; k--)
        {
            sv = d[k];
            d[k] = 2.0 * d[k - 1] - dd[k];
            dd[k] = sv;
        }

        sv = d[0];
        d[0] = -dd[0] + c[j];
        dd[0] = sv;
    }

    for (int j = HARMONICS - 1; j >= 1; j--)
    {
        d[j] = d[j - 1] - dd[j];
    }

    d[0] = -dd[0] + 0.5 * c[0];
}

void
HarmEnhancer::calcula_mag(float *Rmag)
{

    float mag[HARMONICS] = {
        0.0f, Rmag[0], Rmag[1], Rmag[2], Rmag[3], Rmag[4], Rmag[5],
        Rmag[6], Rmag[7], Rmag[8], Rmag[9]
    };

    // Normalise magnitudes
    float mag_fix = 0.0f;
    for (int i = 0; i < 10; i++)
    {
        mag_fix += fabs(Rmag[i]);
    }

    if (mag_fix < 1.0f)
    {
        mag_fix = 1.0f;
    }
    else
    {
        mag_fix = 1.0f / mag_fix;
    }

    for (int i = 0; i < HARMONICS; i++)
    {
        mag[i] *= mag_fix;
    }

    // Calculate polynomial coefficients, using Chebychev aproximation
    chebpc(mag, p);
}

void
HarmEnhancer::harm_out(float *efxoutl, float *efxoutr)
{
    bool have_nans = false;

    memcpy(inputl, efxoutl, sizeof (float)*PERIOD);
    memcpy(inputr, efxoutr, sizeof (float)*PERIOD);

    hpfl->filterout(inputl, PERIOD);
    hpfr->filterout(inputr, PERIOD);

    limiter->out(inputl, inputr);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        float xl = inputl[i];
        float xr = inputr[i];
        float yl = 0.0f;
        float yr = 0.0f;

        for (unsigned j = 10; j > 0; j--)
        {
            yl = (yl + p[j]) * xl;
            yr = (yr + p[j]) * xr;
        }

        yl += p[0];
        yr += p[0];

#ifdef UNUSED_STUFF
        // this unused stuff is effectively the same as the used below
        otm1l = 0.999f * otm1l + yl - itm1l;
        itm1l = yl;
        otm1r = 0.999f * otm1r + yr - itm1r;
        itm1r = yr;

        otm1l = yl;
        otm1r = yr;

        inputl[i] = otm1l;
        inputr[i] = otm1r;
#else
        inputl[i] = yl;
        inputr[i] = yr;
#endif
    }

    lpfl->filterout(inputl, PERIOD);
    lpfr->filterout(inputr, PERIOD);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        efxoutl[i] = (efxoutl[i] + inputl[i] * vol);
        efxoutr[i] = (efxoutr[i] + inputr[i] * vol);

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();
}
