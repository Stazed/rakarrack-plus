/*
  rakarrack - a guitar effects software

  Harmonizer.C  -  Harmonizer
  Copyright (C) 2008 Josep Andreu
  Author:  Josep Andreu

  Using Stephan M. Bernsee smbPitchShifter engine.

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


#include "Harmonizer.h"

Harmonizer::Harmonizer(long int Quality, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsize)
{
    PERIOD = intermediate_bufsize; // correct for rakarrack, may be adjusted by lv2
    fSAMPLE_RATE = sample_rate;

    hq = Quality;
    SAMPLE_RATE = (unsigned int) sample_rate;
    adjust(DS, PERIOD);
    DS_init = 0;

    initialize();

    U_Resample = new Resample(dq);
    D_Resample = new Resample(uq);

    PS = new PitchShifter(window, hq, nfSAMPLE_RATE);
    PS->ratio = 1.0f;

    Ppreset = 0;
    PMIDI = 0;
    mira = 0;
    r_ratio = 0.0f;
    outvolume = 0.5f;

    setpreset(Ppreset);

    cleanup();
}

Harmonizer::~Harmonizer()
{
    clear_initialize();
    delete U_Resample;
    delete D_Resample;
    delete PS;
}

void
Harmonizer::cleanup()
{
    mira = 0;
    memset(outi, 0, sizeof (float)*nPERIOD);
    memset(outo, 0, sizeof (float)*nPERIOD);
}

#ifdef LV2_SUPPORT
void
Harmonizer::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        adjust(DS_state, PERIOD); //readjust now that we know period size
        clear_initialize();
        initialize();
        fsetfreq(fPfreq);
        fsetgain(fPgain);
        fsetq(fPq);
    }
    else
    {
        PERIOD = period;
        adjust(DS_state, PERIOD); //readjust now that we know period size
    }
}
#endif // LV2

void Harmonizer::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);


    outi = (float *) malloc(sizeof (float) * PERIOD);
    outo = (float *) malloc(sizeof (float) * PERIOD);

    unsigned int i;
    
    for (i = 0; i < PERIOD; i++)
    {
        templ[i] = tempr[i] = 0;
        outi[i] = outo[i] = 0;
    }

    interpbuf = new float[PERIOD];
    pl = new AnalogFilter(6, 22000, 1, 0, fSAMPLE_RATE, interpbuf);
}

void Harmonizer::clear_initialize()
{
    free(templ);
    free(tempr);
    free(outi);
    free(outo);
    delete pl;
    delete[] interpbuf;
}

void
Harmonizer::applyfilters(float * efxoutl, uint32_t period)
{
    pl->filterout(efxoutl, period);
}

void
Harmonizer::out(float *efxoutl, float *efxoutr)
{
    int i = 0;

    if ((DS_state != 0) && (Pinterval != 12))
    {
        memcpy(templ, efxoutl, sizeof (float)*PERIOD);
        memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
        U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
    }

    for (i = 0; i < nPERIOD; i++)
    {
        outi[i] = (efxoutl[i] + efxoutr[i])*.5;
        
        if (outi[i] > 1.0)
            outi[i] = 1.0f;
        
        if (outi[i] < -1.0)
            outi[i] = -1.0f;
    }

    if ((PMIDI) || (PSELECT))
        PS->ratio = r_ratio; // rakarrack value passed from r__ratio[0] in process

    if (Pinterval != 12)
    {
        PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

        if ((DS_state != 0) && (Pinterval != 12))
        {
            D_Resample->mono_out(outo, templ, nPERIOD, u_down, PERIOD);
        }
        else
        {
            memcpy(templ, outo, sizeof (float)*PERIOD);
        }

        applyfilters(templ, PERIOD);

        for (i = 0; i < (signed int) PERIOD; i++)
        {
            efxoutl[i] = templ[i] * gain * (1.0f - panning);
            efxoutr[i] = templ[i] * gain * panning;
        }
    }

#if 0 // rkrlv2
    if ((DS_state != 0) && (Pinterval != 12))
    {
        U_Resample->out(smpsl, smpsr, templ, tempr, PERIOD, u_up);
    }


    for (i = 0; i < nPERIOD; i++)
    {
        outi[i] = (templ[i] + tempr[i])*.5;
        if (outi[i] > 1.0)
            outi[i] = 1.0f;
        if (outi[i] < -1.0)
            outi[i] = -1.0f;
    }

    if ((PMIDI) || (PSELECT))
        PS->ratio = r_ratio;

    if (Pinterval != 12)
    {
        PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);
    }

    if ((DS_state != 0) && (Pinterval != 12))
    {
        D_Resample->mono_out(outo, templ, nPERIOD, u_down, PERIOD);
    }
    else
    {
        memcpy(templ, smpsl, sizeof (float)*PERIOD);
    }

    applyfilters(templ, PERIOD);

    //for (i = 0; i < (signed int)period; i++) {
    //efxoutl[i] = templ[i] * gain * panning;
    //efxoutr[i] = templ[i] * gain * (1.0f - panning);
    //}
    for (i = 0; i < (signed int) PERIOD; i++)
    {
        efxoutl[i] = templ[i] * gain * (1.0f - panning);
        efxoutr[i] = templ[i] * gain * panning;
    }
#endif // 0

}

void
Harmonizer::setvolume(int value)
{
    this->Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
Harmonizer::setpanning(int value)
{
    this->Ppan = value;
    panning = (float) Ppan / 127.0f;
}

void
Harmonizer::setgain(int value)
{
    this->Pgain = value;
    gain = (float) Pgain / 127.0f;
    gain *= 2.0;
}

void
Harmonizer::setinterval(int value)
{
    this->Pinterval = value;
    interval = (float) Pinterval - 12.0f;
    PS->ratio = powf(2.0f, interval / 12.0f);
    
    if (Pinterval % 12 == 0)
        mira = 0;
    else
        mira = 1;
}

void
Harmonizer::fsetfreq(int value)
{
    fPfreq = value;
    float tmp = (float) value;
    pl->setfreq(tmp);
}

void
Harmonizer::fsetgain(int value)
{
    this->fPgain = value;
    float tmp = 30.0f * ((float) value - 64.0f) / 64.0f;
    pl->setgain(tmp);
}

void
Harmonizer::fsetq(int value)
{
    this->fPq = value;
    float tmp = powf(30.0f, ((float) value - 64.0f) / 64.0f);
    pl->setq(tmp);
}

void
Harmonizer::setMIDI(int value)
{
    this->PMIDI = value;
}

void
Harmonizer::adjust(int DS, uint32_t period)
{
    DS_state = DS;
    DS_init = 1;
    float fSAMPLE_RATE = SAMPLE_RATE;
    float fPERIOD = period;

    switch (DS)
    {
    case 0:
        nPERIOD = period;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        window = 2048;
        break;

    case 1:
        nPERIOD = lrintf(fPERIOD * 96000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        window = 2048;
        break;


    case 2:
        nPERIOD = lrintf(fPERIOD * 48000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        window = 2048;
        break;

    case 3:
        nPERIOD = lrintf(fPERIOD * 44100.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        window = 2048;
        break;

    case 4:
        nPERIOD = lrintf(fPERIOD * 32000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        window = 2048;
        break;

    case 5:
        nPERIOD = lrintf(fPERIOD * 22050.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        window = 1024;
        break;

    case 6:
        nPERIOD = lrintf(fPERIOD * 16000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        window = 1024;
        break;

    case 7:
        nPERIOD = lrintf(fPERIOD * 12000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        window = 512;
        break;

    case 8:
        nPERIOD = lrintf(fPERIOD * 8000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        window = 512;
        break;

    case 9:
        nPERIOD = lrintf(fPERIOD * 4000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        window = 256;
        break;
    }
    u_up = (double) nPERIOD / (double) period;
    u_down = (double) period / (double) nPERIOD;
}

void
Harmonizer::setpreset(int npreset)
{
    const int PRESET_SIZE = 11;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, 64, 12, 6000, 0, 0, 0, 64, 64, 0},
        //Octavador
        {64, 64, 64, 0, 6000, 0, 0, 0, 64, 64, 0},
        //3mdown
        {64, 64, 64, 9, 6000, 0, 0, 0, 64, 64, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(14, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = npreset;
}

void
Harmonizer::changepar(int npar, int value)
{
    switch (npar)
    {
    case 0:
        setvolume(value);
        break;
    case 1:
        setpanning(value);
        break;
    case 2:
        setgain(value);
        break;
    case 3:
        setinterval(value);
        break;
    case 4:
        fsetfreq(value);
        break;
    case 5:
        PSELECT = value;
        if (!value)
            setinterval(Pinterval);
        break;
    case 6:
        Pnote = value;
        break;
    case 7:
        Ptype = value;
        break;
    case 8:
        fsetgain(value);
        break;
    case 9:
        fsetq(value);
        break;
    case 10:
        setMIDI(value);
        if (!value)
            setinterval(Pinterval);
        break;
    }
}

int
Harmonizer::getpar(int npar)
{
    switch (npar)
    {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Ppan);
        break;
    case 2:
        return (Pgain);
        break;
    case 3:
        return (Pinterval);
        break;
    case 4:
        return (fPfreq);
        break;
    case 5:
        return (PSELECT);
        break;
    case 6:
        return (Pnote);
        break;
    case 7:
        return (Ptype);
        break;
    case 8:
        return (fPgain);
        break;
    case 9:
        return (fPq);
        break;
    case 10:
        return (PMIDI);
        break;
    default:
        return (0);
    }
}
