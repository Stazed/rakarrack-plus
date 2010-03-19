/*
  ZynAddSubFX - a software synthesizer
 
  SV Filter.h - Several state-variable filters
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

#ifndef RB_FILTER_H
#define RB_FILTER_H

#include "global.h"
#include "Filter_.h"
class RBFilter:public Filter_
{
public:
  RBFilter (unsigned char Ftype, float Ffreq, float Fq,
	    unsigned char Fstages);
   ~RBFilter ();
  void filterout (float * smp);
  void setfreq (float frequency);
  void setfreq_and_q (float frequency, float q_);
  void setq (float q_);

  void settype (int type_);
  void setgain (float dBgain);
  void setstages (int stages_);
  void setmix (int mix, float lpmix, float bpmix, float hpmix);
  void cleanup ();

private:
  struct fstage
  {
    float low, high, band, notch;
  } st[MAX_FILTER_STAGES + 1];

  struct parameters
  {
    float f, q, q_sqrt;
  } par, ipar;


  void singlefilterout (float * smp, fstage & x, parameters & par);
  void computefiltercoefs ();
  int type;			//The type of the filter (LPF1,HPF1,LPF2,HPF2...)
  int stages;			//how many times the filter is applied (0->1,1->2,etc.)
  float freq;		//Frequency given in Hz
  float q;			//Q factor (resonance or Q factor)
  float gain;		//the gain of the filter (if are shelf/peak) filters
  float hpg, lpg, bpg;
  float oldq, oldsq, oldf;
  float iper;			//inverse of PERIOD

  int abovenq;			//this is 1 if the frequency is above the nyquist
  int oldabovenq;
  int needsinterpolation, firsttime, en_mix;
};


#endif
