/*
  Rakarrack   Audio FX software
  Exciter.C - Harmonic Enhancer
  Based in Steve Harris LADSPA Plugin harmonic_gen
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Exciter.h"

Exciter::Exciter(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    Prm(),
    Pvolume(50),
    lpffreq(8000),
    hpffreq(2500),
    rm(),
    harm(NULL)
{
    harm = new HarmEnhancer(rm, 2500.0f, 8000.0, 1.0f, sample_rate, PERIOD);

    cleanup();
    setpreset(Ppreset);
}

Exciter::~Exciter()
{
    delete harm;
}

/*
 * Cleanup the effect
 */
void
Exciter::cleanup()
{
    harm->cleanup();
}

#ifdef LV2_SUPPORT
void
Exciter::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    harm->lv2_update_params(period);
}
#endif // LV2

void
Exciter::set_random_parameters()
{
    for(int i = 0; i < C_EXCITER_PARAMETERS; i++)
    {
        switch(i)
        {
            case Exciter_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Exciter_Harm_1:
            case Exciter_Harm_2:
            case Exciter_Harm_3:
            case Exciter_Harm_4:
            case Exciter_Harm_5:
            case Exciter_Harm_6:
            case Exciter_Harm_7:
            case Exciter_Harm_8:
            case Exciter_Harm_9:
            case Exciter_Harm_10:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64 );
            }
            break;

            case Exciter_LPF:
            {
                int value = (int) (RND * 25980);
                changepar (i, value + 20);
            }
            break;

            case Exciter_HPF:
            {
                int value = (int) (RND * 19980);
                changepar (i, value + 20);
            }
            break;
        }
    }
}

void
Exciter::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_EXCITER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Exciter_Gain:
            case Exciter_Harm_1:
            case Exciter_Harm_2:
            case Exciter_Harm_3:
            case Exciter_Harm_4:
            case Exciter_Harm_5:
            case Exciter_Harm_6:
            case Exciter_Harm_7:
            case Exciter_Harm_8:
            case Exciter_Harm_9:
            case Exciter_Harm_10:
            case Exciter_LPF:
            case Exciter_HPF:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), exciter_parameters[i * 3 + 1], exciter_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Exciter_HPF )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
        }
    }
}

/*
 * Effect output
 */
void
Exciter::out(float * efxoutl, float * efxoutr)
{
    harm->harm_out(efxoutl, efxoutr);
}

void
Exciter::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */
void
Exciter::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
    harm->set_vol(0, outvolume);
}

void
Exciter::setlpf(int value)
{
    lpffreq = value;
    harm->set_freql(0, (float) value);
}

void
Exciter::sethpf(int value)
{
    hpffreq = value;
    harm->set_freqh(0, (float) value);
}

void
Exciter::sethar(int num, int value)
{
    float har = 32.0f * ((float) num + 1.0f);
    
    if (num % 2 == 1)
    {
        har = 11200.0f - 64.0f * ((float) num + 1.0f);
    }
    
    Prm[num] = value;
    rm[num] = (float) value / har;
    harm->calcula_mag(rm);
}

void
Exciter::setpreset(int npreset)
{
    const int PRESET_SIZE = C_EXCITER_PARAMETERS;
    const int NUM_PRESETS = 5;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20},
        //Loudness
        {64, 0, 0, 0, 0, 24, 0, -24, 0, 24, 0, 80, 20},
        //Exciter 1
        {64, 64, 0, 0, 0, 64, 0, 0, 0, 64, 0, 20000, 20},
        //Exciter 2
        {64, 0, 0, 0, -32, 0, -32, -64, 0, 48, 0, 14100, 5660},
        //Exciter 3
        {64, 64, 0, -64, 0, 64, 0, -64, 0, 64, 0, 20000, 20}

    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_EXCITER, npreset - NUM_PRESETS + 1, pdata);
        
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
Exciter::changepar(int npar, int value)
{
    switch (npar)
    {
    case Exciter_Gain:
        setvolume(value);
        break;
    case Exciter_Harm_1:
        sethar(0, value);
        break;
    case Exciter_Harm_2:
        sethar(1, value);
        break;
    case Exciter_Harm_3:
        sethar(2, value);
        break;
    case Exciter_Harm_4:
        sethar(3, value);
        break;
    case Exciter_Harm_5:
        sethar(4, value);
        break;
    case Exciter_Harm_6:
        sethar(5, value);
        break;
    case Exciter_Harm_7:
        sethar(6, value);
        break;
    case Exciter_Harm_8:
        sethar(7, value);
        break;
    case Exciter_Harm_9:
        sethar(8, value);
        break;
    case Exciter_Harm_10:
        sethar(9, value);
        break;
    case Exciter_LPF:
        setlpf(value);
        break;
    case Exciter_HPF:
        sethpf(value);
        break;
    default:
        return;
    }
}

int
Exciter::getpar(int npar)
{
    switch (npar)
    {
    case Exciter_Gain:
        return (Pvolume);

    case Exciter_Harm_1:
        return (Prm[0]);

    case Exciter_Harm_2:
        return (Prm[1]);

    case Exciter_Harm_3:
        return (Prm[2]);

    case Exciter_Harm_4:
        return (Prm[3]);

    case Exciter_Harm_5:
        return (Prm[4]);

    case Exciter_Harm_6:
        return (Prm[5]);

    case Exciter_Harm_7:
        return (Prm[6]);

    case Exciter_Harm_8:
        return (Prm[7]);

    case Exciter_Harm_9:
        return (Prm[8]);

    case Exciter_Harm_10:
        return (Prm[9]);

    case Exciter_LPF:
        return (lpffreq);

    case Exciter_HPF:
        return (hpffreq);

    }
    return (0); //in case of bogus parameter number
}

