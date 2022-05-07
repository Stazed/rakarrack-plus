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

Harmonizer::Harmonizer(long int Quality, int DS, int uq, int dq,
                       double sample_rate, uint32_t intermediate_bufsize) :
    Effect(Quality, DS, uq, dq, sample_rate, intermediate_bufsize),
    Pinterval(),
    PMIDI(),
    PSELECT(),
    r_ratio(),
    mira(),
    DS_state(),
    nPERIOD(),
    nSAMPLE_RATE(),
    window(),
    hq(Quality),
    u_up(),
    u_down(),
    nfSAMPLE_RATE(),
    outi(NULL),
    outo(NULL),
    templ(NULL),
    tempr(NULL),
    Pvolume(),
    Pgain(),
    Ppan(),
    Pnote(),
    Ptype(),
    fPfreq(),
    fPgain(),
    fPq(),
    panning(),
    gain(),
    interval(),
    pl(NULL),
    interpbuf(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    PS(NULL),
    SAMPLE_RATE((unsigned int) sample_rate),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    DS_init()
{
    adjust(DS, PERIOD);
    initialize();

    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq);

    PS = new PitchShifter(window, hq, nfSAMPLE_RATE);
    PS->ratio = 1.0f;

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

std::vector<int>
Harmonizer::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_HARM_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Harmonizer::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_HARM_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
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

void
Harmonizer::set_random_parameters()
{
    for(int i = 0; i < C_HARM_PARAMETERS; i++)
    {
        switch(i)
        {
            case Harm_Filter_Freq:
            {
                int value = (int) (RND * 25980);
                changepar (i, value + 20);
            }
            break;

            case Harm_Select:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Harm_Note:
            {
                int value = (int) (RND * 24);
                changepar (i, value);
            }
            break;

            case Harm_Chord:
            {
                int value = (int) (RND * 34);
                changepar (i, value);
            }
            break;

            case Harm_DryWet:
            case Harm_Pan:
            case Harm_Gain:
            case Harm_Filter_Gain:
            case Harm_Filter_Q:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Harm_Interval:
            {
                int value = (int) (RND * 25);
                changepar (i, value);
            }
            break;

            case Harm_MIDI:
                break;
        }
    }
}

void
Harmonizer::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_HARM_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Harm_Filter_Freq:
            case Harm_Select:
            case Harm_Note:
            case Harm_Chord:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), harm_parameters[i * 3 + 1], harm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));
                    s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Harm_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Harm_DryWet )), harm_parameters[i * 3 + 1], harm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Harm_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset 64
            case Harm_Pan:
            case Harm_Gain:
            case Harm_Filter_Gain:
            case Harm_Filter_Q:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, harm_parameters[i * 3 + 1], harm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ) - 64);

                    if ( i !=  Harm_Filter_Q)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Offset 12
            case Harm_Interval:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 12, harm_parameters[i * 3 + 1], harm_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ) - 12);
                    s_buf += ":";
                }
            }
            break;

            case Harm_MIDI:
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), harm_parameters[i * 3 + 1], harm_parameters[i * 3 + 2]);
                }
                else
                {
                    // NON_MIXER does not have midi
                }
            break;
        }
    }
}

std::string
Harmonizer::get_URI(int type)
{
    if(type == NON_MIXER)
        return HARMNOMIDLV2_URI;
    
    return HARMLV2_URI;
};

std::string
Harmonizer::get_name(int type)
{
    if(type == NON_MIXER)
        return HARM_NAME_NO_MIDI;

    return HARM_NAME;
};

