/*
  Rakarrack Guitar FX

  Sustainer.h - Simple compressor/sustainer effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef SUSTAINER_H
#define SUSTAINER_H

#include "Effect.h"

#define SUSTAINLV2_URI "https://github.com/Stazed/rakarrack-plus#Sustainer"
#define SUSTAIN_NAME "RakarrackPlus Sustainer"

const int C_SUSTAIN_PARAMETERS = 2;

enum Sustain_Index
{
    Sustain_Gain = 0,
    Sustain_Sustain
};

static const char *sus_parameters[] = 
{
    strdup(NTS(Sustain_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Sustain_Sustain).c_str()), "Sustain",  "SUS"
};

class Sustainer : public Effect
{
public:
    Sustainer (double sample_rate, uint32_t intermediate_bufsize);
    ~Sustainer ();
    void cleanup ();
    int get_number_efx_parameters () {return C_SUSTAIN_PARAMETERS;};
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return SUSTAINLV2_URI; };
    virtual std::string get_name(int) {return SUSTAIN_NAME; };
    
    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void changepar (int npar, int value);
    int getpar (int npar);
    void setpreset (int npreset);

private:

    uint32_t PERIOD;

    //Parametrii
    int Pvolume;	//Output Level
    int Psustain;	//Compression amount

    int timer, hold;
    float level, fsustain, input,tmpgain;
    float prls, compeak, compg, compenv, oldcompenv, calpha, cbeta, cthresh, cratio, cpthresh;

};


#endif


