/*
  ZynAddSubFX - a software synthesizer

  Reverb.C - Reverberation effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

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

#include <math.h>
#include <stdlib.h>
#include "Reverb.h"

/*TODO: EarlyReflections,Prdelay,Perbalance */

Reverb::Reverb(double samplerate, uint16_t intermediate_bufsize) :
    Effect(samplerate, intermediate_bufsize),
    fSAMPLE_RATE(samplerate),
    PERIOD(intermediate_bufsize),
    Pvolume(48),
    Ppan(64),
    Ptime(64),
    Pidelay(40),
    Pidelayfb(),
    Prdelay(),
    Perbalance(64),
    Plpf(127),
    Phpf(),
    Plohidamp(80),
    Ptype(1),
    Proomsize(64),
    lohidamptype(),
    idelaylen(),
    rdelaylen(),
    idelayk(),
    comblen(),
    aplen(),
    combk(),
    apk(),
    lohifb(),
    idelayfb(),
    roomsize(1.0f),
    rs(1.0f),
    rs_coeff(rs / (float) REV_COMBS),
    pan(),
    erbalance(),
    comb(),
    combfb(),
    lpcomb(),
    ap(),
    inputbuf(NULL),
    idelay(NULL),
    interpbuf(NULL),
    lpf(NULL),
    hpf(NULL)
{
    initialize();

    //max comb length
    unsigned int tmp = lrintf(220023.0 * samplerate / 44100.0);

    for (int i = 0; i < REV_COMBS * 2; i++)
    {
        comblen[i] = 800 + (int) (RND * 1400);
        combk[i] = 0;
        lpcomb[i] = 0;
        combfb[i] = -0.97f;
        comb[i] = new float[tmp]; //set to make length so we don't need to reallocate ever
    }

    //max ap length
    tmp = lrintf(100023.0 * samplerate / 44100.0);

    for (int i = 0; i < REV_APS * 2; i++)
    {
        aplen[i] = 500 + (int) (RND * 500);
        apk[i] = 0;
        ap[i] = new float[tmp]; //set to max length
    }

    //max delay length
    tmp = lrintf(2.5 * samplerate);
    idelay = new float[tmp]; //set to max length

    Reverb::setpreset(Ppreset);
    Reverb::cleanup(); //do not call this before the comb initialisation
}

Reverb::~Reverb()
{
    clear_initialize();

    for (int i = 0; i < REV_COMBS * 2; i++)
    {
        delete[] comb[i];
    }

    for (int i = 0; i < REV_APS * 2; i++)
    {
        delete[] ap[i];
    }

    delete[] idelay;
}

/*
 * Cleanup the effect
 */
void
Reverb::cleanup()
{
    for (int i = 0; i < REV_COMBS * 2; i++)
    {
        lpcomb[i] = 0.0;

        for (int j = 0; j < comblen[i]; j++)
        {
            comb[i][j] = 0.0;
        }
    }

    for (int i = 0; i < REV_APS * 2; i++)
    {
        for (int j = 0; j < aplen[i]; j++)
        {
            ap[i][j] = 0.0;
        }
    }

    if (idelay != NULL)
    {
        for (int i = 0; i < idelaylen; i++)
        {
            idelay[i] = 0.0;
        }
    }

    hpf->cleanup();
    lpf->cleanup();
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
Reverb::lv2_update_params(uint32_t period)
{
    PERIOD = period_master = period;
    clear_initialize();
    initialize();
    sethpf(Phpf);
    setlpf(Plpf);
}
#endif // LV2

void
Reverb::set_random_parameters()
{
    for(int i = 0; i < C_REVERB_PARAMETERS; i++)
    {
        switch(i)
        {
            case Reverb_DryWet:
            case Reverb_Pan:
            case Reverb_Time:
            case Reverb_I_Delay:
            case Reverb_Delay_FB:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Reverb_LPF:
            {
                int value = (int) (RND * 25980);
                changepar (i, value + 20);
            }
            break;

            case Reverb_HPF:
            {
                int value = (int) (RND * 19980);
                changepar (i, value + 20);
            }
            break;

            case Reverb_Damp:
            {
                int value = (int) (RND * 63);
                changepar (i, value + 64);
            }
            break;

            case Reverb_Type:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Reverb_Room:
            {
                int value = (int) (RND * 127);
                changepar (i, value + 1);
            }
            break;

            case Reverb_SKIP_5:
            case Reverb_SKIP_6:
                break;
        }
    }
}

void
Reverb::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    for(int i = 0; i < (C_REVERB_PARAMETERS - 2); i++)  // -2 skips 5 & 6 unused
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Reverb_Time:
            case Reverb_I_Delay:
            case Reverb_LPF:
            case Reverb_HPF:
            case Reverb_Damp:
            case Reverb_Type:
            case Reverb_Room:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), reverb_parameters[i * 3 + 1], reverb_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Reverb_Room )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // wet/dry -> dry/wet reversal
            case Reverb_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Reverb_DryWet )), reverb_parameters[i * 3 + 1], reverb_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Reverb_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Reverb_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Reverb_Pan ) - 64, reverb_parameters[i * 3 + 1], reverb_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Reverb_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;

            // Skip after
            case Reverb_Delay_FB:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Reverb_Delay_FB ), reverb_parameters[i * 3 + 1], reverb_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) );
                    s_buf += ":";
                }

                param_case_offset += 2; // Skip 5 & 6
            }
            break;
        }

        param_case_offset++;
    }
}

