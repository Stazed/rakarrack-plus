/*
  ZynAddSubFX - a software synthesizer

  APhaser.h - Phaser effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu and Ryan Billing

  Further modified for rakarrack by Ryan Billing (Transmogrifox) to model Analog Phaser behavior 2009

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

#ifndef APHASER_H
#define APHASER_H

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"

#define APHASELV2_URI "https://github.com/Stazed/rakarrack-plus#aphas"
#define APHASE_NAME "RakarrackPlus Analog Phaser"

const int C_APHASER_PARAMETERS = 13;

enum APhaser_Index
{
    APhase_DryWet = 0,
    APhase_Distortion,
    APhase_LFO_Tempo,
    APhase_LFO_Random,
    APhase_LFO_Type,
    APhase_LFO_Stereo,
    APhase_Width,
    APhase_Feedback,
    APhase_Stages,
    APhase_Mismatch,
    APhase_Subtract,
    APhase_Depth,
    APhase_Hyper
};

static const char *aphase_parameters[] = 
{
    strdup(NTS(APhase_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(APhase_Distortion).c_str()), "Distort",  "DISTORT", 
    strdup(NTS(APhase_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(APhase_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(APhase_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(APhase_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDL", 
    strdup(NTS(APhase_Width).c_str()), "Width",  "WIDTH", 
    strdup(NTS(APhase_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(APhase_Stages).c_str()), "Stages",  "STAGES", 
    strdup(NTS(APhase_Mismatch).c_str()), "Mismatch",  "MISMATCH", 
    strdup(NTS(APhase_Subtract).c_str()), "Subtract",  "SUB", 
    strdup(NTS(APhase_Depth).c_str()), "Phase Depth",  "DEPTH", 
    strdup(NTS(APhase_Hyper).c_str()), "Hyper",  "HYPER"
};

class Analog_Phaser : public Effect
{
public:
    Analog_Phaser (double sample_rate, uint32_t intermediate_bufsize);
    ~Analog_Phaser ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_APHASER_PARAMETERS;};
    void set_random_parameters();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return APHASELV2_URI; };
    virtual std::string get_name(int) {return APHASE_NAME; };

private:
    float fPERIOD;
    uint32_t PERIOD;
    
    //Phaser parameters
    int Pvolume;        //Used in Process.C to set wet/dry mix
    int Pdistortion;    //Model distortion added by FET element
    int Pwidth;		//Phaser width (LFO amplitude)
    int Pfb;		//feedback
    int Poffset;	//Model mismatch between variable resistors
    int Pstages;	//Number of first-order All-Pass stages
    int Poutsub;	//if I wish to subtract the output instead of the adding it
    int Phyper;		//lfo^2 -- converts tri into hyper-sine
    int Pdepth;         //Depth of phaser sweep
    int Pbarber;         //Enable barber pole phasing

    //Control parameters
    void setvolume (int _Pvolume);
    void setdistortion (int _Pdistortion);
    void setwidth (int _Pwidth);
    void setfb (int _Pfb);
    void setoffset (int _Poffset);
    void setstages (int _Pstages);
    void setdepth (int _Pdepth);

    //Internal Variables
    bool barber;			//Barber pole phasing flag
    float distortion, fb, width, offsetpct, fbl, fbr, depth;
    float *lxn1, *lyn1,*rxn1, *ryn1, *offset;
    float oldlgain, oldrgain, rdiff, ldiff, invperiod;

    float mis;
    float Rmin;	// 2N5457 typical on resistance at Vgs = 0
    float Rmax;	// Resistor parallel to FET
    float Rmx;		// Rmin/Rmax to avoid division in loop
    float Rconst;      // Handle parallel resistor relationship
    float C;	        // Capacitor
    float CFs;		// A constant derived from capacitor and resistor relationships

    EffectLFO *lfo;		//Phaser modulator

};

#endif
