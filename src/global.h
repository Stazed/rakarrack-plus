/*
  rakarrack - a guitar efects software

  global.h  -  Variable Definitions and functions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#ifndef DXEMU_H
#define DXEMU_H


#define D_PI 6.283185f
#define PI 3.141598f
#define LOG_10 2.302585f
#define LOG_2  0.693147f
#define LN2R 1.442695041f
#define CNST_E  2.71828182845905f
#define AMPLITUDE_INTERPOLATION_THRESHOLD 0.0001f
#define FF_MAX_VOWELS 6
#define FF_MAX_FORMANTS 12
#define FF_MAX_SEQUENCE 8
#define MAX_FILTER_STAGES 5
#define RND (rand()/(RAND_MAX+1.0))
#define RND1 (((float) rand())/(((float) RAND_MAX)+1.0f))
#define F2I(f,i) (i)=((f>0) ? ( (int)(f) ) :( (int)(f-1.0f) ))
#define dB2rap(dB) (float)((expf((dB)*LOG_10/20.0f)))
#define rap2dB(rap) (float)((20*log(rap)/LOG_10))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define INTERPOLATE_AMPLITUDE(a,b,x,size) ( (a) + ( (b) - (a) ) * (float)(x) / (float) (size) )
#define ABOVE_AMPLITUDE_THRESHOLD(a,b) ( ( 2.0f*fabs( (b) - (a) ) / ( fabs( (b) + (a) + 0.0000000001f) ) ) > AMPLITUDE_INTERPOLATION_THRESHOLD )
#define POLY 8
#define DENORMAL_GUARD 1e-18f	// Make it smaller until CPU problem re-appears
#define SwapFourBytes(data) ( (((data) >> 24) & 0x000000ff) | (((data) >> 8) & 0x0000ff00) | (((data) << 8) & 0x00ff0000) | (((data) << 24) & 0xff000000) )
#define D_NOTE          1.059463f
#define LOG_D_NOTE      0.057762f
#define D_NOTE_SQRT     1.029302f
#define MAX_PEAKS 8
#define MAX_ALIENWAH_DELAY 100
#define ATTACK  0.175f  //crossover time for reverse delay
#define MAX_DELAY 2	// Number of Seconds
#define MAXHARMS  8    // max number of harmonics available
#define MAX_PHASER_STAGES 12
#define MAX_CHORUS_DELAY 250.0f	//ms
#define LN2                       (1.0f)  //Uncomment for att/rel to behave more like a capacitor.
#define MUG_CORR_FACT  0.4f
//Crunch waveshaping constants
#define Thi		0.67f			//High threshold for limiting onset
#define Tlo		-0.65f			//Low threshold for limiting onset
#define Tlc		-0.6139445f		//Tlo + sqrt(Tlo/500)
#define Thc		0.6365834f	        //Thi - sqrt(Thi/600)
#define CRUNCH_GAIN	100.0f			//Typical voltage gain for most OD stompboxes
#define DIV_TLC_CONST   0.002f			// 1/300
#define DIV_THC_CONST	0.0016666f		// 1/600 (approximately)
//End waveshaping constants
#define D_FLANGE_MAX_DELAY	0.055f			// Number of Seconds  - 50ms corresponds to fdepth = 20 (Hz). Added some extra for padding
#define LFO_CONSTANT		9.765625e-04		// 1/(2^LOG_FMAX - 1)
#define LOG_FMAX		10.0f			//  -- This optimizes LFO sweep for useful range.
#define MINDEPTH		20.0f			// won't allow filter lower than 20Hz
#define MAXDEPTH		15000.0f		// Keeps delay greater than 2 samples at 44kHz SR
#define MAX_EQ_BANDS 16
#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define CLOSING 4
#define ENV_TR 0.0001f
#define HARMONICS 11
#define REV_COMBS 8
#define REV_APS 4
#define MAX_SFILTER_STAGES 12

#define TEMPBUFSIZE 1024
#define MAX_PDATA_SIZE 50
#define USERFILE 100    // used by Convolotron, Echotron, Reverbtron to indicate user file in setfile()
#define MAX_PRESET_SIZE 19  // fileio.C presets_default[][]
#define JACK_RINGBUFFER_SIZE 16384 // Default size for ringbuffer
#define SPACE_BAR 32    // for space bar pressed

inline int Dry_Wet (int x) {return 127 - x;}

typedef union {
    float f;
    long i;
} ls_pcast32;

/*
static inline float f_pow2(float x)
{
        ls_pcast32 *px, tx, lx;
        float dx;

        px = (ls_pcast32 *)&x; // store address of float as long pointer
        tx.f = (x-0.5f) + (3<<22); // temporary value for truncation
        lx.i = tx.i - 0x4b400000; // integer power of 2
        dx = x - (float)lx.i; // float remainder of power of 2

        x = 1.0f + dx * (0.6960656421638072f + // cubic apporoximation of 2^x
                   dx * (0.224494337302845f +  // for x in the range [0, 1]
                   dx * (0.07944023841053369f)));
        (*px).i += (lx.i << 23); // add integer power of 2 to exponent

        return (*px).f;
}
*/
/*
#define P2a0  1.00000534060469
#define P2a1   0.693057900547259
#define P2a2   0.239411678986933
#define P2a3   0.0532229404911678
#define P2a4   0.00686649174914722
#include <math.h>
static inline float f_pow2(float x)
{
float y,xx, intpow;
long xint = (int) fabs(ceil(x));
xx = x - ceil(x);
xint = xint<<xint;
if(x>0) intpow = (float) xint;
else intpow = 1.0f;

y = intpow*(xx*(xx*(xx*(xx*P2a4 + P2a3) + P2a2) + P2a1) + P2a0);

return y;

}
*/

