/*
    HarnEnhancer - Harmonic Enhancer Class

    HarmEnhancer.h  - headers.
    Copyright (C) 2008-2010 Josep Andreu
    Author: Josep Andreu

    Based on Steve Harris LADSPA harmonic

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

#ifndef HARM_ENHANCER_H
#define HARM_ENHANCER_H

#include "AnalogFilter.h"
#include "../Limiter.h"



class HarmEnhancer
{
public:
    HarmEnhancer(float *harmonics, float hfreq, float lfreq, float gain, double sample_rate, uint32_t intermediate_bufsize);
    ~HarmEnhancer();
    void cleanup();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void initialize();
    void clear_initialize();
    void chebpc(const float c[], float d[]);
    void calcula_mag(float *Rmag);
    void harm_out(float *efxoutl, float *efxoutr);
    void set_vol(int mode, float gain);
    void set_freqh(int mode, float freq);
    void set_freql(int mode, float freq);

    float realvol;
    float hpffreq;
    float lpffreq;

private:

    uint32_t PERIOD;
    double fSAMPLE_RATE;
    float HFREQ, LFREQ;
    float *inputl;
    float *inputr;
    float vol;
#ifdef UNUSED_STUFF
    float itm1l;
    float itm1r;
    float otm1l;
    float otm1r;
#endif

    float p[HARMONICS];

    AnalogFilter *hpfl, *hpfr;
    AnalogFilter *lpfl, *lpfr;
    float* interpbuf; //buffer for filters

    class Compressor *limiter;

};

#endif
