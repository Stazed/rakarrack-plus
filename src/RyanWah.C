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
  
  base = 7.0f;		//sets curve of modulation to frequency relationship
  ibase = 1.0f/base;

  Pampsns = 0;
  Pampsnsinv= 0;
  Pampsmooth= 0;
  maxfreq = 5000.0f;
  minfreq = 40.0f;
  frequency = 40.0f;
  q = 10.0f;
  Pqm = 1;  //Set backward compatibility mode by default.
  hpmix = 0.0f;
  lpmix = 0.5f;
  bpmix = 2.0f;
  Ppreset = 0;
  wahsmooth = 1.0f - expf(-1.0f/(0.02f*fSAMPLE_RATE));  //0.02 seconds
  
  Fstages = 1;
  Ftype = 1;
  filterl = new RBFilter (0, 80.0f, 70.0f, 1);
  filterr = new RBFilter (0, 80.0f, 70.0f, 1);
  setpreset (Ppreset);

  cleanup ();
};

RyanWah::~RyanWah ()
{
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
      ms1 = ms1 * ampsmooth + x * (1.0f - ampsmooth) + 1e-10f;
      
      //oldfbias -= 0.001 * oldfbias2;
      oldfbias = oldfbias * (1.0f - wahsmooth) + fbias * wahsmooth + 1e-10f;  //smooth MIDI control
      oldfbias1 = oldfbias1 * (1.0f - wahsmooth) + oldfbias * wahsmooth + 1e-10f;
      oldfbias2 = oldfbias2 * (1.0f - wahsmooth) + oldfbias1 * wahsmooth + 1e-10f;
    };


  float rms = ms1 * ampsns + oldfbias2;
  
  if(rms>0.0f) //apply some smooth limiting
  {
  rms = 1.0f - 1.0f/(rms*rms + 1.0f);
  }
  else
  {
  rms = -1.0f + 1.0f/(rms*rms + 1.0f);  
  }
      
  if(variq) q = powf(2.0f,(2.0f*(1.0f-rms)+1.0f));

   float lmod =(lfol + rms);
   float rmod = (lfor + rms);
  if(lmod>1.0f) lmod = 1.0f;
  if(lmod<0.0f) lmod = 0.0f;
  if(rmod>1.0f) rmod = 1.0f;
  if(rmod<0.0f) rmod = 0.0f;  
  
   //rms*=rms;    
   float frl = minfreq + maxfreq*(powf(base, lmod) - 1.0f)*ibase;
   float frr = minfreq + maxfreq*(powf(base, rmod) - 1.0f)*ibase;
   
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
  filterl->cleanup();
  filterr->cleanup();
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
};

void
RyanWah::setampsns (int Pp)
{
  Pampsns = Pp;
  if(Pampsns>0)
  {
  ampsns = expf(0.083f*(float)Pampsns);
  }  
  else
  {
  ampsns = - expf(-0.083f*(float)Pampsns);
  }  
  fbias  =  ((float)Pampsnsinv )/ 127.0f;
  ampsmooth = expf(-1.0f/((((float) Pampsmooth)/127.0f + 0.01f)*fSAMPLE_RATE)); //goes up to 1 second

};

void
RyanWah::reinitfilter ()
{
  //setmix (int mix, float lpmix, float bpmix, float hpmix)
  filterl->setmix(1, lpmix, bpmix, hpmix);
  filterr->setmix(1, lpmix, bpmix, hpmix);
  
};

void
RyanWah::setpreset (int npreset)
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
     Pstages = (value - 1);
     filterl->setstages(Pstages);
     filterr->setstages(Pstages);
     cleanup();
      break;  
     case 14:
     Prange = value;
     maxfreq = ((float) Prange);
     break; 
     case 15:
     Pminfreq = value;
     minfreq = (float) value;
     break;  
     case 16:
     variq = value;
     break;
     case 17:
     Pqm = value;
     filterl->setmode(Pqm);
     filterr->setmode(Pqm);     
     break;
     case 18:
     Ppreset = value;
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
      return (Pstages + 1);
      break;
    case 14:
      return (Prange);    
      break;
    case 15:
      return (Pminfreq);
      break;
    case 16:
      return (variq);
      break;
    case 17:
      return (Pqm);  
      break;
    case 18:
      return (Ppreset);
      break;
    default:
      return (0);
    };

};