//The below pow function really works & is good to 16 bits, but is it faster than math lib powf()???
//globals
#include <math.h>
static const float a[5] = { 1.00000534060469, 0.693057900547259, 0.239411678986933, 0.0532229404911678, 0.00686649174914722 };
//lookup for positive powers of 2
static const float pw2[25] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f, 16384.0f, 32768.0f, 65536.0f, 131072.0f, 262144.0f, 524288.0f, 1048576.0f, 2097152.0f, 4194304.0f, 8388608.0f, 16777216.0f};
//negative powers of 2, notice ipw2[0] will never be indexed.
static const float ipw2[25] = {1.0, 5.0e-01, 2.5e-01, 1.25e-01, 6.25e-02, 3.125e-02, 1.5625e-02, 7.8125e-03, 3.90625e-03, 1.953125e-03, 9.765625e-04, 4.8828125e-04, 2.44140625e-04, 1.220703125e-04, 6.103515625e-05, 3.0517578125e-05, 1.52587890625e-05, 7.62939453125e-06, 3.814697265625e-06, 1.9073486328125e-06, 9.5367431640625e-07, 4.76837158203125e-07, 2.38418579101562e-07, 1.19209289550781e-07, 5.96046447753906e-08};

static inline float f_pow2(float x)
{
    float y = 0.0f;

    if(x >=24)
    {
        return pw2[24];
    }
    else if (x <= -24.0f)
    {
        return ipw2[24];
    }
    else
    {
        float whole =  ceilf(x);
        int xint = (int) whole;
        x = x - whole;

        if (xint>=0)
        {
            y = pw2[xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);
        }
        else
        {
            y = ipw2[-xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);
        }

        return y;
    }
}

#define f_exp(x) f_pow2(x * LN2R)

#include <signal.h>
#include <dirent.h>
#include <search.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FPreset.h"

/* Global GUI items */
#include <FL/Fl_Tiled_Image.H>
class RKRGUI;   // forward declaration
class RKR;      // forward declaration

extern RKRGUI *rgui;    // define in main
extern RKR *rkr;        // define in rakarrack.cxx 
extern Fl_Tiled_Image *back; 
extern Fl_Color leds_color; 
extern Fl_Color back_color; 
extern Fl_Color fore_color; 
extern Fl_Color label_color;

/* These are used by settings midi scroll for identifying the scroll widgets - user data (void *) */
const int c_bank_used = 1000;
const int c_preset_used = 2000;
const int c_bank_number = 8000;

/* For file .rkrb extension size usually for removing the extension from bank names */
const unsigned c_rkrb_ext_size = 5;
/* For file .rkr extension size usually for removing the extension from preset names */
const unsigned c_rkr_ext_size = 4;

//TODO: move these values into the RKR object
extern int Pexitprogram, preset;
extern int preserve;  // See the PRSRV_* constants below.
extern int commandline, gui;
extern int exitwithhelp, nojack;
extern int error_num;
extern int needtoloadstate;
extern int needtoloadbank;
extern int stecla;
extern int looper_lqua;
extern char *s_uuid;
extern char *statefile;
extern char *filetoload;
extern char *banktoload;
extern char *jack_client_name;

