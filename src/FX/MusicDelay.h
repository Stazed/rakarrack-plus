/*
  ZynAddSubFX - a software synthesizer

  Echo.h - Echo Effect
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

#ifndef MUSDELAY_H
#define MUSDELAY_H

#include "Effect.h"

#define MDELLV2_URI "https://github.com/Stazed/rakarrack-plus#delm"
#define MDEL_NAME "RakarrackPlus Musical Delay"

const int C_MUSIC_PARAMETERS = 13;

enum Music_Index
{
    Music_DryWet = 0,
    Music_Pan_1,
    Music_Delay_1,
    Music_Del_Offset,
    Music_LR_Cross,
    Music_Feedback_1,
    Music_Damp,
    Music_Pan_2,
    Music_Delay_2,
    Music_Feedback_2,
    Music_Tempo,
    Music_Gain_1,
    Music_Gain_2
};

static const char *mdel_parameters[] = 
{
    strdup(NTS(Music_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Music_Pan_1).c_str()), "Panning 1",  "PAN1", 
    strdup(NTS(Music_Delay_1).c_str()), "Delay 1",  "DEL1", 
    strdup(NTS(Music_Del_Offset).c_str()), "Delay Offset",  "OFFS", 
    strdup(NTS(Music_LR_Cross).c_str()), "Left/Right Crossover",  "LRCr", 
    strdup(NTS(Music_Feedback_1).c_str()), "Feedback 1",  "FB1", 
    strdup(NTS(Music_Damp).c_str()), "Damping",  "DAMP", 
    strdup(NTS(Music_Pan_2).c_str()), "Panning 2",  "PAN2", 
    strdup(NTS(Music_Delay_2).c_str()), "Delay 2",  "DEL2", 
    strdup(NTS(Music_Feedback_2).c_str()), "Feedback 2",  "FB2", 
    strdup(NTS(Music_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Music_Gain_1).c_str()), "Gain 1",  "GAIN1", 
    strdup(NTS(Music_Gain_2).c_str()), "Gain 2",  "GAIN2"
};

class MusicDelay : public Effect
{
public:
    MusicDelay (double sample_rate, uint32_t intermediate_bufsize);
    ~MusicDelay ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_MUSIC_PARAMETERS;};
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return MDELLV2_URI; };
    virtual std::string get_name(int) {return MDEL_NAME; };

private:

    void setvolume (int _Pvolume);
    void setpanning (int num, int _Ppanning);
    void setdelay (int num, int _Pdelay);
    void setgain (int num, int _Pgain);
    void setlrdelay (int _Plrdelay);
    void setlrcross (int _Plrcross);
    void setfb (int num, int _Pfb);
    void sethidamp (int _Phidamp);
    void settempo (int _Ptempo);
    void initdelays ();

    uint32_t PERIOD;
    float fSAMPLE_RATE;
    //Parametrii
    int Pvolume;			//Volumul or E/R
    int Ppanning1;		//Panning
    int Ppanning2;
    int Pgain1;
    int Pgain2;
    int Pdelay1;
    int Pdelay2;
    int Plrdelay;			// L/R delay difference
    int Plrcross;			// L/R Mixing
    int Pfb1;			//Feed-back-ul
    int Pfb2;
    int Phidamp;
    int Ptempo;

    //Parametrii reali
    int maxx_delay;
    int dl1, dr1, dl2, dr2, delay1, delay2, lrdelay;
    int kl1, kr1, kl2, kr2;
    float panning1, panning2, lrcross, fb1, fb2, hidamp;
    float gain1, gain2;
    float *ldelay1, *rdelay1, *ldelay2, *rdelay2;
    float oldl1, oldr1, oldl2, oldr2;	//pt. lpf

};


#endif
