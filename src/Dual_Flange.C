/*
  Rakarrack Audio FX
 
  Dual_Flange.C - Super Flanger
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing

  Higher intensity flanging accomplished by picking two points out of the delay line to create a wider notch filter.
    
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
#include "Dual_Flange.h"

Dflange::Dflange (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;
  
  period_const = (float) 1.0f/PERIOD;
  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ppanning = 64;
  Pdelay = 60;
  Plrdelay = 100;
  Plrcross = 100;
  Pfb = 40;
  Phidamp = 60;

  ldelay = NULL;
  rdelay = NULL;
  lrdelay = 0;
  
  maxx_delay = SAMPLE_RATE * MAX_DELAY;
  ldelay = new REALTYPE[maxx_delay];  
  rdelay = new REALTYPE[maxx_delay];
  
  setpreset (Ppreset);
  cleanup ();
};

Dflange::~Dflange ()
{
  delete[]ldelay;
  delete[]rdelay;
};

/*
 * Cleanup the effect
 */
void
Dflange::cleanup ()
{
  int i;
  for (i = 0; i < maxx_delay; i++)
    ldelay[i] = 0.0;
  for (i = 0; i < maxx_delay; i++)
    rdelay[i] = 0.0;
  oldl = 0.0;
  oldr = 0.0;
  
  //loop variables
  l = 0.0f;
  r = 0.0f;
  ldl = 0.0f;
  rdl = 0.0f;
  rflange0 = 0.0f;
  lflange0 = 0.0f;
  rflange1 = 0.0f;
  lflange1 = 0.0f;
  
};


/*
 * Initialize the delays
 */
void
Dflange::initdelays ()
{
  kl = 0;
  kr = 0;

  dl = delay;
  if (dl < 1)
    dl = 1;
  dr = delay;
  if (dr < 1)
    dr = 1;

//Make sure calling program cannot request longer delay than buffer:
 if (dl > maxx_delay) dl = maxx_delay - 1;
 if (dr > maxx_delay) dr = maxx_delay - 1;
 
 fdl = (float) dl;
 fdr = (float) dr;
 

  cleanup ();
};

/*
 * Effect output
 */