/* The font size adjustments */
extern int global_font_size;
const int C_DEFAULT_FONT_SIZE = 10;
const int C_DONT_CHANGE_FONT_SIZE = 0;

/* Milliseconds - used for quality changes by usleep().
   The amounts are much greater than necessary for the 
   delete and re-initialize. But the delay is useful to
   let the user know that something has changed and it
   is not real time safe. */
const unsigned C_MILLISECONDS_25 = 250000;   // 1/4 second
const unsigned C_MILLISECONDS_50 = 500000;   // 1/2 second

/* Each effect has a unique numeric identifier. */
enum EffectIndex {
    FX_EQ1 = 0,
    FX_Compressor,
    FX_Distortion,
    FX_Overdrive,
    FX_Echo,
    FX_Chorus,
    FX_Phaser,
    FX_Flanger,
    FX_Reverb,
    FX_EQ2,
    FX_WahWah,
    FX_Alienwah,
    FX_Cabinet,
    FX_Pan,
    FX_Harmonizer,
    FX_MusDelay,
    FX_Gate,
    FX_Derelict,
    FX_APhaser,
    FX_Valve,
    FX_DFlange,
    FX_Ring,
    FX_Exciter,
    FX_DistBand,
    FX_Arpie,
    FX_Expander,
    FX_Shuffle,
    FX_Synthfilter,
    FX_VaryBand,
    FX_Convolotron,
    FX_Looper,
    FX_MuTroMojo,
    FX_Echoverse,
    FX_CoilCrafter,
    FX_ShelfBoost,
    FX_Vocoder,
    FX_Sustainer,
    FX_Sequence,
    FX_Shifter,
    FX_StompBox,
    FX_Reverbtron,
    FX_Echotron,
    FX_StereoHarm,
    FX_CompBand,
    FX_OpticalTrem,
    FX_Vibe,
    FX_Infinity,

    /* "Main" isn't really an affect, we just use this to reference global
       settings like master volume and gain.  It must be the last element in
       the enum. */
    FX_Main
};

const int FXCount = FX_Main + 1;

/* Parameter index. */
enum ParmIndex {
    // Exciter, CoilCrafter, ShelfBoost, Sustainer, StompBox, EQ1/EQ2/Cabinet
    Parm_Volume = 0,

    // WahWah, Distortion/Overdrive, Harmonizer, Chorus/Flanger, Phaser,
    // Alienwah, MusDelay, Reverb, Pan, Echo, Derelict, Valve, Ring, DistBand,
    // Arpie, Convolotron, Vocoder, Echoverse, Shifter, DFlange
    Parm_Pan = 1,

    // WahWah, Distortion/Overdrive, Harmonizer, Chorus/Flanger, Phaser,
    // Alienwah, MusDelay, Reverb, Pan, Echo, Derelict
    // APhaser, Valve, Ring, DistBand, Arpie, Shuffle, Synthfilter, VaryBand,
    // MuTroMojo, Looper, Convolotron, Vocoder, Echoverse, Shifter,
    // Reverbtron, Echotron, StereoHarm, CompBand, Infinity, DFlange
    Parm_DryWet = 0,

    // Chorus/Flanger, Phaser, WahWah, Alienwah, Distortion/Overdrive, APhaser,
    // Synthfilter, MuTroMojo, Pan
    Parm_LFOFreq = 2,

    // Distortion/Overdrive, Derelict, Valve, DistBand
    Parm_Drive = 3,
    Parm_Level = 4,

    // Distortion/Overdrive, Derelict, DFlange, Valve, Ring, DistBand
    Parm_Dist_LRCross = 2,

    // Distortion/Overdrive, Reverb, Derelict
    Parm_LowPassFilter = 7,
    Parm_HighPassFilter = 8,

    // Distortion/Overdrive
    Parm_Dist_Octave = 12,

    // Distortion/Overdrive, Derelict
    Parm_Distortion_Type = 5,

    // Harmonizer
    Parm_Interval = 3,
    Parm_Freq = 4,
    Parm_Note = 6,
    Parm_FGain = 8,
    Parm_Harmonizer_Type = 7,
    Parm_Harmonizer_Select = 5,

    // Chorus/Flanger, Phaser, Alienwah, Vibe
    Parm_LRCross = 9,

    // Echo, MusDelay, Arpie, Echoverse
    Parm_Feedback = 5,

    // Echo, Arpie, MusDelay, Echoverse
    Parm_Echo_LRCross = 4,

