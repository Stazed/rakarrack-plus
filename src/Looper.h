/*
  ZynAddSubFX - a software synthesizer
 
  Echo.h - Echo Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  
  Reverse Echo by Transmogrifox
    
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

#ifndef LOOPER_H
#define LOOPER_H

#include "global.h"
#define  ATTACK  0.175f  //crossover time for reverse delay
#define  MAX_LOOP_DELAY 12	// Number of Seconds
class Looper
{
public:
  Looper (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~Looper ();
  void out (REALTYPE * smpsl, REALTYPE * smpr);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();

  void setdryonly ();
  int Ppreset;

  REALTYPE *efxoutl;
  REALTYPE *efxoutr;

  REALTYPE volume;

  REALTYPE outvolume;

private:
  //Parameters
  int Pvolume;	//Mix
  int Precord;	//set to 1
  int Pplay;	//set to 1
  int Pstop;	//set to 1
  int Pclear;   //set to 1
  int Preverse;	//0 or 1
  int Pfade;    //100ms to 2000ms//  loop self-fading


  void initdelays ();
  REALTYPE *ldelay, *rdelay;
  REALTYPE oldl, oldr;		//pt. lpf
  int kl, rvkl, rvfl, maxx_delay, fade, dl, first_time;

  REALTYPE  Srate_Attack_Coeff;
};


#endif
