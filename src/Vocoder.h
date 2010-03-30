/*
  Vocoder.h - Vocoder Effect

  Author: Ryam Billing & Josep Andreu
  
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

#ifndef VOCODER_H
#define VOCODER_H
#define VOC_BANDS  16		//filter bands to process

#include "global.h"
#include "AnalogFilter.h"

class Vocoder
{
public:
  Vocoder (float * efxoutl_, float * efxoutr_, float *auxresampled_);
  ~Vocoder ();
  void out (float * smpsl, float * smpr);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();
  int Ppreset;
  float *efxoutl;
  float *efxoutr;
  float *auxresampled;
  float outvolume;



private:
  //Parametrii
  int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
  int Ppanning;	//Panning
  int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
  int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
  void setvolume (int Pvolume);
  void setpanning (int Ppanning);
  void init_filters();
  float cperiod;
  float lpanning, rpanning, level;
  float *vocbuf;
  float *tmpsmpsl, *tmpsmpsr; 
  float *tmpl, *tmpr;
    struct
  {
  float sfreq, sq, gain, oldgain;
    AnalogFilter *l, *r, *aux;

  } filterbank[VOC_BANDS];



};


#endif