void
Dflange::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;

  //deal with LFO's
    REALTYPE lfol, lfor, lmod, rmod, lmodfreq, rmodfreq, rx0, rx1, lx0, lx1;
    REALTYPE ldif0, ldif1, rdif0, rdif1;  //Difference between fractional delay and floor(fractional delay)
    REALTYPE drA, drB, dlA, dlB;	//LFO inside the loop.
    REALTYPE rsA, rsB, lsA, lsB;	//Audio sample at given delay
    int tmp0, tmp1;
    
  lfo.effectlfoout (&lfol, &lfor);
  lmod = lfol;
  rmod = lfor;
  lmod = (powf (2.0f, lmod*LOG_FMAX) - 1.0f) * LFO_CONSTANT;  //2^x type sweep for musical interpretation of moving delay line.
  rmod = (powf (2.0f, rmod*LOG_FMAX) - 1.0f) * LFO_CONSTANT;


  lmodfreq = fdepth + lmod * width;				//sets frequency of lowest notch. // 20 <= fdepth <= 4000 // 20 <= width <= 16000 //
  rmodfreq = fdepth + rmod * width;

  if (lmodfreq > 20000.0f)
    lmodfreq = 20000.0f;
  else if (lmodfreq < 20.0f)
    lmodfreq = 20.0f;
  if (rmodfreq > 20000.0)
    rmodfreq = 20000.0f;
  else if (rmodfreq < 20.0f)
    rmodfreq = 20.0f;

 rflange0 = SAMPLE_RATE * 0.5f/rmodfreq;		//Turn the notch frequency into a number for delay
 rflange1 = foffset * rflange0;				//Set relationship of second delay line
 lflange0 = SAMPLE_RATE * 0.5f/lmodfreq;
 lflange1 = foffset * rflange0;
 
 //now is a delay expressed in number of samples.  Number here
 //will be fractional, but will use linear interpolation inside the loop to make a decent guess at 
 //the numbers between samples.
 
 rx0 = (rflange0 - oldrflange0) * period_const;  //amount to add each time around the loop.  Less processing of linear LFO interp inside the loop.
 rx1 =  (rflange1 - oldrflange1) * period_const;
 lx0 = (lflange0 - oldlflange0) * period_const;
 lx1  = (lflange1 - oldlflange1) * period_const; 
 // Now there is a fractional amount to add 
 
 drA = oldrflange0;
 drB = oldrflange1;
 dlA = oldlflange0;
 dlB = oldlflange1;
  // dr, dl variables are the LFO inside the loop.
  
  //lfo ready...


  for (i = 0; i < PERIOD; i++)
    {
    
    //Delay line utility
      ldl = ldelay[kl];
      rdl = rdelay[kr];
      l = ldl * (1.0f - lrcross) + rdl * lrcross;
      r = rdl * (1.0f - lrcross) + ldl * lrcross;
      ldl = l;
      rdl = r;
      ldl = smpsl[i] * panning - ldl * fb;
      rdl = smpsr[i] * (1.0f - panning) - rdl * fb;
      
      
      //LowPass Filter
      ldelay[kl] = ldl = ldl * hidamp + oldl * (1.0f - hidamp);
      rdelay[kr] = rdl = rdl * hidamp + oldr * (1.0f - hidamp);
      oldl = ldl + DENORMAL_GUARD;
      oldr = rdl + DENORMAL_GUARD;

	//End delay line management, start flanging:
	
	//Right Channel, delay A
	rdif0 = drA - floor(drA);
	tmp0 = (kr + (int) floor(drA)) % dr;
	tmp1 = tmp0 + 1;
	if (tmp1 < 0) tmp1 = dr;	
	rsA = rdelay[tmp0] + rdif0 * rdelay[tmp1]	//here is the first right channel delay
	
	//Right Channel, delay B	
	rdif1 = drB - floor(drB);
	tmp0 = (kr + (int) floor(drB)) % dr;
	tmp1 = tmp0 + 1;
	if (tmp1 < 0) tmp1 = dr;
	rsB = rdelay[tmp0] + rdif1 * rdelay[tmp1]	//here is the second right channel delay	
	
	//Left Channel, delay A
	ldif0 = dlA - floor(dlA);
	tmp0 = (kl + (int) floor(dlA)) % dl;
	tmp1 = tmp0 + 1;
	if (tmp1 < 0) tmp1 = dl;
	lsA = ldelay[tmp0] + ldif0 * ldelay[tmp1]	//here is the first left channel delay
	
	//Left Channel, delay B	
	ldif1 = drB - floor(drB);
	tmp0 = (kl + (int) floor(dlB)) % dl;
	tmp1 = tmp0 + 1;
	if (tmp1 < 0) tmp1 = dl;
	lsB = ldelay[tmp0] + ldif1 * ldelay[tmp1]	//here is the second leftt channel delay
		
	//End flanging, next process outputs


      efxoutl[i]= dry * smpsl[i] + 0.5f * wet * (lsA + lsB);    // Make final FX out mix
      efxoutr[i]= dry * smpsr[i] + 0.5f * wet * (rsA + rsB);
     


      
      if (--kl < 0)   //Cycle delay buffer in reverse so delay time can be indexed directly with addition (above)
	kl = dl;
      if (--kr < 0)
	kr = dr;

// Increment LFO
 drA += rx0;
 drB += rx1;
 dlA += lx0;
 dlB += lx1;
 
    };
    
    oldrflange0 =  rflange0; 
    oldrflange1 =  rflange1;
    oldlflange0 =  lflange0;
    oldlflange1 =  lflange1;

};


/*
 * Parameter control
 */

void
Dflange::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
      setdelay (value);
      break;
    case 3:
      setlrdelay (value);
      break;
    case 4:
      setlrcross (value);
      break;
    case 5:
      setfb (value);
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      setreverse (value);
      break;
    };
};

int
Dflange::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppanning);
      break;
    case 2:
      return (Pdelay);
      break;
    case 3:
      return (Plrdelay);
      break;
    case 4:
      return (Plrcross);
      break;
    case 5:
      return (Pfb);
      break;
    case 6:
      return (Phidamp);
      break;
    case 7:
      return (Preverse);
      break;
    };
  return (0);			//in case of bogus parameter number
};


void
Dflange::setpreset (int npreset)
{
  const int PRESET_SIZE = 8;
  const int NUM_PRESETS = 9;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Dflange 1
    {67, 64, 35, 64, 30, 59, 0, 127},
    //Dflange 2
    {67, 64, 21, 64, 30, 59, 0, 64},
    //Dflange 3
    {67, 75, 60, 64, 30, 59, 10, 0},
    //Simple Dflange
    {67, 60, 44, 64, 30, 0, 0, 0},
    //Canyon
    {67, 60, 102, 50, 30, 82, 48, 0},
    //Panning Dflange 1
    {67, 64, 44, 17, 0, 82, 24, 0},
    //Panning Dflange 2
    {81, 60, 46, 118, 100, 68, 18, 0},
    //Panning Dflange 3
    {81, 60, 26, 100, 127, 67, 36, 0},
    //Feedback Dflange
    {62, 64, 28, 64, 100, 90, 55, 0}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};
