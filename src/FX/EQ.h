/*
  ZynAddSubFX - a software synthesizer

  EQ.h - EQ Effect
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

#ifndef EQ_H
#define EQ_H

#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"

#define EQLV2_URI "https://github.com/Stazed/rakarrack-plus#eql"
#define EQ_NAME "RakarrackPlus EQ"

const int C_EQ_PARAMETERS = 12;

// This is the order in the presets
enum EQ_Index
{
    EQ_31_HZ = 0,
    EQ_63_HZ,
    EQ_125_HZ,
    EQ_250_HZ,
    EQ_500_HZ,
    EQ_1_KHZ,
    EQ_2_KHZ,
    EQ_4_KHZ,
    EQ_8_KHZ,
    EQ_16_KHZ,
    EQ_Gain,    // 10
    EQ_Q        // 11
};

static const char *eq_parameters[] = 
{
    strdup(NTS(EQ_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(EQ_Q).c_str()), "Q",  "Q", 
    strdup(NTS(EQ_31_HZ).c_str()), "31 Hz",  "HZ31", 
    strdup(NTS(EQ_63_HZ).c_str()), "63 Hz",  "HZ63", 
    strdup(NTS(EQ_125_HZ).c_str()), "125 Hz",  "HZ125", 
    strdup(NTS(EQ_250_HZ).c_str()), "250 Hz",  "HZ250", 
    strdup(NTS(EQ_500_HZ).c_str()), "500 Hz",  "HZ500", 
    strdup(NTS(EQ_1_KHZ).c_str()), "1 kHz",  "KHZ1", 
    strdup(NTS(EQ_2_KHZ).c_str()), "2 kHz",  "KHZ2", 
    strdup(NTS(EQ_4_KHZ).c_str()), "4 kHz",  "KHZ4", 
    strdup(NTS(EQ_8_KHZ).c_str()), "8 kHz",  "KHZ8", 
    strdup(NTS(EQ_16_KHZ).c_str()), "16 kHz",  "KHZ16", 
};

#if 0
enum EQ_Index
{
    EQ_Gain     = 0,
    EQ_Q        = 13,
    EQ_31_HZ    = 12,
    EQ_63_HZ    = 17,
    EQ_125_HZ   = 22,
    EQ_250_HZ   = 27,
    EQ_500_HZ   = 32,
    EQ_1_KHZ    = 37,
    EQ_2_KHZ    = 42,
    EQ_4_KHZ    = 47,
    EQ_8_KHZ    = 52,
    EQ_16_KHZ   = 57
};
#endif //0

class EQ : public Effect
{
public:
    EQ (double sample_rate, uint32_t intermediate_bufsize);
    virtual ~EQ ();
    void out (float * efxoutl, float * efxoutr);
    
    virtual void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    
    virtual void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    void change_parameters (int npar, int value);
    int get_parameters (int npar);
    int get_number_efx_parameters () {return C_EQ_PARAMETERS;};
    void set_random_parameters();
    
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    virtual void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return EQLV2_URI; };
    virtual std::string get_name(int) {return EQ_NAME; };
    
    void init_filters();
    virtual void initialize();
    void clear_initialize();
    float getfreqresponse (float freq); // not used
    void setvolume (int _Pvolume);
    
private:
    void changepar_Q(int npar, int value);

    uint32_t PERIOD;
    double fSAMPLE_RATE;
    int Pvolume;

    float * interpbuf;  //buffer for filter

    struct filter
    {
        //parameters
        int Ptype, Pfreq, Pgain, Pq, Pstages;
        //internal values
        AnalogFilter *l, *r;
        
        filter():
            Ptype(),
            Pfreq(),
            Pgain(),
            Pq(),
            Pstages(),
            l(),
            r() {}
        
    } filter[MAX_EQ_BANDS];

};


#endif
