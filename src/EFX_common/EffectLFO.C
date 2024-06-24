/*
  ZynAddSubFX - a software synthesizer

  EffectLFO.C - Stereo LFO used by some effects
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu 6 Ryan Billing


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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../global.h"
#include "EffectLFO.h"
#include "f_sin.h"

EffectLFO::EffectLFO(double sample_rate) :
    Pfreq(40),
    Prandomness(),
    PLFOtype(),
    Pstereo(96),
    ui_freq_min(LFO_FREQ_MIN),
    ui_freq_max(LFO_FREQ_MAX),
    fSAMPLE_RATE(sample_rate),
    fPERIOD(256),       // best guess, later will be changed in updateparams()
    iperiod(fPERIOD / fSAMPLE_RATE),
    xl(),
    xr(),
    incx(),
    ampl1(),
    ampl2(),
    ampr1(),
    ampr2(),
    lfointensity(),
    lfornd(),
    lfotype(),
    x0(),
    y0(),
    z0(),
    x1(),
    y1(),
    z1(),
    radius(),
    h(iperiod),
    a(10.0f),
    b(28.0f),
    c(8.0f / 5.0f),
    scale(1.0f / 36.0f),
    ratediv(0.1f),
    lfostatic_l (0.0f),
    lfostatic_r (0.0f),
    holdflag(),
    tca(iperiod / (iperiod + 0.02)),    // 20ms default
    tcb(1.0f - tca),
    maxrate(),
    rreg(),
    lreg(),
    xlreg(),
    xrreg(),
    oldrreg(),
    oldlreg()
{
    updateparams((uint32_t) fPERIOD);

    ampl1 = (1.0f - lfornd) + lfornd * (float) RND;
    ampl2 = (1.0f - lfornd) + lfornd * (float) RND;
    ampr1 = (1.0f - lfornd) + lfornd * (float) RND;
    ampr2 = (1.0f - lfornd) + lfornd * (float) RND;
}

EffectLFO::~EffectLFO()
{
}

/*
 * Update the changed parameters
 */
void
EffectLFO::updateparams(uint32_t period)
{
    fPERIOD = period;
    //must update several parameters once we actually know the period
    iperiod = fPERIOD / fSAMPLE_RATE;
    h = iperiod;
    tca = iperiod / (iperiod + 0.02); //20ms default
    tcb = 1.0f - tca;

    if (PLFOtype == LFO_STATIC)
        incx = 0;
    else
        incx = (float) Pfreq * fPERIOD / (fSAMPLE_RATE * 60.0f);

    if (incx > 0.49999999)
        incx = 0.499999999f; //Limit the Frequency

    lfornd = (float) Prandomness / 127.0f;
    
    if (lfornd < 0.0)
    {
        lfornd = 0.0;
    }
    else if (lfornd > 1.0)
    {
        lfornd = 1.0;
    }

    if (PLFOtype > LFO_NUM_TYPES)
        PLFOtype = 0;
    
    lfotype = PLFOtype;

    xr = fmodf(xl + ((float) Pstereo - 64.0f) / 127.0f + 1.0f, 1.0f);

    if ((h = incx * ratediv) > 0.02) h = 0.02; //keeps it stable

    a = 10.0f + (((float) RND) - 0.5f)*8.0f;
    b = 28.0f + (((float) RND) - 0.5f)*12.0f;
    c = 1.25f + 3.0f * ((float) RND);

    // printf("incx %f x0 %f y0 %f z0 %f out %f c %f b %f a %f\n",incx,x0,y0,z0, (2.0f * radius - 1.0f), c, b, a);

    x0 = 0.1f + 0.1f * ((float) RND);
    y0 = 0.0f;
    z0 = 0.2f;
    x1 = y1 = z1 = radius = 0.0f;

    float tmp = 6.0f / ((float) Pfreq); //S/H time attack  0.2*60=12.0
    tca = iperiod / (iperiod + tmp); //
    tcb = 1.0f - tca;
    maxrate = 4.0f * iperiod;

    float slider_range = ui_freq_max - ui_freq_min; // avoid div by zero
    if (slider_range < 1)
        slider_range = 1;
    float sep = CLAMP (((float) (Pstereo + 64.0f) / 128.0f) - 1.0f, -0.5, 0.5);

    // we return these directly from effectlfoout ()
#if 0
    if (static_wraparound)
    {
        // why not keep this, might be useful for ex. "barberpole" type effects
        lfostatic_l = (float) (Pfreq - ui_freq_min) / (0.5f + (float) slider_range);
        lfostatic_r = lfostatic_l + (float) (Pstereo + 64.0f) / 128.0f;
        lfostatic_l = fmod (CLAMP (lfostatic_l, 0.0f, ui_freq_max), 1.0f);
        lfostatic_r = fmod (CLAMP (lfostatic_r, 0.0f, ui_freq_max), 1.0f);
    } else
#endif
    {
        // leading slider reaches edge, goes back the other direction
        lfostatic_l = (float) (Pfreq - ui_freq_min) / (0.5f + (float) slider_range);
        lfostatic_l = fmod (CLAMP (lfostatic_l, 0.0f, ui_freq_max), 1.0f);
        lfostatic_r = lfostatic_l + sep;

        if (lfostatic_r > 1.0f)
            lfostatic_r = 2.0f - lfostatic_r;
        else if (lfostatic_r < 0.0f)
            lfostatic_r = 0.0f - lfostatic_r;
    }
}

