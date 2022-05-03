/*
  Rakarrack   Audio FX software
  ShelfBoost.C - Tone Booster
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
#include "ShelfBoost.h"

ShelfBoost::ShelfBoost(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(50),
    Plevel(),
    Pq1(),
    Pfreq1(),
    Pstereo(),
    freq1(1.0f),
    q1(1.0f),
    gain(1.0f),
    u_gain(1.0f),
    RB1l(NULL),
    RB1r(NULL),
    interpbuf(NULL)
{
    initialize();
    cleanup();

    setpreset(Ppreset);
}

ShelfBoost::~ShelfBoost()
{
    clear_initialize();
}

/*
 * Cleanup the effect
 */
void
ShelfBoost::cleanup()
{
    RB1l->cleanup();
    RB1r->cleanup();
}

#ifdef LV2_SUPPORT
void
ShelfBoost::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        clear_initialize();
        initialize();
        set_q(Pq1);
        set_freq(Pfreq1);
        set_level(Plevel);
    }
    else
    {
        PERIOD = period;
    }
}
#endif // LV2

void
ShelfBoost::set_random_parameters()
{
    for(int i = 0; i < C_SHELF_PARAMETERS; i++)
    {
        switch(i)
        {
            case Shelf_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Shelf_Presence:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Shelf_Tone:
            {
                int value = (int) (RND * 15780);
                changepar (i, value + 220);
            }
            break;

            case Shelf_Stereo:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Shelf_Level:
            {
                int value = (int) (RND * 126);
                changepar (i, value + 1);
            }
            break;
        }
    }
}

void
ShelfBoost::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SHELF_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Shelf_Gain:
            case Shelf_Presence:
            case Shelf_Tone:
            case Shelf_Stereo:
            case Shelf_Level:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), shelf_parameters[i * 3 + 1], shelf_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Shelf_Level )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
        }
    }
}

void
ShelfBoost::initialize()
{
    interpbuf = new float[PERIOD];
    RB1l = new AnalogFilter(7, 3200.0f, 0.5f, 0, fSAMPLE_RATE, interpbuf);
    RB1r = new AnalogFilter(7, 3200.0f, 0.5f, 0, fSAMPLE_RATE, interpbuf);
}

void
ShelfBoost::clear_initialize()
{
    delete RB1l;
    delete RB1r;
    delete[] interpbuf;
}

/*
 * Effect output
 */
void
ShelfBoost::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    RB1l->filterout(efxoutl, PERIOD);
    
    if (Pstereo)
    {
        RB1r->filterout(efxoutr, PERIOD);
    }

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        efxoutl[i] *= outvolume*u_gain;
        
        if (Pstereo)
        {
            efxoutr[i] *= outvolume * u_gain;
        }

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();

    if (!Pstereo)
    {
        memcpy(efxoutr, efxoutl, sizeof (float)*PERIOD);
    }
}

void
ShelfBoost::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */
void
ShelfBoost::setvolume(int value)
{
    Pvolume = value;
    outvolume = (float) Pvolume / 127.0f;
}

void
ShelfBoost::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SHELF_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Treble
        {127, 64, 16000, 1, 24},
        //Mid
        {127, 64, 4400, 1, 24},
        //Bass
        {127, 64, 220, 1, 24},
        //Distortion 1
        {6, 40, 12600, 1, 127}

    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_SHELFBOOST, npreset - NUM_PRESETS + 1, pdata);
        
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
ShelfBoost::set_q(int value)
{
    Pq1 = value;
    q1 = powf(30.0f, ((float) value - 64.0f) / 64.0f);
    RB1l->setq(q1);
    RB1r->setq(q1);
}

void
ShelfBoost::set_freq(int value)
{
    Pfreq1 = value;
    freq1 = (float) value;
    RB1l->setfreq(freq1);
    RB1r->setfreq(freq1);
}

void
ShelfBoost::set_level(int value)
{
    Plevel = value;
    gain = .375f * (float) value;
    u_gain = 1.0f / gain;
    RB1l->setgain(gain);
    RB1r->setgain(gain);
}

void
ShelfBoost::changepar(int npar, int value)
{
    switch (npar)
    {
    case Shelf_Gain:
        setvolume(value);
        break;
    case Shelf_Presence:
        set_q(value);
        break;
    case Shelf_Tone:
        set_freq(value);
        break;
    case Shelf_Stereo:
        Pstereo = value;
        break;
    case Shelf_Level:
        set_level(value);
        break;
    default:
        return;
    }
}

int
ShelfBoost::getpar(int npar)
{
    switch (npar)
    {
    case Shelf_Gain:
        return (Pvolume);

    case Shelf_Presence:
        return (Pq1);

    case Shelf_Tone:
        return (Pfreq1);

    case Shelf_Stereo:
        return (Pstereo);

    case Shelf_Level:
        return (Plevel);

    }
    return (0); //in case of bogus parameter number
}

