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

#include <math.h>
#include <signal.h>
#include <dirent.h>
#include <search.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <iomanip>  // setprecision - FTSP
#include <stdio.h>
#include <stdlib.h>
#include <sstream>  // NTS
#include <FL/Fl_Tiled_Image.H>

// un-comment to activate stress test - to toggle stress testing, middle mouse click
// the random button. This should only be activated for testing purposes.
//#define STRESS_TEST_CHECK 1

class RKRGUI;   // forward declaration
class RKR;      // forward declaration

// These are defined in rakarrack.cxx
extern Fl_Tiled_Image *back; 
extern Fl_Color global_leds_color; 
extern Fl_Color global_back_color; 
extern Fl_Color global_fore_color; 
extern Fl_Color global_label_color;
extern int global_font_type;
extern int global_font_size;
extern int global_look_changed;

#ifdef NSM_SUPPORT
extern int global_gui_show;
const int CONST_GUI_SHOW = 1;
const int CONST_GUI_HIDE = 2;
const int CONST_GUI_OFF  = 0;
#endif

extern std::string nsm_preferences_file;

// Used by FPreset
extern std::string global_user_directory;

const int C_DEFAULT_FONT_SIZE = 10;

/* For file .rkrb extension size usually for removing the extension from bank names */
const unsigned c_rkrb_ext_size = 5;
/* For file .rkr extension size usually for removing the extension from preset names */
const unsigned c_rkr_ext_size = 4;
/* Flag to indicate the preset should not be changed */
const unsigned C_CHANGE_PRESET_OFF = 1000;
// Flag to indicate a CC Bank change 
const unsigned C_BANK_CHANGE_OFF = 1000;

/* Flag to indicate that drag should not be done */
const unsigned C_NO_DRAG = 1000;

extern int global_error_number;
extern char *jack_client_name;


/* Milliseconds - used for quality changes by usleep().
   The amounts are much greater than necessary for the 
   delete and re-initialize. But the delay is useful to
   let the user know that something has changed and it
   is not real time safe. */
const unsigned C_MILLISECONDS_25 = 250000;   // 1/4 second
const unsigned C_MILLISECONDS_50 = 500000;   // 1/2 second

inline int Dry_Wet (int x) {return 127 - x;}

/**
 * Number To String (NTS)
 * 
 * @param Number
 *      Float, Double, Integer, etc to convert.
 * 
 * @return 
 *      The converted number.
 */
template <typename T>
std::string NTS ( T Number )
{
    std::stringstream ss;
    ss << Number;
    return ss.str();
}

#define UD_NOT_SET "   "

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
#define f_exp(x) f_pow2(x * LN2R)

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
#define JACK_RINGBUFFER_SIZE 16384 // Default size for ringbuffer

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
static const float a[5] = { 1.00000534060469, 0.693057900547259, 0.239411678986933, 0.0532229404911678, 0.00686649174914722 };
//lookup for positive powers of 2
static const float pw2[25] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f, 16384.0f, 32768.0f, 65536.0f, 131072.0f, 262144.0f, 524288.0f, 1048576.0f, 2097152.0f, 4194304.0f, 8388608.0f, 16777216.0f};
//negative powers of 2, notice ipw2[0] will never be indexed.
static const float ipw2[25] = {1.0, 5.0e-01, 2.5e-01, 1.25e-01, 6.25e-02, 3.125e-02, 1.5625e-02, 7.8125e-03, 3.90625e-03, 1.953125e-03, 9.765625e-04, 4.8828125e-04, 2.44140625e-04, 1.220703125e-04, 6.103515625e-05, 3.0517578125e-05, 1.52587890625e-05, 7.62939453125e-06, 3.814697265625e-06, 1.9073486328125e-06, 9.5367431640625e-07, 4.76837158203125e-07, 2.38418579101562e-07, 1.19209289550781e-07, 5.96046447753906e-08};

static inline float f_pow2(float x)
{
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
        float y;
        float whole =  ceilf(x);
        int xint = (int) whole;
        x = x - whole;

        if (xint>=0)
        {
            if(xint >=24)
                return pw2[24];

            y = pw2[xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);
        }
        else
        {
            if (xint <= -24.0f)
                return ipw2[24];

            y = ipw2[-xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);
        }

        return y;
    }
}

