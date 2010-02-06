/*

  HarmonicEnhancer.C  -  Class
  This file is based in the harmonic_gen_1220.c by Steve Harris
  Copyright (C) 2008-2009 Josep Andreu (Holborn)
  Author: Josep Andreu

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


HarmEnhancer::HarmEnhancer(float *Rmag, float hfreq, float lfreq, float gain) 
{

 inputl = (float *) malloc (sizeof (float) * PERIOD);
 inputr = (float *) malloc (sizeof (float) * PERIOD);

  set_vol(0,gain);
  realvol = gain;
  itm1l = 0.0f;
  itm1r = 0.0f;
  otm1l = 0.0f;
  otm1r = 0.0f;
  coeff = 1.0;

  hpffreq = hfreq;
  lpffreq = lfreq;
  hpfl = new AnalogFilter(3, hfreq, 1, 0);
  hpfr = new AnalogFilter(3, hfreq, 1, 0);
  lpfl = new AnalogFilter(2, lfreq, 1, 0);
  lpfr = new AnalogFilter(2, lfreq, 1, 0);

  calcula_mag(Rmag);
}


HarmEnhancer::~HarmEnhancer()
{
};

void
HarmEnhancer::cleanup()
{
  lpfl->cleanup ();
  hpfl->cleanup ();
  lpfr->cleanup ();
  hpfr->cleanup ();


};




void
HarmEnhancer::set_vol(int mode, float gain)
{
  if(!mode) vol = gain;
  else
  vol = realvol + 2.0 * gain;
}

void  
HarmEnhancer::set_freqh(int mode, float freq)
{
if(!mode) 
{
hpffreq = freq;
freq = 0.0;
}

hpfl->setfreq(hpffreq+freq);
hpfr->setfreq(hpffreq+freq);

}


void  
HarmEnhancer::set_freql(int mode, float freq)
{
if(!mode)
{
 lpffreq = freq;
 freq = 0.0;
} 

lpfl->setfreq(lpffreq+freq);
lpfr->setfreq(lpffreq+freq);

}




/* Calculate Chebychev coefficents from partial magnitudes, adapted from
 * example in Num. Rec. */
void
HarmEnhancer::chebpc (float c[], float d[])
{
  int j,k;

  float sv, dd[HARMONICS];

  for (j = 0; j < HARMONICS; j++)
    {
      d[j] = dd[j] = 0.0;
    }

  d[0] = c[HARMONICS - 1];

  for (j = HARMONICS - 2; j >= 1; j--)
    {
      for (k = HARMONICS - j; k >= 1; k--)
	{
	  sv = d[k];
	  d[k] = 2.0 * d[k - 1] - dd[k];
	  dd[k] = sv;
	}
      sv = d[0];
      d[0] = -dd[0] + c[j];
      dd[0] = sv;
    }

  for (j = HARMONICS - 1; j >= 1; j--)
    {
      d[j] = d[j - 1] - dd[j];
    }
  d[0] = -dd[0] + 0.5 * c[0];
}



void
HarmEnhancer::calcula_mag (float *Rmag)
{

  int i;
  float mag_fix = 0.0f;

  float mag[HARMONICS] =
    { 0.0f, Rmag[0], Rmag[1], Rmag[2], Rmag[3], Rmag[4], Rmag[5],
    Rmag[6], Rmag[7], Rmag[8], Rmag[9]
  };

  // Normalise magnitudes

  for (i = 0; i < 10; i++)
    mag_fix += fabs (Rmag[i]);


  if (mag_fix < 1.0f)
    {
      mag_fix = 1.0f;
    }
  else
    {
      mag_fix = 1.0f / mag_fix;
    }
  for (i = 0; i < HARMONICS; i++)
    {
      mag[i] *= mag_fix;
    }

  // Calculate polynomial coefficients, using Chebychev aproximation
  chebpc (mag, p);

}

void
HarmEnhancer::harm_out(float *smpsl, float *smpsr)
{

  int i;
  float max=0.0;

  for(i=0;i<PERIOD;i++)
   {
     if(fabsf(smpsl[i])> max) max = smpsl[i];
     if(fabsf(smpsr[i])> max) max = smpsr[i];     
   }  

     coeff = fabsf(max)+8.0;
     max = 1.0f / coeff;
             

    for(i=0;i<PERIOD;i++)
   {
     
     inputl[i] =smpsl[i]*max;
     inputr[i] =smpsr[i]*max;
   }  
       

  hpfl->filterout(inputl);
  hpfr->filterout(inputr);

  for (i=0; i<PERIOD; i++)
    {
      float xl = inputl[i];
      float xr = inputr[i];
      float yl,yr;

      // Calculate the polynomial using Horner's Rule
      yl = p[0] + (p[1] + (p[2] + (p[3] + (p[4] + (p[5] + (p[6] + (p[7] + (p[8] + (p[9] + p[10] * xl) * xl) * xl) * xl) * xl) * xl) * xl) * xl) * xl) * xl;

      yr = p[0] + (p[1] + (p[2] + (p[3] + (p[4] + (p[5] + (p[6] + (p[7] + (p[8] + (p[9] + p[10] * xr) * xr) * xr) * xr) * xr) * xr) * xr) * xr) * xr) * xr;

      // DC offset remove (odd harmonics cause DC offset)
      otm1l = 0.999f * otm1l + yl - itm1l;
      itm1l = yl;
      otm1r = 0.999f * otm1r + yr - itm1r;
      itm1r = yr;
      inputl[i] = otm1l * vol * coeff;
      inputr[i] = otm1r * vol * coeff;

     }

  lpfl->filterout(inputl);
  lpfr->filterout(inputr);

    for (i=0; i<PERIOD; i++)
    {
      smpsl[i] = (smpsl[i]+inputl[i])*.5;
      smpsr[i] = (smpsr[i]+inputr[i])*.5;
    }
    
}



