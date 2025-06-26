/*
  ZynAddSubFX - a software synthesizer

  Rakarrack Guitar Effects Processor

  InfiniT : Infinitely rising & falling filter effect
  Author:  Ryan Billing (aka Transmogrifox)
  Copyright (C) 2010

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

#include <math.h>
#include "Infinity.h"
#include <stdio.h>

Infinity::Infinity(double sample_rate, uint32_t intermediate_bufsize) :
    Effect(sample_rate, intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    Pvolume(64),
    Pb(),
    Pq(30),
    Pstartfreq(5),
    Pendfreq(80),
    Prate(2),
    Pstdf(),
    Psubdiv(32),
    Pautopan(),
    Preverse(),
    Pstages(),
    rbandstate(),
    lbandstate(),
    rphaser(),
    lphaser(),
    phaserfb(),
    cfconst(),
    fconst(),
    rampconst(1.0f),
    irampconst(),
    crampconst(),
    cirampconst(),
    alpha(),
    beta(),
    fstart(),
    fend(),
    linconst(),
    logconst(),
    minlevel(),
    maxlevel(),
    frmin(),
    frmax(),
    qq(),
    stdiff(),
    volmaster(0.25),
    autopan(),
    msin(),
    dsin(),
    mcos(),
    mconst(),
    ratescale(1.0f),
    tflag(),
    interpbuf(NULL),
    filterl(),
    filterr()
{
    Ppreset = 2;

    initialize();

    float dt = 1.0f / fSAMPLE_RATE;
    alpha = dt / (0.5f + dt); //200ms time constant on parameter change -- quick but not jerky
    beta = 1.0f - alpha;

    Infinity::setpreset(Ppreset);
    adjustfreqs();
    reinitfilter();
}

Infinity::~Infinity()
{
    clear_initialize();
}

float inline
Infinity::phaser(phasevars *pstruct, float fxn, int j)
{
    float xn = fxn + DENORMAL_GUARD;

    for (int k = 0; k < Pstages; k++)
    {
        pstruct[j].yn1[k] = pstruct[j].xn1[k] - pstruct[j].gain * (xn + pstruct[j].yn1[k]);
        //pstruct[j].yn1[k] += DENORMAL_GUARD;
        pstruct[j].xn1[k] = fxn;
        xn = pstruct[j].yn1[k];
    }
    
    pstruct[j].yn1[0] -= phaserfb*fxn;

    return fxn;
}

void inline
Infinity::oscillator()
{
    //master oscillator
    msin += mconst*mcos;
    mcos -= msin*mconst;

    //introduce doppler effect
    if (Pstages < 9)
    {
        //smooth parameters for tempo change
        rampconst = alpha * rampconst + beta*crampconst;
        irampconst = 1.0f / rampconst;
        fconst = alpha * fconst + beta*cfconst;
    }
    else
    {
        dsin = autopan * ratescale*msin;
        float ratemod = (1.0f + dsin / fSAMPLE_RATE);
        //smooth parameters for tempo change
        rampconst = alpha * rampconst + beta * crampconst*ratemod;
        irampconst = 1.0f / rampconst;
        fconst = alpha * fconst + beta * cfconst*ratemod;
    }

    for (int i = 0; i < NUM_INF_BANDS; i++)
    {
        //right
        rbandstate[i].sinp += fconst * rbandstate[i].cosp;
        rbandstate[i].cosp -= rbandstate[i].sinp*fconst;
        rbandstate[i].lfo = (1.0f + rbandstate[i].sinp); //lfo modulates filter band volume
        rbandstate[i].ramp *= rampconst; //ramp modulates filter band frequency cutoff
        
        if (rbandstate[i].ramp > maxlevel)
        {
            rbandstate[i].ramp = minlevel;
            rbandstate[i].sinp = -1.0f;
            rbandstate[i].cosp = 0.0f; //phase drift catch-all
            //printf("i: %d sin: %f lfo: %f ramp: %f\n",i,rbandstate[i].sinp, rbandstate[i].lfo, rbandstate[i].ramp);
        }
        
        if (rbandstate[i].ramp < minlevel)
        {
            rbandstate[i].ramp = maxlevel; //if it is going in reverse (rampconst < 0)
            rbandstate[i].sinp = -1.0f;
            rbandstate[i].cosp = 0.0f; //phase drift catch-all
        }
        
        rbandstate[i].vol = rbandstate[i].level * rbandstate[i].lfo;

        //left
        lbandstate[i].sinp += fconst * lbandstate[i].cosp;
        lbandstate[i].cosp -= lbandstate[i].sinp*fconst;
        lbandstate[i].lfo = (1.0f + lbandstate[i].sinp); //lfo modulates filter band volume
        
        if (Preverse)
        {
            lbandstate[i].ramp *= irampconst; //left reversed from right
        }
        else
        {
            lbandstate[i].ramp *= rampconst; //normal mode
        }
        
        if (lbandstate[i].ramp > maxlevel)
        {
            lbandstate[i].ramp = minlevel;
            lbandstate[i].sinp = -1.0f;
            lbandstate[i].cosp = 0.0f; //phase drift catch-all
            //printf("i: %d sin: %f lfo: %f ramp: %f\n",i,lbandstate[i].sinp, lbandstate[i].lfo, lbandstate[i].ramp);
        }
        
        if (lbandstate[i].ramp < minlevel)
        {
            lbandstate[i].ramp = maxlevel; //if it is going in reverse (rampconst < 0)
            lbandstate[i].sinp = -1.0f;
            lbandstate[i].cosp = 0.0f; //phase drift catch-all
        }
        
        lbandstate[i].vol = lbandstate[i].level * lbandstate[i].lfo;

        //lmodulate = linconst*f_pow2(logconst*lbandstate[i].ramp);
        //rmodulate = linconst*f_pow2(logconst*rbandstate[i].ramp);
        float lmodulate = 1.0f - 0.25f * lbandstate[i].ramp;
        float rmodulate = 1.0f - 0.25f * rbandstate[i].ramp;

        filterl[i]->directmod(lbandstate[i].ramp);
        filterr[i]->directmod(rbandstate[i].ramp);

        lphaser[i].gain = lmodulate;
        rphaser[i].gain = rmodulate;
    }
}

/*
 * Apply the effect
 */
