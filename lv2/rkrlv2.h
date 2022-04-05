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

#include"FX/Effect.h"
#include"FX/EQ.h"
#include"FX/Compressor.h"
#include"Limiter.h"
#include"FX/Distorsion.h"
#include"FX/Overdrive.h"
#include"FX/Echo.h"
#include"FX/Chorus.h"
#include"FX/Flanger.h"
#include"FX/APhaser.h"
#include"FX/Harmonizer.h"
#include"RecChord.h"
#include"RecognizeNote.h"
#include"FX/Exciter.h"
#include"FX/Pan.h"
#include"FX/Alienwah.h"
#include"FX/Reverb.h"
#include"FX/Cabinet.h"
#include"FX/MusicDelay.h"
#include"FX/WahWah.h"
#include"FX/Derelict.h"
#include"FX/Valve.h"
#include"FX/Dual_Flange.h"
#include"FX/Ring.h"
#include"FX/DistBand.h"
#include"FX/Arpie.h"
#include"FX/Expander.h"
#include"FX/Shuffle.h"
#include"FX/Synthfilter.h"
#include"FX/VaryBand.h"
#include"FX/MuTroMojo.h"
#include"FX/Echoverse.h"
#include"FX/CoilCrafter.h"
#include"FX/ShelfBoost.h"
#include"FX/Vocoder.h"
#include"FX/Sequence.h"
#include"FX/Shifter.h"
#include"FX/StompBox.h"
#include"FX/Reverbtron.h"
#include"FX/Echotron.h"
#include"FX/StereoHarm.h"
#include"FX/CompBand.h"
#include"FX/Opticaltrem.h"
#include"FX/Vibe.h"
#include"FX/Infinity.h"
#include"FX/Phaser.h"
#include"FX/Gate.h"
#include"FX/MIDIConverter.h"
#include"FX/Convolotron.h"
#include"FX/ParametricEQ.h"

#define RVBFILE_URI "https://github.com/Stazed/rakarrack-plus#Reverbtron:rvbfile"
#define DLYFILE_URI "https://github.com/Stazed/rakarrack-plus#Echotron:dlyfile"
#define SNDFILE_URI "https://github.com/Stazed/rakarrack-plus#Convolotron:sndfile"

class MIDIConverter;    // forward declaration

enum RKRLV2_ports_
{
    INL = 0,
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
    IEQ = 0,
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
        LV2_URID    patch_property;
        LV2_URID    patch_value;
        LV2_URID    filetype_rvb;
        LV2_URID    filetype_dly;
        LV2_URID    filetype_snd;
        
        urids():
            midi_MidiEvent(),
            atom_Float(),
            atom_Int(),
            atom_Object(),
            atom_Path(),
            atom_URID(),
            bufsz_max(),
            patch_Set(),
            patch_Get(),
            patch_property(),
            patch_value(),
            filetype_rvb(),
            filetype_dly(),
            filetype_snd() {}

    } URIDs;

    //effect modules
    EQ* eq;             //0
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
    ParametricEQ* peq;  //11
    Cabinet* cab;       //12
    MusicDelay* mdel; 	//13
    WahWah* wah; //14
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
    MuTroMojo* mutro; 	//25
    Echoverse* echoverse;	//26
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
    Flanger* flanger;     //46
    Overdrive* overdrive;   //47

    
} RKRLV2;

void forge_midimessage(RKRLV2* plug,
		uint32_t tme,
		const uint8_t* const buffer,
		uint32_t size);

#endif
