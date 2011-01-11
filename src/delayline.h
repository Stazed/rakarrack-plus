/*
  Rakarrack Guitar FX
 
  delayline.h - An interpolated delay line.  Input new sample and desired delay time for output.  
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License 
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef DLINE_H
#define DLINE_H

#include "global.h"


class delayline
{
public:
  delayline (float maxdelay, int maxtaps_);  //construct the object with intended maximum delay time
  ~delayline ();
  void cleanup (); 
  void set_averaging();  //use this if you want the time change averaging longer or shorter
  float delay (float smps, float time, int tap_, int touch);  //puts a new sample at the current pointer

private:
  int zero_index; 
  int tap, maxtaps;
  float maxtime;
  long maxdelaysmps;
 
  float *oldtime, *avgtime;     //keeping it from changing too quickly
  float tconst, alpha, beta;  //don't allow change in delay time exceed 1 sample at a time
  
  struct phasevars {
  float yn1[4];
  float xn1[4];
  float gain[4];
  } *pstruct; 
  int *stctr;
  
  float phaser(float fxn);
  
  float *ringbuffer;


};


#endif
