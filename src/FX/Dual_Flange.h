/*
  Rakarrack Audio FX

  Dual_Flange.h - Super Flanger
  Copyright (C) 2010 Ryan Billing
  Authors:
  Ryan Billing (a.k.a Transmogrifox)  --  Signal Processing
  Copyright (C) 2010 Ryan Billing

  Nasca Octavian Paul -- Remnants of ZynAddSubFX Echo.h structure and utility routines common to ZynSubFX source
  Copyright (C) 2002-2005 Nasca Octavian Paul

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

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"
#include "../EFX_common/delayline.h"

#define DFLANGELV2_URI "https://github.com/Stazed/rakarrack-plus#Dual_Flange"
#define DFLANGE_NAME "RakarrackPlus Dual Flange"

const int C_DFLANGE_PARAMETERS = 15;

enum DFlange_Index
{
    DFlange_DryWet = 0,
    DFlange_Pan,
    DFlange_LR_Cross,
    DFlange_Depth,
    DFlange_Width,
    DFlange_Offset,
    DFlange_Feedback,
    DFlange_LPF,
    DFlange_Subtract,
    DFlange_Zero,
    DFlange_LFO_Tempo,
    DFlange_LFO_Stereo,
    DFlange_LFO_Type,
    DFlange_LFO_Random,
    DFlange_Intense
};

static const char *dflange_parameters[] = 
{
    strdup(NTS(DFlange_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(DFlange_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(DFlange_LR_Cross).c_str()), "Left/Right Crossover",  "LRC", 
    strdup(NTS(DFlange_Depth).c_str()), "Depth",  "DEPTH", 
    strdup(NTS(DFlange_Width).c_str()), "Sweep Width",  "WIDTH", 
    strdup(NTS(DFlange_Offset).c_str()), "Offset Delays",  "OFFS", 
    strdup(NTS(DFlange_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(DFlange_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(DFlange_Subtract).c_str()), "Subtract",  "SUB", 
    strdup(NTS(DFlange_Zero).c_str()), "Sweep Through Zero",  "THZ", 
    strdup(NTS(DFlange_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(DFlange_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDL", 
    strdup(NTS(DFlange_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(DFlange_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(DFlange_Intense).c_str()), "Intense",  "INTENSE"
};

class Dflange : public Effect
{
public:
    Dflange (double sample_rate, uint32_t intermediate_bufsize);
    ~Dflange ();
    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_DFLANGE_PARAMETERS;};
    void set_random_parameters();
    
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return DFLANGELV2_URI; };
    virtual std::string get_name(int) {return DFLANGE_NAME; };

private:
    
    float fSAMPLE_RATE;
    uint32_t PERIOD;
    
    void setvolume(int value);
    //Parameters
    int Pwetdry;		// 0 //Wet/Dry mix.  Range -64 to 64
    int Ppanning;		// 1 //Panning.  Range -64 to 64
    int Plrcross;		// 2 //L/R Mixing.  Range 0 to 127
    int Pdepth;		// 3 //Max delay deviation expressed as frequency of lowest frequency notch.  Min = 20, Max = 4000
    int Pwidth;		// 4 //LFO amplitude.  Range 0 to 16000 (Hz)
    int Poffset;		// 5 //Offset of notch 1 to notch 2.  Range 0 to 100 (percent)
    int Pfb;		// 6 //Feedback parameter.  Range -64 to 64
    int Phidamp;		// 7 //Lowpass filter delay line.  Range 20 to 20000 (Hz)
    int Psubtract;	// 8 //Subtract wet/dry instead of add.  Nonzero is true
    int Pzero;		// 9 //Enable through-zero flanging,   Nonzero is true
    // 10 //LFO Speed
    // 11 //LFO stereo diff
    // 12 //LFO type
    // 13 //LFO Randomness
    int Pintense;		// 14 //Intense Mode

    float wet, dry;		//Wet/Dry mix.
    float lpan, rpan;		//Panning.
    float flrcross, frlcross;	// L/R Mixing.
    float fdepth;		//Max delay deviation expressed as frequency of lowest frequency notch.  Min = 20, Max = 15000
    float fwidth;		//LFO amplitude.
    float foffset;		// Offset of notch 1 to notch 2.  Range 0 to 1.0
    float ffb;			//Feedback parameter.  Range -0.99 to 0.99
    float fhidamp;		//Lowpass filter delay line.  Range 20 to 20000 (Hz)
    float fsubtract;		//Subtract wet/dry instead of add.  Nonzero is true
    float logmax;

    //Internally used variables
    int maxx_delay;
    int kl, kr, zl, zr;
    int zcenter;

    float l, r, ldl, rdl, zdr, zdl;
    float rflange0, rflange1, lflange0, lflange1, oldrflange0, oldrflange1, oldlflange0, oldlflange1;
    float period_const, base, ibase;
    float *ldelay, *rdelay, *zldelay, *zrdelay;
    float oldl, oldr;		//pt. lpf
    float rsA, rsB, lsA, lsB;	//Audio sample at given delay

    delayline *ldelayline0, *rdelayline0, *ldelayline1, *rdelayline1;

    EffectLFO *lfo;		//lfo Flanger

};


#endif
