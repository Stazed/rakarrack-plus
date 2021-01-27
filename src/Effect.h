/*
  ZynAddSubFX - a software synthesizer

  Effect.h - this class is inherited by the all effects(Reverb, Echo, ..)
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

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

#ifndef EFFECT_H
#define EFFECT_H

#include "global.h"
#include <vector>


class Effect
{
public:

    Effect (double sample_rate, uint32_t intermediate_bufsize) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), PMIDI(), PSELECT(), Pinterval(), r_ratio(), mira() {};

    // Distortion, Overdrive, Derelict, DistBand, StompBox, Convolotron, Reverbtron
    Effect (int, int, int, double, uint32_t) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), PMIDI(), PSELECT(), Pinterval(), r_ratio(), mira() {};

    // Harmonizer, Sequence, Shifter, StereoHarm
    Effect (long int Quality, int DS, int uq, int dq, double sample_rate,
        uint32_t intermediate_bufsize) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), PMIDI(), PSELECT(), Pinterval(), r_ratio(), mira() {};

    // Looper
    Effect (float size, double samplerate, uint32_t intermediate_bufsize) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), PMIDI(), PSELECT(), Pinterval(), r_ratio(), mira() {};

    // Vocoder
    Effect (float *auxresampled_, int bands, int DS, int uq, int dq,
        double sample_rate, uint32_t intermediate_bufsize) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), PMIDI(), PSELECT(), Pinterval(), r_ratio(), mira() {};

    virtual ~ Effect () {};
    
    virtual void setpreset (int npreset) {};
    virtual void changepar (int npar, int value) {};
    virtual int getpar (int npar) {return (0);};
    virtual void out (float * smpsl, float * smpsr) {};
    virtual void cleanup () {};
    
    virtual std::vector<int> save_parameters() {};
    virtual void reset_parameters(std::vector<int> parameters) {};

#ifdef LV2_SUPPORT
    virtual void lv2_update_params(uint32_t period) {};
#endif // LV2

    // The volume of the effect and is public.
    float outvolume;
    int Ppreset;
    class FPreset *Fpre;
    
    // Harmonizer
    int Pinterval;
    int PMIDI;
    int PSELECT;
    float r_ratio;
    int mira;

};

#endif