void
Infinity::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;
    float tmpr, tmpl;   // initialize o.k.

    for (unsigned i = 0; i < PERIOD; i++)
    {
        //modulate
        oscillator();
        tmpr = tmpl = 0.0f;
        //run filter

        if (Pstages)
        {
            for (unsigned j = 0; j < NUM_INF_BANDS; j++)
            {
                tmpl += phaser(lphaser, filterl[j]->filterout_s(lbandstate[j].vol * efxoutl[i]), j);
                tmpr += phaser(rphaser, filterr[j]->filterout_s(rbandstate[j].vol * efxoutr[i]), j);
            }
        }
        else
        {
            for (unsigned j = 0; j < NUM_INF_BANDS; j++)
            {
                tmpl += filterl[j]->filterout_s(lbandstate[j].vol * efxoutl[i]);
                tmpr += filterr[j]->filterout_s(rbandstate[j].vol * efxoutr[i]);
            }
        }

        efxoutl[i] = (1.0f + autopan * mcos) * volmaster*tmpl;
        efxoutr[i] = (1.0f - autopan * mcos) * volmaster*tmpr;

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();
}

/*
 * Cleanup the effect
 */
void
Infinity::cleanup()
{
    reinitfilter();
    
    for (int i = 0; i < NUM_INF_BANDS; i++)
    {
        filterl[i]->cleanup();
        filterr[i]->cleanup();
        lphaser[i].gain = 0.5f;
        rphaser[i].gain = 0.5f;
        
        for (int j = 0; j < MAX_PHASER_STAGES; j++)
        {
            lphaser[i].yn1[j] = 0.0f;
            rphaser[i].yn1[j] = 0.0f;
            lphaser[i].xn1[j] = 0.0f;
            rphaser[i].xn1[j] = 0.0f;
        }
    }
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Infinity::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    cleanup();
    adjustfreqs();
    reinitfilter();
}
#endif // LV2

