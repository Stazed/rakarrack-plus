/*
  rakarrack - a guitar effects software

  Harmonizer.h  -  Harmonizer definitions
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

#ifndef HARMONIZER_H
#define HARMONIZER_H

#include "global.h"
#include "AnalogFilter.h"
#include "smbPitchShift.h"


class Harmonizer
{

public:
  Harmonizer (float *efxoutl_, float *efxoutr_, long int Quality);
   ~Harmonizer ();
  void out (float *smpsl, float *smpsr);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();
  void applyfilters (float * efxoutl);


  int Ppreset;
  int Pinterval;
  int PMIDI;
  int PSELECT;
  int mira;

  float *efxoutl;
  float *efxoutr;
  float *outi;
  float *outo;

  float outvolume;
  float volume;

  long int hq;


private:

  int Pvolume;
  int Pgain;
  int Ppan;
  int Pnote;
  int Ptype;

  int fPfreq;
  int fPgain;
  int fPq;

  float panning;
  float gain;
  float interval;

  void setvolume (int Pvolume);
  void setpanning (int Ppan);
  void setinterval (int Pinterval);
  void setgain (int Pgain);
  void setMIDI (int PMIDI);
  void fsetfreq (int value);
  void fsetgain (int value);
  void fsetq (int value);


  AnalogFilter *pl;
  PitchShifter *PS;

};

#endif
