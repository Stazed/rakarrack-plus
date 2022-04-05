/*
  ZynAddSubFX - a software synthesizer
  Synthfilter.h - Synthesizer filter effect
  Copyright (C) 2010 Ryan Billing
  Based on Analog Phaser  derived from
  Phaser.h/.C
  Copyright (C) 2002-2005 Nasca Octavian Paul

  Authors: Nasca Octavian Paul, Ryan Billing, Josep Andreu

  Modified for rakarrack by Josep Andreu

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

#ifndef SYNTHFILTER_H
#define SYNTHFILTER_H

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"

#define SYNTHLV2_URI "https://github.com/Stazed/rakarrack-plus#Synthfilter"

const int C_SYNTHFILTER_PARAMETERS = 16;

enum Synthfilter_Index
{
    Synthfilter_DryWet = 0,
    Synthfilter_Distort,
    Synthfilter_LFO_Tempo,
    Synthfilter_LFO_Random,
    Synthfilter_LFO_Type,
    Synthfilter_LFO_Stereo,
    Synthfilter_Width,
    Synthfilter_Feedback,
    Synthfilter_LPF_Stages,
    Synthfilter_HPF_Stages,
    Synthfilter_Subtract,
    Synthfilter_Depth,
    Synthfilter_Env_Sens,
    Synthfilter_Attack,
    Synthfilter_Release,
    Synthfilter_Offset
};

class Synthfilter : public Effect
{
public:
    Synthfilter (double sample_rate, uint32_t intermediate_bufsize);
    ~Synthfilter ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_SYNTHFILTER_PARAMETERS;};

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf);
    virtual std::string get_URI() { return SYNTHLV2_URI; };

private:

    //Control parameters
    void setvolume (int _Pvolume);
    void setdistortion (int _Pdistortion);
    void setwidth (int _Pwidth);
    void setfb (int _Pfb);
    void setdepth (int _Pdepth);

    uint32_t PERIOD;

    //Phaser parameters
    int Pvolume;			 //0//Used in Process.C to set wet/dry mix
    int Pdistortion;		 //1//0...127//Model distortion
    //2//Tempo//LFO frequency
    //3//0...127//LFO Random
    //4//0...max types//LFO Type
    //5//0...127//LFO stereo offset
    int Pwidth;			 //6//0...127//Phaser width (LFO amplitude)
    int Pfb;			 //7//-64...64//feedback
    int Plpstages;	         //8//0...12//Number of first-order Low-Pass stages
    int Phpstages;		 //9//0...12//Number of first-order High-Pass stages
    int Poutsub;			 //10//0 or 1//subtract the output instead of the adding it
    int Pdepth;			 //11//0...127//Depth of phaser sweep
    int Penvelope;		 //12//-64...64//envelope sensitivity
    int Pattack;			 //13//0...1000ms//Attack Time
    int Prelease;			 //14//0...500ms//Release Time
    int Pbandwidth;		 //15//0...127//Separate high pass & low pass

    //Internal Variables
    float delta;
    float distortion, fb, width, env, envdelta, sns, att, rls, fbl, fbr, depth, bandgain;
    float *lyn1, *ryn1, *lx1hp, *ly1hp, *rx1hp, *ry1hp;
    float oldlgain, oldrgain, inv_period;

    float Rmin;	// 2N5457 typical on resistance at Vgs = 0
    float Rmax;	// Resistor parallel to FET
    float C, Clp, Chp;	        // Capacitor

    EffectLFO* lfo;	         //Filter modulator
};

#endif
