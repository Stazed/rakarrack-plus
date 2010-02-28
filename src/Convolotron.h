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
#include "Resample.h"
#define  MAX_C_SIZE 500.0f // Number of milliseconds
#define  RSMP_QUAL  0	   // Best quality Sinc interpolation

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
  int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
  int Ppanning;	//Panning
  int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
  int Phidamp;
  int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
  int Plength;		//5...500 ms// Set maximum length of IR.
  int Pfb;		//-64...64//Feedback.
  int Preverb;		//0 or 1//For processing long reverb responses using some tricks to make CPU ok.
  int Pquality;
  int Filenum;
  char Filename[128];
  int setfile (int value);
  void setvolume (int Pvolume);
  void setpanning (int Ppanning);
  void setlrcross (int Plrcross);
  void sethidamp (int Phidamp);
  void process_rbuf();

  SNDFILE *infile;
  SF_INFO sfinfo;

  int howmany,offset;

  //Parametrii reali
  REALTYPE panning, lrcross,  hidamp, convlength;
  REALTYPE *rbuf, *buf, *lxn, *rxn;
  float maxx_size;
  float fquality,level;
};


#endif
