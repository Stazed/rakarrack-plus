/*
  Rakarrack   Audio FX software
  CoilCrafter.C - Harmonic Enhancer
  Based in Steve Harris LADSPA Plugin harmonic_gen
  Modified for rakarrack by Ryan Billing and Josep Andreu


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
#include "CoilCrafter.h"

CoilCrafter::CoilCrafter(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),   // correct for rakarrack, may be adjusted for lv2
    Pvolume(50),
    Ppo(),
    Ppd(),
    Ptone(20),
    Pq1(),
    Pfreq1(),
    Pq2(),
    Pfreq2(),
    Pmode(),
    tfreqs(),
    tqs(),
    rm(),
    freq1(),
    q1(),
    freq2(),
    q2(),
    att(16.0f * powf(PI, sample_rate / 44100.0f)),
    harm(NULL),
    RB1l(NULL),
    RB1r(NULL),
    RB2l(NULL),
    RB2r(NULL),
    interpbuf(NULL)
{
    //default values
    outvolume = 1.5f;

    tfreqs[0] = 4000.0f;
    tfreqs[1] = 4400.0f;
    tfreqs[2] = 4200.0f;
    tfreqs[3] = 2900.0f;
    tfreqs[4] = 3000.0f;
    tfreqs[5] = 2700.0f;
    tfreqs[6] = 3300.0f;
    tfreqs[7] = 3300.0f;
    tfreqs[8] = 2800.0f;

    tqs[0] = 4.2f;
    tqs[1] = 4.2f;
    tqs[2] = 2.3f;
    tqs[3] = 1.8f;
    tqs[4] = 2.2f;
    tqs[5] = 2.1f;
    tqs[6] = 1.7f;
    tqs[7] = 1.7f;
    tqs[8] = 1.8f;

    for (int i = 0; i < 10; i++)
    {
        rm[i] = 0.0f;
    }
    
    rm[0] = 1.0;
    rm[2] = -1.0;
    rm[4] = 1.0;
    rm[6] = -1.0;
    rm[8] = 1.0;

    harm = new HarmEnhancer(rm, 2500.0f, 4400.0f, 1.0f, sample_rate, PERIOD);

    interpbuf = new float[PERIOD];
    RB1l = new AnalogFilter(2, 2000.0f, 1.0f, 0, sample_rate, interpbuf);
    RB1r = new AnalogFilter(2, 2000.0f, 1.0f, 0, sample_rate, interpbuf);
    RB2l = new AnalogFilter(2, 2000.0f, 1.0f, 0, sample_rate, interpbuf);
    RB2r = new AnalogFilter(2, 2000.0f, 1.0f, 0, sample_rate, interpbuf);

    CoilCrafter::cleanup();

    CoilCrafter::setpreset(Ppreset);
}

CoilCrafter::~CoilCrafter()
{
    delete harm;
    delete RB1l;
    delete RB1r;
    delete RB2l;
    delete RB2r;
    delete[] interpbuf;
}

/*
 * Cleanup the effect
 */
void
CoilCrafter::cleanup()
{
    harm->cleanup();
    harm->calcula_mag(rm);
    harm->set_vol(1, 1.0f);

    RB1l->cleanup();
    RB1r->cleanup();
    RB2l->cleanup();
    RB2r->cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
CoilCrafter::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    harm->lv2_update_params(period);
}
#endif // LV2

void
CoilCrafter::set_random_parameters()
{
    for(int i = 0; i < C_COIL_PARAMETERS; i++)
    {
        switch(i)
        {
            case Coil_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Coil_Tone:
            {
                int value = (int) (RND * 4380);
                changepar (i, value + 20);
            }
            break;

            // Skip these
            case Coil_Origin:
            case Coil_Destiny:
                break;

            case Coil_Freq_1:
            case Coil_Freq_2:
            {
                int value = (int) (RND * 1900);
                changepar (i, value + 2600);
            }
            break;
            
            case Coil_Q_1:
            case Coil_Q_2:
            {
                int value = (int) (RND * 55);
                changepar (i, value + 10);
            }
            break;
            
            case Coil_NeckMode:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;
        }
    }
}

void
CoilCrafter::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    
    for(int i = 0; i < (C_COIL_PARAMETERS - 2); i++)    // -2  Coil_Origin, Coil_Destiny skipped
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Coil_Freq_1:
            case Coil_Q_1:
            case Coil_Freq_2:
            case Coil_Q_2:
            case Coil_Tone:
            case Coil_NeckMode:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), coil_parameters[i * 3 + 1], coil_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Coil_NeckMode )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            //  Coil_Origin, Coil_Destiny, are skipped after gain
            case Coil_Gain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Coil_Gain ), coil_parameters[i * 3 + 1], coil_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Coil_Gain ) );
                    s_buf += ":";
                }

                // skip Coil_Origin, Coil_Destiny
                param_case_offset += 2;
            }
            break;
        }
        
        param_case_offset++;
    }
}

