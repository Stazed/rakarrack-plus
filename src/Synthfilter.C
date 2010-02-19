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
#define PHASER_LFO_SHAPE 2
#define ONE_  0.99999f        // To prevent LFO ever reaching 1.0 for filter stability purposes
#define ZERO_ 0.00001f        // Same idea as above.

Synthfilter::Synthfilter (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  lxn1 = NULL;

  lyn1 = NULL;

  rxn1 = NULL;

  ryn1 = NULL;

  offset = new REALTYPE[12];	//model mismatch between JFET devices
  offset[0] = -0.2509303f;
  offset[1] = 0.9408924f;
  offset[2] = 0.998f;
  offset[3] = -0.3486182f;
  offset[4] = -0.2762545f;
  offset[5] = -0.5215785f;
  offset[6] = 0.2509303f;
  offset[7] = -0.9408924f;
  offset[8] = -0.998f;
  offset[9] = 0.3486182f;
  offset[10] = 0.2762545f;
  offset[11] = 0.5215785f;
   
   barber = 0;  //Deactivate barber pole phasing by default
   
   mis = 1.0f/((float) SAMPLE_RATE);
   Rmin = 125.0f;	// 2N5457 typical on resistance at Vgs = 0
   Rmax = 25000.0f;	// Resistor parallel to FET
   Rmx = Rmin/Rmax;
   Rconst = 1.0f/((float) SAMPLE_RATE);  // Handle parallel resistor relationship
   C = 0.00000005f;	     // 50 nF
   CFs = (float) 2.0f*(float)SAMPLE_RATE*C;


  Ppreset = 0;
  setpreset (Ppreset);
  cleanup ();
};

Synthfilter::~Synthfilter ()
{

  if (lxn1 != NULL)
    delete[]lxn1;

  if (lyn1 != NULL)
    delete[]lyn1;

  if (rxn1 != NULL)
    delete[]rxn1;

  if (ryn1 != NULL)
    delete[]ryn1;

  if (offset != NULL)
    delete[]offset;
};


/*
 * Effect output
 */
void
Synthfilter::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i, j;
  REALTYPE lfol, lfor, lgain, rgain, bl, br, rmod, lmod, d, hpfr, hpfl;
  lgain = 0.0;
  rgain = 0.0;
  
  //initialize hpf
  hpfl = 0.0;
  hpfr = 0.0;
  	
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


  for (i = 0; i < PERIOD; i++)
    {
      REALTYPE x = (REALTYPE) i / (float)PERIOD;
      REALTYPE gl = (lmod  - oldlgain) * x + oldlgain;	// Linear interpolation between LFO samples
      REALTYPE gr = (rmod - oldrgain) * x + oldrgain;
      REALTYPE lxn = smpsl[i];
      REALTYPE rxn = smpsr[i];


	if (barber) {
	gl = fmodf((gl + 0.25f) , ONE_);
	gr = fmodf((gr + 0.25f) , ONE_);
	 };

    
      //Left channel
       for (j = 0; j < Pstages; j++)
	{			//Phasing
	d = 1.0f + fabs(lxn)*distortion;  //This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.

	lgain = mis/( (Rmax * gl * d + Rmin) * C + mis);
//alpha = lgain = dt/(RC + dt)
	  lyn1[j] = lgain * lxn + (1.0f - lgain) * lyn1[j];
	  lyn1[j] += DENORMAL_GUARD;
	  //lxn1[j] = lxn;
	  lxn = lyn1[j];
	if (j==1) lxn += fbl;  //Insert feedback after first filter stage

	};
	
      //Right channel
       for (j = 0; j < Pstages; j++)
	{			//Phasing
	d = 1.0f + fabs(rxn)*distortion;  //This is symmetrical. FET is not, so this deviates slightly, however sym dist. is better sounding than a real FET.

	rgain = mis/((Rmax*gr*d + Rmin)*C + mis);
//alpha = rgain = dt/(RC + dt)
	  ryn1[j] = rgain * rxn + (1.0f - rgain) * ryn1[j];
	  ryn1[j] += DENORMAL_GUARD;
	  //rxn1[j] = rxn;
	  rxn = ryn1[j];
	if (j==1) rxn += fbr;  //Insert feedback after first filter stage

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
  for (int i = 0; i < Pstages; i++)
    {
      lxn1[i] = 0.0;

      lyn1[i] = 0.0;

      rxn1[i] = 0.0;

      ryn1[i] = 0.0;

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
Synthfilter::setoffset (int Poffset)
{
  this->Poffset = Poffset;  
  offsetpct = (float)Poffset / 127.0f;
};

void
Synthfilter::setstages (int Pstages)
{

  if (lxn1 != NULL)
    delete[]lxn1;

  if (lyn1 != NULL)
    delete[]lyn1;

  if (rxn1 != NULL)
    delete[]rxn1;

  if (ryn1 != NULL)
    delete[]ryn1;


  if (Pstages >= MAX_PHASER_STAGES)
    Pstages = MAX_PHASER_STAGES ;
  this->Pstages = Pstages;


      lxn1 = new REALTYPE[Pstages];
      lyn1 = new REALTYPE[Pstages];
 
      rxn1 = new REALTYPE[Pstages];
      ryn1 = new REALTYPE[Pstages];
 
  cleanup ();
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
    {64, 20, 14, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1},
    //Phaser2
    {64, 20, 14, 5, 1, 64, 110, 40, 6, 10, 0, 70, 1},
    //Phaser3
    {64, 20, 9, 0, 0, 64, 40, 40, 8, 10, 0, 60, 0},
    //Phaser4
    {64, 20, 14, 10, 0, 64, 110, 80, 7, 10, 1, 45, 1},
    //Phaser5
    {25, 20, 240, 10, 0, 64, 25, 16, 8, 100, 0, 25, 0},
    //Phaser6
    {64, 20, 1, 10, 1, 64, 110, 40, 12, 10, 0, 70, 1}
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
      barber = 0;
      if (value == 2) barber = 1;
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
      setstages (value);
      break;
    case 9:
      setoffset (value);
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
      Phyper = value;
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
      return (Pstages);
      break;
    case 9:
      return (Poffset);
      break;
    case 10:
      return (Poutsub);
      break;
    case 11:
      return (Pdepth);
      break;
    case 12:
      return (Phyper);
      break;

    default:
      return (0);
    };

};
