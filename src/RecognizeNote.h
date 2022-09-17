/* tuneit.c -- Detect fundamental frequency of a sound
* Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
*
* Modified for rakarrack by Josep Andreu
* Recognizer.h  Recognizer Audio Note definitions
*
* This is free software, placed under the terms of the
* GNU General Public License, as published by the Free Software Foundation.
* Please see the file COPYING for details.
*/


#ifndef RECOGNIZE_H_
#define RECOGNIZE_H_

#include <math.h>
#include "global.h"
#include "EFX_common/AnalogFilter.h"
#include "FX/Sustainer.h"

class Recognize
{
public:
    Recognize (float trig, float tune, double sample_rate, uint32_t intermediate_bufsize);
    ~Recognize ();

    void schmittFloat (float *indatal, float *indatar);
    void sethpf(int value);
    void setlpf(int value);
    void update_freqs(float tune);
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void initialize();
    void clear_initialize();
    int note;

    signed short int *schmittBuffer;
    signed short int *schmittPointer;
    const char **notes;
    float trigfact;
    float lafreq;
    float nfreq, afreq, freq;

    int reconota;
    int last;
    float freqs[12];
    float lfreqs[12];


private:

    void displayFrequency (float freq);
    void schmittInit (int size, double sample_rate);
    void schmittS16LE (const signed short int *indata);
    
    float fSAMPLE_RATE;
    double dSAMPLE_RATE;
    uint32_t PERIOD;

    int ultima;
    int blockSize;

    AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;
    float* interpbuf; //buffer for filters

    class Sustainer *Sus;

};

#endif
