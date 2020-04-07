//spencer jackson

#ifndef PROCESS_H
#define PROCESS_H

//main class for processing engine
#include "global.h"

#include "Reverb.h"
#include "Chorus.h"
#include "Echo.h"
#include "Phaser.h"
#include "Distorsion.h"
#include "EQ.h"
#include "Compressor.h"
#include "Alienwah.h"
#include "Cabinet.h"
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
#include "StompBox.h"
#include "Reverbtron.h"
#include "Echotron.h"
#include "StereoHarm.h"
#include "CompBand.h"
#include "Opticaltrem.h"
#include "Vibe.h"
#include "Infinity.h"
#include "beattracker.h"
#include <jack/jack.h>

class RKR
{



public:

    RKR ();
    ~RKR ();

    void Alg (float *origl, float *origr ,void *);
    void Control_Gain (float *origl, float *origr);
    void Control_Volume (float *origl, float *origr);

    void Vol_Efx (int NumEffect, float volume);
    void Vol2_Efx ();
    void Vol3_Efx ();
    void cleanup_efx ();
    void midievents();
    void miramidi ();
    void calculavol (int i);
    void Bank_to_Preset (int Num);
    void Preset_to_Bank (int i);
    void Actualizar_Audio ();
    void loadfile (char *filename);
    void getbuf (char *buf, int j);
    void putbuf (char *buf, int j);
    void savefile (char *filename);
    void SaveIntPreset(int num, char *name);
    void DelIntPreset(int num, char *name);
    void MergeIntPreset(char *filename);
    void loadmiditable (char *filename);
    void savemiditable (char *filename);
    void loadskin (char *filename);
    void saveskin (char *filename);
    int loadbank (char *filename);
    void loadnames();
    int savebank (char *filename);
    void ConvertOldFile(char *filename);
    void ConvertReverbFile(char * filename);
    void dump_preset_names ();
    void New ();
    void New_Bank ();
    void Adjust_Upsample();
    void add_metro();
    void init_rkr ();
    int Message (int prio, const char *labelwin, const char *message_text);
    char *PrefNom (const char *dato);
    void EQ1_setpreset (int npreset);
    void EQ2_setpreset (int npreset);
    void InitMIDI ();
    void ConnectMIDI ();
    void ActiveUn(int value);
    void ActOnOff();
    void jack_process_midievents (jack_midi_event_t *midievent);
    void process_midi_controller_events(int parameter, int value);
    int ret_Tempo(int value);
    int ret_LPF(int value);
    int ret_HPF(int value);
    void Conecta ();
    void disconectaaconnect ();
    void conectaaconnect ();
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
    void Error_Handle(int num);
    void file_error(FILE *fn);
    void load_skin_error(FILE *fn);

    class FPreset *Fpre;
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
    class Cabinet *efx_Cabinet;
    class Pan *efx_Pan;
    class Harmonizer *efx_Har;
    class MusicDelay *efx_MusDelay;
    class Gate *efx_Gate;
    class NewDist *efx_NewDist;
    class Tuner *efx_Tuner;
    class MIDIConverter *efx_MIDIConverter;
    class metronome *M_Metronome;
    class beattracker *beat;

    class Recognize *HarmRecNote;
    class Recognize *StHarmRecNote;
    class Recognize *RingRecNote;
    class RecChord *RC_Harm;
    class RecChord *RC_Stereo_Harm;
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
    class AnalogFilter *DC_Offsetl;
    class AnalogFilter *DC_Offsetr;
    class Looper *efx_Looper;
    class RyanWah *efx_RyanWah;
    class RBEcho *efx_RBEcho;
    class CoilCrafter *efx_CoilCrafter;
    class ShelfBoost *efx_ShelfBoost;
    class Vocoder *efx_Vocoder;
    class Sustainer *efx_Sustainer;
    class Sequence *efx_Sequence;
    class Shifter *efx_Shifter;
    class StompBox *efx_StompBox;
    class Reverbtron *efx_Reverbtron;
    class Echotron *efx_Echotron;
    class StereoHarm *efx_StereoHarm;
    class CompBand *efx_CompBand;
    class Opticaltrem *efx_Opticaltrem;
    class Vibe *efx_Vibe;
    class Infinity *efx_Infinity;

    jack_client_t *jackclient;
    jack_options_t options;
    jack_status_t status;
    char jackcliname[64];

    int db6booster;
    int jdis;
    int jshut;
    int DC_Offset;
    int Bypass;
    int MIDIConverter_Bypass;
    int Metro_Bypass;
    int Tuner_Bypass;
    int Tap_Bypass;
    int ACI_Bypass;
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
    int StompBox_Bypass;
    int Reverbtron_Bypass;
    int Echotron_Bypass;
    int StereoHarm_Bypass;
    int CompBand_Bypass;
    int Opticaltrem_Bypass;
    int Vibe_Bypass;
    int Infinity_Bypass;
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
    int StompBox_B;
    int Reverbtron_B;
    int Echotron_B;
    int StereoHarm_B;
    int CompBand_B;
    int Opticaltrem_B;
    int Vibe_B;
    int Infinity_B;

    int Selected_Preset;
    int lv[70][20];
    int saved_order[16];
    int efx_order[16];
    int new_order[16];
    int availables[60];
    int active[12];
    int MidiCh;
    int HarCh;
    int StereoHarCh;
    int init_state;
    int actuvol;
    int help_displayed;
    int modified;
    int autoassign;
    int comemouse;
    int aconnect_MI;
    int aconnect_JA;
    int aconnect_JIA;

    int cuan_jack;
    int cuan_ijack;
    int IsCoIn;
    int Cyoin;
    int Pyoin;
    int Ccin;
    int Pcin;

