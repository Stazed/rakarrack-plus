/*
  rakarrack - Audio effects software

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
 * File:   ResSolution.h
 * Author: sspresto
 *
 * Created on June 24, 2024, 4:26 PM
 */

#ifndef RESSOLUTION_H
#define RESSOLUTION_H

#include "APhaser.h"

#define RESSOLLV2_URI "https://github.com/Stazed/rakarrack-plus#ressol"
#define RESSOL_NAME "RakarrackPlus ResSolution"

const int C_RESSOL_PARAMETERS = 13;

enum Ressol_Index
{
    Ressol_DryWet = 0,
    Ressol_Distortion,
    Ressol_LFO_Tempo,       // Label is 'Shift' for ResSolution
    Ressol_LFO_Random,      // Not used here
    Ressol_LFO_Type,        // Fixed to static
    Ressol_LFO_Stereo,
    Ressol_Width,
    Ressol_Feedback,
    Ressol_Stages,
    Ressol_Mismatch,
    Ressol_Subtract,
    Ressol_Depth,
    Ressol_Hyper
};

static const char *ressol_parameters[] = 
{
    strdup(NTS(Ressol_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Ressol_Distortion).c_str()), "Distort",  "DISTORT", 
    strdup(NTS(Ressol_LFO_Tempo).c_str()), "Shift",  "SHIFT", 
    strdup(NTS(Ressol_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDL", 
    strdup(NTS(Ressol_Width).c_str()), "Width",  "WIDTH", 
    strdup(NTS(Ressol_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(Ressol_Stages).c_str()), "Stages",  "STAGES", 
    strdup(NTS(Ressol_Mismatch).c_str()), "Mismatch",  "MISMATCH", 
    strdup(NTS(Ressol_Subtract).c_str()), "Subtract",  "SUB", 
    strdup(NTS(Ressol_Depth).c_str()), "Phase Depth",  "DEPTH", 
    strdup(NTS(Ressol_Hyper).c_str()), "Hyper",  "HYPER"
};

class ResSolution :public Analog_Phaser
{
public:
    ResSolution (double sample_rate, uint32_t intermediate_bufsize);

    void setpreset (int npreset);
    void changepar (int npar, int value);
    int get_number_efx_parameters () {return C_RESSOL_PARAMETERS;};

    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return RESSOLLV2_URI; };
    virtual std::string get_name(int) {return RESSOL_NAME; };

private:

};

#endif /* RESSOLUTION_H */