template <typename T>
std::string FTSP(T value, int digits)
{   // For conversion from float to string precision
    std::ostringstream out;
    out << std::setprecision(digits) << std::fixed << value;
    return out.str();
}

enum RKR_Default_Colors
{
    RKR_background_color = 926365440,       // Dark grey
    RKR_labels_color = 255,                 // White
    RKR_buttons_color = 56,                 // Black
    RKR_leds_color = 12660480               // Light green
};

/**
 * The effect index used by switch(): case: and order number.
 */
enum EFX_Index
{
    EFX_EQ = 0,
    EFX_COMPRESSOR,
    EFX_DISTORTION,
    EFX_OVERDRIVE,
    EFX_ECHO,
    EFX_CHORUS,
    EFX_PHASER,
    EFX_FLANGER,
    EFX_REVERB,
    EFX_PARAMETRIC,
    EFX_WAHWAH,         // 10
    EFX_ALIENWAH,
    EFX_CABINET,
    EFX_PAN,
    EFX_HARMONIZER,
    EFX_MUSICAL_DELAY,
    EFX_NOISEGATE,
    EFX_DERELICT,
    EFX_ANALOG_PHASER,
    EFX_VALVE,
    EFX_DUAL_FLANGE,    // 20
    EFX_RING,
    EFX_EXCITER,
    EFX_DISTBAND,
    EFX_ARPIE,
    EFX_EXPANDER,
    EFX_SHUFFLE,
    EFX_SYNTHFILTER,
    EFX_VARYBAND,
    EFX_CONVOLOTRON,
    EFX_LOOPER,         // 30
    EFX_MUTROMOJO,
    EFX_ECHOVERSE,
    EFX_COILCRAFTER,
    EFX_SHELFBOOST,
    EFX_VOCODER,
    EFX_SUSTAINER,
    EFX_SEQUENCE,
    EFX_SHIFTER,
    EFX_STOMPBOX,
    EFX_REVERBTRON,     // 40
    EFX_ECHOTRON,
    EFX_STEREOHARM,
    EFX_COMPBAND,
    EFX_OPTICALTREM,
    EFX_VIBE,
    EFX_INFINITY,       // 46 - (0 to 46 = C_NUMBER_EFFECTS)
    
    // Indexes 47 - 68 are unused and can be used for expansion of rack effects.
    
    // This is the main window effect order. Must be hard coded to 69,
    // the last item of lv[70][20] bank.
    EFX_ORDER = 69,
    
    // The following are not rack effects but are used in MIDI
    // control within the same switch() statements and thus should
    // always be different from the EFX_Index for the rack effects.
    EFX_MASTER = 100,
    
    // These are part of the MIDI Implementation Chart CC 116 values.
    EFX_TAP_TEMPO_ON_OFF = 121,
    EFX_MIDI_CONVERTER_ON_OFF = 122,
    EFX_TUNER_ON_OFF = 123,
    EFX_MASTER_ON_OFF = 124
};

enum ASCII_Index
{
    ASCII_Space = 32,
    ASCII_Plus  = 43,
    ASCII_Minus = 45,
    ASCII_Zero  = 48,
    ASCII_One,
    ASCII_Two,
    ASCII_Three,
    ASCII_Four,
    ASCII_Five,
    ASCII_Six,
    ASCII_Seven,
    ASCII_Eight,
    ASCII_Nine
};

/**
 * The user_data highlight range for below mouse.
 */
const int C_UD_Highlight_Begin    = 0;      // After
const int C_UD_Highlight_End      = 14000;  // Before

/**
 * FLTK widget user_data is used for identifying widgets from user actions
 * as well as below mouse highlighting. Any items in the index range from
 * C_UD_Highlight_Begin to C_UD_Highlight_End will be highlighted when
 * below mouse. Also if active will become focused if not disabled in 
 * Settings/Misc.
 * 
 * The index items should not overlap, including those from function calculated
 * amounts of bank window preset buttons and custom MIDI table bank and presets.
 */