    // bank of flags telling GUI which midi controlled items to update
    int Mcontrol[500];
    // flag telling GUI that Mcontrol has at least one set flag
    int RControl;
    int ControlGet;
    int CountWait;
    int XUserMIDI[128][20];

    int eff_filter;
    int Har_Down;
    int Har_U_Q;
    int Har_D_Q;
    int Rev_Down;
    int Rev_U_Q;
    int Rev_D_Q;
    int Con_Down;
    int Con_U_Q;
    int Con_D_Q;
    int Shi_Down;
    int Shi_U_Q;
    int Shi_D_Q;
    int Seq_Down;
    int Seq_U_Q;
    int Seq_D_Q;
    int Voc_Down;
    int Voc_U_Q;
    int Voc_D_Q;
    int Ste_Down;
    int Ste_U_Q;
    int Ste_D_Q;
    float aFreq;

    int Metro_Vol;
    int M_Metro_Sound;
    int deachide;
    int scalable;
    int change_scale;
    int font;
    int flpos;
    int upsample;
    int UpQual;
    int DownQual;
    int UpAmo;
    int J_SAMPLE_RATE;
    int J_PERIOD;
    int m_displayed;
    int Mvalue;
    int Mnumeff[32];
    int OnOffC;

    //ssj
    int period;
    float fPeriod;
    int sample_rate;
    float fSample_rate;
    float cSample_rate;
    float *interpbuf;//buffer for analog filters to interpolate (shared)

    int Wave_res_amount;
    int Wave_up_q;
    int Wave_down_q;

    int sw_stat;
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

    // Harmonizer
    int HarQual;
    int SteQual;

    // Tap Tempo

    int tempocnt;
    int Tap_Display;
    int Tap_Selection;
    int Tap_TempoSet;
    int Tap_SetValue;
    int t_timeout;
    int jt_state;
    int Tap_Updated;
    int note_old;
    int cents_old;

    int cpufp;
    int mess_dis;
    int numpi, numpo, numpa, numpmi, numpmo;
    int numpc;
    int midi_table;
    int a_bank;
    int new_bank_loaded;

    int Aux_Gain;
    int Aux_Threshold;
    int Aux_MIDI;
    int Aux_Minimum;
    int Aux_Maximum;
    int Aux_Source;
    int last_auxvalue;
    int ena_tool;
    int VocBands;
    int RCOpti_Harm;
    int RCOpti_Stereo;
    int RCOpti_Ring;
    
    int M_Metro_Tempo;
    int M_Metro_Bar;
    int mtc_counter;
    int EnableBackgroundImage;
    int ML_filter;
    int ML_clist[150];

    long Tap_time_Init;

    double Tap_timeB;
    double Tap_timeC;
    double jt_tempo;

    double tempobuf[6];


    double u_down;
    double u_up;

    timeval timeA;

    float booster;
    float cpuload;
    float rtrig;

    float *efxoutl;
    float *efxoutr;
    float *auxdata;
    float *auxresampled;
    float *anall;
    float *analr;
    float *smpl;
    float *smpr;
    float *denormal;
    float *m_ticks;

    float Master_Volume;
    float Input_Gain;
    float Fraction_Bypass;
    float Log_I_Gain;
    float Log_M_Volume;
    float M_Metro_Vol;

    float val_sum;
    float old_il_sum;
    float old_ir_sum;
    float old_vl_sum;
    float old_vr_sum;
    float val_vl_sum;
    float val_vr_sum;
    float val_il_sum;
    float val_ir_sum;
    float old_a_sum;
    float val_a_sum;


    float bogomips;
    float looper_size;

// Tunner

    float nfreq_old;
    float afreq_old;

    char tmpprefname[128];

    char *Preset_Name;
    char *Author;
    char *Bank_Saved;
    char *UserRealName;



    char MID[128];
    char BankFilename[128];
    char UDirFilename[128];
    char BackgroundImage[256];




    struct Effects_Names
    {
        char Nom[24];
        int Pos;
        int Type;
        
        Effects_Names():
            Nom(),
            Pos(),
            Type() {}
    } efx_names[70];

    struct Effects_Params
    {
        char Nom[32];
        int Ato;
        int Effect;
        
        Effects_Params():
            Nom(),
            Ato(),
            Effect() {}
    } efx_params[500];


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
        char RevFiname[128];
        char EchoFiname[128];
        int lv[70][20];
        int XUserMIDI[128][20];
        int XMIDIrangeMin[128];
        int XMIDIrangeMax[128];
        
        Preset_Bank_Struct():
            Preset_Name(),
            Author(),
            Classe(),
            Type(),
            ConvoFiname(),
            cInput_Gain(),
            cMaster_Volume(),
            cBalance(),
            Input_Gain(),
            Master_Volume(),
            Balance(),
            Bypass(),
            RevFiname(),
            EchoFiname(),
            lv(),
            XUserMIDI(),
            XMIDIrangeMin(),
            XMIDIrangeMax() {}
    } Bank[62];


    struct MIDI_table
    {
        int bank;
        int preset;
        
        MIDI_table():
            bank(),
            preset() {}
    } M_table[128];

    struct Bank_Names
    {
        char Preset_Name[64];
        
        Bank_Names():
            Preset_Name() {}
    } B_Names[4][62];


    // Alsa MIDI

    snd_seq_t *midi_in, *midi_out;


    struct JackPorts
    {
        char name[128];
        
        JackPorts():
            name() {}
    } jack_po[16],jack_poi[16];




};

struct list_element
{
    struct list_element *forward;
    struct list_element *backward;
    char *name;
    
    list_element():
        forward(),
        backward(),
        name() {}
};

#endif
