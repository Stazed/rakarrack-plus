/*

  CompBand.C - 4 Bands Compressor

  Using Compressor and AnalogFilters by other authors.

  Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
  Stefan Westerfeld <stefan@space.twc.de>

  Modified by Ryan Billing & Josep Andreu

  ZynAddSubFX - a software synthesizer
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CompBand.h"

/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */



CompBand::CompBand(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    level(),
    lowl(NULL),
    lowr(NULL),
    midll(NULL),
    midlr(NULL),
    midhl(NULL),
    midhr(NULL),
    highl(NULL),
    highr(NULL),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(50),
    Plevel(),
    PLratio(),
    PMLratio(),
    PMHratio(),
    PHratio(),
    PLthres(),
    PMLthres(),
    PMHthres(),
    PHthres(),
    Cross1(),
    Cross2(),
    Cross3(),
    lpf1l(NULL),
    lpf1r(NULL),
    hpf1l(NULL),
    hpf1r(NULL),
    lpf2l(NULL),
    lpf2r(NULL),
    hpf2l(NULL),
    hpf2r(NULL),
    lpf3l(NULL),
    lpf3r(NULL),
    hpf3l(NULL),
    hpf3r(NULL),
    interpbuf(NULL),
    CL(NULL),
    CML(NULL),
    CMH(NULL),
    CH(NULL)
{
    initialize();

    // The Limiter here is derived from Compressor with special presets
    CL = new Limiter(sample_rate, PERIOD);
    CML = new Limiter(sample_rate, PERIOD);
    CMH = new Limiter(sample_rate, PERIOD);
    CH = new Limiter(sample_rate, PERIOD);

    CL->setpreset(2);
    CML->setpreset(2);
    CMH->setpreset(2);
    CH->setpreset(2);

    CompBand::setpreset(Ppreset);
    CompBand::cleanup();
}

CompBand::~CompBand()
{
    clear_initialize();

    delete CL;
    delete CML;
    delete CMH;
    delete CH;
}

/*
 * Cleanup the effect
 */
void
CompBand::cleanup()
{
    lpf1l->cleanup();
    hpf1l->cleanup();
    lpf1r->cleanup();
    hpf1r->cleanup();
    lpf2l->cleanup();
    hpf2l->cleanup();
    lpf2r->cleanup();
    hpf2r->cleanup();
    lpf3l->cleanup();
    hpf3l->cleanup();
    lpf3r->cleanup();
    hpf3r->cleanup();
    CL->cleanup();
    CML->cleanup();
    CMH->cleanup();
    CH->cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
CompBand::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    setCross1(Cross1);
    setCross2(Cross2);
    setCross3(Cross3);

    CL->lv2_update_params(period);
    CML->lv2_update_params(period);
    CMH->lv2_update_params(period);
    CH->lv2_update_params(period);
}
#endif // LV2

void
CompBand::set_random_parameters()
{
    for(int i = 0; i < C_COMPBAND_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case CompBand_Low_Ratio:
            case CompBand_Mid_1_Ratio:
            case CompBand_Mid_2_Ratio:
            case CompBand_High_Ratio:
            {
                int value = (int) (RND * 40);
                changepar (i, value + 2);
            }
            break;

            case CompBand_Low_Thresh:
            case CompBand_Mid_1_Thresh:
            case CompBand_Mid_2_Thresh:
            case CompBand_High_Thresh:
            {
                int value = (int) (RND * 94);
                changepar (i, value - 70);
            }
            break;

            case CompBand_Cross_1:
            {
                int value = (int) (RND * 980);
                changepar (i, value + 20);
            }
            break;

            case CompBand_Cross_2:
            {
                int value = (int) (RND * 7000);
                changepar (i, value + 1000);
            }
            break;

            case CompBand_Cross_3:
            {
                int value = (int) (RND * 24000);
                changepar (i, value + 2000);
            }
            break;

            case CompBand_DryWet:
            case CompBand_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;
        }
    }
}

