#ifndef RKRLV2_H
#define RKRLV2_H

#include<lv2.h>
#include<lv2/lv2plug.in/ns/ext/urid/urid.h>
#include<lv2/lv2plug.in/ns/ext/midi/midi.h>
#include<lv2/lv2plug.in/ns/ext/atom/util.h>
#include<lv2/lv2plug.in/ns/ext/atom/forge.h>
#include<lv2/lv2plug.in/ns/ext/time/time.h>
#include<lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include<lv2/lv2plug.in/ns/ext/options/options.h>
#include<lv2/lv2plug.in/ns/ext/atom/atom.h>
#include<lv2/lv2plug.in/ns/ext/patch/patch.h>
#include<lv2/lv2plug.in/ns/ext/worker/worker.h>
#include<lv2/lv2plug.in/ns/ext/state/state.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include"EQ.h"
#include"Compressor.h"
#include"Distorsion.h"
#include"Echo.h"
#include"Chorus.h"
#include"APhaser.h"
#include"Harmonizer.h"
#include"RecChord.h"
#include"RecognizeNote.h"
#include"Exciter.h"
#include"Pan.h"
#include"Alienwah.h"
#include"Reverb.h"
#include"Cabinet.h"
#include"MusicDelay.h"
#include"DynamicFilter.h"
#include"NewDist.h"
#include"Valve.h"
#include"Dual_Flange.h"
#include"Ring.h"
#include"MBDist.h"
#include"Arpie.h"
#include"Expander.h"
#include"Shuffle.h"
#include"Synthfilter.h"
#include"MBVvol.h"
#include"RyanWah.h"
#include"RBEcho.h"
#include"CoilCrafter.h"
#include"ShelfBoost.h"
#include"Vocoder.h"
#include"Sequence.h"
#include"Shifter.h"
#include"StompBox.h"
#include"Reverbtron.h"
#include"Echotron.h"
#include"StereoHarm.h"
#include"CompBand.h"
#include"Opticaltrem.h"
#include"Vibe.h"
#include"Infinity.h"
#include"Phaser.h"
#include"Gate.h"
#include"MIDIConverter.h"
#include"Convolotron.h"

