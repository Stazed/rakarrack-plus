/*
  ZynAddSubFX - a software synthesizer
 
  Phaser.h - Phaser effect
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

#ifndef PHASER_H
#define PHASER_H
#include "global.h"
#include "EffectLFO.h"

#define MAX_PHASER_STAGES 12
class Phaser
{
public:
  Phaser (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~Phaser ();
  void out (REALTYPE * smpsl, REALTYPE * smpsr);
  void setpreset (unsigned char npreset);
  void changepar (int npar, unsigned char value);
  unsigned char getpar (int npar);
  void cleanup ();
  void setdryonly ();
  unsigned char Ppreset;
  REALTYPE *efxoutl;
  REALTYPE *efxoutr;

  REALTYPE outvolume;
  REALTYPE volume;




private:
  //Parametrii Phaser
    EffectLFO lfo;		//lfo-ul Phaser
  unsigned char Pvolume;
  unsigned char Ppanning;
  unsigned char Pdepth;		//the depth of the Phaser
  unsigned char Pfb;		//feedback
  unsigned char Plrcross;	//feedback
  unsigned char Pstages;
  unsigned char Poutsub;	//if I wish to substract the output instead of the adding it
  unsigned char Pphase;

  //Control Parametrii
  void setvolume (unsigned char Pvolume);
  void setpanning (unsigned char Ppanning);
  void setdepth (unsigned char Pdepth);
  void setfb (unsigned char Pfb);
  void setlrcross (unsigned char Plrcross);
  void setstages (unsigned char Pstages);
  void setphase (unsigned char Pphase);

  //Valorile interne
  REALTYPE panning, fb, depth, lrcross, fbl, fbr, phase;
  REALTYPE *oldl, *oldr;
  REALTYPE oldlgain, oldrgain;
};

#endif
