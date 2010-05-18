/*
  MBDist.C - Distorsion effect
  
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
#include "MBDist.h"

/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */



MBDist::MBDist (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  lowl = (float *) malloc (sizeof (float) * PERIOD);
  lowr = (float *) malloc (sizeof (float) * PERIOD);
  midl = (float *) malloc (sizeof (float) * PERIOD);
  midr = (float *) malloc (sizeof (float) * PERIOD);
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

  mbwshape1 = new Waveshaper();
  mbwshape2 = new Waveshaper();  
  mbwshape3 = new Waveshaper(); 
  
  //default values
  Ppreset = 0;
  Pvolume = 50;
  Plrcross = 40;
  Pdrive = 90;
  Plevel = 64;
  PtypeL = 0;
  PtypeM = 0;
  PtypeH = 0;
  PvolL = 0;
  PvolM = 0;
  PvolH = 0;
  Pnegate = 0;
  Pstereo = 0;

  setpreset (Ppreset);
  cleanup ();
};

MBDist::~MBDist ()
{
};

/*
 * Cleanup the effect
 */
void
MBDist::cleanup ()
{
  lpf1l->cleanup ();
  hpf1l->cleanup ();
  lpf1r->cleanup ();
  hpf1r->cleanup ();
  lpf2l->cleanup ();
  hpf2l->cleanup ();
  lpf2r->cleanup ();
  hpf2r->cleanup ();

};
/*
 * Effect output
 */
void
MBDist::out (float * smpsl, float * smpsr)
{
  int i;
  float l, r, lout, rout;

  float inputvol = powf (5.0f, ((float)Pdrive - 32.0f) / 127.0f);
  if (Pnegate != 0)
    inputvol *= -1.0f;

      for (i = 0; i < PERIOD; i++)
	{
	  efxoutl[i] = smpsl[i] * inputvol * 2.0f;
	  efxoutr[i] = smpsr[i] * inputvol * 2.0f;        
	};
 
  memcpy(lowl,efxoutl,sizeof(float) * PERIOD);
  memcpy(midl,efxoutl,sizeof(float) * PERIOD);
  memcpy(highl,efxoutl,sizeof(float) * PERIOD);

  lpf1l->filterout(lowl);
  hpf1l->filterout(midl);
  lpf2l->filterout(midl);
  hpf2l->filterout(highl);

  mbwshape1->waveshapesmps (PERIOD, lowl, PtypeL, PdriveL, 1);
  mbwshape2->waveshapesmps (PERIOD, midl, PtypeM, PdriveM, 1);
  mbwshape3->waveshapesmps (PERIOD, highl, PtypeH, PdriveH, 1);


if (Pstereo)
{
  memcpy(lowr,efxoutr,sizeof(float) * PERIOD);
  memcpy(midr,efxoutr,sizeof(float) * PERIOD);
  memcpy(highr,efxoutr,sizeof(float) * PERIOD);

  lpf1r->filterout(lowr);
  hpf1r->filterout(midr);
  lpf2r->filterout(midr);
  hpf2r->filterout(highr);
  
  mbwshape1->waveshapesmps (PERIOD, lowr, PtypeL, PdriveL, 1);
  mbwshape2->waveshapesmps (PERIOD, midr, PtypeM, PdriveM, 1);
  mbwshape3->waveshapesmps (PERIOD, highr, PtypeH, PdriveH, 1);


}

  for (i = 0; i < PERIOD; i++)
  {
    efxoutl[i]=lowl[i]*volL+midl[i]*volM+highl[i]*volH;
    if (Pstereo) efxoutr[i]=lowr[i]*volL+midr[i]*volM+highr[i]*volH;
  }      
    
  if (!Pstereo) memcpy(efxoutr, efxoutl, sizeof(float)* PERIOD);    
 

  float level = dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);

  for (i = 0; i < PERIOD; i++)
    {
      lout = efxoutl[i];
      rout = efxoutr[i];
      
      l = lout * (1.0f - lrcross) + rout * lrcross;
      r = rout * (1.0f - lrcross) + lout * lrcross;
      lout = l;
      rout = r;  
       
      efxoutl[i] = lout * 2.0f * level * panning;
      efxoutr[i] = rout * 2.0f * level * (1.0f -panning);  

    };
    
};


/*
 * Parameter control
 */
void
MBDist::setvolume (int value)
{
  Pvolume = value;
  outvolume = (float)Pvolume / 127.0f;
};

void
MBDist::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};


void
MBDist::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 127.0f * 1.0f;
};

void
MBDist::setCross1 (int value)
{
  Cross1 = value;
  lpf1l->setfreq ((float)value);
  lpf1r->setfreq ((float)value);
  hpf1l->setfreq ((float)value);
  hpf1r->setfreq ((float)value);


};

void
MBDist::setCross2 (int value)
{
  Cross2 = value;
  hpf2l->setfreq ((float)value);
  hpf2r->setfreq ((float)value);
  lpf2l->setfreq ((float)value);
  lpf2r->setfreq ((float)value);

  
};


void
MBDist::setpreset (int npreset)
{
  const int PRESET_SIZE = 15;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Dist 1
    {14, 64, 64, 20, 90, 0, 14, 13, 38, 49, 40, 0, 288, 1315, 1},
    //Soft
    {0, 64, 64, 32, 81, 6, 13, 6, 58, 63, 61, 0, 364, 1708, 1},
    //Modulated
    {0, 64, 64, 36, 77, 18, 17, 18, 66, 69, 54, 0, 504, 2730, 1},
    //Crunch
    {0, 64, 64, 30, 66, 15, 14, 15, 44, 60, 42, 0, 385, 2730, 1}


  };

  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();
};


void
MBDist::changepar (int npar, int value)
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
      setlrcross (value);
      break;
    case 3:
      Pdrive = value;
      PdriveL = (int)((float)Pdrive*volL);
      PdriveM = (int)((float)Pdrive*volM);
      PdriveH = (int)((float)Pdrive*volH);
      break;
    case 4:
      Plevel = value;
      break;
    case 5:
      PtypeL = value;
      break;
    case 6:
      PtypeM = value;
      break;
    case 7:
      PtypeH = value;
      break;
    case 8:
      PvolL = value;
      volL = (float) value /100.0;
      PdriveL = (int)((float)Pdrive*volL);
      break;
    case 9:
      PvolM = value;
      volM = (float) value /100.0;
      PdriveM = (int)((float)Pdrive*volM);
      break;
    case 10:
      PvolH = value;
      volH = (float) value /100.0;
      PdriveH = (int)((float)Pdrive*volH);
      break;
    case 11:
      Pnegate = value;
      break;
    case 12:
      setCross1 (value);
      break;
    case 13:
      setCross2 (value);
      break;
    case 14:
      Pstereo = value;
      break;
    };
};

int
MBDist::getpar (int npar)
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
      return (Plrcross);
      break;
    case 3:
      return (Pdrive);
      break;
    case 4:
      return (Plevel);
      break;
    case 5:
      return (PtypeL);
      break;
    case 6:
      return (PtypeM);
      break;
    case 7:
      return (PtypeH);
      break;
    case 8:
      return (PvolL);
      break;
    case 9:
      return (PvolM);
      break;
    case 10:
      return (PvolH);
      break;
    case 11:
      return (Pnegate);
      break;
    case 12:
      return (Cross1);
      break;
    case 13:
      return (Cross2);
      break;
    case 14:
      return (Pstereo);
      break;
    };
  return (0);			//in case of bogus parameter number
};

