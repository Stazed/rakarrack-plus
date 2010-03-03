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

Looper::Looper (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  //default values
      Pclear = 1;
      Pplay = 0;
      Precord = 0;
      Pstop = 1;
      
  Srate_Attack_Coeff = 1.0f / ((float)SAMPLE_RATE * ATTACK);
  maxx_delay = SAMPLE_RATE * MAX_LOOP_DELAY;
  fade = (int) SAMPLE_RATE / 2;    //1/2 SR fade time available

  ldelay = new REALTYPE[maxx_delay];  
  rdelay = new REALTYPE[maxx_delay];
  
  setpreset (Ppreset);
  cleanup ();
};

Looper::~Looper ()
{
  delete[]ldelay;
  delete[]rdelay;
};

/*
 * Cleanup the effect
 */
void
Looper::cleanup ()
{
  int i;
  for (i = 0; i < maxx_delay; i++)
    ldelay[i] = 0.0f;
  for (i = 0; i < maxx_delay; i++)
    rdelay[i] = 0.0f;
    
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
      ldelay[kl] += smpsl[i];
      rdelay[kl] += smpsr[i];
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
  const int PRESET_SIZE = 7;
  const int NUM_PRESETS = 2;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Looper 2 seconds
    {64, 0, 0, 0, 1, 0, 1000},
    {64, 0, 0, 0, 1, 1, 1000} 
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
    printf("Play was true:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl);
    }
    else
    {
      Pplay = 1;
         printf("Play was false:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl); 
    }
    if(Pstop)
    {
    Pstop = 0;
    kl = 0;
    printf("Play mode:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, kl, dl);
    }
    Pclear=0;
    printf("Play mode:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, kl, dl);
      break;
    case 2:	//stop and reset pointer to 0
      Pstop = 1;
      Precord = 0;
      Pplay = 0;
      kl = 0;
     printf("Stop Mode:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl);     
      break;
    case 3:		//Record at current position.  If first time (clear = true), then set end of loop, "dl"
      if(Precord)
      {
      if(first_time)
        {
	   dl = kl;
	   first_time = 0;
	   printf("End Record:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl); 
	}  
      Precord = 0;
      }
      else
      {
      Precord = 1;
      printf("Start Record:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl);
      }
      Pstop = 0;
      Pclear = 0;
     printf("Record Mode:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl); 
      break;
    case 4:
      Pclear = 1;    //Clear everything and erase the loop
      first_time = 1;
      Pplay = 0;
      Precord = 0;
      Pstop = 1;
      initdelays ();
      printf("Clear mode:  play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl); 
      break;
    case 5:
      Preverse = value;		//Playback in reverse
     printf("Reverse Mode...play %d stop %d record %d clear %d dl %d kl %d\n", Pplay, Pstop, Precord, Pclear, dl, kl); 
      break;
    case 6:
      Pfade = value;
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
    };
  return (0);			//in case of bogus parameter number
};
