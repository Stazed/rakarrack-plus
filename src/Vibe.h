/*
  rakarrack - a guitar effects software

 Vibe.h  -  Vibe Effect definitions

  Copyright (C) 2008-2010 Ryan Billing
  Author: Josep Andreu & Ryan Billing

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


#ifndef Vibe_H
#define Vibe_H

#include "global.h"
#include "EffectLFO.h"

class Vibe
{

public:

  Vibe (float * efxoutl_, float * efxoutr_);
  ~Vibe ();

  void out (float * smpsl, float * smpsr);
  void setvolume(int value);
  void setpanning(int value);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();

  float outvolume;
  float *efxoutl;
  float *efxoutr;


private:
  int Pdepth;
  int Ppanning;
  int Pvolume;
  
  float Ra, Rb, b, dTC, dRCl, dRCr, minTC, alphal, alphar, stepl, stepr, oldstepl, oldstepr, fdepth;
  float dalphal, dalphar;
  float lstep,rstep;
  float cperiod;
  float gl, oldgl;
  float gr, oldgr; 
  float rpanning, lpanning; 
  EffectLFO lfo;
  
  struct fparams {
  float x1;
  float y1;
  //filter coefficients
  float n0;
  float n1;
  float d0;
  float d1; 
  } vc[8], vcvo[8], vevo[8], bootstrap[8];

  float vibefilter(float data, fparams *ftype, int stage); 
  void init_vibes();
  void modulate(float ldr);
  float bjt_shape(float data);

float R1;
float Rv;
float C2;
float C1[4];
float beta;  //transistor forward gain.
float gain, k;
float en1[4], en0[4], ed1[4], ed0[4];
float cn1[4], cn0[4], cd1[4], cd0[4];
float on1[4], on0[4], od1[4], od0[4];


};

#endif
