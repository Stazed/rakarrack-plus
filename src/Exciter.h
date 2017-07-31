/*
  Rakarrack   Audio FX software
  Exciter.h - Harmonic Enhancer
  Based in Steve Harris LADSPA Plugin harmonic_gen
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

#ifndef EXCITER_H
#define EXCITER_H

#include "HarmonicEnhancer.h"


class Exciter
{
public:
    Exciter (double sample_rate, uint32_t intermediate_bufsize);
    ~Exciter ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    void lv2_update_params(uint32_t period);
    
    int Ppreset;
    float outvolume;

private:

    void setvolume (int value);
    void setlpf (int value);
    void sethpf (int value);
    void sethar(int num, int value);


    int Prm[10];
    int Pvolume;
    int lpffreq;
    int hpffreq;

    float rm[10];

    uint32_t PERIOD;

    class HarmEnhancer *harm;

    class FPreset *Fpre;

};


#endif
