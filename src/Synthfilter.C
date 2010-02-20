/*

  Synthfilter.C  - Approximate digital model of an analog JFET phaser.
  Analog modeling implemented by Ryan Billing aka Transmogrifox.
  November, 2009
  
  Credit to:
  ///////////////////
  ZynAddSubFX - a software synthesizer
 
  Phaser.C - Phaser effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  
  DSP analog modeling theory & practice largely influenced by various CCRMA publications, particularly works by Julius O. Smith.
  ////////////////////
  
  
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
#include "Synthfilter.h"
#include <stdio.h>

#define ONE_  0.99999f        // To prevent LFO ever reaching 1.0 for filter stability purposes
#define ZERO_ 0.00001f        // Same idea as above.

Synthfilter::Synthfilter (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  lyn1 = new REALTYPE[MAX_SFILTER_STAGES];
  ryn1 = new REALTYPE[MAX_SFILTER_STAGES];
  lx1hp = new REALTYPE[MAX_SFILTER_STAGES]; 
  rx1hp = new REALTYPE[MAX_SFILTER_STAGES];
  ly1hp = new REALTYPE[MAX_SFILTER_STAGES]; 
  ry1hp = new REALTYPE[MAX_SFILTER_STAGES];
   
  Plpstages = 4;
  Phpstages = 2;
  
  inv_period = 1.0f/((float) PERIOD);

   delta = 1.0f/((float) SAMPLE_RATE);
   Rmin = 85.0f;	// 2N5457 typical on resistance at Vgs = 0
   Rmax = 25000.0f;	// Resistor parallel to FET
   C = 0.00000005f;  	     // 50 nF
   Chp = 0.00000005f;
   Clp = 0.00000005f;


  Ppreset = 0;
  setpreset (Ppreset);
  cleanup ();
};

Synthfilter::~Synthfilter ()
{

  if (lyn1 != NULL)
    delete[]lyn1;

  if (ryn1 != NULL)
    delete[]ryn1;

  if (lx1hp != NULL)
    delete[]lx1hp;

  if (rx1hp != NULL)
    delete[]rx1hp;
    
  if (ly1hp != NULL)
    delete[]ly1hp;

  if (ry1hp != NULL)
    delete[]ry1hp;    

};


/*
 * Effect output
 */
void
Synthfilter::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i, j;
  REALTYPE lfol, lfor, lgain, rgain,rmod, lmod, d;
  lgain = 0.0;
  rgain = 0.0;
  	
  lfo.effectlfoout (&lfol, &lfor);
  lmod = lfol*width + depth;
  rmod = lfor*width + depth;

  if (lmod > ONE_)
    lmod = ONE_;
  else if (lmod < ZERO_)
    lmod = ZERO_;
  if (rmod > ONE_)
    rmod = ONE_;
  else if (rmod < ZERO_)
    rmod = ZERO_;
    
    lmod = 1.0f - lmod;
    rmod = 1.0f - rmod;
    lmod*=lmod;
    rmod*=rmod;

      REALTYPE xl = (lmod  - oldlgain) * inv_period;
      REALTYPE xr = (rmod - oldrgain) * inv_period;
      REALTYPE gl = oldlgain;	// Linear interpolation between LFO samples
      REALTYPE gr = oldrgain;
           
  for (i = 0; i < PERIOD; i++)
    {

      REALTYPE lxn = smpsl[i];
      REALTYPE rxn = smpsr[i];
      gl += xl;
      gr += xr;   //linear interpolation of LFO
    
      //Left channel Low Pass Filter
       for (j = 0; j < Plpstages; j++)
	{			
	d = 1.0f + fabs(lxn)*distortion;  // gain decreases as signal amplitude increases.
	
	  //low pass filter:  alpha*x[n] + (1-alpha)*y[n-1] 
	  // alpha = lgain = dt/(RC + dt)
          lgain = delta/( (Rmax * gl * d + Rmin) * Clp + delta);
	  lyn1[j] = lgain * lxn + (1.0f - lgain) * lyn1[j];
	  lyn1[j] += DENORMAL_GUARD;
	  lxn = lyn1[j];
	if (j==1) lxn += fbl;  //Insert feedback after first filter stage

	};
	
      //Left channel High Pass Filter
       for (j = 0; j < Phpstages; j++)
	{			
	d = 1.0f + fabs(lxn)*distortion;  // gain decreases as signal amplitude increases.
	
	  //high pass filter:  alpha*(y[n-1] + x[n] - x[n-1]) // alpha = lgain = RC/(RC + dt)
	  lgain = (Rmax * gl * d + Rmin) * Chp/( (Rmax * gl * d + Rmin) * Chp + delta);
	  ly1hp[j] = lgain * (lxn + ly1hp[j] + lx1hp[j]);
	  	  
	  ly1hp[j] += DENORMAL_GUARD;
	  lx1hp[j] = lxn;
	  lxn = ly1hp[j];
	};	
	
	
      //Right channel Low Pass Filter
       for (j = 0; j < Plpstages; j++)
	{			
	d = 1.0f + fabs(rxn)*distortion;  //This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.

	rgain = delta/((Rmax*gr*d + Rmin)*Clp + delta);
	  ryn1[j] = rgain * rxn + (1.0f - rgain) * ryn1[j];
	  ryn1[j] += DENORMAL_GUARD;
	  //ry1hp[j] = rxn;
	  rxn = ryn1[j];
	if (j==1) rxn += fbr;  //Insert feedback after first filter stage
	};
	
      //Right channel High Pass Filter
       for (j = 0; j < Phpstages; j++)
	{			
	d = 1.0f + fabs(rxn)*distortion;  // gain decreases as signal amplitude increases.
	
	  //high pass filter:  alpha*(y[n-1] + x[n] - x[n-1]) // alpha = rgain = RC/(RC + dt)
	  rgain = (Rmax * gr * d + Rmin) * Chp/( (Rmax * gr * d + Rmin) * Chp + delta);
	  ry1hp[j] = rgain * (rxn + ry1hp[j] + rx1hp[j]);
	  	  
	  ry1hp[j] += DENORMAL_GUARD;
	  rx1hp[j] = rxn;
	  rxn = ry1hp[j];
	};

      fbl = lxn * fb;
      fbr = rxn * fb;
      efxoutl[i] = lxn;
      efxoutr[i] = rxn;

    };

  oldlgain = lmod;
  oldrgain = rmod;

  if (Poutsub != 0)
    for (i = 0; i < PERIOD; i++)
      {
	efxoutl[i] *= -1.0f;
	efxoutr[i] *= -1.0f;
      };

};

