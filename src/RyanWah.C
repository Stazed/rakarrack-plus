/*
  ZynAddSubFX - a software synthesizer
 
  RyanWah.C - "WahWah" effect and others
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Ryan Billing

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
#include "RyanWah.h"
#include <stdio.h>

RyanWah::RyanWah (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  Ppreset = 0;

  filterl = NULL;
  filterr = NULL;

  Pampsns = 0;
  Pampsnsinv= 0;
  Pampsmooth= 0;
  maxfreq = 5000.0f;
  minfreq = 40.0f;
  frequency = 40.0f;
  q = 75.0f;
  hpmix = 0.0f;
  lpmix = 0.5f;
  bpmix = 2.0f;
  Ppreset = 0;
  
  Fstages = 1;
  Ftype = 1;
  filterl = new SVFilter (0, 80.0f, 70.0f, 1);
  filterr = new SVFilter (0, 80.0f, 70.0f, 1);
  setpreset (Ppreset);

  cleanup ();
};

RyanWah::~RyanWah ()
{

  delete (filterl);
  delete (filterr);
};


/*
 * Apply the effect
 */
void
RyanWah::out (float * smpsl, float * smpsr)
{
  int i;

  float lfol, lfor;
  lfo.effectlfoout (&lfol, &lfor);
  lfol *= depth * 5.0f;
  lfor *= depth * 5.0f;

  for (i = 0; i < PERIOD; i++)
    {
      efxoutl[i] = smpsl[i];
      efxoutr[i] = smpsr[i];

      float x = (fabsf (smpsl[i]) + fabsf (smpsr[i])) * 0.5f;
      ms1 = ms1 * (1.0f - ampsmooth) + x * ampsmooth + 1e-10f;
      
      //oldfbias -= 0.001 * oldfbias2;
      oldfbias = oldfbias * (1.0f - ampsmooth) + fbias * ampsmooth + 1e-10f;  //smooth MIDI control
      oldfbias1 = oldfbias1 * (1.0f - ampsmooth) + oldfbias * ampsmooth + 1e-10f;
      oldfbias2 = oldfbias2 * (1.0f - ampsmooth) + oldfbias1 * ampsmooth + 1e-10f;
    };


  float rms = ms1 * ampsns + oldfbias2;
  if(rms>1.0f) rms = 1.0f;

   float frl = minfreq + maxfreq*(lfol + rms);
   float frr = minfreq + maxfreq*(lfor + rms);
   
   centfreq = frl; //testing variable

  filterl->setfreq_and_q (frl, q);
  filterr->setfreq_and_q (frr, q);

  filterl->filterout (efxoutl);
  filterr->filterout (efxoutr);


};

/*
 * Cleanup the effect
 */
void
RyanWah::cleanup ()
{
  reinitfilter ();
  ms1 = 0.0;
  oldfbias = oldfbias1 = oldfbias2 = 0.0f;

};


/*
 * Parameter control
 */

void
RyanWah::setwidth (int Pwidth)
{
  this->Pwidth = Pwidth;
  depth = powf (((float)Pwidth / 127.0f), 2.0f);
};


void
RyanWah::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
  volume = outvolume;
};

void
RyanWah::setampsns (int Pp)
{
  Pampsns = Pp;
  if(Pampsns>0)
  {
  ampsns = powf (((float)Pampsns) / 64.0f, 2.5f) * 100.0f;
  }  
  else
  {
  ampsns = - powf (( -((float) Pampsns)) / 64.0f, 2.5f) * 100.0f;
  }  
  fbias  =  powf ((float)Pampsnsinv / 127.0f, 2.5f);
  ampsmooth = expf (-(70.0f + (float) Pampsmooth) / 127.0f * 10.0f) * 0.99f;

};

void
RyanWah::reinitfilter ()
{
  filterl->cleanup();
  filterr->cleanup();
  //setmix (int mix, float lpmix, float bpmix, float hpmix)
  filterl->setmix(1, lpmix, bpmix, hpmix);
  filterr->setmix(1, lpmix, bpmix, hpmix);
  
  printf("lp %f bp %f hp %f q %f maxfreq %f stages %d minfreq %f fbias2 %f center %f\n", lpmix, bpmix, hpmix, q, maxfreq, Pstages, minfreq,oldfbias2,centfreq);
};

void
RyanWah::setpreset (int npreset)
{
  const int PRESET_SIZE = 15;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Wah Pedal
    {0, 72, 138, 0, 0, 64, 0, 50, 25, 45, 16, -40, 0, 1, 2000 },
    //Mutron
    {0, 90, 138, 0, 0, 64, 0, 50, 0, 30, 32, 0, 5, 1, 2000 },
    //Phase Wah
    {0, 50, 60, 0, 0, 64, 30, 10, 10, 30, 32, 0, 10, 2, 2000 },
    //Phaser
    {64, 60, 60, 0, 0, 64, 50, 10, 10, 40, 32, 32, 32, 4, 2000 },

  };

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);

  Ppreset = npreset;

  reinitfilter ();
};


void
RyanWah::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      Pq = value;
      q = (float) Pq;      
      break;
    case 2:
      lfo.Pfreq = value;
      lfo.updateparams ();
      break;
    case 3:
      lfo.Prandomness = 0;//value;
      lfo.updateparams ();
      break;
    case 4:
      lfo.PLFOtype = value;
      lfo.updateparams ();
      break;
    case 5:
      lfo.Pstereo = value;
      lfo.updateparams ();
      break;
    case 6:
      setwidth (value);
      break;
    case 7:
      setampsns (value);
      break;
    case 8:
      Pampsnsinv = value;
      setampsns (Pampsns);
      break;
    case 9:
      Pampsmooth = value;
      setampsns (Pampsns);
      break;
     case 10:
      Plp = value;
      lpmix = ((float) Plp)/32.0f;
      reinitfilter ();     
      break;     
     case 11:
      Pbp = value;
      bpmix = ((float) Pbp)/32.0f;
      reinitfilter ();
      break;     
     case 12:
      Php = value;
      hpmix = ((float) Php)/32.0f;
      reinitfilter (); 
      break;
     case 13:
     Pstages = (value);
     filterl->setstages(Pstages);
     filterr->setstages(Pstages);
      break;  
     case 14:
     Prange = value;
     maxfreq = ((float) Prange) + 100.0 + fbias * 800.0f;
     minfreq = 40.0f + 800.0f * ((float) Prange)/6000.0f;
     break;   
      
    };
};

int
RyanWah::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Pq);
      break;
    case 2:
      return (lfo.Pfreq);
      break;
    case 3:
      return (lfo.Prandomness);
      break;
    case 4:
      return (lfo.PLFOtype);
      break;
    case 5:
      return (lfo.Pstereo);
      break;
    case 6:
      return (Pwidth);
      break;
    case 7:
      return (Pampsns);
      break;
    case 8:
      return (Pampsnsinv);
      break;
    case 9:
      return (Pampsmooth);
      break;
    case 10:
      return (Plp);
      break;
    case 11:
      return (Pbp);
      break;
    case 12:
      return (Php);
      break;
    case 13:
      return (Pstages);
      break;
    case 14:
      return (Prange);    
      break;
    default:
      return (0);
    };

};
