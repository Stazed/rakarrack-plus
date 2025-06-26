/*

  Vocoder.C - Vocoder effect
  Author: Ryam Billing & Josep Andreu

  Adapted effect structure of ZynAddSubFX - a software synthesizer
  Author: Nasca Octavian Paul

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
#include "Vocoder.h"

Vocoder::Vocoder(float *auxresampled_, int bands, int DS, int uq, int dq,
                 double sample_rate, uint32_t intermediate_bufsize) :
    Effect(auxresampled_, bands, DS, uq, dq, sample_rate, intermediate_bufsize),
    vulevel(0.0f),
    auxresampled(auxresampled_),
    VOC_BANDS(bands),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    DS_state(DS),
    Pvolume(50),
    Ppanning(64),
    Plrcross(100),
    Plevel(),
    Pinput(0),
    Pband(),
    Pmuffle(10),
    Pqq(),
    Pring(),
    nPERIOD(),
    nSAMPLE_RATE(),
    nRATIO(),
    ncSAMPLE_RATE(),
    nfSAMPLE_RATE(),
    u_up(),
    u_down(),
    ringworm(),
    lpanning(),
    rpanning(),
    input(dB2rap(75.0f * (float) Pinput / 127.0f - 40.0f)),
    level(),
    alpha(),
    beta(),
    prls(),
    gate(0.005f),
    compeak(),
    compg(),
    compenv(),
    oldcompenv(),
    calpha(),
    cbeta(),
    cthresh(0.25f),
    cratio(0.25f),
    cpthresh(cthresh),      //  dynamic threshold
    tmpl(NULL),
    tmpr(NULL),
    tsmpsl(NULL),
    tsmpsr(NULL),
    tmpaux(NULL),
    filterbank(NULL),
    vhp(NULL),
    vlp(NULL),
    interpbuf(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    A_Resample(NULL)
{
    adjust(DS, sample_rate);

    /* These must be set after adjust() is run for ncSAMPLE_RATE */
    float tmp = 0.01f;          //  10 ms decay time on peak detectors
    alpha = ncSAMPLE_RATE / (ncSAMPLE_RATE + tmp);
    beta = 1.0f - alpha;
    prls = beta;
    
    tmp = 0.05f;                // 50 ms att/rel on compressor
    calpha = ncSAMPLE_RATE / (ncSAMPLE_RATE + tmp);
    cbeta = 1.0f - calpha;
    
    initialize();
    
    A_Resample = new Resample(uq);
    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq);

    setbands(VOC_BANDS, 200.0f, 4000.0f);
    init_filters();
    Vocoder::setpreset(Ppreset);
}

Vocoder::~Vocoder()
{
    clear_initialize();

    delete A_Resample;
    delete U_Resample;
    delete D_Resample;
}

/*
 * Cleanup the effect
 */
void
Vocoder::cleanup()
{
    for (int k = 0; k < VOC_BANDS; k++)
    {
        filterbank[k].l->cleanup();
        filterbank[k].r->cleanup();
        filterbank[k].aux->cleanup();
        filterbank[k].speak = 0.0f;
        filterbank[k].gain = 0.0f;
        filterbank[k].oldgain = 0.0f;
    }

    vhp->cleanup();
    vlp->cleanup();

    compeak = compg = compenv = oldcompenv = 0.0f;
}

std::vector<int>
Vocoder::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_VOCODER_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Vocoder::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_VOCODER_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Vocoder::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    adjust(DS_state, fSAMPLE_RATE);
    clear_initialize();
    initialize();
    setbands(VOC_BANDS, 200.0f, 4000.0f);
    init_filters();
    adjustq(Pqq);
}
#endif // LV2

void
Vocoder::set_random_parameters()
{
    for(int i = 0; i < C_VOCODER_PARAMETERS; i++)
    {
        switch(i)
        {
            case Vocoder_DryWet:
            case Vocoder_Input:
            case Vocoder_Level:
            case Vocoder_Ring:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Vocoder_Pan:
            {
                int value = (int) (RND * 129);
                changepar (i, value);
            }
            break;

            case Vocoder_Smear:
            {
                int value = (int) (RND * 127);
                changepar (i, value + 1);
            }
            break;

            case Vocoder_Q:
            {
                int value = (int) (RND * 130);
                changepar (i, value + 40);
            }
            break;
        }
    }
}