/*
 * Cleanup the effect
 */
void
Synthfilter::cleanup ()
{
  fbl = 0.0;
  fbr = 0.0;
  oldlgain = 0.0;
  oldrgain = 0.0;
  for (int i = 0; i < Plpstages; i++)
    {
      lyn1[i] = 0.0;
      ryn1[i] = 0.0;
      
      ly1hp[i] = 0.0;
      lx1hp[i] = 0.0;      
      ry1hp[i] = 0.0;
      rx1hp[i] = 0.0;
    };
};

/*
 * Parameter control
 */
void
Synthfilter::setwidth (int Pwidth)
{
  this->Pwidth = Pwidth;
  width = ((float)Pwidth / 127.0f);
};


void
Synthfilter::setfb (int Pfb)
{
  this->Pfb = Pfb;
  fb = (float) (Pfb - 64);
  if (fb<0.0f) fb /= 32.0f;
  else if (fb>0.0f) fb/=128.2f;
};

void
Synthfilter::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  // outvolume is needed in calling program
  outvolume = (float)Pvolume / 127.0f;
};

void
Synthfilter::setdistortion (int Pdistortion)
{
  this->Pdistortion = Pdistortion;
  distortion = (float)Pdistortion / 100.0f;
};


void
Synthfilter::setdepth (int Pdepth)
{
  this->Pdepth = Pdepth;
  depth = (float)(Pdepth - 64) / 64.0f;  //Pdepth input should be 0-127.  depth shall range 0-0.5 since we don't need to shift the full spectrum.
};


void
Synthfilter::setpreset (int npreset)
{
  const int PRESET_SIZE = 13;
  const int NUM_PRESETS = 6;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Phaser1
    {64, 20, 14, 0, 1, 64, 110, 40, 4, 2, 0, 64, 1},
    //Phaser2
    {64, 20, 14, 5, 1, 64, 110, 40, 6, 2, 0, 70, 1},
    //Phaser3
    {64, 20, 9, 0, 0, 64, 40, 40, 8, 10, 2, 60, 0},
    //Phaser4
    {64, 20, 14, 10, 0, 64, 110, 80, 7, 2, 1, 45, 1},
    //Phaser5
    {25, 20, 240, 10, 0, 64, 25, 16, 8, 2, 0, 25, 0},
    //Phaser6
    {64, 20, 1, 10, 1, 64, 110, 40, 12, 2, 0, 70, 1}
  };
  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Synthfilter::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setdistortion (value);
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
      setwidth (value);
      break;
    case 7:
      setfb (value);
      break;
    case 8:
      Plpstages = value;
      if (Plpstages >= MAX_SFILTER_STAGES)
      Plpstages = MAX_SFILTER_STAGES ;
      cleanup ();
      break;
    case 9:
     Phpstages = value;
      if (Phpstages >= MAX_SFILTER_STAGES)
      Phpstages = MAX_SFILTER_STAGES ;
      cleanup ();
      break;
    case 10:
      if (value > 1)
	value = 1;
      Poutsub = value;
      break;
    case 11:
      setdepth (value);
      break;
    case 12:
      if (value > 1)
	value = 1;
      Penvelope = value;
      break;
    };
};

int
Synthfilter::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Pdistortion);
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
      return (Pfb);
      break;
    case 8:
      return (Plpstages);
      break;
    case 9:
      return (Phpstages);
      break;
    case 10:
      return (Poutsub);
      break;
    case 11:
      return (Pdepth);
      break;
    case 12:
      return (Penvelope);
      break;

    default:
      return (0);
    };

};
