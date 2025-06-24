/*
  Rakarrack Audio FX
  Ring DSP Code based on "(author)" LADSPA plugin(swh-plugins).
  ZynAddSubFX effect structure - Copyright (C) 2002-2005 Nasca Octavian Paul
  Modified and adapted for rakarrack by Josep Andreu

  Ring.C - Ring Modulator effect

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
#include "Ring.h"

Ring::Ring(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    Pafreq(),
    Pfreq(),
    SAMPLE_RATE(lrintf(sample_rate)),
    PERIOD(intermediate_bufsize),
    Pvolume(50),
    Ppanning(),
    Plrcross(40),
    Pinput(),
    Plevel(64),
    Pdepthp(),
    Psin(),
    Ptri(),
    Psaw(),
    Psqu(),
    Pstereo(),
    offset(),
    panning(),
    lrcross(),
    sin_tbl(NULL),
    tri_tbl(NULL),
    saw_tbl(NULL),
    squ_tbl(NULL),
    sin(),
    tri(),
    saw(),
    squ(),
    scale(1.0f),
    depth(),
    idepth()
{
    sin_tbl = (float *) malloc(sizeof (float) * SAMPLE_RATE);
    tri_tbl = (float *) malloc(sizeof (float) * SAMPLE_RATE);
    squ_tbl = (float *) malloc(sizeof (float) * SAMPLE_RATE);
    saw_tbl = (float *) malloc(sizeof (float) * SAMPLE_RATE);
    //sin_tbl = new float[SAMPLE_RATE];//(float *) malloc(sizeof(float) * SAMPLE_RATE);
    //tri_tbl = new float[SAMPLE_RATE];//(float *) malloc(sizeof(float) * SAMPLE_RATE);
    //squ_tbl = new float[SAMPLE_RATE];//(float *) malloc(sizeof(float) * SAMPLE_RATE);
    //saw_tbl = new float[SAMPLE_RATE];//(float *) malloc(sizeof(float) * SAMPLE_RATE);

    Create_Tables(sample_rate);

    Ring::setpreset(Ppreset);
    Ring::cleanup();
}

Ring::~Ring()
{
    free(sin_tbl);
    free(tri_tbl);
    free(squ_tbl);
    free(saw_tbl);
}

/*
 * Create Tables
 */
void
Ring::Create_Tables(float fSAMPLE_RATE)
{
    unsigned int i;
    float SR = fSAMPLE_RATE;

    for (i = 0; i < SAMPLE_RATE; i++) sin_tbl[i] = sinf((float) i * D_PI / SR);
    for (i = 0; i < SAMPLE_RATE; i++) tri_tbl[i] = acosf(cosf((float) i * D_PI / SR)) / D_PI - 1.0f;
    for (i = 0; i < SAMPLE_RATE; i++) squ_tbl[i] = (i < SAMPLE_RATE / 2) ? 1.0f : -1.0f;
    for (i = 0; i < SAMPLE_RATE; i++) saw_tbl[i] = ((2.0f * i) - SR) / SR;
}

/*
 * Cleanup the effect
 */
void
Ring::cleanup()
{

}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Ring::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
}
#endif // LV2

void
Ring::set_random_parameters()
{
    for(int i = 0; i < C_RING_PARAMETERS; i++)
    {
        switch(i)
        {
            case Ring_Pan:
            case Ring_LR_Cross:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Ring_Freq:
            {
                int value = (int) (RND * 20000);
                changepar (i, value + 1);
            }
            break;

            case Ring_Stereo:
            case Ring_Auto_Freq:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Ring_Depth:
            case Ring_Sine:
            case Ring_Triangle:
            case Ring_Saw:
            case Ring_Square:
            {
                int value = (int) (RND * 101);
                changepar (i, value);
            }
            break;

            case Ring_DryWet:
            case Ring_Level:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Ring_Input:
            {
                int value = (int) (RND * 127);
                changepar (i, value + 1);
            }
            break;
        }
    }
}

void
Ring::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_RING_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Ring_Pan:
            case Ring_LR_Cross:
            case Ring_Level:
            case Ring_Depth:
            case Ring_Freq:
            case Ring_Stereo:
            case Ring_Sine:
            case Ring_Triangle:
            case Ring_Saw:
            case Ring_Square:
            case Ring_Input:
            case Ring_Auto_Freq:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), ring_parameters[i * 3 + 1], ring_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Ring_Auto_Freq )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Ring_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Ring_DryWet )), ring_parameters[i * 3 + 1], ring_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Ring_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

/*
 * Apply the filters
 */

/*
 * Effect output
 */
