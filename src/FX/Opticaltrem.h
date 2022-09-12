/*
  rakarrack - a guitar effects software

 Opticaltrem.h  -  Opticaltrem Effect definitions

  Copyright (C) 2008-2010 Ryan Billing
  Author: Josep Andreu & Ryan Billing

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


#ifndef Opticaltrem_H
#define Opticaltrem_H

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"

#define OPTTREMLV2_URI "https://github.com/Stazed/rakarrack-plus#Otrem"
#define OPTTREM_NAME "RakarrackPlus OpticalTrem"

const int C_OPTICAL_PARAMETERS = 7;

enum Optical_Index
{
    Optical_Depth = 0,
    Optical_LFO_Tempo,
    Optical_LFO_Random,
    Optical_LFO_Type,
    Optical_LFO_Stereo,
    Optical_Pan,
    Optical_Invert
};

static const char *optical_parameters[] = 
{
    strdup(NTS(Optical_Depth).c_str()), "Depth",  "DEPTH", 
    strdup(NTS(Optical_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Optical_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(Optical_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(Optical_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDF", 
    strdup(NTS(Optical_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Optical_Invert).c_str()), "Invert",  "INV"
};

class Opticaltrem : public Effect
{

public:

    Opticaltrem (double sample_rate, uint32_t intermediate_bufsize);
    ~Opticaltrem ();

    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpanning(int value);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_OPTICAL_PARAMETERS;};
    void set_random_parameters();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return OPTTREMLV2_URI; };
    virtual std::string get_name(int) {return OPTTREM_NAME; };
    
private:
    float cSAMPLE_RATE;
    uint32_t PERIOD;
    int Pdepth;
    int Ppanning;
    int Pinvert;  //Invert the opto and resistor relationship
 
    float Ra, Rb, R1, Rp, b, dTC, dRCl, dRCr, minTC, alphal, alphar, stepl, stepr, oldstepl, oldstepr, fdepth;
    float lstep,rstep;
    float cperiod;
    float gl, oldgl;
    float gr, oldgr;
    float rpanning, lpanning;

    EffectLFO* lfo;

};

#endif
