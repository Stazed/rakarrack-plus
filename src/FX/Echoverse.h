/*
  ZynAddSubFX - a software synthesizer

  Echo.h - Echo Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

  Reverse Echo by Transmogrifox

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

// Renamed from RBEcho.h on 4/19/2020 by stazed

#ifndef ECHOVERSE_H
#define ECHOVERSE_H

#include "Effect.h"
#include "../EFX_common/delayline.h"

#define ECHOVERSELV2_URI "https://github.com/Stazed/rakarrack-plus#Echoverse"
#define ECHOVERSE_NAME "RakarrackPlus Echoverse"

const int C_ECHOVERSE_PARAMETERS = 10;

enum Echoverse_Index
{
    Echoverse_DryWet = 0,
    Echoverse_Pan,
    Echoverse_Tempo,
    Echoverse_LR_Delay,
    Echoverse_Angle,
    Echoverse_Feedback,
    Echoverse_Damp,
    Echoverse_Reverse,
    Echoverse_Subdivision,
    Echoverse_Ext_Stereo
};

static const char *echoverse_parameters[] = 
{
    strdup(NTS(Echoverse_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Echoverse_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Echoverse_Tempo).c_str()), "Tempo",  "DELAY", 
    strdup(NTS(Echoverse_LR_Delay).c_str()), "Left/Right Delay Offset",  "LR_DELAY", 
    strdup(NTS(Echoverse_Angle).c_str()), "Angle",  "ANGLE", 
    strdup(NTS(Echoverse_Feedback).c_str()), "Feedback",  "FEEDBACK", 
    strdup(NTS(Echoverse_Damp).c_str()), "Damping",  "DAMP", 
    strdup(NTS(Echoverse_Reverse).c_str()), "Reverse",  "REV", 
    strdup(NTS(Echoverse_Subdivision).c_str()), "Subdivision",  "SUBD", 
    strdup(NTS(Echoverse_Ext_Stereo).c_str()), "Extra Stereo",  "ES"
};

class Echoverse : public Effect
{
public:
    Echoverse (double sample_rate, uint32_t intermediate_bufsize);
    ~Echoverse ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_ECHOVERSE_PARAMETERS;};
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return ECHOVERSELV2_URI; };
    virtual std::string get_name(int) {return ECHOVERSE_NAME; };

private:

    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setdelay (int _Pdelay);
    void setlrdelay (int _Plrdelay);
    void setlrcross (int _Plrcross);
    void setfb (int _Pfb);
    void sethidamp (int _Phidamp);
    void setreverse (int _Preverse);
    void initdelays ();

    float fSAMPLE_RATE;
    uint32_t PERIOD;

    //Parametrii
    int Pvolume;	// E/R
    int Ppanning;	//Panning
    int Pdelay;
    int Plrdelay;	// L/R delay difference
    int Plrcross;	// L/R Mixing
    int Pfb;		//Feed-back
    int Phidamp;
    int Preverse;
    int Psubdiv;
    int Pes;

    int maxx_delay;

    float delay, lrdelay, ltime, rtime;
    float fdelay, subdiv, pes, pingpong, ipingpong;
    float rvl, rvr;
    float rpanning, lpanning, lrcross, fb, hidamp, reverse, ireverse, lfeedback, rfeedback;
    float oldl, oldr;		//pt. lpf
    float  Srate_Attack_Coeff;

    class delayline *ldelay, *rdelay;
};


#endif
