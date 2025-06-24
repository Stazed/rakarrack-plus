/*
  rakarrack - a guitar effects software

 Compressor.C  -  Compressor Effect
 Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
 Stefan Westerfeld <stefan@space.twc.de>

  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

        Patches:
        September 2009  Ryan Billing (a.k.a. Transmogrifox)
                --Modified DSP code to fix discontinuous gain change at threshold.
                --Improved automatic gain adjustment function
                --Improved handling of knee
                --Added support for user-adjustable knee
                --See inline comments

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

#include <math.h>
#include "../global.h"
#include "Compressor.h"
#define  MIN_GAIN  0.00001f        // -100dB  This will help prevent evaluation of denormal numbers

Compressor::Compressor(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    tatt(20),
    trel(50),
    tratio(4),
    toutput(-10),
    tthreshold(-24),
    a_out(1),
    stereo(),
    tknee(30),
    peak(),
    clipping(),
    limit(),
    cSAMPLE_RATE(1.0 / sample_rate),
    PERIOD(intermediate_bufsize),       // correct for rakarrack, may be adjusted by lv2
    rvolume(),
    lvolume(),
    rvolume_db(),
    lvolume_db(),
    thres_db(),
    knee(),
    thres_mx(),
    kpct(),
    ratio(1.0),
    kratio(),
    eratio(),
    makeup(),
    makeuplin(),
    outlevel(),
    att(),
    attr(1.0f),
    attl(1.0f),
    rel(),
    relr(1.0f),
    rell(1.0f),
    relcnst(),
    attconst(),
    ltimer(),
    rtimer(),
    hold((int) (sample_rate * 0.0125)),  //  12.5ms
    rgain(1.0f),
    rgain_old(1.0f),
    lgain(1.0f),
    lgain_old(1.0f),
    lgain_t(1.0f),
    rgain_t(1.0f),
    coeff_kratio(),
    coeff_ratio(),
    coeff_knee(),
    coeff_kk(),
    lpeak(),
    rpeak()
{
    Compressor::setpreset(0);
}

Compressor::~Compressor()
{
}

void
Compressor::cleanup()
{
    lgain = rgain = lgain_t = rgain_t = 1.0f;
    lgain_old = rgain_old = 1.0f;
    rpeak = 0.0f;
    lpeak = 0.0f;
    limit = 0;
    clipping = 0;
    eratio = 0.0f;
    lvolume = lvolume_db = 0.0f;
    rvolume = rvolume_db = 0.0f;
    attr = relr = rell = 1.0f;
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Compressor::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
}
#endif // LV2

void
Compressor::set_random_parameters()
{
    for(int i = 0; i < C_COMPRESS_PARAMETERS; i++)
    {
        switch(i)
        {
            case Compress_Threshold:
            {
                int value = (int) (RND * 57);
                changepar (i, value - 60);
            }
            break;

            case Compress_Ratio:
            {
                int value = (int) (RND * 40);
                changepar (i, value + 2);
            }
            break;

            case Compress_Output:
            {
                int value = (int) (RND * 40);
                changepar (i, value - 40);
            }
            break;

            case Compress_Attack:
            {
                int value = (int) (RND * 240);
                changepar (i, value + 10);
            }
            break;

            case Compress_Release:
            {
                int value = (int) (RND * 490);
                changepar (i, value + 10);
            }
            break;

            case Compress_Knee:
            {
                int value = (int) (RND * 100);
                changepar (i, value);
            }
            break;
                
            case Compress_Auto_Out:
            case Compress_Stereo:
            case Compress_Peak:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Compressor::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_COMPRESS_PARAMETERS; i++)
    {
        switch(i)
        {
            case Compress_Threshold:
            case Compress_Ratio:
            case Compress_Output:
            case Compress_Attack:
            case Compress_Release:
            case Compress_Auto_Out:
            case Compress_Knee:
            case Compress_Stereo:
            case Compress_Peak:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), comp_parameters[i * 3 + 1], comp_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Compress_Peak)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Compressor::out(float *efxoutl, float *efxoutr)
{
    bool have_nans = false;

    for (unsigned i = 0; i < PERIOD; i++)
    {
        //Right Channel

        if (peak)
        {
            if (rtimer > hold)
            {
                rpeak *= 0.9998f; //The magic number corresponds to ~0.1s based on T/(RC + T),
                rtimer--;
            }
            
            if (ltimer > hold)
            {
                lpeak *= 0.9998f; //leaky peak detector.
                ltimer--; //keeps the timer from eventually exceeding max int & rolling over
            }
            
            ltimer++;
            rtimer++;
            
            if (rpeak < fabs(efxoutr[i]))
            {
                rpeak = fabs(efxoutr[i]);
                rtimer = 0;
            }
            
            if (lpeak < fabs(efxoutl[i]))
            {
                lpeak = fabs(efxoutl[i]);
                ltimer = 0;
            }

            if (lpeak > 20.0f) lpeak = 20.0f;
            
            if (rpeak > 20.0f) rpeak = 20.0f; //keeps limiter from getting locked up when signal levels go way out of bounds (like hundreds)
        }
        else
        {
            rpeak = efxoutr[i];
            lpeak = efxoutl[i];
        }

        if (stereo)
        {
            float rdelta = fabsf(rpeak);
            
            if (rvolume < 0.9f)
            {
                attr = att;
                relr = rel;
            }
            else if (rvolume < 1.0f)
            {
                attr = att + ((1.0f - att)*(rvolume - 0.9f)*10.0f); //dynamically change attack time for limiting mode
                relr = rel / (1.0f + (rvolume - 0.9f)*9.0f); //release time gets longer when signal is above limiting
            }
            else
            {
                attr = 1.0f;
                relr = rel * 0.1f;
            }

            if (rdelta > rvolume)
            {
                rvolume = attr * rdelta + (1.0f - attr) * rvolume;
            }
            else
            {
                rvolume = relr * rdelta + (1.0f - relr) * rvolume;
            }


            rvolume_db = rap2dB(rvolume);
            if (rvolume_db < thres_db)
            {
                rgain = outlevel;
            }
            else if (rvolume_db < thres_mx)
            {
                //Dynamic ratio that depends on volume.  As can be seen, ratio starts
                //at something negligibly larger than 1 once volume exceeds thres, and increases toward selected
                // ratio by the time it has reached thres_mx.  --Transmogrifox

                eratio = 1.0f + (kratio - 1.0f)*(rvolume_db - thres_db) * coeff_knee;
                rgain = outlevel * dB2rap(thres_db + (rvolume_db - thres_db) / eratio - rvolume_db);
            }
            else
            {
                rgain = outlevel * dB2rap(thres_db + coeff_kk + (rvolume_db - thres_mx) * coeff_ratio - rvolume_db);
                limit = 1;
            }

            if (rgain < MIN_GAIN) rgain = MIN_GAIN;
            rgain_t = .4f * rgain + .6f * rgain_old;
        }

        //Left Channel
        float ldelta;   // initialize o.k.
        if (stereo)
        {
            ldelta = fabsf(lpeak);
        }
        else
        {
            ldelta = 0.5f * (fabsf(lpeak) + fabsf(rpeak));
        }  //It's not as efficient to check twice, but it's small expense worth code clarity

        if (lvolume < 0.9f)
        {
            attl = att;
            rell = rel;
        }
        else if (lvolume < 1.0f)
        {
            attl = att + ((1.0f - att)*(lvolume - 0.9f)*10.0f); //dynamically change attack time for limiting mode
            rell = rel / (1.0f + (lvolume - 0.9f)*9.0f); //release time gets longer when signal is above limiting
        }
        else
        {
            attl = 1.0f;
            rell = rel * 0.1f;
        }

        if (ldelta > lvolume)
        {
            lvolume = attl * ldelta + (1.0f - attl) * lvolume;
        }
        else
        {
            lvolume = rell * ldelta + (1.0f - rell) * lvolume;
        }

        lvolume_db = rap2dB(lvolume);

        if (lvolume_db < thres_db)
        {
            lgain = outlevel;
        }
        else if (lvolume_db < thres_mx)
        { //knee region
            eratio = 1.0f + (kratio - 1.0f)*(lvolume_db - thres_db) * coeff_knee;
            lgain = outlevel * dB2rap(thres_db + (lvolume_db - thres_db) / eratio - lvolume_db);
        }
        else
        {
            lgain = outlevel * dB2rap(thres_db + coeff_kk + (lvolume_db - thres_mx) * coeff_ratio - lvolume_db);
            limit = 1;
        }

        if (lgain < MIN_GAIN) lgain = MIN_GAIN;
        
        lgain_t = .4f * lgain + .6f * lgain_old;

        if (stereo)
        {
            efxoutl[i] *= lgain_t;
            efxoutr[i] *= rgain_t;
            rgain_old = rgain;
            lgain_old = lgain;
        }
        else
        {
            efxoutl[i] *= lgain_t;
            efxoutr[i] *= lgain_t;
            lgain_old = lgain;
        }

        if (peak)
        {
            if (efxoutl[i] > 0.999f)
            { //output hard limiting
                efxoutl[i] = 0.999f;
                clipping = 1;
            }
            
            if (efxoutl[i]<-0.999f)
            {
                efxoutl[i] = -0.999f;
                clipping = 1;
            }
            
            if (efxoutr[i] > 0.999f)
            {
                efxoutr[i] = 0.999f;
                clipping = 1;
            }
            
            if (efxoutr[i]<-0.999f)
            {
                efxoutr[i] = -0.999f;
                clipping = 1;
            }
            //highly probably there is a more elegant way to do that, but what the hey...
        }

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();
}

void
Compressor::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

void
Compressor::setpreset(int npreset)
{
    const int PRESET_SIZE = C_COMPRESS_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //2:1
        {-30, 2, -6, 20, 120, 1, 0, 0, 0},
        //4:1
        {-26, 4, -8, 20, 120, 1, 10, 0, 0},
        //8:1
        {-24, 8, -12, 20, 35, 1, 30, 0, 0},
        //limiter - from rkrlv2
        {-3, 15, 0, 10, 250, 0, 0, 1, 1},
    };

    if (npreset > NUM_PRESETS - 1) 
    {
        Fpre->ReadPreset(EFX_COMPRESSOR, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else    // No user defined (Insert) presets for Compressor
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
}

void
Compressor::changepar(int npar, int value)
{
    switch (npar)
    {
    case Compress_Threshold:
        tthreshold = value;
        thres_db = (float) tthreshold; //implicit type cast int to float
        break;

    case Compress_Ratio:
        tratio = value;
        ratio = (float) tratio;
        break;

    case Compress_Output:
        toutput = value;
        break;

    case Compress_Attack:
        tatt = value;
        att = cSAMPLE_RATE / (((float) value / 1000.0f) + cSAMPLE_RATE);
        attr = att;
        attl = att;
        break;

    case Compress_Release:
        trel = value;
        rel = cSAMPLE_RATE / (((float) value / 1000.0f) + cSAMPLE_RATE);
        rell = rel;
        relr = rel;
        break;

    case Compress_Auto_Out:
        a_out = value;
        break;

    case Compress_Knee:
        tknee = value; //knee expressed a percentage of range between thresh and zero dB
        kpct = (float) tknee / 100.1f;
        break;

    case Compress_Stereo:
        stereo = value;
        break;
    case Compress_Peak:
        peak = value;
        break;

    default:
        return;
    }

    kratio = logf(ratio) / LOG_2; //  Log base 2 relationship matches slope
    knee = -kpct*thres_db;

    coeff_kratio = 1.0 / kratio;
    coeff_ratio = 1.0 / ratio;
    coeff_knee = 1.0 / knee;

    coeff_kk = knee * coeff_kratio;


    thres_mx = thres_db + knee; //This is the value of the input when the output is at t+k
    makeup = -thres_db - knee / kratio + thres_mx / ratio;
    makeuplin = dB2rap(makeup);
    
    if (a_out)
    {
        outlevel = dB2rap((float) toutput) * makeuplin;
    }
    else
    {
        outlevel = dB2rap((float) toutput);
    }
}

int
Compressor::getpar(int npar)
{
    switch (npar)
    {
    case Compress_Threshold:
        return (tthreshold);

    case Compress_Ratio:
        return (tratio);

    case Compress_Output:
        return (toutput);

    case Compress_Attack:
        return (tatt);

    case Compress_Release:
        return (trel);

    case Compress_Auto_Out:
        return (a_out);

    case Compress_Knee:
        return (tknee);

    case Compress_Stereo:
        return (stereo);

    case Compress_Peak:
        return (peak);

    }

    return (0);
}
