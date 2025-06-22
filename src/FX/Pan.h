/*
  rakarrack - a guitar effects software

  pan.h  -  Auto/Pan - Extra stereo definitions
  Copyright (C) 2008-2010 Josep Andreu
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

#ifndef AUTOPAN_H
#define AUTOPAN_H

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"

#define PANLV2_URI "https://github.com/Stazed/rakarrack-plus#pan"
#define PAN_NAME "RakarrackPlus Pan"

const int C_PAN_PARAMETERS = 9;

enum Pan_Index
{
    Pan_DryWet = 0,
    Pan_Pan,
    Pan_LFO_Tempo,
    Pan_LFO_Random,
    Pan_LFO_Type,
    Pan_LFO_Stereo,
    Pan_Ex_St_Amt,
    Pan_AutoPan,
    Pan_Enable_Extra
};

static const char *pan_parameters[] = 
{
    strdup(NTS(Pan_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Pan_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Pan_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Pan_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(Pan_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(Pan_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDL", 
    strdup(NTS(Pan_Ex_St_Amt).c_str()), "Extra Stereo Amount",  "EXTRA_AMOUNT", 
    strdup(NTS(Pan_AutoPan).c_str()), "Autopan",  "AUTO", 
    strdup(NTS(Pan_Enable_Extra).c_str()), "Extra Stereo Enable",  "EXTRA_ON"
};

class Pan : public Effect
{

public:
    Pan (double sample_rate, uint32_t intermediate_bufsize);
    ~Pan ();
    void out (float *efxoutl, float *efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_PAN_PARAMETERS;};
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return PANLV2_URI; };
    virtual std::string get_name(int) {return PAN_NAME; };

private:
    uint32_t PERIOD;
    float fPERIOD;

    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setextra (int _Pextra);

    int Pvolume;
    int Ppanning;
    int Pextra;
    int PAutoPan;
    int PextraON;


    float dvalue,cdvalue,sdvalue;
    float panning, mul;
    float lfol, lfor;
    float ll, lr;

    EffectLFO* lfo;

};

#endif
