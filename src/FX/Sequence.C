/*
  Rakarrack Guitar FX

  Sequence.C - Simple compressor/Sequence effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing & Josep Andreu

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License
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
#include "Sequence.h"
#include <time.h>
#include "../EFX_common/f_sin.h"

Sequence::Sequence(long int Quality, int DS, int uq, int dq,
                   double sample_rate, uint32_t intermediate_bufsize) :
    Effect(Quality, DS, uq, dq, sample_rate, intermediate_bufsize),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    DS_state(DS),
    hq(Quality),
    Pvolume(),
    Psequence(),
    Ptempo(),
    Pq(),
    Pamplitude(),
    Pstdiff(1),
    Pmode(3),
    Prange(),
    tcount(),
    scount(0),
    dscount(0),
    intperiod(),
    subdiv(2),
    rndflag(),
    nPERIOD(),
    nSAMPLE_RATE(),
    nRATIO(),
    u_up(),
    u_down(),
    window(),
    nfSAMPLE_RATE(),
    MINFREQ(100.0f),
    MAXFREQ(10000.0f),
    fsequence(),
    fq(75.0f),
    panning(),
    ifperiod(),
    fperiod(),
    seqpower(),
    outi(NULL),
    outo(NULL),
    templ(NULL),
    tempr(NULL),
    peakpulse(),
    peak(),
    envrms(),
    peakdecay(10.0f / sample_rate),
    trigthresh(0.15f),
    trigtimeout(),
    trigtime(sample_rate / 12),         //  time to take next peak
    onset(),
    atk(200.0f / sample_rate),
    targatk(12.0f / sample_rate),       // for smoothing filter transition
    lmod(0.5f),
    rmod(0.5f),
    maxdly(4.0f),
    tempodiv(maxdly),
    fb(),
    rdlyfb(),
    ldlyfb(),
    avtime(0.25f),
    avflag(1),
    filterl(NULL),
    filterr(NULL),
    modfilterl(NULL),
    modfilterr(NULL),
    rmsfilter(NULL),
    peaklpfilter(NULL),
    peakhpfilter(NULL),
    peaklpfilter2(NULL),
    interpbuf(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    PS(NULL),
    beats(NULL),
    ldelay(NULL),
    rdelay(NULL)
{
    adjust(DS, sample_rate);

    initialize();

    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq);

    filterl->setmix(1, 0.33f, -1.0f, 0.25f);
    filterr->setmix(1, 0.33f, -1.0f, 0.25f);

    ldelay = new delayline(maxdly, 1, sample_rate);
    rdelay = new delayline(maxdly, 1, sample_rate);

    PS = new PitchShifter(window, hq, nfSAMPLE_RATE);
    PS->ratio = 1.0f;

    setpreset(Ppreset);
    cleanup();
}

Sequence::~Sequence()
{
    clear_initialize();

    delete U_Resample;
    delete D_Resample;
    delete ldelay;
    delete rdelay;
    delete PS;
}

/*
 * Cleanup the effect
 */
void
Sequence::cleanup()
{
    memset(outi, 0, sizeof (float)*nPERIOD);
    memset(outo, 0, sizeof (float)*nPERIOD);

    memset(templ, 0, sizeof (float)*PERIOD);
    memset(tempr, 0, sizeof (float)*PERIOD);

    ldelay->cleanup();
    rdelay->cleanup();
    ldelay->set_averaging(0.25f);
    rdelay->set_averaging(0.25f);
}

std::vector<int>
Sequence::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_SEQUENCE_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Sequence::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_SEQUENCE_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#ifdef LV2_SUPPORT
void
Sequence::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        adjust(DS_state, fSAMPLE_RATE);
        clear_initialize();
        initialize();
        filterl->setmix(1, 0.33f, -1.0f, 0.25f);
        filterr->setmix(1, 0.33f, -1.0f, 0.25f);
        cleanup();
    }
    else
    {
        PERIOD = period;
        adjust(DS_state, fSAMPLE_RATE);
    }
}
#endif // LV2

