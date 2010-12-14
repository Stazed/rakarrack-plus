/*
  ZynAddSubFX - a software synthesizer
 
  Rakarrack Guitar Effects Processor
  
  InfiniT : Infinitely rising & falling filter effect
  Author:  Ryan Billing (aka Transmogrifox)
  Copyright (C) 2010

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

#include <math.h>
#include "Infinity.h"
#include <stdio.h>

Infinity::Infinity (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  Ppreset = 0;
  int i;
  for (i = 0; i<NUM_INF_BANDS; i++) {
  filterl[i] = new RBFilter (0, 80.0f, 70.0f, 1);
  filterr[i] = new RBFilter (0, 80.0f, 70.0f, 1);
  }
  setpreset (Ppreset);
  reinitfilter();
};

Infinity::~Infinity ()
{
};

void inline
Infinity::oscillator()
{
for (int i=0; i<NUM_INF_BANDS; i++)  {
bandstate[i].sinp += fconst*bandstate[i].cosp;
bandstate[i].cosp -= bandstate[i].sinp*fconst;
bandstate[i].lfo = 0.5f*(1.0f + bandstate[i].sinp);
bandstate[i].ramp += rampconst;
if (bandstate[i].ramp >= 1.0f) bandstate[i].ramp = 0.0f;  //probably faster than fmod()
bandstate[i].vol = bandstate[i].level*bandstate[i].lfo;

  filterl[i]->directmod(bandstate[i].ramp);
  filterr[i]->directmod(bandstate[i].ramp);
}

}

/*
 * Apply the effect
 */
void
Infinity::out (float * smpsl, float * smpsr)
{
  int i, j;
  float tmpr, tmpl;
  for (i = 0; i<PERIOD; i++)  {
    tmpr = tmpl = 0.0f;
    
    //modulate
    oscillator();
    
    //run filter
    for (j=0; j<NUM_INF_BANDS; j++)  {
    tmpl+=bandstate[j].vol*filterl[i]->filterout_s(smpsl[i]);
    tmpr+=bandstate[j].vol*filterr[i]->filterout_s(smpsr[i]);
    }
    efxoutl[i] = tmpl;
    efxoutr[i] = tmpr;
  }
 
};

/*
 * Cleanup the effect
 */
void
Infinity::cleanup ()
{
  reinitfilter ();
  for ( int i = 0; i<NUM_INF_BANDS; i++) {
  filterl[i]->cleanup();
  filterr[i]->cleanup();
  }
  
  
};


/*
 * Parameter control
 */

void
Infinity::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
};


void
Infinity::reinitfilter ()
{

for (int i=0; i<NUM_INF_BANDS; i++)  {  //get them started on their respective phases
bandstate[i].sinp = sinf(D_PI*((float) i)/((float) NUM_INF_BANDS));
bandstate[i].cosp = cosf(D_PI*((float) i)/((float) NUM_INF_BANDS));
bandstate[i].ramp = ((float) i)/((float) NUM_INF_BANDS);
  
  filterl[i]->setmix(0, NULL, NULL, NULL);
  filterr[i]->setmix(0, NULL, NULL, NULL);
  filterl[i]->setmode(1);
  filterr[i]->setmode(1);
  filterl[i]->settype(2);
  filterr[i]->settype(2);  //bandpass  
  filterl[i]->setq(qq);
  filterr[i]->setq(qq);
  filterl[i]->directmod(bandstate[i].ramp);
  filterr[i]->directmod(bandstate[i].ramp);
}
  
};

void
Infinity::setpreset (int npreset)
{
  const int PRESET_SIZE = 18;
  const int NUM_PRESETS = 5;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Wah Pedal
    {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450, 1, 1 },
    //Mutron
    {0, 15, 138, 0, 0, 64, 0, 50, 0, 30, 32, 0, 5, 1, 2000, 60, 0, 1 },
    //Phase Wah
    {0, 50, 60, 0, 0, 64, 30, 10, 10, 30, 32, 0, 10, 2, 2000, 350, 1, 1 },
    //Succulent Phaser
    {64, 8, 35, 10, 0, 64, 50, -10, 53, 35, 28, -16, 32, 4, 2600, 300, 1, 1 },
    //Quacky
    {16, 10, 60, 0, 0, 64, 0, 40, 10, 32, -16, 40, -3, 1, 2000, 400, 1, 1 } 

  };

  if(npreset>NUM_PRESETS-1)  
    {   
     Fpre->ReadPreset(31,npreset-NUM_PRESETS+1);    
     for (int n = 0; n < PRESET_SIZE; n++)    
     changepar (n, pdata[n]);    
    }    
  else                                      
  {     

  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  }

  Ppreset = npreset;
  
  reinitfilter ();
};


void
Infinity::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
     case 10:
      break;     
     case 11:
      break;     
     case 12:

      break;

           
    };
};

int
Infinity::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return ();
      break;
    case 2:
      return ();
      break;
    case 3:
      return ();
      break;
    case 4:
      return ();
      break;
    case 5:
      return ();
      break;
    case 6:
      return ();
      break;
    case 7:
      return ();
      break;
    case 8:
      return ();
      break;
    case 9:
      return ();
      break;
    case 10:
      return ();
      break;
    case 11:
      return ();
      break;
    case 12:
      return ();
      break;
    case 13:
      return ();
      break;
    case 14:
      return ();    
      break;
    case 15:
      return ();
      break;
    case 16:
      return ();
      break;
    case 17:
      return ();  
      break;
    case 18:
      return ();
      break;
    default:
      return (0);
    };

};
