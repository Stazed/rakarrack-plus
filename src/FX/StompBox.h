/*
  Rakarrack   Audio FX software
  Stompbox.h - stompbox modeler
  Using Steve Harris LADSPA Plugin harmonic_gen
  Modified for rakarrack by Ryan Billing & Josep Andreu

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

#ifndef STOMPBOX_H
#define STOMPBOX_H

#include <vector>
#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"
#include "../EFX_common/Waveshaper.h"

#define STOMPLV2_URI "https://github.com/Stazed/rakarrack-plus#StompBox"
#define STOMP_NAME "RakarrackPlus StompBox"

const int C_STOMP_PARAMETERS = 6;

enum Stomp_Index
{
    Stomp_Level = 0,
    Stomp_Tone,
    Stomp_Mid,
    Stomp_Bias,
    Stomp_Gain,
    Stomp_Mode
};

static const char *stomp_parameters[] = 
{
    strdup(NTS(Stomp_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Stomp_Tone).c_str()), "Tone",  "HI", 
    strdup(NTS(Stomp_Mid).c_str()), "Mid",  "MID", 
    strdup(NTS(Stomp_Bias).c_str()), "Bias",  "LO", 
    strdup(NTS(Stomp_Gain).c_str()), "Gain",  "GAIN", 
    strdup(NTS(Stomp_Mode).c_str()), "Mode",  "MODE"
};

class StompBox : public Effect
{
public:
    StompBox (int wave_res, int wave_upq, int wave_dnq, double sample_rate, uint32_t intermediate_bufsize);
    ~StompBox ();
    void out (float * efxoutl, float * efxoutr);
    void Dry_Wet_Mix(int NumEffect, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_STOMP_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return STOMPLV2_URI; };
    virtual std::string get_name(int) {return STOMP_NAME; };
    
    void initialize();
    void clear_initialize();

    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

private:

    void setvolume (int value);
    void init_mode (int value);
    void init_tone ();

    int WAVE_RES;
    int WAVE_UPQ;
    int WAVE_DNQ;
    uint32_t PERIOD;
    double fSAMPLE_RATE;

    int Pvolume;
    int Pgain;
    int Phigh;
    int Pmid;
    int Plow;
    int Pmode;

    float gain, pre1gain, pre2gain, lowb, midb, highb, volume;
    float LG, MG, HG, RGP2, RGPST, pgain;

    float* interpbuf; //buffer for filters
    AnalogFilter *linput, *lpre1, *lpre2, *lpost, *ltonehg, *ltonemd, *ltonelw;
    AnalogFilter *rinput, *rpre1, *rpre2, *rpost, *rtonehg, *rtonemd, *rtonelw;
    AnalogFilter *ranti, *lanti;
    class Waveshaper *lwshape, *rwshape, *lwshape2, *rwshape2;

};


#endif
