/*
  Rakarrack Guitar FX
 
  beattracker.h - Detect beats and compute tempo
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing & Josep Andreu
  
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

#ifndef BEAT_H
#define BEAT_H

#include "global.h"
#include "RBFilter.h"


class beattracker
{
public:
  beattracker ();
  ~beattracker ();
  void cleanup (); 
  void detect (float * smpsl, float * smpsr);
  float get_tempo();  //returns tempo in float beats per minute
  int *index;

private:

  long timeseries[20];
  int tsidx; 
  long tscntr;
//Variables for TrigStepper detecting trigger state.
  float peakpulse, peak, envrms, peakdecay, trigthresh;
  int trigtimeout, trigtime, onset, atk;
  float targatk, lmod, rmod;
  
  class RBFilter *rmsfilter, *peaklpfilter, *peakhpfilter, *peaklpfilter2;


};


#endif
