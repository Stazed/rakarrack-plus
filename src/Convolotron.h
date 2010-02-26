/*
  ZynAddSubFX - a software synthesizer
 
  Convolotron.h - Convolotron Effect
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

#ifndef CONVOLOTRON_H
#define CONVOLOTRON_H

#include <sndfile.h>
#include "global.h"
#define  MAX_C_SIZE 2	// Number of Seconds
class Convolotron
{
public:
  Convolotron (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~Convolotron ();
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
  //Parametrii
  int Pvolume;	//Volumul or E/R
  int Ppanning;	//Panning
  int Plrcross;	// L/R Mixing
  int Phidamp;
  int Plevel;
  int Filenum;
  char Filename[128];
  int setfile (int value);
  void setvolume (int Pvolume);
  void setpanning (int Ppanning);
  void setlrcross (int Plrcross);
  void sethidamp (int Phidamp);

  SNDFILE *infile;
  SF_INFO sfinfo;
  unsigned int howmany,offset;

  //Parametrii reali
  REALTYPE panning, lrcross,  hidamp;
  REALTYPE *buf;
  float maxx_size;
};


#endif
