/*
  ZynAddSubFX - a software synthesizer

  MuTroMojo.C - "WahWah" effect and others
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Ryan Billing

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

// Renamed from RyanWah.C 4/18/2020 by stazed

#include <math.h>
#include "MuTroMojo.h"        // Mutromojo
#include "../EFX_common/AnalogFilter.h"
#include <stdio.h>

MuTroMojo::MuTroMojo(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    Pmode(),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    Pvolume(),
    Pwidth(),
    Pampsns(),
    Pampsnsinv(),
    Pampsmooth(),
    Prange(),
    Php(),
    Plp(),
    Pbp(),
    Pq(),
    Pqm(1),                 //  Set backward compatibility mode by default.
    Pstages(),
    Pminfreq(),
    Pamode(),
    variq(),
    Ftype(1),
    Fstages(1),
    rpanning(),
    lpanning(),
    depth(),
    ampsns(),
    ampsmooth(),
    wahsmooth(1.0f - expf(-1.0f / (0.02f * sample_rate))),  //  0.02 seconds
    fbias(),
    oldfbias(),
    oldfbias1(),
    oldfbias2(),
    q(10.0f),
    frequency(40.0f),
    maxfreq(5000.0f),
    base(7.0f),             //  sets curve of modulation to frequency relationship
    ibase(1.0f / base),
    minfreq(40.0f),
    ms1(),
    lpmix(0.5f),
    hpmix(),
    bpmix(2.0f),
    centfreq(),
    filterl(NULL),
    filterr(NULL),
    sidechain_filter(NULL),
    interpbuf(NULL),
    lfo(NULL)
{
    lfo = new EffectLFO(sample_rate);

    initialize();
    setpreset(Ppreset);

    cleanup();
}

MuTroMojo::~MuTroMojo()
{
    delete lfo;
    clear_initialize();
}

/*
 * Apply the effect
 */
void
MuTroMojo::out(float * efxoutl, float * efxoutr)
{
    float lfol, lfor;
    lfo->effectlfoout(&lfol, &lfor);
    
    if (Pamode)
    {
        lfol *= depth;
        lfor *= depth;
    }
    else
    {
        lfol *= depth * 5.0f;
        lfor *= depth * 5.0f;
    }

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        float x = (fabsf(sidechain_filter->filterout_s(efxoutl[i] + efxoutr[i]))) * 0.5f;
        ms1 = ms1 * ampsmooth + x * (1.0f - ampsmooth) + 1e-10f;

        //oldfbias -= 0.001 * oldfbias2;
        oldfbias = oldfbias * (1.0f - wahsmooth) + fbias * wahsmooth + 1e-10f; //smooth MIDI control
        oldfbias1 = oldfbias1 * (1.0f - wahsmooth) + oldfbias * wahsmooth + 1e-10f;
        oldfbias2 = oldfbias2 * (1.0f - wahsmooth) + oldfbias1 * wahsmooth + 1e-10f;

        if (Pamode)
        {
            float rms = ms1 * ampsns + oldfbias2;
            
            if (rms < 0.0f)
            {
                rms = 0.0f;
            }
            
            float lmod = (minfreq + lfol + rms) * maxfreq;
            float rmod = (minfreq + lfor + rms) * maxfreq;
            
            if (variq)
            {
                q = f_pow2((2.0f * (1.0f - rms) + 1.0f));
            }
            
            filterl->setq(q);
            filterr->setq(q);
            filterl->directmod(rmod);
            filterr->directmod(lmod);
            efxoutl[i] = filterl->filterout_s(efxoutl[i]);
            efxoutr[i] = filterr->filterout_s(efxoutr[i]);
        }
    }

    if (!Pamode)
    {
        float rms = ms1 * ampsns + oldfbias2;

        if (rms > 0.0f)
        { //apply some smooth limiting
            rms = 1.0f - 1.0f / (rms * rms + 1.0f);
        }
        else
        {
            rms = -1.0f + 1.0f / (rms * rms + 1.0f);
        }

        if (variq)
        {
            q = f_pow2((2.0f * (1.0f - rms) + 1.0f));
        }

        float lmod = (lfol + rms);
        float rmod = (lfor + rms);
        
        if (lmod > 1.0f) lmod = 1.0f;
        if (lmod < 0.0f) lmod = 0.0f;
        if (rmod > 1.0f) rmod = 1.0f;
        if (rmod < 0.0f) rmod = 0.0f;

        //rms*=rms;
        float frl = minfreq + maxfreq * (powf(base, lmod) - 1.0f) * ibase;
        float frr = minfreq + maxfreq * (powf(base, rmod) - 1.0f) * ibase;

        centfreq = frl; //testing variable

        filterl->setfreq_and_q(frl, q);
        filterr->setfreq_and_q(frr, q);

        filterl->filterout(efxoutl, PERIOD);
        filterr->filterout(efxoutr, PERIOD);
    }
}