void
Ring::out(float * efxoutl, float * efxoutr)
{
    float inputvol = (float) Pinput / 127.0f;

    if (Pstereo != 0)
    {
        //Stereo
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            efxoutl[i] = efxoutl[i] * inputvol;
            efxoutr[i] = efxoutr[i] * inputvol;
            
            if (inputvol == 0.0)
            {
                efxoutl[i] = 1.0;
                efxoutr[i] = 1.0;
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < PERIOD; i++)
        {
            efxoutl[i] = (efxoutl[i] + efxoutr[i]) * inputvol;
            
            if (inputvol == 0.0)
            {
                efxoutl[i] = 1.0;
            }
        }
    }

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        float tmpfactor = depth * (scale * (sin * sin_tbl[offset] + tri * tri_tbl[offset] + saw * saw_tbl[offset] + squ * squ_tbl[offset]) + idepth); //This is now mathematically equivalent, but less computation
        efxoutl[i] *= tmpfactor;

        if (Pstereo != 0)
        {
            efxoutr[i] *= tmpfactor;
        }
        
        offset += Pfreq;
        
        if (offset >= SAMPLE_RATE)
        {
            offset -= SAMPLE_RATE;
        }
    }

    if (Pstereo == 0)
    {
        memcpy(efxoutr, efxoutl, PERIOD * sizeof (float));
    }

    float level = dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f);
    
    for (unsigned int i = 0; i < PERIOD; i++)
    {
        float lout = efxoutl[i];
        float rout = efxoutr[i];

        float l = lout * (1.0f - lrcross) + rout * lrcross;
        float r = rout * (1.0f - lrcross) + lout * lrcross;

        lout = l;
        rout = r;

        efxoutl[i] = lout * level * (1.0f - panning);
        efxoutr[i] = rout * level * panning;
    }
}

/*
 * Parameter control
 */

void
Ring::setvolume (int volume)
{
    Pvolume = volume;
    outvolume = (float) volume / 127.0f;
}

void
Ring::setpanning(int Ppan)
{
    Ppanning = Ppan;
    panning = (float) (Ppanning + 64) / 128.0f;
    // is Ok ...
}

void
Ring::setlrcross(int Plrc)
{
    Plrcross = Plrc;
    lrcross = (float) (Plrcross + 64) / 128.0f;
}

void
Ring::setscale()
{
    scale = sin + tri + saw + squ;
    
    if (scale == 0.0)
    {
        scale = 1.0;
    }
    
    scale = 1.0 / scale;
}

void
Ring::setpreset(int npreset)
{
    const int PRESET_SIZE = C_RING_PARAMETERS;
    const int NUM_PRESETS = 6;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Saw-Sin
        {0, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1},
        //E string
        {64, 0, 0, 64, 100, 82, 0, 100, 0, 0, 0, 64, 0},
        //A string
        {64, 0, 0, 64, 100, 110, 0, 0, 100, 50, 0, 64, 0},
        //dissonance
        {64, 0, 0, 64, 100, 817, 0, 20, 0, 100, 0, 64, 1},
        //Fast Beat
        {64, 0, 0, 64, 100, 15, 0, 20, 0, 100, 0, 64, 1},
        //Ring Amp
        {64, 0, 0, 64, 100, 1, 0, 20, 0, 100, 0, 64, 0},
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_RING, npreset - NUM_PRESETS + 1, pdata);
        
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
Ring::changepar(int npar, int value)
{
    switch (npar)
    {
    case Ring_DryWet:
        setvolume(value);
        break;
    case Ring_Pan:
        setpanning(value);
        break;
    case Ring_LR_Cross:
        setlrcross(value);
        break;
    case Ring_Level:
        Plevel = value;
        break;
    case Ring_Depth:
        Pdepthp = value;
        depth = (float) Pdepthp / 100.0;
        idepth = 1.0f - depth;
        break;
    case Ring_Freq:
        if (value > 20000)
        { //Make sure bad inputs can't cause buffer overflow
            Pfreq = 20000;
        }
        else if (value < 1)
        {
            Pfreq = 1;
        }
        else
        {
            Pfreq = value;
        }
        break;
    case Ring_Stereo:
        if (value > 1)
            value = 1;
        Pstereo = value;
        break;
    case Ring_Sine:
        Psin = value;
        sin = (float) Psin / 100.0;
        setscale();
        break;
    case Ring_Triangle:
        Ptri = value;
        tri = (float) Ptri / 100.0;
        setscale();
        break;
    case Ring_Saw:
        Psaw = value;
        saw = (float) Psaw / 100.0;
        setscale();
        break;
    case Ring_Square:
        Psqu = value;
        squ = (float) Psqu / 100.0;
        setscale();
        break;
    case Ring_Input:
        Pinput = value;
        break;
    case Ring_Auto_Freq:
        Pafreq = value;
        break;
    default:
        return;
    }
}

int
Ring::getpar(int npar)
{
    switch (npar)
    {
    case Ring_DryWet:
        return (Pvolume);

    case Ring_Pan:
        return (Ppanning);

    case Ring_LR_Cross:
        return (Plrcross);

    case Ring_Level:
        return (Plevel);

    case Ring_Depth:
        return (Pdepthp);

    case Ring_Freq:
        return (Pfreq);

    case Ring_Stereo:
        return (Pstereo);

    case Ring_Sine:
        return (Psin);

    case Ring_Triangle:
        return (Ptri);

    case Ring_Saw:
        return (Psaw);

    case Ring_Square:
        return (Psqu);

    case Ring_Input:
        return (Pinput);

    case Ring_Auto_Freq:
        return (Pafreq);

    }
    return (0); //in case of bogus parameter number
}

