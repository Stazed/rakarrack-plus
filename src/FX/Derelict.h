/*
  ZynAddSubFX - a software synthesizer

  Distorsion.h - Distorsion Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu  & Ryan Billing

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

// Renamed from NewDist.h on 4/18/2020 by Stazed

#ifndef DERELICT_H
#define DERELICT_H

#include <vector>

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"
#include "../EFX_common/Waveshaper.h"
#include "../EFX_common/FilterParams.h"
#include "../EFX_common/Filter.h"

#define DERELV2_URI "https://github.com/Stazed/rakarrack-plus#dere"
#define DERE_NAME "RakarrackPlus Derelict"

const int C_DERE_PARAMETERS = 12;

enum Dere_Index
{
    Dere_DryWet = 0,
    Dere_Pan,
    Dere_LR_Cross,
    Dere_Drive,
    Dere_Level,
    Dere_Type,
    Dere_Negate,
    Dere_LPF,
    Dere_HPF,
    Dere_Color,
    Dere_Prefilter,
    Dere_Suboctave
};

static const char *dere_parameters[] = 
{
    strdup(NTS(Dere_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Dere_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Dere_LR_Cross).c_str()), "Left/Right Crossover",  "LRCr", 
    strdup(NTS(Dere_Drive).c_str()), "Drive",  "DRIVE", 
    strdup(NTS(Dere_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Dere_Type).c_str()), "Type",  "TYPE", 
    strdup(NTS(Dere_Negate).c_str()), "Negate (Polarity Switch)",  "NEG", 
    strdup(NTS(Dere_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(Dere_HPF).c_str()), "Highpass Filter",  "HPF", 
    strdup(NTS(Dere_Color).c_str()), "Color",  "COLOR", 
    strdup(NTS(Dere_Prefilter).c_str()), "Prefilter",  "PREFILTER", 
    strdup(NTS(Dere_Suboctave).c_str()), "Suboctave",  "OCT"
};

//Waveshaping(called by Distorsion effect and waveshape from OscilGen)
// void waveshapesmps (int n, float * smps, int type,
// 		    int drive);

class Derelict : public Effect
{
public:
    Derelict (int wave_res, int wave_upq, int wave_dnq, double sample_rate, uint32_t intermediate_bufsize);
    ~Derelict ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_DERE_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return DERELV2_URI; };
    virtual std::string get_name(int) {return DERE_NAME; };
    
    void initialize();
    void clear_initialize();
    void applyfilters (float * efxoutl, float * efxoutr, uint32_t period);

    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

private:
    uint32_t PERIOD;
    double fSAMPLE_RATE;
    int WAVE_RES, WAVE_UPQ, WAVE_DNQ;
public:

    float inpll[4096];
    float inplr[4096];

private:

    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setlrcross (int _Plrcross);
    void setoctave (int _Poctave);
    void setlpf (int value);
    void sethpf (int value);

    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing
    int Pdrive;		//the input amplification
    int Plevel;		//the ouput amplification
    int Ptype;		//Distorsion type
    int Pnegate;	//if the input is negated
    int Plpf;		//lowpass filter
    int Phpf;		//highpass filter
    int Prfreq;
    int Pprefiltering;	//if you want to do the filtering before the distorsion
    int Poctave;	//mix sub octave

    float rfreq;
    float panning, lrcross, octave_memoryl, togglel, octave_memoryr, toggler, octmix;
    float *octoutl, *octoutr;

    //Parametrii reali
    float* interpbuf;//buffer for filters
    AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr, *blockDCl, *blockDCr, *DCl, *DCr;
    class Waveshaper *wshapel, *wshaper;

    class Filter *filterl, *filterr;
    class FilterParams *filterpars;

};


#endif