enum USER_DATA_index
{
    UD_Bank_Preset_Start    = 1,
    // The Bank Window user data values go from 1 to 60
    // Set in: BankWindowGui::make_window_banks()
    UD_Bank_Preset_End      = 60,

    UD_RKR_Highlight        = 91,       // For highlighting and focus when below mouse
    
    // Effect main rack labels
    UD_Label_1              = 770,
    UD_Label_2,
    UD_Label_3,
    UD_Label_4,
    UD_Label_5,
    UD_Label_6,
    UD_Label_7,
    UD_Label_8,
    UD_Label_9,
    UD_Label_10,            // 779
    
    // Custom MIDI table banks and presets
    UD_Bank_Used_Start      = 1000,
    UD_Bank_Used_End        = 1127,
    // In between these should not be used !!
    UD_Preset_Used_Start    = 2000,
    UD_Preset_Used_End      = 2127,
    
    // Echotron Delay file window group
    UD_delay_group          = 4000,
    
    UD_random_edit          = 5000,
    // Everything between here is used by random editor
    UD_random_end           = (5000 + 47), //  C_NUMBER_EFFECTS = 47
    
    // The user_data for RKR_Choice 'Preset' widgets. For use in identifying
    // the correct widget when user 'Insert' or 'Delete' key is used. For
    // adding or deleting effect user defined presets. The range is the same
    // as EFX_Index + 12000.
    UD_PRESET_EQ                   = (12000 + EFX_EQ),     // For clarity
    UD_PRESET_COMPRESSOR,
    UD_PRESET_DISTORTION,
    UD_PRESET_OVERDRIVE,
    UD_PRESET_ECHO,
    UD_PRESET_CHORUS,
    UD_PRESET_PHASER,
    UD_PRESET_FLANGER,
    UD_PRESET_REVERB,
    UD_PRESET_PARAMETRIC,
    UD_PRESET_WAHWAH,              // 12010
    UD_PRESET_ALIENWAH,
    UD_PRESET_CABINET,
    UD_PRESET_PAN,
    UD_PRESET_HARMONIZER,
    UD_PRESET_MUSICAL_DELAY,
    UD_PRESET_NOISEGATE,
    UD_PRESET_DERELICT,
    UD_PRESET_ANALOG_PHASER,
    UD_PRESET_VALVE,
    UD_PRESET_DUAL_FLANGE,         // 12020
    UD_PRESET_RING,
    UD_PRESET_EXCITER,
    UD_PRESET_DISTBAND,
    UD_PRESET_ARPIE,
    UD_PRESET_EXPANDER,
    UD_PRESET_SHUFFLE,
    UD_PRESET_SYNTHFILTER,
    UD_PRESET_VARYBAND,
    UD_PRESET_CONVOLOTRON,
    UD_PRESET_LOOPER,              // 12030
    UD_PRESET_MUTROMOJO,
    UD_PRESET_ECHOVERSE,
    UD_PRESET_COILCRAFTER,
    UD_PRESET_SHELFBOOST,
    UD_PRESET_VOCODER,
    UD_PRESET_SUSTAINER,
    UD_PRESET_SEQUENCE,
    UD_PRESET_SHIFTER,
    UD_PRESET_STOMPBOX,
    UD_PRESET_REVERBTRON,          // 12040
    UD_PRESET_ECHOTRON,
    UD_PRESET_STEREOHARM,
    UD_PRESET_COMPBAND,
    UD_PRESET_OPTICALTREM,
    UD_PRESET_VIBE,
    UD_PRESET_INFINITY,            // 12046
    
    // The max number of effects based on bank file saving
    // The range from 12047 to 12068 can be used for expansion
    UD_PRESET_MAX_EFFECTS          = 12068,
    
    // End Highlighted Items - after (UD_Highlight_End)
    
    UD_Group_Efx                   = 14000,        // All rack effects
    UD_Disable_Highlight           = 14001,        // Disable highlight for override of RKR defaults
    UD_Bank_Number                 = 15000         // Custom MIDI table bank CC Number

};


#endif


