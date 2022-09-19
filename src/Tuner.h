/* tuneit.c -- Detect fundamental frequency of a sound
 * Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
 *
 * Modified for rakarrack by Daniel Vidal & Josep Andreu
 * tune.h  Tuner definitions
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software Foundation.
 * Please see the file COPYING for details.
 */



#ifndef TUNER_H_
#define TUNER_H_

#include <math.h>
#include <stdlib.h>


class Tuner
{
public:
    explicit Tuner (double sample_rate);
    ~Tuner ();
    void schmittFloat (int nframes, const float *indatal, const float *indatar, float *freqs, float *lfreqs);

private:
    float fSAMPLE_RATE;
    unsigned int SAMPLE_RATE;
public:

    int note;
    int preparada;
    int note_actual;
    int cents;
    signed short int *schmittBuffer;
    signed short int *schmittPointer;
    const char **notes;
    float nfreq, afreq, freq;
    float *efxoutl;
    float *efxoutr;

private:

    void displayFrequency (float freq, const float *freqs, const float *lfreqs);
    void schmittInit (int size);
    void schmittS16LE (int nframes, const signed short int *indata, float *freqs, float *lfreqs);
    void schmittFree ();

    int blockSize;
};

#endif /*TUNER_H_ */
