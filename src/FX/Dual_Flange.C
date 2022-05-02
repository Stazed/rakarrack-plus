/*
  Rakarrack Audio FX

  Dual_Flange.C - Super Flanger
  Copyright (C) 2010 Ryan Billing
  Authors:
  Ryan Billing (a.k.a Transmogrifox)  --  Signal Processing
  Copyright (C) 2010 Ryan Billing

  Nasca Octavian Paul -- Remnants of ZynAddSubFX Echo.h structure and utility routines common to ZynSubFX source
  Copyright (C) 2002-2005 Nasca Octavian Paul

  Higher intensity flanging accomplished by picking two points out of the delay line to create a wider notch filter.

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
#include "Dual_Flange.h"

Dflange::Dflange(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    Pwetdry(),
    Ppanning(),
    Plrcross(),
    Pdepth(),
    Pwidth(),
    Poffset(),
    Pfb(),
    Phidamp(),
    Psubtract(),
    Pzero(),
    Pintense(),
    wet(0.5f),
    dry(0.5f),
    lpan(1.0f),
    rpan(1.0f),
    flrcross(0.5f),
    frlcross(0.5f),
    fdepth(50),
    fwidth(800),
    foffset(),
    ffb(),
    fhidamp(1.0f),
    fsubtract(0.5f),
    logmax(logf(1000.0f) / logf(2.0f)),
    maxx_delay((int) sample_rate * 0.055f),
    kl(),
    kr(),
    zl(),
    zr(),
    zcenter((int) fSAMPLE_RATE / floorf(0.5f * (fdepth + fwidth))),
    l(),
    r(),
    ldl(),
    rdl(),
    zdr(),
    zdl(),
    rflange0(),
    rflange1(),
    lflange0(),
    lflange1(),
    oldrflange0(),
    oldrflange1(),
    oldlflange0(),
    oldlflange1(),
    period_const(),
    base(7.0f),         // sets curve of modulation to frequency relationship
    ibase(1.0f / base),
    ldelay(NULL),
    rdelay(NULL),
    zldelay(NULL),
    zrdelay(NULL),
    oldl(),
    oldr(),
    rsA(),
    rsB(),
    lsA(),
    lsB(),
    ldelayline0(NULL),
    rdelayline0(NULL),
    ldelayline1(NULL),
    rdelayline1(NULL),
    lfo(NULL)
{
    ldelay = new float[maxx_delay];
    rdelay = new float[maxx_delay];
    zldelay = new float[maxx_delay];
    zrdelay = new float[maxx_delay];

    ldelayline0 = new delayline(0.055f, 2, sample_rate);
    rdelayline0 = new delayline(0.055f, 2, sample_rate);
    ldelayline1 = new delayline(0.055f, 2, sample_rate);
    rdelayline1 = new delayline(0.055f, 2, sample_rate);
    ldelayline0 -> set_averaging(0.05f);
    rdelayline0 -> set_averaging(0.05f);
    ldelayline0->set_mix(0.5f);
    rdelayline0->set_mix(0.5f);
    ldelayline1 -> set_averaging(0.05f);
    rdelayline1 -> set_averaging(0.05f);
    ldelayline1->set_mix(0.5f);
    rdelayline1->set_mix(0.5f);

    lfo = new EffectLFO(sample_rate);
    setpreset(Ppreset);
    cleanup();
}

Dflange::~Dflange()
{
    delete[] ldelay;
    delete[] rdelay;
    delete[] zldelay;
    delete[] zrdelay;
    delete ldelayline0;
    delete rdelayline0;
    delete ldelayline1;
    delete rdelayline1;
    delete lfo;
}

/*
 * Cleanup the effect
 */
