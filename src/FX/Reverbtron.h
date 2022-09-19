/*
  Reverbtron.h - Reverbtron Effect

  Author: Ryan Billing & Josep Andreu

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

#ifndef REVERBTRON_H
#define REVERBTRON_H

#include <vector>
#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"
#include "../EFX_common/Resample.h"

#define REVTRONLV2_URI "https://github.com/Stazed/rakarrack-plus#Reverbtron"
#define REVTRON_NAME "RakarrackPlus Reverbtron"

const int C_REVTRON_PARAMETERS = 16;

enum Revtron_Index
{
    Revtron_DryWet = 0,
    Revtron_Fade,
    Revtron_Safe,
    Revtron_Length,
    Revtron_User_File,
    Revtron_I_Delay,
    Revtron_Damp,
    Revtron_Level,
    Revtron_Set_File,
    Revtron_Stretch,
    Revtron_Feedback,
    Revtron_Pan,
    Revtron_Ex_Stereo,
    Revtron_Shuffle,
    Revtron_LPF,
    Revtron_Diffusion
};

static const char *revtron_parameters[] = 
{
    strdup(NTS(Revtron_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Revtron_Fade).c_str()), "Fade",  "FADE", 
    strdup(NTS(Revtron_Safe).c_str()), "Safe",  "SAFE", 
    strdup(NTS(Revtron_Length).c_str()), "Length",  "LENGTH", 
    strdup(NTS(Revtron_I_Delay).c_str()), "Initial Delay",  "DELAY", 
    strdup(NTS(Revtron_Damp).c_str()), "Dampening",  "DAMP", 
    strdup(NTS(Revtron_Level).c_str()), "Level",  "LEVEL", 
    strdup(NTS(Revtron_Stretch).c_str()), "Time Stretch",  "STRETCH", 
    strdup(NTS(Revtron_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(Revtron_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Revtron_Ex_Stereo).c_str()), "Extra Stereo",  "ES", 
    strdup(NTS(Revtron_Shuffle).c_str()), "Shuffle",  "SH", 
    strdup(NTS(Revtron_LPF).c_str()), "Lowpass Filter",  "LPF", 
    strdup(NTS(Revtron_Diffusion).c_str()), "Diffusion",  "DIFFUSE"
};

class RvbFile
{
public:
	char Filename[128];
	float tdata[2000];
	float ftime[2000];
	int data_length;
	float maxtime;
	float maxdata;
	
	RvbFile ():
	    Filename(),
	    tdata(),
	    ftime(),
	    data_length(),
	    maxtime(),
	    maxdata() {}
};

class Reverbtron : public Effect
{
public:
    Reverbtron (int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsize);
    ~Reverbtron ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_REVTRON_PARAMETERS;};
    void set_random_parameters();

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return REVTRONLV2_URI; };
    virtual std::string get_name(int) {return REVTRON_NAME; };
    
    void initialize();
    void clear_initialize();
    int setfile (int value);
    void adjust(int DS, double sample_rate);
    RvbFile loadfile(char* filename);
    void applyfile(const RvbFile &file);
    void set_user_files(const std::vector<User_Files> &rvb_files){RVB_Files = rvb_files;};
    
    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

    char Filename[128];
    RvbFile File;

private:

    void setvolume (int _Pvolume);
    void setpanning (int value);
    void sethidamp (int _Phidamp);
    void setlpf (int value);
    void setfb(int value);
    void convert_time();
    RvbFile loaddefault();

    std::vector<User_Files> RVB_Files;
    uint32_t PERIOD;
    int nPERIOD;
    int nSAMPLE_RATE;
    double fSAMPLE_RATE;
    float nfSAMPLE_RATE;
    
    //Parametrii
    int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
    int Phidamp;
    int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
    int Plength;		//20... 1500// Set maximum number of IR points to calculate.
    int Puser;		//-64...64//Feedback.
    int Pstretch;		//-64 ... 64//For stretching reverb responses
    int Pidelay;         //0 ... 500ms// initial delay time
    int Filenum;
    int Psafe;
    int Pfb;		//-64 ... 64// amount of feedback
    int Pfade;
    int Pes;		//0 or 1// Add stereo spatialization
    int Prv;              //Shuffle
    int Plpf;
    int Pdiff;

    int imctr;
    int imax;
    int offset;
    int hoffset;
    int maxx_size;
    //int data_length;
    int error;// flag if error when loading file
    int Llength; //Plength but limited
//    int avgtime;
    int hrtf_size;
    int hlength;
    int DS_state;
    float nRATIO;


    int *time, *rndtime;
    double u_up;
    double u_down;

    //float fstretch, idelay, ffade, maxtime, maxdata, decay, diffusion;
    float fstretch, idelay, ffade, decay, diffusion;
    float lpanning, rpanning, hidamp, alpha_hidamp, convlength, oldl;
    //float *data, *lxn, *imdelay, *ftime, *tdata, *rnddata, *hrtf;
    float *data, *lxn, *imdelay, *rnddata, *hrtf;
    float *templ, *tempr;
    float level,fb, feedback,levpanl,levpanr;
    float roomsize;

    class Resample *U_Resample;
    class Resample *D_Resample;


    float* interpbuf; //buffer for filters
    class AnalogFilter *lpfl, *lpfr;	//filters

};


#endif
