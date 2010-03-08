/*
  ZynAddSubFX - a software synthesizer
 
  RyanWah.h - "WahWah" effect and others
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Ryan Billing

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

#ifndef RYANWAH_H
#define RYANWAH_H
#include "global.h"
#include "EffectLFO.h"
#include "SVFilter.h"

class RyanWah
{
public:
  RyanWah (float * efxoutl_, float * efxoutr_);
  ~RyanWah ();
  void out (float * smpsl, float * smpsr);

  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();

//      void setdryonly();


  int Ppreset;
  float *efxoutl;
  float *efxoutr;
  float outvolume;
  float volume;


private:
  //Parametrii RyanWah
    EffectLFO lfo;		//lfo-ul RyanWah
  int Pvolume;
  int Ppanning;
  int Pdepth;		//the depth of the lfo of the RyanWah
  int Pampsns;	//how the filter varies according to the input amplitude
  int Pampsnsinv;	//if the filter freq is lowered if the input amplitude rises
  int Pampsmooth;	//how smooth the input amplitude changes the filter

  //Control Parametrii
  void setvolume (int Pvolume);
  void setpanning (int Ppanning);
  void setdepth (int Pdepth);
  void setampsns (int Pampsns);

  void reinitfilter ();

  //Valorile interne

  float rpanning, lpanning, depth, ampsns, ampsmooth, fbias, oldfbias, oldfbias1, oldfbias2, q, frequency, maxfreq, minfreq;
  unsigned char Ftype;
  unsigned char Fstages;
  class SVFilter *filterl, *filterr;

  float ms1;	//mean squares



};

#endif
