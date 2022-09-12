/*
  Rakarrack   Audio FX software
  CoilCrafter.h - Pick Up Emulation
  Using RBFilter
  Using Steve Harris LADSPA Plugin harmonic_gen
  Modified for rakarrack by Ryan Billing & Josep Andreu

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

#ifndef COILCRAFTER_H
#define COILCRAFTER_H

#include "Effect.h"
#include "../EFX_common/HarmonicEnhancer.h"
#include "../EFX_common/AnalogFilter.h"

#define COILLV2_URI "https://github.com/Stazed/rakarrack-plus#CoilCrafter"
#define COIL_NAME "RakarrackPlus Coil Crafter"

const int C_COIL_PARAMETERS = 9;

enum Coil_Index
{
    Coil_Gain = 0,
    Coil_Origin,
    Coil_Destiny,
    Coil_Freq_1,
    Coil_Q_1,
    Coil_Freq_2,
    Coil_Q_2,
    Coil_Tone,
    Coil_NeckMode
};

static const char *coil_parameters[] = 
{
    strdup(NTS(Coil_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Coil_Freq_1).c_str()), "Origin Pickup Frequency",  "F1", 
    strdup(NTS(Coil_Q_1).c_str()), "Origin Pickup Resonance",  "Q1", 
    strdup(NTS(Coil_Freq_2).c_str()), "Destination Pickup Frequency",  "F2", 
    strdup(NTS(Coil_Q_2).c_str()), "Destination Pickup Resonance",  "Q2", 
    strdup(NTS(Coil_Tone).c_str()), "Tone",  "TONE", 
    strdup(NTS(Coil_NeckMode).c_str()), "Neck Pickup",  "MODE"
};

class CoilCrafter : public Effect
{
public:
    CoilCrafter (double sample_rate, uint32_t intermediate_bufsize);
    ~CoilCrafter ();
    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_COIL_PARAMETERS;};
    void set_random_parameters();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return COILLV2_URI; };
    virtual std::string get_name(int) {return COIL_NAME; };

private:

    uint32_t PERIOD;
    
    void setvolume (int value);
    void sethpf (int value);
    void setfreq1();
    void setfreq2();
    void setq1();
    void setq2();

    int Pvolume;
    int Ppo;
    int Ppd;
    int Ptone;
    int Pq1;
    int Pfreq1;
    int Pq2;
    int Pfreq2;
    int Pmode;

    float tfreqs[10];
    float tqs[10];
    float rm[10];
    float freq1,q1,freq2,q2;
    float att;

    class HarmEnhancer *harm;
    class AnalogFilter *RB1l, *RB1r;
    class AnalogFilter *RB2l,*RB2r;
    float* interpbuf;

};


#endif
