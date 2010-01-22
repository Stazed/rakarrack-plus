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
  void setpreset (unsigned char npreset);
  void changepar (int npar, unsigned char value);
  unsigned char getpar (int npar);
  void cleanup ();
  void applyfilters (REALTYPE * efxoutl);


  unsigned char Ppreset;
  unsigned char Pinterval;
  unsigned char PMIDI;
  unsigned char PSELECT;
  int mira;

  float *efxoutl;
  float *efxoutr;
  float *outi;
  float *outo;

  float outvolume;
  float volume;

  long int hq;


private:

  unsigned char Pvolume;
  unsigned char Pgain;
  unsigned char Ppan;
  unsigned char Pnote;
  unsigned char Ptype;

  unsigned char fPfreq;
  unsigned char fPgain;
  unsigned char fPq;

  float panning;
  float gain;
  float interval;

  void setvolume (unsigned char Pvolume);
  void setpanning (unsigned char Ppan);
  void setinterval (unsigned char Pinterval);
  void setgain (unsigned char Pgain);
  void setMIDI (unsigned char PMIDI);
  void fsetfreq (unsigned char value);
  void fsetgain (unsigned char value);
  void fsetq (unsigned char value);


  AnalogFilter *pl;
  PitchShifter *PS;

};

#endif