void
Infinity::set_random_parameters()
{
    for(int i = 0; i < C_INFINITY_PARAMETERS; i++)
    {
        switch(i)
        {
            case Infinity_Band_1:
            case Infinity_Band_2:
            case Infinity_Band_3:
            case Infinity_Band_4:
            case Infinity_Band_5:
            case Infinity_Band_6:
            case Infinity_Band_7:
            case Infinity_Band_8:
            case Infinity_LR_Delay:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Infinity_Resonance:
            {
                int value = (int) (RND * 2001);
                changepar (i, value - 1000);
            }
            break;

            case Infinity_DryWet:
            case Infinity_Start:
            case Infinity_End:
            case Infinity_AutoPan:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Infinity_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Infinity_Subdivision:
            {
                int value = (int) (RND * 33);
                changepar (i, value - 16);
            }
            break;

            case Infinity_Reverse:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Infinity_Stages:
            {
                int value = (int) (RND * 12);
                changepar (i, value + 1);
            }
            break;
        }
    }
}

void
Infinity::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_INFINITY_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Infinity_Band_1:
            case Infinity_Band_2:
            case Infinity_Band_3:
            case Infinity_Band_4:
            case Infinity_Band_5:
            case Infinity_Band_6:
            case Infinity_Band_7:
            case Infinity_Band_8:
            case Infinity_Resonance:
            case Infinity_Start:
            case Infinity_End:
            case Infinity_Tempo:
            case Infinity_LR_Delay:
            case Infinity_Subdivision:
            case Infinity_AutoPan:
            case Infinity_Reverse:
            case Infinity_Stages:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), inf_parameters[i * 3 + 1], inf_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Infinity_Stages )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Infinity_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Infinity_DryWet )), inf_parameters[i * 3 + 1], inf_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Infinity_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Infinity::initialize()
{
    interpbuf = new float[PERIOD];
    
    for (int i = 0; i < NUM_INF_BANDS; i++)
    {
        filterl[i] = new RBFilter(0, 80.0f, 70.0f, 1.0f, fSAMPLE_RATE, interpbuf);
        filterr[i] = new RBFilter(0, 80.0f, 70.0f, 1.0f, fSAMPLE_RATE, interpbuf);
        rbandstate[i].level = 1.0f;
        rbandstate[i].vol = 1.0f;
        lphaser[i].gain = 0.5f;
        rphaser[i].gain = 0.5f;
        
        for (int j = 0; j < MAX_PHASER_STAGES; j++)
        {
            lphaser[i].yn1[j] = 0.0f;
            rphaser[i].yn1[j] = 0.0f;
            lphaser[i].xn1[j] = 0.0f;
            rphaser[i].xn1[j] = 0.0f;
        }

        Pb[i] = 1;
    }
}

void
Infinity::clear_initialize()
{
    delete[] interpbuf;
    
    for (int i = 0; i < NUM_INF_BANDS; i++)
    {
        delete filterl[i];
        delete filterr[i];
    }
}

/*
 * Parameter control
 */

void
Infinity::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
}

void
Infinity::setq()
{
    float fq = (float) Pq;
    
    if (Pq < 0)
    {
        qq = powf(2.0f, fq / 500.0f); //q ranges down to 0.5
        volmaster = 1.0f;
    }
    else
    {
        qq = powf(2.0f, fq / 125.0f); //q can go up to 256
        volmaster = (1.0f - fq / 1500.0f) / sqrt(qq);
    }

    for (int i = 0; i < NUM_INF_BANDS; i++)
    {
        filterl[i]->setq(qq);
        filterr[i]->setq(qq);
    }
}

