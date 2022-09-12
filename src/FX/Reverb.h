/*
  ZynAddSubFX - a software synthesizer

  Reverb.h - Reverberation effect
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

#ifndef REVERB_H
#define REVERB_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"

#define REVELV2_URI "https://github.com/Stazed/rakarrack-plus#reve"
#define REVE_NAME "RakarrackPlus Reverb"

const int C_REVERB_PARAMETERS = 12;

enum Reverb_Index
{
    Reverb_DryWet = 0,
    Reverb_Pan,
    Reverb_Time,
    Reverb_I_Delay,
    Reverb_Delay_FB,
    Reverb_SKIP_5,
    Reverb_SKIP_6,
    Reverb_LPF,
    Reverb_HPF,
    Reverb_Damp,
    Reverb_Type,
    Reverb_Room
};

static const char *reverb_parameters[] = 
{
    strdup(NTS(Reverb_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Reverb_Pan).c_str()), "Panning",  "PAN", 
    strdup(NTS(Reverb_Time).c_str()), "Time",  "TIME", 
    strdup(NTS(Reverb_I_Delay).c_str()), "Initial Delay",  "IDEL", 
    strdup(NTS(Reverb_Delay_FB).c_str()), "Initial Delay Feedback",  "IDELFB", 
    strdup(NTS(Reverb_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(Reverb_HPF).c_str()), "Highpass Filter",  "HPF", 
    strdup(NTS(Reverb_Damp).c_str()), "Damping",  "DAMP", 
    strdup(NTS(Reverb_Type).c_str()), "Type",  "TYPE", 
    strdup(NTS(Reverb_Room).c_str()), "Room Size",  "RS"
};

class Reverb : public Effect
{
public:
    Reverb (double samplerate, uint16_t intermediate_bufsize);
    ~Reverb ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_REVERB_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return REVELV2_URI; };
    virtual std::string get_name(int) {return REVE_NAME; };
    
    void initialize();
    void clear_initialize();

private:

    void setvolume (int _Pvolume);
    void setpan (int _Ppan);
    void settime (int _Ptime);
    void setlohidamp (int _Plohidamp);
    void setidelay (int _Pidelay);
    void setidelayfb (int _Pidelayfb);
    void sethpf (int value);
    void setlpf (int value);
    void settype (int _Ptype);
    void setroomsize (int _Proomsize);
    void processmono (unsigned int ch, float * output);

    float fSAMPLE_RATE;
    uint32_t PERIOD;

    //Parametrii
    //Amount of the reverb,
    int Pvolume;

    //LefT/Right Panning
    int Ppan;

    //duration of reverb
    int Ptime;

    //Initial delay
    int Pidelay;

    //Initial delay feedback
    int Pidelayfb;

    //delay between ER/Reverbs
    int Prdelay;

    //EarlyReflections/Reverb Balance
    int Perbalance;

    //HighPassFilter
    int Plpf;

    //LowPassFilter
    int Phpf;

    //Low/HighFrequency Damping
    int Plohidamp;	// 0..63 lpf,64=off,65..127=hpf(TODO)

    //Reverb type
    int Ptype;

    //Room Size
    int Proomsize;

    //Parametrii 2
    int lohidamptype;		//0=disable,1=highdamp(lowpass),2=lowdamp(highpass))
    int idelaylen, rdelaylen;
    int idelayk;
    int comblen[REV_COMBS * 2];
    int aplen[REV_APS * 2];


    int combk[REV_COMBS * 2];
    int apk[REV_APS * 2];

    float lohifb, idelayfb, roomsize, rs;	//rs is used to "normalise" the volume according to the roomsize
    float rs_coeff;
    //parameter control
    float pan, erbalance;


    //Valorile interne

    float *comb[REV_COMBS * 2];

    float combfb[REV_COMBS * 2];	//feedback-ul fiecarui filtru "comb"
    float lpcomb[REV_COMBS * 2];	//pentru Filtrul LowPass

    float *ap[REV_APS * 2];
    float *inputbuf;
    float *idelay;

    float* interpbuf; //buffer for filters
    class AnalogFilter *lpf, *hpf;	//filters
};




#endif
