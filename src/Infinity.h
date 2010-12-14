/*
  Rakarrack Guitar Effects Processor
  
  InfiniT : Infinitely rising & falling filter effect
  Author:  Ryan Billing (aka Transmogrifox)
  Copyright (C) 2010

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

#ifndef INIFINITY_H
#define INFINITY_H
#include "global.h"
#include "EffectLFO.h"
#include "RBFilter.h"

#define NUM_INF_BANDS 8

class Infinity
{
public:
  Infinity (float * efxoutl_, float * efxoutr_);
  ~Infinity ();
  void out (float * smpsl, float * smpsr);

  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();


  int Ppreset;
  float outvolume;

  float *efxoutl;
  float *efxoutr;


private:

  void setvolume (int Pvolume);
  void oscillator();
  void reinitfilter ();
	  
  int Pvolume;		//For wet/dry mix
  int Pb[NUM_INF_BANDS];          //Pbx -64 to 64// adjusts mix of each filter band
  int Pq;              //0 to 127// filter resonance
  int Pcenter;         //300 to 8000// where the filter is centered
  int Pspread;	       //300 to 8000// how wide of area covered by filter range
  int Prate;           //0 to 127// how fast it sweeps

  struct filterstate {
  float sinp;
  float cosp;      //sinusoid oscillator state variables
  float ramp;      //frequency oscillator
  float level;
  float lfo;
  float vol;
  } bandstate[NUM_INF_BANDS];
  
  /*
  fconst = freq* 2*pi/fs;
  rconst = 2*freq/fs;
 */
  float fconst;  //sine wave oscillator frequency constant // 2*pi*freq/fs 
  float rampconst;  //ramp oscillator constant// 2*freq/SR  
  float powconst;  //log(1 + spread/centerfreq)/log(2)

  class RBFilter *filterl[NUM_INF_BANDS], *filterr[NUM_INF_BANDS];

  class FPreset *Fpre;


};

#endif