void
Sequence::set_random_parameters()
{
    for(int i = 0; i < C_SEQUENCE_PARAMETERS; i++)
    {
        switch(i)
        {
            case Sequence_DryWet:
            case Sequence_Step_1:
            case Sequence_Step_2:
            case Sequence_Step_3:
            case Sequence_Step_4:
            case Sequence_Step_5:
            case Sequence_Step_6:
            case Sequence_Step_7:
            case Sequence_Step_8:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Sequence_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Sequence_Amp:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Sequence_Stdf:
            case Sequence_Range:
            {
                int value = (int) (RND * 8);
                changepar (i, value);
            }
            break;

            case Sequence_Mode:
            {
                int value = (int) (RND * 9);
                changepar (i, value);
            }
            break;

            case Sequence_Resonance:
            {
                int value = (int) (RND * 129);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Sequence::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_SEQUENCE_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Sequence_Step_1:
            case Sequence_Step_2:
            case Sequence_Step_3:
            case Sequence_Step_4:
            case Sequence_Step_5:
            case Sequence_Step_6:
            case Sequence_Step_7:
            case Sequence_Step_8:
            case Sequence_Tempo:
            case Sequence_Amp:
            case Sequence_Stdf:
            case Sequence_Mode:
            case Sequence_Range:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), sequence_parameters[i * 3 + 1], sequence_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Sequence_Range )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Sequence_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Sequence_DryWet )), sequence_parameters[i * 3 + 1], sequence_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Sequence_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Sequence_Resonance:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Sequence_Resonance ) - 64, sequence_parameters[i * 3 + 1], sequence_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Sequence_Resonance ) - 64);
                    s_buf += ":";
                }
            }
            break;
        }
    }
}

void
Sequence::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);

    outi = (float *) malloc(sizeof (float) * nPERIOD);
    outo = (float *) malloc(sizeof (float) * nPERIOD);

    beats = new beattracker(fSAMPLE_RATE, PERIOD);

    interpbuf = new float[PERIOD];
    filterl = new RBFilter(0, 80.0f, 40.0f, 2, fSAMPLE_RATE, interpbuf);
    filterr = new RBFilter(0, 80.0f, 40.0f, 2, fSAMPLE_RATE, interpbuf);
    modfilterl = new RBFilter(0, 15.0f, 0.5f, 1, fSAMPLE_RATE, interpbuf);
    modfilterr = new RBFilter(0, 15.0f, 0.5f, 1, fSAMPLE_RATE, interpbuf);
    rmsfilter = new RBFilter(0, 15.0f, 0.15f, 1, fSAMPLE_RATE, interpbuf);
    peaklpfilter = new RBFilter(0, 25.0f, 0.5f, 0, fSAMPLE_RATE, interpbuf);
    peaklpfilter2 = new RBFilter(0, 25.0f, 0.5f, 0, fSAMPLE_RATE, interpbuf);
    peakhpfilter = new RBFilter(1, 45.0f, 0.5f, 0, fSAMPLE_RATE, interpbuf);
}

void
Sequence::clear_initialize()
{
    free(templ);
    free(tempr);
    free(outi);
    free(outo);

    delete beats;
    delete filterl;
    delete filterr;
    delete modfilterl;
    delete modfilterr;
    delete rmsfilter;
    delete peaklpfilter;
    delete peaklpfilter2;
    delete peakhpfilter;
    delete[] interpbuf;
}

/*
 * Effect output
 */
