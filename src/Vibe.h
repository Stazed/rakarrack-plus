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
  
  float Ra, Rb, R1, b, dTC, dRCl, dRCr, minTC, alphal, alphar, stepl, stepr, oldstepl, oldstepr, fdepth;
  float lstep,rstep;
  float cperiod;
  float gl, oldgl;
  float gr, oldgr; 
  float rpanning, lpanning; 
  EffectLFO lfo;

  
  struct circuit {
  float R1;  //Collector and emitter resistor == 4.7k
  float Rv;  //LDR
  float C1;  //dominant pole
  float C2;  //DC blocking
  float alpha; //Transistor gain term
  } vibeck[4]; //4 stages
  
  struct fparams {
  float x1;
  float y1;
  //filter coefficients
  float n0;
  float n1;
  float d0;
  float d1; 
  } vc[4], vcvo[4], vevo[4];

  float vibefilter(float data, fparams *ftype, int stage); 

};

#endif
