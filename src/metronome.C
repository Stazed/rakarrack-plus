/*
  ZynAddSubFX - a software synthesizer

  metronome.C - Stereo LFO used by some effects
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu 6 Ryan Billing


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

#include <math.h>

#include "global.h"
#include "metronome.h"

metronome::metronome(double samplerate, uint32_t intermediate_bufsize) :
    markctr(),
    interpbuf(NULL),
    fSAMPLE_RATE(float(samplerate)),
    SAMPLE_RATE(samplerate),
    dSAMPLERATE(samplerate),
    PERIOD(intermediate_bufsize),
    tick_interval(SAMPLE_RATE),
    tickctr(),
    meter(3),
    tickper(lrintf(0.012f * fSAMPLE_RATE)),
    ticktype(4),
    dulltick(NULL),
    sharptick(NULL),
    hpf(NULL)
{
    initialize();
}

metronome::~metronome()
{
    clear_initialize();
}

void
metronome::initialize()
{
    interpbuf = new float[PERIOD];

    dulltick = new AnalogFilter(4, 1600.0f, 80.0f, 1, dSAMPLERATE, interpbuf); //BPF
    sharptick = new AnalogFilter(4, 2800.0f, 80.0f, 1, dSAMPLERATE, interpbuf); //BPF
    hpf = new AnalogFilter(3, 850.0f, 60.0f, 1, dSAMPLERATE, interpbuf); //HPF
}

void
metronome::clear_initialize()
{
    delete[] interpbuf;
    delete dulltick;
    delete sharptick;
    delete hpf;
}

void
metronome::cleanup()
{
    tickctr = 0;
    markctr = 0;
    dulltick->cleanup();
    sharptick->cleanup();
    hpf->cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
metronome::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    clear_initialize();
    initialize();
}
#endif

/*
 * Update the changed parameters
 */
void
metronome::set_tempo(int bpm)
{
    float tickperiod = 60.0f / ((float) bpm);
    tick_interval = lrintf(fSAMPLE_RATE * tickperiod);
}

void
metronome::set_meter(int counts) //how many counts to hear the "mark"
{
    ticktype = counts; //always dull if 0, always sharp if 1, mark on interval if more

    if (counts < 1) counts = 1;

    meter = counts - 1;
}

/*
 * Audible output
 */
void
metronome::metronomeout(float * tickout, int period)
{
    float ticker = 0.0f;

    for (int i = 0; i < period; i++)
    {
        tickctr++;

        if (tickctr > tick_interval)
        {
            tickctr = 0;
            markctr++;

            if (markctr > meter)
                markctr = 0;
        }

        if (tickctr < tickper)
        {
            ticker = 1.0f;
        }
        else
        {
            ticker = 0.0f;
        }

        float hipass = hpf->filterout_s(ticker);

        if (hipass > 0.5f) hipass = 0.5f;
        if (hipass<-0.5f) hipass = -0.5f;

        float outdull = dulltick->filterout_s(hipass);
        float outsharp = sharptick-> filterout_s(hipass);

        switch (ticktype)
        {
        case 0:
            tickout[i] = 1.25f * outdull;
            break;
        case 1:
            tickout[i] = 0.65f * outsharp;
            break;
        default:
            if (markctr == 0)
            {
                tickout[i] = 0.65f * outsharp;
            }
            else
            {
                tickout[i] = 1.25f * outdull;
            }
            break;
        }
    }
}

