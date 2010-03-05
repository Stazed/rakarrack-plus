/*
  ZynAddSubFX - a software synthesizer
 
  Looper.C - Looper effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Looper effect by Transmogrifox
  
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
#include "Looper.h"

Looper::Looper (REALTYPE * efxoutl_, REALTYPE * efxoutr_, float size)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  //default values
      Pclear = 1;
      Pplay = 0;
      Precord = 0;
      Pstop = 1;
      PT1 = 1;
      PT2 = 0;
      
  Srate_Attack_Coeff = 1.0f / ((float)SAMPLE_RATE * ATTACK);
  maxx_delay = lrintf((float)SAMPLE_RATE * size);
  fade = (int) SAMPLE_RATE / 2;    //1/2 SR fade time available

  ldelay = new REALTYPE[maxx_delay];  
  rdelay = new REALTYPE[maxx_delay];
  t2ldelay = new REALTYPE[maxx_delay];  
  t2rdelay = new REALTYPE[maxx_delay];
    
  setpreset (Ppreset);
  cleanup ();
};

Looper::~Looper ()
{
  delete[]ldelay;
  delete[]rdelay;
  delete[]t2ldelay;
  delete[]t2rdelay;
};

/*
 * Cleanup the effect
 */
void
Looper::cleanup ()
{
  int i;
  for (i = 0; i < maxx_delay; i++)
  {
    ldelay[i] = 0.0f;
    rdelay[i] = 0.0f;
    t2ldelay[i] = 0.0f;
    t2rdelay[i] = 0.0f;
    }
    
};


/*
 * Initialize the delays
 */
void
Looper::initdelays ()
{
  kl = 0;

  dl = maxx_delay;

  rvkl = dl - 1;

  Srate_Attack_Coeff = 45.0f / dl;   // Set swell time
  cleanup ();
};

/*
 * Effect output
 */
void
Looper::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;
  REALTYPE rswell, lswell;

  for (i = 0; i < PERIOD; i++)
    {
    
    if(Precord)
     {
     if(PT1)
     {
      ldelay[kl] += smpsl[i];
      rdelay[kl] += smpsr[i];
      }
     if(PT2)
     {
      t2ldelay[kl] += smpsl[i];
      t2rdelay[kl] += smpsr[i];
      }      
      
      }
    
    if(!Pclear)
    {
    if (++kl >= dl)
    kl = 0;
    rvkl = dl - 1 - kl;  
    } 
      
      if(Pplay)
      {
	      if(Preverse)
	      {

	      lswell =	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;
		      if (lswell <= PI) 
		      {
		      lswell = 0.5f * (1.0f - cosf(lswell));  //Clickless transition
		      efxoutl[i] = ldelay[rvkl] * lswell;   //Volume ducking near zero crossing.     
		      }  
		      else
		      {
		      efxoutl[i] = ldelay[rvkl];        
		      }
       
	      rswell = 	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;  
		      if (rswell <= PI)
		      {
		       rswell = 0.5f * (1.0f - cosf(rswell));   //Clickless transition 
		       efxoutr[i] = rdelay[rvkl] * rswell;  //Volume ducking near zero crossing.
		      }
		      else
		      {
		      efxoutr[i] = rdelay[rvkl];
		      }
      
	      }
	      else
	      {
	      efxoutl[i]= ldelay[kl];
	      efxoutr[i]= rdelay[kl];
	      }
      
      }
      else
      {
	      efxoutl[i]= 0.0f;
	      efxoutr[i]= 0.0f;      
      }
          
 
      
    };

};


/*
 * Parameter control
 */


void
Looper::setpreset (int npreset)
{
  const int PRESET_SIZE = 9;
  const int NUM_PRESETS = 2;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Looper 2 seconds
    {64, 0, 1, 0, 1, 0, 1000, 1, 0},
    {64, 0, 1, 0, 1, 1, 1000, 1, 0} 
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Looper::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
    Pvolume = value;
    volume = outvolume = (float)Pvolume / 127.0f;
//    if (Pvolume == 0)
//    cleanup ();
      break;
    case 1:	//Play at current pointer position
    if(Pplay)
    {
    Pplay = 0;
    }
    else
    {
      Pplay = 1;
    }
    if(Pstop)
    {
    Pstop = 0;
    kl = 0;
    }
    Pclear=0;
      break;
    case 2:	//stop and reset pointer to 0
      Pstop = 1;
      Precord = 0;
      Pplay = 0;
      kl = 0;
      break;
    case 3:		//Record at current position.  If first time (clear = true), then set end of loop, "dl"
      if(Precord)
      {
      if(first_time)
        {
	   dl = kl;
	   first_time = 0;
	}  
      Precord = 0;
      }
      else
      {
      Precord = 1;
      }
      Pstop = 0;
      Pclear = 0;
      break;
    case 4:
      Pclear = 1;    //Clear everything and erase the loop
      first_time = 1;
      Pplay = 0;
      Precord = 0;
      Pstop = 1;
      initdelays ();
      break;
    case 5:
      Preverse = value;		//Playback in reverse
      break;
    case 6:
      Pfade = value;
      upfade = (float) Pfade/
      break;
    case 7:
    if(PT2) {
     PT2 = 0;
     }
     else 
     {
     PT2 = 1;
     }
      break;
    case 8:
    if(PT2) {
     PT2 = 0;
     }
     else 
     {
     PT2 = 1;
     }
      break;  
      
    };
    
};

int
Looper::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Pplay);
      break;
    case 2:
      return (Pstop);
      break;
    case 3:
      return (Precord);
      break;
    case 4:
      return (Pclear);
      break;
    case 5:
      return (Preverse);
      break;
    case 6:
      return (Pfade);
      break;
    case 7:
      return (PT1);
      break;
    case 8:
      return (PT2);    
      break;
    };
  return (0);			//in case of bogus parameter number
};

void Looper::getstate ()
{
      progstate[0] = Pplay;
      progstate[1] = Pstop;
      progstate[2] = Precord;
      progstate[3] = Pclear;
      progstate[4] = PT1;
      progstate[5] = PT2;
};