/*
 * Cleanup the effect
 */
void
MuTroMojo::cleanup()
{
    reinitfilter();
    ms1 = 0.0;
    oldfbias = oldfbias1 = oldfbias2 = 0.0f;
    filterl->cleanup();
    filterr->cleanup();
}

#ifdef LV2_SUPPORT
void
MuTroMojo::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        clear_initialize();
        initialize();
        reinitfilter();
        filterl->setstages(Pstages);
        filterr->setstages(Pstages);
        filterl->setmode(Pqm);
        filterr->setmode(Pqm);
    }
    else
    {
        PERIOD = period;
    }

    lfo->updateparams(period);
}
#endif // LV2

void
MuTroMojo::set_random_parameters()
{
    for(int i = 0; i < C_MUTRO_PARAMETERS; i++)
    {
        switch(i)
        {
            case MuTro_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case MuTro_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case MuTro_DryWet:
            case MuTro_Resonance:
            case MuTro_Depth:
            case MuTro_Wah:
            case MuTro_Env_Smooth:
            case MuTro_LFO_Random:
            case MuTro_LFO_Stereo:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;
    
            case MuTro_LowPass:
            case MuTro_BandPass:
            case MuTro_HighPass:
            case MuTro_Env_Sens:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case MuTro_Stages:
            {
                int value = (int) (RND * 12);
                changepar (i, value + 1);
            }
            break;

            case MuTro_Range:
            {
                int value = (int) (RND * 5990);
                changepar (i, value + 10);
            }
            break;

            case MuTro_St_Freq:
            {
                int value = (int) (RND * 770);
                changepar (i, value + 30);
            }
            break;

            case MuTro_Mod_Res:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Mutro_Mode_Legacy:
            {
                int value = (int) (RND * 3);
                changepar (i, value);
            }
            break;
        }
    }
}

void
MuTroMojo::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    for(int i = 0; i < (C_MUTRO_PARAMETERS + 1); i++)   // -1 for Mutro_Mode_Legacy, +2 for MuTro_AG_Mode, MuTro_Exp_Wah
    {
        switch(param_case_offset)
        {
            // Normal processing
            case MuTro_Resonance:
            case MuTro_LFO_Tempo:
            case MuTro_LFO_Random:
            case MuTro_LFO_Type:
            case MuTro_Depth:
            case MuTro_Env_Sens:
            case MuTro_Wah:
            case MuTro_Env_Smooth:
            case MuTro_LowPass:
            case MuTro_BandPass:
            case MuTro_HighPass:
            case MuTro_Stages:
            case MuTro_Range:
            case MuTro_St_Freq:
            case MuTro_AG_Mode:
            case MuTro_Exp_Wah:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), mutro_parameters[i * 3 + 1], mutro_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  MuTro_Exp_Wah)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case MuTro_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( MuTro_DryWet )), mutro_parameters[i * 3 + 1], mutro_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( MuTro_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case MuTro_LFO_Stereo:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( MuTro_LFO_Stereo ) - 64, mutro_parameters[i * 3 + 1], mutro_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( MuTro_LFO_Stereo ) - 64);
                    s_buf += ":";
                }
            }
            break;

            // Skip after
            case MuTro_Mod_Res:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( MuTro_Mod_Res ), mutro_parameters[i * 3 + 1], mutro_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( MuTro_Mod_Res ) );
                    s_buf += ":";
                }

                param_case_offset += 1; // Skip: Mutro_Mode_Legacy
            }
            break;
        }
        
        param_case_offset++;
    }
}

