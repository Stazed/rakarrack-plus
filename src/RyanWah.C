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

RyanWah::RyanWah (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
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
RyanWah::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;

  REALTYPE lfol, lfor;
  lfo.effectlfoout (&lfol, &lfor);
  lfol *= depth * 5.0f;
  lfor *= depth * 5.0f;

  for (i = 0; i < PERIOD; i++)
    {
      efxoutl[i] = smpsl[i];
      efxoutr[i] = smpsr[i];

      REALTYPE x = (fabsf (smpsl[i]) + fabsf (smpsr[i])) * 0.5f;
      ms1 = ms1 * (1.0f - ampsmooth) + x * ampsmooth + 1e-10f;
      
      //oldfbias -= 0.001 * oldfbias2;
      oldfbias = oldfbias * (1.0f - ampsmooth) + fbias * ampsmooth + 1e-10f;  //smooth MIDI control
      oldfbias1 = oldfbias1 * (1.0f - ampsmooth) + oldfbias * ampsmooth + 1e-10f;
      oldfbias2 = oldfbias2 * (1.0f - ampsmooth) + oldfbias1 * ampsmooth + 1e-10f;
    };


  REALTYPE rms = ms1 * ampsns + oldfbias2;
  if(rms>1.0f) rms = 1.0f;

   REALTYPE frl = minfreq + maxfreq*(lfol + rms);
   REALTYPE frr = minfreq + maxfreq*(lfor + rms);

  filterl->setfreq_and_q (frl, q);
  filterr->setfreq_and_q (frr, q);

  filterl->filterout (efxoutl);
  filterr->filterout (efxoutr);

  //panning    
  for (i = 0; i < PERIOD; i++)
    {
      efxoutl[i] *= lpanning;
      efxoutr[i] *= rpanning;
    };

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
RyanWah::setdepth (int Pdepth)
{
  this->Pdepth = Pdepth;
  depth = powf (((float)Pdepth / 127.0f), 2.0f);
};


void
RyanWah::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
  volume = outvolume;
};

void
RyanWah::setpanning (int Pp)
{
  Ppanning = Pp;
  lpanning = 0.5f + ((float)Ppanning) / 127.0f;
  rpanning = 1.0f - lpanning;
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
  filterl->setmix(1, 0.5f, 2.0f, 0.0f);
  filterr->setmix(1, 0.5f, 2.0f, 0.0f);
};

void
RyanWah::setpreset (int npreset)
{
  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 5;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //WahWah
    {0, 64, 138, 0, 0, 64, 70, 90, 0, 60},
    //AutoWah
    {64, 64, 80, 0, 0, 80, 70, 0, 0, 60},
    //Sweep
    {64, 64, 7, 0, 0, 50, 80, 0, 0, 60},
    //VocalMorph1
    {64, 64, 138, 0, 0, 64, 70, 64, 0, 60},
    //VocalMorph1
    {64, 64, 33, 0, 0, 96, 64, 0, 0, 60}
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
      setpanning (value);
      break;
    case 2:
      lfo.Pfreq = value;
      lfo.updateparams ();
      break;
    case 3:
      lfo.Prandomness = value;
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
      setdepth (value);
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
      return (Ppanning);
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
      return (Pdepth);
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
    default:
      return (0);
    };

};