void
Vocoder::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_VOCODER_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Vocoder_Smear:
            case Vocoder_Q:
            case Vocoder_Input:
            case Vocoder_Level:
            case Vocoder_Ring:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), vocoder_parameters[i * 3 + 1], vocoder_parameters[i * 3 + 2]);
                }
                else
                {
                    // not used NON-MIXER
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Vocoder_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Vocoder_DryWet )), vocoder_parameters[i * 3 + 1], vocoder_parameters[i * 3 + 2]);
                }
                else
                {
                    // not used NON-MIXER
                }
            }
            break;

            // Offset
            case Vocoder_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Vocoder_Pan ) - 64, vocoder_parameters[i * 3 + 1], vocoder_parameters[i * 3 + 2]);
                }
                else
                {
                    // not used NON-MIXER
                }
            }
            break;
        }
    }
}

void
Vocoder::initialize()
{
    filterbank = static_cast<fbank *> (malloc(sizeof (fbank) * VOC_BANDS));
    tmpl = (float *) malloc(sizeof (float) * nPERIOD);
    tmpr = (float *) malloc(sizeof (float) * nPERIOD);
    tsmpsl = (float *) malloc(sizeof (float) * nPERIOD);
    tsmpsr = (float *) malloc(sizeof (float) * nPERIOD);
    tmpaux = (float *) malloc(sizeof (float) * nPERIOD);

    memset(tmpl, 0, sizeof (float)*nPERIOD);
    memset(tmpr, 0, sizeof (float)*nPERIOD);
    memset(tsmpsl, 0, sizeof (float)*nPERIOD);
    memset(tsmpsr, 0, sizeof (float)*nPERIOD);
    memset(tmpaux, 0, sizeof (float)*nPERIOD);

    interpbuf = new float[PERIOD];

    for (int i = 0; i < VOC_BANDS; i++)
    {
        float center = (float) i * 20000.0f / ((float) VOC_BANDS);
        float qq = 60.0f;

        filterbank[i].l = new AnalogFilter(4, center, qq, 0, fSAMPLE_RATE, interpbuf);
        filterbank[i].l->setSR(nSAMPLE_RATE);
        filterbank[i].r = new AnalogFilter(4, center, qq, 0, fSAMPLE_RATE, interpbuf);
        filterbank[i].r->setSR(nSAMPLE_RATE);
        filterbank[i].aux = new AnalogFilter(4, center, qq, 0, fSAMPLE_RATE, interpbuf);
        filterbank[i].aux->setSR(nSAMPLE_RATE);
    }

    vlp = new AnalogFilter(2, 4000.0f, 1.0f, 1, fSAMPLE_RATE, interpbuf);
    vhp = new AnalogFilter(3, 200.0f, 0.707f, 1, fSAMPLE_RATE, interpbuf);

    vlp->setSR(nSAMPLE_RATE);
    vhp->setSR(nSAMPLE_RATE);
}

void
Vocoder::clear_initialize()
{
    free(tmpl);
    free(tmpr);
    free(tsmpsl);
    free(tsmpsr);
    free(tmpaux);

    delete[] interpbuf;

    for (int i = 0; i < VOC_BANDS; i++)
    {
        delete filterbank[i].l;
        delete filterbank[i].r;
        delete filterbank[i].aux;
    }

    free(filterbank); // don't free until after the voc bands are deleted first
    delete vlp;
    delete vhp;
}

void
Vocoder::adjust(int DS, double SAMPLE_RATE)
{
    DS_state = DS;

    switch (DS)
    {
    case 0:
        nRATIO = 1;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = SAMPLE_RATE;
        break;

    case 1:
        nRATIO = 96000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        break;


    case 2:
        nRATIO = 48000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        break;

    case 3:
        nRATIO = 44100.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        break;

    case 4:
        nRATIO = 32000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        break;

    case 5:
        nRATIO = 22050.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        break;

    case 6:
        nRATIO = 16000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        break;

    case 7:
        nRATIO = 12000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        break;

    case 8:
        nRATIO = 8000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        break;

    case 9:
        nRATIO = 4000.0f / SAMPLE_RATE;
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        break;
    }

    ncSAMPLE_RATE = 1.0f / nfSAMPLE_RATE;
    nPERIOD = lrintf((float) PERIOD * nRATIO);
    u_up = (double) nPERIOD / (double) PERIOD;
    u_down = (double) PERIOD / (double) nPERIOD;
}