void Harmonizer::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);


    outi = (float *) malloc(sizeof (float) * PERIOD);
    outo = (float *) malloc(sizeof (float) * PERIOD);

    for (unsigned int i = 0; i < PERIOD; i++)
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
    bool have_nans = false;

    if (DS_state != 0)
    {
        /* copy to temp then replace efxoutl and efxoutr with up sample */
        memcpy(templ, efxoutl, sizeof (float)*PERIOD);
        memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
        U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
    }

    /* Here we mix down the stereo inputs to mono */
    for (int i = 0; i < nPERIOD; i++)
    {
        outi[i] = (efxoutl[i] + efxoutr[i])*.5;
        
        if (outi[i] > 1.0)
            outi[i] = 1.0f;
        
        if (outi[i] < -1.0)
            outi[i] = -1.0f;
    }

    if ((PMIDI) || (PSELECT))
    {
        PS->ratio = r_ratio; // rakarrack value passed from r__ratio[0] in process
    }

    /* This changes the pitch */
    PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

    if (DS_state != 0)
    {
        /* We mixed down the stereo to mono above so we only need to down sample mono here.
         * Using templ for the result out. The tempr is not needed. */
        D_Resample->mono_out(outo, templ, nPERIOD, u_down, PERIOD);
    }
    else
    {
        /* Down sample not used so copy pitch changed outo to the mono templ */
        memcpy(templ, outo, sizeof (float)*PERIOD);
    }

    applyfilters(templ, PERIOD);

    /* Sets templ mono to both channels */
    for (int i = 0; i < (signed int) PERIOD; i++)
    {
        efxoutl[i] = templ[i] * gain * (1.0f - panning);
        efxoutr[i] = templ[i] * gain * panning;

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
Harmonizer::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
Harmonizer::setpanning(int value)
{
    Ppan = value;
    panning = (float) Ppan / 127.0f;
}

void
Harmonizer::setgain(int value)
{
    Pgain = value;
    gain = (float) Pgain / 127.0f;
    gain *= 2.0;
}

void
Harmonizer::setinterval(int value)
{
    Pinterval = value;
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
    fPgain = value;
    float tmp = 30.0f * ((float) value - 64.0f) / 64.0f;
    pl->setgain(tmp);
}

void
Harmonizer::fsetq(int value)
{
    fPq = value;
    float tmp = powf(30.0f, ((float) value - 64.0f) / 64.0f);
    pl->setq(tmp);
}

void
Harmonizer::setMIDI(int value)
{
    PMIDI = value;
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
    const int PRESET_SIZE = C_HARM_PARAMETERS;
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
        Fpre->ReadPreset(EFX_HARMONIZER, npreset - NUM_PRESETS + 1, pdata);
        
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
    case Harm_DryWet:
        setvolume(value);
        break;
    case Harm_Pan:
        setpanning(value);
        break;
    case Harm_Gain:
        setgain(value);
        break;
    case Harm_Interval:
        setinterval(value);
        break;
    case Harm_Filter_Freq:
        fsetfreq(value);
        break;
    case Harm_Select:
        PSELECT = value;
        if (!value)
            setinterval(Pinterval);
        break;
    case Harm_Note:
        Pnote = value;
        break;
    case Harm_Chord:
        Ptype = value;
        break;
    case Harm_Filter_Gain:
        fsetgain(value);
        break;
    case Harm_Filter_Q:
        fsetq(value);
        break;
    case Harm_MIDI:
        setMIDI(value);
        if (!value)
            setinterval(Pinterval);
        break;
    default:
        return;
    }
}

int
Harmonizer::getpar(int npar)
{
    switch (npar)
    {
    case Harm_DryWet:
        return (Pvolume);

    case Harm_Pan:
        return (Ppan);

    case Harm_Gain:
        return (Pgain);

    case Harm_Interval:
        return (Pinterval);

    case Harm_Filter_Freq:
        return (fPfreq);

    case Harm_Select:
        return (PSELECT);

    case Harm_Note:
        return (Pnote);

    case Harm_Chord:
        return (Ptype);

    case Harm_Filter_Gain:
        return (fPgain);

    case Harm_Filter_Q:
        return (fPq);

    case Harm_MIDI:
        return (PMIDI);

    }

    return (0);
}
