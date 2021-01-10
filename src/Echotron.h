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

#include "AnalogFilter.h"
#include "EffectLFO.h"
#include "delayline.h"
#include "RBFilter.h"

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

class DlyFile
{
public:
    //arrays of parameters from text file:
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

    int fLength;
    char Filename[128];
    
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
        Filename() {}

};

class Echotron
{
public:
    Echotron (double sample_rate, uint32_t intermediate_bufsize);
    ~Echotron ();
    void out (float * efxoutl, float * efxoutr);
    void setpreset (int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    
#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2
    
    void initialize();
    void clear_initialize();
    int setfile (int value);
    DlyFile loadfile(char* path);
    void applyfile(DlyFile);
    
    int Ppreset;
    float outvolume;
    char Filename[128];
    DlyFile File;
    int error;

private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void sethidamp (int Phidamp);
    void setlpf (int Plpf);
    void setfb(int value);
    void init_params();
    void modulate_delay();
    void modulate_filters();
    DlyFile loaddefault();

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

    class FPreset *Fpre;

#ifdef LV2_SUPPORT
    char* FILENAME;     // for lv2 if need to re-initialize and reset .dly file
#endif // LV2
};


#endif
