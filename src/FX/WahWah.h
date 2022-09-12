/*
  ZynAddSubFX - a software synthesizer

  WahWah.h - "WahWah" effect and others
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

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

// Renamed from DynamicFilter.h on 4/19/2020 by stazed

#ifndef WAHWAH_H
#define WAHWAH_H

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"
#include "../EFX_common/Filter.h"

#define WAHLV2_URI "https://github.com/Stazed/rakarrack-plus#wha"
#define WAH_NAME "RakarrackPlus WahWah"

const int C_WAHWAH_PARAMETERS = 11;

enum WahWah_Index
{
    WahWah_DryWet = 0,
    WahWah_Pan,
    WahWah_LFO_Tempo,
    WahWah_LFO_Random,
    WahWah_LFO_Type,
    WahWah_LFO_Stereo,
    WahWah_Depth,
    WahWah_Sense,
    WahWah_ASI,
    WahWah_Smooth,
    WahWah_Mode
};

static const char *wah_parameters[] = 
{
    strdup(NTS(WahWah_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(WahWah_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(WahWah_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(WahWah_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(WahWah_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(WahWah_LFO_Stereo).c_str()), "LFO L/R Delay",  "WIDTH", 
    strdup(NTS(WahWah_Depth).c_str()), "Depth",  "DEPTH", 
    strdup(NTS(WahWah_Sense).c_str()), "Sensitivity",  "SENSE", 
    strdup(NTS(WahWah_ASI).c_str()), "Invert",  "INV", 
    strdup(NTS(WahWah_Smooth).c_str()), "Smooth",  "SMOOTH", 
    strdup(NTS(WahWah_Mode).c_str()), "Filter Type",  "MODE"
};

class WahWah : public Effect
{
public:
    WahWah (double sample_rate, uint32_t intermediate_bufsize);
    ~WahWah ();
    void out (float * efxoutl, float * efxoutr);

    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_WAHWAH_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return WAHLV2_URI; };
    virtual std::string get_name(int) {return WAH_NAME; };

private:
    
    uint32_t PERIOD;
    double fSAMPLE_RATE;
    //Parametrii WahWah
    int Pvolume;
    int Ppanning;
    int Pdepth;		//the depth of the lfo of the WahWah
    int Pampsns;	//how the filter varies according to the input amplitude
    int Pampsnsinv;	//if the filter freq is lowered if the input amplitude rises
    int Pampsmooth;	//how smooth the input amplitude changes the filter
    int Pmode;      //settings of filters

    //Control Parametrii
    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setdepth (int _Pdepth);
    void setampsns (int _Pampsns);

    void reinitfilter ();

    //Valorile interne

    float panning, depth, ampsns, ampsmooth;
    float ms1, ms2, ms3, ms4;	//mean squares
    
    EffectLFO* lfo;		//lfo-ul WahWah

    class Filter *filterl, *filterr;
    class FilterParams *filterpars;

};

#endif
