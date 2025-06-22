/*
  Rakarrack

  Cabinet.C - eq curves to approximate a guitar cabinet
  Copyright (C) 2015 Spencer Jackson
  Author: Spencer Jackson

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

#ifndef CABINET_H
#define CABINET_H

#include "EQ.h"

#define CABLV2_URI "https://github.com/Stazed/rakarrack-plus#cabe"
#define CAB_NAME "RakarrackPlus Cabinet"

/* Used for Gain and Preset setting */
const int C_CABINET_PARAMETERS = 2;

enum Cabinet_Index
{
    Cabinet_Preset_Idx = 0,
    Cabinet_Gain
};

static const char *cab_parameters[] = 
{
    strdup(NTS(Cabinet_Preset_Idx).c_str()), "Preset",  "PRESET",
    strdup(NTS(Cabinet_Gain).c_str()), "Gain",  "GAIN"
};

class Cabinet :public EQ
{
public:
    Cabinet (double sample_frequency, uint32_t intermediate_bufsize);
    
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset(int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    int get_number_efx_parameters () {return C_CABINET_PARAMETERS;};
    void set_random_parameters();
    
    void initialize();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return CABLV2_URI; };
    virtual std::string get_name(int) {return CAB_NAME; };

};

#endif