void
Reverb::initialize()
{
    inputbuf = new float[PERIOD];
 
    for (unsigned int i = 0; i < PERIOD; i++)
    {
        inputbuf[i] = 0;
    }

    interpbuf = new float[PERIOD];
    lpf = new AnalogFilter(2, 22000, 1, 0, fSAMPLE_RATE, interpbuf);
    hpf = new AnalogFilter(3, 20, 1, 0, fSAMPLE_RATE, interpbuf);
}

void
Reverb::clear_initialize()
{
    delete lpf;
    delete hpf;
    delete[] interpbuf;
    delete[] inputbuf;
}

/*
 * Process one channel; 0=left,1=right
 */
void
Reverb::processmono(unsigned int ch, float * output)
{
    //TODO: implement the high part from lohidamp

    for (unsigned j = REV_COMBS * ch; j < REV_COMBS * (ch + 1); j++)
    {
        int ck = combk[j];
        int comblength = comblen[j];
        float lpcombj = lpcomb[j];

        for (unsigned i = 0; i < PERIOD; i++)
        {
            float fbout = comb[j][ck] * combfb[j];
            fbout = fbout * (1.0f - lohifb) + (lpcombj * lohifb);
            lpcombj = fbout;

            comb[j][ck] = inputbuf[i] + fbout;
            output[i] += fbout;

            if ((++ck) >= comblength)
            {
                ck = 0;
            }
        }

        combk[j] = ck;
        lpcomb[j] = lpcombj;
    }

    for (unsigned j = REV_APS * ch; j < REV_APS * (1 + ch); j++)
    {
        int ak = apk[j];
        int aplength = aplen[j];

        for (unsigned i = 0; i < PERIOD; i++)
        {
            float tmp = ap[j][ak];
            ap[j][ak] = 0.7f * tmp + output[i];
            output[i] = tmp - 0.7f * ap[j][ak];

            if ((++ak) >= aplength)
            {
                ak = 0;
            }
        }
        apk[j] = ak;
    }
}

/*
 * Effect output
 */
void
Reverb::out(float * efxoutl, float * efxoutr)
{
    for (unsigned int i = 0; i < PERIOD; i++)
    {
        inputbuf[i] = (efxoutl[i] + efxoutr[i]) * .5f;
        //Initial delay r
        if (idelay != NULL)
        {
            float tmp = inputbuf[i] + idelay[idelayk] * idelayfb;
            inputbuf[i] = idelay[idelayk];
            idelay[idelayk] = tmp;
            idelayk++;

            if (idelayk >= idelaylen)
                idelayk = 0;
        }
    }

    lpf->filterout(inputbuf, PERIOD);
    hpf->filterout(inputbuf, PERIOD);

    processmono(0, efxoutl); //left
    processmono(1, efxoutr); //right

    float lvol = rs_coeff * (1.0f - pan) * 2.0f;
    float rvol = rs_coeff * pan * 2.0f;

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        efxoutl[i] *= lvol;
        efxoutr[i] *= rvol;
    }
}

