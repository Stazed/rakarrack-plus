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

#ifndef ECHO_H
#define ECHO_H

#include "Effect.h"
#include "../delayline.h"

#define ECHOLV2_URI "https://github.com/Stazed/rakarrack-plus#eco"

const int C_ECHO_PARAMETERS = 9;

enum Echo_Index
{
    Echo_DryWet = 0,
    Echo_Pan,
    Echo_Delay,
    Echo_LR_Delay,
    Echo_LR_Cross,
    Echo_Feedback,
    Echo_Damp,
    Echo_Reverse,
    Echo_Direct
};

class Echo : public Effect
{
public:
    Echo (double sample_rate, uint32_t intermediate_bufsize);
    ~Echo ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_ECHO_PARAMETERS;};
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf);
    virtual std::string get_URI() { return ECHOLV2_URI; };

    void Tempo2Delay(int delay);

private:
    
    uint32_t PERIOD;
    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Ppanning;	//Panning
    int Pdelay;
    int Plrdelay;	// L/R delay difference
    int Plrcross;	// L/R Mixing
    int Pfb;		//Feed-back-ul
    int Phidamp;
    int Preverse;
    int Pdirect;

    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void setdelay (int _Pdelay);
    void setlrdelay (int _Plrdelay);
    void setlrcross (int _Plrcross);
    void setfb (int _Pfb);
    void sethidamp (int _Phidamp);
    void setreverse (int _Preverse);
    void setdirect (int _Pdirect);

    //Parametrii reali
    void initdelays ();

    int dl, dr;
    float delay, lrdelay;
    int kl, kr, rvkl, rvkr, rvfl, rvfr, maxx_delay, fade;

    class delayline *ldelay, *rdelay;
    float ltime, rtime;
    float oldl, oldr;		//pt. lpf
    float panning, lrcross, fb, hidamp, reverse, ireverse;
    float  Srate_Attack_Coeff;

};


#endif
