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
  ml = new AnalogFilter (6, 2200.0f,.3f, 0);
  mr = new AnalogFilter (6, 2200.0f,.3f, 0);


  //default values
  Ppreset = 0;
  Pvolume = 50;
  PvolL = 0;
  PvolM = 0;
  PvolH = 0;

  setpreset (Ppreset);
  cleanup ();
};

Shuffle::~Shuffle ()
{
  delete (ll);
  delete (lr);
  delete (hl);
  delete (hr);
  delete (ml);
  delete (mr);
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
  ml->cleanup ();
  mr->cleanup ();

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
	};
 

  ll->filterout(inputl);
  lr->filterout(inputr);
  ml->filterout(inputl);
  mr->filterout(inputr);
  hl->filterout(inputl);
  hr->filterout(inputr);
  



  for (i = 0; i < PERIOD; i++)
  {
    efxoutl[i]=inputl[i]+inputr[i]+smpsl[i];
    efxoutr[i]=inputl[i]-inputr[i]+smpsr[i];
  }      
    
 


    
};


/*
 * Parameter control
 */
void
Shuffle::setvolume (int value)
{
  Pvolume = value;
  outvolume = (float)Pvolume / 127.0f;
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
  ml->setfreq ((float)value);
  mr->setfreq ((float)value);

};

void
Shuffle::setCross3 (int value)
{
  Cross3 = value;
  hl->setfreq ((float)value);
  hr->setfreq ((float)value);

};




void
Shuffle::setpreset (int npreset)
{
  const int PRESET_SIZE = 7;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Shuffle 1
    {64, 10, 0, 0, 600, 2000, 6000},
    //Shuffle 2
    {64, 0, 0, 0, 120, 2400, 8000},
    //Shuffle 3
    {64, 0, 0, 0, 60, 3700, 12000},
    //Shuffle 4
    {64, 0, 0, 0, 100, 1200, 5600}

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
      PvolL = value+64;
      volL = 30.0f * ((float)PvolL - 64.0f) / 64.0f;
      ll->setgain(volL);
      volLr = 30.0f * ((float)-1.0f*PvolL - 64.0f) / 64.0f;
      lr->setgain(volLr);
      break;
    case 2:
      PvolM = value+64;
      volM = 30.0f * ((float)PvolM - 64.0f) / 64.0f;;
      ml->setgain(volM);
      volMr= 30.0f * ((float)-1.0f*PvolM - 64.0f) / 64.0f;;
      mr->setgain(volMr);
      break;
    case 3:
      PvolH = value+64;
      volH = 30.0f * ((float)PvolH - 64.0f) / 64.0f;;
      hl->setgain(volH);
      volHr = 30.0f * ((float)-1.0f*PvolH - 64.0f) / 64.0f;;
      hr->setgain(volHr);
      break;
    case 4:
      setCross1 (value);
      break;
    case 5:
      setCross2 (value);
      break;
    case 6:
      setCross3 (value);
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
      return (PvolM-64);
      break;
    case 3:
      return (PvolH-64);
      break;
    case 4:
      return (Cross1);
      break;
    case 5:
      return (Cross2);
      break;
    case 6:
      return (Cross3);
      break;
    };
  return (0);			//in case of bogus parameter number
};

