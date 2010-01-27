/*
  Rakarrack Audio FX
 
  Dual_Flange.h - Super Flanger
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing

  Higher intensity flanging accomplished by picking two points out of the delay line to create a wider notch filter.
    
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

#ifndef DUAL_FLANGE_H
#define DUAL_FLANGE_H

#include "global.h"
#include "EffectLFO.h"

#define  D_FLANGE_MAX_DELAY	0.055f			// Number of Seconds  - 50ms corresponds to fdepth = 20 (Hz). Added some extra for padding
#define  LFO_CONSTANT		.00006103515625		// 1/(e^LOG_FMAX - 1)
#define  LOG_FMAX		14.0f			// 2^14 = 16384 -- This optimizes LFO sweep for 20Hz to about 16kHz
#define  MINDEPTH		20.0f			// won't allow filter lower than 20Hz
#define	 MAXDEPTH		20000.0f		// Keeps delay greater than 2 samples at 44kHz SR


class Dflange
{
public:
  Dflange (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~Dflange ();
  void out (REALTYPE * smpsl, REALTYPE * smpr);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();

  int Ppreset;

  REALTYPE *efxoutl;
  REALTYPE *efxoutr;


private:
  //Parameters
  int Pwetdry;		// 0 //Wet/Dry mix.  Range -100 to 100 (percent)
  int Ppanning;		// 1 //Panning.  Range -100 to 100 (percent)
  int Plrcross;		// 2 //L/R Mixing.  Range 0 to 100 (percent)
  int Pdepth;		// 3 //Max delay deviation expressed as frequency of lowest frequency notch.  Min = 20, Max = 4000
  int Pwidth;		// 4 //LFO amplitude.  Range 0 to 16000 (Hz)
  int Poffset;		// 5 //Offset of notch 1 to notch 2.  Range 0 to 100 (percent)
  int Pfb;		// 6 //Feedback parameter.  Range 0 to 100 (percent)
  int Phidamp;		// 7 //Lowpass filter delay line.  Range 20 to 20000 (Hz)
  int Psubtract;	// 8 //Subtract wet/dry instead of add.  Nonzero is true
  int Pzero;		// 9 //Enable through-zero flanging
  int Prate;		// 10 //LFO Speed
  int Pstdiff		// 11 //LFO stereo diff

  REALTYPE wet, dry;		//Wet/Dry mix.  Range -100 to 100 (percent)
  REALTYPE lpan, rpan;		//Panning.  Range -100 to 100 (percent)
  REALTYPE flrcross;		// L/R Mixing.  Range 0 to 100 (percent)
  REALTYPE fdepth;		//Max delay deviation expressed as frequency of lowest frequency notch.  Min = 20, Max = 15000
  REALTYPE fwidth;		//LFO amplitude.  Range 0 to 100 (percent)
  REALTYPE foffset;		// Offset of notch 1 to notch 2.  Range 0 to 100 (percent)
  REALTYPE ffb;			//Feedback parameter.  Range 0 to 100 (percent)
  REALTYPE fhidamp;		//Lowpass filter delay line.  Range 20 to 20000 (Hz)
  REALTYPE fsubtract;		//Subtract wet/dry instead of add.  Nonzero is true
  REALTYPE fzero;		//Enable through-zero flanging
  EffectLFO lfo;		//lfo Flanger
  
  //Internally used variables
  int lfptrA, lfptrB, rfptrA, rfptrB;  //index to each current delay adder

  
  REALTYPE l, r, ldl, rdl;
  REALTYPE fdl, fdr;  //float representation of delay line length
  REALTYPE rflange0, rflange1, lflange0, lflange1, oldrflange0, oldrflange1, oldlflange0, oldlflange1;
  REALTYPE period_const;
  REALTYPE *ldelay, *rdelay;
  REALTYPE oldl, oldr;		//pt. lpf

};

 
#endif
