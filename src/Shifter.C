/*
  rakarrack - a guitar effects software

  Shifter.C  -  Shifter
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

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


#include "Shifter.h"



Shifter::Shifter (float *efxoutl_, float *efxoutr_, long int Quality)
{



  efxoutl = efxoutl_;
  efxoutr = efxoutr_;
  hq = Quality;

  outi = (float *) malloc (sizeof (float) * PERIOD);
  outo = (float *) malloc (sizeof (float) * PERIOD);

  PS = new PitchShifter (2048, hq, fSAMPLE_RATE);
  PS->ratio = 1.0f;

  env = 0.0f;
  tune = 0.0f;
  Pupdown = 0;
  Pinterval = 0;
  Ppreset = 0;
  setpreset (Ppreset);


  cleanup ();

};



Shifter::~Shifter ()
{

  delete[]outi;
  delete[]outo;
  delete PS;

};

void
Shifter::cleanup ()
{
  memset(outi, 0, sizeof(float)*PERIOD);
  memset(outo, 0, sizeof(float)*PERIOD);
};



void
Shifter::out (float *smpsl, float *smpsr)
{

  int i;
  float sum;
  float use;

    for (i=0; i < PERIOD; i++)
    {
     if(Pmode == 0)
     {
     sum = fabsf(smpsl[i])+fabsf(smpsr[i]);
     if (sum>env) env = sum;  else env=sum*ENV_TR+env*(1.0f-ENV_TR);
     
     if (env <= tz_level)
     {
      state=IDLE;	
      tune = 0.0;
     } 

     if ((state == IDLE) && (env >= t_level)) state=UP;

     if (state==UP)    
        {
         tune +=a_rate;
         if (tune >=1.0f) state = WAIT;
        }
     
     if (state==WAIT)
        {
         if (env<td_level)
         state=DOWN;
         }
            
     if (state==DOWN)
        {
         tune -= d_rate;
         if(tune<=0.0)
          {
          tune = 0.0;
          state=IDLE;
          }           
        }
      }
      outi[i] = (smpsl[i] + smpsr[i])*.5;
      if (outi[i] > 1.0)
	outi[i] = 1.0f;
      if (outi[i] < -1.0)
	outi[i] = -1.0f;
    
    }

  if (Pmode == 1) use = whammy; else use = tune;

  if(Pupdown)
    PS->ratio = 1.0f-(1.0f-range)*use;
   else 
   PS->ratio = 1.0f+((range-1.0f)*use);


      PS->smbPitchShift (PS->ratio, PERIOD, 2048, hq, fSAMPLE_RATE, outi, outo);

      for (i = 0; i < PERIOD; i++)
	{
	  efxoutl[i] = outo[i] * gain * panning;
	  efxoutr[i] = outo[i] * gain * (1.0f - panning);
	}



};



void
Shifter::setvolume (int value)
{
  this->Pvolume = value;
  outvolume = (float)Pvolume / 127.0f;
};



void
Shifter::setpanning (int value)
{
  this->Ppan = value;
  panning = (float)Ppan / 127.0f;
};



void
Shifter::setgain (int value)
{
  this->Pgain = value;
  gain = (float)Pgain / 127.0f;
  gain *=2.0f;
};


void
Shifter::setinterval (int value)
{
  interval = (float) value;
  if(Pupdown) interval *=-1.0f;
  range = powf (2.0f, interval / 12.0f);

};



void
Shifter::setpreset (int npreset)
{
  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 9;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Fast
    {0, 64, 64, 200, 200, -20, 2, 0, 0, 0},
    //Slowup
    {0, 64, 64, 900, 200, -20, 2, 0, 0, 0},
    //Slowdown
    {0, 64, 64, 900, 200, -20, 3, 1, 0, 0}
  };

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;


};



void
Shifter::changepar (int npar, int value)
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
      setgain (value);
      break;
    case 3:
      Pattack = value;
      a_rate = 1000.0f / ((float)Pattack * fSAMPLE_RATE);
      break;
    case 4:
      Pdecay = value;
      d_rate = 1000.0f / ((float)Pdecay * fSAMPLE_RATE);
      break;
    case 5:
      Pthreshold = value;
      t_level = dB2rap ((float)Pthreshold);
      td_level = t_level*.75f;
      tz_level = t_level*.5f;
      break;
    case 6:
      Pinterval = value;
      setinterval(Pinterval);
      break;
    case 7:
      Pupdown = value;
      setinterval(Pinterval);
      break;
    case 8:
      Pmode = value;
      break; 
     case 9:
      Pwhammy = value;
      whammy = (float) value / 127.0f;
      break;
  
    }


};


int
Shifter::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppan);
      break;
    case 2:
      return (Pgain);
      break;
    case 3:
      return (Pattack);
      break;
    case 4:
      return (Pdecay);
      break;
    case 5:
      return (Pthreshold);
      break;
    case 6:
      return (Pinterval);
      break;
    case 7:
      return (Pupdown);
      break;
    case 8:
      return (Pmode);
      break;
    case 9:
      return (Pwhammy);  
    }
   
return(0);
};