/*
 * Effect output
 */
void
Vocoder::out(float * efxoutl, float * efxoutr)
{
    if (DS_state != 0)
    {
        A_Resample->mono_out(auxresampled, tmpaux, PERIOD, u_up, nPERIOD);
    }
    else
    {
        memcpy(tmpaux, auxresampled, sizeof (float)*nPERIOD);
    }

    float tmpgain = 0.0f;
    
    for (int i = 0; i < nPERIOD; i++)
    { //apply compression to auxresampled
        float auxtemp = input * tmpaux[i];

        if (auxtemp > compeak)
        {
            compeak = fabs(auxtemp);                        //  First do peak detection on the signal
        }
        
        compeak *= prls;
        compenv = cbeta * oldcompenv + calpha * compeak;    //  Next average into envelope follower
        oldcompenv = compenv;

        if (compenv > cpthresh)
        { //if envelope of signal exceeds thresh, then compress
            compg = cpthresh + cpthresh * (compenv - cpthresh) / compenv;
            cpthresh = cthresh + cratio * (compg - cpthresh); //  cpthresh changes dynamically
            tmpgain = compg / compenv;
        }
        else
        {
            tmpgain = 1.0f;
        }

        if (compenv < cpthresh)
        {
            cpthresh = compenv;
        }

        if (cpthresh < cthresh)
        {
            cpthresh = cthresh;
        }

        tmpaux[i] = auxtemp * tmpgain;

        tmpaux[i] = vlp->filterout_s(tmpaux[i]);
        tmpaux[i] = vhp->filterout_s(tmpaux[i]);
    }

    //End compression

    if (DS_state != 0)
    {
        U_Resample->out(efxoutl, efxoutr, tsmpsl, tsmpsr, PERIOD, u_up);
    }
    else
    {
        memcpy(tsmpsl, efxoutl, sizeof (float)*nPERIOD);
        memcpy(tsmpsr, efxoutr, sizeof (float)*nPERIOD);
    }

    memset(tmpl, 0, sizeof (float)*nPERIOD);
    memset(tmpr, 0, sizeof (float)*nPERIOD);

    float maxgain = 0.0f;
    
    for (int j = 0; j < VOC_BANDS; j++)
    {
        for (int i = 0; i < nPERIOD; i++)
        {
            float auxtemp = tmpaux[i];

            if (filterbank[j].speak < gate)
            {
                filterbank[j].speak = 0.0f; //gate
            }

            if (auxtemp > maxgain)
            {
                maxgain = auxtemp; //vu meter level.
            }

            auxtemp = filterbank[j].aux->filterout_s(auxtemp);

            if (fabs(auxtemp) > filterbank[j].speak)
            {
                filterbank[j].speak = fabs(auxtemp); //Leaky Peak detector
            }

            filterbank[j].speak *= prls;

            filterbank[j].gain = beta * filterbank[j].oldgain + alpha * filterbank[j].speak;
            filterbank[j].oldgain = filterbank[j].gain;


            float tempgain = (1.0f - ringworm) * filterbank[j].oldgain + ringworm*auxtemp;

            tmpl[i] += filterbank[j].l->filterout_s(tsmpsl[i]) * tempgain;
            tmpr[i] += filterbank[j].r->filterout_s(tsmpsr[i]) * tempgain;
        }
    }

    for (int i = 0; i < nPERIOD; i++)
    {
        tmpl[i] *= lpanning*level;
        tmpr[i] *= rpanning*level;
    }

    if (DS_state != 0)
    {
        D_Resample->out(tmpl, tmpr, efxoutl, efxoutr, nPERIOD, u_down);
    }
    else
    {
        memcpy(efxoutl, tmpl, sizeof (float)*nPERIOD);
        memcpy(efxoutr, tmpr, sizeof (float)*nPERIOD);
    }

    /* The auxiliary volume level meter */
    vulevel = (float) CLAMP(rap2dB(maxgain), -48.0, 15.0);
}

