/*
  ZynAddSubFX - a software synthesizer
 
  Echo.C - Echo effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Echo effect by Transmogrifox
  
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
#include "Echo.h"

Echo::Echo (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
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
  
  setpreset (Ppreset);
  cleanup ();
};

Echo::~Echo ()
{
  delete[]ldelay;
  delete[]rdelay;
};

/*
 * Cleanup the effect
 */
void
Echo::cleanup ()
{
  int i;
  for (i = 0; i < dl; i++)
    ldelay[i] = 0.0;
  for (i = 0; i < dr; i++)
    rdelay[i] = 0.0;
  oldl = 0.0;
  oldr = 0.0;
};


/*
 * Initialize the delays
 */
void
Echo::initdelays ()
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

  if (ldelay != NULL)
    delete[]ldelay;
  if (rdelay != NULL)
    delete[]rdelay;
  ldelay = new REALTYPE[dl];
  rdelay = new REALTYPE[dr];

  cleanup ();
};

/*
 * Effect output
 */
void
Echo::out (REALTYPE * smpsl, REALTYPE * smpsr)
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


      lswell =	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;
      rswell = 	(float)(abs(kr - rvkr)) * Srate_Attack_Coeff;
      
      if (lswell > 1.0f)  lswell = 1.0f;
      if (rswell > 1.0f)  rswell = 1.0f;
      
      //lswell = dB2rap(-60.0 * (1.0 - lswell));  //Log swell
      //rswell = dB2rap(-60.0 * (1.0 - rswell));
      

      //This is what you hear:
      efxoutl[i] = 2.0f * ((ldelay[rvkl] * reverse * lswell*lswell*lswell*lswell)  + (ldl * (1-reverse)));   //x^4 exponential sounds about the best w/o using e^x
      efxoutr[i] = 2.0f * ((rdelay[rvkr] * reverse * rswell*rswell*rswell*rswell)  + (rdl * (1-reverse)));

      //LowPass Filter
      ldelay[kl] = ldl = ldl * hidamp + oldl * (1.0f - hidamp);
      rdelay[kr] = rdl = rdl * hidamp + oldr * (1.0f - hidamp);
      oldl = ldl + DENORMAL_GUARD;
      oldr = rdl + DENORMAL_GUARD;
      oldl -=  DENORMAL_GUARD;  // These are likely to resolve to denormals in a short time period.
      oldr -=  DENORMAL_GUARD;
      
      if (++kl >= dl)
	kl = 0;
      if (++kr >= dr)
	kr = 0;
      rvkl = dl - 1 - kl;
      rvkr = dr - 1 - kr;
    };

};


/*
 * Parameter control
 */
void
Echo::setvolume (unsigned char Pvolume)
{
  this->Pvolume = Pvolume;
  volume = outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Echo::setpanning (unsigned char Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};

void
Echo::setreverse (unsigned char Preverse)
{
  this->Preverse = Preverse;
  reverse = (float) Preverse / 127.0f;
};

void
Echo::setdelay (unsigned char Pdelay)
{
  this->Pdelay = Pdelay;
  delay = 1 + lrintf ((float)Pdelay / 127.0f * (float)SAMPLE_RATE * 2.0f);	//0 .. 1.5 sec

  initdelays ();
};

void
Echo::setlrdelay (unsigned char Plrdelay)
{
  REALTYPE tmp;
  this->Plrdelay = Plrdelay;
  tmp =
    (powf (2.0, fabsf ((float)Plrdelay - 64.0f) / 64.0f * 9.0f) -
     1.0f) / 1000.0f * (float)SAMPLE_RATE;
  if (Plrdelay < 64.0)
    tmp = -tmp;
  lrdelay = lrintf(tmp);
  initdelays ();
};

void
Echo::setlrcross (unsigned char Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 127.0f * 1.0f;
};

void
Echo::setfb (unsigned char Pfb)
{
  this->Pfb = Pfb;
  fb = (float)Pfb / 128.0f;
};

void
Echo::sethidamp (unsigned char Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.0f;
};

void
Echo::setpreset (unsigned char npreset)
{
  const int PRESET_SIZE = 8;
  const int NUM_PRESETS = 9;
  unsigned char presets[NUM_PRESETS][PRESET_SIZE] = {
    //Echo 1
    {67, 64, 35, 64, 30, 59, 0, 127},
    //Echo 2
    {67, 64, 21, 64, 30, 59, 0, 64},
    //Echo 3
    {67, 75, 60, 64, 30, 59, 10, 0},
    //Simple Echo
    {67, 60, 44, 64, 30, 0, 0, 0},
    //Canyon
    {67, 60, 102, 50, 30, 82, 48, 0},
    //Panning Echo 1
    {67, 64, 44, 17, 0, 82, 24, 0},
    //Panning Echo 2
    {81, 60, 46, 118, 100, 68, 18, 0},
    //Panning Echo 3
    {81, 60, 26, 100, 127, 67, 36, 0},
    //Feedback Echo
    {62, 64, 28, 64, 100, 90, 55, 0}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Echo::changepar (int npar, unsigned char value)
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

unsigned char
Echo::getpar (int npar)
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
