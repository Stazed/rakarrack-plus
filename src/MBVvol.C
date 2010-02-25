/*
  MBVvol.C - Vary Band Volumen effect
  
  ZynAddSubFX - a software synthesizer
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  
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
#include "MBVvol.h"

/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */



MBVvol::MBVvol (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  lowl = (float *) malloc (sizeof (float) * PERIOD);
  lowr = (float *) malloc (sizeof (float) * PERIOD);
  midll = (float *) malloc (sizeof (float) * PERIOD);
  midlr = (float *) malloc (sizeof (float) * PERIOD);
  midhl = (float *) malloc (sizeof (float) * PERIOD);
  midhr = (float *) malloc (sizeof (float) * PERIOD);
  highl = (float *) malloc (sizeof (float) * PERIOD);
  highr = (float *) malloc (sizeof (float) * PERIOD);


  lpf1l = new AnalogFilter (2, 500.0f, .7071f, 0);
  lpf1r = new AnalogFilter (2, 500.0f, .7071f, 0);
  hpf1l = new AnalogFilter (3, 500.0f, .7071f, 0);
  hpf1r = new AnalogFilter (3, 500.0f, .7071f, 0);
  lpf2l = new AnalogFilter (2, 2500.0f, .7071f, 0);
  lpf2r = new AnalogFilter (2, 2500.0f, .7071f, 0);
  hpf2l = new AnalogFilter (3, 2500.0f, .7071f, 0);
  hpf2r = new AnalogFilter (3, 2500.0f, .7071f, 0);
  lpf3l = new AnalogFilter (2, 5000.0f, .7071f, 0);
  lpf3r = new AnalogFilter (2, 5000.0f, .7071f, 0);
  hpf3l = new AnalogFilter (3, 5000.0f, .7071f, 0);
  hpf3r = new AnalogFilter (3, 5000.0f, .7071f, 0);


  //default values
  Ppreset = 0;
  Pvolume = 50;
  volL=volLr=volML=volMLr=volMH=volMHr=volH=volHr=.25f;

  setpreset (Ppreset);
  cleanup ();
};

MBVvol::~MBVvol ()
{
  delete (lpf1l);
  delete (lpf1r);
  delete (hpf1l);
  delete (hpf1r);
  delete (lpf2l);
  delete (lpf2r);
  delete (hpf2l);
  delete (hpf2r);
  delete (lpf3l);
  delete (lpf3r);
  delete (hpf3l);
  delete (hpf3r);

};

/*
 * Cleanup the effect
 */
void
MBVvol::cleanup ()
{
  lpf1l->cleanup ();
  hpf1l->cleanup ();
  lpf1r->cleanup ();
  hpf1r->cleanup ();
  lpf2l->cleanup ();
  hpf2l->cleanup ();
  lpf2r->cleanup ();
  hpf2r->cleanup ();
  lpf3l->cleanup ();
  hpf3l->cleanup ();
  lpf3r->cleanup ();
  hpf3r->cleanup ();

};
/*
 * Effect output
 */
void
MBVvol::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;

 
  memcpy(lowl,smpsl,sizeof(float) * PERIOD);
  memcpy(midll,smpsl,sizeof(float) * PERIOD);
  memcpy(midhl,smpsl,sizeof(float) * PERIOD);
  memcpy(highl,smpsl,sizeof(float) * PERIOD);

  lpf1l->filterout(lowl);
  hpf1l->filterout(midll);
  lpf2l->filterout(midll);
  hpf2l->filterout(midhl);
  lpf3l->filterout(midhl);
  hpf3l->filterout(highl);

  memcpy(lowr,smpsr,sizeof(float) * PERIOD);
  memcpy(midlr,smpsr,sizeof(float) * PERIOD);
  memcpy(midhr,smpsr,sizeof(float) * PERIOD);
  memcpy(highr,smpsr,sizeof(float) * PERIOD);

  lpf1r->filterout(lowr);
  hpf1r->filterout(midlr);
  lpf2r->filterout(midlr);
  hpf2r->filterout(midhr);
  lpf3r->filterout(midhr);
  hpf3r->filterout(highr);

  lfo1.effectlfoout (&lfo1l, &lfo1r);
  lfo2.effectlfoout (&lfo2l, &lfo2r);

  volL=lfo1l*2.0f;
  volLr=lfo1r*2.0f;
  volML=lfo2l*2.0f;
  volMLr=lfo2r*2.0f;
  volMH=lfo1l*2.0f;
  volMHr=lfo1r*2.0f;
  volH =lfo2l*2.0f;
  volHr=lfo2r*2.0f;
  
  printf("%f %f\n",lfo1l,lfo1r); 

  for (i = 0; i < PERIOD; i++)
  {
    efxoutl[i]=lowl[i]*volL+midll[i]*volML+midhl[i]*volMH+highl[i]*volH;
    efxoutr[i]=lowr[i]*volLr+midlr[i]*volMLr+midhr[i]*volMHr+highr[i]*volHr;
  }      
    
};


/*
 * Parameter control
 */
void
MBVvol::setvolume (int value)
{
  Pvolume = value;
  outvolume = (float)Pvolume / 127.0f;
};


void
MBVvol::setCross1 (int value)
{
  Cross1 = value;
  lpf1l->setfreq ((float)value);
  lpf1r->setfreq ((float)value);
  hpf1l->setfreq ((float)value);
  hpf1r->setfreq ((float)value);

};

void
MBVvol::setCross2 (int value)
{
  Cross2 = value;
  hpf2l->setfreq ((float)value);
  hpf2r->setfreq ((float)value);
  lpf2l->setfreq ((float)value);
  lpf2r->setfreq ((float)value);

};

void
MBVvol::setCross3 (int value)
{
  Cross3 = value;
  hpf3l->setfreq ((float)value);
  hpf3r->setfreq ((float)value);
  lpf3l->setfreq ((float)value);
  lpf3r->setfreq ((float)value);

};


void
MBVvol::setpreset (int npreset)
{
  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Vary1
    {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000},
    //Vary2
    {0, 80, 0, 64, 40, 0, 0, 120, 600, 2300},
    //Vary3
    {0, 120, 0, 64, 40, 0, 0, 800, 2300, 5200}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();
};


void
MBVvol::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      lfo1.Pfreq = value;
      lfo1.updateparams ();
      break;
    case 2:
      lfo1.PLFOtype = value;
      lfo1.updateparams ();
      break;
    case 3:
      lfo1.Pstereo = value;
      lfo1.updateparams ();
      break;
    case 4:
      lfo2.Pfreq = value;
      lfo2.updateparams ();
      break;
    case 5:
      lfo2.PLFOtype = value;
      lfo2.updateparams ();
      break;
    case 6:
      lfo2.Pstereo = value;
      lfo2.updateparams ();
      break;
    case 7:
      setCross1 (value);
      break;
    case 8:
      setCross2 (value);
      break;
    case 9:
      setCross3(value);
      break;
    };
};

int
MBVvol::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (lfo1.Pfreq);
      break;
    case 2:
      return (lfo1.PLFOtype);
      break;
    case 3:
      return (lfo1.Pstereo);
      break;
    case 4:
      return (lfo2.Pfreq);
      break;
    case 5:
      return (lfo2.PLFOtype);
      break;
    case 6:
      return (lfo2.Pstereo);
      break;
    case 7:
      return (Cross1);
      break;
    case 8:
      return (Cross2);
      break;
    case 9:
      return (Cross3);
      break;
    };
  return (0);			//in case of bogus parameter number
};