void
Vocoder::setbands(int numbands, float startfreq, float endfreq)
{
    float start = startfreq; //useful variables
    float endband = endfreq;
    float fnumbands = (float) numbands;
    float output[VOC_BANDS + 1];

    //calculate intermediate values
    float pwer = logf(endband / start) / log(2.0f);

    for (int k = 0; k <= VOC_BANDS; k++)
    {
        output[k] = start * f_pow2(((float) k) * pwer / fnumbands);
    }

    for (int k = 0; k < VOC_BANDS; k++)
    {
        filterbank[k].sfreq = output[k] + (output[k + 1] - output[k])*0.5f;
        filterbank[k].sq = filterbank[k].sfreq / (output[k + 1] - output[k]);

        filterbank[k].l->setfreq_and_q(filterbank[k].sfreq, filterbank[k].sq);
        filterbank[k].r->setfreq_and_q(filterbank[k].sfreq, filterbank[k].sq);
        filterbank[k].aux->setfreq_and_q(filterbank[k].sfreq, filterbank[k].sq);
    }
    Vocoder::cleanup();
}

/*
 * Parameter control
 */
void
Vocoder::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;

    if (_Pvolume == 0)
        cleanup();
}

void
Vocoder::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    rpanning = ((float) _Ppanning + 0.5f) / 127.0f;
    lpanning = 1.0f - rpanning;
}

void
Vocoder::init_filters()
{
    for (int ii = 0; ii < VOC_BANDS; ii++)
    {
        float ff = filterbank[ii].sfreq;
        float qq = filterbank[ii].sq;
        filterbank[ii].l->setfreq_and_q(ff, qq);
        filterbank[ii].r->setfreq_and_q(ff, qq);
        filterbank[ii].aux->setfreq_and_q(ff, qq);
    }
}

void
Vocoder::adjustq(int value)
{
    Pqq = value;
    float q = (float) value;

    for (int ii = 0; ii < VOC_BANDS; ii++)
    {
        filterbank[ii].l->setq(q);
        filterbank[ii].r->setq(q);
        filterbank[ii].aux->setq(q);
    }
}

void
Vocoder::setpreset(int npreset)
{
    const int PRESET_SIZE = C_VOCODER_PARAMETERS;
    const int NUM_PRESETS = 5;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Vocoder 1
        {0, 64, 10, 70, 70, 40, 0},
        //Vocoder 2
        {0, 64, 14, 80, 70, 40, 32},
        //Vocoder 3
        {0, 64, 20, 90, 70, 40, 64},
        //Vocoder 4
        {0, 64, 30, 100, 70, 40, 127},
        //Vocoder 5
        {0, 74, 10, 71, 10, 40, 50}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_VOCODER, npreset - NUM_PRESETS + 1, pdata);

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
Vocoder::changepar(int npar, int value)
{
    switch (npar)
    {
    case Vocoder_DryWet:
        setvolume(value);
        break;
    case Vocoder_Pan:
        setpanning(value);
        break;
    case Vocoder_Smear:
    {
        Pmuffle = value;
        float tmp = (float) Pmuffle;
        tmp *= 0.0001f + tmp / 64000;
        alpha = ncSAMPLE_RATE / (ncSAMPLE_RATE + tmp);
        beta = 1.0f - alpha;
        break;
    }
    case Vocoder_Q:
        adjustq(value);
        /*        Pqq = value;
                tmp = (float) value;
                adjustq(tmp);*/
        break;
    case Vocoder_Input:
        Pinput = value;
        input = dB2rap(75.0f * (float) Pinput / 127.0f - 40.0f);
        break;
    case Vocoder_Level:
        Plevel = value;
        level = dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f);
        break;

    case Vocoder_Ring:
        Pring = value;
        ringworm = (float) Pring / 127.0f;
        break;

    default:
        return;
    }
}

int
Vocoder::getpar(int npar)
{
    switch (npar)
    {
    case Vocoder_DryWet:
        return (Pvolume);

    case Vocoder_Pan:
        return (Ppanning);

    case Vocoder_Smear:
        return (Pmuffle);

    case Vocoder_Q:
        return (Pqq);

    case Vocoder_Input:
        return (Pinput);

    case Vocoder_Level:
        return (Plevel);

    case Vocoder_Ring:
        return (Pring);

    }
    return (0); //in case of bogus parameter number
}
