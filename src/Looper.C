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

Looper::Looper (float * efxoutl_, float * efxoutr_, float size)
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
      Pautoplay = 0;
      rplaystate = 0;
      first_time1 = 1;
      first_time2 = 1;
      
  Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
  maxx_delay = lrintf(fSAMPLE_RATE * size);
  fade = (int) SAMPLE_RATE / 2;    //1/2 SR fade time available

  ldelay = new float[maxx_delay];  
  rdelay = new float[maxx_delay];
  t2ldelay = new float[maxx_delay];  
  t2rdelay = new float[maxx_delay];
    
  setpreset (Ppreset);
  cleanup ();
};

Looper::~Looper ()
{
};

/*
 * Cleanup the effect
 */
void
Looper::cleanuppt1 ()
{
  int i;
  for (i = 0; i < maxx_delay; i++)
  {
    ldelay[i] = 0.0f;
    rdelay[i] = 0.0f;
    }
    
};
void
Looper::cleanuppt2 ()
{
  int i;
  for (i = 0; i < maxx_delay; i++)
  {
    t2ldelay[i] = 0.0f;
    t2rdelay[i] = 0.0f;
    }
    
};

void Looper::cleanup ()
{
  cleanuppt1 ();
  cleanuppt2 (); 
    
};
/*
 * Initialize the delays
 */
void
Looper::initdelays ()
{

if(PT1)
{
  kl = 0;
  dl = maxx_delay;
  rvkl = dl - 1;
  cleanuppt1 ();
  }

if(PT2)
{
  kl2 = 0;
  dl2 = maxx_delay;
  rvkl2 = dl2 - 1;
  cleanuppt2 ();
  }
  Srate_Attack_Coeff = 90.0f / (dl + dl2);   // Set swell time

};

/*
 * Effect output
 */
void
Looper::out (float * smpsl, float * smpsr)
{
  int i;
  float rswell, lswell;
  float ticktock[PERIOD];
  ticker.metronomeout(ticktock);

  for (i = 0; i < PERIOD; i++)
    {
    
    if(Precord)
     {
     
	     if((Prec1) && (PT1))
	     {
	      ldelay[kl] += pregain1*smpsl[i];
	      rdelay[kl] += pregain1*smpsr[i];
	      }
	     if((Prec2) && (PT2))
	     {
	      t2ldelay[kl2] += pregain2*smpsl[i];
	      t2rdelay[kl2] += pregain2*smpsr[i];
	      }      
      
      }
    
    if(!Pclear)
    {
    if (++kl >= dl)
    kl = 0;
    rvkl = dl - 1 - kl;  
    if (++kl2 >= dl2)
    kl2 = 0;
    rvkl2 = dl2 - 1 - kl2;     
    } 
     
      
      if(Pplay)
      {
	      if(Preverse)
	      {

	      lswell =	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;
		      if (lswell <= PI) 
		      {
		      lswell = 0.5f * (1.0f - cosf(lswell));  //Clickless transition
		      efxoutl[i] = (fade1 * ldelay[rvkl] + fade2 * t2ldelay[rvkl2]) * lswell;   //Volume ducking near zero crossing.     
		      }  
		      else
		      {
		      efxoutl[i] = fade1 * ldelay[rvkl] + fade2 * t2ldelay[rvkl2];        
		      }
       
	      rswell = 	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;  
		      if (rswell <= PI)
		      {
		       rswell = 0.5f * (1.0f - cosf(rswell));   //Clickless transition 
		       efxoutr[i] = ( fade1 * rdelay[rvkl] + fade2 * t2rdelay[rvkl2] )* rswell;  //Volume ducking near zero crossing.
		      }
		      else
		      {
		      efxoutr[i] = fade1 * rdelay[rvkl] + fade2 * t2rdelay[rvkl2];
		      }
      
	      }
	      else
	      {

	      efxoutl[i]= fade1*ldelay[kl] + fade2*t2ldelay[kl2];
	      efxoutr[i]= fade1*rdelay[kl] + fade2*t2rdelay[kl2];

	      }
      
      }
      else
      {
	      efxoutl[i]= 0.0f;
	      efxoutr[i]= 0.0f;      
      }
          
 	      //efxoutl[i] += ticktock[i];  //if you want to hear the metronome in Looper
	      //efxoutr[i] += ticktock[i];  //Holborn, maybe we want to play it through from some other place...you can decide :)   
      
    };

};


/*
 * Parameter control
 */

void Looper::setfade ()
{
      fade1 = (float) Pfade1/64.0f;
      fade2 = (float) Pfade2/64.0f;
      pregain1 = 1.0f/fade1;
      pregain2 = 1.0f/fade2;      //this is so material being recorded mixes at the same level as what you hear
      fade1 *= track1gain;
      fade2 *= track2gain;
};
void
Looper::setpreset (int npreset)
{
  const int PRESET_SIZE = 14;
  const int NUM_PRESETS = 2;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Looper 2 seconds
    {64, 0, 1, 0, 1, 0, 64, 1, 0, 1, 64, 1, 0, 0},
    {64, 0, 1, 0, 1, 1, 64, 1, 0, 1, 64, 1, 0, 0} 
  };

  if(npreset>NUM_PRESETS-1)  
    {   
     Fpre->ReadPreset(30,npreset-NUM_PRESETS+1);    
     for (int n = 0; n < PRESET_SIZE; n++)    
     changepar (n, pdata[n]);    
    }    
  else                                      
  {     

  for (int n = 0; n < PRESET_SIZE; n++)
  loadpreset (n, presets[npreset][n]);
  }
  Ppreset = npreset;
};


