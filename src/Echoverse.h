/*
  ZynAddSubFX - a software synthesizer

  Echo.h - Echo Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

  Reverse Echo by Transmogrifox

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

// Renamed from RBEcho.h on 4/19/2020 by stazed

#ifndef ECHOVERSE_H
#define ECHOVERSE_H

#include "Effect.h"
#include "delayline.h"

const int C_ECHOVERSE_PARAMETERS = 10;

enum Echoverse_Index
{
    Echoverse_DryWet = 0,
    Echoverse_Pan,
    Echoverse_Tempo,
    Echoverse_LR_Delay,
    Echoverse_Angle,
    Echoverse_Feedback,
    Echoverse_Damp,
    Echoverse_Reverse,
    Echoverse_Subdivision,
    Echoverse_Ext_Stereo
};

class Echoverse : public Effect
{
public:
    Echoverse (double sample_rate, uint32_t intermediate_bufsize);
    ~Echoverse ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2

private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setdelay (int Pdelay);
    void setlrdelay (int Plrdelay);
    void setlrcross (int Plrcross);
    void setfb (int Pfb);
    void sethidamp (int Phidamp);
    void setreverse (int Preverse);
    void initdelays ();

    float fSAMPLE_RATE;
    uint32_t PERIOD;

    //Parametrii
    int Pvolume;	// E/R
    int Ppanning;	//Panning
    int Pdelay;
    int Plrdelay;	// L/R delay difference
    int Plrcross;	// L/R Mixing
    int Pfb;		//Feed-back
    int Phidamp;
    int Preverse;
    int Psubdiv;
    int Pes;

    int maxx_delay;

    float delay, lrdelay, ltime, rtime;
    float fdelay, subdiv, pes, pingpong, ipingpong;
    float rvl, rvr;
    float rpanning, lpanning, lrcross, fb, hidamp, reverse, ireverse, lfeedback, rfeedback;
    float oldl, oldr;		//pt. lpf
    float  Srate_Attack_Coeff;

    class delayline *ldelay, *rdelay;
};


#endif
