/*

  VaryBand.h - Distorsion Effect

  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul
  ZynAddSubFX - a software synthesizer

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

// Renamed from MBVvol.h 4/18/2020 by stazed

#ifndef VARYBAND_H
#define VARYBAND_H

#include "Effect.h"
#include "EffectLFO.h"
#include "AnalogFilter.h"

/**
 * This is the Rakarrack-plus parameters number for presets and file saving
 * which does not use last four listed
 */
const int C_VARYBAND_PARAMETERS = 11;

enum VaryBand_Index
{
    VaryBand_DryWet = 0,
    VaryBand_LFO_Tempo_1,
    VaryBand_LFO_Type_1,
    VaryBand_LFO_Stereo_1,
    VaryBand_LFO_Tempo_2,
    VaryBand_LFO_Type_2,
    VaryBand_LFO_Stereo_2,
    VaryBand_Cross_1,
    VaryBand_Cross_2,
    VaryBand_Cross_3,
    VaryBand_Combination,      // Pcombi legacy
    VaryBand_Low_Band,
    VaryBand_Mid_Band_1,
    VaryBand_Mid_Band_2,
    VaryBand_High_Band
};

class VaryBand : public Effect
{
public:
    VaryBand (double sample_rate, uint32_t intermediate_bufsize);
    ~VaryBand ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_VARYBAND_PARAMETERS;};

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void initialize ();
    void clear_initialize();
    
private:

    uint32_t PERIOD;
    double fSAMPLE_RATE;

    float *lowl;
    float *lowr;
    float *midll;
    float *midlr;
    float *midhl;
    float *midhr;
    float *highl;
    float *highr;

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setCross1 (int value);
    void setCross2 (int value);
    void setCross3 (int value);
    void setCombi (int value);
    void calcCombi ();
    void parseCombi (int value);
    void updateVols(void);
    void setSource(float** ptr, float** ptrr, int val);

    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Pcombi;
    int Cross1;
    int Cross2;
    int Cross3;
    int PsL, PsML, PsMH, PsH;//volume source per band

    //Parametrii reali

    float lfo1l,lfo1r,lfo2l,lfo2r;
    float v1l,v1r,v2l,v2r;
    float d1,d2,d3,d4;
    float volL,volML,volMH,volH;
    float volLr,volMLr,volMHr,volHr;
    float *sourceL,*sourceML,*sourceMH,*sourceH;
    float *sourceLr,*sourceMLr,*sourceMHr,*sourceHr;
    float one, zero;
    AnalogFilter *lpf1l, *lpf1r, *hpf1l, *hpf1r;
    AnalogFilter *lpf2l, *lpf2r, *hpf2l, *hpf2r;
    AnalogFilter *lpf3l, *lpf3r, *hpf3l, *hpf3r;
    float* interpbuf;   //buffer for filters

    EffectLFO* lfo1,*lfo2;

};


#endif
