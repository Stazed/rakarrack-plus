/*
  Rakarrack Guitar FX
 
  Sustainer.C - Simple compressor/sustainer effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License 
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
#include "Sustainer.h"

Sustainer::Sustainer (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  Pvolume = 64;
  Psustain = 64;
  fsustain = 0.5f;
  level = 0.5f;
  
  float tmp = 0.01f;  //10 ms decay time on peak detectors
  prls = 1.0f - (cSAMPLE_RATE/(cSAMPLE_RATE + tmp));

  tmp = 0.05f; //50 ms att/rel on compressor
  calpha =  cSAMPLE_RATE/(cSAMPLE_RATE + tmp);
  cbeta = 1.0f - calpha;
  cthresh = 0.25f;
  cratio = 0.25f;
    
  cleanup ();
};

Sustainer::~Sustainer ()
{
 
};

/*
 * Cleanup the effect
 */
void
Sustainer::cleanup ()
{
  compeak = 0.0f;
  compenv = 0.0f;  
  oldcompenv = 0.0f;
  cpthresh = cthresh; //dynamic threshold  
};




/*
 * Effect output
 */
void
Sustainer::out (float * smpsl, float * smpsr)
{
  int i;
  
   for (i = 0; i<PERIOD; i++)    //apply compression to auxresampled
   {
   auxtemp = input * smpsl[i];
   if(fabs(auxtemp > compeak)) compeak = fabs(auxtemp);   //First do peak detection on the signal
   compeak *= prls;
   compenv = cbeta * oldcompenv + calpha * compeak;       //Next average into envelope follower
   oldcompenv = compenv;
   
   if(compenv > cpthresh)                                //if envelope of signal exceeds thresh, then compress
   {
   compg = cpthresh + cpthresh*(compenv - cpthresh)/compenv; 
   cpthresh = cthresh + cratio*(compg - cpthresh);   //cpthresh changes dynamically
   tmpgain = compg/compenv;
   }
   else
   {
   tmpgain = 1.0f;
   }
   
   if(compenv < cpthresh) cpthresh = compenv;
   if(cpthresh < cthresh) cpthresh = cthresh;
   
   smpsl[i] = auxtemp * tmpgain * level;   
   };
      //End compression
};


/*
 * Parameter control
 */


void
Sustainer::setpreset (int npreset)
{
  const int PRESET_SIZE = 2;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Sus1
    {67, 64},
    //Sus2
    {67, 64},
    //Sus3
    {67, 75},

  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Sustainer::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      Pvolume = value;
      level = (float) Pvolume/127.0f;
      break;
    case 1:
      Psustain = value;
      fsustain =  (float) Psustain/127.0f;
      cratio = 1.25f - fsustain;
      input = 1.0f + fsustain * 30.0f;
      cthresh = 0.25 + fsustain;
      break;

    };
};

int
Sustainer::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Psustain);
      break;
    };
  return (0);			//in case of bogus parameter number
};
