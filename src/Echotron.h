/*
  Echotron.h - Convolution-based Echo Effect

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

#ifndef ECHOTRON_H
#define ECHOTRON_H

#include "global.h"
#include "AnalogFilter.h"

//some of the C++ std libs
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define  ECHOTRON_F_SIZE   500       //Allow up to 500 points in the file
#define  ECHOTRON_MAXFILTERS  8      //filters available

//and for some of the std libs functions
using namespace std;

class Echotron
{
public:
  Echotron (float * efxoutl_, float * efxoutr_);
  ~Echotron ();
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
  void setlpf (int Plpf);
  void setfb(int value);
  void convert_time();



  //Parametrii
  int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
  int Ppanning;	//Panning
  int Plrcross;	// L/R Mixing  // 
  int Phidamp;
  int Puser;		//0,1//
  int Ptempo;		//-64 ... 64//For stretching reverb responses
  int Filenum;
  int Pfb;		//-64 ... 64// amount of feedback
  int Pfade;
  int Pes;		//0 ... 127// Add stereo spatialization
  int Prv;              //Shuffle
  int Plpf;
  int Pdiff;
  
  //variables to keep the compiler happy during development:
  int Plevel;
  int Plength;
  int Psafe;
  int Pidelay;
  int Pstretch;
  float lrcross, feedback;

  int offset;
  int maxx_size;
  int data_length;

  
  //arrays of parameters:
float fPan[ECHOTRON_F_SIZE];  //1+Pan from text file
float fTime[ECHOTRON_F_SIZE];
float fLevel[ECHOTRON_F_SIZE];
float fLP[ECHOTRON_MAXFILTERS];
float fBP[ECHOTRON_MAXFILTERS];
float fHP[ECHOTRON_MAXFILTERS];
float fFreq[ECHOTRON_MAXFILTERS];
float fQ[ECHOTRON_MAXFILTERS];
int iStages[ECHOTRON_MAXFILTERS];

int time[ECHOTRON_F_SIZE];
float i_fPan[ECHOTRON_F_SIZE];  //1-fPan
float ldata[ECHOTRON_F_SIZE];
float rdata[ECHOTRON_F_SIZE];

  float fstretch, idelay, ffade, maxtime, maxdata, decay, diffusion;
  float lpanning, rpanning, hidamp, alpha_hidamp, convlength, oldl;
  
  float *lxn, *rxn;
  
  float level,fb, rfeedback, lfeedback,levpanl,levpanr, ilrcross;
  float roomsize;
 
  class AnalogFilter *lpfl, *lpfr;	//filters
  
    struct 
  { 
    float sfreq, sq,sLP,sBP,sHP, sStg;
    class RBFilter *l, *r;

  } filterbank[ECHOTRON_MAXFILTERS];

};


#endif
