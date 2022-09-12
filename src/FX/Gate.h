
// Based in gate_1410.c LADSPA Swh-plugins

/*
  rakarrack - a guitar effects software

 Gate.h  -  Noise Gate Effect definitions
 Based on Steve Harris LADSPA gate.

  Copyright (C) 2008 Josep Andreu
  Author: Josep Andreu

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


#ifndef NOISEGATE_H
#define NOISEGATE_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"

#define GATELV2_URI "https://github.com/Stazed/rakarrack-plus#gate"
#define GATE_NAME "RakarrackPlus NoiseGate"

const int C_GATE_PARAMETERS = 7;

enum Gate_Index
{
    Gate_Threshold = 0,
    Gate_Range,
    Gate_Attack,
    Gate_Release,
    Gate_LPF,
    Gate_HPF,
    Gate_Hold
};

static const char *gate_parameters[] = 
{
    strdup(NTS(Gate_Threshold).c_str()), "Threshold",  "THRESHOLD", 
    strdup(NTS(Gate_Range).c_str()), "Range",  "RANGE", 
    strdup(NTS(Gate_Attack).c_str()), "Attack Time",  "ATTACK", 
    strdup(NTS(Gate_Release).c_str()), "Release Time",  "RELEASE", 
    strdup(NTS(Gate_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(Gate_HPF).c_str()), "Highpass Filter",  "HPF", 
    strdup(NTS(Gate_Hold).c_str()), "Hold",  "HOLD"
};

class Gate : public Effect
{

public:

    Gate (double sample_rate, uint32_t intermediate_bufsize);
    ~Gate ();

    void out (float *efxoutl, float *efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_GATE_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return GATELV2_URI; };
    virtual std::string get_name(int) {return GATE_NAME; };
    
    void initialize();
    void clear_initialize();

private:
    // Compressor

    int Pthreshold;	// attack time  (ms)
    int Pattack;        // release time (ms)
    int Ohold;
    int Pdecay;
    int Prange;
    int Plpf;
    int Phpf;
    int Phold;

    void setlpf (int Plpf);
    void sethpf (int Phpf);

    uint32_t PERIOD;
    double fSAMPLE_RATE;

    int hold_count;
    int state;
    float range;
    float cut;
    float t_level;
    float a_rate;
    float d_rate;
    float env;
    float gate;
    float fs;
    float hold;
    
    float* interpbuf; //buffer for filters
    AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;

};

#endif
