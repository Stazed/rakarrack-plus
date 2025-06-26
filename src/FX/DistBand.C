/*
  DistBand.C - Distorsion effect

  ZynAddSubFX - a software synthesizer
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

// Renamed from MBDist.h on 4/18/2020 by stazed.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "DistBand.h"

/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */

// DistBand

DistBand::DistBand(int wave_res, int wave_upq, int wave_dnq,
        double sample_rate, uint32_t intermediate_bufsize) :
    Effect(wave_res, wave_upq, wave_dnq, sample_rate, intermediate_bufsize),
    WAVE_RES(wave_res),
    WAVE_UPQ(wave_upq),
    WAVE_DNQ(wave_dnq),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    lowl(NULL),
    lowr(NULL),
    midl(NULL),
    midr(NULL),
    highl(NULL),
    highr(NULL),
    Pvolume(50),
    Ppanning(),
    Plrcross(40),
    Pdrive(90),
    Plevel(64),
    PtypeL(),
    PtypeM(),
    PtypeH(),
    PvolL(),
    PvolM(),
    PvolH(),
    PdriveL(),
    PdriveM(),
    PdriveH(),
    Pnegate(),
    Cross1(),
    Cross2(),
    Pstereo(),
    panning(),
    lrcross(),
    volL(),
    volM(),
    volH(),
    lpf1l(NULL),
    lpf1r(NULL),
    hpf1l(NULL),
    hpf1r(NULL),
    lpf2l(NULL),
    lpf2r(NULL),
    hpf2l(NULL),
    hpf2r(NULL),
    DCl(NULL),
    DCr(NULL),
    interpbuf(NULL),
    mbwshape1l(NULL),
    mbwshape2l(NULL),
    mbwshape3l(NULL),
    mbwshape1r(NULL),
    mbwshape2r(NULL),
    mbwshape3r(NULL)
{
    initialize();

    DistBand::setpreset(Ppreset);
    DistBand::cleanup();
}

DistBand::~DistBand()
{
    clear_initialize();
}

/*
 * Cleanup the effect
 */
void
DistBand::cleanup()
{
    lpf1l->cleanup();
    hpf1l->cleanup();
    lpf1r->cleanup();
    hpf1r->cleanup();
    lpf2l->cleanup();
    hpf2l->cleanup();
    lpf2r->cleanup();
    hpf2r->cleanup();
    DCl->cleanup();
    DCr->cleanup();
}

std::vector<int>
DistBand::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_DBAND_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
DistBand::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_DBAND_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
DistBand::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    setCross1(Cross1);
    setCross2(Cross2);
}
#endif // LV2

void
DistBand::set_random_parameters()
{
    for(int i = 0; i < C_DBAND_PARAMETERS; i++)
    {
        switch(i)
        {
            case DistBand_DryWet:
            case DistBand_Pan:
            case DistBand_LR_Cross:
            case DistBand_Drive:
            case DistBand_Level:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case DistBand_Type_Low:
            case DistBand_Type_Mid:
            case DistBand_Type_Hi:
            {
                int value = (int) (RND * 30);
                changepar (i, value);
            }
            break;

            case DistBand_Gain_Low:
            case DistBand_Gain_Mid:
            case DistBand_Gain_Hi:
            {
                int value = (int) (RND * 100);
                changepar (i, value);
            }
            break;

            case DistBand_Negate:
            case DistBand_Stereo:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case DistBand_Cross_1:
            {
                int value = (int) (RND * 980);
                changepar (i, value + 20);
            }
            break;

            case DistBand_Cross_2:
            {
                int value = (int) (RND * 11200);
                changepar (i, value + 800);
            }
            break;
        }
    }
}

