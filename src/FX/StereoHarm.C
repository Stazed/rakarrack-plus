/*
  rakarrack - a guitar effects software

  StereoHarm.C  -  Stereo Harmonizer
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


#include "StereoHarm.h"

StereoHarm::StereoHarm(long int Quality, int DS, int uq, int dq,
                       double sample_rate, uint32_t intermediate_bufsize) :
    Effect(Quality, DS, uq, dq, sample_rate, intermediate_bufsize),
    STE_DOWN(DS),
    PERIOD(intermediate_bufsize),
    SAMPLE_RATE((unsigned int) sample_rate),
    Pintervall(),
    Pintervalr(),
    PMIDI(),
    PSELECT(),
    mira(),
    DS_state(STE_DOWN),
    window(),
    hq(Quality),
    u_up(),
    u_down(),
    outil(NULL),
    outir(NULL),
    outol(NULL),
    outor(NULL),
    templ(NULL),
    tempr(NULL),
    r_ratiol(),
    r_ratior(),
    nPERIOD(),
    nRATIO(),
    nSAMPLE_RATE(),
    nfSAMPLE_RATE(),
    Pvolume(),
    Plrcross(),
    Pgainl(),
    Pgainr(),
    Pchromel(),
    Pchromer(),
    Pnote(),
    Ptype(),
    gainl(0.5f),
    gainr(0.5f),
    intervall(),
    intervalr(),
    chromel(),
    chromer(),
    lrcross(0.5f),
    U_Resample(NULL),
    D_Resample(NULL),
    PSl(NULL),
    PSr(NULL)
{
    adjust(STE_DOWN, PERIOD);
    initialize();

    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq);

    PSl = new PitchShifter(window, hq, nfSAMPLE_RATE);
    PSl->ratio = 1.0f;
    PSr = new PitchShifter(window, hq, nfSAMPLE_RATE);
    PSr->ratio = 1.0f;

    StereoHarm::setpreset(Ppreset);

    StereoHarm::cleanup();
}

StereoHarm::~StereoHarm()
{
    clear_initialize();
    delete U_Resample;
    delete D_Resample;
    delete PSl;
    delete PSr;
}

void
StereoHarm::cleanup()
{
    mira = 0;
    chromel = 0;
    chromer = 0;
    memset(outil, 0, sizeof (float)*nPERIOD);
    memset(outir, 0, sizeof (float)*nPERIOD);
    memset(outol, 0, sizeof (float)*nPERIOD);
    memset(outor, 0, sizeof (float)*nPERIOD);
    memset(templ, 0, sizeof (float)*PERIOD);
    memset(tempr, 0, sizeof (float)*PERIOD);
}

std::vector<int>
StereoHarm::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_SHARM_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
StereoHarm::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_SHARM_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
StereoHarm::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    adjust(STE_DOWN, PERIOD);
    clear_initialize();
    initialize();
}
#endif // LV2

void
StereoHarm::set_random_parameters()
{
    for(int i = 0; i < C_SHARM_PARAMETERS; i++)
    {
        switch(i)
        {
            case Sharm_L_Chroma:
            case Sharm_R_Chroma:
            {
                int value = (int) (RND * 4001);
                changepar (i, value - 2000);
            }
            break;

            case Sharm_Select:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Sharm_Note:
            {
                int value = (int) (RND * 24);
                changepar (i, value);
            }
            break;

            case Sharm_DryWet:
            case Sharm_LR_Cross:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Sharm_L_Gain:
            case Sharm_R_Gain:
            {
                int value = (int) (RND * 129);
                changepar (i, value);
            }
            break;

            case Sharm_L_Interval:
            case Sharm_R_Interval:
            {
                int value = (int) (RND * 25);
                changepar (i, value);
            }
            break;

            case Sharm_Chord:
            {
                int value = (int) (RND * 34);
                changepar (i, value);
            }
            break;

            case Sharm_MIDI:
                break;
        }
    }
}

void
StereoHarm::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SHARM_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Sharm_L_Chroma:
            case Sharm_R_Chroma:
            case Sharm_Select:
            case Sharm_Note:
            case Sharm_LR_Cross:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), sharm_parameters[i * 3 + 1], sharm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Sharm_LR_Cross)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Sharm_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Sharm_DryWet )), sharm_parameters[i * 3 + 1], sharm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Sharm_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset 64
            case Sharm_L_Gain:
            case Sharm_R_Gain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, sharm_parameters[i * 3 + 1], sharm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ) - 64);
                    s_buf += ":";
                }
            }
            break;

            // Offset 12
            case Sharm_L_Interval:
            case Sharm_R_Interval:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 12, sharm_parameters[i * 3 + 1], sharm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ) - 12);
                    s_buf += ":";
                }
            }
            break;

            case Sharm_Chord:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Sharm_Chord ), sharm_parameters[i * 3 + 1], sharm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Sharm_Chord ));
                    s_buf += ":";
                }
            }
            break;
            
            case Sharm_MIDI:
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Sharm_MIDI ), sharm_parameters[i * 3 + 1], sharm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Sharm_MIDI ));
                    s_buf += ":";
                }
            break;
        }
    }
}

std::string
StereoHarm::get_URI(int type)
{
    if(type == NON_MIXER_XT)
        return STEROHARMLV2_URI;

    return STEROHARMLV2_URI;
};

std::string
StereoHarm::get_name(int type)
{
    if(type == NON_MIXER_XT)
        return SHARM_NAME;

    return SHARM_NAME;
};

void
StereoHarm::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);

    outil = (float *) malloc(sizeof (float) * nPERIOD);
    outir = (float *) malloc(sizeof (float) * nPERIOD);

    outol = (float *) malloc(sizeof (float) * nPERIOD);
    outor = (float *) malloc(sizeof (float) * nPERIOD);

    memset(outil, 0, sizeof (float) * nPERIOD);
    memset(outir, 0, sizeof (float) * nPERIOD);

    memset(outol, 0, sizeof (float) * nPERIOD);
    memset(outor, 0, sizeof (float) * nPERIOD);

    memset(templ, 0, sizeof (float) * PERIOD);
    memset(tempr, 0, sizeof (float) * PERIOD);
}

void
StereoHarm::clear_initialize()
{
    free(templ);
    free(tempr);
    free(outil);
    free(outir);
    free(outol);
    free(outor);
}

void
StereoHarm::out(float *efxoutl, float *efxoutr)
{
    bool have_nans = false;

    if (DS_state != 0)
    {
        U_Resample->out(efxoutl, efxoutr, templ, tempr, PERIOD, u_up);
    }
    else
    {
        memcpy(templ, efxoutl, sizeof (float)*PERIOD);
        memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
    }

    for (unsigned int i = 0; i < nPERIOD; i++)
    {
        outil[i] = templ[i];
        
        if (outil[i] > 1.0)
            outil[i] = 1.0f;
        
        if (outil[i] < -1.0)
            outil[i] = -1.0f;

        outir[i] = tempr[i];
        
        if (outir[i] > 1.0)
            outir[i] = 1.0f;
        if (outir[i] < -1.0)
            outir[i] = -1.0f;
    }

    if ((PMIDI) || (PSELECT))
    {
        PSl->ratio = r_ratiol;
        PSr->ratio = r_ratior;
    }

    if (PSl->ratio != 1.0f)
    {
        PSl->smbPitchShift(PSl->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outil, outol);
    }
    else
        memcpy(outol, outil, sizeof (float)*nPERIOD);

    if (PSr->ratio != 1.0f)
    {
        PSr->smbPitchShift(PSr->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outir, outor);
    }
    else
        memcpy(outor, outir, sizeof (float)*nPERIOD);


    if (DS_state != 0)
    {
        D_Resample->out(outol, outor, templ, tempr, nPERIOD, u_down);
    }
    else
    {
        memcpy(templ, outol, sizeof (float)*PERIOD);
        memcpy(tempr, outor, sizeof (float)*PERIOD);
    }

    //for (i = 0; i < PERIOD; i++) {
    //efxoutl[i] = templ[i] * gainl;
    //efxoutr[i] = tempr[i] * gainr;

    //}
    for (unsigned int i = 0; i < PERIOD; i++)
    {
        efxoutl[i] = templ[i] * gainl * (1.0f - lrcross) + tempr[i] * gainr * lrcross;
        efxoutr[i] = tempr[i] * gainr * (1.0f - lrcross) + templ[i] * gainl * lrcross;
        
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
StereoHarm::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
StereoHarm::setgain(int chan, int value)
{
    switch (chan)
    {
    case 0:
        Pgainl = value;
        gainl = (float) Pgainl / 127.0f;
        gainl *= 2.0;
        break;
    case 1:
        Pgainr = value;
        gainr = (float) Pgainr / 127.0f;
        gainr *= 2.0;
        break;
    }
}

void
StereoHarm::setinterval(int chan, int value)
{
    switch (chan)
    {
    case 0:
        Pintervall = value;
        intervall = (float) Pintervall - 12.0f;
        PSl->ratio = powf(2.0f, intervall / 12.0f) + chromel;
        if (Pintervall % 12 == 0)
            mira = 0;
        else
            mira = 1;
        break;

    case 1:
        Pintervalr = value;
        intervalr = (float) Pintervalr - 12.0f;
        PSr->ratio = powf(2.0f, intervalr / 12.0f) + chromer;
        if (Pintervalr % 12 == 0)
            mira = 0;
        else
            mira = 1;
        break;
    }
}

void
StereoHarm::setchrome(int chan, int value)
{
    float max, min;
    max = 0.0;
    min = 0.0;

    switch (chan)
    {
    case 0:
        max = powf(2.0f, (intervall + 1.0f) / 12.0f);
        min = powf(2.0f, (intervall - 1.0f) / 12.0f);
        break;
    case 1:
        max = powf(2.0f, (intervalr + 1.0f) / 12.0f);
        min = powf(2.0f, (intervalr - 1.0f) / 12.0f);
        break;
    }

    if (max > 2.0) max = 2.0f;
    if (min < 0.5) min = 0.5f;

    switch (chan)
    {
    case 0:
        Pchromel = value;
        chromel = (max - min) / 4000.0f * (float) value;
        PSl->ratio = powf(2.0f, intervall / 12.0f) + chromel;
        break;
    case 1:
        Pchromer = value;
        chromer = (max - min) / 4000.0f * (float) value;
        PSr->ratio = powf(2.0f, intervalr / 12.0f) + chromer;
        break;
    }
}

void
StereoHarm::setMIDI(int value)
{
    PMIDI = value;
}

void
StereoHarm::adjust(int DS, uint32_t period)
{
    DS_state = DS;
    float fSAMPLE_RATE = SAMPLE_RATE;

    switch (DS)
    {
    case 0:
        //nPERIOD = period;
        nRATIO = 1;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        window = 2048;
        break;

    case 1:
        //nPERIOD = lrintf(fPERIOD*96000.0f/fSAMPLE_RATE);
        nRATIO = 96000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        window = 2048;
        break;

    case 2:
        //nPERIOD = lrintf(fPERIOD*48000.0f/fSAMPLE_RATE);
        nRATIO = 48000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        window = 2048;
        break;

    case 3:
        //nPERIOD = lrintf(fPERIOD*44100.0f/fSAMPLE_RATE);
        nRATIO = 44100.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        window = 2048;
        break;

    case 4:
        //nPERIOD = lrintf(fPERIOD*32000.0f/fSAMPLE_RATE);
        nRATIO = 32000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        window = 2048;
        break;

    case 5:
        //nPERIOD = lrintf(fPERIOD*22050.0f/fSAMPLE_RATE);
        nRATIO = 22050.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        window = 1024;
        break;

    case 6:
        //nPERIOD = lrintf(fPERIOD*16000.0f/fSAMPLE_RATE);
        nRATIO = 16000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        window = 1024;
        break;

    case 7:
        //nPERIOD = lrintf(fPERIOD*12000.0f/fSAMPLE_RATE);
        nRATIO = 12000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        window = 512;
        break;

    case 8:
        //nPERIOD = lrintf(fPERIOD*8000.0f/fSAMPLE_RATE);
        nRATIO = 8000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        window = 512;
        break;

    case 9:
        //nPERIOD = lrintf(fPERIOD*4000.0f/fSAMPLE_RATE);
        nRATIO = 4000.0f / fSAMPLE_RATE;
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        window = 256;
        break;
    }

    nPERIOD = lrintf(PERIOD * nRATIO);
    u_up = (double) nPERIOD / (double) period;
    u_down = (double) period / (double) nPERIOD;
}

void
StereoHarm::setlrcross(int value)
{
    Plrcross = value;
    lrcross = (float) Plrcross / 127.0f;
}

void
StereoHarm::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SHARM_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, 12, 0, 64, 12, 0, 0, 0, 0, 0, 64},
        //Octavador
        {64, 64, 0, 0, 64, 0, 0, 0, 0, 0, 0, 64},
        //Chorus
        {64, 64, 12, 80, 64, 12, -80, 0, 0, 0, 0, 64},
        //Chorus
        {64, 64, 12, 280, 64, 12, -280, 0, 0, 0, 0, 64}
    };

    cleanup();
    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_STEREOHARM, npreset - NUM_PRESETS + 1, pdata);
        
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
StereoHarm::changepar(int npar, int value)
{
    switch (npar)
    {
    case Sharm_DryWet:
        setvolume(value);
        break;
    case Sharm_L_Gain:
        setgain(0, value);
        break;
    case Sharm_L_Interval:
        setinterval(0, value);
        break;
    case Sharm_L_Chroma:
        setchrome(0, value);
        break;
    case Sharm_R_Gain:
        setgain(1, value);
        break;
    case Sharm_R_Interval:
        setinterval(1, value);
        break;
    case Sharm_R_Chroma:
        setchrome(1, value);
        break;
    case Sharm_Select:
        PSELECT = value;
        break;
    case Sharm_Note:
        Pnote = value;
        break;
    case Sharm_Chord:
        Ptype = value;
        if (Ptype == 0)
        {
            setchrome(0, Pchromel);
            setchrome(1, Pchromer);
        }
        break;
    case Sharm_MIDI:
        setMIDI(value);
        break;
    case Sharm_LR_Cross:
        setlrcross(value);
        break;
    default:
        return;
    }
}

int
StereoHarm::getpar(int npar)
{
    switch (npar)
    {
    case Sharm_DryWet:
        return (Pvolume);

    case Sharm_L_Gain:
        return (Pgainl);

    case Sharm_L_Interval:
        return (Pintervall);

    case Sharm_L_Chroma:
        return (Pchromel);

    case Sharm_R_Gain:
        return (Pgainr);

    case Sharm_R_Interval:
        return (Pintervalr);

    case Sharm_R_Chroma:
        return (Pchromer);

    case Sharm_Select:
        return (PSELECT);

    case Sharm_Note:
        return (Pnote);

    case Sharm_Chord:
        return (Ptype);

    case Sharm_MIDI:
        return (PMIDI);

    case Sharm_LR_Cross:
        return (Plrcross);

    }

    return (0);
}
