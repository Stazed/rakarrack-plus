/*
  ZynAddSubFX - a software synthesizer
 
  EQ.h - EQ Effect
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

#ifndef EQ_H
#define EQ_H

#include "global.h"
#include "AnalogFilter.h"

#define MAX_EQ_BANDS 16

class EQ
{
public:
  EQ (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~EQ ();
  void out (REALTYPE * smpsl, REALTYPE * smpr);
  void setpreset (unsigned char npreset);
  void changepar (int npar, unsigned char value);
  unsigned char getpar (int npar);
  void cleanup ();
  REALTYPE getfreqresponse (REALTYPE freq);
  unsigned char Ppreset;
  REALTYPE *efxoutl;
  REALTYPE *efxoutr;

  REALTYPE outvolume;		//this is the volume of effect and is public because need it in system effect. The out volume of

  REALTYPE volume;

  unsigned char Pvolume;	//Volumul 

  void setvolume (unsigned char Pvolume);




private:
  //Parametrii
//      unsigned char Pvolume;//Volumul 

//      void setvolume(unsigned char Pvolume);

  struct
  {
    //parameters
    unsigned char Ptype, Pfreq, Pgain, Pq, Pstages;
    //internal values
    AnalogFilter *l, *r;
  } filter[MAX_EQ_BANDS];

};


#endif