/*
 * Compute the shape of the LFO
 */
float EffectLFO::getlfoshape(float x)
{
    float tmpv = 0.0;
    float out = 0.0;
    int iterations = 1; //make fractal go faster
    
    switch (lfotype)
    {
    case LFO_TRIANGLE:      // 1
        if ((x > 0.0) && (x < 0.25))
        {
            out = 4.0f * x;
        }
        else if ((x > 0.25) && (x < 0.75))
        {
            out = 2.0f - 4.0f * x;
        }
        else
        {
            out = 4.0f * x - 4.0f;
        }
        break;
    case LFO_RAMP_UP:       // 2
        out = 2.0f * x - 1.0f;
        break;
    case LFO_RAMP_DOWN:     // 3
        out = -2.0f * x + 1.0f;
        break;
    case LFO_ZIGZAG:        // 4
        x = x * 2.0f - 1.0f;
        tmpv = 0.33f * f_sin(x);
        out = f_sin(f_sin(x * D_PI) * x / tmpv);
        break;
    case LFO_MOD_SQUARE:    // 5
        tmpv = x * D_PI;
        out = f_sin(tmpv + f_sin(2.0f * tmpv));
        break;
    case LFO_MOD_SAW:       // 6 
        tmpv = x * D_PI;
        out = f_sin(tmpv + f_sin(tmpv));
        break;
    case LFO_L_FRACTAL:     // 8
        iterations = 4;     // intentional fall through
    case LFO_L_FRACTAL_XY:  // 7 -  faster, using X,Y outputs
        for (int j = 0; j < iterations; j++)
        {
            x1 = x0 + h * a * (y0 - x0);
            y1 = y0 + h * (x0 * (b - z0) - y0);
            z1 = z0 + h * (x0 * y0 - c * z0);
            x0 = x1;
            y0 = y1;
            z0 = z1;
        }
        if (lfotype == LFO_L_FRACTAL_XY)
        {
            if ((radius = (sqrtf(x0 * x0 + y0 * y0 + z0 * z0) * scale) - 0.25f) > 1.0f)
                radius = 1.0f;

            if (radius < 0.0)
                radius = 0.0;

            out = 2.0f * radius - 1.0f;
        }
        break;
    case LFO_SH_RANDOM:     // 9
        if (fmod(x, 0.5f) <= (2.0f * incx))
        { //this function is called by left, then right...so must toggle each time called
            rreg = lreg;
            lreg = RND1;
        }

        if (xlreg < lreg)
        {
            xlreg += maxrate;
        }
        else
        {
            xlreg -= maxrate;
        }
        
        if (xrreg < rreg)
        {
            xrreg += maxrate;
        }
        else
        {
            xrreg -= maxrate;
        }

        oldlreg = xlreg * tca + oldlreg*tcb;
        oldrreg = xrreg * tca + oldrreg*tcb;

        if (holdflag)
        {
            out = 2.0f * oldlreg - 1.0f;
            holdflag = (1 + holdflag) % 2;
        }
        else
        {
            out = 2.0f * oldrreg - 1.0f;
        }
        break;
    case LFO_TRI_TOP:       // 10
        if (x <= 0.5f)
        {
            out = -f_sin(x * D_PI);
        }
        else if ((x > 0.5f) && (x < 0.75f))
        {
            out = 6 * (x - 0.5);
        }
        else
        {
            out = 1.5 - 6.0f * (x - 0.75f);
        }
        
        out -= 0.25f;
        out *= 0.88888889f;
        break;
    case LFO_TRI_BOTTOM:    // 11
        if (x <= 0.5f)
        {
            out = -f_sin(x * D_PI);
        }
        else if ((x > 0.5f) && (x < 0.75f))
        {
            out = 6 * (x - 0.5);
        }
        else
        {
            out = 1.5 - 6.0f * (x - 0.75f);
        }

        out -= 0.25f;
        out *= -0.88888889f;
        break;
    case LFO_STATIC:        // 12
        out = 0;
    break;
        //more to be added here
    case LFO_SINE:          // 0 - for clarity
    default:
        out = f_cos(x * D_PI); // EffectLFO_SINE
    }

    return (out);
}

/*
 * LFO output
 */
void
EffectLFO::effectlfoout(float * outl, float * outr)
{
    float out = getlfoshape(xl);

    if (lfotype == LFO_STATIC)
    {
        *outl = lfostatic_l;
        *outr = lfostatic_r;
    }
    else
    {
        out *= (ampl1 + xl * (ampl2 - ampl1));
        xl += incx;

        if (xl > 1.0)
        {
            xl -= 1.0f;
            ampl1 = ampl2;
            ampl2 = (1.0f - lfornd) + lfornd * (float) RND;
        }

        if (lfotype == LFO_L_FRACTAL) // currently 8
        {
            out = scale * x0; //fractal parameter
        }

        *outl = (out + 1.0f) * 0.5f;

        if (lfotype == LFO_L_FRACTAL) // currently 8
        {
            out = scale * y0; //fractal parameter
        }
        else
        {
            out = getlfoshape(xr);
        }

        out *= (ampr1 + xr * (ampr2 - ampr1));
        xr += incx;

        if (xr > 1.0)
        {
            xr -= 1.0f;
            ampr1 = ampr2;
            ampr2 = (1.0f - lfornd) + lfornd * (float) RND;
        }

        *outr = (out + 1.0f) * 0.5f;
    }
}