void
Sequence::out(float * efxoutl, float * efxoutr)
{
    if (avflag)
    {
        ldelay->set_averaging(avtime);
        rdelay->set_averaging(avtime);
        avflag = 0;
    }

    int hPERIOD = 0;
    if ((Pmode == 3) || (Pmode == 5) || (Pmode == 6))
    {
        hPERIOD = nPERIOD;
    }
    else
    {
        hPERIOD = PERIOD;
    }

    if ((rndflag) && (tcount < hPERIOD + 1))
    { //This is an Easter Egg
        srand(time(NULL));
        
        for (unsigned int i = 0; i < 8; i++)
        {
            fsequence[i] = RND1;
        }
    }

    switch (Pmode)
    {
        case 0: //Lineal
        {
            int nextcount = scount + 1;

            if (nextcount > 7)
            {
                nextcount = 0;
            }

            float ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
            float lfol = fsequence[scount];

            dscount = (scount + Pstdiff) % 8;
            int dnextcount = dscount + 1;

            if (dnextcount > 7)
            {
                dnextcount = 0;
            }

            float rdiff = ifperiod * (fsequence[dnextcount] - fsequence[dscount]);
            float lfor = fsequence[dscount];

            for (unsigned int i = 0; i < PERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    nextcount = scount + 1;

                    if (nextcount > 7)
                    {
                        nextcount = 0;
                    }

                    ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
                    lfol = fsequence[scount];

                    dscount = (scount + Pstdiff) % 8;
                    dnextcount = dscount + 1;

                    if (dnextcount > 7)
                    {
                        dnextcount = 0;
                    }

                    rdiff = ifperiod * (fsequence[dnextcount] - fsequence[dscount]);
                    lfor = fsequence[dscount];
                }

                float ftcount = (float) tcount;


                lmod = lfol + ldiff * ftcount;
                rmod = lfor + rdiff * ftcount;

                if (Pamplitude)
                {
                    float ldbl = lmod * (1.0f - cosf(D_PI * ifperiod * ftcount));
                    float ldbr = rmod * (1.0f - cosf(D_PI * ifperiod * ftcount));

                    efxoutl[i] = ldbl * efxoutl[i];
                    efxoutr[i] = ldbr * efxoutr[i];
                }

                float frl = MINFREQ + MAXFREQ*lmod;
                float frr = MINFREQ + MAXFREQ*rmod;

                if (i % 8 == 0)
                {
                    filterl->setfreq_and_q(frl, fq);
                    filterr->setfreq_and_q(frr, fq);
                }

                efxoutl[i] = filterl->filterout_s(efxoutl[i]);
                efxoutr[i] = filterr->filterout_s(efxoutr[i]);

            }
            break;
        }
        case 1: //Up Down
        {
            for (unsigned int i = 0; i < PERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    dscount = (scount + Pstdiff) % 8;
                }

                float ftcount = M_PI * ifperiod * (float) (tcount);

                lmod = sinf(ftcount) * fsequence[scount];
                rmod = sinf(ftcount) * fsequence[dscount];

                if (Pamplitude)
                {
                    float ldbl = lmod * (1.0f - cosf(2.0f * ftcount));
                    float ldbr = rmod * (1.0f - cosf(2.0f * ftcount));

                    efxoutl[i] = ldbl * efxoutl[i];
                    efxoutr[i] = ldbr * efxoutr[i];
                }

                float frl = MINFREQ + MAXFREQ*lmod;
                float frr = MINFREQ + MAXFREQ*rmod;


                if (i % 8 == 0)
                {
                    filterl->setfreq_and_q(frl, fq);
                    filterr->setfreq_and_q(frr, fq);
                }

                efxoutl[i] = filterl->filterout_s(efxoutl[i]);
                efxoutr[i] = filterr->filterout_s(efxoutr[i]);

            }

            break;
        }
        case 2: //Stepper
        {
            for (unsigned int i = 0; i < PERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }
                    dscount = (scount + Pstdiff) % 8;
                }

                lmod = fsequence[scount];
                rmod = fsequence[dscount];

                lmod = modfilterl->filterout_s(lmod);
                rmod = modfilterr->filterout_s(rmod);

                if (Pamplitude)
                {
                    float ldbl = seqpower * lmod;
                    float ldbr = seqpower * rmod;

                    efxoutl[i] = ldbl * efxoutl[i];
                    efxoutr[i] = ldbr * efxoutr[i];
                }

                float frl = MINFREQ + lmod * MAXFREQ;
                float frr = MINFREQ + rmod * MAXFREQ;


                if (i % 8 == 0)
                {
                    filterl->setfreq_and_q(frl, fq);
                    filterr->setfreq_and_q(frr, fq);
                }

                efxoutl[i] = filterl->filterout_s(efxoutl[i]);
                efxoutr[i] = filterr->filterout_s(efxoutr[i]);

            }

            break;
        }
        case 3: //Shifter
        {
            int nextcount = scount + 1;

            if (nextcount > 7)
            {
                nextcount = 0;
            }

            float ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
            float lfol = fsequence[scount];

            if (DS_state != 0)
            {
                memcpy(templ, efxoutl, sizeof (float)*PERIOD);
                memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
                U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
            }


            for (unsigned int i = 0; i < nPERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    nextcount = scount + 1;

                    if (nextcount > 7)
                    {
                        nextcount = 0;
                    }

                    ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
                    lfol = fsequence[scount];
                }

                float ftcount = (float) tcount;

                lmod = 1.0f + lfol + ldiff * ftcount;

                if (Pamplitude) lmod = 1.0f - (lfol + ldiff * ftcount) * .5f;

                outi[i] = (efxoutl[i] + efxoutr[i])*.5;

                if (outi[i] > 1.0)
                {
                    outi[i] = 1.0f;
                }

                if (outi[i] < -1.0)
                {
                    outi[i] = -1.0f;
                }
            }

            PS->ratio = lmod;
            PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

            memcpy(templ, outo, sizeof (float)*nPERIOD);
            memcpy(tempr, outo, sizeof (float)*nPERIOD);

            if (DS_state != 0)
            {
                D_Resample->out(templ, tempr, efxoutl, efxoutr, nPERIOD, u_down);
            }
            else
            {
                memcpy(efxoutl, templ, sizeof (float)*PERIOD);
                memcpy(efxoutr, tempr, sizeof (float)*PERIOD);
            }
            break;
        }
        case 4: //Tremor
        {
            int nextcount = scount + 1;

            if (nextcount > 7)
            {
                nextcount = 0;
            }

            float ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
            float lfol = fsequence[scount];

            dscount = (scount + Pstdiff) % 8;
            int dnextcount = dscount + 1;

            if (dnextcount > 7)
            {
                dnextcount = 0;
            }

            float rdiff = ifperiod * (fsequence[dnextcount] - fsequence[dscount]);
            float lfor = fsequence[dscount];

            for (unsigned int i = 0; i < PERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    nextcount = scount + 1;

                    if (nextcount > 7)
                    {
                        nextcount = 0;
                    }

                    ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
                    lfol = fsequence[scount];

                    dscount = (scount + Pstdiff) % 8;
                    dnextcount = dscount + 1;

                    if (dnextcount > 7)
                    {
                        dnextcount = 0;
                    }

                    rdiff = ifperiod * (fsequence[dnextcount] - fsequence[dscount]);
                    lfor = fsequence[dscount];
                }
                //Process Amplitude modulation
                if (Pamplitude)
                {
                    float ftcount = (float) tcount;
                    lmod = lfol + ldiff * ftcount;
                    rmod = lfor + rdiff * ftcount;

                    float ldbl = seqpower * lmod * (1.0f - cosf(D_PI * ifperiod * ftcount));
                    float ldbr = seqpower * rmod * (1.0f - cosf(D_PI * ifperiod * ftcount));

                    efxoutl[i] = ldbl * efxoutl[i];
                    efxoutr[i] = ldbr * efxoutr[i];
                }
                else
                {
                    lmod = seqpower * fsequence[scount];
                    rmod = seqpower * fsequence[dscount];
                    lmod = modfilterl->filterout_s(lmod);
                    rmod = modfilterr->filterout_s(rmod);

                    efxoutl[i] = lmod * efxoutl[i];
                    efxoutr[i] = rmod * efxoutr[i];
                }
            }
            break;
        }
        case 5: //Arpegiator
        {
            float lfol = floorf(fsequence[scount]*12.75f);

            if (DS_state != 0)
            {
                memcpy(templ, efxoutl, sizeof (float)*PERIOD);
                memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
                U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
            }

            for (unsigned int i = 0; i < nPERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    lfol = floorf(fsequence[scount]*12.75f);
                }

                lmod = powf(2.0f, lfol / 12.0f);

                if (Pamplitude) lmod = powf(2.0f, -lfol / 12.0f);

                outi[i] = (efxoutl[i] + efxoutr[i])*.5;
                if (outi[i] > 1.0)
                    outi[i] = 1.0f;
                if (outi[i] < -1.0)
                    outi[i] = -1.0f;
            }

            PS->ratio = lmod;
            PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

            memcpy(templ, outo, sizeof (float)*nPERIOD);
            memcpy(tempr, outo, sizeof (float)*nPERIOD);

            if (DS_state != 0)
            {
                D_Resample->out(templ, tempr, efxoutl, efxoutr, nPERIOD, u_down);
            }
            else
            {
                memcpy(efxoutl, templ, sizeof (float)*nPERIOD);
                memcpy(efxoutr, tempr, sizeof (float)*nPERIOD);
            }
            break;
        }
        case 6: //Chorus
        {
            int nextcount = scount + 1;

            if (nextcount > 7)
            {
                nextcount = 0;
            }

            float ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
            float lfol = fsequence[scount];

            if (DS_state != 0)
            {
                memcpy(templ, efxoutl, sizeof (float)*PERIOD);
                memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
                U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
            }

            for (unsigned int i = 0; i < nPERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    nextcount = scount + 1;

                    if (nextcount > 7)
                    {
                        nextcount = 0;
                    }

                    ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);
                    lfol = fsequence[scount];
                }

                float ftcount = (float) tcount;

                lmod = 1.0f + (lfol + ldiff * ftcount)*.03f;

                if (Pamplitude)
                {
                    lmod = 1.0f - (lfol + ldiff * ftcount)*.03f;
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
            }

            PS->ratio = lmod;
            PS->smbPitchShift(PS->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outi, outo);

            if (Pstdiff == 1)
            {
                for (unsigned int i = 0; i < nPERIOD; i++)
                {
                    templ[i] = efxoutl[i] - efxoutr[i] + outo[i];
                    tempr[i] = efxoutl[i] - efxoutr[i] + outo[i];
                }
            }
            else if (Pstdiff == 2)
            {
                for (unsigned int i = 0; i < nPERIOD; i++)
                {
                    templ[i] = outo[i]*(1.0f - panning);
                    tempr[i] = outo[i] * panning;
                }
            }
            else
            {
                memcpy(templ, outo, sizeof (float)*nPERIOD);
                memcpy(tempr, outo, sizeof (float)*nPERIOD);
            }

            if (DS_state != 0)
            {
                D_Resample->out(templ, tempr, efxoutl, efxoutr, nPERIOD, u_down);
            }
            else
            {
                memcpy(efxoutl, templ, sizeof (float)*nPERIOD);
                memcpy(efxoutr, tempr, sizeof (float)*nPERIOD);
            }
            break;
        }
        case 7: //TrigStepper
        {
            //testing beattracker object -- doesn't do anything useful yet other than a convenient place
            //to see how well it performs.
            beats->detect(efxoutl, efxoutr, PERIOD);

            float tmp = 0.0f;
            float ltarget = 0.0f;
            float rtarget = 0.0f;
            for (unsigned int i = 0; i < PERIOD; i++)
            { //Detect dynamics onset

                tmp = 10.0f * fabs(efxoutl[i] + efxoutr[i]);
                envrms = rmsfilter->filterout_s(tmp);

                if (tmp > peak)
                {
                    peak = atk + tmp;
                }

                if (envrms < peak)
                {
                    peak -= peakdecay;
                }

                if (peak < 0.0f)
                {
                    peak = 0.0f;
                }

                peakpulse = peaklpfilter2->filterout_s(fabs(peakhpfilter->filterout_s(peak)));

                if (peakpulse > trigthresh)
                {
                    if (trigtimeout == 0)
                    {
                        onset = 1;
                        trigtimeout = trigtime;
                    }
                    else
                    {
                        onset = 0;
                    }
                }
                else
                {
                    if (--trigtimeout < 0)
                    {
                        trigtimeout = 0;
                    }

                }

                if (onset)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    dscount = (scount + Pstdiff) % 8;
                }

                ltarget = fsequence[scount];
                rtarget = fsequence[dscount];

                if (lmod < ltarget)
                {
                    lmod += targatk;
                }
                else
                {
                    lmod -= targatk;
                }

                if (rmod < rtarget)
                {
                    rmod += targatk;
                }
                else
                {
                    rmod -= targatk;
                }

                ltarget = peaklpfilter->filterout_s(lmod);
                rtarget = peaklpfilter->filterout_s(rmod);

                if (Pamplitude)
                {
                    float ldbl = seqpower * ltarget;
                    float ldbr = seqpower * rtarget;

                    efxoutl[i] = ldbl * efxoutl[i];
                    efxoutr[i] = ldbr * efxoutr[i];
                }

                float frl = MINFREQ + ltarget * MAXFREQ;
                float frr = MINFREQ + rtarget * MAXFREQ;


                if (i % 8 == 0)
                {
                    filterl->setfreq_and_q(frl, fq);
                    filterr->setfreq_and_q(frr, fq);
                }

                efxoutl[i] = filterl->filterout_s(efxoutl[i]);
                efxoutr[i] = filterr->filterout_s(efxoutr[i]);

                //efxoutl[i] += triggernow;  //test to see the pulse
                //efxoutr[i] = peakpulse;
            }
            break;
        }
        case 8: //delay
        {
            for (unsigned int i = 0; i < PERIOD; i++)
            { //Maintain sequenced modulator

                if (++tcount >= intperiod)
                {
                    tcount = 0;
                    scount++;

                    if (scount > 7)
                    {
                        scount = 0; //reset to beginning of sequence buffer
                    }

                    dscount = (scount + Pstdiff) % 8;
                }

                if (Pamplitude)
                {
                    float ftcount = M_PI * ifperiod * (float) (tcount);

                    lmod = f_sin(ftcount) * fsequence[scount];
                    rmod = f_sin(ftcount) * fsequence[dscount];

                    float ldbl = lmod * (1.0f - f_cos(2.0f * ftcount));
                    float ldbr = rmod * (1.0f - f_cos(2.0f * ftcount));

                    lmod = tempodiv * fsequence[scount];
                    rmod = tempodiv * fsequence[dscount];

                    efxoutl[i] = ldbl * ldelay->delay((ldlyfb + efxoutl[i]), lmod, 0, 1, 0);
                    efxoutr[i] = ldbr * rdelay->delay((rdlyfb + efxoutr[i]), rmod, 0, 1, 0);

                }

                lmod = tempodiv * fsequence[scount];
                rmod = tempodiv * fsequence[dscount];

                efxoutl[i] = ldelay->delay_simple((ldlyfb + efxoutl[i]), lmod, 0, 1, 0);
                efxoutr[i] = rdelay->delay_simple((rdlyfb + efxoutr[i]), rmod, 0, 1, 0);

                ldlyfb = fb * efxoutl[i];
                rdlyfb = fb * efxoutr[i];

            }
            break;
        }
            // here case 9:
            //
            // break;
    }   // switch
};

