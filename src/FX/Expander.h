
// Based in gate_1410.c LADSPA Swh-plugins

/*
  rakarrack - a guitar effects software

 Expander.h  -  Noise Gate Effect definitions
 Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
 Stefan Westerfeld <stefan@space.twc.de>

  Copyright (C) 2008-2010 Ryan Billing & Josep Andreu
  Author: Ryan Billing & Josep Andreu

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#ifndef EXPANDER_H
#define EXPANDER_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"

#define EXPANDLV2_URI "https://github.com/Stazed/rakarrack-plus#Expander"

const int C_EXPANDER_PARAMETERS = 7;

enum Expander_Index
{
    Expander_Threshold = 0,
    Expander_Shape,
    Expander_Attack,
    Expander_Release,
    Expander_LPF,
    Expander_HPF,
    Expander_Gain
};

class Expander : public Effect
{

public:

    Expander (double sample_rate, uint32_t intermediate_bufsize);
    ~Expander ();

    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_EXPANDER_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI() { return EXPANDLV2_URI; };
    
    void initialize();
    void clear_initialize();

private:
    // Compressor

    int Pthreshold;		// Threshold, -80 to 0dB
    int Pattack;			// attack time  (ms)  Set range from 10ms ... 2000ms
    int Pdecay;			// release time (ms)  Set range from 10ms ... 500ms
    int Pshape;			// Sharpness of transition from off to on.  Range is 0 ... 50
    int Plpf;
    int Phpf;
    int Plevel;
    int efollower;		// This is a mode allowing this object to be used as a "dynamics detector"
    // (envelope follower).  If efollower == 1, then efxoutl is a level to be used in place of an LFO
    // for filter modulation, etc. Then efxoutr is signal + envelope for things such as dynamic distortion.
    // Variable efollower is set to 0 by default in constructor.  Do not set this mode unless using this object
    // to control a parameter with signal dynamics.

    void setlpf (int Plpf);
    void sethpf (int Phpf);

    uint32_t PERIOD;
    double fSAMPLE_RATE;
    
    float sgain;
    float sfactor;
    float tfactor;
    float tlevel;
    float a_rate;
    float d_rate;
    float env;
    float oldgain;
    float gain;
    float fs;
    float level;


    float* interpbuf; //buffer for filters
    AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;

};

#endif
