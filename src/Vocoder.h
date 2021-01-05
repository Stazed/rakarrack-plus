/*
  Vocoder.h - Vocoder Effect

  Author: Ryam Billing & Josep Andreu

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

#ifndef VOCODER_H
#define VOCODER_H

#include <vector>
#include "AnalogFilter.h"
#include "Resample.h"

#define VOCODER_PRESET_SIZE 7

enum Vocoder_Index
{
    Vocoder_DryWet = 0,
    Vocoder_Pan,
    Vocoder_Smear,
    Vocoder_Q,
    Vocoder_Input,
    Vocoder_Level,
    Vocoder_Ring
};

class Vocoder
{
public:
    Vocoder (float *auxresampled_,int bands, int DS, int uq, int dq, double sample_rate, uint32_t intermediate_bufsize);
    ~Vocoder ();
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
    void adjust(int DS, double sample_rate);

    /* Change quality */
    std::vector<int> save_parameters();
    void reset_parameters(std::vector<int> parameters);

    int Ppreset;
    float outvolume;
    float vulevel;

    float *auxresampled;

private:

    void setvolume (int Pvolume);
    void setpanning (int Ppanning);
    void init_filters();
    void adjustq(int value);
    void   setbands(int numbands, float startfreq, float endfreq);
    
    int VOC_BANDS;
    uint32_t PERIOD;
    double fSAMPLE_RATE;
    int DS_state;
    
    //Parametrii
    int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
    int Ppanning;	//Panning
    int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
    int Plevel;		//This should only adjust the level of the IR effect, and not wet/dry mix
    int Pinput;
    int Pband;
    int Pmuffle;
    int Pqq;
    int Pring;

    int nPERIOD;
    int nSAMPLE_RATE;
    float nRATIO;

    float ncSAMPLE_RATE;
    float nfSAMPLE_RATE;

    double u_up;
    double u_down;


    float ringworm;
    float lpanning, rpanning, input,level;
    float alpha,beta,prls,gate;
    float compeak, compg, compenv, oldcompenv, calpha, cbeta, cthresh, cratio, cpthresh;
    float *tmpl, *tmpr;
    float *tsmpsl, *tsmpsr;
    float *tmpaux;
    struct fbank
    {
        float sfreq, sq,speak,gain,oldgain;
        AnalogFilter *l, *r, *aux;
        
        fbank():
            sfreq(),
            sq(),
            speak(),
            gain(),
            oldgain(),
            l(),
            r(),
            aux() {}
    } *filterbank;

    AnalogFilter *vhp, *vlp;
    float* interpbuf; //buffer for filters

    Resample *U_Resample;
    Resample *D_Resample;
    Resample *A_Resample;

    class FPreset *Fpre;

};


#endif