/*
 * Parameter control
 */

void
Sequence::setvolume(int volume)
{
    Pvolume = volume;
    outvolume = (float) Pvolume / 127.0f;
}

void
Sequence::setranges(int value)
{
    switch (value)
    {
    case 1: //typical for wahwah pedal
        MINFREQ = 450.0f;
        MAXFREQ = 2500.0f;
        break;
    case 2:
        MINFREQ = 150.0f;
        MAXFREQ = 4000.0f;
        break;
    case 3:
        MINFREQ = 40.0f;
        MAXFREQ = 800.0f;
        break;
    case 4:
        MINFREQ = 100.0f;
        MAXFREQ = 1600.0f;
        break;
    case 5:
        MINFREQ = 80.0f;
        MAXFREQ = 16000.0f;
        break;
    case 6:
        MINFREQ = 60.0f;
        MAXFREQ = 18000.0f;
        break;
    case 7:
        MINFREQ = 40.0f;
        MAXFREQ = 2200.0f;
        break;
    case 8:
        MINFREQ = 20.0f;
        MAXFREQ = 6000.0f;
        break;
    }
}

void
Sequence::adjust(int DS, double SAMPLE_RATE)
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
Sequence::settempo(int value)
{
    if ((Pmode == 3) || (Pmode == 5) || (Pmode == 6))
    {
        fperiod = nfSAMPLE_RATE * 60.0f / (subdiv * (float) value);
    }
    else
    {
        fperiod = fSAMPLE_RATE * 60.0f / (subdiv * (float) value); //number of samples before next value
    }

    ifperiod = 1.0f / fperiod;
    intperiod = (int) fperiod;

    tempodiv = 240.0f / ((float) value);
    
    if (tempodiv > maxdly)
    {
        tempodiv = maxdly;
    }

    avtime = 60.0f / ((float) value);
    avflag = 1;
}

