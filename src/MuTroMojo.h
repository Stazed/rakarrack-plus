/*
  ZynAddSubFX - a software synthesizer

  MuTroMojo.h - "WahWah" effect and others
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Ryan Billing

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

// Renamed from RyanWah.h 4/18/2020 by stazed

#ifndef MUTROMOJO_H
#define MUTROMOJO_H
#include "EffectLFO.h"
#include "RBFilter.h"
#include "AnalogFilter.h"

class MuTroMojo
{
public:
    MuTroMojo (double sample_rate, uint32_t intermediate_bufsize);
    ~MuTroMojo ();
    void out (float * efxoutl, float * efxoutr);

    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2

    void initialize();
    void clear_initialize();

    int Ppreset;
    int Pmode;
    float outvolume;
    
private:

    void setvolume (int Pvolume);
    void setwidth (int Pwidth);
    void setampsns (int Pampsns);
    void reinitfilter ();

    float fSAMPLE_RATE;
    uint32_t PERIOD;

    int Pvolume;		//For wet/dry mix
    int Pwidth;		//0/127// The width of the lfo of the MuTroMojo
    int Pampsns;	        //0/127//how the filter varies according to the input amplitude
    int Pampsnsinv;	//-64/64//bias of the filter
    int Pampsmooth;	//0/127//how smooth the input amplitude changes the filter, also for Pampsinsinv smoothing for wah wah pedal
    int Prange;		//10/6000Hz//how far filter can deviate.
    int Php;		//-64/64//high pass filter mix
    int Plp;		//-64/64//low pass filter mix
    int Pbp;		//-64/64//Bandpass filter mix
    int Pq;		//0/127//filter resonance.
    int Pqm;	        //Qmode
    int Pstages;		//0-6// counter// Filter stages
    int Pminfreq;		//30/800//set lowest freq in range.
    int Pamode;           //0 or 1// modulation mode.
    int variq;		//on-off//checkbox//Selects whether Q is varied w/ frequency to emulate a wahwah


    //Control Parametrii

    int Ftype;
    int Fstages;

    //Valorile interne

    float rpanning, lpanning, depth, ampsns, ampsmooth, wahsmooth, fbias, oldfbias, oldfbias1, oldfbias2, q, frequency, maxfreq, base, ibase, minfreq;
    float ms1, lpmix, hpmix, bpmix;	//mean squares
    float centfreq; //testing
    class RBFilter *filterl, *filterr;
    class AnalogFilter *sidechain_filter;
    float* interpbuf; //buffer for filters

    class FPreset *Fpre;
    EffectLFO *lfo;		//lfo-ul MuTroMojo
    
};

#endif
