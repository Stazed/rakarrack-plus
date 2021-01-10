/*
  rakarrack - a guitar effects software

 Compressor.h  -  Compressor Effect definitions
 Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
 Stefan Westerfeld <stefan@space.twc.de>

  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "FPreset.h"

const int C_COMPRESS_PARAMETERS = 9;

enum Compress_Index
{
    Compress_Threshold = 0,
    Compress_Ratio,
    Compress_Output,
    Compress_Attack,
    Compress_Release,
    Compress_Auto_Out,
    Compress_Knee,
    Compress_Stereo,
    Compress_Peak
};

class Compressor
{

public:

    Compressor (double samplerate, uint32_t intermediate_bufsize);
    ~Compressor ();

    void out (float *efxoutl, float *efxoutr);
    void setpreset (int dgui,int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    // Compressor

    int tatt;			// attack time  (ms)
    int trel;			// release time (ms)
    int tratio;
    int toutput;
    int tthreshold;
    int a_out;
    int stereo;
    int tknee;
    int peak;
    int clipping;
    int limit;

private:

    float cSAMPLE_RATE;
    uint32_t PERIOD;
    
    float rvolume;
    float lvolume;
    float rvolume_db;
    float lvolume_db;
    float thres_db;		// threshold
    float knee;
    float thres_mx;
    float kpct;
    float ratio;			// ratio
    float kratio;			// ratio maximum for knee region
    float eratio;			// dynamic ratio
    float makeup;			// make-up gain
    float makeuplin;

    float outlevel;
    float att, attr, attl;
    float rel, relr, rell;
    float relcnst, attconst;
    int ltimer, rtimer, hold;

    float rgain;
    float rgain_old;

    float lgain;
    float lgain_old;

    float lgain_t;
    float rgain_t;

    float coeff_kratio;
    float coeff_ratio;
    float coeff_knee;
    float coeff_kk;
    float lpeak;
    float rpeak;

    class FPreset *Fpre;

};

#endif
