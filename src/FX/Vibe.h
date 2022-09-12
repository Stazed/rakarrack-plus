/*
  rakarrack - a guitar effects software

 Vibe.h  -  Vibe Effect definitions

  Copyright (C) 2008-2010 Ryan Billing
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#ifndef Vibe_H
#define Vibe_H

#include "Effect.h"
#include "../EFX_common/EffectLFO.h"

#define VIBELV2_URI "https://github.com/Stazed/rakarrack-plus#Vibe"
#define VIBE_NAME "RakarrackPlus Vibe"

//#define VIBE_INLINE 1     // uncomment for inline

const int C_VIBE_PARAMETERS = 11;

enum Vibe_Index
{
    Vibe_Width = 0,
    Vibe_LFO_Tempo,
    Vibe_LFO_Random,
    Vibe_LFO_Type,
    Vibe_LFO_Stereo,
    Vibe_Pan,
    Vibe_DryWet,
    Vibe_Feedback,
    Vibe_Depth,
    Vibe_LR_Cross,
    Vibe_Stereo
};

static const char *vibe_parameters[] = 
{
    strdup(NTS(Vibe_Width).c_str()), "Width",  "WIDTH", 
    strdup(NTS(Vibe_LFO_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Vibe_LFO_Random).c_str()), "Randomness",  "RND", 
    strdup(NTS(Vibe_LFO_Type).c_str()), "LFO Type",  "TYPE", 
    strdup(NTS(Vibe_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDL", 
    strdup(NTS(Vibe_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Vibe_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Vibe_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(Vibe_Depth).c_str()), "Depth",  "DEPTH", 
    strdup(NTS(Vibe_LR_Cross).c_str()), "Left/Right Crossover",  "LRCR", 
    strdup(NTS(Vibe_Stereo).c_str()), "Stereo",  "STEREO"
};

class Vibe : public Effect
{

public:

    Vibe (double sample_rate, uint32_t intermediate_bufsize);
    ~Vibe ();

    void out (float * efxoutl, float * efxoutr);
    void setvolume(int value);
    void setpanning(int value);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_VIBE_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return VIBELV2_URI; };
    virtual std::string get_name(int) {return VIBE_NAME; };

private:
    
    uint32_t PERIOD;
    float cSAMPLE_RATE;
    float fSAMPLE_RATE;
    
    int Pwidth;
    int Pfb;
    int Plrcross;
    int Pdepth;
    int Ppanning;
    int Pvolume;
    int Pstereo;

    float fwidth;
    float fdepth;
    float rpanning, lpanning;
    float flrcross, fcross;
    float fb;

    float Ra, Rb, b, dTC, dRCl, dRCr, lampTC, ilampTC, minTC, alphal, alphar, stepl, stepr, oldstepl, oldstepr;
    float fbr, fbl;
    float dalphal, dalphar;
    float lstep,rstep;
//    float cperiod;
    float gl, oldgl;
    float gr, oldgr;

    class fparams
    {
    public:
        float x1;
        float y1;
        //filter coefficients
        float n0;
        float n1;
        float d0;
        float d1;
        
        fparams():
            x1(),
            y1(),
            n0(),
            n1(),
            d0(),
            d1() {}
        
        void clear ()
        {
            x1 = y1 = 0;
        }
    } vc[8], vcvo[8], ecvc[8], vevo[8], bootstrap[8];

    float vibefilter(float data, fparams *ftype, int stage) const;
    void init_vibes();
    void modulate(float ldrl, float ldrr);
    float bjt_shape(float data) const;

    float R1;
    float Rv;
    float C2;
    float C1[8];
    float beta;  //transistor forward gain.
    float gain, k;
    float oldcvolt[8] ;
    float en1[8], en0[8], ed1[8], ed0[8];
    float cn1[8], cn0[8], cd1[8], cd0[8];
    float ecn1[8], ecn0[8], ecd1[8], ecd0[8];
    float on1[8], on0[8], od1[8], od0[8];

    EffectLFO* lfo;

};

#endif
