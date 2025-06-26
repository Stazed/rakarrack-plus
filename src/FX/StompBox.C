/*
  Rakarrack   Audio FX software
  Stompbox.C - stompbox modeler
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "StompBox.h"

StompBox::StompBox(int wave_res, int wave_upq, int wave_dnq,
                   double sample_rate, uint32_t intermediate_bufsize) :
    Effect(wave_res, wave_upq, wave_dnq, sample_rate, intermediate_bufsize),
    WAVE_RES(wave_res),
    WAVE_UPQ(wave_upq),
    WAVE_DNQ(wave_dnq),
    PERIOD(intermediate_bufsize),
    fSAMPLE_RATE(sample_rate),
    Pvolume(50),
    Pgain(64),
    Phigh(),
    Pmid(),
    Plow(),
    Pmode(),
    gain(),
    pre1gain(100.0f),
    pre2gain(3000.0f),
    lowb(),
    midb(),
    highb(),
    volume((float) Pvolume / 127.0f),
    LG(22.0f),
    MG(11.0f),
    HG(11.0f),
    RGP2(10.0f),
    RGPST(3.0f),
    pgain(110.0f),
    interpbuf(NULL),
    linput(NULL),
    lpre1(NULL),
    lpre2(NULL),
    lpost(NULL),
    ltonehg(NULL),
    ltonemd(NULL),
    ltonelw(NULL),
    rinput(NULL),
    rpre1(NULL),
    rpre2(NULL),
    rpost(NULL),
    rtonehg(NULL),
    rtonemd(NULL),
    rtonelw(NULL),
    ranti(NULL),
    lanti(NULL),
    lwshape(NULL),
    rwshape(NULL),
    lwshape2(NULL),
    rwshape2(NULL)
{
    initialize();

    StompBox::setpreset(Ppreset);
}

StompBox::~StompBox()
{
    clear_initialize();
}

/*
 * Cleanup the effect
 */
void
StompBox::cleanup()
{
    linput->cleanup();
    lpre1->cleanup();
    lpre2->cleanup();
    lpost->cleanup();
    ltonehg->cleanup();
    ltonemd->cleanup();
    ltonelw->cleanup();

    //right channel filters
    rinput->cleanup();
    rpre1->cleanup();
    rpre2->cleanup();
    rpost->cleanup();
    rtonehg->cleanup();
    rtonemd->cleanup();
    rtonelw->cleanup();

    ranti->cleanup();
    lanti->cleanup();

    rwshape->cleanup();
    lwshape->cleanup();
    rwshape2->cleanup();
    lwshape2->cleanup();
}

std::vector<int>
StompBox::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_STOMP_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
StompBox::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_STOMP_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
StompBox::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    init_mode(Pmode);
    init_tone();
    cleanup();
}
#endif // LV2

void
StompBox::set_random_parameters()
{
    for(int i = 0; i < C_STOMP_PARAMETERS; i++)
    {
        switch(i)
        {
            case Stomp_Level:
            case Stomp_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Stomp_Tone:
            case Stomp_Mid:
            case Stomp_Bias:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Stomp_Mode:
            {
                int value = (int) (RND * 8);
                changepar (i, value);
            }
            break;
        }
    }
}

void
StompBox::LV2_parameters(std::string &s_buf, int type)
{
    for(int i = 0; i < C_STOMP_PARAMETERS; i++)
    {
        switch(i)
        {
            // Normal processing
            case Stomp_Level:
            case Stomp_Tone:
            case Stomp_Mid:
            case Stomp_Bias:
            case Stomp_Gain:
            case Stomp_Mode:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), stomp_parameters[i * 3 + 1], stomp_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Stomp_Mode )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
        }
    }
}

