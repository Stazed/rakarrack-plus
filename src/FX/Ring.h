/*
  ZynAddSubFX - a software synthesizer
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Based on Ring Modulator Steve Harris LADSPA plugin.
  Ring.h - Distorsion Effect

  Modified and adapted to rakarrack by Josep Andreu

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

#ifndef RING_H
#define RING_H

#include "Effect.h"

#define RINGLV2_URI "https://github.com/Stazed/rakarrack-plus#Ring"
#define RING_NAME "RakarrackPlus Ring"

const int C_RING_PARAMETERS = 13;

enum Ring_Index
{
    Ring_DryWet = 0,
    Ring_Pan,
    Ring_LR_Cross,
    Ring_Level,
    Ring_Depth,
    Ring_Freq,
    Ring_Stereo,
    Ring_Sine,
    Ring_Triangle,
    Ring_Saw,
    Ring_Square,
    Ring_Input,
    Ring_Auto_Freq
};

static const char *ring_parameters[] = 
{
    strdup(NTS(Ring_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Ring_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Ring_LR_Cross).c_str()), "Left/Right Crossover",  "LRCr", 
    strdup(NTS(Ring_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Ring_Depth).c_str()), "Depth",  "DEPTH", 
    strdup(NTS(Ring_Freq).c_str()), "Frequency",  "FREQ", 
    strdup(NTS(Ring_Stereo).c_str()), "Stereo",  "STEREO", 
    strdup(NTS(Ring_Sine).c_str()), "Sine",  "SIN", 
    strdup(NTS(Ring_Triangle).c_str()), "Triangle",  "TRI", 
    strdup(NTS(Ring_Saw).c_str()), "Sawtooth",  "SAW", 
    strdup(NTS(Ring_Square).c_str()), "Square",  "SQUARE", 
    strdup(NTS(Ring_Input).c_str()), "Input Gain",  "INPUT", 
    strdup(NTS(Ring_Auto_Freq).c_str()), "Auto-Frequency",  "AFREQ"
};

class Ring : public Effect
{
public:
    Ring (double sample_rate, uint32_t intermediate_bufsize);
    ~Ring ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void setscale();
    void cleanup ();
    int get_number_efx_parameters () {return C_RING_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return RINGLV2_URI; };
    virtual std::string get_name(int) {return RING_NAME; };
    
    void Create_Tables(float sample_rate);

    int Pafreq;
    int Pfreq;

private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void setlrcross (int Plrcross);

    unsigned int SAMPLE_RATE;
    uint32_t PERIOD;

    //Parametrii
    int Pvolume;	//Volumul or E/R
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing
    int Pinput;
    int Plevel;		//the ouput amplification
    int Pdepthp;
    int Psin;		//highpass filter
    int Ptri;
    int Psaw;
    int Psqu;
    int Pstereo;	//0=mono,1=stereo


    //Parametrii reali
    unsigned int offset;
    float panning, lrcross;
    float *sin_tbl, *tri_tbl, *saw_tbl, *squ_tbl;
    float sin,tri,saw,squ,scale,depth, idepth;

};


#endif