void
CompBand::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_COMPBAND_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case CompBand_Low_Ratio:
            case CompBand_Mid_1_Ratio:
            case CompBand_Mid_2_Ratio:
            case CompBand_High_Ratio:
            case CompBand_Low_Thresh:
            case CompBand_Mid_1_Thresh:
            case CompBand_Mid_2_Thresh:
            case CompBand_High_Thresh:
            case CompBand_Cross_1:
            case CompBand_Cross_2:
            case CompBand_Cross_3:
            case CompBand_Gain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), compband_parameters[i * 3 + 1], compband_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  CompBand_Gain )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case CompBand_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( CompBand_DryWet )), compband_parameters[i * 3 + 1], compband_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( CompBand_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
CompBand::initialize()
{
    lowl = (float *) malloc(sizeof (float) * PERIOD);
    lowr = (float *) malloc(sizeof (float) * PERIOD);
    midll = (float *) malloc(sizeof (float) * PERIOD);
    midlr = (float *) malloc(sizeof (float) * PERIOD);
    midhl = (float *) malloc(sizeof (float) * PERIOD);
    midhr = (float *) malloc(sizeof (float) * PERIOD);
    highl = (float *) malloc(sizeof (float) * PERIOD);
    highr = (float *) malloc(sizeof (float) * PERIOD);

    interpbuf = new float[PERIOD];
    lpf1l = new AnalogFilter(2, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf1r = new AnalogFilter(2, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf1l = new AnalogFilter(3, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf1r = new AnalogFilter(3, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf2l = new AnalogFilter(2, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf2r = new AnalogFilter(2, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf2l = new AnalogFilter(3, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf2r = new AnalogFilter(3, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf3l = new AnalogFilter(2, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf3r = new AnalogFilter(2, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf3l = new AnalogFilter(3, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf3r = new AnalogFilter(3, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
}

void
CompBand::clear_initialize()
{
    free(lowl);
    free(lowr);
    free(midll);
    free(midlr);
    free(midhl);
    free(midhr);
    free(highl);
    free(highr);

    delete lpf1l;
    delete lpf1r;
    delete hpf1l;
    delete hpf1r;
    delete lpf2l;
    delete lpf2r;
    delete hpf2l;
    delete hpf2r;
    delete lpf3l;
    delete lpf3r;
    delete hpf3l;
    delete hpf3r;
    delete[] interpbuf;
}

/*
 * Effect output
 */
void
CompBand::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    memcpy(lowl, efxoutl, sizeof (float) * PERIOD);
    memcpy(midll, efxoutl, sizeof (float) * PERIOD);
    memcpy(midhl, efxoutl, sizeof (float) * PERIOD);
    memcpy(highl, efxoutl, sizeof (float) * PERIOD);

    lpf1l->filterout(lowl, PERIOD);
    hpf1l->filterout(midll, PERIOD);
    lpf2l->filterout(midll, PERIOD);
    hpf2l->filterout(midhl, PERIOD);
    lpf3l->filterout(midhl, PERIOD);
    hpf3l->filterout(highl, PERIOD);

    memcpy(lowr, efxoutr, sizeof (float) * PERIOD);
    memcpy(midlr, efxoutr, sizeof (float) * PERIOD);
    memcpy(midhr, efxoutr, sizeof (float) * PERIOD);
    memcpy(highr, efxoutr, sizeof (float) * PERIOD);

    lpf1r->filterout(lowr, PERIOD);
    hpf1r->filterout(midlr, PERIOD);
    lpf2r->filterout(midlr, PERIOD);
    hpf2r->filterout(midhr, PERIOD);
    lpf3r->filterout(midhr, PERIOD);
    hpf3r->filterout(highr, PERIOD);

    CL->out(lowl, lowr);
    CML->out(midll, midlr);
    CMH->out(midhl, midhr);
    CH->out(highl, highr);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        efxoutl[i] = (lowl[i] + midll[i] + midhl[i] + highl[i]) * level;
        efxoutr[i] = (lowr[i] + midlr[i] + midhr[i] + highr[i]) * level;

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();
}

/*
 * Parameter control
 */
void
CompBand::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 128.0f;
}

void
CompBand::setlevel(int value)
{
    Plevel = value;
    level = dB2rap(60.0f * (float) value / 127.0f - 36.0f);
}

void
CompBand::setratio(int ch, int value)
{
    switch (ch)
    {
    case 0:
        CL->changepar(Compress_Ratio, value);
        break;
    case 1:
        CML->changepar(Compress_Ratio, value);
        break;
    case 2:
        CMH->changepar(Compress_Ratio, value);
        break;
    case 3:
        CH->changepar(Compress_Ratio, value);
        break;
    }
}

void
CompBand::setthres(int ch, int value)
{
    switch (ch)
    {
    case 0:
        CL->changepar(Compress_Threshold, value);
        break;
    case 1:
        CML->changepar(Compress_Threshold, value);
        break;
    case 2:
        CMH->changepar(Compress_Threshold, value);
        break;
    case 3:
        CH->changepar(Compress_Threshold, value);
        break;
    }
}

void
CompBand::setCross1(int value)
{
    Cross1 = value;
    lpf1l->setfreq((float) value);
    lpf1r->setfreq((float) value);
    hpf1l->setfreq((float) value);
    hpf1r->setfreq((float) value);
}

void
CompBand::setCross2(int value)
{
    Cross2 = value;
    hpf2l->setfreq((float) value);
    hpf2r->setfreq((float) value);
    lpf2l->setfreq((float) value);
    lpf2r->setfreq((float) value);
}

void
CompBand::setCross3(int value)
{
    Cross3 = value;
    hpf3l->setfreq((float) value);
    hpf3r->setfreq((float) value);
    lpf3l->setfreq((float) value);
    lpf3r->setfreq((float) value);
}

void
CompBand::setpreset(int npreset)
{
    const int PRESET_SIZE = C_COMPBAND_PARAMETERS;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Good Start
        {0, 16, 16, 16, 16, 0, 0, 0, 0, 1000, 5000, 10000, 48},

        //Loudness
        {0, 16, 2, 2, 4, -16, 24, 24, -8, 140, 1000, 5000, 48},

        //Loudness 2
        {64, 16, 2, 2, 2, -32, 24, 24, 24, 100, 1000, 5000, 48}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_COMPBAND, npreset - NUM_PRESETS + 1, pdata);

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = npreset;
    cleanup();
}

void
CompBand::changepar(int npar, int value)
{
    switch (npar)
    {
    case CompBand_DryWet:
        setvolume(value);
        break;
    case CompBand_Low_Ratio:
        if (value < 2) // some old bank files have garbage in them
            value = 2;
        PLratio = value;
        setratio(0, value);
        break;
    case CompBand_Mid_1_Ratio:
        if (value < 2) // some old bank files have garbage in them
            value = 2;
        PMLratio = value;
        setratio(1, value);
        break;
    case CompBand_Mid_2_Ratio:
        if (value < 2) // some old bank files have garbage in them
            value = 2;
        PMHratio = value;
        setratio(2, value);
        break;
    case CompBand_High_Ratio:
        if (value < 2) // some old bank files have garbage in them
            value = 2;
        PHratio = value;
        setratio(3, value);
        break;
    case CompBand_Low_Thresh:
        PLthres = value;
        setthres(0, value);
        break;
    case CompBand_Mid_1_Thresh:
        PMLthres = value;
        setthres(1, value);
        break;
    case CompBand_Mid_2_Thresh:
        PMHthres = value;
        setthres(2, value);
        break;
    case CompBand_High_Thresh:
        PHthres = value;
        setthres(3, value);
        break;
    case CompBand_Cross_1:
        if (value < 20) // some old bank files have garbage in them
            value = 20;
        setCross1(value);
        break;
    case CompBand_Cross_2:
        if (value < 1000) // some old bank files have garbage in them
            value = 1000;
        setCross2(value);
        break;
    case CompBand_Cross_3:
        if (value < 2000) // some old bank files have garbage in them
            value = 2000;
        setCross3(value);
        break;
    case CompBand_Gain:
        setlevel(value);
        break;
    default:
        return;
    }
}

int
CompBand::getpar(int npar)
{
    switch (npar)
    {
    case CompBand_DryWet:
        return (Pvolume);

    case CompBand_Low_Ratio:
        return (PLratio);

    case CompBand_Mid_1_Ratio:
        return (PMLratio);

    case CompBand_Mid_2_Ratio:
        return (PMHratio);

    case CompBand_High_Ratio:
        return (PHratio);

    case CompBand_Low_Thresh:
        return (PLthres);

    case CompBand_Mid_1_Thresh:
        return (PMLthres);

    case CompBand_Mid_2_Thresh:
        return (PMHthres);

    case CompBand_High_Thresh:
        return (PHthres);

    case CompBand_Cross_1:
        return (Cross1);

    case CompBand_Cross_2:
        return (Cross2);

    case CompBand_Cross_3:
        return (Cross3);

    case CompBand_Gain:
        return (Plevel);

    }
    return (0); //in case of bogus parameter number
}