void
MuTroMojo::initialize()
{
    interpbuf = new float[PERIOD];
    filterl = new RBFilter(0, 80.0f, 70.0f, 1, fSAMPLE_RATE, interpbuf);
    filterr = new RBFilter(0, 80.0f, 70.0f, 1, fSAMPLE_RATE, interpbuf);

    sidechain_filter = new AnalogFilter(1, 630.0, 1.0, 1, fSAMPLE_RATE, interpbuf);
}

void
MuTroMojo::clear_initialize()
{
    delete filterl;
    delete filterr;
    delete sidechain_filter;
    delete[] interpbuf;
}

/*
 * Parameter control
 */

void
MuTroMojo::setwidth(int _Pwidth)
{
    Pwidth = _Pwidth;
    depth = powf(((float) _Pwidth / 127.0f), 2.0f);
}

void
MuTroMojo::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
MuTroMojo::setampsns(int _Pampsns)
{
    Pampsns = _Pampsns;
    
    if (Pampsns > 0)
    {
        ampsns = expf(0.083f * (float) Pampsns);
    }
    else
    {
        ampsns = -expf(-0.083f * (float) Pampsns);
    }
    
    fbias = ((float) Pampsnsinv) / 127.0f;

    ampsmooth = f_exp(-1.0f / ((((float) Pampsmooth) / 127.0f + 0.01f) * fSAMPLE_RATE)); //goes up to 1 second
}

void
MuTroMojo::reinitfilter()
{
    //setmix (int mix, float lpmix, float bpmix, float hpmix)
    filterl->setmix(1, lpmix, bpmix, hpmix);
    filterr->setmix(1, lpmix, bpmix, hpmix);
}

void
MuTroMojo::setpreset(int npreset)
{
    const int PRESET_SIZE = C_MUTRO_PARAMETERS;
    const int NUM_PRESETS = 7;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Wah Pedal
        {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450, 1, 1},
        //Mutron
        {0, 15, 138, 0, 0, 64, 0, 50, 0, 30, 32, 0, 5, 1, 2000, 60, 0, 1},
        //Phase Wah
        {0, 50, 60, 0, 0, 64, 30, 10, 10, 30, 32, 0, 10, 2, 2000, 350, 1, 1},
        //Succulent Phaser
        {64, 8, 35, 10, 0, 64, 50, -10, 53, 35, 28, -16, 32, 4, 2600, 300, 1, 1},
        //Quacky
        {16, 10, 60, 0, 0, 64, 0, 40, 10, 32, -16, 40, -3, 1, 2000, 400, 1, 1},
        //Smoothtron
        {0, 15, 138, 0, 0, 64, 0, 15, 0, 20, 32, 0, 5, 1, 2000, 60, 0, 3},
        //Wah Fever
        {0, 10, 128, 0, 0, 62, 50, -30, 2, 0, 0, 10, -30, 1, 2000, 300, 1, 0}

    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_MUTROMOJO, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = npreset;

    reinitfilter();
}