void
Dflange::cleanup()
{
    for (int i = 0; i < maxx_delay; i++)
    {
        ldelay[i] = 0.0;
        rdelay[i] = 0.0;
        zldelay[i] = 0.0;
        zrdelay[i] = 0.0;
    }

    //loop variables
    l = 0.0f;
    r = 0.0f;
    ldl = 0.0f;
    rdl = 0.0f;
    zdr = 0.0f;
    zdl = 0.0f;
    rflange0 = 0.0f;
    lflange0 = 0.0f;
    rflange1 = 0.0f;
    lflange1 = 0.0f;
    oldrflange0 = 0.0f;
    oldrflange1 = 0.0f;
    oldlflange0 = 0.0f;
    oldlflange1 = 0.0f;
    kl = kr = zl = zr = 0;
    oldl = oldr = 0.0f;
    rsA = rsB = lsA = lsB = 0.0f;
}

#ifdef LV2_SUPPORT
void
Dflange::lv2_update_params(uint32_t period)
{
    PERIOD = period;
    lfo->updateparams(period);
}
#endif // LV2

void
Dflange::set_random_parameters()
{
    for(int i = 0; i < C_DFLANGE_PARAMETERS; i++)
    {
        switch(i)
        {
            case DFlange_Pan:
            case DFlange_Feedback:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case DFlange_DryWet:
            case DFlange_LR_Cross:
            case DFlange_LFO_Stereo:
            case DFlange_LFO_Random:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case DFlange_Depth:
            {
                int value = (int) (RND * 2480);
                changepar (i, value + 20);
            }
            break;

            case DFlange_Width:
            {
                int value = (int) (RND * 6001);
                changepar (i, value);
            }
            break;

            case DFlange_Offset:
            {
                int value = (int) (RND * 101);
                changepar (i, value);
            }
            break;

            case DFlange_LPF:
            {
                int value = (int) (RND * 19980);
                changepar (i, value + 20);
            }
            break;

            case DFlange_Subtract:
            case DFlange_Zero:
            case DFlange_Intense:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case DFlange_LFO_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case DFlange_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Dflange::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_DFLANGE_PARAMETERS; i++)
    {
        switch(i)
        {
            case DFlange_Pan:
            case DFlange_LR_Cross:
            case DFlange_Depth:
            case DFlange_Width:
            case DFlange_Offset:
            case DFlange_Feedback:
            case DFlange_LPF:
            case DFlange_Subtract:
            case DFlange_Zero:
            case DFlange_LFO_Tempo:
            case DFlange_LFO_Stereo:
            case DFlange_LFO_Type:
            case DFlange_LFO_Random:
            case DFlange_Intense:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), dflange_parameters[i * 3 + 1], dflange_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  DFlange_Intense)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case DFlange_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( DFlange_DryWet )), dflange_parameters[i * 3 + 1], dflange_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( DFlange_DryWet )) );
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
Dflange::out(float * efxoutl, float * efxoutr)
{
    //deal with LFO's
    period_const = 1.0f / (float) PERIOD;

    float lfol, lfor;
    lfo->effectlfoout(&lfol, &lfor);
    float lmod = lfol;
    
    float rmod; // initialize o.k.
    if (Pzero && Pintense)
    {
        rmod = 1.0f - lfol; //using lfol is intentional
    }
    else
    {
        rmod = lfor;
    }
    
    bool have_nans = false;

    if (Pintense)
    {
        //do intense stuff
        float lmodfreq = (f_pow2(lmod * lmod * logmax)) * fdepth; //2^x type sweep for musical interpretation of moving delay line.
        float rmodfreq = (f_pow2(rmod * rmod * logmax)) * fdepth; //logmax depends on depth
        rflange0 = 0.5f / rmodfreq; //Turn the notch frequency into 1/2 period delay
        rflange1 = rflange0 + (1.0f - foffset) / fdepth; //Set relationship of second delay line
        lflange0 = 0.5f / lmodfreq;
        lflange1 = lflange0 + (1.0f - foffset) / fdepth;

        float rx0 = (rflange0 - oldrflange0) * period_const; //amount to add each time around the loop.  Less processing of linear LFO interp inside the loop.
        float rx1 = (rflange1 - oldrflange1) * period_const;
        float lx0 = (lflange0 - oldlflange0) * period_const;
        float lx1 = (lflange1 - oldlflange1) * period_const;

        // Now there is a fractional amount to add
        float drA = oldrflange0;
        float drB = oldrflange1;
        float dlA = oldlflange0;
        float dlB = oldlflange1;

        oldrflange0 = rflange0;
        oldrflange1 = rflange1;
        oldlflange0 = lflange0;
        oldlflange1 = lflange1;
        //lfo ready...

        if (Pzero)
        {
            for (unsigned i = 0; i < PERIOD; i++)
            {
                ldl = efxoutl[i] * lpan + ldl * ffb;
                rdl = efxoutr[i] * rpan + rdl * ffb;

                //LowPass Filter
                ldl = ldl * (1.0f - fhidamp) + oldl * fhidamp;
                rdl = rdl * (1.0f - fhidamp) + oldr * fhidamp;
                oldl = ldl + DENORMAL_GUARD;
                oldr = rdl + DENORMAL_GUARD;
                /*
                Here do the delay line stuff
                basically,
                dl1(dl2(smps));
                ^^This runs two flangers in series so you can get a double notch
                 */

                ldl = ldelayline0->delay(ldl, dlA, 0, 1, 0) + ldelayline1->delay(ldl, drA, 0, 1, 0);
                rdl = rdelayline0->delay(rdl, dlB, 0, 1, 0) + rdelayline1->delay(rdl, drB, 0, 1, 0);

                efxoutl[i] = ldl = ldl * flrcross + rdl * frlcross;
                efxoutr[i] = rdl = rdl * flrcross + ldl * frlcross;

                if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
                {
                    efxoutl[i] = efxoutr[i] = ldl = rdl = 0.0;
                    have_nans = true;
                }

                // Increment LFO
                drA += rx0;
                drB += rx1;
                dlA += lx0;
                dlB += lx1;
            }
        }
        else
        {
            for (unsigned i = 0; i < PERIOD; i++)
            {
                ldl = efxoutl[i] * lpan + ldl * ffb;
                rdl = efxoutr[i] * rpan + rdl * ffb;

                //LowPass Filter
                ldl = ldl * (1.0f - fhidamp) + oldl * fhidamp;
                rdl = rdl * (1.0f - fhidamp) + oldr * fhidamp;
                oldl = ldl + DENORMAL_GUARD;
                oldr = rdl + DENORMAL_GUARD;
                /*
                Here do the delay line stuff
                basically,
                dl1(dl2(smps));
                ^^This runs two flangers in series so you can get a double notch
                 */

                ldl = ldelayline0->delay(ldl, dlA, 0, 1, 0);
                ldl = ldelayline1->delay(ldl, dlB, 0, 1, 0);

                rdl = rdelayline0->delay(rdl, drA, 0, 1, 0);
                rdl = rdelayline1->delay(rdl, drB, 0, 1, 0);

                efxoutl[i] = ldl = ldl * flrcross + rdl * frlcross;
                efxoutr[i] = rdl = rdl * flrcross + ldl * frlcross;

                if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
                {
                    efxoutl[i] = efxoutr[i] = ldl = rdl = 0.0;
                    have_nans = true;
                }

                // Increment LFO
                drA += rx0;
                drB += rx1;
                dlA += lx0;
                dlB += lx1;
            }
        }
    }
    else
    {
        float lmodfreq = fdepth + fwidth * (powf(base, lmod) - 1.0f) * ibase; //sets frequency of lowest notch. // 20 <= fdepth <= 4000 // 20 <= width <= 16000 //
        float rmodfreq = fdepth + fwidth * (powf(base, rmod) - 1.0f) * ibase;

        if (lmodfreq > 10000.0f)
        {
            lmodfreq = 10000.0f;
        }
        else if (lmodfreq < 10.0f)
        {
            lmodfreq = 10.0f;
        }
        
        if (rmodfreq > 10000.0)
        {
            rmodfreq = 10000.0f;
        }
        else if (rmodfreq < 10.0f)
        {
            rmodfreq = 10.0f;
        }

        rflange0 = fSAMPLE_RATE * 0.5f / rmodfreq; //Turn the notch frequency into a number for delay
        rflange1 = rflange0 * foffset; //Set relationship of second delay line
        lflange0 = fSAMPLE_RATE * 0.5f / lmodfreq;
        lflange1 = lflange0 * foffset;

        //now is a delay expressed in number of samples.  Number here
        //will be fractional, but will use linear interpolation inside the loop to make a decent guess at
        //the numbers between samples.

        float rx0 = (rflange0 - oldrflange0) * period_const; //amount to add each time around the loop.  Less processing of linear LFO interp inside the loop.
        float rx1 = (rflange1 - oldrflange1) * period_const;
        float lx0 = (lflange0 - oldlflange0) * period_const;
        float lx1 = (lflange1 - oldlflange1) * period_const;
        // Now there is a fractional amount to add

        float drA = oldrflange0;
        float drB = oldrflange1;
        float dlA = oldlflange0;
        float dlB = oldlflange1;
        // dr, dl variables are the LFO inside the loop.

        oldrflange0 = rflange0;
        oldrflange1 = rflange1;
        oldlflange0 = lflange0;
        oldlflange1 = lflange1;
        //lfo ready...

        for (unsigned i = 0; i < PERIOD; i++)
        {
            //Delay line utility
            ldl = ldelay[kl];
            rdl = rdelay[kr];
            l = ldl * flrcross + rdl * frlcross;
            r = rdl * flrcross + ldl * frlcross;
            ldl = l;
            rdl = r;
            ldl = efxoutl[i] * lpan - ldl * ffb;
            rdl = efxoutr[i] * rpan - rdl * ffb;

            //LowPass Filter
            ldelay[kl] = ldl = ldl * (1.0f - fhidamp) + oldl * fhidamp;
            rdelay[kr] = rdl = rdl * (1.0f - fhidamp) + oldr * fhidamp;
            oldl = ldl + DENORMAL_GUARD;
            oldr = rdl + DENORMAL_GUARD;

            if (Pzero)
            {
                //Offset zero reference delay
                zdl = zldelay[zl];
                zdr = zrdelay[zr];
                zldelay[zl] = efxoutl[i];
                zrdelay[zr] = efxoutr[i];
                
                if (--zl < 0) //Cycle delay buffer in reverse so delay time can be indexed directly with addition
                    zl = zcenter;
                
                if (--zr < 0)
                    zr = zcenter;
            }

            //End delay line management, start flanging:

            //Right Channel, delay A
            float rdif0 = drA - floor(drA);
            int tmp0 = (kr + (int) floor(drA)) % maxx_delay;
            int tmp1 = tmp0 + 1;
            
            if (tmp1 >= maxx_delay) tmp1 = 0;
            //rsA = rdelay[tmp0] + rdif0 * (rdelay[tmp1] - rdelay[tmp0] );	//here is the first right channel delay
            rsA = rdelay[tmp1] + rdif0 * (rdelay[tmp0] - rsA); //All-pass interpolator

            //Right Channel, delay B
            float rdif1 = drB - floor(drB);
            tmp0 = (kr + (int) floor(drB)) % maxx_delay;
            tmp1 = tmp0 + 1;
            
            if (tmp1 >= maxx_delay) tmp1 = 0;
            //rsB = rdelay[tmp0] + rdif1 * (rdelay[tmp1] - rdelay[tmp0]);	//here is the second right channel delay
            rsB = rdelay[tmp1] + rdif1 * (rdelay[tmp0] - rsB);

            //Left Channel, delay A
            float ldif0 = dlA - floor(dlA);
            tmp0 = (kl + (int) floor(dlA)) % maxx_delay;
            tmp1 = tmp0 + 1;
            
            if (tmp1 >= maxx_delay) tmp1 = 0;
            //lsA = ldelay[tmp0] + ldif0 * (ldelay[tmp1] - ldelay[tmp0]);	//here is the first left channel delay
            lsA = ldelay[tmp1] + ldif0 * (ldelay[tmp0] - lsA);

            //Left Channel, delay B
            float ldif1 = dlB - floor(dlB);
            tmp0 = (kl + (int) floor(dlB)) % maxx_delay;
            tmp1 = tmp0 + 1;
            
            if (tmp1 >= maxx_delay) tmp1 = 0;
            //lsB = ldelay[tmp0] + ldif1 * (ldelay[tmp1] - ldelay[tmp0]);	//here is the second left channel delay
            lsB = ldelay[tmp1] + ldif1 * (ldelay[tmp0] - lsB);
            //End flanging, next process outputs

            if (Pzero)
            {
                efxoutl[i] = dry * efxoutl[i] + fsubtract * wet * (fsubtract * (lsA + lsB) + zdl); // Make final FX out mix
                efxoutr[i] = dry * efxoutr[i] + fsubtract * wet * (fsubtract * (rsA + rsB) + zdr);
            }
            else
            {
                efxoutl[i] = dry * efxoutl[i] + wet * fsubtract * (lsA + lsB); // Make final FX out mix
                efxoutr[i] = dry * efxoutr[i] + wet * fsubtract * (rsA + rsB);
            }
            
            if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
            {
                efxoutl[i] = ldl = lsA = lsB = zdl = 0.0;
                efxoutr[i] = rdl = rsA = rsB = zdr = 0.0;
                have_nans = true;
            }

            if (--kl < 0) //Cycle delay buffer in reverse so delay time can be indexed directly with addition
                kl = maxx_delay - 1; // -1 or out of range since array is maxx_delay size
            
            if (--kr < 0)
                kr = maxx_delay - 1; // -1 or out of range since array is maxx_delay size

            // Increment LFO
            drA += rx0;
            drB += rx1;
            dlA += lx0;
            dlB += lx1;

        } //end for loop
    } //end intense if statement
    
    if(have_nans)
        cleanup();
}

