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
  maxx_delay = 1 + SAMPLE_RATE * MAX_DELAY;

  ldelay = new float[maxx_delay];  
  rdelay = new float[maxx_delay];
  
  setpreset (Ppreset);
  cleanup ();
};

RBEcho::~RBEcho ()
{
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
  
  if(Plrdelay>0)
  {
  rkl = lrdelay;
  rkr = lrdelay/2;
  }
  else
  {
  rkr = lrdelay;
  rkl = lrdelay/2;  
  }
  if (rkl > delay)
    rkl = delay;
  if (rkr < 1)
    rkr = 1;
  if (rkr > delay)
    rkr = delay;
  if (rkl < 1)
    rkl = 1;
    
  rvkl = delay;
  rvkr = delay;
  Srate_Attack_Coeff = 15.0f * cSAMPLE_RATE;   // Set swell time to 66ms 

  for (i = delay; i < maxx_delay; i++){
    ldelay[i] = 0.0;
    rdelay[i] = 0.0;
    }

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
        
      //LowPass Filter
      ldl = lfeedback * hidamp + oldl * (1.0f - hidamp);
      rdl = rfeedback * hidamp + oldr * (1.0f - hidamp);
      oldl = ldl + DENORMAL_GUARD;
      oldr = rdl + DENORMAL_GUARD;
     
      ldelay[kl] = ldl + smpsl[i];
      rdelay[kr] = ldl + smpsr[i];
      
      if (++kl > delay)
	kl = 0;
      if (++kr > delay)
	kr = 0;
      rvkl = delay - kl;
      rvkr = delay - kr;
      
      if (++rkl > delay)
	rkl = 0;
      if (++rkr > delay)
	rkr = 0;
          
      if(reverse > 0.0f)
      {

      lswell =	((float) (kl - rvkl)) * Srate_Attack_Coeff;
      lswell = 1.0f - 1.0f/(lswell*lswell + 1.0f);
      efxoutl[i] = reverse * (ldelay[rvkl] * lswell)  + (ldelay[kl] * (1.0f - reverse));
      
      rswell = 	((float) (kr - rvkr)) * Srate_Attack_Coeff;  
      rswell = 1.0f - 1.0f/(rswell*rswell + 1.0f);
      efxoutr[i] = reverse * (rdelay[rvkr] * rswell)  + (rdelay[kr] * (1.0f - reverse));  //Volume ducking near zero crossing.

      }
      else
      {
      efxoutl[i]= ldelay[kl];
      efxoutr[i]= rdelay[kr];
      }   
                  

      lfeedback = lpanning * fb * efxoutl[i];
      rfeedback = rpanning * fb * efxoutr[i];      
      
      if(Pes)
      {
          efxoutl[i] *= cosf(lrcross);
          efxoutr[i] *= sinf(lrcross);
          
       	  avg = (efxoutl[i] + efxoutr[i]) * 0.5f;
	  ldiff = efxoutl[i] - avg;
	  rdiff = efxoutr[i] - avg;

	  tmp = avg + ldiff * pes;
	  efxoutl[i] = 0.5 * tmp;

	  tmp = avg + rdiff * pes;
	  efxoutr[i] = 0.5f * tmp;


      }   
       efxoutl[i] = (efxoutl[i] + pingpong * rdelay[rkl]) * lpanning;
       efxoutr[i] = (efxoutr[i] + pingpong * ldelay[rkr]) * rpanning;      
            
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
  lpanning = ((float)Ppanning) / 64.0f;
  rpanning = 2.0f - lpanning;
  lpanning = 10.0f * powf(lpanning, 4);
  rpanning = 10.0f * powf(rpanning, 4);
  lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
  rpanning = 1.0f - 1.0f/(rpanning + 1.0f); 
  lpanning *= 1.1f;
  rpanning *= 1.1f; 
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
    ((float) delay) * fabs(((float)Plrdelay - 64.0f) / 65.0f);
  lrdelay = lrintf(tmp);
  
  tmp = fabs( ((float) Plrdelay - 64.0f)/32.0f);
  pingpong = 1.0f - 1.0f/(5.0f*tmp*tmp + 1.0f);
  pingpong *= 1.2f;
  initdelays ();
};

void
RBEcho::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = D_PI * (float)Plrcross / 128.0f;
  
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
  hidamp = expf(-D_PI * 500.0f * ((float) Phidamp)/fSAMPLE_RATE);
};

void
RBEcho::setpreset (int npreset)
{
  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Echo 1
    {64, 64, 90, 64, 64, 64, 64, 0, 1, 96},
    //Echo 2
    {64, 64, 90, 64, 64, 64, 64, 0, 2 ,96},
    //Echo 3
    {64, 64, 90, 64, 64, 64, 64, 0, 3 ,96}
  };

  if(npreset>NUM_PRESETS-1)  
    {   
     Fpre->ReadPreset(32,npreset-NUM_PRESETS+1);    
     for (int n = 0; n < PRESET_SIZE; n++)    
     changepar (n, pdata[n]);    
    }    
  else                                      
  {     
  for (int n = 0; n < PRESET_SIZE; n++)
  changepar (n, presets[npreset][n]);
  }
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
      Psubdiv = value;
      subdiv = 1.0f/((float)(value + 1));
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
