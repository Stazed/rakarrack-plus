/*
  ZynAddSubFX - a software synthesizer

  Alienwah.h - "AlienWah" effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul


  Modified for rakarrack by Josep Andreu

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

#ifndef ALIENWAH_H
#define ALIENWAH_H
#include "EffectLFO.h"



struct COMPLEXTYPE
{
    float a, b;
    
    COMPLEXTYPE():
        a(),
        b() {}
};

class Alienwah
{
public:
    Alienwah (double sample_rate, uint32_t intermediate_bufsize);
    ~Alienwah ();
    void out (float * efxoutl, float * efxoutr);

    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    void lv2_update_params(uint32_t period);

    int Ppreset;
    float outvolume;

private:
    //Parametrii Alienwah
    int Pvolume;
    int Ppanning;
    int Pdepth;		//the depth of the Alienwah
    int Pfb;		//feedback
    int Plrcross;	//feedback
    int Pdelay;
    int Pphase;

    int oldk;
    int oldpdelay;

    //Control Parametrii
    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setdepth (int Pdepth);
    void setfb (int Pfb);
    void setlrcross (int Plrcross);
    void setdelay (int Pdelay);
    void setphase (int Pphase);
    
    float fPERIOD;
    uint32_t PERIOD;
    EffectLFO *lfo;		//lfo-ul Alienwah
    
    //Valorile interne
    float panning, fb, depth, lrcross, phase;
    struct COMPLEXTYPE oldl[MAX_ALIENWAH_DELAY], oldr[MAX_ALIENWAH_DELAY];
    COMPLEXTYPE oldclfol, oldclfor;
    class FPreset *Fpre;

};

#endif
