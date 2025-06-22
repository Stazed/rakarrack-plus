/*
  Rakarrack   Audio FX software
  ShelfBoost.h - Tone Booster
  Modified for rakarrack by Ryan Billing & Josep Andreu

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

#ifndef SHELFBOOST_H
#define SHELFBOOST_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"

#define SHELFLV2_URI "https://github.com/Stazed/rakarrack-plus#ShelfBoost"
#define SHELF_NAME "RakarrackPlus Shelf Boost"

const int C_SHELF_PARAMETERS = 5;

enum Shelf_Index
{
    Shelf_Gain = 0,
    Shelf_Presence,
    Shelf_Tone,
    Shelf_Stereo,
    Shelf_Level
};

static const char *shelf_parameters[] = 
{
    strdup(NTS(Shelf_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Shelf_Presence).c_str()), "Presence",  "PRES", 
    strdup(NTS(Shelf_Tone).c_str()), "Tone",  "TONE", 
    strdup(NTS(Shelf_Stereo).c_str()), "Stereo",  "STEREO", 
    strdup(NTS(Shelf_Level).c_str()), "Level",  "LEVEL"
};

class ShelfBoost : public Effect
{
public:
    ShelfBoost (double sample_rate, uint32_t intermediate_bufsize);
    ~ShelfBoost ();
    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_SHELF_PARAMETERS;};
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return SHELFLV2_URI; };
    virtual std::string get_name(int) {return SHELF_NAME; };
    
    void initialize();
    void clear_initialize();
    void set_q(int value);
    void set_freq(int value);
    void set_level(int value);

private:

    void setvolume (int value);
    
    uint32_t PERIOD;
    double fSAMPLE_RATE;
    int Pvolume;
    int Plevel;
    int Pq1;
    int Pfreq1;
    int Pstereo;

    float freq1,q1,gain,u_gain;

    class AnalogFilter *RB1l, *RB1r;
    float* interpbuf; //buffer for filters

};


#endif
