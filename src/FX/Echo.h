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

#ifndef ECHO_H
#define ECHO_H

#include "Effect.h"
#include "../EFX_common/delayline.h"

#define ECHOLV2_URI "https://github.com/Stazed/rakarrack-plus#eco"
#define ECHO_NAME "RakarrackPlus Echo"

const int C_ECHO_PARAMETERS = 9;

enum Echo_Index
{
    Echo_DryWet = 0,
    Echo_Pan,
    Echo_Delay,
    Echo_LR_Delay,
    Echo_LR_Cross,
    Echo_Feedback,
    Echo_Damp,
    Echo_Reverse,
    Echo_Direct
};

static const char *echo_parameters[] = 
{
    strdup(NTS(Echo_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Echo_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Echo_Delay).c_str()), "Delay",  "DELAY", 
    strdup(NTS(Echo_LR_Delay).c_str()), "Left/Right Delay Offset",  "LR_DELAY", 
    strdup(NTS(Echo_LR_Cross).c_str()), "Left/Right Crossover",  "LRCr", 
    strdup(NTS(Echo_Feedback).c_str()), "Feedback",  "FEEDBACK", 
    strdup(NTS(Echo_Damp).c_str()), "Damping",  "DAMP", 
    strdup(NTS(Echo_Reverse).c_str()), "Reverse",  "REV", 
    strdup(NTS(Echo_Direct).c_str()), "Direct",  "DIRECT"
};

class Echo : public Effect
{
public:
    Echo (double sample_rate, uint32_t intermediate_bufsize);
    ~Echo ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_ECHO_PARAMETERS;};
    void set_random_parameters();
    
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return ECHOLV2_URI; };
    virtual std::string get_name(int) {return ECHO_NAME; };

    void Tempo2Delay(int delay);

private:
    
    uint32_t PERIOD;
    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Ppanning;	//Panning
    int Pdelay;
    int Plrdelay;	// L/R delay difference
    int Plrcross;	// L/R Mixing
    int Pfb;		//Feed-back-ul
    int Phidamp;
    int Preverse;
    int Pdirect;

    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setdelay (int _Pdelay);
    void setlrdelay (int _Plrdelay);
    void setlrcross (int _Plrcross);
    void setfb (int _Pfb);
    void sethidamp (int _Phidamp);
    void setreverse (int _Preverse);
    void setdirect (int _Pdirect);

    //Parametrii reali
    void initdelays ();

    int dl, dr;
    float delay, lrdelay;
    int kl, kr, rvkl, rvkr, rvfl, rvfr, maxx_delay, fade;

    class delayline *ldelay, *rdelay;
    float ltime, rtime;
    float oldl, oldr;		//pt. lpf
    float panning, lrcross, fb, hidamp, reverse, ireverse;
    float  Srate_Attack_Coeff;

};


#endif
