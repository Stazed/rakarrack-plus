/*
  ZynAddSubFX - a software synthesizer

  metronome.h - Stereo LFO used by some effects
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu & Ryan Billing


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

#ifndef EFFECT_METRONOME_H
#define EFFECT_METRONOME_H
#include "global.h"
#include "EFX_common/AnalogFilter.h"

class metronome
{
public:
    metronome (double samplerate, uint32_t intermediate_bufsize);
    ~metronome ();
    void cleanup();
    void initialize();
    void clear_initialize();
    void metronomeout (float * tickout, int period);
    void set_tempo (int bpm);
    void set_meter (int counts);
    int markctr;
    float * interpbuf;//buffer for filter

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2

private:
    float fSAMPLE_RATE;
    unsigned int SAMPLE_RATE;
    double dSAMPLERATE;
    uint32_t PERIOD;
    int tick_interval;
    int tickctr;
    int meter;
    int tickper;
    int ticktype;

    class AnalogFilter *dulltick,*sharptick, *hpf;
    

};


#endif