void
Dflange::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */

void 
Dflange::setvolume(int value)
{
    Pwetdry = value;
    dry = (float) Pwetdry / 127.0f;
    wet = 1.0f - dry;

    if (Psubtract)
    {
        ldelayline0->set_mix(-dry);
        rdelayline0->set_mix(-dry);
        ldelayline1->set_mix(-dry);
        rdelayline1->set_mix(-dry);
    }
    else
    {
        ldelayline0->set_mix(dry);
        rdelayline0->set_mix(dry);
        ldelayline1->set_mix(dry);
        rdelayline1->set_mix(dry);
    }
}

void
Dflange::setpreset(int npreset)
{
    const int PRESET_SIZE = C_DFLANGE_PARAMETERS;
    const int NUM_PRESETS = 8;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Dual Flange 1
        {32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0},
        //Flange-Wha
        {64, 0, 64, 500, 3000, 50, -40, 8000, 1, 0, 196, 96, 0, 0, 0},
        //FbFlange
        {64, 0, 64, 68, 75, 50, -50, 8000, 0, 1, 23, 96, 5, 0, 0},
        //SoftFlange
        {32, 0, 64, 60, 10, 100, 20, 16000, 0, 0, 16, 90, 4, 0, 0},
        //Flanger
        {32, 0, 64, 170, 1200, 50, 0, 16000, 1, 0, 68, 127, 0, 0, 0},
        //Chorus 1
        {49, 0, 0, 42, 12, 50, -10, 1500, 0, 0, 120, 0, 0, 20, 0},
        //Chorus 2
        {24, 0, 0, 35, 9, 67, 12, 4700, 1, 1, 160, 75, 0, 60, 0},
        //Dual Flange 2
        {32, 0, 64, 20, 4, 4, 0, 2000, 0, 0, 14, 64, 1, 0, 1}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_DUAL_FLANGE, npreset - NUM_PRESETS + 1, pdata);
        
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
Dflange::changepar(int npar, int value)
{
    switch (npar)
    {
    case DFlange_DryWet:
        setvolume(value);
        break;
    case DFlange_Pan:
        Ppanning = value;
        if (value < 0)
        {
            rpan = 1.0f + (float) Ppanning / 64.0;
            lpan = 1.0f;
        }
        else
        {
            lpan = 1.0f - (float) Ppanning / 64.0;
            rpan = 1.0f;
        };
        break;
    case DFlange_LR_Cross:
        Plrcross = value;
        flrcross = (float) Plrcross / 127.0;
        frlcross = 1.0f - flrcross; //keep this out of the DSP loop
        break;
    case DFlange_Depth:
        Pdepth = value;
        fdepth = (float) Pdepth;
        zcenter = (int) fSAMPLE_RATE / floor(0.5f * (fdepth + fwidth));
        logmax = logf((fdepth + fwidth) / fdepth) / LOG_2;
        break;
    case DFlange_Width:
        Pwidth = value;
        fwidth = (float) Pwidth;
        zcenter = (int) fSAMPLE_RATE / floor(0.5f * (fdepth + fwidth));
        logmax = logf((fdepth + fwidth) / fdepth) / LOG_2;
        break;
    case DFlange_Offset:
        Poffset = value;
        foffset = 0.5f + (float) Poffset / 255.0;
        break;
    case DFlange_Feedback:
        Pfb = value;
        ffb = (float) Pfb / 64.5f;
        break;
    case DFlange_LPF:
        Phidamp = value;
        fhidamp = f_exp(-D_PI * (float) Phidamp / fSAMPLE_RATE);
        break;
    case DFlange_Subtract:
        Psubtract = value;
        fsubtract = 0.5f;
        if (Psubtract)
        {
            fsubtract = -0.5f; //In loop a mult by 0.5f is necessary, so this kills 2 birds with 1...
            ldelayline0->set_mix(-dry);
            rdelayline0->set_mix(-dry);
            ldelayline1->set_mix(-dry);
            rdelayline1->set_mix(-dry);
        }
        break;
    case DFlange_Zero:
        Pzero = value;
        break;
    case DFlange_LFO_Tempo:
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case DFlange_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case DFlange_LFO_Type:
        lfo->PLFOtype = value;
        lfo->updateparams(PERIOD);
        break;
    case DFlange_LFO_Random:
        lfo->Prandomness = value;
        lfo->updateparams(PERIOD);
        break;
    case DFlange_Intense:
        Pintense = value;
        break;
    default:
        return;
    }
}

int
Dflange::getpar(int npar)
{
    switch (npar)
    {
    case DFlange_DryWet:
        return (Pwetdry);

    case DFlange_Pan:
        return (Ppanning);

    case DFlange_LR_Cross:
        return (Plrcross);

    case DFlange_Depth:
        return (Pdepth);

    case DFlange_Width:
        return (Pwidth);

    case DFlange_Offset:
        return (Poffset);

    case DFlange_Feedback:
        return (Pfb);

    case DFlange_LPF:
        return (Phidamp);

    case DFlange_Subtract:
        return (Psubtract);

    case DFlange_Zero:
        return (Pzero);

    case DFlange_LFO_Tempo:
        return (lfo->Pfreq);

    case DFlange_LFO_Stereo:
        return (lfo->Pstereo);

    case DFlange_LFO_Type:
        return (lfo->PLFOtype);

    case DFlange_LFO_Random:
        return (lfo->Prandomness);

    case DFlange_Intense:
        return Pintense;

    }
    return (0); //in case of bogus parameter number
}
