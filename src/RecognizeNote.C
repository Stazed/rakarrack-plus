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
/*
  rakarrack - a guitar effects software

  RecognizeNote.C  -  Recognize Audio Notess
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

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


#include "RecognizeNote.h"

Recognize::Recognize(float trig, float tune, double sample_rate, uint32_t intermediate_bufsize) :
    note(),
    schmittBuffer(NULL),
    schmittPointer(NULL),
    notes(NULL),
    trigfact(trig),
    lafreq(),
    nfreq(),
    afreq(),
    freq(),
    reconota(),
    last(),
    freqs(),
    lfreqs(),
    fSAMPLE_RATE((float) sample_rate),
    dSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    ultima(-1),
    blockSize(),
    lpfl(NULL),
    lpfr(NULL),
    hpfl(NULL),
    hpfr(NULL),
    interpbuf(NULL),
    Sus(NULL)
{
    static const char *englishNotes[12] ={"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    notes = englishNotes;

    Sus = new Sustainer(sample_rate, PERIOD);
    Sus->changepar(Sustain_Gain, 101); // This approximates the original wrong settings in rakarrack ;)
    Sus->changepar(Sustain_Sustain, 64); // This approximates the original wrong settings in rakarrack ;)
    //Sus->changepar(1,64);   // This is wrong - parameters are 0 & 1, not 1 & 2
    //Sus->changepar(2,127);  // This is wrong - parameters are 0 & 1, not 1 & 2

    initialize();

    update_freqs(tune);
    schmittInit(24, sample_rate);
}

Recognize::~Recognize()
{
    free(schmittBuffer);
    clear_initialize();
    delete Sus;
}

void
Recognize::schmittInit(int size, double SAMPLE_RATE)
{
    blockSize = SAMPLE_RATE / size;
    schmittBuffer = (signed short int *) malloc(sizeof (signed short int) * (blockSize + 1));

    memset(schmittBuffer, 0, sizeof (signed short int) * (blockSize + 1));
    schmittPointer = schmittBuffer;
}

void
Recognize::schmittS16LE(const signed short int *indata)
{
    int j;  // initialize ok.

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        *schmittPointer++ = indata[i];
        
        if (schmittPointer - schmittBuffer >= blockSize)
        {
            schmittPointer = schmittBuffer;
            
            int  A1, A2; A1 = A2 = 0;
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
            
            int t1 = lrintf((float) A1 * trigfact + 0.5f);
            int t2 = -lrintf((float) A2 * trigfact + 0.5f);
            
            for (j = 1; j < blockSize && schmittBuffer[j] <= t1; j++);
            
            for (; j < blockSize && !(schmittBuffer[j] >= t2 &&
                 schmittBuffer[j + 1] < t2); j++);
            
            int startpoint = j;
            int schmittTriggered = 0;
            int endpoint = startpoint + 1;
            int tc = 0;

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
                displayFrequency(afreq);
            }
        }
    }
}

void
Recognize::setlpf(int value)
{
    float fr = (float) value;
    lpfl->setfreq(fr);
    lpfr->setfreq(fr);
}

void
Recognize::sethpf(int value)
{
    float fr = (float) value;

    hpfl->setfreq(fr);
    hpfr->setfreq(fr);
}

void
Recognize::schmittFloat(float *indatal, float *indatar)
{
    signed short int buf[PERIOD]; //TODO: buf should probably be a member of this class

    lpfl->filterout(indatal, PERIOD);
    hpfl->filterout(indatal, PERIOD);
    lpfr->filterout(indatar, PERIOD);
    hpfr->filterout(indatar, PERIOD);

    Sus->out(indatal, indatar);

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        buf[i] = (short) ((indatal[i] + indatar[i]) * 32768);
    }
    
    schmittS16LE(buf);
}

void
Recognize::displayFrequency(float freq)
{
//    int offset = 4;
    int noteoff = 0;
    int octave = 4;

    if (freq < 1E-15)
    {
        freq = 1E-15f;
    }
    
    float lfreq = logf(freq);
    
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

    nfreq = freqs[note];

    while (nfreq / freq > D_NOTE_SQRT)
    {
        nfreq *= 0.5f;
        octave--;
        if (octave < -1)
        {
            noteoff = 1;
            break;
        }
    }
    
    while (freq / nfreq > D_NOTE_SQRT)
    {
        nfreq *= 2.0f;
        octave++;
        
        if (octave > 7)
        {
            noteoff = 1;
            break;
        }
    }

    if (!noteoff)
    {
        //    reconota = 24 + (octave * 12) + note - 3;
        int offset = lrintf(nfreq / 20.0);
        
        if (fabsf(lafreq - freq) > offset)
        {
            lafreq = nfreq;
            reconota = 24 + (octave * 12) + note - 3;
            //    printf("%f\n",lafreq);
        }
    }
}

void
Recognize::update_freqs(float tune)
{
    //update freqs (tables for stuff)

    freqs[0] = tune;
    lfreqs[0] = logf(freqs[0]);
    
    for (int i = 1; i < 12; i++)
    {
        freqs[i] = freqs[i - 1] * D_NOTE;
        lfreqs[i] = lfreqs[i - 1] + LOG_D_NOTE;
    }
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Recognize::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    clear_initialize();
    initialize();
    setlpf(5500);
    sethpf(80);

    Sus->lv2_update_params(period);
}
#endif // LV2

void Recognize::initialize()
{
    interpbuf = new float[PERIOD];
    lpfl = new AnalogFilter(2, 3000, 1, 0, dSAMPLE_RATE, interpbuf);
    lpfr = new AnalogFilter(2, 3000, 1, 0, dSAMPLE_RATE, interpbuf);
    hpfl = new AnalogFilter(3, 300, 1, 0, dSAMPLE_RATE, interpbuf);
    hpfr = new AnalogFilter(3, 300, 1, 0, dSAMPLE_RATE, interpbuf);
}

void Recognize::clear_initialize()
{
    delete lpfl;
    delete lpfr;
    delete hpfl;
    delete hpfr;
    delete[] interpbuf;
}