void
Infinity::reinitfilter()
{
    float fbandnum = (float) (NUM_INF_BANDS);
    float halfpi = -M_PI / 2.0f; //offset so rbandstate[0].sinp = -1.0 when rbandstate[0].ramp = 0;

    for (int i = 0; i < NUM_INF_BANDS; i++)
    { //get them started on their respective phases
        //right
        float idx = (float) i;
        rbandstate[i].sinp = sinf(halfpi + D_PI * idx / fbandnum);
        rbandstate[i].cosp = cosf(halfpi + D_PI * idx / fbandnum);
        rbandstate[i].ramp = linconst * powf(2.0f, logconst * idx / fbandnum);
        rbandstate[i].lfo = 0.5f * (1.0f + rbandstate[i].sinp); //lfo modulates filter band volume
        //left
        float stateconst = fmod((stdiff + idx), fbandnum);
        lbandstate[i].sinp = sinf(halfpi + D_PI * stateconst / fbandnum);
        lbandstate[i].cosp = cosf(halfpi + D_PI * stateconst / fbandnum);
        lbandstate[i].ramp = linconst * powf(2.0f, logconst * stateconst / fbandnum);
        lbandstate[i].lfo = 0.5f * (1.0f + rbandstate[i].sinp); //lfo modulates filter band volume
        //printf("i: %d sin: %f lfo: %f ramp: %f max: %f min: %f\n",i,rbandstate[i].sinp, rbandstate[i].lfo, rbandstate[i].ramp, maxlevel, minlevel);

        filterl[i]->setmix(0, 80.0f, 70.0f, 1.0f);
        filterr[i]->setmix(0, 80.0f, 70.0f, 1.0f);
        filterl[i]->setmode(1);
        filterr[i]->setmode(1);
        filterl[i]->settype(2);
        filterr[i]->settype(2); //bandpass
        filterl[i]->setq(qq);
        filterr[i]->setq(qq);
        filterl[i]->directmod(lbandstate[i].ramp);
        filterr[i]->directmod(rbandstate[i].ramp);

    }
    msin = 0.0f;
    mcos = 1.0f;
}

void
Infinity::adjustfreqs()
{
    float frate;    // initialize o.k.
    float fs = fSAMPLE_RATE;

    fstart = 20.0f + 6000.0f * ((float) Pstartfreq / 127.0f);
    fend = 20.0f + 6000.0f * ((float) Pendfreq / 127.0f);
    
    if (Psubdiv > 0)
    {
        frate = ((float) (1 + Prate)) / (((float) Psubdiv)*60.0f); //beats/second
    }
    else
    {
        frate = ((float) (1 - Psubdiv))*((float) Prate) / 60.0f;
    }

    if (fstart < fend)
    {
        frmin = fstart;
        frmax = fend;
        crampconst = 1.0f + frate * logf(frmax / frmin) / fs;
    }
    else
    {
        frmax = fstart;
        frmin = fend;
        crampconst = 1.0f / (1.0f + frate * logf(frmax / frmin) / fs);
    }

    cirampconst = 1.0f / crampconst;
    logconst = logf(frmax / frmin) / logf(2.0f);
    linconst = D_PI * frmin / fs; //these lines are for using powf() in the initialization

    minlevel = D_PI * frmin / fs;
    maxlevel = minlevel * frmax / frmin;
    //printf("min %f max %f rampconst %f irampconst %f\n", minlevel, maxlevel,rampconst, irampconst);
    //fconst =  2.0f * sinf(PI*frate / fs);  //this is a more precise interpretation
    cfconst = D_PI * frate / fs; //this seems to work well at low frequencies
    mconst = D_PI * ((float) Prate) / (fs * 60.0f * 4.0f);
}

