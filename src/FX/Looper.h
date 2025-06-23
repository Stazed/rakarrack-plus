/*
  ZynAddSubFX - a software synthesizer

  Looper.h - Looper Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Ryan Billing & Josep Andreu
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

#ifndef LOOPER_H
#define LOOPER_H

#include "Effect.h"
#include "../metronome.h"

/**
 * This is the number of parameters saved to file and presets.
 * There are actually 18 parameters, but last 4 are not saved...
 */
const int C_LOOPER_PARAMETERS = 14;

enum Looper_Index
{
    Looper_DryWet = 0,
    Looper_Play,
    Looper_Stop,
    Looper_Record,
    Looper_Clear,
    Looper_Reverse,
    Looper_Level_1,
    Looper_Track_1,
    Looper_Track_2,
    Looper_AutoPlay,
    Looper_Level_2,
    Looper_Rec_1,
    Looper_Rec_2,
    Looper_Link,
    Looper_Tempo,   // not saved
    Looper_Bar,     // not saved
    Looper_Metro,   // not saved
    Looper_M_S      // not saved
};

class Looper :public Effect
{
public:
    Looper (float size, double samplerate, uint32_t intermediate_bufsize);
    ~Looper ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void set_value (int npar, int value);  // to set one from a preset
    void changepar (int npar, int value);
    int getpar (int npar);
    int get_number_efx_parameters () {return C_LOOPER_PARAMETERS;};
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void cleanuppt1 ();
    void cleanuppt2 ();
    void cleanup ();
    void settempo(int value);
    void setmvol(int value);
    void getstate ();
    
    int Pplay;
    int Pstop;
    int looper_bar;
    int looper_qua;
    int progstate[6];
    
private:
    class metronome ticker;
    
    float fSAMPLE_RATE;
    unsigned int SAMPLE_RATE;
    uint32_t PERIOD;

    void initdelays ();
    void setfade ();
    void setbar(int value);
    void timeposition(int value);
    int set_len(int value);
    int cal_len(int value);
    void setvolume (int volume);

    //Parameters
    int Pvolume;	//Mix
    int Precord;	//set to 1
    int Pclear;         //set to 1
    int Preverse;	//0 or 1
    int Pfade1;         //0...127//  crossfade track1 & 2
    int Pfade2;
    int PT1;
    int PT2;
    int Pautoplay;
    int Prec1;
    int Prec2;
    int Plink;
    int Ptempo;
    int Pbar;
    int Pmetro;
    int Pms;

    int kl, kl2, rvkl, rvkl2, maxx_delay, fade, dl, dl2, first_time1, first_time2, rplaystate;
    int barlen, looper_ts;

    float *ldelay, *rdelay;
    float *t2ldelay, *t2rdelay;

    float oldl, oldr;   //pt. lpf

    float  Srate_Attack_Coeff, track1gain, track2gain, fade1, fade2, pregain1, pregain2;
    float mvol;

};


#endif
