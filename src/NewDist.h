/*
  ZynAddSubFX - a software synthesizer
 
  Distorsion.h - Distorsion Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu & Hernan Ordiales
  
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

#ifndef NEWDIST_H
#define NEWDIST_H

#include "global.h"
#include "AnalogFilter.h"


//Waveshaping(called by Distorsion effect and waveshape from OscilGen)
void waveshapesmps (int n, REALTYPE * smps, unsigned char type,
		    unsigned char drive);

class NewDist
{
public:
  NewDist (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~NewDist ();
  void out (REALTYPE * smpsl, REALTYPE * smpr);
  void setpreset (unsigned char npreset);
  void changepar (int npar, unsigned char value);
  unsigned char getpar (int npar);
  void cleanup ();
  void applyfilters (REALTYPE * efxoutl, REALTYPE * efxoutr);

  unsigned char Ppreset;
  REALTYPE *efxoutl;
  REALTYPE *efxoutr;
  REALTYPE outvolume;
  REALTYPE volume;
  REALTYPE inpll[4096];
  REALTYPE inplr[4096];

private:
  //Parametrii
  unsigned char Pvolume;	//Volumul or E/R
  unsigned char Ppanning;	//Panning
  unsigned char Plrcross;	// L/R Mixing
  unsigned char Pdrive;		//the input amplification
  unsigned char Plevel;		//the ouput amplification
  unsigned char Ptype;		//Distorsion type
  unsigned char Pnegate;	//if the input is negated 
  unsigned char Plpf;		//lowpass filter
  unsigned char Phpf;		//highpass filter
  unsigned char Prfreq;	        
  unsigned char Pprefiltering;	//if you want to do the filtering before the distorsion
  unsigned char Poctave;	//mix sub octave


  float rfreq;

  void setvolume (unsigned char Pvolume);
  void setpanning (unsigned char Ppanning);
  void setlrcross (unsigned char Plrcross);
  void setoctave (unsigned char Poctave);
  void setlpf (unsigned char Plpf);
  void sethpf (unsigned char Phpf);

  //Parametrii reali
  REALTYPE panning, lrcross, octave_memoryl, togglel, octave_memoryr, toggler, octmix;
  AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr, *blockDCl, *blockDCr, *DCl, *DCr;

  float *octoutl, *octoutr;

  class Filter *filterl, *filterr;
  class FilterParams *filterpars;  

};


#endif
