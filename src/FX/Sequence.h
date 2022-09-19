/*
  Rakarrack Guitar FX

  Sequence.h - Simple compressor/Sequence effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing & Josep Andreu

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <vector>
#include "Effect.h"
#include "../EFX_common/RBFilter.h"
#include "../EFX_common/smbPitchShift.h"
#include "../EFX_common/beattracker.h"
#include "../EFX_common/delayline.h"
#include "../EFX_common/Resample.h"

#define SEQUENCELV2_URI "https://github.com/Stazed/rakarrack-plus#Sequence"
#define SEQUENCE_NAME "RakarrackPlus Sequence"

const int C_SEQUENCE_PARAMETERS = 15;

enum Sequence_Index
{
    Sequence_Step_1,
    Sequence_Step_2,
    Sequence_Step_3,
    Sequence_Step_4,
    Sequence_Step_5,
    Sequence_Step_6,
    Sequence_Step_7,
    Sequence_Step_8,
    Sequence_DryWet,
    Sequence_Tempo,
    Sequence_Resonance,
    Sequence_Amp,
    Sequence_Stdf,
    Sequence_Mode,
    Sequence_Range
};

static const char *sequence_parameters[] = 
{
    strdup(NTS(Sequence_Step_1).c_str()), "Step 1",  "S1", 
    strdup(NTS(Sequence_Step_2).c_str()), "Step 2",  "S2", 
    strdup(NTS(Sequence_Step_3).c_str()), "Step 3",  "S3", 
    strdup(NTS(Sequence_Step_4).c_str()), "Step 4",  "S4", 
    strdup(NTS(Sequence_Step_5).c_str()), "Step 5",  "S5", 
    strdup(NTS(Sequence_Step_6).c_str()), "Step 6",  "S6", 
    strdup(NTS(Sequence_Step_7).c_str()), "Step 7",  "S7", 
    strdup(NTS(Sequence_Step_8).c_str()), "Step 8",  "S8", 
    strdup(NTS(Sequence_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Sequence_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Sequence_Resonance).c_str()), "Q",  "Q", 
    strdup(NTS(Sequence_Amp).c_str()), "Amplitude/Alt. Mode",  "AMP", 
    strdup(NTS(Sequence_Stdf).c_str()), "Right Channel Step Lag",  "LAG", 
    strdup(NTS(Sequence_Mode).c_str()), "Modulation Mode",  "MODE", 
    strdup(NTS(Sequence_Range).c_str()), "Step Adjustment Range",  "RANGE"
};

class Sequence : public Effect
{
public:
    Sequence (long int Quality, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsze);
    ~Sequence ();
    void cleanup ();
    int get_number_efx_parameters () {return C_SEQUENCE_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return SEQUENCELV2_URI; };
    virtual std::string get_name(int) {return SEQUENCE_NAME; };
    
    void initialize();
    void clear_initialize();
    void out (float * efxoutl, float * efxoutr);
    void changepar (int npar, int value);
    int getpar (int npar);
    void setpreset (int npreset);
    void setranges(int value);
    void settempo(int value);
    void adjust(int DS, double sample_rate);

    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

private:
    
    void setvolume (int volume);
    
    uint32_t PERIOD;
    float fSAMPLE_RATE;
    
    //Parametrii
    int DS_state;
    long int hq;
    int Pvolume;	       //Output Level
    int Psequence[8];	//Sequence.  Each value is 0...127
    int Ptempo;           // Rate
    int Pq;                //Filter Q
    int Pamplitude;     // 0 or 1.  Checkbox on or off...
    int Pstdiff;       // 1 2 3 4 5 6 7 8
    int Pmode;
    int Prange;
    unsigned int tcount, scount, dscount, intperiod;
    int subdiv;
    int rndflag;
    unsigned int nPERIOD;
    int nSAMPLE_RATE;
    float nRATIO;

    double u_up;
    double u_down;

    long window;

    float nfSAMPLE_RATE;
    float MINFREQ,MAXFREQ;
    float fsequence[8];
    float fq;
    float panning;
    float ifperiod,fperiod, seqpower;

    float *outi;
    float *outo;
    float *templ, *tempr;

//Variables for TrigStepper detecting trigger state.
    float peakpulse, peak, envrms, peakdecay, trigthresh;
    int trigtimeout, trigtime, onset, atk;
    float targatk, lmod, rmod;

    float maxdly, tempodiv, fb, rdlyfb, ldlyfb;
    float avtime;
    int avflag;

    class RBFilter *filterl, *filterr, *modfilterl, *modfilterr, *rmsfilter, *peaklpfilter, *peakhpfilter, *peaklpfilter2;
    float* interpbuf;

    Resample *U_Resample;
    Resample *D_Resample;

    PitchShifter *PS;

    class beattracker *beats;
    class delayline *ldelay, *rdelay;

};


#endif
