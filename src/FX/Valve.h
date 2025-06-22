/*
  ZynAddSubFX - a software synthesizer
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Based on valve Steve Harris LADSPA plugin.
  Valve.h - Distorsion Effect

  Modified and adapted to rakarrack by Josep Andreu.

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

#ifndef VALVE_H
#define VALVE_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"
#include "../EFX_common/HarmonicEnhancer.h"

#define VALVELV2_URI "https://github.com/Stazed/rakarrack-plus#Valve"
#define VALVE_NAME "RakarrackPlus Valve"

const int C_VALVE_PARAMETERS = 13;

enum Valve_Index
{
    Valve_DryWet = 0,
    Valve_Pan,
    Valve_LR_Cross,
    Valve_Drive,
    Valve_Level,
    Valve_Negate,
    Valve_LPF,
    Valve_HPF,
    Valve_Stereo,
    Valve_Prefilter,
    Valve_Distortion,
    Valve_Ex_Dist,
    Valve_Presence
};

static const char *valve_parameters[] = 
{
    strdup(NTS(Valve_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Valve_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Valve_LR_Cross).c_str()), "Left/Right Crossover",  "LRCr", 
    strdup(NTS(Valve_Drive).c_str()), "Drive",  "DRIVE", 
    strdup(NTS(Valve_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Valve_Negate).c_str()), "Negate (Polarity Switch)",  "NEG", 
    strdup(NTS(Valve_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(Valve_HPF).c_str()), "Highpass Filter",  "HPF", 
    strdup(NTS(Valve_Stereo).c_str()), "Stereo",  "STEREO", 
    strdup(NTS(Valve_Prefilter).c_str()), "Prefilter",  "PREFILTER", 
    strdup(NTS(Valve_Distortion).c_str()), "Distortion",  "DIST", 
    strdup(NTS(Valve_Ex_Dist).c_str()), "Extra Distortion",  "ED", 
    strdup(NTS(Valve_Presence).c_str()), "Presence",  "PRES"
};

class Valve : public Effect
{
public:
    Valve (double sample_rate, uint32_t intermediate_bufsize);
    ~Valve ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    float Wshape(float x);
    void cleanup ();
    int get_number_efx_parameters () {return C_VALVE_PARAMETERS;};
    void set_random_parameters();
    
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return VALVELV2_URI; };
    virtual std::string get_name(int) {return VALVE_NAME; };
    
    void initialize();
    void clear_initialize();
    void applyfilters (float * efxoutl, float * efxoutr);

private:
    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setlrcross (int _Plrcross);
    void setlpf (int value);
    void sethpf (int value);
    void setpresence(int value);
    void init_coefs();

    uint32_t PERIOD;
    double fSAMPLE_RATE;

    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing
    int Pdrive;		//the input amplification
    int Plevel;		//the ouput amplification
    int Pnegate;	//if the input is negated
    int Plpf;		//lowpass filter
    int Phpf;		//highpass filter
    int Pstereo;	//0=mono,1=stereo
    int Pprefiltering;	//if you want to do the filtering before the distorsion
    int Q_q;
    int Ped;
    int Presence;

    //Parametrii reali
    float panning, lrcross, q, dist, otml,otmr, itml, itmr, factor, atk;
    float rm[10];

    float coef;
    float qcoef;
    float fdist;
    float inputvol;

    float* interpbuf; //buffer for filters
    AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;
    class HarmEnhancer *harm;

};


#endif
