/*
  VaryBand.C - Vary Band Volumen effect

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

// Renamed from MBVvol.h 4/18/2020 by stazed

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "VaryBand.h"

VaryBand::VaryBand(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    lowl(NULL),
    lowr(NULL),
    midll(NULL),
    midlr(NULL),
    midhl(NULL),
    midhr(NULL),
    highl(NULL),
    highr(NULL),
    Pvolume(50),
    Pcombi(0),
    Cross1(500),
    Cross2(2500),
    Cross3(5000),
    PsL(0),
    PsML(0),
    PsMH(0),
    PsH(0),
    lfo1l(),
    lfo1r(),
    lfo2l(),
    lfo2r(),
    v1l(),
    v1r(),
    v2l(),
    v2r(),
    d1(),
    d2(),
    d3(),
    d4(),
    volL(2.0f),
    volML(2.0f),
    volMH(2.0f),
    volH(2.0f),
    volLr(2.0f),
    volMLr(2.0f),
    volMHr(2.0f),
    volHr(2.0f),
    sourceL(NULL),
    sourceML(NULL),
    sourceMH(NULL),
    sourceH(NULL),
    sourceLr(NULL),
    sourceMLr(NULL),
    sourceMHr(NULL),
    sourceHr(NULL),
    one(1.0f),
    zero(0.0f),
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
    lfo1(NULL),
    lfo2(NULL)
{
    initialize();

    lfo1 = new EffectLFO(fSAMPLE_RATE);
    lfo2 = new EffectLFO(fSAMPLE_RATE);

    VaryBand::setpreset(Ppreset);
    VaryBand::cleanup();
}

VaryBand::~VaryBand()
{
    clear_initialize();
    delete lfo1;
    delete lfo2;
}

/*
 * Cleanup the effect
 */
void
VaryBand::cleanup()
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
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
VaryBand::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    setCross1(Cross1);
    setCross2(Cross2);
    setCross3(Cross3);

    lfo1->updateparams(PERIOD);
    lfo2->updateparams(PERIOD);
}
#endif // LV2

void
VaryBand::set_random_parameters()
{
    for(int i = 0; i < (C_VARYBAND_PARAMETERS + 4); i++)    // + 4 parameters un-combined volume bands
    {
        switch(i)
        {
            case VaryBand_LFO_Tempo_1:
            case VaryBand_LFO_Tempo_2:
            {
                int value = (int) (RND * LFO_FREQ_MAX);
                changepar (i, value + 1);
            }
            break;

            case VaryBand_LFO_Type_1:
            case VaryBand_LFO_Type_2:
            {
                int value = (int) (RND * LFO_NUM_TYPES);
                changepar (i, value);
            }
            break;

            case VaryBand_DryWet:
            case VaryBand_LFO_Stereo_1:
            case VaryBand_LFO_Stereo_2:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case VaryBand_Cross_1:
            {
                int value = (int) (RND * 980);
                changepar (i, value + 20);
            }
            break;

            case VaryBand_Cross_2:
            {
                int value = (int) (RND * 7000);
                changepar (i, value + 1000);
            }
            break;

            case VaryBand_Cross_3:
            {
                int value = (int) (RND * 24000);
                changepar (i, value + 2000);
            }
            break;

            case VaryBand_Low_Band:
            case VaryBand_Mid_Band_1:
            case VaryBand_Mid_Band_2:
            case VaryBand_High_Band:
            {
                int value = (int) (RND * 4);
                changepar (i, value);
            }
            break;

            case VaryBand_Combination:
                break;
        }
    }
}

void
VaryBand::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    for(int i = 0; i < (C_VARYBAND_PARAMETERS + 3); i++)    // +3 for the 4 LV2 parameters added - 1 for legacy pcombi
    {
        switch(param_case_offset)
        {
            // Normal processing
            case VaryBand_LFO_Tempo_1:
            case VaryBand_LFO_Type_1:
            case VaryBand_LFO_Stereo_1:
            case VaryBand_LFO_Tempo_2:
            case VaryBand_LFO_Type_2:
            case VaryBand_LFO_Stereo_2:
            case VaryBand_Cross_1:
            case VaryBand_Cross_2:
            case VaryBand_Low_Band:
            case VaryBand_Mid_Band_1:
            case VaryBand_Mid_Band_2:
            case VaryBand_High_Band:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), vary_parameters[i * 3 + 1], vary_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  VaryBand_High_Band )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case VaryBand_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( VaryBand_DryWet )), vary_parameters[i * 3 + 1], vary_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( VaryBand_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Skip after
            case VaryBand_Cross_3:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( VaryBand_Cross_3 ), vary_parameters[i * 3 + 1], vary_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( VaryBand_Cross_3 ) );
                    s_buf += ":";
                }
 
                param_case_offset++;    // skip VaryBand_Combination (legacy)
            }
            break;
        }

        param_case_offset++;
    }
}