    // WahWah, Alienwah, Phaser, Flanger/Chorus, MuTroMojo
    Parm_Depth = 6,

    // WahWah
    Parm_AmpSns = 7,
    Parm_AmpSnsNS = 8,
    Parm_AmpSnsSmooth = 9,

    // Phaser, Alienwah, APhaser, Synthfilter, Vibe
    Parm_Phaser_Feedback = 7,

    // Phaser
    Parm_Phase = 11,

    // Alienwah
    Parm_Alien_Phase = 10,
    Parm_Alien_Delay = 8,

    // Chorus/Flanger
    Parm_Chorus_Feedback = 8,
    Parm_Chorus_Delay = 7,

    // Chorus/Flanger, Alienwah, APhaser, MuTroMojo, Pan, Phaser, Synthfilter,
    // WahWah
    Parm_LFOType = 4,

    // Chorus/Flanger, Phaser, Alienwah, APhaser, Synthfilter, MuTroMojo,
    // WahWah, Pan
    Parm_LFOStereo = 5,

    // Chorus/Flanger, Phaser, WahWah, Alienwah, APhaser, MuTroMojo,
    // Synthfilter, Pan
    Parm_LFORandomness = 3,

    // MusDelay
    Parm_MusDelay_Feedback2 = 9,

    // MusDelay
    Parm_Pan2 = 7,
    Parm_Gain1 = 11,
    Parm_Gain2 = 12,
    Parm_MusDelay_Tempo = 10,

    // DFlange
    Parm_DFlange_Depth = 3,
    Parm_DFlange_Width = 4,
    Parm_DFlange_Offset = 5,
    Parm_DFlange_Feedback = 6,
    Parm_DFlange_HiDamp = 7,
    Parm_DFlange_LFOFreq = 10,
    Parm_DFlange_LFOStereo = 11,
    Parm_DFlange_LFOType = 12,
    Parm_DFlange_LFORandomness = 13,

    // APhaser, Synthfilter
    Parm_Distortion = 1,
    Parm_Width = 6,
    Parm_APhaser_Depth = 11,

    // APhaser
    Parm_Offset = 9,

    // Derelict
    Parm_RFreq = 9,
    Parm_Octave = 11,

    // Compressor, Gate
    Parm_Threshold = 1,

    // Compressor
    Parm_Ratio = 2,
    Parm_Output = 3,
    Parm_AttTime = 4,
    Parm_RelTime = 5,
    Parm_Knee = 7,

    // Main
    Parm_FractionBypass = 1,

    // Main, Shifter, StereoHarm, Harmonizer
    Parm_InputGain = 2,

    // Valve
    Parm_Q = 10,
    Parm_Presence = 12,
    Parm_Valve_LowPass = 6,
    Parm_Valve_HighPass = 7,

    // Ring
    Parm_Input = 11,
    Parm_Ring_Level = 3,
    Parm_Ring_Depth = 4,
    Parm_Ring_Freq = 5,
    Parm_Ring_Sine = 7,
    Parm_Ring_Tri = 8,
    Parm_Ring_Saw = 9,
    Parm_Ring_Square = 10,

    // Exciter
    Parm_Exciter_LowPassFilter = 11,
    Parm_Exciter_HighPassFilter = 12,
    Parm_Exciter_H1 = 1,
    Parm_Exciter_H2 = 2,
    Parm_Exciter_H3 = 3,
    Parm_Exciter_H4 = 4,
    Parm_Exciter_H5 = 5,
    Parm_Exciter_H6 = 6,
    Parm_Exciter_H7 = 7,
    Parm_Exciter_H8 = 8,
    Parm_Exciter_H9 = 9,
    Parm_Exciter_H10 = 10,

    // DistBand
    Parm_DistBand_TypeLow = 5,
    Parm_DistBand_TypeMid = 6,
    Parm_DistBand_TypeHigh = 7,
    Parm_DistBand_LowVol = 8,
    Parm_DistBand_MidVol = 9,
    Parm_DistBand_HighVol = 10,
    Parm_DistBand_Cross1 = 12,
    Parm_DistBand_Cross2 = 13,

    // Arpie, Echoverse, Echo
    Parm_Arpie_Reverse = 7,

    // Arpie
    Parm_Arpie_Tempo = 2,

    // Arpie, Echoverse, Echo
    Parm_Arpie_LRDelay = 3,

    // Arpie, Convolotron, Echoverse, Reverbtron, Echotron, Echo, MusDelay
    Parm_Arpie_HiDamp = 6,