void
MuTroMojo::changepar(int npar, int value)
{
    switch (npar)
    {
    case MuTro_DryWet:
        setvolume(value);
        break;
    case MuTro_Resonance:
        Pq = value;
        q = (float) Pq;
        break;
    case MuTro_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case MuTro_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case MuTro_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case MuTro_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case MuTro_Depth:
        setwidth(value);
        break;
    case MuTro_Env_Sens:
        setampsns(value);
        break;
    case MuTro_Wah:
        Pampsnsinv = value;
        setampsns(Pampsns);
        break;
    case MuTro_Env_Smooth:
        Pampsmooth = value;
        setampsns(Pampsns);
        break;
    case MuTro_LowPass:
        Plp = value;
        lpmix = ((float) Plp) / 32.0f;
        reinitfilter();
        break;
    case MuTro_BandPass:
        Pbp = value;
        bpmix = ((float) Pbp) / 32.0f;
        reinitfilter();
        break;
    case MuTro_HighPass:
        Php = value;
        hpmix = ((float) Php) / 32.0f;
        reinitfilter();
        break;
    case MuTro_Stages:
        Pstages = (value - 1);
        filterl->setstages(Pstages);
        filterr->setstages(Pstages);
        cleanup();
        break;
    case MuTro_Range:
        {
            Prange = value;
            if (Pamode)
            {
                maxfreq = ((float) Prange) / (fSAMPLE_RATE / 6.0f);
            }
            else
            {
                maxfreq = ((float) Prange);
            }
        }
        break;
    case MuTro_St_Freq:
        {
            Pminfreq = value;
            if (Pamode)
            {
                minfreq = ((float) Pminfreq) / (fSAMPLE_RATE / 6.0f);
            }
            else
            {
                minfreq = (float) value;
            }
        }
        break;
    case MuTro_Mod_Res: // Modulate Resonance
        {
            variq = value;
            if (!variq) // when variq is unset, we need to reset q back to default (case 1)
            {
                q = (float) Pq; // or q continues with variq values until user manually moves slider for resonance (q)
            }
        }
        break;
    case Mutro_Mode_Legacy:
        //legacy method of changing Pqm and Pamode, presets use this
        {
            Pmode = value;
            
            if ((Pmode == 1) || (Pmode == 3))
            {
                Pqm = 1;
            }
            else
            {
                Pqm = 0;
            }
            
            filterl->setmode(Pqm);
            filterr->setmode(Pqm);

            if ((Pmode == 2) || (Pmode == 3))
            {
                Pamode = 1;
            }
            else
            {
                Pamode = 0;
            }
            
            if (Pamode)
            {
                minfreq = ((float) Pminfreq) / (fSAMPLE_RATE / 6.0f);
                maxfreq = ((float) Prange) / (fSAMPLE_RATE / 6.0f);
            }
            else
            {
                minfreq = (float) Pminfreq;
                maxfreq = (float) Prange;
            }
        }
        break;

#ifdef LV2_SUPPORT
    case MuTro_AG_Mode: // LV2 - Analog Gain Mode (M box in rakarrack) 
        Pqm = value;
        filterl->setmode(Pqm);
        filterr->setmode(Pqm);
        break;
    case MuTro_Exp_Wah: // LV2 - Exponential Wah (N box in rakarrack)
        {
            Pamode = value;
            if (Pamode)
            {
                minfreq = ((float) Pminfreq) / (fSAMPLE_RATE / 6.0f);
                maxfreq = ((float) Prange) / (fSAMPLE_RATE / 6.0f);
            }
            else
            {
                minfreq = (float) Pminfreq;
                maxfreq = (float) Prange;
            }
        }
        break;
#endif // LV2_SUPPORT
    default:
        return;
    }
}

int
MuTroMojo::getpar(int npar)
{
    switch (npar)
    {
    case MuTro_DryWet:
        return (Pvolume);

    case MuTro_Resonance:
        return (Pq);

    case MuTro_LFO_Tempo:
        return (lfo->Pfreq);

    case MuTro_LFO_Random:
        return (lfo->Prandomness);

    case MuTro_LFO_Type:
        return (lfo->PLFOtype);

    case MuTro_LFO_Stereo:
        return (lfo->Pstereo);

    case MuTro_Depth:
        return (Pwidth);

    case MuTro_Env_Sens:
        return (Pampsns);

    case MuTro_Wah:
        return (Pampsnsinv);

    case MuTro_Env_Smooth:
        return (Pampsmooth);

    case MuTro_LowPass:
        return (Plp);

    case MuTro_BandPass:
        return (Pbp);

    case MuTro_HighPass:
        return (Php);

    case MuTro_Stages:
        return (Pstages + 1);

    case MuTro_Range:
        return (Prange);

    case MuTro_St_Freq:
        return (Pminfreq);

    case MuTro_Mod_Res:
        return (variq);

    case Mutro_Mode_Legacy:
        return (Pmode);

//#ifdef LV2_SUPPORT
    case MuTro_AG_Mode:
        return (Pqm);

    case MuTro_Exp_Wah:
        return (Pamode);

//#endif // LV2_SUPPORT
    }

    return (0);
}