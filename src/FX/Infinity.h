/*
  Rakarrack Guitar Effects Processor

  InfiniT : Infinitely rising & falling filter effect
  Author:  Ryan Billing (aka Transmogrifox)
  Copyright (C) 2010

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

#ifndef INFINITY_H
#define INFINITY_H

#include "Effect.h"
#include "../EFX_common/RBFilter.h"

#define INFLV2_URI "https://github.com/Stazed/rakarrack-plus#Infinity"

#define NUM_INF_BANDS 8

const int C_INFINITY_PARAMETERS = 18;

enum Infinity_Index
{
    Infinity_DryWet = 0,
    Infinity_Band_1,
    Infinity_Band_2,
    Infinity_Band_3,
    Infinity_Band_4,
    Infinity_Band_5,
    Infinity_Band_6,
    Infinity_Band_7,
    Infinity_Band_8,
    Infinity_Resonance,
    Infinity_Start,
    Infinity_End,
    Infinity_Tempo,
    Infinity_LR_Delay,
    Infinity_Subdivision,
    Infinity_AutoPan,
    Infinity_Reverse,
    Infinity_Stages
};

class Infinity : public Effect
{
public:
    Infinity (double sample_rate, uint32_t intermediate_bufsize);
    ~Infinity ();
    void out (float * efxoutl, float * efxoutr);

    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_INFINITY_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf);
    virtual std::string get_URI() { return INFLV2_URI; };
    
    void initialize();
    void clear_initialize();

private:

    void setvolume (int _Pvolume);
    void adjustfreqs();
    void oscillator();
    void reinitfilter ();
    void setq ();
    
    float fSAMPLE_RATE;
    uint32_t PERIOD;
    
    int Pvolume;		//For wet/dry mix // "Wet/Dry"
    int Pb[NUM_INF_BANDS];          //Pbx -64 to 64// "Band" each label "1" through "8" adjusts mix of each filter band
    int Pq;              //-64 to 64// "Res." filter resonance
    int Pstartfreq;         //0 to 127// "Start" where the filter starts
    int Pendfreq;	       //0 to 127// "End" when the filter ends
    int Prate;           //BPM// "Tempo" how fast it sweeps
    int Pstdf;           //filter bands offset L&R
    int Psubdiv;
    int Pautopan;
    int Preverse;
    int Pstages;        //filter stages

    struct filterstate
    {
        float sinp;
        float cosp;      //sinusoid oscillator state variables
        float ramp;      //frequency oscillator
        float level;
        float lfo;
        float vol;
        
        filterstate():
            sinp(),
            cosp(),
            ramp(),
            level(),
            lfo(),
            vol() {}
    } rbandstate[NUM_INF_BANDS], lbandstate[NUM_INF_BANDS];

    struct phasevars
    {
        float yn1[MAX_PHASER_STAGES];
        float xn1[MAX_PHASER_STAGES];
        float gain;
        
        phasevars():
            yn1(),
            xn1(),
            gain() {}
    } rphaser[NUM_INF_BANDS], lphaser[NUM_INF_BANDS];
    float phaserfb;
    float phaser(phasevars *pstruct, float fxn, int j);
    /*
    fconst = freq* 2*pi/fs;
    rampconst = 2*freq/fs;
     */
    float cfconst, fconst;  //sine wave oscillator frequency constant // 2*pi*freq/fs
    float rampconst, irampconst, crampconst, cirampconst;  //ramp oscillator constant// 2*freq/SR
    float alpha, beta;
    float fstart, fend;  //range of filter sweep
    float linconst, logconst;
    float minlevel, maxlevel;
    float frmin, frmax;
    float qq;
    float stdiff;
    float volmaster;
    float autopan, msin, dsin, mcos, mconst;
    float ratescale;
    int tflag;

    float* interpbuf; //buffer for filters
    class RBFilter *filterl[NUM_INF_BANDS], *filterr[NUM_INF_BANDS];

};

#endif