    // Expander
    Parm_Expander_Threshold = 1,
    Parm_Expander_Shape = 2,
    Parm_Expander_OutGain = 7,

    // Expander, Gate
    Parm_Expander_LowPassFilter = 5,
    Parm_Expander_HighPassFilter = 6,

    // Expander, Shifter, Gate
    Parm_Expander_Attack = 3,
    Parm_Expander_Decay = 4,

    // Shuffle
    Parm_Shuffle_GainLow = 1,
    Parm_Shuffle_GainMidLow = 2,
    Parm_Shuffle_GainMidHigh = 3,
    Parm_Shuffle_GainHigh = 4,
    Parm_Shuffle_Cross1 = 5,
    Parm_Shuffle_Cross2 = 6,
    Parm_Shuffle_Cross3 = 7,
    Parm_Shuffle_Cross4 = 8,

    // Shuffle, Infinity, Harmonizer
    Parm_Shuffle_Q = 9,

    // Synthfilter
    Parm_Synthfilter_Envelope = 12,
    Parm_Synthfilter_Attack = 13,
    Parm_Synthfilter_Release = 14,
    Parm_Synthfilter_Bandwidth = 15,

    // VaryBand
    Parm_VaryBand_LFOType = 2,
    Parm_VaryBand_LFOStereo = 3,
    Parm_VaryBand_LFO2Freq = 4,
    Parm_VaryBand_LFO2Type = 5,
    Parm_VaryBand_LFO2Stereo = 6,
    Parm_VaryBand_Cross1 = 7,
    Parm_VaryBand_Cross2 = 8,
    Parm_VaryBand_Cross3 = 9,

    // VaryBand, OpticalTrem, Vibe
    Parm_VaryBand_LFOFreq = 1,

    // MuTroMojo, ShelfBoost
    Parm_MuTroMojo_Q = 1,

    // MuTroMojo, Sequence
    Parm_Range = 14,

    // MuTroMojo,
    Parm_MuTroMojo_AmpSns = 7,
    Parm_MuTroMojo_AmpSnsInv = 8,
    Parm_MuTroMojo_AmpSmooth = 9,
    Parm_MuTroMojo_LowPassLvl = 10,
    Parm_MuTroMojo_BandPassLvl = 11,
    Parm_MuTroMojo_HighPassLvl = 12,
    Parm_MuTroMojo_MinFreq = 15,

    // Looper
    Parm_Looper_Play = 1,
    Parm_Looper_Stop = 2,
    Parm_Looper_Record = 3,
    Parm_Looper_Clear = 4,
    Parm_Looper_Fade1 = 6,
    Parm_Looper_Track1 = 7,
    Parm_Looper_Track2 = 8,
    Parm_Looper_Autoplay = 9,
    Parm_Looper_Fade2 = 10,
    Parm_Looper_Record1 = 11,
    Parm_Looper_Record2 = 12,
    Parm_Looper_Reverse = 5,
    Parm_Looper_Tempo = 14,

    // Convolotron, Reverbtron
    Parm_Convolotron_Level = 7,

    // Convolotron, Reverbtron, Echotron
    Parm_Convolotron_Length = 3,
    Parm_Convolotron_Feedback = 10,

    // CoilCrafter
    Parm_CoilCrafter_Freq1 = 3,
    Parm_CoilCrafter_Q1 = 4,
    Parm_CoilCrafter_Freq2 = 5,
    Parm_CoilCrafter_Q2 = 6,
    Parm_CoilCrafter_HighPassFilter = 7,

    // ShelfBoost
    Parm_ShelfBoost_Freq = 2,

    // ShelfBoost, StompBox
    Parm_ShelfBoost_Gain = 4,

    // Vocoder
    Parm_Vocoder_Muffle = 2,
    Parm_Vocoder_Q = 3,
    Parm_Vocoder_Input = 4,
    Parm_Vocoder_Level = 5,
    Parm_Vocoder_Ring = 6,

    // Echoverse, Echo
    Parm_Echoverse_Delay = 2,

    // Echoverse
    Parm_Echoverse_ExStereo = 9,

    // Sustainer
    Parm_Sustain = 1,

    // Sequence
    Parm_Sequence_1 = 0,
    Parm_Sequence_2 = 1,
    Parm_Sequence_3 = 2,
    Parm_Sequence_4 = 3,
    Parm_Sequence_5 = 4,
    Parm_Sequence_6 = 5,
    Parm_Sequence_7 = 6,
    Parm_Sequence_8 = 7,
    Parm_Sequence_DryWet = 8,
    Parm_Sequence_Tempo = 9,
    Parm_Sequence_Q = 10,
    Parm_Sequence_Amp = 11,
    Parm_Sequence_StereoDiff = 12,
    Parm_Sequence_Mode = 13,

