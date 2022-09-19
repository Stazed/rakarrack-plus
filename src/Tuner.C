/* tuneit.c -- Detect fundamental frequency of a sound
 * Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
 *
 * Modified for rakarrack by Daniel Vidal & Josep Andreu
 * Tuner.C Tuner class
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software Foundation.
 * Please see the file COPYING for details.
 */
#include "Tuner.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"

Tuner::Tuner(double sample_rate) :
    fSAMPLE_RATE(float(sample_rate)),
    SAMPLE_RATE(sample_rate),
    note(),
    preparada(-1),
    note_actual(),
    cents(),
    schmittBuffer(NULL),
    schmittPointer(NULL),
    notes(NULL),
    nfreq(),
    afreq(),
    freq(),
    efxoutl(NULL),
    efxoutr(NULL),
    blockSize()
{
    static const char *englishNotes[12] ={"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

    notes = englishNotes;
    schmittInit(2);
}

Tuner::~Tuner()
{
    schmittFree();
}

void
Tuner::displayFrequency(float ffreq, const float *freqs, const float *lfreqs)
{
    if (ffreq < 1E-15)
    {
        ffreq = 1E-15f;
    }
    
    float lfreq = logf(ffreq);
    
    while (lfreq < lfreqs[0] - LOG_D_NOTE * .5f)
    {
        lfreq += LOG_2;
    }
    
    while (lfreq >= lfreqs[0] + LOG_2 - LOG_D_NOTE * .5f)
    {
        lfreq -= LOG_2;
    }
    
    float mldf = LOG_D_NOTE;
    
    for (int i = 0; i < 12; i++)
    {
        float ldf = fabsf(lfreq - lfreqs[i]);
        
        if (ldf < mldf)
        {
            mldf = ldf;
            note = i;
        }
    }

    if (preparada == note)
    {
        note_actual = note;
        nfreq = freqs[note];
        
        while (nfreq / ffreq > D_NOTE_SQRT)
        {
            nfreq *= .5f;
        }
        
        while (ffreq / nfreq > D_NOTE_SQRT)
        {
            nfreq *= 2.0f;
        }
        
        cents = lrintf(1200 * (logf(ffreq / nfreq) / LOG_2));
    }

    preparada = note;
}

void
Tuner::schmittInit(int size)
{
    blockSize = SAMPLE_RATE / size;
    schmittBuffer = (signed short int *) malloc(sizeof (signed short int) * (blockSize + 1));

    memset(schmittBuffer, 0, sizeof (signed short int) * (blockSize + 1));
    schmittPointer = schmittBuffer;
}

void
Tuner::schmittS16LE(int nframes, const signed short int *indata, float *freqs, float *lfreqs)
{
    int j = 0.0f;
    float trigfact = 0.5f;

    for (int i = 0; i < nframes; i++)
    {
        *schmittPointer++ = indata[i];
        if (schmittPointer - schmittBuffer >= blockSize)
        {
            int endpoint, startpoint, t1, t2, A1, A2, tc, schmittTriggered;

            schmittPointer = schmittBuffer;

            for (j = 0, A1 = 0, A2 = 0; j < blockSize; j++)
            {
                if (schmittBuffer[j] > 0 && A1 < schmittBuffer[j])
                {
                    A1 = schmittBuffer[j];
                }
                if (schmittBuffer[j] < 0 && A2 < -schmittBuffer[j])
                {
                    A2 = -schmittBuffer[j];
                }
            }
            
            t1 = lrintf((float) A1 * trigfact + 0.5f);
            t2 = -lrintf((float) A2 * trigfact + 0.5f);
            startpoint = 0;
            
            for (j = 1; j < blockSize && schmittBuffer[j] <= t1; j++);
            
            for (; j < blockSize && !(schmittBuffer[j] >= t2 &&
                 schmittBuffer[j + 1] < t2); j++);
            
            startpoint = j;
            schmittTriggered = 0;
            endpoint = startpoint + 1;
            
            for (j = startpoint, tc = 0; j < blockSize; j++)
            {
                if (!schmittTriggered)
                {
                    schmittTriggered = (schmittBuffer[j] >= t1);
                }
                else if (schmittBuffer[j] >= t2 && schmittBuffer[j + 1] < t2)
                {
                    endpoint = j;
                    tc++;
                    schmittTriggered = 0;
                }
            }
            
            if (endpoint > startpoint)
            {
                afreq = fSAMPLE_RATE * ((float) tc / (float) (endpoint - startpoint));
                displayFrequency(afreq, freqs, lfreqs);
            }
        }
    }
}

void
Tuner::schmittFree()
{
    free(schmittBuffer);
}

void
Tuner::schmittFloat(int nframes, const float *indatal, const float *indatar, float *freqs, float *lfreqs)
{
    signed short int buf[nframes];
    
    for (int i = 0; i < nframes; i++)
    {
        buf[i] = (short) ((indatal[i] + indatar[i]) * 32768);
    }
    
    schmittS16LE(nframes, buf, freqs, lfreqs);
}