void
StompBox::initialize()
{
    //left channel filters
    interpbuf = new float[PERIOD];
    linput = new AnalogFilter(1, 80.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf); //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
    lpre1 = new AnalogFilter(1, 630.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf); // LPF = 0, HPF = 1
    lpre2 = new AnalogFilter(1, 220.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    lpost = new AnalogFilter(0, 720.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    ltonehg = new AnalogFilter(1, 1500.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    ltonemd = new AnalogFilter(4, 1000.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    ltonelw = new AnalogFilter(0, 500.0f, 1.0, 0, fSAMPLE_RATE, interpbuf);

    //Right channel filters
    rinput = new AnalogFilter(1, 80.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf); //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
    rpre1 = new AnalogFilter(1, 630.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf); // , sample_rateLPF = 0, HPF = 1
    rpre2 = new AnalogFilter(1, 220.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    rpost = new AnalogFilter(0, 720.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    rtonehg = new AnalogFilter(1, 1500.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    rtonemd = new AnalogFilter(4, 1000.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);
    rtonelw = new AnalogFilter(0, 500.0f, 1.0f, 0, fSAMPLE_RATE, interpbuf);

    //Anti-aliasing for between stages
    ranti = new AnalogFilter(0, 6000.0f, 0.707f, 1, fSAMPLE_RATE, interpbuf);
    lanti = new AnalogFilter(0, 6000.0f, 0.707f, 1, fSAMPLE_RATE, interpbuf);

    rwshape = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    lwshape = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    rwshape2 = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);
    lwshape2 = new Waveshaper(fSAMPLE_RATE, WAVE_RES, WAVE_UPQ, WAVE_DNQ, PERIOD);

    //    cleanup ();
}

void
StompBox::clear_initialize()
{
    delete linput;
    delete lpre1;
    delete lpre2;
    delete lpost;
    delete ltonehg;
    delete ltonemd;
    delete ltonelw;

    delete[] interpbuf;
    //Right channel filters
    delete rinput;
    delete rpre1;
    delete rpre2;
    delete rpost;
    delete rtonehg;
    delete rtonemd;
    delete rtonelw;

    //Anti-aliasing for between stages
    delete ranti;
    delete lanti;

    delete rwshape;
    delete lwshape;
    delete rwshape2;
    delete lwshape2;
}

/*
 * Effect output
 */
void
StompBox::out(float * efxoutl, float * efxoutr)
{
    switch (Pmode)
    {
        case 0: //Odie
        {
            lpre2->filterout(efxoutl, PERIOD);
            rpre2->filterout(efxoutr, PERIOD);
            rwshape->waveshapesmps(PERIOD, efxoutl, 28, 20, 1); //Valve2
            lwshape->waveshapesmps(PERIOD, efxoutr, 28, 20, 1);
            ranti->filterout(efxoutr, PERIOD);
            lanti->filterout(efxoutl, PERIOD);
            lpre1->filterout(efxoutl, PERIOD);
            rpre1->filterout(efxoutr, PERIOD);
            rwshape2->waveshapesmps(PERIOD, efxoutl, 28, Pgain, 1); //Valve2
            lwshape2->waveshapesmps(PERIOD, efxoutr, 28, Pgain, 1);

            lpost->filterout(efxoutl, PERIOD);
            rpost->filterout(efxoutr, PERIOD);
            
            for (unsigned int i = 0; i < PERIOD; i++)
            {
                //left channel
                float lfilter = ltonelw->filterout_s(efxoutl[i]);
                float mfilter = ltonemd->filterout_s(efxoutl[i]);
                float hfilter = ltonehg->filterout_s(efxoutl[i]);

                efxoutl[i] = 0.5f * volume * (efxoutl[i] + lowb * lfilter + midb * mfilter + highb * hfilter);

                //Right channel
                lfilter = rtonelw->filterout_s(efxoutr[i]);
                mfilter = rtonemd->filterout_s(efxoutr[i]);
                hfilter = rtonehg->filterout_s(efxoutr[i]);

                efxoutr[i] = 0.5f * volume * (efxoutr[i] + lowb * lfilter + midb * mfilter + highb * hfilter);

            }

            break;
        }
        case 1: //Grunge
        case 5: //Death Metal
        case 6: //Metal Zone
        {
            linput->filterout(efxoutl, PERIOD);
            rinput->filterout(efxoutr, PERIOD);
            
            for (unsigned int i = 0; i < PERIOD; i++)
            {
                float templ = efxoutl[i] * (gain * pgain + 0.01f);
                float tempr = efxoutr[i] * (gain * pgain + 0.01f);
                efxoutl[i] += lpre1->filterout_s(templ);
                efxoutr[i] += rpre1->filterout_s(tempr);
            }
            rwshape->waveshapesmps(PERIOD, efxoutl, 24, 1, 1); // Op amp limiting
            lwshape->waveshapesmps(PERIOD, efxoutr, 24, 1, 1);

            ranti->filterout(efxoutr, PERIOD);
            lanti->filterout(efxoutl, PERIOD);

            rwshape2->waveshapesmps(PERIOD, efxoutl, 23, Pgain, 1); // hard comp
            lwshape2->waveshapesmps(PERIOD, efxoutr, 23, Pgain, 1);

            for (unsigned int i = 0; i < PERIOD; i++)
            {
                efxoutl[i] = efxoutl[i] + RGP2 * lpre2->filterout_s(efxoutl[i]);
                efxoutr[i] = efxoutr[i] + RGP2 * rpre2->filterout_s(efxoutr[i]);
                efxoutl[i] = efxoutl[i] + RGPST * lpost->filterout_s(efxoutl[i]);
                efxoutr[i] = efxoutr[i] + RGPST * rpost->filterout_s(efxoutr[i]);

                //left channel
                float lfilter = ltonelw->filterout_s(efxoutl[i]);
                float mfilter = ltonemd->filterout_s(efxoutl[i]);
                float hfilter = ltonehg->filterout_s(efxoutl[i]);

                efxoutl[i] = 0.1f * volume * (efxoutl[i] + lowb * lfilter + midb * mfilter + highb * hfilter);

                //Right channel
                lfilter = rtonelw->filterout_s(efxoutr[i]);
                mfilter = rtonemd->filterout_s(efxoutr[i]);
                hfilter = rtonehg->filterout_s(efxoutr[i]);

                efxoutr[i] = 0.1f * volume * (efxoutr[i] + lowb * lfilter + midb * mfilter + highb * hfilter);

            }
            break;
        }
        case 2: //Rat
        case 3: //Fat Cat  //Pre gain & filter freqs the only difference
        {
            linput->filterout(efxoutl, PERIOD);
            rinput->filterout(efxoutr, PERIOD);

            for (unsigned int i = 0; i < PERIOD; i++)
            {
                float templ = efxoutl[i];
                float tempr = efxoutr[i];
                efxoutl[i] += lpre1->filterout_s(pre1gain * gain * templ);
                efxoutr[i] += rpre1->filterout_s(pre1gain * gain * tempr); //Low freq gain stage
                efxoutl[i] += lpre2->filterout_s(pre2gain * gain * templ);
                efxoutr[i] += rpre2->filterout_s(pre2gain * gain * tempr); //High freq gain stage
            }

            rwshape->waveshapesmps(PERIOD, efxoutl, 24, 1, 1); // Op amp limiting
            lwshape->waveshapesmps(PERIOD, efxoutr, 24, 1, 1);

            ranti->filterout(efxoutr, PERIOD);
            lanti->filterout(efxoutl, PERIOD);

            rwshape2->waveshapesmps(PERIOD, efxoutl, 23, 1, 0); // hard comp
            lwshape2->waveshapesmps(PERIOD, efxoutr, 23, 1, 0);

            for (unsigned int i = 0; i < PERIOD; i++)
            {
                //left channel
                float lfilter = ltonelw->filterout_s(efxoutl[i]);
                float mfilter = ltonemd->filterout_s(efxoutl[i]);

                efxoutl[i] = 0.5f * ltonehg->filterout_s(volume * (efxoutl[i] + lowb * lfilter + midb * mfilter));

                //Right channel
                lfilter = rtonelw->filterout_s(efxoutr[i]);
                mfilter = rtonemd->filterout_s(efxoutr[i]);

                efxoutr[i] = 0.5f * rtonehg->filterout_s(volume * (efxoutr[i] + lowb * lfilter + midb * mfilter));
            }
            break;
        }
        case 4: //Dist+
        {
            linput->filterout(efxoutl, PERIOD);
            rinput->filterout(efxoutr, PERIOD);

            for (unsigned int i = 0; i < PERIOD; i++)
            {
                float templ = efxoutl[i];
                float tempr = efxoutr[i];
                efxoutl[i] += lpre1->filterout_s(pre1gain * gain * templ);
                efxoutr[i] += rpre1->filterout_s(pre1gain * gain * tempr); //Low freq gain stage
            }

            rwshape->waveshapesmps(PERIOD, efxoutl, 24, 1, 1); // Op amp limiting
            lwshape->waveshapesmps(PERIOD, efxoutr, 24, 1, 1);

            ranti->filterout(efxoutr, PERIOD);
            lanti->filterout(efxoutl, PERIOD);

            rwshape2->waveshapesmps(PERIOD, efxoutl, 29, 1, 0); // diode limit
            lwshape2->waveshapesmps(PERIOD, efxoutr, 29, 1, 0);

            for (unsigned int i = 0; i < PERIOD; i++)
            {
                //left channel
                float lfilter = ltonelw->filterout_s(efxoutl[i]);
                float mfilter = ltonemd->filterout_s(efxoutl[i]);

                efxoutl[i] = 0.5f * ltonehg->filterout_s(volume * (efxoutl[i] + lowb * lfilter + midb * mfilter));

                //Right channel
                lfilter = rtonelw->filterout_s(efxoutr[i]);
                mfilter = rtonemd->filterout_s(efxoutr[i]);

                efxoutr[i] = 0.5f * rtonehg->filterout_s(volume * (efxoutr[i] + lowb * lfilter + midb * mfilter));
            }
            break;
        }
        case 7: //Classic Fuzz
        {
            lpre1->filterout(efxoutl, PERIOD);
            rpre1->filterout(efxoutr, PERIOD);
            linput->filterout(efxoutl, PERIOD);
            rinput->filterout(efxoutr, PERIOD);
            rwshape->waveshapesmps(PERIOD, efxoutr, 19, 25, 1); //compress
            lwshape->waveshapesmps(PERIOD, efxoutl, 19, 25, 1);
            
            for (unsigned int i = 0; i < PERIOD; i++)
            {
                //left channel
                float mfilter = ltonemd->filterout_s(efxoutl[i]);

                float templ = lpost->filterout_s(fabs(efxoutl[i]));
                float tempr = rpost->filterout_s(fabs(efxoutr[i])); //dynamic symmetry

                efxoutl[i] += lowb * templ + midb*mfilter; //In this case, lowb control tweaks symmetry

                //Right channel
                mfilter = rtonemd->filterout_s(efxoutr[i]);
                efxoutr[i] += lowb * tempr + midb*mfilter;
            }

            ranti->filterout(efxoutr, PERIOD);
            lanti->filterout(efxoutl, PERIOD);
            rwshape2->waveshapesmps(PERIOD, efxoutr, 25, Pgain, 1); //JFET
            lwshape2->waveshapesmps(PERIOD, efxoutl, 25, Pgain, 1);
            lpre2->filterout(efxoutl, PERIOD);
            rpre2->filterout(efxoutr, PERIOD);

            for (unsigned int i = 0; i < PERIOD; i++)
            {
                //left channel
                float lfilter = ltonelw->filterout_s(efxoutl[i]);
                float hfilter = ltonehg->filterout_s(efxoutl[i]);

                efxoutl[i] = volume * ((1.0f - highb) * lfilter + highb * hfilter); //classic BMP tone stack

                //Right channel
                lfilter = rtonelw->filterout_s(efxoutr[i]);
                hfilter = rtonehg->filterout_s(efxoutr[i]);

                efxoutr[i] = volume * ((1.0f - highb) * lfilter + highb * hfilter);
            }
            break;
        }
    }   // switch()
}

void
StompBox::Dry_Wet_Mix(int, float, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/*
 * Parameter control
 */
void StompBox::init_mode(int value)
{
    int tinput = 1;
    float finput = 80.0f;
    float qinput = 1.0f;
    int sinput = 0;

    int tpre1 = 1;
    float fpre1 = 708.0f;
    float qpre1 = 1.0f;
    int spre1 = 0;

    int tpre2 = 1;
    float fpre2 = 30.0f;
    float qpre2 = 1.0f;
    int spre2 = 0;

    int tpost = 0;
    float fpost = 720.0f;
    float qpost = 1.0f;
    int spost = 0;

    int ttonehg = 1;
    float ftonehg = 1500.0f;
    float qtonehg = 1.0f;
    int stonehg = 0;

    int ttonemd = 4;
    float ftonemd = 720.0f;
    float qtonemd = 1.0f;
    int stonemd = 0;

    int ttonelw = 0;
    float ftonelw = 500.0f;
    float qtonelw = 1.0f;
    int stonelw = 0;

    switch (value)
    {
    case 0:
        tinput = 1;
        finput = 80.0f;
        qinput = 1.0f;
        sinput = 0;

        tpre1 = 1;
        fpre1 = 630.0f;
        qpre1 = 1.0f;
        spre1 = 0;

        tpre2 = 1;
        fpre2 = 220.0f;
        qpre2 = 1.0f;
        spre2 = 0;

        tpost = 0;
        fpost = 720.0f;
        qpost = 1.0f;
        spost = 0;

        ttonehg = 1;
        ftonehg = 1500.0f;
        qtonehg = 1.0f;
        stonehg = 0;

        ttonemd = 4;
        ftonemd = 720.0f;
        qtonemd = 1.0f;
        stonemd = 0;

        ttonelw = 0;
        ftonelw = 500.0f;
        qtonelw = 1.0f;
        stonelw = 0;
        break;

    case 1: //Grunge
        // Some key filter stages based upon a schematic for a grunge pedal
        // Total gain up to 25,740/2 (91dB)
        // Fc1 =  999.02  Gain = 110 = 40.8dB
        // Q1 =  2.9502
        // gain stage 1rst order HP @ 340 Hz, Gain = 21.3 ... 234 (26dB ... 47dB)
        // Fc2 =  324.50
        // Q2 =  4.5039
        // Fc3 =  5994.1
        // Q3 =  1.7701
        // Fc4 =  127.80
        // Q4 =  3.7739

        tinput = 4; //Pre-Emphasis filter
        finput = 1000.0f;
        qinput = 2.95f;
        sinput = 0;
        pgain = 110.0f;

        tpre1 = 0; //Gain stage reduce aliasing
        fpre1 = 6000.0f;
        qpre1 = 0.707f;
        spre1 = 2;

        tpre2 = 4; //being used as a recovery filter, gain = 10
        fpre2 = 324.5f;
        qpre2 = 4.5f;
        spre2 = 0;
        RGP2 = 10.0f;

        tpost = 4; //The other recovery filter, gain = 3
        fpost = 6000.0f;
        qpost = 1.77f;
        spost = 0;
        RGPST = 3.0f;

        ttonehg = 1; //high shelf ranging 880 to 9700 Hz, gain 10
        ftonehg = 4000.0f;
        qtonehg = 1.0f;
        stonehg = 0;

        ttonemd = 4; // Pedal has no mid filter so I'll make up my own
        ftonemd = 1000.0f;
        qtonemd = 2.0f;
        stonemd = 0;

        ttonelw = 4; //Low Eq band, peaking type, gain = up to 22.
        ftonelw = 128.0f;
        qtonelw = 3.8f;
        stonelw = 0;
        break;

    case 2: //ProCo Rat Distortion emulation
        // Some key filter stages based upon a schematic for a grunge pedal

        tinput = 0; //Reduce some noise aliasing
        finput = 5000.0f;
        qinput = 1.0f;
        sinput = 3;

        tpre1 = 1; //Gain stage high boost, gain = 1 ... 268 (max)
        fpre1 = 60.0f;
        qpre1 = 1.0f;
        spre1 = 0;
        pre1gain = 268.0f;

        tpre2 = 1; //being used as a recovery filter, gain = 1 ... 3000
        fpre2 = 1539.0f;
        qpre2 = 1.0f;
        spre2 = 0;
        pre2gain = 3000.0f;

        tpost = 0; //Not used...initialized to "something"
        fpost = 6000.0f;
        qpost = 1.77f;
        spost = 0;

        ttonehg = 0; //frequency sweeping LPF
        ftonehg = 1000.0f;
        qtonehg = 1.0f;
        stonehg = 0;

        ttonemd = 4; // Pedal has no mid filter so I'll make up my own
        ftonemd = 700.0f;
        qtonemd = 2.0f;
        stonemd = 0;

        ttonelw = 0; //Pedal has no Low filter, so make up my own...Low Eq band, peaking type
        ftonelw = 328.0f; //Mild low boost
        qtonelw = 0.50f;
        stonelw = 1;
        break;

    case 3: //Fat Cat Distortion emulation
    case 4: //MXR Dist+ emulation (many below filters unuse)
        // Some key filter stages based upon a schematic for a grunge pedal

        tinput = 0; //Reduce some noise aliasing
        finput = 5000.0f;
        qinput = 1.0f;
        sinput = 3;

        tpre1 = 1; //Gain stage high boost, gain = 1 ... 100 (max)
        fpre1 = 33.0f;
        qpre1 = 1.0f;
        spre1 = 0;
        pre1gain = 100.0f;

        tpre2 = 1; //being used as a recovery filter, gain = 1 ... 1700
        fpre2 = 861.0f;
        qpre2 = 1.0f;
        spre2 = 0;
        pre2gain = 1700.0f;

        tpost = 0; //Not used...initialized to "something"
        fpost = 6000.0f;
        qpost = 1.77f;
        spost = 0;

        ttonehg = 0; //frequency sweeping LPF
        ftonehg = 1000.0f;
        qtonehg = 1.0f;
        stonehg = 0;

        ttonemd = 4; // Pedal has no mid filter so I'll make up my own
        ftonemd = 700.0f;
        qtonemd = 2.0f;
        stonemd = 0;

        ttonelw = 0; //Pedal has no Low filter, so make up my own...Low Eq band, peaking type
        ftonelw = 328.0f; //Mild low boost
        qtonelw = 0.50f;
        stonelw = 1;
        break;

    case 5: //Death Metal
        // Some key filter stages based upon a schematic for a grunge pedal

        tinput = 4; //Pre-Emphasis filter
        finput = 6735.4f;
        qinput = 0.43f;
        sinput = 0;
        pgain = 110.0f;

        tpre1 = 0; //Gain stage reduce aliasing
        fpre1 = 6000.0f;
        qpre1 = 0.707f;
        spre1 = 2;

        tpre2 = 4; //being used as a recovery filter, gain = 10
        fpre2 = 517.0f;
        qpre2 = 7.17f;
        spre2 = 0;
        RGP2 = 1.0f;

        tpost = 4; //The other recovery filter, gain = 10
        fpost = 48.0f;
        qpost = 6.68f;
        spost = 0;
        RGPST = 10.0f;

        ttonehg = 1; //high shelf ranging 880 to 9700 Hz, gain 11
        ftonehg = 4000.0f;
        qtonehg = 1.0f;
        stonehg = 0;
        HG = 11.0f;

        ttonemd = 4; // Mid band EQ gain 11
        ftonemd = 1017.0f;
        qtonemd = 1.15f;
        stonemd = 0;
        MG = 11.0f;

        ttonelw = 4; //Low Eq band, peaking type, gain = up to 22.
        ftonelw = 107.0f;
        qtonelw = 3.16f;
        stonelw = 0;
        LG = 22.0f;

        break;
    case 6: //Metal Zone
        // Some key filter stages based upon a schematic for a grunge pedal

        tinput = 4; //Pre-Emphasis filter
        finput = 952.53f;
        qinput = 2.8f;
        sinput = 0;
        pgain = 100.0f;

        tpre1 = 0; //Gain stage reduce aliasing
        fpre1 = 6000.0f;
        qpre1 = 0.707f;
        spre1 = 2;

        tpre2 = 4; //being used as a recovery filter, gain = 10
        fpre2 = 4894.0f;
        qpre2 = 2.16f;
        spre2 = 0;
        RGP2 = 3.3f;

        tpost = 4; //The other recovery filter, gain = 10
        fpost = 105.0f;
        qpost = 14.62f;
        spost = 0;
        RGPST = 7.0f;

        ttonehg = 1; //high shelf ranging 880 to 9700 Hz, gain 11
        ftonehg = 4000.0f;
        qtonehg = 1.0f;
        stonehg = 0;
        HG = 10.0f;

        ttonemd = 4; // Mid band EQ gain 11
        ftonemd = 1017.0f;
        qtonemd = 1.15f;
        stonemd = 0;
        MG = 11.0f;

        ttonelw = 4; //Low Eq band, peaking type, gain = up to 22.
        ftonelw = 105.50f;
        qtonelw = 3.11f;
        stonelw = 0;
        LG = 3.33f;

        break;

    case 7: //Classic Fuzz
        tinput = 1;
        finput = 80.0f;
        qinput = 1.0f;
        sinput = 0;

        tpre1 = 0;
        fpre1 = 4500.0f;
        qpre1 = 1.0f;
        spre1 = 1;

        tpre2 = 1;
        fpre2 = 40.0f;
        qpre2 = 1.0f;
        spre2 = 0;

        tpost = 0;
        fpost = 2.0f;
        qpost = 1.0f;
        spost = 0;

        ttonehg = 1;
        ftonehg = 397.0f;
        qtonehg = 1.0f;
        stonehg = 0;

        ttonemd = 4;
        ftonemd = 515.0f; //sort of like a stuck wahwah
        qtonemd = 4.0f;
        stonemd = 0;

        ttonelw = 0;
        ftonelw = 295.0f;
        qtonelw = 1.0f;
        stonelw = 0;
        break;
    }

    //left channel filters
    //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
    // LPF = 0, HPF = 1
    linput->settype(tinput);
    linput->setfreq_and_q(finput, qinput);
    linput->setstages(sinput);

    lpre1->settype(tpre1);
    lpre1->setfreq_and_q(fpre1, qpre1);
    lpre1->setstages(spre1);

    lpre2->settype(tpre2);
    lpre2->setfreq_and_q(fpre2, qpre2);
    lpre2->setstages(spre2);

    lpost->settype(tpost);
    lpost->setfreq_and_q(fpost, qpost);
    lpost->setstages(spost);

    ltonehg->settype(ttonehg);
    ltonehg->setfreq_and_q(ftonehg, qtonehg);
    ltonehg->setstages(stonehg);

    ltonemd->settype(ttonemd);
    ltonemd->setfreq_and_q(ftonemd, qtonemd);
    ltonemd->setstages(stonemd);

    ltonelw->settype(ttonelw);
    ltonelw->setfreq_and_q(ftonelw, qtonelw);
    ltonelw->setstages(stonelw);

    //right channel filters

    rinput->settype(tinput);
    rinput->setfreq_and_q(finput, qinput);
    rinput->setstages(sinput);

    rpre1->settype(tpre1);
    rpre1->setfreq_and_q(fpre1, qpre1);
    rpre1->setstages(spre1);

    rpre2->settype(tpre2);
    rpre2->setfreq_and_q(fpre2, qpre2);
    rpre2->setstages(spre2);

    rpost->settype(tpost);
    rpost->setfreq_and_q(fpost, qpost);
    rpost->setstages(spost);

    rtonehg->settype(ttonehg);
    rtonehg->setfreq_and_q(ftonehg, qtonehg);
    rtonehg->setstages(stonehg);

    rtonemd->settype(ttonemd);
    rtonemd->setfreq_and_q(ftonemd, qtonemd);
    rtonemd->setstages(stonemd);

    rtonelw->settype(ttonelw);
    rtonelw->setfreq_and_q(ftonelw, qtonelw);
    rtonelw->setstages(stonelw);
}

void StompBox::init_tone()
{
    switch (Pmode)
    {
        case 0:
        {
            {
            float varf = 2533.0f + highb * 1733.0f; //High tone ranges from 800 to 6000Hz
            rtonehg->setfreq(varf);
            ltonehg->setfreq(varf);
            if (highb > 0.0f) highb = ((float) Phigh) / 8.0f;
            break;
            }
        }
        case 1:
        {
            float varf = 3333.0f + highb * 2500.0f; //High tone ranges from 833 to 8333Hz
            rtonehg->setfreq(varf);
            ltonehg->setfreq(varf);

            if (highb > 0.0f) highb = ((float) Phigh) / 16.0f;
            if (lowb > 0.0f) lowb = ((float) Plow) / 18.0f;
            break;
        }
        case 2:
        case 3:
        {
            float varf = 3653.0f + highb * 3173.0f; //High tone ranges from ~480 to 10k
            rtonehg->setfreq(varf);
            ltonehg->setfreq(varf);
            break;
        }
        case 4:
        {
            float varf = gain * 700.0f + 20.0f;
            rpre1->setfreq(varf);
            lpre1->setfreq(varf);
            pre1gain = 212.0f;
            varf = 3653.0f + highb * 3173.0f; //High tone ranges from ~480 to 10k
            rtonehg->setfreq(varf);
            ltonehg->setfreq(varf);
            break;
        }
        case 5: //Death Metal
        case 6: //Mid Elves Own
        {
            float varf = 3653.0f + highb * 3173.0f; //High tone ranges from ~480 to 10k
            rtonehg->setfreq(varf);
            ltonehg->setfreq(varf);

            if (highb > 0.0f) highb = HG * ((float) Phigh) / 64.0f;
            if (lowb > 0.0f) lowb = LG * ((float) Plow) / 64.0f;
            if (midb > 0.0f) midb = MG * ((float) Pmid) / 64.0f;
            break;
        }
        case 7:
        {
            highb = ((float) Phigh + 64) / 127.0f;
            float varf = 40.0f + gain * 200.0f;
            linput->setfreq(varf);
            rinput->setfreq(varf);
            if (midb > 0.0f) midb = ((float) Pmid) / 8.0f;
            lowb = ((float) Plow) / 64.0f;

            varf = 1085.0f - lowb * 1000.0f;
            lpre1->setfreq(varf);
            rpre1->setfreq(varf);
            break;
        }
    }   // switch()

    cleanup();
}

void
StompBox::setvolume(int value)
{
    Pvolume = value;
    volume = (float) Pvolume / 127.0f;
}

void
StompBox::setpreset(int npreset)
{
    const int PRESET_SIZE = C_STOMP_PARAMETERS;
    const int NUM_PRESETS = 11;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        // Amp 1
        {48, 32, 0, 32, 65, 0},
        //Odie
        {80, 32, 0, 32, 10, 0},
        //Grunger
        {48, 10, -6, 55, 85, 1},
        //Hard Dist.
        {48, -22, -6, 38, 12, 1},
        //Ratty
        {48, -20, 0, 0, 70, 2},
        //Classic Dist
        {50, 64, 0, 0, 110, 4},
        //Morbid Impalement
        {38, 6, 6, 6, 105, 5},
        //Mid Elve
        {48, 0, -12, 0, 127, 6},
        //Fuzz
        {48, 0, 0, 0, 127, 7},
        //Gnasty
        {48, 28, -13, 64, 127, 7},
        // Amp 2
        {40, 0, 0, 0, 64, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_STOMPBOX, npreset - NUM_PRESETS + 1, pdata);
        
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
StompBox::changepar(int npar, int value)
{
    switch (npar)
    {
    case Stomp_Level:
        setvolume(value);
        break;
    case Stomp_Tone:
        Phigh = value;
        if (value < 0) highb = ((float) value) / 64.0f;
        if (value > 0) highb = ((float) value) / 32.0f;
        break;
    case Stomp_Mid:
        Pmid = value;
        if (value < 0) midb = ((float) value) / 64.0f;
        if (value > 0) midb = ((float) value) / 32.0f;
        break;
    case Stomp_Bias:
        Plow = value;
        if (value < 0) lowb = ((float) value) / 64.0f;
        if (value > 0) lowb = ((float) value) / 32.0f;
        break;
    case Stomp_Gain:
        Pgain = value;
        gain = dB2rap(50.0f * ((float) value) / 127.0f - 50.0f);
        break;
    case Stomp_Mode:
        Pmode = value;
        init_mode(Pmode);
        break;
    default:
        return;
    }
    init_tone();
}

int
StompBox::getpar(int npar)
{
    switch (npar)
    {
    case Stomp_Level:
        return (Pvolume);

    case Stomp_Tone:
        return (Phigh);

    case Stomp_Mid:
        return (Pmid);

    case Stomp_Bias:
        return (Plow);

    case Stomp_Gain:
        return (Pgain);

    case Stomp_Mode:
        return (Pmode);

    }
    return (0); //in case of bogus parameter number
}

