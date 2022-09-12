/*
  ZynAddSubFX - a software synthesizer

  Distorsion.h - Distorsion Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu & Hernan Ordiales & Ryan Billing

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

/* 
 * File:   Overdrive.h
 * Author: sspresto
 *
 * Created on January 26, 2021, 8:16 AM
 */

#ifndef OVERDRIVE_H
#define OVERDRIVE_H

#include "Distorsion.h"

#define OVERDRIVELV2_URI "https://github.com/Stazed/rakarrack-plus#Overdrive"
#define OVERDRIVE_NAME "RakarrackPlus Overdrive"

const int C_OVERDRIVE_PARAMETERS = 13;

enum Overdrive_Index
{
    Overdrive_DryWet = 0,
    Overdrive_Pan,
    Overdrive_LR_Cross,
    Overdrive_Drive,
    Overdrive_Level,
    Overdrive_Type,
    Overdrive_Negate,
    Overdrive_LPF,
    Overdrive_HPF,
    Overdrive_Stereo,
    Overdrive_Prefilter,
    Overdrive_SKIP_11,
    Overdrive_Suboctave
};

static const char *overdrive_parameters[] = 
{
    strdup(NTS(Overdrive_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Overdrive_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Overdrive_LR_Cross).c_str()), "Left/Right Crossover",  "LRCr", 
    strdup(NTS(Overdrive_Drive).c_str()), "Drive",  "DRIVE", 
    strdup(NTS(Overdrive_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Overdrive_Type).c_str()), "Type",  "TYPE", 
    strdup(NTS(Overdrive_Negate).c_str()), "Negate (Polarity Switch)",  "NEG", 
    strdup(NTS(Overdrive_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(Overdrive_HPF).c_str()), "Highpass Filter",  "HPF", 
    strdup(NTS(Overdrive_Stereo).c_str()), "Stereo",  "STEREO", 
    strdup(NTS(Overdrive_Prefilter).c_str()), "Prefilter",  "PREFILTER"
};

class Overdrive: public Distorsion
{
public:
    Overdrive(int wave_res, int wave_upq, int wave_dnq, double samplerate, uint32_t intermediate_bufsize);

    void setpreset (int npreset);
    int get_number_efx_parameters () {return C_OVERDRIVE_PARAMETERS;};
    void set_random_parameters();
    
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return OVERDRIVELV2_URI; };
    virtual std::string get_name(int) {return OVERDRIVE_NAME; };

};

#endif /* OVERDRIVE_H */

