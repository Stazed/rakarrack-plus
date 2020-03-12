/*
  Rakarrack   Audio FX software
  CoilCrafter.h - Pick Up Emulation
  Using RBFilter
  Using Steve Harris LADSPA Plugin harmonic_gen
  Modified for rakarrack by Ryan Billing & Josep Andreu

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

#ifndef COILCRAFTER_H
#define COILCRAFTER_H

#include "HarmonicEnhancer.h"
#include "AnalogFilter.h"

class CoilCrafter
{
public:
    CoilCrafter (double sample_rate, uint32_t intermediate_bufsize);
    ~CoilCrafter ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    int Ppreset;

    float outvolume;

private:

    uint32_t PERIOD;
    
    void setvolume (int value);
    void sethpf (int value);
    void setfreq1();
    void setfreq2();
    void setq1();
    void setq2();

    int Pvolume;
    int Ppo;
    int Ppd;
    int Ptone;
    int Pq1;
    int Pfreq1;
    int Pq2;
    int Pfreq2;
    int Pmode;

    float tfreqs[10];
    float tqs[10];
    float rm[10];
    float freq1,q1,freq2,q2;
    float att;

    class HarmEnhancer *harm;
    class AnalogFilter *RB1l, *RB1r;
    class AnalogFilter *RB2l,*RB2r;
    float* interpbuf;

    class FPreset *Fpre;
};


#endif