/*
 * Parameter control
 */
void
Reverb::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;

    if (_Pvolume == 0)
        cleanup();
}

void
Reverb::setpan(int _Ppan)
{
    Ppan = _Ppan;
    pan = (float) _Ppan / 127.0f;
}

void
Reverb::settime(int _Ptime)
{
    Ptime = _Ptime;
    float t = powf(60.0f, (float) _Ptime / 127.0f) - 0.97f;

    for (int i = 0; i < REV_COMBS * 2; i++)
    {
        combfb[i] =  -expf((float) comblen[i] / fSAMPLE_RATE * logf(0.001f) / t);
        //the feedback is negative because it removes the DC
    }
}

void
Reverb::setlohidamp(int _Plohidamp)
{
    if (_Plohidamp < 64)
    {
        _Plohidamp = 64; //remove this when the high part from lohidamp will be added
    }

    Plohidamp = _Plohidamp;

    if (_Plohidamp == 64)
    {
        lohidamptype = 0;
        lohifb = 0.0;
    }
    else
    {
        if (_Plohidamp < 64)
        {
            lohidamptype = 1;
        }

        if (_Plohidamp > 64)
        {
            lohidamptype = 2;
        }

        float x = fabsf((float) (_Plohidamp - 64) / 64.1f);
        lohifb = x * x;
    }
}

void
Reverb::setidelay(int _Pidelay)
{
    Pidelay = _Pidelay;
    float delay = powf(50.0f * (float) _Pidelay / 127.0f, 2.0f) - 1.0f;

    idelaylen = lrintf(fSAMPLE_RATE * delay / 1000.0f);

    if (idelaylen > 1)
    {
        idelayk = 0;

        for (int i = 0; i < idelaylen; i++)
        {
            idelay[i] = 0.0;
        }
    }
}

void
Reverb::setidelayfb(int _Pidelayfb)
{
    Pidelayfb = _Pidelayfb;
    idelayfb = (float) _Pidelayfb / 128.0f;
}

void
Reverb::sethpf(int value)
{
    Phpf = value;
    float fr = (float) Phpf;
    hpf->setfreq(fr);
}

void
Reverb::setlpf(int value)
{
    Plpf = value;
    float fr = (float) Plpf;
    lpf->setfreq(fr);
}

void
Reverb::settype(int _Ptype)
{
    const int NUM_TYPES = 2;
    int combtunings[NUM_TYPES][REV_COMBS] = {
        //this is unused (for random)
        {0, 0, 0, 0, 0, 0, 0, 0},
        //Freeverb by Jezar at Dreampoint
        {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617}
    };

    int aptunings[NUM_TYPES][REV_APS] = {
        //this is unused (for random)
        {0, 0, 0, 0},
        //Freeverb by Jezar at Dreampoint
        {225, 341, 441, 556}
    };

    if (_Ptype >= NUM_TYPES)
    {
        _Ptype = NUM_TYPES - 1;
    }

    Ptype = _Ptype;

    float tmp = 0.0;

    for (int i = 0; i < REV_COMBS * 2; i++)
    {
        if (_Ptype == 0)
        {
            tmp = 800.0f + (float) (RND * 1400.0f);
        }
        else
        {
            tmp = (float) combtunings[_Ptype][i % REV_COMBS];
        }

        tmp *= roomsize;

        if (i > REV_COMBS)
        {
            tmp += 23.0f;
        }

        tmp *= fSAMPLE_RATE / 44100.0f; //adjust the combs according to the samplerate

        if (tmp < 10)
        {
            tmp = 10;
        }

        comblen[i] = lrintf(tmp);
        combk[i] = 0;
        lpcomb[i] = 0;
    }

    for (int i = 0; i < REV_APS * 2; i++)
    {
        if (_Ptype == 0)
        {
            tmp = 500.0f + (float) (RND * 500.0f);
        }
        else
        {
            tmp = (float) aptunings[_Ptype][i % REV_APS];
        }

        tmp *= roomsize;

        if (i > REV_APS)
        {
            tmp += 23.0f;
        }

        tmp *= fSAMPLE_RATE / 44100.0f; //adjust the combs according to the samplerate

        if (tmp < 10)
        {
            tmp = 10;
        }

        aplen[i] = lrintf(tmp);
        apk[i] = 0;
    }

    settime(Ptime);
    cleanup();
}

