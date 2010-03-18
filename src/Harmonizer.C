/*
  rakarrack - a guitar effects software

  Harmonizer.C  -  Harmonizer
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


#include "Harmonizer.h"



Harmonizer::Harmonizer (float *efxoutl_, float *efxoutr_, long int Quality)
{



  efxoutl = efxoutl_;
  efxoutr = efxoutr_;
  hq = Quality;

  outi = (float *) malloc (sizeof (float) * PERIOD);
  outo = (float *) malloc (sizeof (float) * PERIOD);

  memset (outi, 0, sizeof (float) * PERIOD);
  memset (outo, 0, sizeof (float) * PERIOD);

  pl = new AnalogFilter (6, 22000, 1, 0);

  PS = new PitchShifter (512, hq, fSAMPLE_RATE);
  PS->ratio = 1.0f;

  Ppreset = 0;
  PMIDI = 0;
  mira = 0;
  setpreset (Ppreset);


  cleanup ();

};



Harmonizer::~Harmonizer ()
{

  delete[]outi;
  delete[]outo;
  delete pl;
  delete PS;

};

void
Harmonizer::cleanup ()
{
  int i;
  mira = 0;

  for (i = 0; i < PERIOD; i++)
    {
      outi[i] = 0.0;
      outo[i] = 0.0;
    }



};


void
Harmonizer::applyfilters (float * efxoutl)
{
  pl->filterout (efxoutl);
};



void
Harmonizer::out (float *smpsl, float *smpsr)
{

  int i;

  for (i = 0; i < PERIOD; i++)
    {
      outi[i] = (smpsl[i] + smpsr[i])*.5;
      if (outi[i] > 1.0)
	outi[i] = 1.0f;
      if (outi[i] < -1.0)
	outi[i] = -1.0f;

    }

  if ((PMIDI) || (PSELECT))
    PS->ratio = r__ratio;

  if (Pinterval != 12)
    {
      PS->smbPitchShift (PS->ratio, PERIOD, 512, hq, fSAMPLE_RATE, outi, outo);

      applyfilters (outo);

      for (i = 0; i < PERIOD; i++)
	{
	  efxoutl[i] = outo[i] * gain * panning;
	  efxoutr[i] = outo[i] * gain * (1.0f - panning);
	}

    }

};



void
Harmonizer::setvolume (int value)
{
  this->Pvolume = value;
  volume = outvolume = (float)Pvolume / 127.0f;
};



void
Harmonizer::setpanning (int value)
{
  this->Ppan = value;
  panning = (float)Ppan / 127.0f;
};



void
Harmonizer::setgain (int value)
{
  this->Pgain = value;
  gain = (float)Pgain / 127.0f;
  gain *=2.0;
};


void
Harmonizer::setinterval (int value)
{

  this->Pinterval = value;
  interval = (float)Pinterval - 12.0f;
  PS->ratio = powf (2.0f, interval / 12.0f);
  if (Pinterval % 12 == 0)
    mira = 0;
  else
    mira = 1;

};

void
Harmonizer::fsetfreq (int value)
{

  fPfreq = value;
  float tmp = (float)value;
  pl->setfreq (tmp);
}

void
Harmonizer::fsetgain (int value)
{

  float tmp;

  this->fPgain = value;
  tmp = 30.0f * ((float)value - 64.0f) / 64.0f;
  pl->setgain (tmp);

}

void
Harmonizer::fsetq (int value)
{

  float tmp;
  this->fPq = value;
  tmp = powf (30.0f, ((float)value - 64.0f) / 64.0f);
  pl->setq (tmp);

}

void
Harmonizer::setMIDI (int value)
{

  this->PMIDI = value;
}




void
Harmonizer::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Plain
    {64, 64, 64, 12, 6000, 0, 0, 0, 64, 64, 0},
    //Octavador
    {64, 64, 64, 0, 6000, 0, 0, 0, 64, 64, 0},
    //3mdown
    {64, 64, 64, 9, 6000, 0, 0, 0, 64, 64, 0}
  };

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;


};



void
Harmonizer::changepar (int npar, int value)
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
      setgain (value);
      break;
    case 3:
      setinterval (value);
      break;
    case 4:
      fsetfreq (value);
      break;
    case 5:
      PSELECT = value;;
      break;
    case 6:
      Pnote = value;
      break;
    case 7:
      Ptype = value;
      break;
    case 8:
      fsetgain (value);
      break;
    case 9:
      fsetq (value);
      break;
    case 10:
      setMIDI (value);
      break;



    }


};


int
Harmonizer::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppan);
      break;
    case 2:
      return (Pgain);
      break;
    case 3:
      return (Pinterval);
      break;
    case 4:
      return (fPfreq);
      break;
    case 5:
      return (PSELECT);
      break;
    case 6:
      return (Pnote);
      break;
    case 7:
      return (Ptype);
      break;
    case 8:
      return (fPgain);
      break;
    case 9:
      return (fPq);
      break;
    case 10:
      return (PMIDI);
      break;
    default:
      return (0);

    }

};