    // Shifter
    Parm_Shifter_Threshold = 5,
    Parm_Shifter_Interval = 6,
    Parm_Shifter_Whammy = 9,

    // StompBox
    Parm_StompBox_High = 1,
    Parm_StompBox_Mid = 2,
    Parm_StompBox_Low = 3,
    Parm_StompBox_Mode = 5,

    // Reverbtron
    Parm_Reverbtron_Fade = 1,
    Parm_Reverbtron_InitialDelay = 5,
    Parm_Reverbtron_Stretch = 9,

    // Reverbtron, Echotron
    Parm_Reverbtron_Pan = 11,

    // Echotron
    Parm_Echotron_Depth = 1,
    Parm_Echotron_Width = 2,
    Parm_Echotron_Tempo = 5,
    Parm_Echotron_LRCross = 7,
    Parm_Echotron_LFOStereo = 9,
    Parm_Echotron_LFOType = 14,

    // StereoHarm
    Parm_StereoHarm_GainLeft = 1,
    Parm_StereoHarm_IntervalLeft = 2,
    Parm_StereoHarm_ChromeLeft = 3,
    Parm_StereoHarm_GainRight = 4,
    Parm_StereoHarm_IntervalRight = 5,
    Parm_StereoHarm_ChromeRight = 6,
    Parm_StereoHarm_Select = 7,
    Parm_StereoHarm_Note = 8,
    Parm_StereoHarm_Type = 9,
    Parm_StereoHarm_LRCross = 11,

    // CompBand
    Parm_CompBand_LowRatio = 1,
    Parm_CompBand_MidLowRatio = 2,
    Parm_CompBand_MidHighRatio = 3,
    Parm_CompBand_HighRatio = 4,
    Parm_CompBand_LowThreshold = 5,
    Parm_CompBand_MidLowThreshold = 6,
    Parm_CompBand_MidHighThreshold = 7,
    Parm_CompBand_HighThreshold = 8,
    Parm_CompBand_Cross1 = 9,
    Parm_CompBand_Cross2 = 10,
    Parm_CompBand_Cross3 = 11,
    Parm_CompBand_Level = 12,

    // OpticalTrem
    Parm_OpticalTrem_Depth = 0,

    // OpticalTrem, Vibe
    Parm_OpticalTrem_LFOType = 3,

    // OpticalTrem, Vibe
    Parm_OpticalTrem_Randomness = 2,
    Parm_OpticalTrem_LFOStereo = 4,
    Parm_OpticalTrem_Pan = 5,

    // Vibe
    Parm_Vibe_Width = 0,
    Parm_Vibe_DryWet = 6,
    Parm_Vibe_Depth = 8,

    // Infinity
    Parm_Infinity_P1 = 1,
    Parm_Infinity_P2 = 2,
    Parm_Infinity_P3 = 3,
    Parm_Infinity_P4 = 4,
    Parm_Infinity_P5 = 5,
    Parm_Infinity_P6 = 6,
    Parm_Infinity_P7 = 7,
    Parm_Infinity_P8 = 8,
    Parm_Infinity_StartFreq = 10,
    Parm_Infinity_EndFreq = 11,
    Parm_Infinity_Rate = 12,
    Parm_Infinity_StereoDiff = 13,
    Parm_Infinity_Subdiv = 14,
    Parm_Infinity_Autopan = 15,

    // Gate
    Parm_Gate_Range = 2,
    Parm_Hold = 7,

    // Pan
    Parm_Extra = 6,

    // Reverb
    Parm_Reverb_Time = 2,
    Parm_Reverb_InitialDelay = 3,
    Parm_Reverb_DelayFeedback = 4,
    Parm_Reverb_RoomSize = 11,
    Parm_Reverb_LoHiDamp = 9,

    // Reverbtron
    Parm_Reverbtron_Diffusion = 15,
    Parm_Reverbtron_LowPassFilter = 14,

    /* This value is used to determine the size of the "preserve" array.  It
       must be large enough to accommodate any parameter number used in a
       changepar() method.  It's currently 63 because of the number of EQ
       params. */
    Parm_Last = 63
};

const int ParmCount = Parm_Last + 1;

#endif


