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

    Effect (double, uint32_t) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    // Distortion, Overdrive, Derelict, DistBand, StompBox, Convolotron, Reverbtron
    Effect (int, int, int, double, uint32_t) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), Filename() {};

    // Harmonizer, Sequence, Shifter, StereoHarm
    Effect (long int, int, int, int, double, uint32_t) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    // Looper
    Effect (float, double, uint32_t) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL), Pplay(), Pstop(1),
        looper_bar(), looper_qua(), progstate() {};

    // Vocoder
    Effect (float *, int, int, int, int, double, uint32_t) :
        outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    virtual ~ Effect () {};
    
    virtual void setpreset (int) {};
    virtual void changepar (int, int) {};
    virtual int getpar (int) {return (0);};
    virtual void out (float *, float *) {};
    virtual void cleanup () {};
    
    virtual std::vector<int> save_parameters() {return std::vector<int>();};
    virtual void reset_parameters(std::vector<int>) {};

#ifdef LV2_SUPPORT
    virtual void lv2_update_params(uint32_t) {};
#endif // LV2

    // The volume of the effect and is public.
    float outvolume;
    int Ppreset;
    class FPreset *Fpre;

    // Looper
    int Pplay;
    int Pstop;
    int looper_bar;
    int looper_qua;
    int progstate[6];
    virtual void settempo(int) {};
    virtual void setmvol(int) {};
    virtual void getstate() {};
    
    // Convolotron, Reverbtron, Echotron
    char Filename[128];
    virtual int setfile (int) {return 0;};
    virtual int get_file_length() {return 0;};
    virtual char *get_file_name() {return 0;};
};

#endif
