/*
  rakarrack - a guitar effects software

  Shifter.C  -  Shifter
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

  Using Stephan M. Bernsee smbPtichShifter engine.

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


#include "Shifter.h"

Shifter::Shifter(long int Quality, int DS, int uq, int dq,
                 double sample_rate, uint32_t intermediate_bufsize) :
    Effect(Quality, DS, uq, dq, sample_rate, intermediate_bufsize),
    hq(Quality),
    outi(NULL),
    outo(NULL),
    DS_state(DS),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(),
    Pgain(),
    Ppan(),
    Pinterval(),
    Pupdown(),
    Pmode(),
    Pattack(),
    Pdecay(),
    Pthreshold(),
    Pwhammy(),
    state(IDLE),
    nPERIOD(),
    nSAMPLE_RATE(),
    nRATIO(),
    window(),
    u_up(),
    u_down(),
    nfSAMPLE_RATE(),
    env(),
    t_level(),
    td_level(),
    tz_level(),
    a_rate(),
    d_rate(),
    tune(),
    range(),
    whammy(),
    panning(),
    gain(),
    interval(),
    templ(NULL),
    tempr(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    PS(NULL)
{
    adjust(DS, sample_rate);
    initialize();

    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq);

    PS = new PitchShifter(window, hq, nfSAMPLE_RATE);
    PS->ratio = 1.0f;

    setpreset(Ppreset);
    cleanup();
}

Shifter::~Shifter()
{
    clear_initialize();
    delete U_Resample;
    delete D_Resample;
    delete PS;
}

void
Shifter::cleanup()
{
    state = IDLE;
    memset(outi, 0, sizeof (float)*nPERIOD);
    memset(outo, 0, sizeof (float)*nPERIOD);

    memset(templ, 0, sizeof (float)*PERIOD);
    memset(tempr, 0, sizeof (float)*PERIOD);
}

std::vector<int>
Shifter::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_SHIFTER_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Shifter::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_SHIFTER_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#ifdef LV2_SUPPORT
void
Shifter::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        adjust(DS_state, fSAMPLE_RATE);
        clear_initialize();
        initialize();
    }
    else
    {
        PERIOD = period;
        adjust(DS_state, fSAMPLE_RATE);
    }
}
#endif // LV2

void
Shifter::set_random_parameters()
{
    for(int i = 0; i < C_SHIFTER_PARAMETERS; i++)
    {
        switch(i)
        {
            case Shifter_Attack:
            case Shifter_Decay:
            {
                int value = (int) (RND * 2000);
                changepar (i, value + 1);
            }
            break;

            case Shifter_Threshold:
            {
                int value = (int) (RND * 90);
                changepar (i, value - 70);
            }
            break;

            case Shifter_Interval:
            {
                int value = (int) (RND * 13);
                changepar (i, value);
            }
            break;

            case Shifter_Shift:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Shifter_Mode:
            {
                int value = (int) (RND * 3);
                changepar (i, value);
            }
            break;

            case Shifter_DryWet:
            case Shifter_Whammy:
            case Shifter_Pan:
            case Shifter_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Shifter::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SHIFTER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Shifter_Attack:
            case Shifter_Decay:
            case Shifter_Threshold:
            case Shifter_Interval:
            case Shifter_Shift:
            case Shifter_Mode:
            case Shifter_Whammy:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), shift_parameters[i * 3 + 1], shift_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Shifter_Whammy )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Shifter_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Shifter_DryWet )), shift_parameters[i * 3 + 1], shift_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Shifter_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Shifter_Pan:
            case Shifter_Gain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ) - 64, shift_parameters[i * 3 + 1], shift_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ) - 64);
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Shifter::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);

    outi = (float *) malloc(sizeof (float) * nPERIOD);
    outo = (float *) malloc(sizeof (float) * nPERIOD);
}

void
Shifter::clear_initialize()
{
    free(templ);
    free(tempr);
    free(outi);
    free(outo);
}

void
Shifter::adjust(int DS, double SAMPLE_RATE)
{
    DS_state = DS;

    switch (DS)
    {
    case 0:
        nRATIO = 1;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = SAMPLE_RATE;
        window = 2048;
        break;

    case 1:
        nRATIO = 96000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        window = 2048;
        break;


    case 2:
        nRATIO = 48000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        window = 2048;
        break;

    case 3:
        nRATIO = 44100.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        window = 2048;
        break;

    case 4:
        nRATIO = 32000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        window = 2048;
        break;

    case 5:
        nRATIO = 22050.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        window = 1024;
        break;

    case 6:
        nRATIO = 16000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        window = 1024;
        break;

    case 7:
        nRATIO = 12000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        window = 512;
        break;

    case 8:
        nRATIO = 8000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        window = 512;
        break;

    case 9:
        nRATIO = 4000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        window = 256;
        break;
    }

    nPERIOD = lrintf((float) PERIOD * nRATIO);
    u_up = (double) nPERIOD / (double) PERIOD;
    u_down = (double) PERIOD / (double) nPERIOD;
}

void
Shifter::out(float *efxoutl, float *efxoutr)
{
    bool have_nans = false;
    if (DS_state != 0)
    {
        memcpy(templ, efxoutl, sizeof (float)*PERIOD);
        memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
        U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
    }

    for (int i = 0; i < nPERIOD; i++)
    {
        if ((Pmode == 0) || (Pmode == 2))
        {
            float sum = fabsf(efxoutl[i]) + fabsf(efxoutr[i]);
            
            if (sum > env)
            {
                env = sum;
            }
            else
            {
                env = sum * ENV_TR + env * (1.0f - ENV_TR);
            }

            if (env <= tz_level)
            {
                state = IDLE;
                tune = 0.0;
            }

            if ((state == IDLE) && (env >= t_level))
            {
                state = UP;
            }

            if (state == UP)
            {
                tune += a_rate;
                if (tune >= 1.0f)
                {
                    state = WAIT;
                }
            }

            if (state == WAIT)
            {
                tune = 1.0f;
                if (env < td_level)
                {
                    state = DOWN;
                }
            }

            if (state == DOWN)
            {
                tune -= d_rate;
                
                if (tune <= 0.0)
                {
                    tune = 0.0;
                    state = IDLE;
                }
            }
        }
        outi[i] = (efxoutl[i] + efxoutr[i])*.5;
        
        if (outi[i] > 1.0)
        {
            outi[i] = 1.0f;
        }

        if (outi[i] < -1.0)
        {
            outi[i] = -1.0f;
        }
        
        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();

    float use = 0.0f;
    if (Pmode == 1)
    {
        use = whammy;
    }
    else
    {
        use = tune;
    }
    
    if ((Pmode == 0) && (Pinterval == 0))
    {
        use = tune * whammy;
    }

    if (Pmode == 2)
    {
        use = 1.0f - tune;
    }

    if (Pupdown)
    {
        PS->ratio = 1.0f - (1.0f - range) * use;
    }
    else
    {
        PS->ratio = 1.0f + ((range - 1.0f) * use);
    }

    PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

    for (int i = 0; i < nPERIOD; i++)
    {
        templ[i] = outo[i] * gain * (1.0f - panning);
        tempr[i] = outo[i] * gain * panning;
    }

    if (DS_state != 0)
    {
        D_Resample->out(templ, tempr, efxoutl, efxoutr, nPERIOD, u_down);
    }
    else
    {
        memcpy(efxoutl, templ, sizeof (float)*PERIOD);
        memcpy(efxoutr, tempr, sizeof (float)*PERIOD);
    }
}

void
Shifter::setvolume(int value)
{
    this->Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
Shifter::setpanning(int value)
{
    this->Ppan = value;
    panning = (float) Ppan / 127.0f;
}

void
Shifter::setgain(int value)
{
    this->Pgain = value;
    gain = (float) Pgain / 127.0f;
    gain *= 2.0f;
}

void
Shifter::setinterval(int value)
{
    interval = (float) value;
    
    if ((Pmode == 0) && (Pinterval == 0))
    {
        interval = 1.0f;
    }

    if (Pupdown)
    {
        interval *= -1.0f;
    }
    
    range = powf(2.0f, interval / 12.0f);
}

void
Shifter::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SHIFTER_PARAMETERS;
    const int NUM_PRESETS = 6;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Fast
        {0, 64, 64, 200, 200, -20, 2, 0, 0, 0},
        //Slowup
        {0, 64, 64, 900, 200, -20, 2, 0, 0, 0},
        //Slowdown
        {0, 64, 64, 900, 200, -20, 3, 1, 0, 0},
        //Chorus
        {64, 64, 64, 1, 1, -20, 1, 0, 1, 22},
        //Trig Chorus
        {64, 64, 64, 250, 100, -10, 0, 0, 0, 25},
        //Shifter 6
        {0, 64, 64, 500, 500, 0, 1, 0, 0, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_SHIFTER, npreset - NUM_PRESETS + 1, pdata);
        
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
Shifter::changepar(int npar, int value)
{
    switch (npar)
    {
    case Shifter_DryWet:
        setvolume(value);
        break;
    case Shifter_Pan:
        setpanning(value);
        break;
    case Shifter_Gain:
        setgain(value);
        break;
    case Shifter_Attack:
        Pattack = value;
        a_rate = 1000.0f / ((float) Pattack * nfSAMPLE_RATE);
        break;
    case Shifter_Decay:
        Pdecay = value;
        d_rate = 1000.0f / ((float) Pdecay * nfSAMPLE_RATE);
        break;
    case Shifter_Threshold:
        Pthreshold = value;
        t_level = dB2rap((float) Pthreshold);
        td_level = t_level * .75f;
        tz_level = t_level * .5f;
        break;
    case Shifter_Interval:
        Pinterval = value;
        setinterval(Pinterval);
        break;
    case Shifter_Shift:
        Pupdown = value;
        setinterval(Pinterval);
        break;
    case Shifter_Mode:
        Pmode = value;
        break;
    case Shifter_Whammy:
        Pwhammy = value;
        whammy = (float) value / 127.0f;
        break;
    default:
        return;
    }
}

int
Shifter::getpar(int npar)
{
    switch (npar)
    {
    case Shifter_DryWet:
        return (Pvolume);

    case Shifter_Pan:
        return (Ppan);

    case Shifter_Gain:
        return (Pgain);

    case Shifter_Attack:
        return (Pattack);

    case Shifter_Decay:
        return (Pdecay);

    case Shifter_Threshold:
        return (Pthreshold);

    case Shifter_Interval:
        return (Pinterval);

    case Shifter_Shift:
        return (Pupdown);

    case Shifter_Mode:
        return (Pmode);

    case Shifter_Whammy:
        return (Pwhammy);

    }

    return (0);
}