#define EQLV2_URI "http://rakarrack.sourceforge.net/effects.html#eql"
#define COMPLV2_URI "http://rakarrack.sourceforge.net/effects.html#comp"
#define DISTLV2_URI "http://rakarrack.sourceforge.net/effects.html#dist"
#define ECHOLV2_URI "http://rakarrack.sourceforge.net/effects.html#eco"
#define CHORUSLV2_URI "http://rakarrack.sourceforge.net/effects.html#chor"
#define APHASELV2_URI "http://rakarrack.sourceforge.net/effects.html#aphas"
#define PANLV2_URI "http://rakarrack.sourceforge.net/effects.html#pan"
#define ALIENLV2_URI "http://rakarrack.sourceforge.net/effects.html#awha"
#define HARMNOMIDLV2_URI "http://rakarrack.sourceforge.net/effects.html#har_no_mid"
#define EXCITERLV2_URI "http://rakarrack.sourceforge.net/effects.html#Exciter"
#define REVELV2_URI "http://rakarrack.sourceforge.net/effects.html#reve"
#define EQPLV2_URI "http://rakarrack.sourceforge.net/effects.html#eqp"
#define CABLV2_URI "http://rakarrack.sourceforge.net/effects.html#cabe"
#define MDELLV2_URI "http://rakarrack.sourceforge.net/effects.html#delm"
#define WAHLV2_URI "http://rakarrack.sourceforge.net/effects.html#wha"
#define DERELV2_URI "http://rakarrack.sourceforge.net/effects.html#dere"
#define VALVELV2_URI "http://rakarrack.sourceforge.net/effects.html#Valve"
#define DFLANGELV2_URI "http://rakarrack.sourceforge.net/effects.html#Dual_Flange"
#define RINGLV2_URI "http://rakarrack.sourceforge.net/effects.html#Ring"
#define MBDISTLV2_URI "http://rakarrack.sourceforge.net/effects.html#DistBand"
#define ARPIELV2_URI "http://rakarrack.sourceforge.net/effects.html#Arpie"
#define EXPANDLV2_URI "http://rakarrack.sourceforge.net/effects.html#Expander"
#define SHUFFLELV2_URI "http://rakarrack.sourceforge.net/effects.html#Shuffle"
#define SYNTHLV2_URI "http://rakarrack.sourceforge.net/effects.html#Synthfilter"
#define MBVOLLV2_URI "http://rakarrack.sourceforge.net/effects.html#VaryBand"
#define MUTROLV2_URI "http://rakarrack.sourceforge.net/effects.html#MuTroMojo"
#define ECHOVERSELV2_URI "http://rakarrack.sourceforge.net/effects.html#Echoverse"
#define COILLV2_URI "http://rakarrack.sourceforge.net/effects.html#CoilCrafter"
#define SHELFLV2_URI "http://rakarrack.sourceforge.net/effects.html#ShelfBoost"
#define VOCODERLV2_URI "http://rakarrack.sourceforge.net/effects.html#Vocoder"
#define SUSTAINLV2_URI "http://rakarrack.sourceforge.net/effects.html#Sustainer"
#define SEQUENCELV2_URI "http://rakarrack.sourceforge.net/effects.html#Sequence"
#define SHIFTERLV2_URI "http://rakarrack.sourceforge.net/effects.html#Shifter"
#define STOMPLV2_URI "http://rakarrack.sourceforge.net/effects.html#StompBox"
#define STOMPFUZZLV2_URI "http://rakarrack.sourceforge.net/effects.html#StompBox_fuzz"
#define REVTRONLV2_URI "http://rakarrack.sourceforge.net/effects.html#Reverbtron"
#define ECHOTRONLV2_URI "http://rakarrack.sourceforge.net/effects.html#Echotron"
#define SHARMNOMIDLV2_URI "http://rakarrack.sourceforge.net/effects.html#StereoHarm_no_mid"
#define HARMLV2_URI "http://rakarrack.sourceforge.net/effects.html#har"
#define MBCOMPLV2_URI "http://rakarrack.sourceforge.net/effects.html#CompBand"
#define OPTTREMLV2_URI "http://rakarrack.sourceforge.net/effects.html#Otrem"
#define VIBELV2_URI "http://rakarrack.sourceforge.net/effects.html#Vibe"
#define INFLV2_URI "http://rakarrack.sourceforge.net/effects.html#Infinity"
#define PHASELV2_URI "http://rakarrack.sourceforge.net/effects.html#phas"
#define GATELV2_URI "http://rakarrack.sourceforge.net/effects.html#gate"
#define MIDICLV2_URI "http://rakarrack.sourceforge.net/effects.html#midi_converter"
#define CONVOLOTRONLV2_URI "http://rakarrack.sourceforge.net/effects.html#Convolotron"


#define RVBFILE_URI "http://rakarrack.sourceforge.net/effects.html#Reverbtron:rvbfile"
#define DLYFILE_URI "http://rakarrack.sourceforge.net/effects.html#Echotron:dlyfile"
#define SNDFILE_URI "http://rakarrack.sourceforge.net/effects.html#Convolotron:sndfile"

class MIDIConverter;    // forward declaration

enum RKRLV2_ports_
{
    INL =0,
    INR,
    OUTL,
    OUTR,
    BYPASS,
    PARAM0,
    PARAM1,
    PARAM2,
    PARAM3,
    PARAM4,
    PARAM5,
    PARAM6,
    PARAM7,
    PARAM8,
    PARAM9,
    PARAM10,
    PARAM11,
    PARAM12,
    PARAM13,
    PARAM14,
    PARAM15,
    PARAM16,
    PARAM17,
    PARAM18,
    DBG,
    EXTRA
};