void
VaryBand::initialize()
{
    lowl = (float *) malloc(sizeof (float) * PERIOD);
    lowr = (float *) malloc(sizeof (float) * PERIOD);
    midll = (float *) malloc(sizeof (float) * PERIOD);
    midlr = (float *) malloc(sizeof (float) * PERIOD);
    midhl = (float *) malloc(sizeof (float) * PERIOD);
    midhr = (float *) malloc(sizeof (float) * PERIOD);
    highl = (float *) malloc(sizeof (float) * PERIOD);
    highr = (float *) malloc(sizeof (float) * PERIOD);
    
    for (unsigned int i = 0; i < PERIOD; i++)
    {
        lowl[i] = lowr[i] = 0;
        midll[i] = midlr[i] = 0;
        midhl[i] = midhr[i] = 0;
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
    lpf3l = new AnalogFilter(2, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    lpf3r = new AnalogFilter(2, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf3l = new AnalogFilter(3, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
    hpf3r = new AnalogFilter(3, 5000.0f, .7071f, 0, fSAMPLE_RATE, interpbuf);
}

void
VaryBand::clear_initialize()
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
VaryBand::out(float * efxoutl, float * efxoutr)
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

    lfo1->effectlfoout(&lfo1l, &lfo1r);
    lfo2->effectlfoout(&lfo2l, &lfo2r);

    d1 = (lfo1l - v1l) / (float) PERIOD;
    d2 = (lfo1r - v1r) / (float) PERIOD;
    d3 = (lfo2l - v2l) / (float) PERIOD;
    d4 = (lfo2r - v2r) / (float) PERIOD;

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        updateVols();

        efxoutl[i] = lowl[i] * volL + midll[i] * volML + midhl[i] * volMH + highl[i] * volH;
        efxoutr[i] = lowr[i] * volLr + midlr[i] * volMLr + midhr[i] * volMHr + highr[i] * volHr;

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
VaryBand::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
VaryBand::updateVols(void)
{
    v1l += d1;
    v1r += d2;
    v2l += d3;
    v2r += d4;

    volL = *sourceL;
    volLr = *sourceLr;
    volML = *sourceML;
    volMLr = *sourceMLr;
    volMH = *sourceMH;
    volMHr = *sourceMHr;
    volH = *sourceH;
    volHr = *sourceHr;
}

//legacy support
void
VaryBand::setCombi(int value)
{
    /*
    NEW         LEGACY
     0            1         LFO 1
     1            2         LF0 2
     2            o         Constant
     3            x         Muted


                                                                 NEW        LEGACY
     {"1122", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0011         0
     {"1221", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0110         1
     {"1212", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0101         2
     {"o11o", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 2002         3
     {"o12o", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 2012         4
     {"x11x", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 3003         5
     {"x12x", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 3013         6
     {"1oo1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0220         7
     {"1oo2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0221         8
     {"1xx1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0330         9
     {"1xx2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0}, == 0331        10

     * The above NEW values plus 10000 are the calculated magic numbers for the legacy case defaults = Pcombi new value.
     * All of this is to maintain the legacy file saving structure and still use the more flexible rkr.lv2 method
     * of allowing users to set their own combination. Legacy had only one saved variable for the four sources.
     * Legacy hard coded the above preset combinations, limiting user flexibility. The rkr.lv2 method separated each
     * source into four parts to allow maximum user flexibility. To avoid having to re-write the rakarrack file structure,
     * the four values are saved in the original Pcombi variable and now use CalcCombi() and parseCombi()
     * for the new method saving and loading.
     */
    if (value < 0 || value > 13333) // sanity check
    {
        Pcombi = value = 0; // set to default
    }

    if (value < 10000) // legacy value from old files and banks
    {
        switch (value)
        {
        case 0:
            Pcombi = 10011;
            break;
        case 1:
            Pcombi = 10110;
            break;
        case 2:
            Pcombi = 10101;
            break;
        case 3:
            Pcombi = 12002;
            break;
        case 4:
            Pcombi = 12012;
            break;
        case 5:
            Pcombi = 13003;
            break;
        case 6:
            Pcombi = 13013;
            break;
        case 7:
            Pcombi = 10220;
            break;
        case 8:
            Pcombi = 10221;
            break;
        case 9:
            Pcombi = 10330;
            break;
        case 10:
            Pcombi = 10331;
            break;
        case 11:        // Added for rkrlv2 Preset Vary4
            Pcombi = 10000;
            break;
        }
    }

    parseCombi(Pcombi);

    // --------------------------------

#if 0
    /* Legacy method */
    switch (value)
    {
    case 0:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &v1l;
        sourceMLr = &v1r;
        sourceMH = &v2l;
        sourceMHr = &v2r;
        sourceH = &v2l;
        sourceHr = &v2r;
        break;
    case 1:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &v2l;
        sourceMLr = &v2r;
        sourceMH = &v2l;
        sourceMHr = &v2r;
        sourceH = &v1l;
        sourceHr = &v1r;
        break;
    case 2:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &v2l;
        sourceMLr = &v2r;
        sourceMH = &v1l;
        sourceMHr = &v1r;
        sourceH = &v2l;
        sourceHr = &v2r;
        break;
    case 3:
        sourceL = &one;
        sourceLr = &one;
        sourceML = &v1l;
        sourceMLr = &v1r;
        sourceMH = &v1l;
        sourceMHr = &v1r;
        sourceH = &one;
        sourceHr = &one;
        break;
    case 4:
        sourceL = &one;
        sourceLr = &one;
        sourceML = &v1l;
        sourceMLr = &v1r;
        sourceMH = &v2l;
        sourceMHr = &v2r;
        sourceH = &one;
        sourceHr = &one;
        break;
    case 5:
        sourceL = &zero;
        sourceLr = &zero;
        sourceML = &v1l;
        sourceMLr = &v1r;
        sourceMH = &v1l;
        sourceMHr = &v1r;
        sourceH = &zero;
        sourceHr = &zero;
        break;
    case 6:
        sourceL = &zero;
        sourceLr = &zero;
        sourceML = &v1l;
        sourceMLr = &v1r;
        sourceMH = &v2l;
        sourceMHr = &v2r;
        sourceH = &zero;
        sourceHr = &zero;
        break;
    case 7:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &one;
        sourceMLr = &one;
        sourceMH = &one;
        sourceMHr = &one;
        sourceH = &v1l;
        sourceHr = &v1r;
        break;
    case 8:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &one;
        sourceMLr = &one;
        sourceMH = &one;
        sourceMHr = &one;
        sourceH = &v2l;
        sourceHr = &v2r;
        break;
    case 9:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &zero;
        sourceMLr = &zero;
        sourceMH = &zero;
        sourceMHr = &zero;
        sourceH = &v1l;
        sourceHr = &v1r;
        break;
    case 10:
        sourceL = &v1l;
        sourceLr = &v1r;
        sourceML = &zero;
        sourceMLr = &zero;
        sourceMH = &zero;
        sourceMHr = &zero;
        sourceH = &v2l;
        sourceHr = &v2r;
        break;
    }
#endif // 0
}

// rakarrack only, not lv2
void
VaryBand::calcCombi()
{
    /* To save all four variables in the one Pcombi variable */
    Pcombi = ((PsL * 1000) + (PsML * 100) + (PsMH * 10) + PsH) + 10000;
}

// rakarrack only, not lv2
void
VaryBand::parseCombi(int value)
{
    /* New file saving for the rkr.lv2 method - four variables saved in one variable location */
    value -= 10000;
    PsL = value / 1000;
    setSource(&sourceL, &sourceLr, PsL);

    value %= 1000;
    PsML = value / 100;
    setSource(&sourceML, &sourceMLr, PsML);

    value %= 100;
    PsMH = value / 10;
    setSource(&sourceMH, &sourceMHr, PsMH);

    value %= 10;
    PsH = value;
    setSource(&sourceH, &sourceHr, PsH);
}

void
VaryBand::setCross1(int value)
{
    Cross1 = value;
    lpf1l->setfreq((float) value);
    lpf1r->setfreq((float) value);
    hpf1l->setfreq((float) value);
    hpf1r->setfreq((float) value);
}

void
VaryBand::setCross2(int value)
{
    Cross2 = value;
    hpf2l->setfreq((float) value);
    hpf2r->setfreq((float) value);
    lpf2l->setfreq((float) value);
    lpf2r->setfreq((float) value);
}

void
VaryBand::setCross3(int value)
{
    Cross3 = value;
    hpf3l->setfreq((float) value);
    hpf3r->setfreq((float) value);
    lpf3l->setfreq((float) value);
    lpf3r->setfreq((float) value);
}

void
VaryBand::setSource(float** ptr, float** ptrr, int val)
{
    switch (val)
    {
    case 0:
        *ptr = &v1l;
        *ptrr = &v1r;
        break;
    case 1:
        *ptr = &v2l;
        *ptrr = &v2r;
        break;
    case 2:
        *ptr = &one;
        *ptrr = &one;
        break;
    case 3:
        *ptr = &zero;
        *ptrr = &zero;
        break;
    default:
        return; //no change
    }

    calcCombi(); // to update the Pcombi file saving value rakarrack only, not lv2
}

void
VaryBand::setpreset(int npreset)
{
    const int PRESET_SIZE = C_VARYBAND_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Vary1
        {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0}, // 0, 0, 1, 1
        //Vary2
        {0, 80, 0, 64, 40, 0, 0, 120, 1000, 2300, 1}, // 0, 1, 1, 0
        //Vary3
        {0, 120, 0, 64, 40, 0, 0, 800, 2300, 5200, 2}, // 0, 1, 0, 1
        //Vary4
        {0, 64, 0, 62, 120, 0, 62, 150, 1200, 12000, 11} // 0, 0, 0, 0
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_VARYBAND, npreset - NUM_PRESETS + 1, pdata);
        
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
VaryBand::changepar(int npar, int value)
{
    switch (npar)
    {
    case VaryBand_DryWet:
        setvolume(value);
        break;
    case VaryBand_LFO_Tempo_1:
        lfo1->Pfreq = value;
        lfo1->updateparams(PERIOD);
        break;
    case VaryBand_LFO_Type_1:
        lfo1->PLFOtype = value;
        lfo1->updateparams(PERIOD);
        break;
    case VaryBand_LFO_Stereo_1:
        lfo1->Pstereo = value;
        lfo1->updateparams(PERIOD);
        break;
    case VaryBand_LFO_Tempo_2:
        lfo2->Pfreq = value;
        lfo2->updateparams(PERIOD);
        break;
    case VaryBand_LFO_Type_2:
        lfo2->PLFOtype = value;
        lfo2->updateparams(PERIOD);
        break;
    case VaryBand_LFO_Stereo_2:
        lfo2->Pstereo = value;
        lfo2->updateparams(PERIOD);
        break;
    case VaryBand_Cross_1:
        setCross1(value);
        break;
    case VaryBand_Cross_2:
        setCross2(value);
        break;
    case VaryBand_Cross_3:
        setCross3(value);
        break;
    case VaryBand_Combination:  // Rakarrack-plus file saving & presets
        Pcombi = value;
        setCombi(value);
        break;
    case VaryBand_Low_Band:
        PsL = value;
        setSource(&sourceL, &sourceLr, value);
        break;
    case VaryBand_Mid_Band_1:
        PsML = value;
        setSource(&sourceML, &sourceMLr, value);
        break;
    case VaryBand_Mid_Band_2:
        PsMH = value;
        setSource(&sourceMH, &sourceMHr, value);
        break;
    case VaryBand_High_Band:
        PsH = value;
        setSource(&sourceH, &sourceHr, value);
        break;
    default:
        return;
    }
}

int
VaryBand::getpar(int npar)
{
    switch (npar)
    {
    case VaryBand_DryWet:
        return (Pvolume);

    case VaryBand_LFO_Tempo_1:
        return (lfo1->Pfreq);

    case VaryBand_LFO_Type_1:
        return (lfo1->PLFOtype);

    case VaryBand_LFO_Stereo_1:
        return (lfo1->Pstereo);

    case VaryBand_LFO_Tempo_2:
        return (lfo2->Pfreq);

    case VaryBand_LFO_Type_2:
        return (lfo2->PLFOtype);

    case VaryBand_LFO_Stereo_2:
        return (lfo2->Pstereo);

    case VaryBand_Cross_1:
        return (Cross1);

    case VaryBand_Cross_2:
        return (Cross2);

    case VaryBand_Cross_3:
        return (Cross3);

    case VaryBand_Combination:  // Rakarrack-plus file saving & presets
        return (Pcombi);

    case VaryBand_Low_Band:
        return (PsL);

    case VaryBand_Mid_Band_1:
        return (PsML);

    case VaryBand_Mid_Band_2:
        return (PsMH);

    case VaryBand_High_Band:
        return (PsH);

    }
    return (0); //in case of bogus parameter number
}

