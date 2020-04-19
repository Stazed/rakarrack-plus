/*
  rkrlv2.h

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
#include"Derelict.h"
#include"Valve.h"
#include"Dual_Flange.h"
#include"Ring.h"
#include"DistBand.h"
#include"Arpie.h"
#include"Expander.h"
#include"Shuffle.h"
#include"Synthfilter.h"
#include"VaryBand.h"
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

#undef OLDRKRLV2    // comment this for original method
//#define OLDRKRLV2  // use this for original

#define EQLV2_URI "https://github.com/Stazed/rakarrack-plus#eql"
#define COMPLV2_URI "https://github.com/Stazed/rakarrack-plus#comp"
#define DISTLV2_URI "https://github.com/Stazed/rakarrack-plus#dist"
#define ECHOLV2_URI "https://github.com/Stazed/rakarrack-plus#eco"
#define CHORUSLV2_URI "https://github.com/Stazed/rakarrack-plus#chor"
#define APHASELV2_URI "https://github.com/Stazed/rakarrack-plus#aphas"
#define PANLV2_URI "https://github.com/Stazed/rakarrack-plus#pan"
#define ALIENLV2_URI "https://github.com/Stazed/rakarrack-plus#awha"
#define HARMNOMIDLV2_URI "https://github.com/Stazed/rakarrack-plus#har_no_mid"
#define EXCITERLV2_URI "https://github.com/Stazed/rakarrack-plus#Exciter"
#define REVELV2_URI "https://github.com/Stazed/rakarrack-plus#reve"
#define EQPLV2_URI "https://github.com/Stazed/rakarrack-plus#eqp"
#define CABLV2_URI "https://github.com/Stazed/rakarrack-plus#cabe"
#define MDELLV2_URI "https://github.com/Stazed/rakarrack-plus#delm"
#define WAHLV2_URI "https://github.com/Stazed/rakarrack-plus#wha"
#define DERELV2_URI "https://github.com/Stazed/rakarrack-plus#dere"
#define VALVELV2_URI "https://github.com/Stazed/rakarrack-plus#Valve"
#define DFLANGELV2_URI "https://github.com/Stazed/rakarrack-plus#Dual_Flange"
#define RINGLV2_URI "https://github.com/Stazed/rakarrack-plus#Ring"
#define DISTBANDLV2_URI "https://github.com/Stazed/rakarrack-plus#DistBand"
#define ARPIELV2_URI "https://github.com/Stazed/rakarrack-plus#Arpie"
#define EXPANDLV2_URI "https://github.com/Stazed/rakarrack-plus#Expander"
#define SHUFFLELV2_URI "https://github.com/Stazed/rakarrack-plus#Shuffle"
#define SYNTHLV2_URI "https://github.com/Stazed/rakarrack-plus#Synthfilter"
#define VARYBANDLV2_URI "https://github.com/Stazed/rakarrack-plus#VaryBand"
#define MUTROLV2_URI "https://github.com/Stazed/rakarrack-plus#MuTroMojo"
#define ECHOVERSELV2_URI "https://github.com/Stazed/rakarrack-plus#Echoverse"
#define COILLV2_URI "https://github.com/Stazed/rakarrack-plus#CoilCrafter"
#define SHELFLV2_URI "https://github.com/Stazed/rakarrack-plus#ShelfBoost"
#define VOCODERLV2_URI "https://github.com/Stazed/rakarrack-plus#Vocoder"
#define SUSTAINLV2_URI "https://github.com/Stazed/rakarrack-plus#Sustainer"
#define SEQUENCELV2_URI "https://github.com/Stazed/rakarrack-plus#Sequence"
#define SHIFTERLV2_URI "https://github.com/Stazed/rakarrack-plus#Shifter"
#define STOMPLV2_URI "https://github.com/Stazed/rakarrack-plus#StompBox"
#define REVTRONLV2_URI "https://github.com/Stazed/rakarrack-plus#Reverbtron"
#define ECHOTRONLV2_URI "https://github.com/Stazed/rakarrack-plus#Echotron"
#define SHARMNOMIDLV2_URI "https://github.com/Stazed/rakarrack-plus#StereoHarm_no_mid"
#define HARMLV2_URI "https://github.com/Stazed/rakarrack-plus#har"
#define MBCOMPLV2_URI "https://github.com/Stazed/rakarrack-plus#CompBand"
#define OPTTREMLV2_URI "https://github.com/Stazed/rakarrack-plus#Otrem"
#define VIBELV2_URI "https://github.com/Stazed/rakarrack-plus#Vibe"
#define INFLV2_URI "https://github.com/Stazed/rakarrack-plus#Infinity"
#define PHASELV2_URI "https://github.com/Stazed/rakarrack-plus#phas"
#define GATELV2_URI "https://github.com/Stazed/rakarrack-plus#gate"
#define MIDICLV2_URI "https://github.com/Stazed/rakarrack-plus#midi_converter"
#define CONVOLOTRONLV2_URI "https://github.com/Stazed/rakarrack-plus#Convolotron"
#define FLANGELV2_URI "https://github.com/Stazed/rakarrack-plus#Flange"
#define OVERDRIVELV2_URI "https://github.com/Stazed/rakarrack-plus#Overdrive"
#define HARMLV2_URI "https://github.com/Stazed/rakarrack-plus#har"
#define STEROHARMLV2_URI "https://github.com/Stazed/rakarrack-plus#StereoHarm"


#define RVBFILE_URI "https://github.com/Stazed/rakarrack-plus#Reverbtron:rvbfile"
#define DLYFILE_URI "https://github.com/Stazed/rakarrack-plus#Echotron:dlyfile"
#define SNDFILE_URI "https://github.com/Stazed/rakarrack-plus#Convolotron:sndfile"

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
	IDISTBAND,
	IARPIE,//20
	IEXPAND,
	ISHUFF,
	ISYNTH,
	IVBAND,
	IMUTRO,
	IECHOVERSE,
	ICOIL,
	ISHELF,
	IVOC,
	ISUS,//30
	ISEQ,
	ISHIFT,
	ISTOMP,
	IREVTRON,
	IECHOTRON,
	ISHARM_NM,
	IMBCOMP,
	IOPTTREM,
	IVIBE,
	IINF,//40
	IPHASE,
	IGATE,
	IMIDIC,
        ICONVO,
        IFLANGE,
        IOVERDRIVE,
        IHARM,
        ISTEREOHARM
};

typedef struct _RKRLV2
{
    uint8_t nparams;
    uint8_t effectindex;//index of effect
    uint32_t period_max;
    uint8_t loading_file;//flag to indicate that file load work is underway
    uint8_t file_changed;
    uint8_t prev_bypass;
    float	*tmp_l;//temporary buffers for wet/dry mixing for hosts with shared in/out buffers(Ardour)
    float 	*tmp_r;
    
#ifdef OLDRKRLV2
    RvbFile* rvbfile;//file for reverbtron
    DlyFile* dlyfile;//file for echotron
#endif
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
    Derelict* dere; 		//15
    Valve* valve;		//16
    Dflange* dflange;   //17
    Ring* ring;			//18
    DistBand* distband;		//19
    Arpie* arp;			//20
    Expander* expand;	//21
    Shuffle* shuf;		//22
    Synthfilter* synth; //23
    VaryBand* varyband;		//24
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
    Chorus* flanger;     //46
    Distorsion* overdrive;   //47

    
} RKRLV2;

void forge_midimessage(RKRLV2* plug,
		uint32_t tme,
		const uint8_t* const buffer,
		uint32_t size);

#endif
