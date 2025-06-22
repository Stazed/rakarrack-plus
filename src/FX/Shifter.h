/*
  rakarrack - a guitar effects software

  PitchShifter.h  -  Shifterr definitions
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

#ifndef SHIFTER_H
#define SHIFTER_H

#include <vector>
#include "Effect.h"
#include "../EFX_common/smbPitchShift.h"
#include "../EFX_common/Resample.h"

#define SHIFTERLV2_URI "https://github.com/Stazed/rakarrack-plus#Shifter"
#define SHIFTER_NAME "RakarrackPlus Shifter"


#define IDLE 0
#define UP   1
#define WAIT 2
#define DOWN 3

const int C_SHIFTER_PARAMETERS = 10;

enum Shifter_Index
{
    Shifter_DryWet = 0,
    Shifter_Pan,
    Shifter_Gain,
    Shifter_Attack,
    Shifter_Decay,
    Shifter_Threshold,
    Shifter_Interval,
    Shifter_Shift,
    Shifter_Mode,
    Shifter_Whammy
};

static const char *shift_parameters[] = 
{
    strdup(NTS(Shifter_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Shifter_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Shifter_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Shifter_Attack).c_str()), "Attack",  "ATACK", 
    strdup(NTS(Shifter_Decay).c_str()), "Decay",  "DECAY", 
    strdup(NTS(Shifter_Threshold).c_str()), "Threshold",  "THRESH", 
    strdup(NTS(Shifter_Interval).c_str()), "Interval",  "INT", 
    strdup(NTS(Shifter_Shift).c_str()), "Shift Down",  "DN", 
    strdup(NTS(Shifter_Mode).c_str()), "Mode",  "MODE", 
    strdup(NTS(Shifter_Whammy).c_str()), "Whammy",  "WHAMMY"
};

class Shifter : public Effect
{

public:
    Shifter (long int Quality, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsize);
    ~Shifter ();
    void out (float *efxoutl, float *efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar(int npar);
    void cleanup();
    int get_number_efx_parameters () {return C_SHIFTER_PARAMETERS;};
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return SHIFTERLV2_URI; };
    virtual std::string get_name(int) {return SHIFTER_NAME; };
    
    void initialize();
    void clear_initialize();
    void applyfilters (float * efxoutl);
    void adjust(int DS, double sample_rate);
    
    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

    long int hq;
    float *outi;
    float *outo;

private:

    void setvolume (int Pvolume);
    void setpanning (int Ppan);
    void setinterval (int Pinterval);
    void setgain (int Pgain);

    int DS_state;
    uint32_t PERIOD;
    double fSAMPLE_RATE;
    int Pvolume;
    int Pgain;
    int Ppan;
    int Pinterval;
    int Pupdown;
    int Pmode;
    int Pattack;
    int Pdecay;
    int Pthreshold;
    int Pwhammy;
    int state;
    int nPERIOD;
    int nSAMPLE_RATE;
    float nRATIO;
    long window;

    double u_up;
    double u_down;
    float nfSAMPLE_RATE;
    float env, t_level, td_level, tz_level;
    float a_rate,d_rate,tune, range, whammy;
    float panning;
    float gain;
    float interval;
    float *templ, *tempr;

    Resample *U_Resample;
    Resample *D_Resample;


    PitchShifter *PS;

};

#endif