/*
 * Effect output
 */
void
CoilCrafter::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    if (Ppo > 0)
    {
        RB1l->filterout(efxoutl, PERIOD);
        RB1r->filterout(efxoutr, PERIOD);

        for (unsigned i = 0; i < PERIOD; i++)
        {
            efxoutl[i] *= att;
            efxoutr[i] *= att;
        }
    }
    
    if (Ppd > 0)
    {
        RB2l->filterout(efxoutl, PERIOD);
        RB2r->filterout(efxoutr, PERIOD);
    }

    if (Pmode) harm->harm_out(efxoutl, efxoutr);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        efxoutl[i] *= outvolume;
        efxoutr[i] *= outvolume;

        if (Pmode)
        {
            efxoutl[i] *= .5f;
            efxoutr[i] *= .5f;
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
CoilCrafter::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */
void
CoilCrafter::setvolume(int value)
{
    Pvolume = value;
    outvolume = (1.0f + (float) Pvolume) / 127.0f;
}

void
CoilCrafter::setfreq1()
{
    RB1l->setfreq(freq1);
    RB1l->reversecoeffs();
    RB1r->setfreq(freq1);
    RB1r->reversecoeffs();
}

void
CoilCrafter::setq1()
{
    RB1l->setq(q1);
    RB1l->reversecoeffs();
    RB1r->setq(q1);
    RB1r->reversecoeffs();
}

void
CoilCrafter::setfreq2()
{
    RB2l->setfreq(freq2);
    RB2r->setfreq(freq2);
}

void
CoilCrafter::setq2()
{
    RB2l->setq(q2);
    RB2r->setq(q2);
}

void
CoilCrafter::sethpf(int value)
{
    harm->set_freqh(0, (float) value);
}

void
CoilCrafter::setpreset(int npreset)
{
    const int PRESET_SIZE = C_COIL_PARAMETERS;
    const int NUM_PRESETS = 2;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //H to S
        {32, 6, 1, 3300, 16, 4400, 42, 20, 0},
        //S to H
        {32, 1, 6, 4400, 42, 3300, 16, 20, 0},
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_COILCRAFTER, npreset - NUM_PRESETS + 1, pdata);
        
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
CoilCrafter::changepar(int npar, int value)
{
    switch (npar)
    {
    case Coil_Gain:
        setvolume(value);
        break;
    case Coil_Origin:
        Ppo = value;
        if (Ppo > 0)
        {
            freq1 = tfreqs[value];
            Pfreq1 = (int) freq1;
            setfreq1();
            q1 = tqs[value];
            Pq1 = (int) (q1 * 10.0f);
            setq1();
        }
        break;
    case Coil_Destiny:
        Ppd = value;
        if (Ppd > 0)
        {
            freq2 = tfreqs[value];
            Pfreq2 = (int) freq2;
            setfreq2();
            q2 = tqs[value];
            Pq2 = (int) (q2 * 10.0f);
            setq2();
        }
        break;
    case Coil_Freq_1:
        Pfreq1 = value;
        freq1 = (float) value;
        setfreq1();
        break;
    case Coil_Q_1:
        Pq1 = value;
        q1 = (float) value / 10.0f;
        setq1();
        break;
    case Coil_Freq_2:
        Pfreq2 = value;
        freq2 = (float) value;
        setfreq2();
        break;
    case Coil_Q_2:
        Pq2 = value;
        q2 = (float) value / 10.0f;
        setq2();
        break;
    case Coil_Tone:
        Ptone = value;
        sethpf(value);
        break;
    case Coil_NeckMode:
        Pmode = value;
        break;
    default:
        return;
    }
}

int
CoilCrafter::getpar(int npar)
{
    switch (npar)
    {
    case Coil_Gain:
        return (Pvolume);

    case Coil_Origin:
        return (Ppo);

    case Coil_Destiny:
        return (Ppd);

    case Coil_Freq_1:
        return (Pfreq1);

    case Coil_Q_1:
        return (Pq1);

    case Coil_Freq_2:
        return (Pfreq2);

    case Coil_Q_2:
        return (Pq2);

    case Coil_Tone:
        return (Ptone);

    case Coil_NeckMode:
        return (Pmode);

    }
    return (0); //in case of bogus parameter number
}