void
Sequence::setpreset(int npreset)
{
    const int PRESET_SIZE = C_SEQUENCE_PARAMETERS;
    const int NUM_PRESETS = 11;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Jumpy
        {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3},
        //Stair Step
        {10, 20, 30, 50, 75, 90, 100, 127, 64, 90, 96, 0, 0, 2, 5},
        //Mild
        {20, 30, 10, 40, 25, 60, 100, 50, 0, 90, 40, 0, 0, 0, 4},
        //WahWah
        {11, 55, 15, 95, 12, 76, 11, 36, 30, 80, 110, 0, 4, 1, 2},
        //Filter Pan
        {28, 59, 94, 127, 120, 80, 50, 24, 64, 180, 107, 0, 3, 0, 8},
        //Stepper
        {30, 127, 30, 50, 80, 40, 110, 80, 0, 240, 95, 1, 1, 2, 2},
        //Shifter
        {0, 0, 127, 127, 0, 0, 127, 127, 64, 114, 64, 1, 0, 3, 1},
        //Tremor
        {30, 127, 30, 50, 80, 40, 110, 80, 0, 240, 95, 1, 1, 4, 2},
        //Boogie
        {0, 40, 50, 60, 70, 60, 40, 0, 0, 220, 64, 0, 0, 5, 1},
        //Chorus
        {64, 30, 45, 20, 60, 25, 42, 15, 64, 120, 64, 0, 0, 6, 1},
        //Sequence 11
        {64, 64, 84, 64, 24, 64, 84, 64, 0, 120, 64, 0, 0, 0, 1}

    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_SEQUENCE, npreset - NUM_PRESETS + 1, pdata);
        
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
Sequence::changepar(int npar, int value)
{
    switch (npar)
    {
    case Sequence_Step_1:
    case Sequence_Step_2:
    case Sequence_Step_3:
    case Sequence_Step_4:
    case Sequence_Step_5:
    case Sequence_Step_6:
    case Sequence_Step_7:
    case Sequence_Step_8:
    {
        Psequence[npar] = value;
        fsequence[npar] = (float) value / 127.0f;

        seqpower = 0.0f;
        for (int i = 0; i < 8; i++)
        {
            seqpower += fsequence[i];
        }
        if (seqpower > 0.1f)
        {
            seqpower = 15.0f / seqpower;
            rndflag = 0;
        }

        int testegg = 0;
        for (int i = 0; i < 8; i++)
        {
            testegg += Psequence[i];
        }
        if (testegg < 4)
        {
            seqpower = 5.0f; //Easter egg
            rndflag = 1;
        }
        break;
    }
    case Sequence_DryWet:
        setvolume(value);
        break;
    case Sequence_Tempo:
        Ptempo = value;
        settempo(value);
        break;
    case Sequence_Resonance:
        Pq = value;
        panning = (((float) value) + 64.0f) / 128.0f;
        fq = powf(60.0f, ((float) value - 64.0f) / 64.0f);
        fb = ((float) value) / 128.0f;
        break;
    case Sequence_Amp:
        Pamplitude = value;
        break;
    case Sequence_Stdf:
        Pstdiff = value;
        break;
    case Sequence_Mode:
        Pmode = value;
        settempo(Ptempo);
        lmod = 0.5f;
        rmod = 0.5f;
        break;
    case Sequence_Range:
        Prange = value;
        setranges(Prange);
        break;
    default:
        return;
    }
}

int
Sequence::getpar(int npar)
{
    switch (npar)
    {
    case Sequence_Step_1:
    case Sequence_Step_2:
    case Sequence_Step_3:
    case Sequence_Step_4:
    case Sequence_Step_5:
    case Sequence_Step_6:
    case Sequence_Step_7:
    case Sequence_Step_8:
        return (Psequence[npar]);

    case Sequence_DryWet:
        return (Pvolume);

    case Sequence_Tempo:
        return (Ptempo);

    case Sequence_Resonance:
        return (Pq);

    case Sequence_Amp:
        return (Pamplitude);

    case Sequence_Stdf:
        return (Pstdiff);

    case Sequence_Mode:
        return (Pmode);

    case Sequence_Range:
        return (Prange);

    }
    return (0); //in case of bogus parameter number
}
