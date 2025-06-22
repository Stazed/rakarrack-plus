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
#include "../EFX_common/AnalogFilter.h"
#include "../EFX_common/smbPitchShift.h"
#include "../EFX_common/Resample.h"

#define HARMLV2_URI "https://github.com/Stazed/rakarrack-plus#har"
#define HARMNOMIDLV2_URI "https://github.com/Stazed/rakarrack-plus#har_no_mid"
#define HARM_NAME_NO_MIDI "RakarrackPlus Harmonizer (no midi)"
#define HARM_NAME "RakarrackPlus Harmonizer"

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

static const char *harm_parameters[] = 
{
    strdup(NTS(Harm_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Harm_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Harm_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Harm_Interval).c_str()), "Interval",  "INT", 
    strdup(NTS(Harm_Filter_Freq).c_str()), "Filter Frequency",  "FREQ", 
    strdup(NTS(Harm_Select).c_str()), "Select Chord Mode",  "SELECT", 
    strdup(NTS(Harm_Note).c_str()), "Note",  "NOTE", 
    strdup(NTS(Harm_Chord).c_str()), "Chord",  "CHORD", 
    strdup(NTS(Harm_Filter_Gain).c_str()), "Filter Gain",  "FGAIN", 
    strdup(NTS(Harm_Filter_Q).c_str()), "Filter Q",  "FQ", 
    strdup(NTS(Harm_MIDI).c_str()), "MIDI Mode",  "MIDI"
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
    void set_random_parameters();

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int);
    virtual std::string get_name(int);
    
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
