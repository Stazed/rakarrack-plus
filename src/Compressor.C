/*
  rakarrack - a guitar effects software

 Compressor.C  -  Compressor Effect
 Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
 Stefan Westerfeld <stefan@space.twc.de> 
 
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

	Patches:
	September 2009  Ryan Billing (a.k.a. Transmogrifox)
		--Modified DSP code to fix discontinuous gain change at threshold.
		--Improved automatic gain adjustment function
		--Improved handling of knee
		--Added support for user-adjustable knee
		--See inline comments

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
#include "Compressor.h"

#define  MIN_GAIN  0.00001f        // -100dB  This will help prevent evaluation of denormal numbers

Compressor::Compressor (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  rvolume = 0.0f;
  rvolume_db = 0.0f;
  lvolume = 0.0f;
  lvolume_db = 0.0f;
  tthreshold = -24;
  tratio = 4;
  toutput = -10;
  tatt = 20;
  trel = 50;
  a_out = 1;
  stereo = 0;
  tknee = 30;
  rgain = 1.0f;
  rgain_old = 1.0f;
  lgain = 1.0f;
  lgain_old = 1.0f;  
  lgain_t = 1.0f;
  rgain_t = 1.0f;
  ratio = 1.0;
  kpct = 0.0f;
  peak = 0;
  lpeak = 0.0f;
  rpeak = 0.0f;


}

Compressor::~Compressor ()
{


}


void
Compressor::cleanup ()
{

  lgain = rgain = 1.0f;
  lgain_old = rgain_old = 1.0f;
  rpeak = 0.0f;
  lpeak = 0.0f;

}


void
Compressor::Compressor_Change (int np, int value)
{

  switch (np)
    {

    case 1:
     	tthreshold = value;
	thres_db = (float)tthreshold;    //implicit type cast int to float
      break;

    case 2:
      tratio = value;
      ratio = (float)tratio;
      break;

    case 3:
	toutput = value;
      break;

    case 4:
      tatt = value;
      att = LN2 / fmaxf ((float)value / 1000.0f * (float)SAMPLE_RATE, LN2);
      break;

    case 5:
      trel = value;
      rel = LN2 / fmaxf ((float)value / 1000.0f * (float)SAMPLE_RATE, LN2);
      break;

    case 6:
      a_out = value;
      break;
      
    case 7:				
      tknee = value;  //knee expressed a percentage of range between thresh and zero dB
      kpct = (float)tknee/100.1f;
      break;
      
    case 8:
      stereo = value;
      break;
    case 9:
      peak = value;
      break;      


    }

      kratio = logf(ratio)/LOG_2;  //  Log base 2 relationship matches slope
      knee = -kpct*thres_db; 
          
      coeff_kratio = 1.0 / kratio;
      coeff_ratio = 1.0 / ratio;
      coeff_knee = 1.0 / knee;

      coeff_kk = knee * coeff_kratio;


      thres_mx = thres_db + knee;  //This is the value of the input when the output is at t+k
      makeup = -thres_db - knee/kratio + thres_mx/ratio;
      makeuplin = dB2rap(makeup);
      if (a_out)  
      outlevel = dB2rap((float)toutput) * makeuplin;
      else 
      outlevel = dB2rap((float)toutput);
      
}

int
Compressor::getpar (int np)
{

  switch (np)

    {

    case 1:
      return (tthreshold);
      break;
    case 2:
      return (tratio);
      break;
    case 3:
      return (toutput);
      break;
    case 4:
      return (tatt);
      break;
    case 5:
      return (trel);
      break;
    case 6:
      return (a_out);
      break;
    case 7:
      return (tknee);
      break;
    case 8:
      return (stereo);
      break;
    case 9:
      return (peak);
      break;   
    }

  return (0);

}


void
Compressor::Compressor_Change_Preset (int npreset)
{

  const int PRESET_SIZE = 8;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //2:1
    {-30, 2, -6, 20, 120, 1, 0, 0},
    //4:1
    {-26, 4, -8, 20, 120, 1, 10, 0},
    //8:1
    {-24, 8, -12, 20, 35, 1, 30, 0}
  };
	
  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    Compressor_Change (n + 1, presets[npreset][n]);


}



void
Compressor::out (float *efxoutl, float *efxoutr)
{

  int i;
 

  for (i = 0; i < PERIOD; i++)
    {
    float rdelta = 0.0f;
    float ldelta = 0.0f;
//Right Channel

     if(peak)
     {
     if(rpeak<efxoutr[i]) rpeak = fabs(efxoutr[i]);
     if(lpeak<efxoutl[i]) lpeak = fabs(efxoutl[i]);
     rpeak -= rpeak*rel;
     lpeak -= lpeak*rel;	//leaky peak detector.
     
     if(lpeak>5.0f) lpeak = 5.0f;
     if(rpeak>5.0f) rpeak = 5.0f; //keeps limiter from getting locked up when signal levels go way out of bounds (like hundreds)
     
     }
     else
     {
     rpeak = efxoutr[i];
     lpeak = efxoutl[i];
     }
     
	if(stereo) 
	{
      rdelta = fabsf (rpeak) - rvolume;

      if (rdelta > 0.0)
	rvolume += att * rdelta;
      else
	rvolume += rel * rdelta;	
	rvolume_db = rap2dB (rvolume);	

      if (rvolume_db < thres_db)
	{
	  rgain = outlevel;
	}
      else if (rvolume_db < thres_mx)
	{
	//Dynamic ratio that depends on volume.  As can be seen, ratio starts
	//at something negligibly larger than 1 once volume exceeds thres, and increases toward selected
	// ratio by the time it has reached thres_mx.  --Transmogrifox

	eratio = 1 + (kratio-1.0f)*(rvolume_db-thres_db)* coeff_knee;  
	rgain =   outlevel*dB2rap(thres_db + (rvolume_db-thres_db)/eratio - rvolume_db);
	}
      else
	{
	  rgain = outlevel*dB2rap(thres_db + coeff_kk + (rvolume_db-thres_mx)*coeff_ratio - rvolume_db);
	}
	
      if ( rgain < MIN_GAIN) rgain = MIN_GAIN;
      rgain_t = .4f * rgain + .6f * rgain_old;
	};

//Left Channel
	if(stereo)  {
	  ldelta = fabsf (lpeak) - lvolume;
	}	
	else  {
	  ldelta = 0.5f*(fabsf (lpeak) + fabsf (rpeak)) - lvolume;
	    };  //It's not as efficient to check twice, but it's small expense worth code clarity
		
      if (ldelta > 0.0)
	lvolume += att * ldelta;
      else
	lvolume += rel * ldelta;	
	
  	lvolume_db = rap2dB (lvolume);

      if (lvolume_db < thres_db)
	{
	  lgain = outlevel;
	}
      else if (lvolume_db < thres_mx)  //knee region
	{
	eratio = 1 + (kratio-1.0f)*(lvolume_db-thres_db)* coeff_knee;  
	lgain =   outlevel*dB2rap(thres_db + (lvolume_db-thres_db)/eratio - lvolume_db);
	}
      else
	{
	  lgain = outlevel*dB2rap(thres_db + coeff_kk + (lvolume_db-thres_mx)*coeff_ratio - lvolume_db);
	}


      if ( lgain < MIN_GAIN) lgain = MIN_GAIN;
      lgain_t = .4f * lgain + .6f * lgain_old;

      if (stereo) {
      efxoutl[i] *= lgain_t;
      efxoutr[i] *= rgain_t;
      rgain_old = rgain;
      lgain_old = lgain;      
      }
      else {
      efxoutl[i] *= lgain_t;
      efxoutr[i] *= lgain_t;  
      lgain_old = lgain;
      }

      if(peak)
      {
      if(efxoutl[i]>0.999f) efxoutl[i] = 0.999f;	//output hard limiting
      if(efxoutl[i]<-0.999f) efxoutl[i] = -0.999f;
      if(efxoutr[i]>0.999f) efxoutr[i] = 0.999f;	
      if(efxoutr[i]<-0.999f) efxoutr[i] = -0.999f;      
      }
    }

}

