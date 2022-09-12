/*
  Rakarrack   Audio FX software
  Exciter.h - Harmonic Enhancer
  Based in Steve Harris LADSPA Plugin harmonic_gen
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

#ifndef EXCITER_H
#define EXCITER_H

#include "Effect.h"
#include "../EFX_common/HarmonicEnhancer.h"

#define EXCITERLV2_URI "https://github.com/Stazed/rakarrack-plus#Exciter"
#define EXCITER_NAME "RakarrackPlus Exciter"

const int C_EXCITER_PARAMETERS = 13;

enum Exciter_index
{
    Exciter_Gain = 0,
    Exciter_Harm_1,
    Exciter_Harm_2,
    Exciter_Harm_3,
    Exciter_Harm_4,
    Exciter_Harm_5,
    Exciter_Harm_6,
    Exciter_Harm_7,
    Exciter_Harm_8,
    Exciter_Harm_9,
    Exciter_Harm_10,
    Exciter_LPF,
    Exciter_HPF
};

static const char *exciter_parameters[] = 
{
    strdup(NTS(Exciter_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Exciter_Harm_1).c_str()), "1st Harmonic",  "HAR1", 
    strdup(NTS(Exciter_Harm_2).c_str()), "2nd Harmonic",  "HAR2", 
    strdup(NTS(Exciter_Harm_3).c_str()), "3rd Harmonic",  "HAR3", 
    strdup(NTS(Exciter_Harm_4).c_str()), "4th Harmonic",  "HAR4", 
    strdup(NTS(Exciter_Harm_5).c_str()), "5th Harmonic",  "HAR5", 
    strdup(NTS(Exciter_Harm_6).c_str()), "6th Harmonic",  "HAR6", 
    strdup(NTS(Exciter_Harm_7).c_str()), "7th Harmonic",  "HAR7", 
    strdup(NTS(Exciter_Harm_8).c_str()), "8th Harmonic",  "HAR8", 
    strdup(NTS(Exciter_Harm_9).c_str()), "9th Harmonic",  "HAR9", 
    strdup(NTS(Exciter_Harm_10).c_str()), "10th Harmonic",  "HAR10", 
    strdup(NTS(Exciter_LPF).c_str()), "Low-Pass Filter Cutoff",  "LPF", 
    strdup(NTS(Exciter_HPF).c_str()), "High-Pass Filter Cutoff",  "HPF"
};

class Exciter : public Effect
{
public:
    Exciter (double sample_rate, uint32_t intermediate_bufsize);
    ~Exciter ();
    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_EXCITER_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return EXCITERLV2_URI; };
    virtual std::string get_name(int) {return EXCITER_NAME; };

private:

    void setvolume (int value);
    void setlpf (int value);
    void sethpf (int value);
    void sethar(int num, int value);

    uint32_t PERIOD;
    int Prm[10];
    int Pvolume;
    int lpffreq;
    int hpffreq;

    float rm[10];

    class HarmEnhancer *harm;

};


#endif
