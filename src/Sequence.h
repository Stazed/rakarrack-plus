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
#include "RBFilter.h"
#include "smbPitchShift.h"
#include "beattracker.h"
#include "delayline.h"
#include "Resample.h"

#define SEQUENCE_PRESET_SIZE 15

class Sequence
{
public:
    Sequence (long int Quality, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsze);
    ~Sequence ();
    void cleanup ();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
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

    int Ppreset;

    float outvolume;

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
    int tcount, scount, dscount, intperiod;
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

    class FPreset *Fpre;
    class beattracker *beats;
    class delayline *ldelay, *rdelay;

};


#endif