void
DistBand::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_DBAND_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case DistBand_LR_Cross:
            case DistBand_Drive:
            case DistBand_Level:
            case DistBand_Type_Low:
            case DistBand_Type_Mid:
            case DistBand_Type_Hi:
            case DistBand_Gain_Low:
            case DistBand_Gain_Mid:
            case DistBand_Gain_Hi:
            case DistBand_Negate:
            case DistBand_Cross_1:
            case DistBand_Cross_2:
            case DistBand_Stereo:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), distband_parameters[i * 3 + 1], distband_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  DistBand_Stereo)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case DistBand_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( DistBand_DryWet )), distband_parameters[i * 3 + 1], distband_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( DistBand_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
            
            // Offset
            case DistBand_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( DistBand_Pan ) - 64, distband_parameters[i * 3 + 1], distband_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( DistBand_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
DistBand::initialize()
{
    lowl = (float *) malloc(sizeof (float) * PERIOD);
    lowr = (float *) malloc(sizeof (float) * PERIOD);
    midl = (float *) malloc(sizeof (float) * PERIOD);
    midr = (float *) malloc(sizeof (float) * PERIOD);
    highl = (float *) malloc(sizeof (float) * PERIOD);
    highr = (float *) malloc(sizeof (float) * PERIOD);

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        lowl[i] = lowr[i] = 0;
        midl[i] = midr[i] = 0;
        highl[i] = highr[i] = 0;
    }

    interpbuf = new float[PERIOD];
    lpf1l = new AnalogFilter(2, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf1r = new AnalogFilter(2, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf1l = new AnalogFilter(3, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf1r = new AnalogFilter(3, 500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf2l = new AnalogFilter(2, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf2r = new AnalogFilter(2, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf2l = new AnalogFilter(3, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf2r = new AnalogFilter(3, 2500.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    DCl = new AnalogFilter(3, 30, 1, 0, fSAMPLE_RATE, interpbuf);
    DCr = new AnalogFilter(3, 30, 1, 0, fSAMPLE_RATE, interpbuf);
    DCl->setfreq(30.0f);
    DCr->setfreq(30.0f);

    mbwshape1l = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    mbwshape2l = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    mbwshape3l = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);

    mbwshape1r = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    mbwshape2r = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    mbwshape3r = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
}

void
DistBand::clear_initialize()
{
    free(lowl);
    free(lowr);
    free(midl);
    free(midr);
    free(highl);
    free(highr);

    delete[] interpbuf;
    delete lpf1l;
    delete lpf1r;
    delete hpf1l;
    delete hpf1r;
    delete lpf2l;
    delete lpf2r;
    delete hpf2l;
    delete hpf2r;
    delete DCl;
    delete DCr;

    delete mbwshape1l;
    delete mbwshape2l;
    delete mbwshape3l;

    delete mbwshape1r;
    delete mbwshape2r;
    delete mbwshape3r;
}

/*
 * Effect output
 */
void
DistBand::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    float inputvol = powf(5.0f, ((float) Pdrive - 32.0f) / 127.0f);
    
    if (Pnegate != 0)
    {
        inputvol *= -1.0f;
    }

    if (Pstereo)
    {
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            efxoutl[i] = efxoutl[i] * inputvol * 2.0f;
            efxoutr[i] = efxoutr[i] * inputvol * 2.0f;
        }
    }
    else
    {
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            efxoutl[i] = (efxoutl[i] + efxoutr[i]) * inputvol;
        }
    }

    memcpy(lowl, efxoutl, sizeof (float) * PERIOD);
    memcpy(midl, efxoutl, sizeof (float) * PERIOD);
    memcpy(highl, efxoutl, sizeof (float) * PERIOD);

    lpf1l->filterout(lowl, PERIOD);
    hpf1l->filterout(midl, PERIOD);
    lpf2l->filterout(midl, PERIOD);
    hpf2l->filterout(highl, PERIOD);

    if (volL > 0) mbwshape1l->waveshapesmps(PERIOD, lowl, PtypeL, PdriveL, 1);
    if (volM > 0) mbwshape2l->waveshapesmps(PERIOD, midl, PtypeM, PdriveM, 1);
    if (volH > 0) mbwshape3l->waveshapesmps(PERIOD, highl, PtypeH, PdriveH, 1);

    if (Pstereo)
    {
        memcpy(lowr, efxoutr, sizeof (float) * PERIOD);
        memcpy(midr, efxoutr, sizeof (float) * PERIOD);
        memcpy(highr, efxoutr, sizeof (float) * PERIOD);

        lpf1r->filterout(lowr, PERIOD);
        hpf1r->filterout(midr, PERIOD);
        lpf2r->filterout(midr, PERIOD);
        hpf2r->filterout(highr, PERIOD);

        if (volL > 0) mbwshape1r->waveshapesmps(PERIOD, lowr, PtypeL, PdriveL, 1);
        if (volM > 0) mbwshape2r->waveshapesmps(PERIOD, midr, PtypeM, PdriveM, 1);
        if (volH > 0) mbwshape3r->waveshapesmps(PERIOD, highr, PtypeH, PdriveH, 1);
    }

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        efxoutl[i] = lowl[i] * volL + midl[i] * volM + highl[i] * volH;
        
        if (Pstereo)
        {
            efxoutr[i] = lowr[i] * volL + midr[i] * volM + highr[i] * volH;
        }
    }

    if (!Pstereo)
    {
        memcpy(efxoutr, efxoutl, sizeof (float)* PERIOD);
    }

    float level = dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f);

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        float lout = efxoutl[i];
        float rout = efxoutr[i];

        float l = lout * (1.0f - lrcross) + rout * lrcross;
        float r = rout * (1.0f - lrcross) + lout * lrcross;

        efxoutl[i] = l * 2.0f * level * (1.0f - panning);
        efxoutr[i] = r * 2.0f * level * panning;

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();

    DCr->filterout(efxoutr, PERIOD);
    DCl->filterout(efxoutl, PERIOD);
}

/*
 * Parameter control
 */
void
DistBand::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
DistBand::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    panning = ((float) _Ppanning + 0.5f) / 127.0f;
}

void
DistBand::setlrcross(int _Plrcross)
{
    Plrcross = _Plrcross;
    lrcross = (float) _Plrcross / 127.0f * 1.0f;
}

void
DistBand::setCross1(int value)
{
    Cross1 = value;
    lpf1l->setfreq((float) value);
    lpf1r->setfreq((float) value);
    hpf1l->setfreq((float) value);
    hpf1r->setfreq((float) value);
}

void
DistBand::setCross2(int value)
{
    Cross2 = value;
    hpf2l->setfreq((float) value);
    hpf2r->setfreq((float) value);
    lpf2l->setfreq((float) value);
    lpf2r->setfreq((float) value);
}

void
DistBand::setpreset(int npreset)
{
    const int PRESET_SIZE = C_DBAND_PARAMETERS;
    const int NUM_PRESETS = 8;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Saturation
        {0, 64, 0, 41, 64, 26, 19, 26, 41, 20, 35, 0, 400, 1200, 0},
        //Dist 1
        {0, 64, 64, 20, 64, 0, 14, 13, 38, 49, 40, 0, 288, 1315, 0},
        //Soft
        {0, 64, 0, 32, 64, 6, 13, 6, 50, 70, 50, 0, 400, 1800, 0},
        //Modulated
        {0, 64, 0, 36, 64, 18, 17, 18, 40, 70, 30, 0, 500, 2200, 0},
        //Crunch
        {0, 64, 0, 24, 64, 19, 14, 19, 30, 80, 30, 0, 800, 1800, 0},
        //Dist 2
        {0, 64, 0, 64, 64, 22, 27, 22, 25, 50, 25, 0, 440, 1500, 0},
        //Dist 3
        {0, 64, 0, 64, 64, 27, 22, 27, 50, 69, 50, 0, 800, 1200, 0},
        //Dist 4
        {0, 64, 0, 30, 64, 19, 25, 26, 20, 51, 83, 0, 329, 800, 0}
    };
    
    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_DISTBAND, npreset - NUM_PRESETS + 1, pdata);
        
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
DistBand::changepar(int npar, int value)
{
    switch (npar)
    {
    case DistBand_DryWet:
        setvolume(value);
        break;
    case DistBand_Pan:
        setpanning(value);
        break;
    case DistBand_LR_Cross:
        setlrcross(value);
        break;
    case DistBand_Drive:
        Pdrive = value;
        PdriveL = (int) ((float) Pdrive * volL);
        PdriveM = (int) ((float) Pdrive * volM);
        PdriveH = (int) ((float) Pdrive * volH);
        break;
    case DistBand_Level:
        Plevel = value;
        break;
    case DistBand_Type_Low:
        PtypeL = value;
        break;
    case DistBand_Type_Mid:
        PtypeM = value;
        break;
    case DistBand_Type_Hi:
        PtypeH = value;
        break;
    case DistBand_Gain_Low:
        PvolL = value;
        volL = (float) value / 100.0;
        PdriveL = (int) ((float) Pdrive * volL);
        break;
    case DistBand_Gain_Mid:
        PvolM = value;
        volM = (float) value / 100.0;
        PdriveM = (int) ((float) Pdrive * volM);
        break;
    case DistBand_Gain_Hi:
        PvolH = value;
        volH = (float) value / 100.0;
        PdriveH = (int) ((float) Pdrive * volH);
        break;
    case DistBand_Negate:
        Pnegate = value;
        break;
    case DistBand_Cross_1:
        setCross1(value);
        break;
    case DistBand_Cross_2:
        setCross2(value);
        break;
    case DistBand_Stereo:
        Pstereo = value;
        break;
    default:
        return;
    }
}

int
DistBand::getpar(int npar)
{
    switch (npar)
    {
    case DistBand_DryWet:
        return (Pvolume);

    case DistBand_Pan:
        return (Ppanning);

    case DistBand_LR_Cross:
        return (Plrcross);

    case DistBand_Drive:
        return (Pdrive);

    case DistBand_Level:
        return (Plevel);

    case DistBand_Type_Low:
        return (PtypeL);

    case DistBand_Type_Mid:
        return (PtypeM);

    case DistBand_Type_Hi:
        return (PtypeH);

    case DistBand_Gain_Low:
        return (PvolL);

    case DistBand_Gain_Mid:
        return (PvolM);

    case DistBand_Gain_Hi:
        return (PvolH);

    case DistBand_Negate:
        return (Pnegate);

    case DistBand_Cross_1:
        return (Cross1);

    case DistBand_Cross_2:
        return (Cross2);

    case DistBand_Stereo:
        return (Pstereo);

    }
    return (0); //in case of bogus parameter number
}