void
Looper::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
    Pvolume = value;
    outvolume = (float)Pvolume / 127.0f;

      break;
    case 1:	//Play at current pointer position
    if(Pplay)
    {
    Pplay = 0;
    }
    else
    {
      Pplay = 1;
      Pstop = 0;
    }
    if(Pstop)
    {
    Pstop = 0;
    Pplay = 0;
    kl = 0;
    }
    Pclear=0;
      break;
    case 2:	//stop and reset pointer to 0
      Pstop = 1;
      if(Precord)
      {
      if((first_time1) && (Prec1))
        {
	   dl = kl;
	   first_time1 = 0;
	   if(Plink)
	   {
	   dl2 = dl;
	   }
	}  
      if((first_time2) && (Prec1))
      {
           dl2 = kl2;
	   first_time2 = 0;
	   if(Plink)
	   {
	   dl = dl2;
	   }
      }
      }
      Precord = 0;
      Pplay = 0;
      kl = 0;
      break;
    case 3:		//Record at current position.  If first time (clear = true), then set end of loop, "dl"
      if(Precord)
      {
      if((first_time1) && (Prec1))
        {
	   dl = kl;
	   first_time1 = 0;
	   if(Plink)
	   {
	   dl2 = dl;
	   }
	}  
      if((first_time2) && (Prec1))
      {
           dl2 = kl2;
	   first_time2 = 0;
	   if(Plink)
	   {
	   dl = dl2;
	   }
      }
      Precord = 0;
      Pplay = rplaystate;
      if(Pautoplay) Pplay = 1;
      Pstop = 0;
      }
      else
      {
      Precord = 1;
      rplaystate = 0;
      Pplay = 0;
      }
      Pstop = 0;
      Pclear = 0;
      break;
    case 4:
      Pclear = 1;    //Clear everything and erase the loop
      if(PT1) first_time1 = 1;
      if(PT2) first_time2 = 1;
      if((PT1) && (PT2)) Pplay = 0;
      Precord = 0;
      initdelays ();
      break;
    case 5:
      Preverse = value;		//Playback in reverse
      break;
    case 6:
      Pfade1 = value;
      setfade ();
      break;
    case 7:
    if(PT1) {
     PT1 = 0;
     }
     else 
     {
     PT1 = 1;
     }
     track1gain = (float) PT1;
     setfade ();
      break;
    case 8:
    if(PT2) {
     PT2 = 0;
     }
     else 
     {
     PT2 = 1;
     }
     track2gain = (float) PT2;
     setfade ();
      break;      
    case 9:
    Pautoplay = value;
     break;    
    case 10:
    Pfade2 = value;
    setfade();
     break;
     case 11:
     Prec1 = value;
     break;
     case 12:
     Prec2 = value;
     break;
     case 13:
     Plink = value;
     dl2 = dl;    
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
      return (Pfade1);
      break;
    case 7:
      return (PT1);
      break;
    case 8:
      return (PT2);    
      break;
    case 9:
      return (Pautoplay);
      break;
    case 10:
      return (Pfade2);
      break;
    case 11:
      return (Prec1);
      break;
    case 12:
      return (Prec2);
      break;
    case 13:
      return(Plink);
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

void
Looper::loadpreset (int npar, int value)
{
  switch (npar)
    {
    case 0:
    Pvolume = value;
    outvolume = (float)Pvolume / 127.0f;

      break;
    case 1:	//Play at current pointer position
    Pplay = value;
      break;
    case 2:	//stop and reset pointer to 0
      Pstop = value;
      break;
    case 3:		//Record at current position.  If first time (clear = true), then set end of loop, "dl"
      Precord = value;
      break;
    case 4:
      Pclear = 1;    //Clear everything and erase the loop
       initdelays ();
      break;
    case 5:
      Preverse = value;		//Playback in reverse
      break;
    case 6:
      Pfade1 = value;
      setfade ();
      break;
    case 7:
     PT1 = value;
     track1gain = (float) PT1;
     setfade ();
      break;
    case 8:
     PT2 = value;
     track2gain = (float) PT2;
     setfade ();
      break;      
    case 9:
    Pautoplay = value;
     break;    
    case 10:
    Pfade2 = value;
    setfade();
     break;
     case 11:
     Prec1 = value;
     break;
     case 12:
     Prec2 = value;
     break;
     case 13:
     Plink = value;
     break;
      


    };

      Pclear = 1;    //Clear everything and erase the loop
      if(PT1) first_time1 = 1;
      if(PT2) first_time2 = 1;
      if((PT1) && (PT2)) Pplay = 0;
      Precord = 0;
    getstate();
};

