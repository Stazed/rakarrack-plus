/*
  Rakarrack Guitar FX
 
  Sequence.h - Simple compressor/Sequence effect with easy interface, minimal controls
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

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "global.h"
#include "RBFilter.h"

#define MINFREQ 220.0f
#define MAXFREQ 8000.0f

class Sequence
{
public:
  Sequence (float * efxoutl_, float * efxoutr_);
  ~Sequence ();
   void cleanup (); 
   
  void out (float * smpsl, float * smpr);
  void changepar (int npar, int value);
  int getpar (int npar);
  int Ppreset;
  void setpreset (int npreset);
  float volume;
  float outvolume;
  
  float *efxoutl;
  float *efxoutr;

private:
  //Parametrii
  int Pvolume;	       //Output Level
  int Psequence[8];	//Sequence.  Each value is 0...127
  int Ptempo;           // Rate
  int Pq;                //Filter Q
  int Pamplitude;     // 0 or 1.  Checkbox on or off...
  int Pstdiff;       // 1 2 3 4 5 6 7 8
  
  int tcount, scount, dscount, intperiod; 
 
  class RBFilter *filterl, *filterr;

  float fsequence[8];
  float maxfreq,minfreq,frequency;  
  float fq;
  float ifperiod,fperiod;
};


#endif
