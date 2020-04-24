/*
  rakarrack - a guitar effects software

  StereoHarm.h  -  Stereo Harmonizer definitions
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

#ifndef STEREOHARM_H
#define STEREOHARM_H

#include "smbPitchShift.h"
#include "Resample.h"

#define SHARM_PRESET_SIZE 12

class StereoHarm
{

public:
    StereoHarm (long int Quality, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsize);
    ~StereoHarm ();
    void out (float *efxoutl, float *efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void initialize();
    void clear_initialize();
    void adjust(int DS, uint32_t period);

    /* Change resample quality, etc */
    void change_quality(int Quality);
    void change_downsample(int DS);
    void change_up_q(int uq);
    void change_down_q(int dq);

private:
    int STE_DOWN;
    uint32_t PERIOD;
    unsigned int SAMPLE_RATE;
public:

    int Ppreset;
    int Pintervall;
    int Pintervalr;

    int PMIDI;
    int PSELECT;
    int mira;
    int DS_state;
    
    long window;

    long int hq;

    double u_up;
    double u_down;

    float *outil,*outir;
    float *outol,*outor;
    float *templ, *tempr;

    float outvolume;

    float r_ratiol;
    float r_ratior;

private:

    unsigned int nPERIOD;
    float nRATIO;
    int nSAMPLE_RATE;
    float nfSAMPLE_RATE;
    
    int Pvolume;
    int Plrcross;
    int Pgainl;
    int Pgainr;
    int Pchromel;
    int Pchromer;
    int Pnote;
    int Ptype;

    float gainl,gainr;
    float intervall;
    float intervalr;
    float chromel;
    float chromer;
    float lrcross;
    
    /* Change quality */
    int m_hold_parameters[SHARM_PRESET_SIZE];  // hold the current parameters to reset on quality change
    void save_parameters();
    void reset_parameters();

    void setvolume (int Pvolume);
    void setinterval (int chan, int value);
    void setchrome(int chan, int value);
    void setgain (int chan, int value);
    void setMIDI (int PMIDI);
    void setlrcross(int value);

    class Resample *U_Resample;
    class Resample *D_Resample;

    PitchShifter *PSl, *PSr;

    class FPreset *Fpre;
};

#endif
