/*
  Reverbtron.h - Reverbtron Effect

  Author: Ryan Billing & Josep Andreu
  
  Adapted effect structure of ZynAddSubFX - a software synthesizer
  Author: Nasca Octavian Paul
   
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

#ifndef REVERBTRON_H
#define REVERBTRON_H

#include "global.h"

class Reverbtron
{
public:
  Reverbtron (float * efxoutl_, float * efxoutr_);
  ~Reverbtron ();
  void out (float * smpsl, float * smpr);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();
  int setfile (int value);

  int Ppreset;

  float *efxoutl;
  float *efxoutr;
  float outvolume;

  char Filename[128];


private:

  void setvolume (int Pvolume);
  void setpanning (int Ppanning);
  void sethidamp (int Phidamp);
  void convert_time();



  //Parametrii
  int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
  int Ppanning;	//Panning
  int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
  int Phidamp;
  int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
  int Plength;		//20... 1500// Set maximum number of IR points to calculate.
  int Puser;		//-64...64//Feedback.
  int Pstretch;		//-64 ... 64//For stretching reverb responses
  int Pidelay;         //0 ... 500ms// initial delay time
  int Filenum;
  int Psafe;
  int Pfb;		//-64 ... 64// amount of feedback
  int Pfade;
  int offset;
  int maxx_size;
  int data_length;
  int avgtime;

  int *time;
  float fstretch, idelay, ffade, maxtime;
  float lpanning, rpanning, hidamp, alpha_hidamp, convlength, oldl;
  float *data, *lxn, *ftime, *tdata;
  float level,fb, feedback;
 


};


#endif
