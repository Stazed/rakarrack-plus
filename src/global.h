/*
  rakarrack - a guitar efects software

  global.h  -  Variable Definitions and functions
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

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







#include "config.h"
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <alsa/asoundlib.h>
#include <X11/xpm.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <Fl/Fl_Preferences.H>
#include "Reverb.h"
#include "Chorus.h"
#include "Echo.h"
#include "Phaser.h"
#include "Distorsion.h"
#include "EQ.h"
#include "Compressor.h"
#include "Alienwah.h"
#include "DynamicFilter.h"
#include "Pan.h"
#include "Harmonizer.h"
#include "MusicDelay.h"
#include "Gate.h"
#include "Tuner.h"
#include "MIDIConverter.h"
#include "RecognizeNote.h"
#include "RecChord.h"
#include "NewDist.h"
#include "APhaser.h"
#include "Valve.h"
#include "Dual_Flange.h"
#include "Ring.h"
#include "Exciter.h"
#include "MBDist.h"
#include "Arpie.h"
#include "Expander.h"
#include "Shuffle.h"
#include "Synthfilter.h"
#include "MBVvol.h"
#include "Convolotron.h"
#include "Resample.h"
#include "Looper.h"
#include "RyanWah.h"
#include "RBEcho.h"
#include "CoilCrafter.h"
#include "ShelfBoost.h"
#include "Vocoder.h"
#include "Sustainer.h"
#include "Sequence.h"
#include "Shifter.h"

extern int Pexitprogram, preset;
extern int commandline, gui;
extern int exitwithhelp, nojack;
extern Pixmap p, mask;
extern XWMHints *hints;
extern int PERIOD;
extern float fPERIOD;
extern unsigned int SAMPLE_RATE;
extern float fSAMPLE_RATE;
extern float cSAMPLE_RATE;
extern float val_sum;
extern int note_active[POLY];
extern int rnote[POLY];
extern int gate[POLY];
extern int reconota;
extern float r__ratio;
extern int maxx_len;


class RKR
{



public:

  RKR ();
  ~RKR ();

  void Alg (float *inl, float *inr,float *origl, float *origr ,void *);
  void midievents();
  void loadfile (char *filename);
  void savefile (char *filename);
  void loadskin (char *filename);
  void saveskin (char *filename);
  int loadbank (char *filename);
  int savebank (char *filename);
  void ConvertOldFile(char *filename);
  void dump_preset_names ();
  void New ();
  void New_Bank ();
  void Bank_to_Preset (int Num);
  void Preset_to_Bank (int i);
  void Actualizar_Audio ();
  void Adjust_Upsample();
  void init_rkr ();
  void Vol_Efx (int NumEffect, float volume);
  void Vol2_Efx ();
  void Vol3_Efx ();
  void cleanup_efx ();
  void Control_Gain (float *origl, float *origr);
  void Control_Volume (float *origl, float *origr);
  int Message (const char *labelwin, const char *message_text);
  char *PrefNom (const char *dato);

  void EQ1_setpreset (int npreset);
  void EQ2_setpreset (int npreset);
  int Cabinet_setpreset (int npreset);
  void miramidi ();
  void InitMIDI ();
  void ConnectMIDI ();
  void ActiveUn(int value);
  void jack_process_midievents (jack_midi_event_t *midievent);
  void process_midi_controller_events(int parameter, int value);
  int ret_Tempo(int value);
  int ret_LPF(int value);
  int ret_HPF(int value);
  void calculavol (int i);
  void Conecta ();
  void disconectaaconnect ();
  void conectaaconnect ();
  int Conv_Data_Version(char* D_Version);
  int BigEndian();
  void fix_endianess();
  void copy_IO();
  void convert_IO();
  int CheckOldBank(char *filename);
  int Get_Bogomips();
  int checkonoff(int value);
  int TapTempo();
  void TapTempo_Timeout(int state);
  void Update_tempo();
  int checkforaux();

  class Reverb *efx_Rev;
  class Chorus *efx_Chorus;
  class Chorus *efx_Flanger;
  class Phaser *efx_Phaser;
  class Analog_Phaser *efx_APhaser;
  class EQ *efx_EQ1;
  class EQ *efx_EQ2;
  class Echo *efx_Echo;
  class Distorsion *efx_Distorsion;
  class Distorsion *efx_Overdrive;
  class Compressor *efx_Compressor;
  class DynamicFilter *efx_WhaWha;
  class Alienwah *efx_Alienwah;
  class EQ *efx_Cabinet;
  class Pan *efx_Pan;
  class Harmonizer *efx_Har;
  class MusicDelay *efx_MusDelay;
  class Gate *efx_Gate;
  class NewDist *efx_NewDist;
  class Tuner *efx_Tuner;
  class MIDIConverter *efx_MIDIConverter;
  class Recognize *RecNote;
  class RecChord *RC;
  class Compressor *efx_FLimiter;
  class Valve *efx_Valve;
  class Dflange *efx_DFlange;
  class Ring *efx_Ring;
  class Exciter *efx_Exciter;
  class MBDist *efx_MBDist;
  class Arpie *efx_Arpie;
  class Expander *efx_Expander;     
  class Synthfilter *efx_Synthfilter;
  class Shuffle *efx_Shuffle;
  class MBVvol *efx_MBVvol;
  class Convolotron *efx_Convol;
  class Resample *U_Resample;
  class Resample *D_Resample;
  class Resample *A_Resample;
  class Looper *efx_Looper;
  class RyanWah *efx_RyanWah;
  class RBEcho *efx_RBEcho;
  class CoilCrafter *efx_CoilCrafter;
  class ShelfBoost *efx_ShelfBoost;
  class Vocoder *efx_Vocoder;
  class Sustainer *efx_Sustainer;
  class Sequence *efx_Sequence;
  class Shifter *efx_Shifter;

  jack_client_t *jackclient;
  jack_options_t options;
  jack_status_t status;

  char jackcliname[64];

  int Bypass;
  int MIDIConverter_Bypass;
  int Tuner_Bypass;
  int Tap_Bypass;
  int Reverb_Bypass;
  int Chorus_Bypass;
  int Flanger_Bypass;
  int Phaser_Bypass;
  int Overdrive_Bypass;
  int Distorsion_Bypass;
  int Echo_Bypass;
  int EQ1_Bypass;
  int EQ2_Bypass;
  int Compressor_Bypass;
  int WhaWha_Bypass;
  int Alienwah_Bypass;
  int Cabinet_Bypass;
  int Pan_Bypass;
  int Harmonizer_Bypass;
  int MusDelay_Bypass;
  int Gate_Bypass;
  int NewDist_Bypass;
  int APhaser_Bypass;
  int Valve_Bypass;
  int DFlange_Bypass;
  int Ring_Bypass;
  int Exciter_Bypass;
  int MBDist_Bypass;
  int Arpie_Bypass;
  int Expander_Bypass;
  int Shuffle_Bypass;
  int Synthfilter_Bypass;
  int MBVvol_Bypass;
  int Convol_Bypass;
  int Looper_Bypass;
  int RyanWah_Bypass;
  int RBEcho_Bypass;
  int CoilCrafter_Bypass;
  int ShelfBoost_Bypass;
  int Vocoder_Bypass;
  int Sustainer_Bypass;
  int Sequence_Bypass;
  int Shifter_Bypass;
  int Bypass_B;
  int Reverb_B;
  int Chorus_B;
  int Flanger_B;
  int Phaser_B;
  int APhaser_B;
  int DFlange_B;
  int Overdrive_B;
  int Distorsion_B;
  int Echo_B;
  int EQ1_B;
  int EQ2_B;
  int Compressor_B;
  int WhaWha_B;
  int Alienwah_B;
  int Cabinet_B;
  int Pan_B;
  int Harmonizer_B;
  int MusDelay_B;
  int Gate_B;
  int NewDist_B;
  int Valve_B;
  int Ring_B;
  int Exciter_B;
  int MBDist_B;
  int Arpie_B;
  int Expander_B;
  int Shuffle_B;
  int Synthfilter_B;
  int MBVvol_B;
  int Convol_B;
  int Looper_B;
  int RyanWah_B;
  int RBEcho_B;
  int CoilCrafter_B;
  int ShelfBoost_B;
  int Vocoder_B;
  int Sustainer_B;
  int Sequence_B;
  int Shifter_B;
  
  int Cabinet_Preset;
  int Selected_Preset;
  int lv[50][20];
  int saved_order[16];
  int efx_order[16];
  int new_order[16];
  int availables[40];
  int active[12];
  int MidiCh;
  int HarCh;
  int init_state;
  int actuvol;
  int help_displayed;
  int modified;
  int aconnect_MI;
  int aconnect_JA;
  int cuan_jack;
  int IsCoIn;
  int Cyoin;
  int Pyoin;
  int Ccin;
  int Pcin;

  // bank of flags telling GUI which midi controlled items to update
  int Mcontrol[384];
  // flag telling GUI that Mcontrol has at least one set flag
  int RControl;
  int ControlGet;
  int CountWait;
  int XUserMIDI[128][20];

  int upsample;
  int UpQual;
  int DownQual;
  int UpAmo;
  int J_SAMPLE_RATE;
  int J_PERIOD;  
  int m_displayed;
  int Mvalue;
  int Mnumeff;

  int MIDIway;
  int NumParams;
  int NumEffects;
  int relfontsize;
  int resolution;
  int sh;
  int sschema;
  int slabel_color;
  int sfore_color;
  int sback_color;
  int sleds_color;

  int have_signal;
  int OnCounter;
  int t_periods;

  double u_down;
  double u_up;


  float *efxoutl;
  float *efxoutr;
  float *auxdata;
  float *auxresampled;
  float *anall;
  float *analr;
  float *smpl;
  float *smpr;
  float *denormal;

  float Master_Volume;
  float Input_Gain;
  float Fraction_Bypass;
  float Log_I_Gain;
  float Log_M_Volume;

  float old_i_sum;
  float old_v_sum;
  float val_v_sum;
  float val_i_sum;

  float bogomips;
  float looper_size;

// Tunner

  int note_old;
  float nfreq_old;
  float afreq_old;
  int cents_old;

  char tmpprefname[128];

  char *Preset_Name;
  char *Data_Version;
  char *Author;
  char *Bank_Saved;
  char *UserRealName;

  char MID[128];
  char BankFilename[128];
  char BackgroundImage[256];

  struct Effects_Names
  {
    char Nom[24];
  } efx_names[50];

  struct Effects_Params
  {
    char Nom[32];
    int Ato;
  } efx_params[336];


  struct Preset_Bank_Struct
  {
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char ConvoFiname[128];
    char cInput_Gain[64];
    char cMaster_Volume[64];
    char cBalance[64];
    float Input_Gain;
    float Master_Volume;
    float Balance;
    int Bypass;
    int Res[64];
    int lv[50][20];
    int XUserMIDI[128][20];
    int XMIDIrangeMin[128];
    int XMIDIrangeMax[128];
  } Bank[62];


  // Alsa MIDI

  snd_seq_t *midi_in, *midi_out;

  //   Recognize

  int last;

  // Harmonizer
  int HarQual;

  struct JackPorts
  {
    char name[128];
  } jack_po[80];


  // Tap Tempo
  
  timeval timeA;
  long Tap_time_Init;
  double Tap_timeB;
  double Tap_timeC;
  int tempocnt;
  double tempobuf[6];  
  int Tap_Display;
  int Tap_Selection;
  int Tap_TempoSet;
  int Tap_SetValue;
  int t_timeout;

  int jt_state;
  double jt_tempo;
};


#endif


