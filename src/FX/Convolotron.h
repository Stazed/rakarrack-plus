/*
  Convolotron.h - Convolotron Effect

  Author: Ryam Billing & Jospe Andreu

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

#ifndef CONVOLOTRON_H
#define CONVOLOTRON_H

#include <sndfile.h>
#include <vector>
#include "Effect.h"
#include "../EFX_common/Resample.h"
#include "../EFX_common/mayer_fft.h"

#define CONVOLOTRONLV2_URI "https://github.com/Stazed/rakarrack-plus#Convolotron"
#define CONVOLOTRON_NAME "RakarrackPlus Convolotron"

const int C_CONVO_PARAMETERS = 11;

enum Convo_Index
{
    Convo_DryWet = 0,
    Convo_Pan,
    Convo_Safe,
    Convo_Length,
    Convo_User_File,
    Convo_SKIP_5,
    Convo_Damp,
    Convo_Level,
    Convo_Set_File,
    Convo_SKIP_9,
    Convo_Feedback
};

static const char *convo_parameters[] = 
{
    strdup(NTS(Convo_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Convo_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Convo_Safe).c_str()), "Safe",  "SAFE", 
    strdup(NTS(Convo_Length).c_str()), "Length",  "LENGTH", 
    strdup(NTS(Convo_Damp).c_str()), "Dampening",  "DAMP", 
    strdup(NTS(Convo_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Convo_Feedback).c_str()), "Feedback",  "FB"
};

class Convolotron :public Effect
{
public:
    Convolotron (int DS, int uq, int dq, double sample_rate, uint16_t intermediate_bufsize);
    ~Convolotron ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_CONVO_PARAMETERS;};
    void set_random_parameters();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return CONVOLOTRONLV2_URI; };
    virtual std::string get_name(int) {return CONVOLOTRON_NAME; };
    
    void initialize();
    void clear_initialize();
    int setfile (int value);
    void adjust(int DS, uint32_t period);
    void loaddefault();
    void set_user_files(const std::vector<User_Files> &wav_files){WAV_Files = wav_files;};

    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

    char Filename[128];

private:

    std::vector<User_Files> WAV_Files;

    unsigned int SAMPLE_RATE;
    float fSAMPLE_RATE;
    int nSAMPLE_RATE;
    float nfSAMPLE_RATE;
    
    uint32_t PERIOD;
    int nPERIOD;
    float fPERIOD;
    
    //Parametrii
    int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
    int Phidamp;
    int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
    int Psafe;
    int Plength;		//5...500 ms// Set maximum length of IR.
    int Puser;		//-64...64//Feedback.
    int Filenum;
    int Pfb;		//-64 ... 64// amount of feedback
    void setvolume (int _Pvolume);
    void setpanning (int _Ppanning);
    void sethidamp (int _Phidamp);
    void process_rbuf();
    void UpdateLength ();

    int offset;
    int maxx_size,maxx_read,real_len,length;
    int DS_state;
    double u_up;
    double u_down;
    float lpanning, rpanning, hidamp, alpha_hidamp, convlength, oldl;
    float *rbuf, *buf, *lxn;
    float *templ, *tempr;

    float level,fb, feedback;
    float levpanl,levpanr;

    SNDFILE *infile;
    SF_INFO sfinfo;

    //Parametrii reali

    class Resample *M_Resample;
    class Resample *U_Resample;
    class Resample *D_Resample;

    fft_filter impulse;


};


#endif
