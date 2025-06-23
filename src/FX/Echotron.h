/*
  Echotron.h - Convolution-based Echo Effect

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

#ifndef ECHOTRON_H
#define ECHOTRON_H

#include <vector>
#include "Effect.h"
#include "../EFX_common/AnalogFilter.h"
#include "../EFX_common/EffectLFO.h"
#include "../EFX_common/delayline.h"
#include "../EFX_common/RBFilter.h"

#define ECHOTRONLV2_URI "https://github.com/Stazed/rakarrack-plus#Echotron"
#define ECHOTRON_NAME "RakarrackPlus Echotron"

#define  ECHOTRON_F_SIZE   128       //Allow up to 150 points in the file
#define  ECHOTRON_MAXFILTERS  32      //filters available

const int C_ECHOTRON_PARAMETERS = 16;

enum Echotron_Index
{
    Echotron_DryWet = 0,
    Echotron_Depth,
    Echotron_LFO_Width,
    Echotron_Taps,
    Echotron_User_File,
    Echotron_Tempo,
    Echotron_Damp,
    Echotron_LR_Cross,
    Echotron_Set_File,
    Echotron_LFO_Stereo,
    Echotron_Feedback,
    Echotron_Pan,
    Echotron_Mod_Delay,
    Echotron_Mod_Filter,
    Echotron_LFO_Type,
    Echotron_Filters
};

static const char *echotron_parameters[] = 
{
    strdup(NTS(Echotron_DryWet).c_str()), "Dry/Wet",  "DRYWET", 
    strdup(NTS(Echotron_Depth).c_str()), "Filter Depth",  "DEPTH", 
    strdup(NTS(Echotron_LFO_Width).c_str()), "LFO Width",  "WIDTH", 
    strdup(NTS(Echotron_Taps).c_str()), "Number of Taps",  "LENGTH", 
    strdup(NTS(Echotron_Tempo).c_str()), "Tempo",  "TEMPO", 
    strdup(NTS(Echotron_Damp).c_str()), "Dampening",  "DAMP", 
    strdup(NTS(Echotron_LR_Cross).c_str()), "Left/Right Crossover",  "LRC", 
    strdup(NTS(Echotron_LFO_Stereo).c_str()), "LFO L/R Delay",  "STDF", 
    strdup(NTS(Echotron_Feedback).c_str()), "Feedback",  "FB", 
    strdup(NTS(Echotron_Pan).c_str()), "Pan",  "PAN", 
    strdup(NTS(Echotron_Mod_Delay).c_str()), "Modulate Delays",  "MD", 
    strdup(NTS(Echotron_Mod_Filter).c_str()), "Modulate Filters",  "MF", 
    strdup(NTS(Echotron_LFO_Type).c_str()), "LFO Type",  "LFOTYPE", 
    strdup(NTS(Echotron_Filters).c_str()), "Activate Filters",  "AF"
};

class DlyFile
{
public:
    //arrays of parameters from text file:
    double fPan[ECHOTRON_F_SIZE];  //1+Pan from text file
    double fTime[ECHOTRON_F_SIZE];
    double fLevel[ECHOTRON_F_SIZE];
    double fLP[ECHOTRON_F_SIZE];
    double fBP[ECHOTRON_F_SIZE];
    double fHP[ECHOTRON_F_SIZE];
    double fFreq[ECHOTRON_F_SIZE];
    double fQ[ECHOTRON_F_SIZE];
    int iStages[ECHOTRON_F_SIZE];
    double subdiv_dmod;
    double subdiv_fmod;
    int f_qmode;

    float rtime[ECHOTRON_F_SIZE];
    float ltime[ECHOTRON_F_SIZE];
    float ldata[ECHOTRON_F_SIZE];
    float rdata[ECHOTRON_F_SIZE];

    int fLength;
    char Filename[128];
    std::string Description;
    
    DlyFile():
        fPan(),
        fTime(),
        fLevel(),
        fLP(),
        fBP(),
        fHP(),
        fFreq(),
        fQ(),
        iStages(),
        subdiv_dmod(0.0),
        subdiv_fmod(0.0),
        f_qmode(0),
        rtime(),
        ltime(),
        ldata(),
        rdata(),

        fLength(0.0),
        Filename(),
        Description() {}

};

#define INVALID_DELAY_FILE_RANGE -1

enum Echotron_file_errors
{
    Dly_Open = 4,
    Dly_Pan,
    Dly_Time,
    Dly_Level,
    Dly_LP,
    Dly_BP,
    Dly_HP,
    Dly_Freq,
    Dly_Q,
    Dly_Stages
};

class Echotron :public Effect
{
public:
    Echotron (double sample_rate, uint32_t intermediate_bufsize);
    ~Echotron ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    int get_number_efx_parameters () {return C_ECHOTRON_PARAMETERS;};
    void set_random_parameters();
    
#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif // LV2
    virtual void LV2_parameters(std::string &s_buf, int type);
    virtual std::string get_URI(int) { return ECHOTRONLV2_URI; };
    virtual std::string get_name(int) {return ECHOTRON_NAME; };
    
    void initialize();
    void clear_initialize();
    int setfile (int value);
    bool check_delay_file_ranges(double value, int item);
    DlyFile loadfile(char* path);
    void applyfile(const DlyFile &);
    int get_file_length() {return File.fLength;};
    char *get_file_name() {return File.Filename;};
    void set_user_files(const std::vector<User_Files> &dly_files){DLY_Files = dly_files;};

    char Filename[128];
    DlyFile File;
    int error;

private:

    void setvolume (int _Pvolume);
    void setpanning (int value);
    void sethidamp (int _Phidamp);
    void setfb(int value);
    void init_params();
    void modulate_delay();
    void modulate_filters();
    DlyFile loaddefault();

    std::vector<User_Files> DLY_Files;

    float fSAMPLE_RATE;
    uint32_t PERIOD;
    float fPERIOD;

    //User input parameters
    int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing  //
    int Phidamp;
    int Puser;		//0,1//
    int Ptempo;		//Tempo, BPM//For stretching reverb responses
    int Filenum;
    int Pfb;		//-64 ... 64// amount of feedback
    int Pdepth;
    int Pwidth;
    int Pfilters;         //0 or 1// use or don't use filters in delay line
    int Pmodfilts;	//0 or 1// apply LFO to filter freqs
    int Pmoddly;		//0 or 1// apply LFO to delay time
    int Pstdiff;
    int Plength;

    int Llength; //limited length

    int offset;
    int maxx_size;


    //arrays of parameters from text file:
    /*
    float fPan[ECHOTRON_F_SIZE];  //1+Pan from text file
    float fTime[ECHOTRON_F_SIZE];
    float fLevel[ECHOTRON_F_SIZE];
    float fLP[ECHOTRON_F_SIZE];
    float fBP[ECHOTRON_F_SIZE];
    float fHP[ECHOTRON_F_SIZE];
    float fFreq[ECHOTRON_F_SIZE];
    float fQ[ECHOTRON_F_SIZE];
    int iStages[ECHOTRON_F_SIZE];
    float subdiv_dmod;
    float subdiv_fmod;
    int f_qmode;

    float rtime[ECHOTRON_F_SIZE];
    float ltime[ECHOTRON_F_SIZE];
    float ldata[ECHOTRON_F_SIZE];
    float rdata[ECHOTRON_F_SIZE];
    */

//end text configurable parameters

    int initparams;

    float ldmod, rdmod, oldldmod, oldrdmod, interpl, interpr;
    float dlyrange;

    float width, depth;
//    float lpanning, rpanning, hidamp, alpha_hidamp, convlength;
    float lpanning, rpanning, hidamp;

    delayline *lxn, *rxn;

//    float level,fb, rfeedback, lfeedback,levpanl,levpanr, lrcross, ilrcross;
    float fb, rfeedback, lfeedback, lrcross, ilrcross;
    float tempo_coeff;

    EffectLFO *lfo;
    EffectLFO *dlfo;

    class AnalogFilter *lpfl, *lpfr;	//filters
    float* interpbuf; //buffer for filters

    struct filterbank
    {
        float sfreq, sq,sLP,sBP,sHP, sStg;
        class RBFilter *l, *r;
        
        filterbank():
            sfreq(),
            sq(),
            sLP(),
            sBP(),
            sHP(),
            sStg(),
            l(),
            r() {}

    } filterbank[ECHOTRON_MAXFILTERS];


#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    char* FILENAME;     // for lv2 if need to re-initialize and reset .dly file
#endif // LV2
};


#endif
