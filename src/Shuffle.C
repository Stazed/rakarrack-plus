/*
  Shuffle.C - Distorsion effect
  
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
#include "Distorsion.h"
#include "Shuffle.h"

/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */



Shuffle::Shuffle (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  inputl = (float *) malloc (sizeof (float) * PERIOD);
  inputr = (float *) malloc (sizeof (float) * PERIOD);


  ll = new AnalogFilter (6, 300.0f, .3f, 0);
  lr = new AnalogFilter (6, 300.0f, .3f, 0);
  hl = new AnalogFilter (6, 8000.0f,.3f, 0);
  hr = new AnalogFilter (6, 8000.0f,.3f, 0);
  mll = new AnalogFilter (6, 1200.0f,.3f, 0);
  mlr = new AnalogFilter (6, 1200.0f,.3f, 0);
  mhl = new AnalogFilter (6, 2400.0f,.3f, 0);
  mhr = new AnalogFilter (6, 2400.0f,.3f, 0);


  //default values
  Ppreset = 0;
  Pvolume = 50;
  PvolL = 0;
  PvolML = 0;
  PvolMH = 0;
  PvolH = 0;
  E=0;
  Rev=1.0f;
  setpreset (Ppreset);
  cleanup ();
};

Shuffle::~Shuffle ()
{
  delete (ll);
  delete (lr);
  delete (hl);
  delete (hr);
  delete (mll);
  delete (mlr);
  delete (mhl);
  delete (mhr);


};

/*
 * Cleanup the effect
 */
void
Shuffle::cleanup ()
{
  ll->cleanup ();
  hl->cleanup ();
  lr->cleanup ();
  hr->cleanup ();
  mll->cleanup ();
  mlr->cleanup ();
  mhl->cleanup ();
  mhr->cleanup ();

};
/*
 * Effect output
 */
void
Shuffle::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;
 
      for (i = 0; i < PERIOD; i++)
	{
  	  inputl[i] = smpsl[i] + smpsr[i];
	  inputr[i] = smpsl[i] - smpsr[i];        
        }
        
  ll->filterout(inputl);
  lr->filterout(inputr);
  mll->filterout(inputl);
  mlr->filterout(inputr);
  mhl->filterout(inputl);
  mhr->filterout(inputr);
  hl->filterout(inputl);
  hr->filterout(inputr);
  

  for (i = 0; i < PERIOD; i++)
  {
    efxoutl[i]=(inputl[i]+inputr[i])*.5f;
    efxoutr[i]=(inputl[i]-inputr[i])*-.5f;
  }      
    
    
};


/*
 * Parameter control
 */
void
Shuffle::setvolume (int value)
{
  Pvolume = value;
  outvolume = (float)Pvolume / 128.0f;
};

void
Shuffle::setCross1 (int value)
{
  Cross1 = value;
  ll->setfreq ((float)value);
  lr->setfreq ((float)value);

};

void
Shuffle::setCross2 (int value)
{
  Cross2 = value;
  mll->setfreq ((float)value);
  mlr->setfreq ((float)value);

};


void
Shuffle::setCross3 (int value)
{
  Cross3 = value;
  mhl->setfreq ((float)value);
  mhr->setfreq ((float)value);

};

void
Shuffle::setCross4 (int value)
{
  Cross4 = value;
  hl->setfreq ((float)value);
  hr->setfreq ((float)value);

};

void
Shuffle::setGainL(int value)
{
      PvolL = value+64;
      volL = 30.0f * ((float)PvolL - 64.0f) / 64.0f;
      ll->setgain(volL);
      volLr = Rev*volL;
      lr->setgain(volLr);
}      

void
Shuffle::setGainML(int value)
{
      PvolML = value+64;
      volML = 30.0f * ((float)PvolML - 64.0f) / 64.0f;;
      mll->setgain(volML);
      volMLr= Rev*volML;
      mlr->setgain(volMLr);
}

void
Shuffle::setGainMH(int value)
{
      PvolMH = value+64;
      volMH = 30.0f * ((float)PvolMH - 64.0f) / 64.0f;;
      mhl->setgain(volMH);
      volMHr= Rev*volMH;
      mhr->setgain(volMHr);
}

void
Shuffle::setGainH(int value)
{
      PvolH = value+64;
      volH = 30.0f * ((float)PvolH - 64.0f) / 64.0f;;
      hl->setgain(volH);
      volHr = Rev*volH;
      hr->setgain(volHr);
}


void
Shuffle::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Shuffle 1
    {64, 10, 0, 0, 0,600, 1200,2000, 6000,-14, 1},
    //Shuffle 2
    {64, 0, 0, 0, 0, 120, 1000,2400, 8000,-7, 1},
    //Shuffle 3
    {64, 0, 0, 0, 0, 60, 1800, 3700, 12000, 7, 0},
    //Shuffle 4
    {64, 0, 0, 0, 0, 100, 1200, 2400 , 5600, 14, 0}

  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();
};


void
Shuffle::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setGainL(value);
      break;
    case 2:
      setGainML(value);
      break;
    case 3:
      setGainMH(value);
      break;
    case 4:
      setGainH(value);
      break;
    case 5:
      setCross1 (value);
      break;
    case 6:
      setCross2 (value);
      break;
    case 7:
      setCross3 (value);
      break;
    case 8:
      setCross4 (value);
      break;
    case 9:
      PQ = value;
      value +=64;
      tmp = powf (30.0f, ((float)value - 64.0f) / 64.0f);
      ll->setq(tmp);
      lr->setq(tmp);
      mll->setq(tmp);
      mlr->setq(tmp);
      mhl->setq(tmp);
      mhr->setq(tmp);
      hl->setq(tmp);
      hr->setq(tmp);
      break;
    case 10:
       E=value;
       if(E) Rev=-1.0f; else Rev=1.0f;
       setGainL(getpar(1));
       setGainML(getpar(2));
       setGainMH(getpar(3));
       setGainH(getpar(4));
       break;  
    };
};

int
Shuffle::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (PvolL-64);
      break;
    case 2:
      return (PvolML-64);
      break;
    case 3:
      return (PvolMH-64);
      break;
    case 4:
      return (PvolH-64);
      break;
    case 5:
      return (Cross1);
      break;
    case 6:
      return (Cross2);
      break;
    case 7:
      return (Cross3);
      break;
    case 8:
      return (Cross4);
      break;
    case 9:
      return (PQ);
      break;
    case 10:
      return (E);  
      
    };
  return (0);			//in case of bogus parameter number
};

