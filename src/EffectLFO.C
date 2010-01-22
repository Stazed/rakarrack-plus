/*
  ZynAddSubFX - a software synthesizer
 
  EffectLFO.C - Stereo LFO used by some effects
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "global.h"
#include "EffectLFO.h"


EffectLFO::EffectLFO ()
{
  xl = 0.0;
  xr = 0.0;
  Pfreq = 40;
  Prandomness = 0;
  PLFOtype = 0;
  Pstereo = 96;

  updateparams ();

  ampl1 = (1.0f - lfornd) + lfornd * (float)RND;
  ampl2 = (1.0f - lfornd) + lfornd * (float)RND;
  ampr1 = (1.0f - lfornd) + lfornd * (float)RND;
  ampr2 = (1.0f - lfornd) + lfornd * (float)RND;
};

EffectLFO::~EffectLFO ()
{
};


/*
 * Update the changed parameters
 */
void
EffectLFO::updateparams ()
{
  REALTYPE lfofreq = (powf (2.0f, (float)Pfreq / 127.0f * 10.0f) - 1.0f) * 0.03f;
  incx = fabsf (lfofreq) * (REALTYPE) PERIOD / (REALTYPE) SAMPLE_RATE;
  if (incx > 0.49999999)
    incx = 0.499999999f;		//Limit the Frequency

  lfornd = (float)Prandomness / 127.0f;
  if (lfornd < 0.0)
    lfornd = 0.0;
  else if (lfornd > 1.0)
    lfornd = 1.0;

  if (PLFOtype > 5)
    PLFOtype = 0;		//this has to be updated if more lfo's are added
  lfotype = PLFOtype;

  xr = fmodf (xl + ((float)Pstereo - 64.0f) / 127.0f + 1.0f, 1.0f);
};


/*
 * Compute the shape of the LFO
 */
REALTYPE EffectLFO::getlfoshape (REALTYPE x)
{
  REALTYPE
    out,tmpv;
  switch (lfotype)
    {
    case 1:			//EffectLFO_TRIANGLE
      if ((x > 0.0) && (x < 0.25))
	out = 4.0f * x;
      else if ((x > 0.25) && (x < 0.75))
	out = 2.0f - 4.0f * x;
      else
	out = 4.0f * x - 4.0f;
      break;
    case 2:			//EffectLFO_RAMP Ramp+
    out = 2.0f * x - 1.0f;	 
    break;  
    case 3:			//EffectLFO_RAMP Ramp-
    out = - 2.0f * x + 1.0f;	 
    break; 
    case 4:                     //ZigZag
       x = x * 2.0f - 1.0f;
        tmpv = 0.33f * sinf(x);
        out = sinf(sinf(x*2.0f*PI)*x/tmpv);       
     break;  
    case 5:                     //Modulated Square ?? ;-)
       tmpv = x * D_PI;
       out=sinf(tmpv+sinf(2.0f*tmpv));  
     break; 
      //more to be added here; also ::updateparams() need to be updated (to allow more lfotypes)
    default:
      out = cosf (x * 2.0f * PI);	//EffectLFO_SINE
    };
  return (out);
};

/*
 * LFO output
 */
void
EffectLFO::effectlfoout (REALTYPE * outl, REALTYPE * outr)
{
  REALTYPE out;

  out = getlfoshape (xl);
 // if ((lfotype == 0) || (lfotype == 1))         //What was that for?
    out *= (ampl1 + xl * (ampl2 - ampl1));
  xl += incx;
  if (xl > 1.0)
    {
      xl -= 1.0f;
      ampl1 = ampl2;
      ampl2 = (1.0f - lfornd) + lfornd * (float)RND;
    };
  *outl = (out + 1.0f) * 0.5f;

  out = getlfoshape (xr);
 // if ((lfotype == 0) || (lfotype == 1))
    out *= (ampr1 + xr * (ampr2 - ampr1));
  xr += incx;
  if (xr > 1.0)
    {
      xr -= 1.0f;
      ampr1 = ampr2;
      ampr2 = (1.0f - lfornd) + lfornd * (float)RND;
    };
  *outr = (out + 1.0f) * 0.5f;
};

