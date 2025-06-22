/*

  CompBand.h - 4 Bands Compressor

  Using Compressor and AnalogFilters by other authors.

  Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
  Stefan Westerfeld <stefan@space.twc.de>
  Modified by Ryan Billing & Josep Andreu

  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul
  ZynAddSubFX - a software synthesizer

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

#ifndef COMPBANDL_H
#define COMPBANDL_H

#include "Effect.h"
#include "../Limiter.h"
#include "../EFX_common/AnalogFilter.h"

#define MBCOMPLV2_URI "https://github.com/Stazed/rakarrack-plus#CompBand"
#define MBCOMP_NAME "RakarrackPlus CompBand"

const int C_COMPBAND_PARAMETERS = 13;

enum CompBand_Index
{
    CompBand_DryWet = 0,
    CompBand_Low_Ratio,
    CompBand_Mid_1_Ratio,
    CompBand_Mid_2_Ratio,
    CompBand_High_Ratio,
    CompBand_Low_Thresh,
    CompBand_Mid_1_Thresh,
    CompBand_Mid_2_Thresh,
    CompBand_High_Thresh,
    CompBand_Cross_1,
    CompBand_Cross_2,
    CompBand_Cross_3,
    CompBand_Gain
};

static const char *compband_parameters[] = 
{
    strdup(NTS(CompBand_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(CompBand_Low_Ratio).c_str()), "Low Band Ratio",  "LR", 
    strdup(NTS(CompBand_Mid_1_Ratio).c_str()), "Mid Band 1 Ratio",  "M1R", 
    strdup(NTS(CompBand_Mid_2_Ratio).c_str()), "Mid Band 2 Ratio",  "M2R", 
    strdup(NTS(CompBand_High_Ratio).c_str()), "High Band Ratio",  "HR", 
    strdup(NTS(CompBand_Low_Thresh).c_str()), "Low Band Threshold",  "LT", 
    strdup(NTS(CompBand_Mid_1_Thresh).c_str()), "Mid Band 1 Threshold",  "M1T", 
    strdup(NTS(CompBand_Mid_2_Thresh).c_str()), "Mid Band 2 Threshold",  "M2T", 
    strdup(NTS(CompBand_High_Thresh).c_str()), "High Threshold",  "HT", 
    strdup(NTS(CompBand_Cross_1).c_str()), "Low/Mid1 Crossover",  "LMCR", 
    strdup(NTS(CompBand_Cross_2).c_str()), "Mid1/Mid2 Crossover",  "MMCR", 
    strdup(NTS(CompBand_Cross_3).c_str()), "Mid2/High Crossover",  "MHCR", 
    strdup(NTS(CompBand_Gain).c_str()), "Gain",  "GAIN"
};

class CompBand : public Effect
{
public:
    CompBand (double sample_rate, uint32_t intermediate_bufsize);
    ~CompBand ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_COMPBAND_PARAMETERS;};
    void set_random_parameters();
    
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return MBCOMPLV2_URI; };
    virtual std::string get_name(int) {return MBCOMP_NAME; };
    
    void initialize();
    void clear_initialize();

    float level;

    float *lowl;
    float *lowr;
    float *midll;
    float *midlr;
    float *midhl;
    float *midhr;
    float *highl;
    float *highr;


private:

    void setvolume (int Pvolume);
    void setlevel(int value);
    void setratio (int ch, int value);
    void setthres (int ch, int value);
    void setCross1 (int value);
    void setCross2 (int value);
    void setCross3 (int value);

    uint32_t PERIOD;
    double fSAMPLE_RATE;

    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Plevel;
    int PLratio;
    int PMLratio;
    int PMHratio;
    int PHratio;
    int PLthres;
    int PMLthres;
    int PMHthres;
    int PHthres;


    int Cross1;
    int Cross2;
    int Cross3;

    //Parametrii reali

    AnalogFilter *lpf1l, *lpf1r, *hpf1l, *hpf1r;
    AnalogFilter *lpf2l, *lpf2r, *hpf2l, *hpf2r;
    AnalogFilter *lpf3l, *lpf3r, *hpf3l, *hpf3r;
    float * interpbuf;//buffer for filters

    Compressor *CL,*CML,*CMH,*CH;

};

#endif