void
Reverb::setroomsize(int _Proomsize)
{
    if (_Proomsize == 0)
    {
        _Proomsize = 64; //this is because the older versions consider roomsize=0
    }

    Proomsize = _Proomsize;
    roomsize = ((float) _Proomsize - 64.0f) / 64.0f;

    if (roomsize > 0.0)
    {
        roomsize *= 2.0f;
    }

    roomsize = powf(10.0f, roomsize);
    rs = sqrtf(roomsize);
    rs_coeff = rs / (float) REV_COMBS;
    settype(Ptype);
}

void
Reverb::setpreset(int npreset)
{
    const int PRESET_SIZE = C_REVERB_PARAMETERS;
    const int NUM_PRESETS = 13;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Cathedral1
        {80, 64, 63, 24, 0, 0, 0, 4002, 27, 83, 1, 64},
        //Cathedral2
        {80, 64, 69, 35, 0, 0, 0, 25040, 21, 71, 0, 64},
        //Cathedral3
        {80, 64, 69, 24, 0, 0, 0, 25040, 2417, 78, 1, 85},
        //Hall1
        {90, 64, 51, 10, 0, 0, 0, 25040, 81, 78, 1, 64},
        //Hall2
        {90, 64, 53, 20, 0, 0, 0, 25040, 2417, 71, 1, 64},
        //Room1
        {100, 64, 33, 0, 0, 0, 0, 25040, 21, 106, 0, 30},
        //Room2
        {100, 64, 21, 26, 0, 0, 0, 1223, 21, 77, 1, 45},
        //Basement
        {110, 64, 14, 0, 0, 0, 0, 25040, 27, 71, 0, 25},
        //Tunnel
        {85, 80, 84, 20, 42, 0, 0, 652, 21, 78, 1, 105},
        //Echoed1
        {95, 64, 26, 60, 71, 0, 0, 14722, 21, 64, 1, 64},
        //Echoed2
        {90, 64, 40, 88, 71, 0, 0, 14722, 21, 88, 1, 64},
        //VeryLong1
        {90, 64, 93, 15, 0, 0, 0, 14722, 21, 77, 0, 95},
        //VeryLong2
        {90, 64, 111, 30, 0, 0, 0, 14722, 5058, 74, 1, 80}
    };


    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_REVERB, npreset - NUM_PRESETS + 1, pdata);

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
Reverb::changepar(int npar, int value)
{
    switch (npar)
    {
    case Reverb_DryWet:
        setvolume(value);
        break;
    case Reverb_Pan:
        setpan(value);
        break;
    case Reverb_Time:
        settime(value);
        break;
    case Reverb_I_Delay:
        setidelay(value);
        break;
    case Reverb_Delay_FB:
        setidelayfb(value);
        break;
        //      case Reverb_SKIP_5: setrdelay(value);
        //              break;
        //      case Reverb_SKIP_6: seterbalance(value);
        //              break;
    case Reverb_LPF:
        setlpf(value);
        break;
    case Reverb_HPF:
        sethpf(value);
        break;
    case Reverb_Damp:
        setlohidamp(value);
        break;
    case Reverb_Type:
        settype(value);
        break;
    case Reverb_Room:
        setroomsize(value);
        break;
    default:
        return;
    }
}

int
Reverb::getpar(int npar)
{
    switch (npar)
    {
    case Reverb_DryWet:
        return (Pvolume);

    case Reverb_Pan:
        return (Ppan);

    case Reverb_Time:
        return (Ptime);

    case Reverb_I_Delay:
        return (Pidelay);

    case Reverb_Delay_FB:
        return (Pidelayfb);

        //      case Reverb_SKIP_5: return(Prdelay);
        //              break;
        //      case Reverb_SKIP_6: return(Perbalance);
        //              break;
    case Reverb_LPF:
        return (Plpf);

    case Reverb_HPF:
        return (Phpf);

    case Reverb_Damp:
        return (Plohidamp);

    case Reverb_Type:
        return (Ptype);

    case Reverb_Room:
        return (Proomsize);

    }

    return (0); //in case of bogus "parameter"
}
