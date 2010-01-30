/*
    HarnEnhancer - Harmonic Enhancer Class                                          
    
    HarmEnhancer.h  - headers.
    Copyright (C) 2008-2010 Josep Andreu               
    Author: Josep Andreu 


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

#ifndef HARM_ENHANCER_H
#define HARM_ENHANCER_H

#include "global.h"
#include "AnalogFilter.h"

#define HARMONICS 11


class HarmEnhancer
{
public:
  HarmEnhancer(float *harmonics, float freq, float gain);
  ~HarmEnhancer(); 
   void chebpc(float c[], float d[]);
   void calcula_mag(float *Rmag);
   void harm_out(float *smpsl, float *smpsr);
   void set_vol(float gain);
   void set_freq(float freq); 


private:

  float *inputl;
  float *inputr;

  float itm1l;
  float itm1r;
  float otm1l;
  float otm1r;
  float vol;
  
  float p[HARMONICS];
  
  AnalogFilter *hpfl, *hpfr;
    
};

#endif