void
Infinity::setpreset(int npreset)
{
    const int PRESET_SIZE = C_INFINITY_PARAMETERS;
    const int NUM_PRESETS = 11;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, -64, 64, -64, 64, -64, 64, -64, 0, 0, 0, 14, 0, 0, 0, 0, 4},
        //Basic
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 700, 20, 80, 60, 0, 1, 0, 0, 1},
        //Rising Comb
        {64, 64, -64, 64, -64, 64, -64, 64, -64, 600, 0, 127, 32, 0, 16, 0, 0, 1},
        //Falling Comb
        {64, 64, -64, 64, -64, 64, -64, 64, -64, 600, 127, 0, 32, 0, 16, 0, 0, 1},
        //Laser
        {0, 64, -64, 64, -64, 64, -64, 64, -64, 600, 127, 2, 70, 0, 1, 0, 0, 1},
        //Doppler
        {0, 64, -64, 64, -64, 64, -64, 64, -64, -564, 0, 127, 150, -31, 2, 99, 0, 10},
        //Freq Shifter
        {0, 64, -64, 64, -64, 64, -64, 64, -64, -564, 0, 127, 60, -48, -1, 0, 0, 10},
        //Dizzy Sailor
        {0, 64, -64, 64, -64, 64, -64, 64, -64, -564, 0, 127, 60, -48, -1, 110, 0, 10},
        //Stereo Phaser
        {42, 64, 0, 64, 0, 64, 0, 64, 0, 91, 0, 127, 60, 32, 16, 92, 1, 6},
        //Corkscrew
        {64, 64, 0, -64, 0, 64, 0, -64, 0, 120, 0, 127, 120, -16, 15, 67, 1, 4},
        //FreqeeVox
        {0, 64, -64, 64, -64, 64, -64, 64, -64, -164, 0, 127, 556, -16, -3, 0, 0, 8}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_INFINITY, npreset - NUM_PRESETS + 1, pdata);
        
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
Infinity::changepar(int npar, int value)
{
    switch (npar)
    {
    case Infinity_DryWet:
        setvolume(value);
        break;
    case Infinity_Band_1:
    case Infinity_Band_2:
    case Infinity_Band_3:
    case Infinity_Band_4:
    case Infinity_Band_5:
    case Infinity_Band_6:
    case Infinity_Band_7:
    case Infinity_Band_8:
        Pb[npar - 1] = value;
        rbandstate[npar - 1].level = (float) value / 64.0f;
        lbandstate[npar - 1].level = (float) value / 64.0f;
        break;
    case Infinity_Resonance:
        Pq = value;
        setq();
        break;
    case Infinity_Start:
        Pstartfreq = value;
        adjustfreqs();
        reinitfilter();
        break;
    case Infinity_End:
        Pendfreq = value;
        adjustfreqs();
        reinitfilter();
        break;
    case Infinity_Tempo:
        Prate = value;
        adjustfreqs();
        break;
    case Infinity_LR_Delay:
        Pstdf = value;
        stdiff = ((float) value) / 127.0f;
        reinitfilter();
        break;
    case Infinity_Subdivision:
        Psubdiv = value;
        if (value != 0) ratescale = 10.0f / ((float) abs(value));
        else ratescale = 10.0f;
        adjustfreqs();
        break;
    case Infinity_AutoPan:
        Pautopan = value;
        autopan = ((float) Pautopan) / 127.0f;
        if (autopan > 1.0f) autopan = 1.0f;
        if (autopan < 0.0f) autopan = 0.0f;
        break;
    case Infinity_Reverse:
        Preverse = value;
        adjustfreqs();
        reinitfilter();
        break;
    case Infinity_Stages:
        Pstages = value - 1;
        //        for (int i=0; i<NUM_INF_BANDS; i++)  {
        //        filterl[i]->setstages(value - 1);
        //        filterr[i]->setstages(value - 1);
        //        }
        phaserfb = 0.5f + (((float) (Pstages)) / 11.01f)*0.5f;
        break;
    default:
        return;
    }
}

int
Infinity::getpar(int npar)
{
    switch (npar)
    {
    case Infinity_DryWet:
        return (Pvolume);

    case Infinity_Band_1:
    case Infinity_Band_2:
    case Infinity_Band_3:
    case Infinity_Band_4:
    case Infinity_Band_5:
    case Infinity_Band_6:
    case Infinity_Band_7:
    case Infinity_Band_8:
        return (Pb[npar - 1]);

    case Infinity_Resonance:
        return (Pq);

    case Infinity_Start:
        return (Pstartfreq);

    case Infinity_End:
        return (Pendfreq);

    case Infinity_Tempo:
        return (Prate);

    case Infinity_LR_Delay:
        return (Pstdf);

    case Infinity_Subdivision:
        return (Psubdiv);

    case Infinity_AutoPan:
        return (Pautopan);

    case Infinity_Reverse:
        return (Preverse);

    case Infinity_Stages:
        return (Pstages + 1);

    }

    return (0);
}
