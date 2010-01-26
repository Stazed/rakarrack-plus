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
  Srate_Attack_Coeff = 1.0f / ((float)SAMPLE_RATE * ATTACK);
  maxx_delay = SAMPLE_RATE * MAX_DELAY;
  fade = (int) SAMPLE_RATE / 5;    //200ms fade time available

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
};


/*
 * Initialize the delays
 */
void
Dflange::initdelays ()
{
  kl = 0;
  kr = 0;

  dl = delay - lrdelay;
  if (dl < 1)
    dl = 1;
  dr = delay + lrdelay;
  if (dr < 1)
    dr = 1;

  rvkl = dl - 1;
  rvkr = dr - 1;
  Srate_Attack_Coeff = 15.0f / (dl + dr);   // Set swell time to 1/10th of average delay time 


  cleanup ();
};

/*
 * Effect output
 */
void
Dflange::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;
  REALTYPE l, r, ldl, rdl, rswell, lswell;

  for (i = 0; i < PERIOD; i++)
    {
      ldl = ldelay[kl];
      rdl = rdelay[kr];
      l = ldl * (1.0f - lrcross) + rdl * lrcross;
      r = rdl * (1.0f - lrcross) + ldl * lrcross;
      ldl = l;
      rdl = r;


      ldl = smpsl[i] * panning - ldl * fb;
      rdl = smpsr[i] * (1.0f - panning) - rdl * fb;


      efxoutl[i]= ldl;
      efxoutr[i]= rdl;
     
      
      //LowPass Filter
      ldelay[kl] = ldl = ldl * hidamp + oldl * (1.0f - hidamp);
      rdelay[kr] = rdl = rdl * hidamp + oldr * (1.0f - hidamp);
      oldl = ldl + DENORMAL_GUARD;
      oldr = rdl + DENORMAL_GUARD;

      
      if (++kl >= dl)
	kl = 0;
      if (++kr >= dr)
	kr = 0;
      rvkl = dl - 1 - kl;
      rvkr = dr - 1 - kr;
      
      rvfl = dl + fade - kl;
      rvfr = dr + fade - kr;
    };

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
