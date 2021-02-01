/*
  rakarrack - a guitar effects software

  Harmonizer.h  -  Harmonizer definitions
  Copyright (C) 2008 Josep Andreu
  Author: Josep Andreu

  Using Stephan M. Bernsee smbPitchShifter engine.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef HARMONIZER_H
#define HARMONIZER_H

#include <vector>
#include "Effect.h"
#include "AnalogFilter.h"
#include "smbPitchShift.h"
#include "Resample.h"

const int C_HARM_PARAMETERS = 11;

enum Harm_Index
{
    Harm_DryWet = 0,
    Harm_Pan,
    Harm_Gain,
    Harm_Interval,
    Harm_Filter_Freq,
    Harm_Select,
    Harm_Note,
    Harm_Chord,
    Harm_Filter_Gain,
    Harm_Filter_Q,
    Harm_MIDI
};

class Harmonizer :public Effect
{

public:
    Harmonizer (long int Quality, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsize);
    ~Harmonizer ();
    void out (float *efxoutl, float *efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_HARM_PARAMETERS;};

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void initialize();
    void clear_initialize();
    void applyfilters (float * efxoutl, uint32_t period);
    void adjust(int DS, uint32_t period);
    
    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);
    
    int Pinterval;
    int PMIDI;
    int PSELECT;
    float r_ratio;
    int mira;

    int DS_state;
    int nPERIOD;
    int nSAMPLE_RATE;
    long window;

    long int hq;

    double u_up;
    double u_down;
    float nfSAMPLE_RATE;

    float *outi;
    float *outo;
    float *templ, *tempr;

private:

    int Pvolume;
    int Pgain;
    int Ppan;
    int Pnote;
    int Ptype;

    int fPfreq;
    int fPgain;
    int fPq;

    float panning;
    float gain;
    float interval;
    
    void setvolume (int Pvolume);
    void setpanning (int Ppan);
    void setinterval (int Pinterval);
    void setgain (int Pgain);
    void setMIDI (int PMIDI);
    void fsetfreq (int value);
    void fsetgain (int value);
    void fsetq (int value);


    AnalogFilter *pl;
    float* interpbuf; //buffer for filters

    class Resample *U_Resample;
    class Resample *D_Resample;

    PitchShifter *PS;

    unsigned int SAMPLE_RATE;
    double fSAMPLE_RATE;
    uint32_t PERIOD;
    bool DS_init;
};

#endif
