/*
  ZynAddSubFX - a software synthesizer

  Chorus.C - Chorus and Flange effects
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

/* 
 * File:   Flanger.h
 * Author: sspresto
 *
 * Created on January 25, 2021, 7:16 PM
 */

#ifndef FLANGER_H
#define FLANGER_H

#include "Chorus.h"

#define FLANGELV2_URI "https://github.com/Stazed/rakarrack-plus#Flange"
#define FLANGE_NAME "RakarrackPlus Flanger"

const int C_FLANGER_PARAMETERS = 13;

enum Flanger_Index
{
    Flanger_DryWet = 0,
    Flanger_Pan,
    Flanger_LFO_Tempo,
    Flanger_LFO_Random,
    Flanger_LFO_Type,
    Flanger_LFO_Stereo,
    Flanger_Depth,
    Flanger_Delay,
    Flanger_Feedback,
    Flanger_LR_Cross,
    Flanger_SKIP_Flange_10,
    Flanger_Subtract,
    Flanger_Intense
};

static const char *flange_parameters[] = 
{
    strdup(NTS(Flanger_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Flanger_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Flanger_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Flanger_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(Flanger_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(Flanger_LFO_Stereo).c_str()), "LFO L/R Delay",  "WIDTH", 
    strdup(NTS(Flanger_Depth).c_str()), "Depth",  "DEPTH", 
    strdup(NTS(Flanger_Delay).c_str()), "Delay",  "DELAY", 
    strdup(NTS(Flanger_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(Flanger_LR_Cross).c_str()), "Left/Right Crossover",  "LRCR", 
    strdup(NTS(Flanger_Subtract).c_str()), "Subtract",  "SUB", 
    strdup(NTS(Flanger_Intense).c_str()), "Intense",  "INTENSE"
};

class Flanger :public Chorus
{
public:
    Flanger(double sample_rate, uint32_t intermediate_bufsize);
    void setpreset (int npreset);
    int get_number_efx_parameters () {return C_FLANGER_PARAMETERS;};

    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return FLANGELV2_URI; };
    virtual std::string get_name(int) {return FLANGE_NAME; };

private:

};

#endif /* FLANGER_H */