//just make sure these line up with the order in manifest.ttl
enum RKRLV2_effects_
{
	IEQ =0,
	ICOMP,
	IDIST,
	IECHO,
	ICHORUS,
	IAPHASE,
	IHARM_NM,
	IEXCITER,
	IPAN,
	IAWAH,
	IREV,//10
	IEQP,
	ICAB,
	IMDEL,
	IWAH,
	IDERE,
	IVALVE,
	IDFLANGE,
	IRING,
	IMBDIST,
	IARPIE,//20
	IEXPAND,
	ISHUFF,
	ISYNTH,
	IMBVOL,
	IMUTRO,
	IECHOVERSE,
	ICOIL,
	ISHELF,
	IVOC,
	ISUS,//30
	ISEQ,
	ISHIFT,
	ISTOMP,
	IFUZZ,
	IREVTRON,
	IECHOTRON,
	ISHARM_NM,
	IMBCOMP,
	IOPTTREM,
	IVIBE,//40
	IINF,
	IPHASE,
	IGATE,
	IMIDIC,
        ICONVO
};

typedef struct _RKRLV2
{
    uint8_t nparams;
    uint8_t effectindex;//index of effect
    uint32_t period_max;
    uint8_t loading_file;//flag to indicate that file load work is underway
    uint8_t init_params; //flag to indicate to force parameter (LFO) update & sample update on first run
    uint8_t file_changed;
    uint8_t prev_bypass;
    RvbFile* rvbfile;//file for reverbtron
    DlyFile* dlyfile;//file for echotron

    //ports
    float *input_l_p;
    float *input_r_p;
    float *output_l_p;
    float *output_r_p;
    float *bypass_p;
    const LV2_Atom_Sequence* atom_in_p;
    LV2_Atom_Sequence* atom_out_p;
    float *param_p[20];
    float *dbg_p;

    //various "advanced" lv2 stuffs
    LV2_Worker_Schedule* scheduler;
    LV2_Atom_Forge	forge;
    LV2_Atom_Forge_Frame atom_frame;
    LV2_URID_Map *urid_map;

    struct urids
    {
        LV2_URID    midi_MidiEvent;
        LV2_URID    atom_Float;
        LV2_URID    atom_Int;
        LV2_URID    atom_Object;
        LV2_URID    atom_Path;
        LV2_URID    atom_URID;
        LV2_URID    bufsz_max;
        LV2_URID    patch_Set;
        LV2_URID    patch_Get;
        LV2_URID	patch_property;
        LV2_URID 	patch_value;
        LV2_URID 	filetype_rvb;
        LV2_URID 	filetype_dly;
        LV2_URID        filetype_snd;

    } URIDs;

    //effect modules
    EQ* eq;             //0, 11
    Compressor* comp;   //1
    Distorsion* dist;   //2
    Echo* echo;         //3
    Chorus* chorus;     //4
    Analog_Phaser* aphase;//5
    Harmonizer* harm;	//6
    RecChord* chordID;
    Recognize* noteID;
    Exciter* exciter;	//7
    Pan* pan;			//8
    Alienwah* alien;    //9
    Reverb* reve;       //10
    Cabinet* cab;       //12
    MusicDelay* mdel; 	//13
    DynamicFilter* wah; //14
    NewDist* dere; 		//15
    Valve* valve;		//16
    Dflange* dflange;   //17
    Ring* ring;			//18
    MBDist* mbdist;		//19
    Arpie* arp;			//20
    Expander* expand;	//21
    Shuffle* shuf;		//22
    Synthfilter* synth; //23
    MBVvol* mbvol;		//24
    RyanWah* mutro; 	//25
    RBEcho* echoverse;	//26
    CoilCrafter* coil;	//27
    ShelfBoost* shelf;	//28
    Vocoder* voc;		//29
    Sustainer* sus;		//30
    Sequence* seq;		//31
    Shifter* shift;		//32
    StompBox* stomp;	//33,34
    Reverbtron* revtron;//35
    Echotron* echotron; //36
    StereoHarm* sharm;  //37
    CompBand* mbcomp; 	//38
    Opticaltrem* otrem; //39
    Vibe* vibe;			//40
    Infinity* inf;		//41
    Phaser* phase;      //42
    Gate* gate;         //43
    MIDIConverter* midic; //44
    Convolotron* convol;  //45
    
} RKRLV2;

void forge_midimessage(RKRLV2* plug,
		uint32_t tme,
		const uint8_t* const buffer,
		uint32_t size);

#endif
