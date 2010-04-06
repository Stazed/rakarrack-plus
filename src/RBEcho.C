/*
  ZynAddSubFX - a software synthesizer
 
  RBEcho.C - Echo effect
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
#include "RBEcho.h"

RBEcho::RBEcho (float * efxoutl_, float * efxoutr_)
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
  Psubdiv = 1;
  subdiv = 1.0f;

  ldelay = NULL;
  rdelay = NULL;
  lrdelay = 0;
  Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
  maxx_delay = SAMPLE_RATE * MAX_DELAY;

  ldelay = new float[maxx_delay];  
  rdelay = new float[maxx_delay];
  
  setpreset (Ppreset);
  cleanup ();
};

RBEcho::~RBEcho ()
{
  delete[]ldelay;
  delete[]rdelay;
};

/*
 * Cleanup the effect
 */
void
RBEcho::cleanup ()
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
RBEcho::initdelays ()
{
  int i;

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
  Srate_Attack_Coeff = 15.0f * cSAMPLE_RATE;   // Set swell time to 66ms of average delay time 

  for (i = dl; i < maxx_delay; i++)
    ldelay[i] = 0.0;
  for (i = dr; i < maxx_delay; i++)
    rdelay[i] = 0.0;

  oldl = 0.0;
  oldr = 0.0;


};

/*
 * Effect output
 */
void
RBEcho::out (float * smpsl, float * smpsr)
{
  int i;
  float ldl, rdl, rswell, lswell;
  float avg, ldiff, rdiff, tmp;
  

  for (i = 0; i < PERIOD; i++)
    {
      ldl = ldelay[kl];
      rdl = rdelay[kr];
        
      //LowPass Filter
      ldelay[kl] = ldl * hidamp + oldl * (1.0f - hidamp);
      rdelay[kr] = rdl * hidamp + oldr * (1.0f - hidamp);
      oldl = ldl + DENORMAL_GUARD;
      oldr = rdl + DENORMAL_GUARD;
     
      if(Pes)
      {
          smpsl[i] *= cosf(lrcross);
          smpsr[i] *= sinf(lrcross);
          
       	  avg = (smpsl[i] + smpsr[i]) * .5f;
	  ldiff = smpsl[i] - avg;
	  rdiff = smpsr[i] - avg;

	  tmp = avg + ldiff * pes;
	  smpsl[i] = tmp;

	  tmp = avg + rdiff * pes;
	  smpsr[i] = tmp;

      }

      ldelay[kl] = smpsl[i] * panning - ldl * fb;
      rdelay[kr] = smpsr[i] * (1.0f - panning) - rdl * fb;
      
      if (++kl >= dl)
	kl = 0;
      if (++kr >= dr)
	kr = 0;
      rvkl = dl - 1 - kl;
      rvkr = dr - 1 - kr;

          
      if(reverse > 0.0f)
      {

      lswell =	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;
	      if (lswell <= PI) 
	      {
	      lswell = (1.0f - cosf(lswell));  //Clickless transition
	      efxoutl[i] = reverse * (ldelay[rvkl] * lswell)  + (ldelay[kl] * (1.0f - reverse));   //Volume ducking near zero crossing.     
	      }  
	      else
	      {
	      efxoutl[i] = 2.0f * ((ldelay[rvkl] * reverse)  + (ldelay[kl] * (1.0f - reverse)));        
	      }
       
      rswell = 	(float)(abs(kr - rvkr)) * Srate_Attack_Coeff;  
	      if (rswell <= PI)
	      {
	       rswell = (1.0f - cosf(rswell));   //Clickless transition 
	       efxoutr[i] = reverse * (rdelay[rvkr] * rswell)  + (rdelay[kr] * (1.0f - reverse));  //Volume ducking near zero crossing.
	      }
	      else
	      {
	      efxoutr[i] = 2.0f * ((rdelay[rvkr] * reverse)  + (rdelay[kr] * (1.0f - reverse)));
	      }
      

      }
      else
      {
      efxoutl[i]= ldelay[kl];
      efxoutr[i]= rdelay[kr];
      }      
            
    };

};


/*
 * Parameter control
 */
void
RBEcho::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;

};

void
RBEcho::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};

void
RBEcho::setreverse (int Preverse)
{
  this->Preverse = Preverse;
  reverse = (float) Preverse / 127.0f;
};

void
RBEcho::setdelay (int Pdelay)
{
  this->Pdelay = Pdelay;
  fdelay= 60.0f/((float) Pdelay);
  if (fdelay < 0.01f) fdelay = 0.01f;
  if (fdelay > (float) MAX_DELAY) fdelay = (float) MAX_DELAY;  //Constrains 10ms ... MAX_DELAY
  delay = 1 + lrintf ( subdiv * fdelay * fSAMPLE_RATE );	
  initdelays ();
};

void
RBEcho::setlrdelay (int Plrdelay)
{
  float tmp;
  this->Plrdelay = Plrdelay;
  tmp =
    (powf (2.0, fabsf ((float)Plrdelay - 64.0f) / 64.0f * 9.0f) -
     1.0f) / 1000.0f * fSAMPLE_RATE;
  if (Plrdelay < 64.0)
    tmp = -tmp;
  lrdelay = lrintf(tmp);
  initdelays ();
};

void
RBEcho::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 128.0f;
  
};

void
RBEcho::setfb (int Pfb)
{
  this->Pfb = Pfb;
  fb = (float)Pfb / 128.0f;
};

void
RBEcho::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.0f;
};

void
RBEcho::setpreset (int npreset)
{
  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 9;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Echo 1
    {67, 64, 565, 64, 30, 59, 0, 127, 0, 0},
    //Echo 2
    {67, 64, 357, 64, 30, 59, 0, 64, 0 ,0 },
    //Echo 3
    {67, 75, 955, 64, 30, 59, 10, 0, 0 ,0 },
    //Simple Echo
    {67, 60, 705, 64, 30, 0, 0, 0, 0 ,0},
    //Canyon
    {67, 60, 1610, 50, 30, 82, 48, 0, 0 ,0},
    //Panning Echo 1
    {67, 64, 705, 17, 0, 82, 24, 0, 0 ,0},
    //Panning Echo 2
    {81, 60, 737, 118, 100, 68, 18, 0, 0 ,0},
    //Panning Echo 3
    {81, 60, 472, 100, 127, 67, 36, 0, 0 ,0},
    //Feedback Echo
    {62, 64, 456, 64, 100, 90, 55, 0, 0 ,0}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
RBEcho::changepar (int npar, int value)
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
    case 8:
      Psubdiv = value + 1;
      subdiv = 1.0f/((float)Psubdiv);
      delay = 1 + lrintf ( subdiv * fdelay * fSAMPLE_RATE );
      initdelays ();
      break;
    case 9:
      Pes = value;
      pes = 8.0f * (float)Pes / 127.0f;
      break;
    };
};

int
RBEcho::getpar (int npar)
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
    case 8:
      return (Psubdiv);
      break;
    case 9:
      return (Pes);
      break; 

    };
  return (0);			//in case of bogus parameter number
};
