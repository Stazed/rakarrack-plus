// Based in gate_1410.c LADSPA Swh-plugins


/*
  rakarrack - a guitar effects software

 Gate.C  -  Noise Gate Effect
 
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <math.h>
#include "Gate.h"


Gate::Gate (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{

  efxoutl = efxoutl_;
  efxoutr = efxoutr_;


  lpfl = new AnalogFilter (2, 22000, 1, 0);
  lpfr = new AnalogFilter (2, 22000, 1, 0);
  hpfl = new AnalogFilter (3, 20, 1, 0);
  hpfr = new AnalogFilter (3, 20, 1, 0);

  env = 0.0;
  gate = 0.0;
  fs = (float)SAMPLE_RATE;
  state = CLOSED;
  hold_count = 0;




}

Gate::~Gate ()
{



}



void
Gate::cleanup ()
{
  lpfl->cleanup ();
  hpfl->cleanup ();
  lpfr->cleanup ();
  hpfr->cleanup ();

}




void
Gate::setlpf (int Plpf)
{
  this->Plpf = Plpf;
  REALTYPE fr = expf (powf ((float)Plpf / 127.0f, 0.5f) * logf (25000.0f)) + 40.0f;
  lpfl->setfreq (fr);
  lpfr->setfreq (fr);
};

void
Gate::sethpf (int Phpf)
{
  this->Phpf = Phpf;
  REALTYPE fr = expf (powf ((float)Phpf / 127.0f, 0.5f) * logf (25000.0f)) + 20.0f;
  hpfl->setfreq (fr);
  hpfr->setfreq (fr);
};


void
Gate::Gate_Change (int np, int value)
{

  switch (np)
    {

    case 1:
      Pthreshold = value - 20;
      t_level = dB2rap ((float)Pthreshold);
      break;
    case 2:
      Prange = value;
      cut = dB2rap ((float)Prange);
      break;
    case 3:
      Pattack = value;
      a_rate = (float)Pattack /(fs*1000.0f);
      break;
    case 4:
      Pdecay = value;
      d_rate = (float)Pdecay /(fs*1000.0f);
      break;
    case 5:
      setlpf(value);
      break;
    case 6:
      sethpf(value);
      break;
    case 7:
      Phold = value;
      hold = (float)Phold;
      break;

    }


}

int
Gate::getpar (int np)
{

  switch (np)

    {
    case 1:
      return (Pthreshold);
      break;
    case 2:
      return (Prange);
      break;
    case 3:
      return (Pattack);
      break;
    case 4:
      return (Pdecay);
      break;
    case 5:
      return (Plpf);
      break;
    case 6:
      return (Phpf);
      break;
    case 7:
      return (Phold);
      break;

    }

  return (0);

}


void
Gate::Gate_Change_Preset (int npreset)
{

  const int PRESET_SIZE = 7;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //0
    {0, 0, 1, 2, 96, 20, 2},
    //-10
    {0, -10, 1, 2, 96, 20, 2},
    //-20
    {0, -20, 1, 2, 96, 20, 2}
  };

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    Gate_Change (n + 1, presets[npreset][n]);


}



void
Gate::out (float *efxoutl, float *efxoutr)
{


  int i;
  float delta = 0.0f;
  float expenv = 0.0f;


  lpfl->filterout (efxoutl);
  hpfl->filterout (efxoutl);
  lpfr->filterout (efxoutr);
  hpfr->filterout (efxoutr);


  for (i = 0; i < PERIOD; i++)
    {
    
      delta = 0.5f*(fabsf (efxoutl[i]) + fabsf (efxoutr[i])) - env;    //envelope follower from Compressor.C
      if (delta > 0.0)
	env += a_rate * delta;
      else
	env += d_rate * delta;
	
	if (env > 2.0f) env = 2.0f;
	
	expenv = t_level * expf(env);
	if (expenv > 1.0f) expenv = 1.0f;
		




      efxoutl[i] *= expenv;
      efxoutr[i] *= expenv;

    }



};
