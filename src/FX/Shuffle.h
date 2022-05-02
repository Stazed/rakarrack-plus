/*

  Shuffle.h - Stereo Shuffle

  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul
  ZynAddSubFX - a software synthesizer

  Modified for rakarrack by Josep Andreu & Hernan Ordiales

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

#ifndef SHUFFLE_H
#define SHUFFLE_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"

#define SHUFFLELV2_URI "https://github.com/Stazed/rakarrack-plus#Shuffle"

const int C_SHUFFLE_PARAMETERS = 11;

enum Shuffle_Index
{
    Shuffle_DryWet = 0,
    Shuffle_Gain_L,
    Shuffle_Gain_ML,
    Shuffle_Gain_MH,
    Shuffle_Gain_H,
    Shuffle_Freq_L,
    Shuffle_Freq_ML,
    Shuffle_Freq_MH,
    Shuffle_Freq_H,
    Shuffle_Width,
    Shuffle_F_Band
};


class Shuffle : public Effect
{
public:
    Shuffle (double sample_rate, uint32_t intermediate_bufsize);
    ~Shuffle ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_SHUFFLE_PARAMETERS;};
    void set_random_parameters();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI() { return SHUFFLELV2_URI; };
    
    void initialize();
    void clear_initialize();

private:

    float *inputl;
    float *inputr;

    void setvolume (int Pvolume);
    void setCross1 (int value);
    void setCross2 (int value);
    void setCross3 (int value);
    void setCross4 (int value);
    void setGainL (int value);
    void setGainML (int value);
    void setGainMH (int value);
    void setGainH (int value);
    void set_q(int value);

    uint32_t PERIOD;
    double fSAMPLE_RATE;
    
    //Parametrii
    int Pvolume;	//Volumul or E/R
    int PvolL;
    int PvolML;
    int PvolMH;
    int PvolH;
    int PQ;
    int E;

    int Cross1;		//lowpass filter
    int Cross2;		//highpass filter
    int Cross3;
    int Cross4;

    //Parametrii reali
    float tmp;
    float volL,volML,volMH,volH;
    AnalogFilter  *lr, *hr;
    AnalogFilter  *mlr,*mhr;
    float* interpbuf; //buffer for filters

};


#endif
