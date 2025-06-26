/*
  rkrlv2.C

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

//Spencer Jackson

#include"rkrlv2.h"
#include"strlcpy.h"

//this is the default, host may use periods greater, it will now be adjusted.
#define INTERMEDIATE_BUFSIZE 1024
#define MAX_INPLACE 8192

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

#include"FX/Effect.h"
#include"FX/EQ.h"
#include"FX/Compressor.h"
#include"Limiter.h"
#include"strlcpy.h"
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
#include"FX/ResSolution.h"

#define RVBFILE_URI "https://github.com/Stazed/rakarrack-plus#Reverbtron:rvbfile"
#define DLYFILE_URI "https://github.com/Stazed/rakarrack-plus#Echotron:dlyfile"
#define SNDFILE_URI "https://github.com/Stazed/rakarrack-plus#Convolotron:sndfile"


typedef struct _RKRLV2
{
    uint8_t nparams;
    uint8_t effectindex;//index of effect
    uint32_t period_max;
    uint16_t period_default;
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
    ResSolution* ressol; // 48

} RKRLV2;

enum other_ports
{
    //be sure to account for index of array vs lv2 port index
    HARMONIZER_MIDI,//unassigned right now
    VOCODER_AUX_IN = 7,
    VOCODER_VU_LEVEL = 8,
    MIDIC_NOTE_REGISTER = 7
};

// A few helper functions taken from the RKR object
void
wetdry_mix (RKRLV2* plug, float mix, uint32_t period)
{
    unsigned int i;
    float v1, v2;

    if (mix < 0.5f)
    {
        v1 = 1.0f;
        v2 = mix * 2.0f;
    }
    else
    {
        v1 = (1.0f - mix) * 2.0f;
        v2 = 1.0f;
    };


//   Reverb and musical delay are exceptions for some reason?
//    if ((NumEffect == 8) || (NumEffect == 15))
//        v2 *= v2;

    for (i = 0; i < period; i++)
    {
        plug->output_l_p[i] = plug->input_l_p[i] * v2 + plug->output_l_p[i] * v1;
        plug->output_r_p[i] = plug->input_r_p[i] * v2 + plug->output_r_p[i] * v1;
    };

}

//this finds the peak value
int
have_signal(float* efxoutl, float* efxoutr, uint32_t period)
{
    float tmp;
    uint32_t i;
    float il_sum = 1e-12f;
    float ir_sum = 1e-12f;
    for (i = 0; i < period; i++) {

        tmp = fabsf (efxoutl[i]);
        if (tmp > il_sum) il_sum = tmp;
        tmp = fabsf (efxoutr[i]);
        if (tmp > ir_sum) ir_sum = tmp;
    }


    if ((il_sum+ir_sum) > 0.0004999f)  return  1;
    else  return 0;
}

//checks if input and output buffers are shared and copies it in a temp buffer so wet/dry works
static void
check_shared_buf(RKRLV2* plug, uint32_t nframes)
{
#if 0
    // This crashes non-mixer sometimes
    if(nframes > MAX_INPLACE)
    {
        return;
    }
#else
    // This original from rkrlv2 works for all
    if(nframes > plug->period_max)
    {
        if(plug->tmp_l)
            free(plug->tmp_l);

        if(plug->tmp_r)
            free(plug->tmp_r);
    
        plug->tmp_l = (float*)malloc(sizeof(float)*nframes);
        plug->tmp_r = (float*)malloc(sizeof(float)*nframes);
    }
#endif
    if(plug->input_l_p == plug->output_l_p )
    {
        memcpy(plug->tmp_l,plug->input_l_p,sizeof(float)*nframes);
        plug->input_l_p = plug->tmp_l;
    }
    if(plug->input_r_p == plug->output_r_p)
    {
        memcpy(plug->tmp_r,plug->input_r_p,sizeof(float)*nframes);
        plug->input_r_p = plug->tmp_r;
    }
}



static void
inline_check(RKRLV2* plug, uint32_t nframes)
{
    // copy only if needed. memcpy() src/dest memory areas must not overlap.
    if (plug->output_l_p != plug->input_l_p) {
        memcpy(plug->output_l_p,plug->input_l_p,sizeof(float)*nframes);
    }
    if (plug->output_r_p != plug->input_r_p) {
        memcpy(plug->output_r_p,plug->input_r_p,sizeof(float)*nframes);
    }
}

void
xfade_in (RKRLV2* plug, uint32_t period)
{
    unsigned int i;
    float v = 0;
    float step = 1/(float)period;
	//just a linear fade since it's (hopefully) correlated
    for (i = 0; i < period; i++)
    {
    	plug->output_l_p[i] = (v)*plug->output_l_p[i] + (1-v)*plug->input_l_p[i];
    	plug->output_r_p[i] = (v)*plug->output_r_p[i] + (1-v)*plug->input_r_p[i];
    	v+=step;
    }
}

void
xfade_out (RKRLV2* plug, uint32_t period)
{
    unsigned int i;
    float v = 0;
    float step = 1/(float)period;
	//just a linear fade since it's (hopefully) correlated
    for (i = 0; i < period; i++)
    {
    	plug->output_l_p[i] = (1-v)*plug->output_l_p[i] + v*plug->input_l_p[i];
    	plug->output_r_p[i] = (1-v)*plug->output_r_p[i] + v*plug->input_r_p[i];
    	v+=step;
    }
}

void
xfade_check (RKRLV2* plug, uint32_t period)
{
    if(*plug->bypass_p)
    {
    	plug->prev_bypass = 1;
    	xfade_out(plug,period);
    }
    else if(plug->prev_bypass)
    {
    	plug->prev_bypass = 0;
    	xfade_in(plug,period);
    }
}

void
Vol3_Efx (RKRLV2* plug, uint32_t period)
{
    unsigned i;
    float att=2.0f;

    for (i = 0; i < period; i++) {
        plug->output_l_p[i] *= att;
        plug->output_r_p[i] *= att;
    }
}

/**
 * Method mix down stereo to mono.
 * 
 * @param nframes
 *      The number of frames to process.
 * 
 * @param efx_inputl
 *      The stereo left channel to process.
 * 
 * @param efx_inputr
 *      The stereo right channel to process.
 * 
 * @param output_mono
 *      The mixed down mono channel.
 */
void
down_to_mono(uint32_t nframes, float* efx_inputl, float* efx_inputr, float *output_mono)
{
    for (unsigned i = 0; i < nframes; i++)
    {
        output_mono[i] = 0.5 * (efx_inputl[i] + efx_inputr[i]);
    }
}

/**
 *  Mix up mono to stereo.
 * 
 * @param nframes
 *      The number of frames to process.
 * 
 * @param input_mono
 *      The mono channel to mix up to stereo.
 * 
 * @param efxoutl
 *      The stereo left channel. 
 * 
 * @param efxoutr
 *      The stereo right channel.
 */
void
up_to_stereo(uint32_t nframes, float *input_mono, float* efxoutl, float* efxoutr)
{
    memcpy(efxoutl, input_mono, nframes * sizeof(float));
    memcpy(efxoutr, input_mono, nframes * sizeof(float));
}


/**
 * Add a midi message to the output port.
 * This is used by MIDIConverter
 */
void
forge_midimessage(_RKRLV2* plug,
		uint32_t tme,
		const uint8_t* const buffer,
		uint32_t size)
{
	LV2_Atom midiatom;
	midiatom.type = plug->URIDs.midi_MidiEvent;
	midiatom.size = size;

	if (0 == lv2_atom_forge_frame_time(&plug->forge, tme)) return;
	if (0 == lv2_atom_forge_raw(&plug->forge, &midiatom, sizeof(LV2_Atom))) return;
	if (0 == lv2_atom_forge_raw(&plug->forge, buffer, size)) return;
	lv2_atom_forge_pad(&plug->forge, sizeof(LV2_Atom) + size);
}


//TODO: make this return error is required feature not supported
void getFeatures(RKRLV2* plug, const LV2_Feature * const* host_features)
{
    uint8_t i,j;
    plug->period_max = INTERMEDIATE_BUFSIZE;
    plug->period_default = INTERMEDIATE_BUFSIZE;
    plug->loading_file = 0;
    plug->file_changed = 0;
    plug->scheduler = 0;
    plug->urid_map = 0;
    plug->URIDs.midi_MidiEvent = 0;
    plug->URIDs.atom_Float = 0;
    plug->URIDs.atom_Int = 0;
    plug->URIDs.atom_Object = 0;
    plug->URIDs.atom_Path = 0;
    plug->URIDs.atom_URID = 0;
    plug->URIDs.bufsz_max = 0;
    plug->URIDs.patch_Set = 0;
    plug->URIDs.patch_Get = 0;
    plug->URIDs.patch_property = 0;
    plug->URIDs.patch_value = 0;
    plug->URIDs.filetype_rvb = 0;
    plug->URIDs.filetype_dly = 0;
    plug->URIDs.filetype_snd = 0;

    for(i=0; host_features[i]; i++)
    {
        if(!strcmp(host_features[i]->URI,LV2_OPTIONS__options))
        { 
            LV2_Options_Option* option; option = (LV2_Options_Option*)host_features[i]->data;

            for(j = 0; option[j].key; j++)
            {
                if(option[j].key == plug->URIDs.bufsz_max)
                {
                    if(option[j].type == plug->URIDs.atom_Int)
                    {
                        plug->period_max = plug->period_default = *(const int*)option[j].value;
                    }
                    //other types?
                }
            }
        }
        else if(!strcmp(host_features[i]->URI,LV2_WORKER__schedule))
        {
            plug->scheduler = (LV2_Worker_Schedule*)host_features[i]->data;
        }
        else if(!strcmp(host_features[i]->URI,LV2_URID__map))
        {
            plug->urid_map = (LV2_URID_Map *) host_features[i]->data;
            if(plug->urid_map)
            {
                plug->URIDs.midi_MidiEvent = plug->urid_map->map(plug->urid_map->handle,LV2_MIDI__MidiEvent);
                plug->URIDs.atom_Float = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__Float);
                plug->URIDs.atom_Int = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__Int);
                plug->URIDs.atom_Object = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__Object);
                plug->URIDs.atom_Path = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__Path);
                plug->URIDs.atom_URID = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__URID);
                plug->URIDs.bufsz_max = plug->urid_map->map(plug->urid_map->handle,LV2_BUF_SIZE__maxBlockLength);
                plug->URIDs.patch_Set = plug->urid_map->map(plug->urid_map->handle,LV2_PATCH__Set);
                plug->URIDs.patch_Get = plug->urid_map->map(plug->urid_map->handle,LV2_PATCH__Get);
                plug->URIDs.patch_property = plug->urid_map->map(plug->urid_map->handle,LV2_PATCH__property);
                plug->URIDs.patch_value = plug->urid_map->map(plug->urid_map->handle,LV2_PATCH__value);
                plug->URIDs.filetype_rvb = plug->urid_map->map(plug->urid_map->handle,RVBFILE_URI);
                plug->URIDs.filetype_dly = plug->urid_map->map(plug->urid_map->handle,DLYFILE_URI);
                plug->URIDs.filetype_snd = plug->urid_map->map(plug->urid_map->handle,SNDFILE_URI);
            }
        }
    }
}

/////////////////////////////////////////
//      EFFECTS
////////////////////////////////////////

///// EQ /////////
LV2_Handle init_eqlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_EQ_PARAMETERS;
    plug->effectindex = IEQ;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->eq = new EQ(sample_freq, plug->period_max);

    return plug;
}

void run_eqlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->eq->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 10;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            case EQ_Gain:   // 0
            {
                val = (int)*plug->param_p[i] + 64;
                if(plug->eq->getpar(EQ_Gain) != val)
                {
                    plug->eq->changepar(EQ_Gain,val);
                }

                param_case_offset = EQ_Q; // set for EQ_Q
            }
            break;

            case EQ_Q:      // 1
            {
                val = (int)*plug->param_p[i] + 64;
                if(plug->eq->getpar(EQ_Q) != val)
                {
                    plug->eq->changepar(EQ_Q, val);
                }

                param_case_offset = EQ_31_HZ;   // set for EQ_31_HZ
            }
            break;
            
            case EQ_31_HZ:  // = 2
            case EQ_63_HZ:
            case EQ_125_HZ:
            case EQ_250_HZ:
            case EQ_500_HZ:
            case EQ_1_KHZ:
            case EQ_2_KHZ:
            case EQ_4_KHZ:
            case EQ_8_KHZ:
            case EQ_16_KHZ: // 9
            {
                val = (int)*plug->param_p[i] + 64;
                if(plug->eq->getpar(param_case_offset) != val)
                {
                    plug->eq->changepar(param_case_offset, val);
                }

                param_case_offset++;     // next parameter
            }
            break;
        }
    }

    //now run
    plug->eq->out(plug->output_l_p, plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->eq->cleanup();
    }
    
    return;
}

///// comp /////////
LV2_Handle init_complv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_COMPRESS_PARAMETERS;
    plug->effectindex = ICOMP;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    plug->comp = new Compressor(sample_freq, plug->period_max);

    return plug;
}

void run_complv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;
    
    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);
    
    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->comp->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;

    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            case Compress_Threshold:
            case Compress_Ratio:
            case Compress_Output:
            case Compress_Attack:
            case Compress_Release:
            case Compress_Auto_Out:
            case Compress_Knee:
            case Compress_Stereo:
            case Compress_Peak:
            {
                val = (int)*plug->param_p[i];
                if(plug->comp->getpar(i) != val)
                {
                    plug->comp->changepar(i, val);
                }
            }
            break;
        }
    }

    //now run
    plug->comp->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);
    
    if(plug->prev_bypass)
    {
        plug->comp->cleanup();
    }
    
    return;
}

///// dist /////////
LV2_Handle init_distlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_DIST_PARAMETERS - 1); // -1 for skipped parameter 11
    plug->effectindex = IDIST;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->dist = new Distorsion(/*oversampling*/2, /*up interpolation method*/4, /*down interpolation method*/2, sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_distlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->dist->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Dist_LR_Cross:
            case Dist_Drive:
            case Dist_Level:
            case Dist_Type:
            case Dist_Negate:
            case Dist_LPF:
            case Dist_HPF:
            case Dist_Stereo:
            case Dist_Suboctave:
            {
                val = (int)*plug->param_p[i];
                if(plug->dist->getpar(param_case_offset) != val)
                {
                    plug->dist->changepar(param_case_offset,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Dist_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->dist->getpar(Dist_DryWet) != val)
                {
                    plug->dist->changepar(Dist_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Dist_Pan:
            {
                val = (int)*plug->param_p[i] + 64;    // offset
                if(plug->dist->getpar(Dist_Pan) != val)
                {
                    plug->dist->changepar(Dist_Pan,val);
                }
            }
            break;
            
            // Skip 1 parameter after this
            case Dist_Prefilter:
            {
                val = (int)*plug->param_p[i];
                if(plug->dist->getpar(Dist_Prefilter) != val)
                {
                    plug->dist->changepar(Dist_Prefilter,val);
                }

                // increment for skipped Dist_SKIP_11
                param_case_offset++;
            }
            break;
            
        }
        // increment offset
        param_case_offset++;
    }

    //now run
    plug->dist->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->dist->outvolume, nframes);

    xfade_check(plug,nframes);
    
    if(plug->prev_bypass)
    {
        plug->dist->cleanup();
    }
    
    return;
}

///// echo /////////
LV2_Handle init_echolv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_ECHO_PARAMETERS;
    plug->effectindex = IECHO;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    
    plug->echo = new Echo(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_echolv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);
    
    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->echo->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Echo_Delay:
            case Echo_LR_Delay:
            case Echo_LR_Cross:
            case Echo_Feedback:
            case Echo_Damp:
            case Echo_Reverse:
            case Echo_Direct:
            {
                val = (int)*plug->param_p[i];
                if(plug->echo->getpar(i) != val)
                {
                    plug->echo->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Echo_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->echo->getpar(Echo_DryWet) != val)
                {
                    plug->echo->changepar(Echo_DryWet,val);
                }
            }
            break;

            // Offset
            case Echo_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->echo->getpar(Echo_Pan) != val)
                {
                    plug->echo->changepar(Echo_Pan,val);
                }
            }
        }
    }

    //now run
    plug->echo->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->echo->outvolume, nframes);

    xfade_check(plug,nframes);
    
    if(plug->prev_bypass)
    {
        plug->echo->cleanup();
    }
    
    return;
}

///// chorus /////////
LV2_Handle init_choruslv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_CHORUS_PARAMETERS - 1);  // -1 skip Chorus_SKIP_Flange_10
    plug->effectindex = ICHORUS;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    
    plug->chorus = new Chorus(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_choruslv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);
    
    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->chorus->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Chorus_LFO_Tempo:
            case Chorus_LFO_Random:
            case Chorus_LFO_Type:
            case Chorus_LFO_Stereo:
            case Chorus_Depth:
            case Chorus_Delay:
            case Chorus_Feedback:
            case Chorus_Subtract:
            case Chorus_Intense:
            {
                val = (int)*plug->param_p[i];
                if(plug->chorus->getpar(param_case_offset) != val)
                {
                    plug->chorus->changepar(param_case_offset,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Chorus_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->chorus->getpar(Chorus_DryWet) != val)
                {
                    plug->chorus->changepar(Chorus_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Chorus_Pan:
            {
                val = (int)*plug->param_p[i] + 64; // offset
                if(plug->chorus->getpar(Chorus_Pan) != val)
                {
                    plug->chorus->changepar(Chorus_Pan,val);
                }
            }
            break;
            
            // Skip after this one
            case Chorus_LR_Cross:
            {
                val = (int)*plug->param_p[i];
                if(plug->chorus->getpar(Chorus_LR_Cross) != val)
                {
                    plug->chorus->changepar(Chorus_LR_Cross,val);
                }
                // increment for skipped parameter
                param_case_offset++;
            }
            break;
        }
        param_case_offset++;
    }

    //now run
    plug->chorus->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->chorus->outvolume, nframes);

    xfade_check(plug,nframes);
    
    if(plug->prev_bypass)
    {
        plug->chorus->cleanup();
    }
    
    return;
}

///// Analog Phaser /////////
LV2_Handle init_aphaselv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_APHASER_PARAMETERS;
    plug->effectindex = IAPHASE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    
    plug->aphase = new Analog_Phaser(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_aphaselv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);
    
    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->aphase->lv2_update_params(nframes);
    }
    
    // we are good to run now
    
    //check and set changed parameters
    int val = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case APhase_Distortion:
            case APhase_LFO_Tempo:
            case APhase_LFO_Random:
            case APhase_LFO_Type:
            case APhase_LFO_Stereo:
            case APhase_Width:
            case APhase_Stages:
            case APhase_Mismatch:
            case APhase_Subtract:
            case APhase_Depth:
            case APhase_Hyper:
            {
                val = (int)*plug->param_p[i];
                if(plug->aphase->getpar(i) != val)
                {
                    plug->aphase->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case APhase_DryWet: 
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->aphase->getpar(APhase_DryWet) != val)
                {
                    plug->aphase->changepar(APhase_DryWet,val);
                }
            }
            break;
            
            // Offset
            case APhase_Feedback:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->aphase->getpar(APhase_Feedback) != val)
                {
                    plug->aphase->changepar(APhase_Feedback,val);
                }
            }
            break;
        }
    }

    //now run
    plug->aphase->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->aphase->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->aphase->cleanup();
    }

    return;
}

//////// harmonizer - no MIDI /////////
LV2_Handle init_harmnomidlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_HARM_PARAMETERS - 1); // -1 for Harm_MIDI - since this is no MIDI
    plug->effectindex = IHARM_NM;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    //magic numbers: shift qual 4, downsample 5, up qual 4, down qual 2,
    plug->harm = new Harmonizer(4,5,4,2, sample_freq, plug->period_max);
    plug->noteID = new Recognize(.6,440.0, sample_freq, plug->period_max);//.6 is default trigger value
    plug->chordID = new RecChord();
    plug->noteID->reconota = -1;

    // set in :void RKRGUI::cb_RC_Opti_i(Fl_Choice* o, void*) and used by schmittFloat();
    plug->noteID->setlpf(5500); // default user option in rakarrack
    plug->noteID->sethpf(80); // default user option in rakarrack

    plug->comp = new Limiter(sample_freq, plug->period_max);
    plug->comp->setpreset(0); //Final Limiter
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

/*
    plug->comp->changepar(1,-24);//threshold
    plug->comp->changepar(2,4);  //ratio
    plug->comp->changepar(3,-10);//output
    plug->comp->changepar(4,20); //attack
    plug->comp->changepar(5,50); //release
    plug->comp->changepar(6,1);  //a_out
    plug->comp->changepar(7,30); //knee
*/
    return plug;
}

void run_harmnomidlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
    
    /* Return from bypass, reset */
    if(plug->prev_bypass)
    {
        plug->harm->cleanup();
        // update parameters after cleanup - interval
        plug->harm->changepar(Harm_Interval,plug->harm->getpar(Harm_Interval));
        //mark chord has changed to update parameters after cleanup
        plug->chordID->cc = 1;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->harm->lv2_update_params(nframes);
        plug->comp->lv2_update_params(nframes);
        plug->noteID->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Harm_Filter_Freq:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                }
            }
            break;
            
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Harm_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->harm->getpar(Harm_DryWet) != val)
                {
                    plug->harm->changepar(Harm_DryWet,val);
                }
            }
            break;


            // Offset 64
            case Harm_Pan:
            case Harm_Gain:
            case Harm_Filter_Gain:
            case Harm_Filter_Q:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                }
            }
            break;

            // Offset 12
            case Harm_Interval:
            {
                val = (int)*plug->param_p[i] + 12;  // offset
                if(plug->harm->getpar(Harm_Interval) != val)
                {
                    plug->harm->changepar(Harm_Interval,val);
                }
            }
            break;
            
            // Select Mode
            case Harm_Select:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                    plug->chordID->cleanup();
                    if(!val) plug->harm->changepar(Harm_Interval,plug->harm->getpar(Harm_Interval)); // Reset 

                    plug->chordID->cc = 1;  // mark chord changed to update after cleanup
                }
            }
            break;

            // Note and Chord
            case Harm_Note:
            case Harm_Chord:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                //    plug->chordID->ctipo = plug->harm->getpar(7);//set chord type
                //    plug->chordID->fundi = plug->harm->getpar(6);//set root note
                    plug->chordID->cc = 1;  // mark chord has changed
                }
            }
            break;
        }
    }

/*
see Chord() in rkr.fl
harmonizer, need recChord and recNote.
see process.C ln 1507
*/
    
    if(have_signal(plug->output_l_p, plug->output_r_p, nframes))
    {
        if(plug->harm->mira)
        {
            if(plug->harm->PSELECT)
            {
                plug->noteID->schmittFloat(plug->output_l_p,plug->output_r_p);
                if((plug->noteID->reconota != -1) && (plug->noteID->reconota != plug->noteID->last))
                {
                    if(plug->noteID->afreq > 0.0)
                    {
                        plug->chordID->Vamos(0,plug->harm->Pinterval - 12,plug->noteID->reconota);
                        plug->harm->r_ratio = plug->chordID->r__ratio[0];   // pass the found ratio
                        plug->noteID->last = plug->noteID->reconota;
                    }
                }
            }
        }
    }
    
    if(plug->harm->PSELECT)
    {
        if (plug->chordID->cc) 
        {
            plug->chordID->cc = 0;
            plug->chordID->ctipo = plug->harm->getpar(Harm_Chord);  // set chord type
            plug->chordID->fundi = plug->harm->getpar(Harm_Note);   // set root note
            plug->chordID->Vamos(0,plug->harm->Pinterval - 12,plug->noteID->reconota);
            plug->harm->r_ratio = plug->chordID->r__ratio[0];       // pass the found ratio
        }
        plug->comp->out(plug->output_l_p,plug->output_r_p);
    }
    
    //now run
    plug->harm->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->harm->outvolume, nframes);

    xfade_check(plug,nframes);

    return;
}


///// exciter /////////
LV2_Handle init_exciterlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_EXCITER_PARAMETERS;
    plug->effectindex = IEXCITER;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->exciter = new Exciter(sample_freq, plug->period_max);

    return plug;
}

void run_exciterlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->exciter->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Exciter_Gain:
            case Exciter_Harm_1:
            case Exciter_Harm_2:
            case Exciter_Harm_3:
            case Exciter_Harm_4:
            case Exciter_Harm_5:
            case Exciter_Harm_6:
            case Exciter_Harm_7:
            case Exciter_Harm_8:
            case Exciter_Harm_9:
            case Exciter_Harm_10:
            case Exciter_LPF:
            case Exciter_HPF:
            {
                val = (int)*plug->param_p[i];
                if(plug->exciter->getpar(i) != val)
                {
                    plug->exciter->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->exciter->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->exciter->cleanup();
    }

    return;
}

///// pan /////////
LV2_Handle init_panlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_PAN_PARAMETERS;
    plug->effectindex = IPAN;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    
    plug->pan = new Pan(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_panlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->pan->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Pan_LFO_Tempo:
            case Pan_LFO_Random:
            case Pan_LFO_Type:
            case Pan_LFO_Stereo:
            case Pan_Ex_St_Amt:
            case Pan_AutoPan:
            case Pan_Enable_Extra:
            {
                val = (int)*plug->param_p[i];
                if(plug->pan->getpar(i) != val)
                {
                    plug->pan->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Pan_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->pan->getpar(Pan_DryWet) != val)
                {
                    plug->pan->changepar(Pan_DryWet,val);
                }
            }
            break;

            // Offset
            case Pan_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->pan->getpar(Pan_Pan) != val)
                {
                    plug->pan->changepar(Pan_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->pan->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->pan->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->pan->cleanup();
    }

    return;
}

///// AlienWah /////////
LV2_Handle init_alienlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_ALIENWAH_PARAMETERS;
    plug->effectindex = IAWAH;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    
    plug->alien = new Alienwah(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_alienlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;
    
    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->alien->lv2_update_params(nframes);
    }
    
    // we are good to run now
    
    //check and set changed parameters
    int val = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Alien_LFO_Tempo:
            case Alien_LFO_Random:
            case Alien_LFO_Type:
            case Alien_LFO_Stereo:
            case Alien_Depth:
            case Alien_Feedback:
            case Alien_Delay:
            case Alien_LR_Cross:
            case Alien_Phase:
            {
                val = (int)*plug->param_p[i];
                if(plug->alien->getpar(i) != val)
                {
                    plug->alien->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Alien_DryWet: 
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->alien->getpar(Alien_DryWet) != val)
                {
                    plug->alien->changepar(Alien_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Alien_Pan:
            {
                val = (int)*plug->param_p[i] + 64;   // offset
                if(plug->alien->getpar(Alien_Pan) != val)
                {
                    plug->alien->changepar(Alien_Pan,val);
                }
            }
            break;
        }
    }
    
    //now run
    plug->alien->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->alien->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->alien->cleanup();
    }

    return;
}

///// reverb /////////
LV2_Handle init_revelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_REVERB_PARAMETERS - 2); // -2 skips 5 & 6 unused
    plug->effectindex = IREV;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->reve = new Reverb(sample_freq,plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_revelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->reve->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Reverb_Time:
            case Reverb_I_Delay:
            case Reverb_LPF:
            case Reverb_HPF:
            case Reverb_Damp:
            case Reverb_Type:
            case Reverb_Room:
            {
                val = (int)*plug->param_p[i];
                if(plug->reve->getpar(param_case_offset) != val)
                {
                    plug->reve->changepar(param_case_offset,val);
                }
            }
            break;

            // wet/dry -> dry/wet reversal
            case Reverb_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->reve->getpar(Reverb_DryWet) != val)
                {
                    plug->reve->changepar(Reverb_DryWet,val);
                }
            }
            break;

            // Offset
            case Reverb_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->reve->getpar(Reverb_Pan) != val)
                {
                    plug->reve->changepar(Reverb_Pan,val);
                }
            }
            break;

            // Skip after
            case Reverb_Delay_FB:
            {
                val = (int)*plug->param_p[i];
                if(plug->reve->getpar(param_case_offset) != val)
                {
                    plug->reve->changepar(param_case_offset,val);
                }
                param_case_offset += 2; // Skip 5 & 6
            }
            break;
        }
        
        param_case_offset++;
    }

    //now run
    plug->reve->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->reve->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->reve->cleanup();
    }

    return;
}

///// EQ Parametric /////////
LV2_Handle init_eqplv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_PARAMETRIC_EQ_PARAMETERS;
    plug->effectindex = IEQP;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->peq = new ParametricEQ(sample_freq, plug->period_max);

    return plug;
}

void run_eqplv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->peq->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = Parametric_Gain;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            case Parametric_Gain:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->peq->getpar(Parametric_Gain) != val)
                {
                    plug->peq->changepar(Parametric_Gain,val);
                }

                // the subsequent parameters start at 0, we add one on break
                param_case_offset = -1;
            }
            break;

            case Parametric_Low_Freq:
            case Parametric_Mid_Freq:
            case Parametric_High_Freq:
            {
                val = (int)*plug->param_p[i];
                if(plug->peq->getpar(param_case_offset) != val)
                {
                    plug->peq->changepar(param_case_offset,val);
                }
            }
            break;
                
            case Parametric_Low_Gain:
            case Parametric_Low_Q:
            case Parametric_Mid_Gain:
            case Parametric_Mid_Q:
            case Parametric_High_Gain:
            case Parametric_High_Q:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->peq->getpar(param_case_offset) != val)
                {
                    plug->peq->changepar(param_case_offset,val);
                }

            }
            break;
        }
        
        param_case_offset++;
    }

    //now run
    plug->peq->out(plug->output_l_p, plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->peq->cleanup();
    }

    return;
}

///// Cabinet /////////
LV2_Handle init_cablv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_CABINET_PARAMETERS;  // This is not used
    plug->effectindex = ICAB;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->cab = new Cabinet(sample_freq, plug->period_max);

    return plug;
}

void run_cablv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->cab->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    // The parameters for gain and preset are reversed for LV2
    int val = (int)*plug->param_p[0] + 64;
    if(plug->cab->getpar(Cabinet_Gain) != val)
    {
        plug->cab->changepar(Cabinet_Gain,val);
    }

    val = (int)*plug->param_p[1];
    if(plug->cab->getpar(Cabinet_Preset_Idx) != val)
    {
        plug->cab->changepar(Cabinet_Preset_Idx, val);
    }

    //now run
    plug->cab->out(plug->output_l_p, plug->output_r_p);
    
    //post effect volume adjustment from rakarrack
    Vol3_Efx(plug,nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->cab->cleanup();
    }

    return;
}

///// Musical Delay /////////
LV2_Handle init_mdellv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_MUSIC_PARAMETERS;
    plug->effectindex = IMDEL;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    
    plug->mdel = new MusicDelay (sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_mdellv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->mdel->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Music_Delay_1:
            case Music_Del_Offset:
            case Music_LR_Cross:
            case Music_Feedback_1:
            case Music_Damp:
            case Music_Delay_2:
            case Music_Feedback_2:
            case Music_Tempo:
            case Music_Gain_1:
            case Music_Gain_2:
            {
                val = (int)*plug->param_p[i];
                if(plug->mdel->getpar(i) != val)
                {
                    plug->mdel->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Music_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->mdel->getpar(Music_DryWet) != val)
                {
                    plug->mdel->changepar(Music_DryWet,val);
                }
            }
            break;

            // Offset
            case Music_Pan_1:
            case Music_Pan_2:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->mdel->getpar(i) != val)
                {
                    plug->mdel->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->mdel->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->mdel->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->mdel->cleanup();
    }

    return;
}

///// wahwah /////////
LV2_Handle init_wahlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_WAHWAH_PARAMETERS;
    plug->effectindex = IWAH;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->wah = new WahWah(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_wahlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->wah->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case WahWah_LFO_Tempo:
            case WahWah_LFO_Random:
            case WahWah_LFO_Type:
            case WahWah_LFO_Stereo:
            case WahWah_Depth:
            case WahWah_Sense:
            case WahWah_ASI:
            case WahWah_Smooth:
            case WahWah_Mode:
            {
                val = (int)*plug->param_p[i];
                if(plug->wah->getpar(i) != val)
                {
                    plug->wah->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case WahWah_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->wah->getpar(WahWah_DryWet) != val)
                {
                    plug->wah->changepar(WahWah_DryWet,val);
                }
            }
            break;

            // Offset
            case WahWah_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->wah->getpar(WahWah_Pan) != val)
                {
                    plug->wah->changepar(WahWah_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->wah->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->wah->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->wah->cleanup();
    }

    return;
}

///// derelict /////////
LV2_Handle init_derelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_DERE_PARAMETERS;
    plug->effectindex = IDERE;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->dere = new Derelict( /*oversampling*/2, /*up interpolation method*/4, /*down interpolation method*/2, sample_freq, plug->period_max); 
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_derelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->dere->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Dere_LR_Cross:
            case Dere_Drive:
            case Dere_Level:
            case Dere_Type:
            case Dere_Negate:
            case Dere_LPF:
            case Dere_HPF:
            case Dere_Color:
            case Dere_Prefilter:
            case Dere_Suboctave:
            {
                val = (int)*plug->param_p[i];
                if(plug->dere->getpar(i) != val)
                {
                    plug->dere->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Dere_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->dere->getpar(Dere_DryWet) != val)
                {
                    plug->dere->changepar(Dere_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Dere_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->dere->getpar(Dere_Pan) != val)
                {
                    plug->dere->changepar(Dere_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->dere->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->dere->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->dere->cleanup();
    }

    return;
}

///// valve /////////
LV2_Handle init_valvelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_VALVE_PARAMETERS;
    plug->effectindex = IVALVE;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->valve = new Valve(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_valvelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->valve->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Valve_LR_Cross:
            case Valve_Drive:
            case Valve_Level:
            case Valve_Negate:
            case Valve_LPF:
            case Valve_HPF:
            case Valve_Stereo:
            case Valve_Prefilter:
            case Valve_Distortion:
            case Valve_Ex_Dist:
            case Valve_Presence:
            {
                val = (int)*plug->param_p[i];
                if(plug->valve->getpar(i) != val)
                {
                    plug->valve->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Valve_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->valve->getpar(Valve_DryWet) != val)
                {
                    plug->valve->changepar(Valve_DryWet,val);
                }
            }
            break;

            // Offset
            case Valve_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->valve->getpar(Valve_Pan) != val)
                {
                    plug->valve->changepar(Valve_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->valve->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->valve->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->valve->cleanup();
    }

    return;
}

///// dual flange /////////
LV2_Handle init_dflangelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_DFLANGE_PARAMETERS;
    plug->effectindex = IDFLANGE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    plug->dflange = new Dflange(sample_freq, plug->period_max);

    return plug;
}

void run_dflangelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->dflange->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            case DFlange_Pan:
            case DFlange_LR_Cross:
            case DFlange_Depth:
            case DFlange_Width:
            case DFlange_Offset:
            case DFlange_Feedback:
            case DFlange_LPF:
            case DFlange_Subtract:
            case DFlange_Zero:
            case DFlange_LFO_Tempo:
            case DFlange_LFO_Stereo:
            case DFlange_LFO_Type:
            case DFlange_LFO_Random:
            case DFlange_Intense:
            {
                val = (int)*plug->param_p[i];
                if(plug->dflange->getpar(i) != val)
                {
                    plug->dflange->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case DFlange_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->dflange->getpar(DFlange_DryWet) != val)
                {
                    plug->dflange->changepar(DFlange_DryWet,val);
                }
            }
            break;
        }
    }

    //now run
    plug->dflange->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->dflange->cleanup();
    }

    return;
}

//////// ring /////////
LV2_Handle init_ringlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_RING_PARAMETERS;
    plug->effectindex = IRING;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    //magic numbers: shift qual 4, downsample 5, up qual 4, down qual 2,
    plug->ring = new Ring(sample_freq, plug->period_max);
    plug->noteID = new Recognize(.6,440.0, sample_freq, plug->period_max);//.6 is default trigger value
    plug->noteID->reconota = -1;
    
    plug->noteID->setlpf(5500);
    plug->noteID->sethpf(80);

    plug->comp = new Limiter(sample_freq, plug->period_max);
    plug->comp->setpreset(0); //Final Limiter
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);
    
    return plug;
}

void run_ringlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->ring->lv2_update_params(nframes);
        plug->noteID->lv2_update_params(nframes);
        plug->comp->lv2_update_params(nframes);
     //   plug->noteID->setlpf(5500);
     //   plug->noteID->sethpf(80);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Ring_Pan:
            case Ring_LR_Cross:
            case Ring_Level:
            case Ring_Depth:
            case Ring_Freq:
            case Ring_Stereo:
            case Ring_Sine:
            case Ring_Triangle:
            case Ring_Saw:
            case Ring_Square:
            case Ring_Input:
            case Ring_Auto_Freq:
            {
                val = (int)*plug->param_p[i];
                if(plug->ring->getpar(i) != val)
                {
                    plug->ring->changepar(i,val);
                }
            }
            break;

            // Special cases

            // wet/dry -> dry/wet reversal
            case Ring_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->ring->getpar(Ring_DryWet) != val)
                {
                    plug->ring->changepar(Ring_DryWet,val);
                }
            }
            break;
        }
    }
//see process.C ln 1539

    if(plug->ring->Pafreq)
    {
        plug->noteID->schmittFloat(plug->output_l_p,plug->output_r_p);
        if(plug->noteID->reconota != -1 && plug->noteID->reconota != plug->noteID->last)
        {
            if(plug->noteID->afreq > 0.0)
            {
                plug->ring->Pfreq = lrintf(plug->noteID->lafreq);//round
                plug->noteID->last = plug->noteID->reconota;
            }
        }
    }

    if(plug->ring->Pafreq)
    {
        plug->comp->out(plug->output_l_p,plug->output_r_p);
    }
    
    //now run
    plug->ring->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->ring->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->ring->cleanup();
    }

    return;
}


///// distband /////////
LV2_Handle init_distbandlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_DBAND_PARAMETERS;
    plug->effectindex = IDISTBAND;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->distband = new DistBand( /*oversampling*/2, /*up interpolation method*/4, /*down interpolation method*/2, sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_distbandlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->distband->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case DistBand_LR_Cross:
            case DistBand_Drive:
            case DistBand_Level:
            case DistBand_Type_Low:
            case DistBand_Type_Mid:
            case DistBand_Type_Hi:
            case DistBand_Gain_Low:
            case DistBand_Gain_Mid:
            case DistBand_Gain_Hi:
            case DistBand_Negate:
            case DistBand_Cross_1:
            case DistBand_Cross_2:
            case DistBand_Stereo:
            {
                val = (int)*plug->param_p[i];
                if(plug->distband->getpar(i) != val)
                {
                    plug->distband->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case DistBand_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->distband->getpar(DistBand_DryWet) != val)
                {
                    plug->distband->changepar(DistBand_DryWet,val);
                }
            }
            break;
            
            // Offset
            case DistBand_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->distband->getpar(DistBand_Pan) != val)
                {
                    plug->distband->changepar(DistBand_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->distband->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->distband->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->distband->cleanup();
    }

    return;
}

///// arp /////////
LV2_Handle init_arplv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_ARPIE_PARAMETERS;
    plug->effectindex = IARPIE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // needed to set period_max
    
    plug->arp = new Arpie(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_arplv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->arp->lv2_update_params(nframes);
    }
    
    // we are good to run now
    //check and set changed parameters
    int val = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Arpie_Tempo:
            case Arpie_LR_Delay:
            case Arpie_LR_Cross:
            case Arpie_Feedback:
            case Arpie_Damp:
            case Arpie_ArpeWD:
            case Arpie_Harm:
            case Arpie_Pattern:
            case Arpie_Subdivision:
            {
                val = (int)*plug->param_p[i];
                if(plug->arp->getpar(i) != val)
                {
                    plug->arp->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Arpie_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->arp->getpar(Arpie_DryWet) != val)
                {
                    plug->arp->changepar(Arpie_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Arpie_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->arp->getpar(Arpie_Pan) != val)
                {
                    plug->arp->changepar(Arpie_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->arp->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->arp->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->arp->cleanup();
    }

    return;
}

///// expand /////////
LV2_Handle init_expandlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_EXPANDER_PARAMETERS;
    plug->effectindex = IEXPAND;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->expand = new Expander(sample_freq, plug->period_max);

    return plug;
}

void run_expandlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->expand->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Expander_Threshold:
            case Expander_Shape:
            case Expander_Attack:
            case Expander_Release:
            case Expander_LPF:
            case Expander_HPF:
            case Expander_Gain:
            {
                val = (int)*plug->param_p[i];
                if(plug->expand->getpar(i) != val)
                {
                    plug->expand->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->expand->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->expand->cleanup();
    }

    return;
}

///// shuffle /////////
LV2_Handle init_shuflv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_SHUFFLE_PARAMETERS;
    plug->effectindex = ISHUFF;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->shuf = new Shuffle(sample_freq,plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_shuflv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->shuf->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Shuffle_Gain_L:
            case Shuffle_Gain_ML:
            case Shuffle_Gain_MH:
            case Shuffle_Gain_H:
            case Shuffle_Freq_L:
            case Shuffle_Freq_ML:
            case Shuffle_Freq_MH:
            case Shuffle_Freq_H:
            case Shuffle_Width:
            case Shuffle_F_Band:
            {
                val = (int)*plug->param_p[i];
                if(plug->shuf->getpar(i) != val)
                {
                    plug->shuf->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Shuffle_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->shuf->getpar(Shuffle_DryWet) != val)
                {
                    plug->shuf->changepar(Shuffle_DryWet,val);
                }
            }
            break;
        }
    }

    //now run
    plug->shuf->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->shuf->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->shuf->cleanup();
    }

    return;
}


///// synth /////////
LV2_Handle init_synthlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_SYNTHFILTER_PARAMETERS;
    plug->effectindex = ISYNTH;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    
    plug->synth = new Synthfilter(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_synthlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->synth->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Synthfilter_Distort:
            case Synthfilter_LFO_Tempo:
            case Synthfilter_LFO_Random:
            case Synthfilter_LFO_Type:
            case Synthfilter_LFO_Stereo:
            case Synthfilter_Width:
            case Synthfilter_Feedback:
            case Synthfilter_LPF_Stages:
            case Synthfilter_HPF_Stages:
            case Synthfilter_Subtract:
            case Synthfilter_Depth:
            case Synthfilter_Env_Sens:
            case Synthfilter_Attack:
            case Synthfilter_Release:
            case Synthfilter_Offset:
            {
                val = (int)*plug->param_p[i];
                if(plug->synth->getpar(i) != val)
                {
                    plug->synth->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Synthfilter_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->synth->getpar(Synthfilter_DryWet) != val)
                {
                    plug->synth->changepar(Synthfilter_DryWet,val);
                }
            }
            break;
        }
    }

    //now run
    plug->synth->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->synth->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->synth->cleanup();
    }

    return;
}

///// varyband /////////
LV2_Handle init_varybandlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    // +3 for the 4 LV2 parameters added - 1 for legacy pcombi
    plug->nparams = (C_VARYBAND_PARAMETERS + 3);
    
    plug->effectindex = IVBAND;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->varyband = new VaryBand(sample_freq,plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_varybandlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->varyband->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case VaryBand_LFO_Tempo_1:
            case VaryBand_LFO_Type_1:
            case VaryBand_LFO_Stereo_1:
            case VaryBand_LFO_Tempo_2:
            case VaryBand_LFO_Type_2:
            case VaryBand_LFO_Stereo_2:
            case VaryBand_Cross_1:
            case VaryBand_Cross_2:
            case VaryBand_Low_Band:
            case VaryBand_Mid_Band_1:
            case VaryBand_Mid_Band_2:
            case VaryBand_High_Band:
            {
                val = (int)*plug->param_p[i];
                if(plug->varyband->getpar(param_case_offset) != val)
                {
                    plug->varyband->changepar(param_case_offset,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case VaryBand_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->varyband->getpar(VaryBand_DryWet) != val)
                {
                    plug->varyband->changepar(VaryBand_DryWet,val);
                }
            }
            break;

            // Skip after
            case VaryBand_Cross_3:
            {
                val = (int)*plug->param_p[i];
                if(plug->varyband->getpar(VaryBand_Cross_3) != val)
                {
                    plug->varyband->changepar(VaryBand_Cross_3,val);
                }
                param_case_offset++;    // skip VaryBand_Combination (legacy)
            }
            break;
        }

        param_case_offset++;
    }

    //now run
    plug->varyband->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->varyband->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->varyband->cleanup();
    }

    return;
}

///// mutro /////////
LV2_Handle init_mutrolv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    // -1 for Mutro_Mode_Legacy, +2 for MuTro_AG_Mode, MuTro_Exp_Wah
    plug->nparams = (C_MUTRO_PARAMETERS + 1);
    plug->effectindex = IMUTRO;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->mutro = new MuTroMojo(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_mutrolv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->mutro->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case MuTro_Resonance:
            case MuTro_LFO_Tempo:
            case MuTro_LFO_Random:
            case MuTro_LFO_Type:
            case MuTro_Depth:
            case MuTro_Env_Sens:
            case MuTro_Wah:
            case MuTro_Env_Smooth:
            case MuTro_LowPass:
            case MuTro_BandPass:
            case MuTro_HighPass:
            case MuTro_Stages:
            case MuTro_Range:
            case MuTro_St_Freq:
            case MuTro_AG_Mode:
            case MuTro_Exp_Wah:
            {
                val = (int)*plug->param_p[i];
                if(plug->mutro->getpar(param_case_offset) != val)
                {
                    plug->mutro->changepar(param_case_offset,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case MuTro_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->mutro->getpar(MuTro_DryWet) != val)
                {
                    plug->mutro->changepar(MuTro_DryWet,val);
                }
            }
            break;

            // Offset
            case MuTro_LFO_Stereo:
            {
                val = (int)*plug->param_p[i] + 64; // offset
                if(plug->mutro->getpar(MuTro_LFO_Stereo) != val)
                {
                    plug->mutro->changepar(MuTro_LFO_Stereo,val);
                }
            }
            break;

            // Skip after
            case MuTro_Mod_Res:
            {
                val = (int)*plug->param_p[i];
                if(plug->mutro->getpar(MuTro_Mod_Res) != val)
                {
                    plug->mutro->changepar(MuTro_Mod_Res,val);
                }
                param_case_offset += 1; // Skip: Mutro_Mode_Legacy
            }
            break;
        }
        
        param_case_offset++;
    }

    //now run
    plug->mutro->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->mutro->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->mutro->cleanup();
    }

    return;
}

///// echoverse /////////
LV2_Handle init_echoverselv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_ECHOVERSE_PARAMETERS;
    plug->effectindex = IECHOVERSE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    
    plug->echoverse = new Echoverse(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_echoverselv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->echoverse->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Echoverse_Tempo:
            case Echoverse_LR_Delay:
            case Echoverse_Feedback:
            case Echoverse_Damp:
            case Echoverse_Reverse:
            case Echoverse_Subdivision:
            case Echoverse_Ext_Stereo:
            {
                val = (int)*plug->param_p[i];
                if(plug->echoverse->getpar(i) != val)
                {
                    plug->echoverse->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Echoverse_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->echoverse->getpar(Echoverse_DryWet) != val)
                {
                    plug->echoverse->changepar(Echoverse_DryWet,val);
                }
            }
            break;

            // Offset
            case Echoverse_Pan:
            case Echoverse_Angle:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->echoverse->getpar(i) != val)
                {
                    plug->echoverse->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->echoverse->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->echoverse->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->echoverse->cleanup();
    }

    return;
}

///// coilcrafter /////////
LV2_Handle init_coillv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_COIL_PARAMETERS - 2);  // -2  Coil_Origin, Coil_Destiny skipped
    plug->effectindex = ICOIL;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->coil = new CoilCrafter(sample_freq,plug->period_max);

    return plug;
}

void run_coillv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->coil->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Coil_Freq_1:
            case Coil_Q_1:
            case Coil_Freq_2:
            case Coil_Q_2:
            case Coil_Tone:
            case Coil_NeckMode:
            {
                val = (int)*plug->param_p[i];
                if(plug->coil->getpar(param_case_offset) != val)
                {
                    plug->coil->changepar(param_case_offset,val);
                }
            }
            break;
            
            //  Coil_Origin, Coil_Destiny, are skipped after gain
            case Coil_Gain:
            {
                val = (int)*plug->param_p[i];
                if(plug->coil->getpar(Coil_Gain) != val)
                {
                    plug->coil->changepar(Coil_Gain,val);
                }

                // skip Coil_Origin, Coil_Destiny
                param_case_offset += 2;
            }
            break;
        }
        
        param_case_offset++;
    }

    //now run
    plug->coil->out(plug->output_l_p, plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->coil->cleanup();
    }

    return;
}

///// shelfboost /////////
LV2_Handle init_shelflv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_SHELF_PARAMETERS;
    plug->effectindex = ISHELF;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->shelf = new ShelfBoost(sample_freq, plug->period_max);

    return plug;
}

void run_shelflv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->shelf->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Shelf_Gain:
            case Shelf_Presence:
            case Shelf_Tone:
            case Shelf_Stereo:
            case Shelf_Level:
            {
                val = (int)*plug->param_p[i];
                if(plug->shelf->getpar(i) != val)
                {
                    plug->shelf->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->shelf->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->shelf->cleanup();
    }

    return;
}

///// Vocoder /////////
LV2_Handle init_voclv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_VOCODER_PARAMETERS;
    plug->effectindex = IVOC;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->voc = new Vocoder(0,/*bands*/32,/*downsamplex2*/5,/*upsample quality*/4,
                            /*downsample quality*/ 2,sample_freq,plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_voclv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->voc->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Vocoder_Smear:
            case Vocoder_Q:
            case Vocoder_Input:
            case Vocoder_Level:
            case Vocoder_Ring:
            {
                val = (int)*plug->param_p[i];
                if(plug->voc->getpar(i) != val)
                {
                    plug->voc->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Vocoder_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->voc->getpar(Vocoder_DryWet) != val)
                {
                    plug->voc->changepar(Vocoder_DryWet,val);
                }
            }
            break;

            // Offset
            case Vocoder_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->voc->getpar(Vocoder_Pan) != val)
                {
                    plug->voc->changepar(Vocoder_Pan,val);
                }
            }
            break;
        }
    }

    //set aux input port
    plug->voc->auxresampled = plug->param_p[VOCODER_AUX_IN];

    //now run
    plug->voc->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->voc->outvolume, nframes);

    //and set VU meter
    *plug->param_p[VOCODER_VU_LEVEL] = plug->voc->vulevel;

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->voc->cleanup();
    }

    return;
}

///// Sustainer /////////
LV2_Handle init_suslv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;
    
    plug->nparams = C_SUSTAIN_PARAMETERS;
    plug->effectindex = ISUS;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->sus = new Sustainer(sample_freq, plug->period_max);

    return plug;
}

void run_suslv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->sus->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Sustain_Gain:
            case Sustain_Sustain:
            {
                val = (int)*plug->param_p[i];
                if(plug->sus->getpar(i) != val)
                {
                    plug->sus->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->sus->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->sus->cleanup();
    }

    return;
}

///// Sequence /////////
LV2_Handle init_seqlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_SEQUENCE_PARAMETERS;
    plug->effectindex = ISEQ;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->seq = new Sequence(/*shifter quality*/4,/*downsamplex2*/5,/*upsample quality*/4,
                             /*downsample quality*/ 2,sample_freq,plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_seqlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->seq->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Sequence_Step_1:
            case Sequence_Step_2:
            case Sequence_Step_3:
            case Sequence_Step_4:
            case Sequence_Step_5:
            case Sequence_Step_6:
            case Sequence_Step_7:
            case Sequence_Step_8:
            case Sequence_Tempo:
            case Sequence_Amp:
            case Sequence_Stdf:
            case Sequence_Mode:
            case Sequence_Range:
            {
                val = (int)*plug->param_p[i];
                if(plug->seq->getpar(i) != val)
                {
                    plug->seq->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Sequence_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->seq->getpar(Sequence_DryWet) != val)
                {
                    plug->seq->changepar(Sequence_DryWet,val);
                }
            }
            break;

            // Offset
            case Sequence_Resonance:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->seq->getpar(Sequence_Resonance) != val)
                {
                    plug->seq->changepar(Sequence_Resonance,val);
                }
            }
            break;
        }
    }

    //now run
    plug->seq->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->seq->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->seq->cleanup();
    }

    return;
}

///// Shifter /////////
LV2_Handle init_shiftlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_SHIFTER_PARAMETERS;
    plug->effectindex = ISHIFT;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->shift = new Shifter(/*shifter quality*/4,/*downsamplex2*/5,/*upsample quality*/4,
                              /*downsample quality*/ 2,sample_freq,plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_shiftlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->shift->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Shifter_Attack:
            case Shifter_Decay:
            case Shifter_Threshold:
            case Shifter_Interval:
            case Shifter_Shift:
            case Shifter_Mode:
            case Shifter_Whammy:
            {
                val = (int)*plug->param_p[i];
                if(plug->shift->getpar(i) != val)
                {
                    plug->shift->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Shifter_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->shift->getpar(Shifter_DryWet) != val)
                {
                    plug->shift->changepar(Shifter_DryWet,val);
                }
            }
            break;

            // Offset
            case Shifter_Pan:
            case Shifter_Gain:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->shift->getpar(i) != val)
                {
                    plug->shift->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->shift->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->shift->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->shift->cleanup();
    }

    return;
}

///// StompBox /////////
LV2_Handle init_stomplv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_STOMP_PARAMETERS;
    plug->effectindex = ISTOMP;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->stomp = new StompBox(/*oversampling*/2,/*up interpolation method*/4, /*down interpolation method*/2, sample_freq, plug->period_max);

    return plug;
}

void run_stomplv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->stomp->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Stomp_Level:
            case Stomp_Tone:
            case Stomp_Mid:
            case Stomp_Bias:
            case Stomp_Gain:
            case Stomp_Mode:
            {
                val = (int)*plug->param_p[i];
                if(plug->stomp->getpar(i) != val)
                {
                    plug->stomp->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->stomp->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->stomp->cleanup();
    }

    return;
}

///// Reverbtron /////////
LV2_Handle init_revtronlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_REVTRON_PARAMETERS - 2);  // -2 Revtron_User_File, Revtron_Set_File
    plug->effectindex = IREVTRON;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);
    if(!plug->scheduler || !plug->urid_map)
    {
    	//a required feature was not supported by host
    	free(plug);
    	return 0;
    }
    lv2_atom_forge_init(&plug->forge, plug->urid_map);

    plug->revtron = new Reverbtron( /*downsample*/5, /*up interpolation method*/4, /*down interpolation method*/2 ,sample_freq, plug->period_max);
    plug->revtron->changepar(Revtron_User_File,1);  // set to user selected files

    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_revtronlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }

    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->revtron->lv2_update_params(nframes);
    }

    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Revtron_Fade:
            case Revtron_Safe:
            case Revtron_I_Delay:
            case Revtron_Damp:
            case Revtron_Stretch:
            case Revtron_Feedback:
            case Revtron_Ex_Stereo:
            case Revtron_Shuffle:
            case Revtron_LPF:
            case Revtron_Diffusion:
            {
                val = (int)*plug->param_p[i];
                if(plug->revtron->getpar(param_case_offset) != val)
                {
                    plug->revtron->changepar(param_case_offset,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Revtron_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->revtron->getpar(Revtron_DryWet) != val)
                {
                    plug->revtron->changepar(Revtron_DryWet,val);
                }
            }
            break;

            // Offset
            case Revtron_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->revtron->getpar(Revtron_Pan) != val)
                {
                    plug->revtron->changepar(Revtron_Pan,val);
                }
            }
            break;

            // Skip parameter
            case Revtron_Length:
            case Revtron_Level:
            {
                val = (int)*plug->param_p[i];
                if(plug->revtron->getpar(param_case_offset) != val)
                {
                    plug->revtron->changepar(param_case_offset,val);
                }

                param_case_offset++;    // skips Revtron_User_File, Revtron_Set_File
            }
            break;
        }

        param_case_offset++;
    }

    // Set up forge to write directly to notify output port.
    const uint32_t notify_capacity = plug->atom_out_p->atom.size;
    lv2_atom_forge_set_buffer(&plug->forge, (uint8_t*)plug->atom_out_p, notify_capacity);

    // Start a sequence in the notify output port.
    lv2_atom_forge_sequence_head(&plug->forge, &plug->atom_frame, 0);

    //if we loaded a state, send the new file name to the host to display
    if(plug->file_changed)
    {
    	plug->file_changed = 0;
    	lv2_atom_forge_frame_time(&plug->forge, 0);
        LV2_Atom_Forge_Frame frame;
        lv2_atom_forge_object( &plug->forge, &frame, 0, plug->URIDs.patch_Set);

        lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_property);
        lv2_atom_forge_urid(&plug->forge, plug->URIDs.filetype_rvb);
        lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_value);
        lv2_atom_forge_path(&plug->forge, plug->revtron->File.Filename, strlen(plug->revtron->File.Filename)+1);

        lv2_atom_forge_pop(&plug->forge, &frame);
    }

    //see if there's a file
    LV2_ATOM_SEQUENCE_FOREACH( plug->atom_in_p, ev)
    {
        if (ev->body.type == plug->URIDs.atom_Object)
        {
            const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;
            if (obj->body.otype == plug->URIDs.patch_Set)
            {
                // Get the property the set message is setting
                const LV2_Atom* property = NULL;
                lv2_atom_object_get(obj, plug->URIDs.patch_property, &property, 0);
                if (property && property->type == plug->URIDs.atom_URID)
                {
                    const uint32_t key = ((const LV2_Atom_URID*)property)->body;
                    if (key == plug->URIDs.filetype_rvb)
                    {
                        // a new file! pass the atom to the worker thread to load it
                        plug->scheduler->schedule_work(plug->scheduler->handle, lv2_atom_total_size(&ev->body), &ev->body);
                    }//property is rvb file
                }//property is URID
            }
            else if (obj->body.otype == plug->URIDs.patch_Get)
            {
                // Received a get message, emit our state (probably to UI)
                lv2_atom_forge_frame_time(&plug->forge, ev->time.frames );//use current event's time
                LV2_Atom_Forge_Frame frame;
                lv2_atom_forge_object( &plug->forge, &frame, 0, plug->URIDs.patch_Set);

            	lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_property);
            	lv2_atom_forge_urid(&plug->forge, plug->URIDs.filetype_rvb);
            	lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_value);
            	lv2_atom_forge_path(&plug->forge, plug->revtron->File.Filename, strlen(plug->revtron->File.Filename)+1);

            	lv2_atom_forge_pop(&plug->forge, &frame);
            }
        }//atom is object
    }//each atom in sequence

    //now run
    plug->revtron->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->revtron->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->revtron->cleanup();
    }

    return;
}

static LV2_Worker_Status revwork(LV2_Handle handle, LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle rhandle, uint32_t /* size */, const void* data)
{
    RKRLV2* plug = (RKRLV2*)handle;
    LV2_Atom_Object* obj = (LV2_Atom_Object*)data;
    const LV2_Atom* file_path = NULL;

    //work was scheduled to load a new file
    lv2_atom_object_get(obj, plug->URIDs.patch_value, &file_path, 0);
    if (file_path && file_path->type == plug->URIDs.atom_Path)
    {
        // Load file.
        char* path = (char*)LV2_ATOM_BODY_CONST(file_path);

        plug->loading_file = 1;
        RKRP::strlcpy(plug->revtron->Filename, path, sizeof(plug->revtron->Filename));
        plug->revtron->setfile(USERFILE);

        respond(rhandle,0,0);
    }//got file
    else
        return LV2_WORKER_ERR_UNKNOWN;

    return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status revwork_response(LV2_Handle handle, uint32_t /* size */, const void* /* data */)
{
    RKRLV2* plug = (RKRLV2*)handle;

    plug->loading_file = 0;//clear flag for next file load
    return LV2_WORKER_SUCCESS;
}

static LV2_State_Status revsave(LV2_Handle handle, LV2_State_Store_Function  store, LV2_State_Handle state_handle,
		uint32_t /* flags */, const LV2_Feature* const* features)
{
    RKRLV2* plug = (RKRLV2*)handle;

    LV2_State_Map_Path* map_path = NULL;
    for (int i = 0; features[i]; ++i)
    {
        if (!strcmp(features[i]->URI, LV2_STATE__mapPath))
        {
            map_path = (LV2_State_Map_Path*)features[i]->data;
        }
    }

    char* abstractpath = map_path->abstract_path(map_path->handle, plug->revtron->File.Filename);

    store(state_handle, plug->URIDs.filetype_rvb, abstractpath, strlen(plug->revtron->File.Filename) + 1,
    		plug->URIDs.atom_Path, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);

    free(abstractpath);

    return LV2_STATE_SUCCESS;
}

static LV2_State_Status revrestore(LV2_Handle handle, LV2_State_Retrieve_Function retrieve,
		LV2_State_Handle state_handle, uint32_t /* flags */, const LV2_Feature* const* /* features */)
{
    RKRLV2* plug = (RKRLV2*)handle;

    size_t   size;
    uint32_t type;
    uint32_t valflags;

    const void* value = retrieve( state_handle, plug->URIDs.filetype_rvb, &size, &type, &valflags);

    if (value)
    {
            char* path = (char*)value;

            RKRP::strlcpy(plug->revtron->Filename, path, sizeof(plug->revtron->Filename));
            plug->revtron->setfile(USERFILE);

            plug->file_changed = 1;
    }

    return LV2_STATE_SUCCESS;
}

static const void* revtron_extension_data(const char* uri)
{
    static const LV2_Worker_Interface worker = { revwork, revwork_response, NULL };
    static const LV2_State_Interface state_iface = { revsave, revrestore };
    if (!strcmp(uri, LV2_STATE__interface))
    {
        return &state_iface;
    }
    else if (!strcmp(uri, LV2_WORKER__interface))
    {
        return &worker;
    }
    return NULL;
}

///// Echotron /////////
LV2_Handle init_echotronlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_ECHOTRON_PARAMETERS - 2); // -2 for skip user file and set file
    plug->effectindex = IECHOTRON;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);
    if(!plug->scheduler || !plug->urid_map)
    {
    	//a required feature was not supported by host
    	free(plug);
    	return 0;
    }
    lv2_atom_forge_init(&plug->forge, plug->urid_map);

    plug->echotron = new Echotron(sample_freq, plug->period_max);
    plug->echotron->changepar(Echotron_User_File,1);    // set to user selected files

    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_echotronlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }

    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->echotron->lv2_update_params(nframes);
    }

    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Echotron_LFO_Width:
            case Echotron_Tempo:
            case Echotron_Damp:
            case Echotron_LFO_Stereo:
            case Echotron_Feedback:
            case Echotron_Mod_Delay:
            case Echotron_Mod_Filter:
            case Echotron_LFO_Type:
            case Echotron_Filters:
            {
                val = (int)*plug->param_p[i];
                if(plug->echotron->getpar(param_case_offset) != val)
                {
                    plug->echotron->changepar(param_case_offset,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Echotron_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->echotron->getpar(Echotron_DryWet) != val)
                {
                    plug->echotron->changepar(Echotron_DryWet,val);
                }
            }
            break;

            // Offset
            case Echotron_Depth:
            case Echotron_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->echotron->getpar(param_case_offset) != val)
                {
                    plug->echotron->changepar(param_case_offset,val);
                }
            }
            break;

            // Adjust file length, Skip user file
            case Echotron_Taps:
            {
                /* 3 taps - has max of File.fLength.
                This works even though the file length is set after the taps with presets.
                Because on the next cycle, the file length will be adjusted after being set,
                and then the taps get adjusted based on the correct file length. */
                val = (int)*plug->param_p[i] > plug->echotron->get_file_length() ? plug->echotron->get_file_length(): (int)*plug->param_p[i];
                if(plug->echotron->getpar(Echotron_Taps) != val)
                {
                    plug->echotron->changepar(Echotron_Taps,val);
                }

                param_case_offset++;    // skip user file
            }
            break;

            // Offset & skip Set file
            case Echotron_LR_Cross:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->echotron->getpar(Echotron_LR_Cross) != val)
                {
                    plug->echotron->changepar(Echotron_LR_Cross,val);
                }

                param_case_offset++;    // skip set file
            }
            break;
        }

        param_case_offset++;
    }

    // Set up forge to write directly to notify output port.
    const uint32_t notify_capacity = plug->atom_out_p->atom.size;
    lv2_atom_forge_set_buffer(&plug->forge, (uint8_t*)plug->atom_out_p, notify_capacity);

    // Start a sequence in the notify output port.
    lv2_atom_forge_sequence_head(&plug->forge, &plug->atom_frame, 0);

    //if we loaded a state, send the new file name to the host to display
    if(plug->file_changed)
    {
    	plug->file_changed = 0;
    	lv2_atom_forge_frame_time(&plug->forge, 0);
        LV2_Atom_Forge_Frame frame;
        lv2_atom_forge_object( &plug->forge, &frame, 0, plug->URIDs.patch_Set);

        lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_property);
        lv2_atom_forge_urid(&plug->forge, plug->URIDs.filetype_dly);
        lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_value);
        lv2_atom_forge_path(&plug->forge, plug->echotron->get_file_name(), strlen(plug->echotron->get_file_name())+1);

        lv2_atom_forge_pop(&plug->forge, &frame);
    }

    //see if there's a file
    LV2_ATOM_SEQUENCE_FOREACH( plug->atom_in_p, ev)
    {
        if (ev->body.type == plug->URIDs.atom_Object)
        {
            const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;
            if (obj->body.otype == plug->URIDs.patch_Set)
            {
                // Get the property the set message is setting
                const LV2_Atom* property = NULL;
                lv2_atom_object_get(obj, plug->URIDs.patch_property, &property, 0);
                if (property && property->type == plug->URIDs.atom_URID)
                {
                    const uint32_t key = ((const LV2_Atom_URID*)property)->body;
                    if (key == plug->URIDs.filetype_dly)
                    {
                        // a new file! pass the atom to the worker thread to load it
                        plug->scheduler->schedule_work(plug->scheduler->handle, lv2_atom_total_size(&ev->body), &ev->body);
                    }//property is dly file
                }//property is URID
            }
            else if (obj->body.otype == plug->URIDs.patch_Get)
            {
                // Received a get message, emit our state (probably to UI)
                lv2_atom_forge_frame_time(&plug->forge, ev->time.frames );//use current event's time
                LV2_Atom_Forge_Frame frame;
                lv2_atom_forge_object( &plug->forge, &frame, 0, plug->URIDs.patch_Set);

            	lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_property);
            	lv2_atom_forge_urid(&plug->forge, plug->URIDs.filetype_dly);
            	lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_value);
            	lv2_atom_forge_path(&plug->forge, plug->echotron->get_file_name(), strlen(plug->echotron->get_file_name())+1);

            	lv2_atom_forge_pop(&plug->forge, &frame);
            }
        }//atom is object
    }//each atom in sequence

    //now run
    plug->echotron->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->echotron->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->echotron->cleanup();
    }

    return;
}

static LV2_Worker_Status echowork(LV2_Handle handle, LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle rhandle, uint32_t /* size */, const void* data)
{
    RKRLV2* plug = (RKRLV2*)handle;
    LV2_Atom_Object* obj = (LV2_Atom_Object*)data;
    const LV2_Atom* file_path = NULL;

    //work was scheduled to load a new file
    lv2_atom_object_get(obj, plug->URIDs.patch_value, &file_path, 0);
    if (file_path && file_path->type == plug->URIDs.atom_Path)
    {
        // Load file.
        char* path = (char*)LV2_ATOM_BODY_CONST(file_path);

        plug->loading_file = 1;
        RKRP::strlcpy(plug->echotron->Filename, path, sizeof(plug->echotron->Filename));
        plug->echotron->setfile(USERFILE);

        respond(rhandle,0,0);
    }//got file
    else
        return LV2_WORKER_ERR_UNKNOWN;

    return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status echowork_response(LV2_Handle handle, uint32_t /* size */, const void* /* data */)
{
    RKRLV2* plug = (RKRLV2*)handle;

    plug->loading_file = 0;//clear flag for next file load
    return LV2_WORKER_SUCCESS;
}

static LV2_State_Status echosave(LV2_Handle handle, LV2_State_Store_Function  store, LV2_State_Handle state_handle,
		uint32_t /* flags */, const LV2_Feature* const* features)
{
    RKRLV2* plug = (RKRLV2*)handle;

    LV2_State_Map_Path* map_path = NULL;
    for (int i = 0; features[i]; ++i)
    {
        if (!strcmp(features[i]->URI, LV2_STATE__mapPath))
        {
            map_path = (LV2_State_Map_Path*)features[i]->data;
        }
    }

    char* abstractpath = map_path->abstract_path(map_path->handle, plug->echotron->get_file_name());

    store(state_handle, plug->URIDs.filetype_dly, abstractpath, strlen(plug->echotron->get_file_name()) + 1,
    		plug->URIDs.atom_Path, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);

    free(abstractpath);

    return LV2_STATE_SUCCESS;
}

static LV2_State_Status echorestore(LV2_Handle handle, LV2_State_Retrieve_Function retrieve,
		LV2_State_Handle state_handle, uint32_t /* flags */, const LV2_Feature* const* /* features */)
{
    RKRLV2* plug = (RKRLV2*)handle;

    size_t   size;
    uint32_t type;
    uint32_t valflags;

    const void* value = retrieve( state_handle, plug->URIDs.filetype_dly, &size, &type, &valflags);

    if (value)
    {
        char* path = (char*)value;

        RKRP::strlcpy(plug->echotron->Filename, path, sizeof(plug->echotron->Filename));
        plug->echotron->setfile(USERFILE);

        plug->file_changed = 1;
    }

    return LV2_STATE_SUCCESS;
}

static const void* echotron_extension_data(const char* uri)
{
    static const LV2_Worker_Interface worker = { echowork, echowork_response, NULL };
    static const LV2_State_Interface state_iface = { echosave, echorestore };
    if (!strcmp(uri, LV2_STATE__interface))
    {
        return &state_iface;
    }
    else if (!strcmp(uri, LV2_WORKER__interface))
    {
        return &worker;
    }
    return NULL;
}


//////// stereo harmonizer no MIDI /////////
LV2_Handle init_sharmnomidlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_SHARM_PARAMETERS - 1);    // -1 for Sharm_MIDI
    plug->effectindex = ISHARM_NM;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    //magic numbers: shift qual 4, downsample 5, up qual 4, down qual 2,
    plug->sharm = new StereoHarm(4,5,4,2,sample_freq, plug->period_max);
    plug->noteID = new Recognize(.6,440.0, sample_freq, plug->period_max);//.6 is default trigger value
    plug->chordID = new RecChord();
    plug->noteID->reconota = -1;
    // set in :void RKRGUI::cb_RC_Opti_i(Fl_Choice* o, void*) and used by schmittFloat();
    plug->noteID->setlpf(5500); // default user option in rakarrack
    plug->noteID->sethpf(80); // default user option in rakarrack

    plug->comp = new Limiter(sample_freq, plug->period_max);
    plug->comp->setpreset(0); //Final Limiter
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

/*    plug->comp->changepar(1,-24);//threshold
    plug->comp->changepar(2,4);  //ratio
    plug->comp->changepar(3,-10);//output
    plug->comp->changepar(4,20); //attack
    plug->comp->changepar(5,50); //release
    plug->comp->changepar(6,1);  //a_out
    plug->comp->changepar(7,30); //knee*/

    return plug;
}

void run_sharmnomidlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }

    /* Return from bypass - reset */
    if(plug->prev_bypass)
    {
        plug->sharm->cleanup();
        plug->sharm->changepar(Sharm_L_Chroma,plug->sharm->getpar(Sharm_L_Chroma));
        plug->sharm->changepar(Sharm_R_Chroma,plug->sharm->getpar(Sharm_R_Chroma));
        plug->chordID->cc = 1;//mark chord has changed
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->sharm->lv2_update_params(nframes);
        plug->comp->lv2_update_params(nframes);
        plug->noteID->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val;
    int param_case_offset = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Sharm_L_Chroma:
            case Sharm_R_Chroma:
            case Sharm_LR_Cross:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(param_case_offset) != val)
                {
                    plug->sharm->changepar(param_case_offset,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Sharm_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->sharm->getpar(Sharm_DryWet) != val)
                {
                    plug->sharm->changepar(Sharm_DryWet,val);
                }
            }
            break;

            // Offset 64
            case Sharm_L_Gain:
            case Sharm_R_Gain:
            {
                val = (int)*plug->param_p[i] + 64;  // offset 64
                if(plug->sharm->getpar(param_case_offset) != val)
                {
                    plug->sharm->changepar(param_case_offset,val);
                }
            }
            break;

            // Offset 12
            case Sharm_L_Interval:
            case Sharm_R_Interval:
            {
                val = (int)*plug->param_p[i] + 12;  // offset 12
                if(plug->sharm->getpar(i) != val)
                {
                    plug->sharm->changepar(i,val);
                }
            }
            break;

            // SELECT
            case Sharm_Select:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(Sharm_Select) != val)
                {
                    plug->sharm->changepar(Sharm_Select,val);
                    plug->chordID->cleanup();
                    if(!val){
                        plug->sharm->changepar(Sharm_L_Chroma,plug->sharm->getpar(Sharm_L_Chroma));
                        plug->sharm->changepar(Sharm_R_Chroma,plug->sharm->getpar(Sharm_R_Chroma));
                    }

                    plug->chordID->cc = 1;  // mark chord has changed
                }
            }
            break;

            // Note/Chord & skip
            case Sharm_Note:
            case Sharm_Chord:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(param_case_offset) != val)
                {
                    plug->sharm->changepar(param_case_offset,val);
                    plug->chordID->cc = 1;  // mark chord has changed
                }

                // Skip one after Sharm_chord
                if(param_case_offset == Sharm_Chord)
                    param_case_offset++;    // skip Sharm_MIDI
            }
            break;
        }

        param_case_offset++;
    }

    if(have_signal(plug->output_l_p, plug->output_r_p, nframes))
    {
        if(plug->sharm->mira)
        {
            if(plug->sharm->PSELECT)
            {
                plug->noteID->schmittFloat(plug->output_l_p,plug->output_r_p);
                if((plug->noteID->reconota != -1) && (plug->noteID->reconota != plug->noteID->last))
                {
                    if(plug->noteID->afreq > 0.0)
                    {
                        plug->chordID->Vamos(1,plug->sharm->Pintervall - 12,plug->noteID->reconota);
                        plug->chordID->Vamos(2,plug->sharm->Pintervalr - 12,plug->noteID->reconota);
                        plug->sharm->r_ratiol = plug->chordID->r__ratio[1]; // pass the found ratio
                        plug->sharm->r_ratior = plug->chordID->r__ratio[2]; // pass the found ratio
                        plug->noteID->last = plug->noteID->reconota;
                    }
                }
            }
        }
    }

    if(plug->sharm->PSELECT)
    {
        if (plug->chordID->cc) 
        {   
            plug->chordID->cc = 0;
            plug->chordID->ctipo = plug->sharm->getpar(Sharm_Chord);    // set chord type
            plug->chordID->fundi = plug->sharm->getpar(Sharm_Note);     // set root note
            plug->chordID->Vamos(1,plug->sharm->Pintervall - 12,plug->noteID->reconota);
            plug->chordID->Vamos(2,plug->sharm->Pintervalr - 12,plug->noteID->reconota);
            plug->sharm->r_ratiol = plug->chordID->r__ratio[1];
            plug->sharm->r_ratior = plug->chordID->r__ratio[2];
        }
        plug->comp->out(plug->output_l_p,plug->output_r_p);
    }

    //now run
    plug->sharm->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->sharm->outvolume, nframes);

    xfade_check(plug,nframes);

    return;
}

///// MB comp /////////
LV2_Handle init_mbcomplv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_COMPBAND_PARAMETERS;
    plug->effectindex = IMBCOMP;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->mbcomp = new CompBand(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_mbcomplv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->mbcomp->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;

    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case CompBand_Low_Ratio:
            case CompBand_Mid_1_Ratio:
            case CompBand_Mid_2_Ratio:
            case CompBand_High_Ratio:
            case CompBand_Low_Thresh:
            case CompBand_Mid_1_Thresh:
            case CompBand_Mid_2_Thresh:
            case CompBand_High_Thresh:
            case CompBand_Cross_1:
            case CompBand_Cross_2:
            case CompBand_Cross_3:
            case CompBand_Gain:
            {
                val = (int)*plug->param_p[i];
                if(plug->mbcomp->getpar(i) != val)
                {
                    plug->mbcomp->changepar(i,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case CompBand_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->mbcomp->getpar(CompBand_DryWet) != val)
                {
                    plug->mbcomp->changepar(CompBand_DryWet,val);
                }
            }
            break;
        }
    }

    //now run
    plug->mbcomp->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->mbcomp->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->mbcomp->cleanup();
    }

    return;
}

///// OptTrem /////////
LV2_Handle init_otremlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_OPTICAL_PARAMETERS;
    plug->effectindex = IOPTTREM;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    
    plug->otrem = new Opticaltrem(sample_freq, plug->period_max);

    return plug;
}

void run_otremlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->otrem->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Optical_Depth:
            case Optical_LFO_Tempo:
            case Optical_LFO_Random:
            case Optical_LFO_Type:
            case Optical_LFO_Stereo:
            case Optical_Invert:
            {
                val = (int)*plug->param_p[i];
                if(plug->otrem->getpar(i) != val)
                {
                    plug->otrem->changepar(i,val);
                }
            }
            break;

            // Offset
            case Optical_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->otrem->getpar(Optical_Pan) != val)
                {
                    plug->otrem->changepar(Optical_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->otrem->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->otrem->cleanup();
    }

    return;
}

///// Vibe /////////
LV2_Handle init_vibelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_VIBE_PARAMETERS;
    plug->effectindex = IVIBE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);
    
    plug->vibe = new Vibe(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_vibelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->vibe->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Vibe_Width:
            case Vibe_LFO_Tempo:
            case Vibe_LFO_Random:
            case Vibe_LFO_Type:
            case Vibe_LFO_Stereo:
            case Vibe_Depth:
            case Vibe_LR_Cross:
            case Vibe_Stereo:
            {
                val = (int)*plug->param_p[i];
                if(plug->vibe->getpar(i) != val)
                {
                    plug->vibe->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Vibe_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->vibe->getpar(Vibe_DryWet) != val)
                {
                    plug->vibe->changepar(Vibe_DryWet,val);
                }
            }
            break;

            // Offset
            case Vibe_Pan:
            case Vibe_Feedback:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->vibe->getpar(i) != val)
                {
                    plug->vibe->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->vibe->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->vibe->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->vibe->cleanup();
    }

    return;
}

///// Infinity /////////
LV2_Handle init_inflv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_INFINITY_PARAMETERS;
    plug->effectindex = IINF;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->inf = new Infinity(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_inflv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->inf->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Infinity_Band_1:
            case Infinity_Band_2:
            case Infinity_Band_3:
            case Infinity_Band_4:
            case Infinity_Band_5:
            case Infinity_Band_6:
            case Infinity_Band_7:
            case Infinity_Band_8:
            case Infinity_Resonance:
            case Infinity_Start:
            case Infinity_End:
            case Infinity_Tempo:
            case Infinity_LR_Delay:
            case Infinity_Subdivision:
            case Infinity_AutoPan:
            case Infinity_Reverse:
            case Infinity_Stages:
            {
                val = (int)*plug->param_p[i];
                if(plug->inf->getpar(i) != val)
                {
                    plug->inf->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Infinity_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->inf->getpar(Infinity_DryWet) != val)
                {
                    plug->inf->changepar(Infinity_DryWet,val);
                }
            }
            break;
        }
    }

    //now run
    plug->inf->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->inf->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->inf->cleanup();
    }

    return;
}


///// Phaser /////////
LV2_Handle init_phaselv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_PHASER_PARAMETERS;
    plug->effectindex = IPHASE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // needed to set period_max

    plug->phase = new Phaser(sample_freq, plug->period_max);    // period max could be wrong at this point
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_phaselv2(LV2_Handle handle, uint32_t nframes)
{
/*  From lv2.h _LV2_Descriptor 
    The plugin must pay careful attention to the block size passed to run() since the block
    allocated may only just be large enough to contain the data, and is not guaranteed to
    remain constant between run() calls. Plugins MUST NOT crash when `sample_count` is 0.
  
    'sample count' is our nframes from rkrlv2.C which is PERIOD/fPERIOD for phaser.
 */
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->phase->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Phaser_LFO_Tempo:
            case Phaser_LFO_Random:
            case Phaser_LFO_Type:
            case Phaser_LFO_Stereo:
            case Phaser_Depth:
            case Phaser_Feedback:
            case Phaser_Stages:
            case Phaser_Subtract:
            case Phaser_Phase:
            {
                val = (int)*plug->param_p[i];
                if(plug->phase->getpar(i) != val)
                {
                    plug->phase->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Phaser_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->phase->getpar(Phaser_DryWet) != val)
                {
                    plug->phase->changepar(Phaser_DryWet,val);
                }
            }
            break;

            // Offset
            case Phaser_Pan:
            case Phaser_LR_Cross:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->phase->getpar(i) != val)
                {
                    plug->phase->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->phase->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->phase->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->phase->cleanup();
    }

    return;
}

///// NoiseGate /////////
LV2_Handle init_gatelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_GATE_PARAMETERS;
    plug->effectindex = IGATE;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->gate = new Gate(sample_freq, plug->period_max);

    return plug;
}

void run_gatelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->gate->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Gate_Threshold:
            case Gate_Range:
            case Gate_Attack:
            case Gate_Release:
            case Gate_LPF:
            case Gate_HPF:
            case Gate_Hold:
            {
                val = (int)*plug->param_p[i];
                if(plug->gate->getpar(i) != val)
                {
                    plug->gate->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->gate->out(plug->output_l_p,plug->output_r_p);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->gate->cleanup();
    }

    return;
}


///// MIDIConverter /////////
LV2_Handle init_midiclv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_MIDICONV_PARAMETERS;
    plug->effectindex = IMIDIC;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    if(!plug->urid_map)
    {
    //a required feature was not supported by host
    	free(plug);
    	return 0;
    }

    lv2_atom_forge_init(&plug->forge, plug->urid_map);

    plug->midic = new MIDIConverter(0, sample_freq, plug->period_max);

    return plug;
}

void run_midiclv2(LV2_Handle handle, uint32_t nframes)
{    
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    plug->midic->plug = plug;       // for MIDIConverter direct access to lv2 

    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->midic->lv2_update_params(nframes);
    }
    
    // we are good to run now
    
    const uint32_t out_capacity = plug->atom_out_p->atom.size;

    lv2_atom_forge_set_buffer(&plug->forge, (uint8_t*)plug->atom_out_p, out_capacity);
    lv2_atom_forge_sequence_head(&plug->forge, &plug->atom_frame, 0);


    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case MIDIConv_Gain:
            case MIDIConv_Trigger:
            case MIDIConv_Velocity:
            case MIDIConv_Midi:
            case MIDIConv_Octave:
            case MIDIConv_FFT:
            case MIDIConv_Panic:
            {
                val = (int)*plug->param_p[i];
                if(plug->midic->getpar(i) != val)
                {
                    plug->midic->changepar(i,val);
                }
            }
            break;
        }
    }

    //now run
    plug->midic->out(plug->output_l_p,plug->output_r_p);

    //and set NOTE meter
    *plug->param_p[MIDIC_NOTE_REGISTER] = plug->midic->hay;

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->midic->cleanup();
    }

    return;
}


///// Convolotron /////////
LV2_Handle init_convollv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    // -4 for: Convo_User_File, Convo_SKIP_5, Convo_Set_File, Convo_SKIP_9
    plug->nparams = (C_CONVO_PARAMETERS - 4);

    plug->effectindex = ICONVO;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);
    if(!plug->scheduler || !plug->urid_map)
    {
    	//a required feature was not supported by host
    	free(plug);
    	return 0;
    }
    lv2_atom_forge_init(&plug->forge, plug->urid_map);

    plug->convol = new Convolotron( /*downsample*/6, /*up interpolation method*/4, /*down interpolation method*/2 ,sample_freq, plug->period_max);
    plug->convol->changepar(Convo_User_File,1);//set to user selected files

    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_convollv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }

    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->convol->lv2_update_params(nframes);
    }

    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;

    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Convo_Safe:
            case Convo_Damp:
            case Convo_Feedback:
            {
                val = (int)*plug->param_p[i];
                if(plug->convol->getpar(param_case_offset) != val)
                {
                    plug->convol->changepar(param_case_offset,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Convo_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->convol->getpar(Convo_DryWet) != val)
                {
                    plug->convol->changepar(Convo_DryWet,val);
                }
            }
            break;

            // Offset
            case Convo_Pan:
            {
                val = (int)*plug->param_p[i] + 64;   //  offset
                if(plug->convol->getpar(Convo_Pan) != val)
                {
                    plug->convol->changepar(Convo_Pan,val);
                }
            }
            break;

            // Skip two after these
            case Convo_Length:
            case Convo_Level:
            {
                val = (int)*plug->param_p[i];
                if(plug->convol->getpar(param_case_offset) != val)
                {
                    plug->convol->changepar(param_case_offset,val);
                }
                // increment for two skipped parameters
                param_case_offset += 2;
            }
            break;
        }
        param_case_offset++;
    }

    // Set up forge to write directly to notify output port.
    const uint32_t notify_capacity = plug->atom_out_p->atom.size;
    lv2_atom_forge_set_buffer(&plug->forge, (uint8_t*)plug->atom_out_p, notify_capacity);

    // Start a sequence in the notify output port.
    lv2_atom_forge_sequence_head(&plug->forge, &plug->atom_frame, 0);

    //if we loaded a state, send the new file name to the host to display
    if(plug->file_changed)
    {
    	plug->file_changed = 0;
    	lv2_atom_forge_frame_time(&plug->forge, 0);
        LV2_Atom_Forge_Frame frame;
        lv2_atom_forge_object( &plug->forge, &frame, 0, plug->URIDs.patch_Set);

        lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_property);
        lv2_atom_forge_urid(&plug->forge, plug->URIDs.filetype_snd);
        lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_value);
        lv2_atom_forge_path(&plug->forge, plug->convol->Filename, strlen(plug->convol->Filename)+1);

        lv2_atom_forge_pop(&plug->forge, &frame);
    }

    //see if there's a file
    LV2_ATOM_SEQUENCE_FOREACH( plug->atom_in_p, ev)
    {
        if (ev->body.type == plug->URIDs.atom_Object)
        {
            const LV2_Atom_Object* obj = (const LV2_Atom_Object*)&ev->body;
            if (obj->body.otype == plug->URIDs.patch_Set)
            {
                // Get the property the set message is setting
                const LV2_Atom* property = NULL;
                lv2_atom_object_get(obj, plug->URIDs.patch_property, &property, 0);
                if (property && property->type == plug->URIDs.atom_URID)
                {
                    const uint32_t key = ((const LV2_Atom_URID*)property)->body;
                    if (key == plug->URIDs.filetype_snd)
                    {
                        // a new file! pass the atom to the worker thread to load it
                        plug->scheduler->schedule_work(plug->scheduler->handle, lv2_atom_total_size(&ev->body), &ev->body);
                    }//property is snd file
                }//property is URID
            }
            else if (obj->body.otype == plug->URIDs.patch_Get)
            {
                // Received a get message, emit our state (probably to UI)
                lv2_atom_forge_frame_time(&plug->forge, ev->time.frames );//use current event's time
                LV2_Atom_Forge_Frame frame;
                lv2_atom_forge_object( &plug->forge, &frame, 0, plug->URIDs.patch_Set);

            	lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_property);
            	lv2_atom_forge_urid(&plug->forge, plug->URIDs.filetype_snd);
            	lv2_atom_forge_key(&plug->forge, plug->URIDs.patch_value);
            	lv2_atom_forge_path(&plug->forge, plug->convol->Filename, strlen(plug->convol->Filename)+1);

            	lv2_atom_forge_pop(&plug->forge, &frame);
            }
        }//atom is object
    }//each atom in sequence

    //now run
    plug->convol->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->convol->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->convol->cleanup();
    }

    return;
}

static LV2_Worker_Status convwork(LV2_Handle handle, LV2_Worker_Respond_Function respond, LV2_Worker_Respond_Handle rhandle, uint32_t /* size */, const void* data)
{
    RKRLV2* plug = (RKRLV2*)handle;
    LV2_Atom_Object* obj = (LV2_Atom_Object*)data;
    const LV2_Atom* file_path = NULL;

    //work was scheduled to load a new file
    lv2_atom_object_get(obj, plug->URIDs.patch_value, &file_path, 0);
    if (file_path && file_path->type == plug->URIDs.atom_Path)
    {
        // Load file.
        char* path = (char*)LV2_ATOM_BODY_CONST(file_path);

        plug->loading_file = 1;
        RKRP::strlcpy(plug->convol->Filename, path, sizeof(plug->convol->Filename));
        plug->convol->setfile(USERFILE);
        respond(rhandle,0,0);
    }//got file
    else
        return LV2_WORKER_ERR_UNKNOWN;

    return LV2_WORKER_SUCCESS;
}

static LV2_Worker_Status convwork_response(LV2_Handle handle, uint32_t /* size */, const void* /* data */)
{
    RKRLV2* plug = (RKRLV2*)handle;
    plug->loading_file = 0;//clear flag for next file load
    return LV2_WORKER_SUCCESS;
}

static LV2_State_Status convsave(LV2_Handle handle, LV2_State_Store_Function  store, LV2_State_Handle state_handle,
		uint32_t /* flags */, const LV2_Feature* const* features)
{
    RKRLV2* plug = (RKRLV2*)handle;

    LV2_State_Map_Path* map_path = NULL;
    for (int i = 0; features[i]; ++i)
    {
        if (!strcmp(features[i]->URI, LV2_STATE__mapPath))
        {
            map_path = (LV2_State_Map_Path*)features[i]->data;
        }
    }

    char* abstractpath = map_path->abstract_path(map_path->handle, plug->convol->Filename);

    store(state_handle, plug->URIDs.filetype_snd, abstractpath, strlen(plug->convol->Filename) + 1,
    		plug->URIDs.atom_Path, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);

    free(abstractpath);

    return LV2_STATE_SUCCESS;
}

static LV2_State_Status convrestore(LV2_Handle handle, LV2_State_Retrieve_Function retrieve,
		LV2_State_Handle state_handle, uint32_t /* flags */, const LV2_Feature* const* /* features */)
{
    RKRLV2* plug = (RKRLV2*)handle;

    size_t   size;
    uint32_t type;
    uint32_t valflags;

    const void* value = retrieve( state_handle, plug->URIDs.filetype_snd, &size, &type, &valflags);

    if (value)
    {
        char* path = (char*)value;
        RKRP::strlcpy(plug->convol->Filename, path, sizeof(plug->convol->Filename));
        plug->convol->setfile(USERFILE);
        plug->file_changed = 1;
    }

    return LV2_STATE_SUCCESS;
}

static const void* convol_extension_data(const char* uri)
{
    static const LV2_Worker_Interface worker = { convwork, convwork_response, NULL };
    static const LV2_State_Interface state_iface = { convsave, convrestore };
    if (!strcmp(uri, LV2_STATE__interface))
    {
        return &state_iface;
    }
    else if (!strcmp(uri, LV2_WORKER__interface))
    {
        return &worker;
    }
    return NULL;
}

///// flanger /////////
LV2_Handle init_flangerlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_FLANGER_PARAMETERS - 1); // -1 skip Flanger_SKIP_Flange_10
    plug->effectindex = IFLANGE;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    
    plug->flanger = new Flanger(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_flangerlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->flanger->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Flanger_LFO_Tempo:
            case Flanger_LFO_Random:
            case Flanger_LFO_Type:
            case Flanger_LFO_Stereo:
            case Flanger_Depth:
            case Flanger_Delay:
            case Flanger_Feedback:
            case Flanger_Subtract:
            case Flanger_Intense:
            {
                val = (int)*plug->param_p[i];
                if(plug->flanger->getpar(param_case_offset) != val)
                {
                    plug->flanger->changepar(param_case_offset,val);
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Flanger_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->flanger->getpar(Flanger_DryWet) != val)
                {
                    plug->flanger->changepar(Flanger_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Flanger_Pan:
            {
                val = (int)*plug->param_p[i] + 64; // offset
                if(plug->flanger->getpar(Flanger_Pan) != val)
                {
                    plug->flanger->changepar(Flanger_Pan,val);
                }
            }
            break;
            
            // Skip after this one
            case Flanger_LR_Cross:
            {
                val = (int)*plug->param_p[i];
                if(plug->flanger->getpar(Flanger_LR_Cross) != val)
                {
                    plug->flanger->changepar(Flanger_LR_Cross,val);
                }
                // increment for skipped parameter
                param_case_offset++;
            }
            break;
        }
        param_case_offset++;
    }

    //now run
    plug->flanger->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->flanger->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->flanger->cleanup();
    }

    return;
}

///// overdrive /////////
LV2_Handle init_overdrivelv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = (C_OVERDRIVE_PARAMETERS - 2); // -2 for Skipped param 11 and Suboctave
    plug->effectindex = IOVERDRIVE;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    plug->overdrive = new Overdrive(/*oversampling*/2, /*up interpolation method*/4, /*down interpolation method*/2, sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_overdrivelv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->overdrive->lv2_update_params(nframes);
    }
    
    // we are good to run now

    //check and set changed parameters
    int val = 0;

    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Overdrive_LR_Cross:
            case Overdrive_Drive:
            case Overdrive_Level:
            case Overdrive_Type:
            case Overdrive_Negate:
            case Overdrive_LPF:
            case Overdrive_HPF:
            case Overdrive_Stereo:
            case Overdrive_Prefilter:
            {
                val = (int)*plug->param_p[i];
                if(plug->overdrive->getpar(i) != val)
                {
                    plug->overdrive->changepar(i,val);
                }
            }
            break;
            
            //Special cases
            // wet/dry -> dry/wet reversal
            case Overdrive_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->overdrive->getpar(Overdrive_DryWet) != val)
                {
                    plug->overdrive->changepar(Overdrive_DryWet,val);
                }
            }
            break;

            // Offset
            case Overdrive_Pan:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->overdrive->getpar(Overdrive_Pan) != val)
                {
                    plug->overdrive->changepar(Overdrive_Pan,val);
                }
            }
            break;
        }
    }

    //now run
    plug->overdrive->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->overdrive->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->overdrive->cleanup();
    }

    return;
}


//////// harmonizer  /////////
LV2_Handle init_harmonizerlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_HARM_PARAMETERS;
    plug->effectindex = IHARM;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    //magic numbers: shift qual 4, downsample 5, up qual 4, down qual 2,
    plug->harm = new Harmonizer(4,5,4,2, sample_freq, plug->period_max);
    plug->noteID = new Recognize(.6,440.0, sample_freq, plug->period_max);//.6 is default trigger value
    plug->chordID = new RecChord();
    plug->noteID->reconota = -1;

    // set in :void RKRGUI::cb_RC_Opti_i(Fl_Choice* o, void*) and used by schmittFloat();
    plug->noteID->setlpf(5500); // default user option in rakarrack
    plug->noteID->sethpf(80); // default user option in rakarrack

    plug->comp = new Limiter(sample_freq, plug->period_max);
    plug->comp->setpreset(0); //Final Limiter
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

/*
    plug->comp->changepar(1,-24);//threshold
    plug->comp->changepar(2,4);  //ratio
    plug->comp->changepar(3,-10);//output
    plug->comp->changepar(4,20); //attack
    plug->comp->changepar(5,50); //release
    plug->comp->changepar(6,1);  //a_out
    plug->comp->changepar(7,30); //knee
*/
    return plug;
}

void run_harmonizerlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    int bypass = 0;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }

    /* Return from bypass - reset */
    if(plug->prev_bypass)
    {
        plug->harm->cleanup();
        // update parameters after cleanup - interval
        plug->harm->changepar(Harm_Interval,plug->harm->getpar(Harm_Interval));
        //mark chord has changed to update parameters after cleanup
        plug->chordID->cc = 1;
        bypass = 1;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->harm->lv2_update_params(nframes);
        plug->comp->lv2_update_params(nframes);
        plug->noteID->lv2_update_params(nframes);
    }
    
    // Process incoming MIDI messages 
    if(plug->harm->getpar(Harm_MIDI))
    {
        // Get the capacity
        const uint32_t out_capacity = plug->atom_out_p->atom.size;
        
        // Write an empty Sequence header to the output
        lv2_atom_sequence_clear(plug->atom_out_p);
        plug->atom_out_p->atom.type = plug->atom_in_p->atom.type;
        
        LV2_ATOM_SEQUENCE_FOREACH(plug->atom_in_p , ev)
        {
            if (ev->body.type ==  plug->URIDs.midi_MidiEvent)
            {
                const uint8_t* const msg = (const uint8_t*)(ev + 1);
                switch (lv2_midi_message_type(msg))
                {
                    case LV2_MIDI_MSG_NOTE_ON:
                    {
                        int cmdnote = msg[1];
                        //int cmdvelo = msg[2]; // we don't need to check for zero velocity, LV2 will send as NOTE_OFF

                        for (int i = 0; i < POLY; i++)
                        {
                            if(plug->chordID->note_active[i] == 0)
                            {
                                plug->chordID->note_active[i] = 1;
                                plug->chordID->rnote[i] = cmdnote;
                                plug->chordID->gate[i] = 1;
                                plug->chordID->MiraChord();
                                plug->chordID->cc = 1; // mark chord has changed
                                break;
                            }
                        }
                        
                    break;
                    }
                    case LV2_MIDI_MSG_NOTE_OFF:
                    {
                        int cmdnote = msg[1];

                        for (int i = 0; i < POLY; i++)
                        {
                            if ((plug->chordID->note_active[i]) && (plug->chordID->rnote[i] == cmdnote))
                            {
                                plug->chordID->note_active[i] = 0;
                                plug->chordID->gate[i] = 0;
                                plug->chordID->cc = 1;  // mark chord has changed
                                break;
                            }
                        }

                    break;
                    }
                    default: break;
                }
                // Pass through all MIDI events directly to output
                lv2_atom_sequence_append_event( plug->atom_out_p, out_capacity, ev);
            }
        }
    }

    // we are good to run now

    //check and set changed parameters
    int val = 0;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Harm_Filter_Freq:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                }
            }
            break;
            
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Harm_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->harm->getpar(Harm_DryWet) != val)
                {
                    plug->harm->changepar(Harm_DryWet,val);
                }
            }
            break;


            // Offset 64
            case Harm_Pan:
            case Harm_Gain:
            case Harm_Filter_Gain:
            case Harm_Filter_Q:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                }
            }
            break;

            // Offset 12
            case Harm_Interval:
            {
                val = (int)*plug->param_p[i] + 12;  // offset
                if(plug->harm->getpar(Harm_Interval) != val)
                {
                    plug->harm->changepar(Harm_Interval,val);
                }
            }
            break;
            
            // Select Mode
            case Harm_Select:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                    plug->chordID->cleanup();
                    if(!val) plug->harm->changepar(Harm_Interval,plug->harm->getpar(Harm_Interval)); // Reset 

                    plug->chordID->cc = 1;  // mark chord changed to update after cleanup
                }
            }
            break;

            // Note and Chord
            case Harm_Note:
            case Harm_Chord:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                //    plug->chordID->ctipo = plug->harm->getpar(7);//set chord type
                //    plug->chordID->fundi = plug->harm->getpar(6);//set root note
                    plug->chordID->cc = 1;  // mark chord has changed
                }
            }
            break;

            // MIDI mode
            case Harm_MIDI:
            {
                val = (int)*plug->param_p[i];
                if(plug->harm->getpar(i) != val)
                {
                    plug->harm->changepar(i,val);
                    plug->chordID->cleanup();
                    plug->chordID->cc = 1;  // mark chord changed to update after cleanup
                }
            }
            break;
        }
    }

/*
see Chord() in rkr.fl
harmonizer, need recChord and recNote.
see process.C ln 1507
*/
    
    if(have_signal(plug->output_l_p, plug->output_r_p, nframes))
    {
        if(plug->harm->mira)
        {
            if(plug->harm->PSELECT || plug->harm->PMIDI)
            {
                plug->noteID->schmittFloat(plug->output_l_p,plug->output_r_p);
                if((plug->noteID->reconota != -1) && (plug->noteID->reconota != plug->noteID->last))
                {
                    if(plug->noteID->afreq > 0.0)
                    {
                        plug->chordID->Vamos(0,plug->harm->Pinterval - 12,plug->noteID->reconota);
                        plug->harm->r_ratio = plug->chordID->r__ratio[0];   // pass the found ratio
                        plug->noteID->last = plug->noteID->reconota;
                    }
                }
            }
        }
    }
    
    if(plug->harm->PSELECT || plug->harm->PMIDI)
    {
        if (plug->chordID->cc) 
        {
            plug->chordID->cc = 0;
            
            if(plug->harm->PSELECT || bypass)
            {
                bypass = 0;
                plug->chordID->ctipo = plug->harm->getpar(Harm_Chord);  // set chord type
                plug->chordID->fundi = plug->harm->getpar(Harm_Note);   // set root note
            }
            plug->chordID->Vamos(0,plug->harm->Pinterval - 12,plug->noteID->reconota);
            plug->harm->r_ratio = plug->chordID->r__ratio[0];           // pass the found ratio
        }
        plug->comp->out(plug->output_l_p,plug->output_r_p);
    }
    
    //now run
    plug->harm->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->harm->outvolume, nframes);

    xfade_check(plug,nframes);

    return;
}

//////// stereo harmonizer /////////
LV2_Handle init_stereoharmlv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_SHARM_PARAMETERS;
    plug->effectindex = ISTEREOHARM;
    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    //magic numbers: shift qual 4, downsample 5, up qual 4, down qual 2,
    plug->sharm = new StereoHarm(4,5,4,2,sample_freq, plug->period_max);
    plug->noteID = new Recognize(.6,440.0, sample_freq, plug->period_max);//.6 is default trigger value
    plug->chordID = new RecChord();
    plug->noteID->reconota = -1;
    // set in :void RKRGUI::cb_RC_Opti_i(Fl_Choice* o, void*) and used by schmittFloat();
    plug->noteID->setlpf(5500); // default user option in rakarrack
    plug->noteID->sethpf(80); // default user option in rakarrack

    plug->comp = new Limiter(sample_freq, plug->period_max);
    plug->comp->setpreset(0); //Final Limiter
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

/*    plug->comp->changepar(1,-24);//threshold
    plug->comp->changepar(2,4);  //ratio
    plug->comp->changepar(3,-10);//output
    plug->comp->changepar(4,20); //attack
    plug->comp->changepar(5,50); //release
    plug->comp->changepar(6,1);  //a_out
    plug->comp->changepar(7,30); //knee*/

    return plug;
}

void run_stereoharmlv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;
    
    int bypass = 0;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);

    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
    
    /* Return from bypass - reset */
    if(plug->prev_bypass)
    {
        plug->sharm->cleanup();
        plug->sharm->changepar(Sharm_L_Chroma,plug->sharm->getpar(Sharm_L_Chroma));
        plug->sharm->changepar(Sharm_R_Chroma,plug->sharm->getpar(Sharm_R_Chroma));
        plug->chordID->cc = 1;//mark chord has changed
        bypass = 1; // For MIDI mode upon return, need to reset default chord type and note
    }
    
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->sharm->lv2_update_params(nframes);
        plug->comp->lv2_update_params(nframes);
        plug->noteID->lv2_update_params(nframes);
    }

    // Process incoming MIDI messages 
    if(plug->sharm->getpar(Sharm_MIDI))
    {
        // Get the capacity
        const uint32_t out_capacity = plug->atom_out_p->atom.size;
        
        // Write an empty Sequence header to the output
        lv2_atom_sequence_clear(plug->atom_out_p);
        plug->atom_out_p->atom.type = plug->atom_in_p->atom.type;
        
        LV2_ATOM_SEQUENCE_FOREACH(plug->atom_in_p , ev)
        {
            if (ev->body.type ==  plug->URIDs.midi_MidiEvent)
            {
                const uint8_t* const msg = (const uint8_t*)(ev + 1);
                switch (lv2_midi_message_type(msg))
                {
                    case LV2_MIDI_MSG_NOTE_ON:
                    {
                        int cmdnote = msg[1];
                        //int cmdvelo = msg[2]; // we don't need to check for zero velocity, LV2 will send as NOTE_OFF

                        for (int i = 0; i < POLY; i++)
                        {
                            if(plug->chordID->note_active[i] == 0)
                            {
                                plug->chordID->note_active[i] = 1;
                                plug->chordID->rnote[i] = cmdnote;
                                plug->chordID->gate[i] = 1;
                                plug->chordID->MiraChord();
                                plug->chordID->cc = 1; // mark chord changed
                                break;
                            }
                        }
                        
                    break;
                    }
                    case LV2_MIDI_MSG_NOTE_OFF:
                    {
                        int cmdnote = msg[1];

                        for (int i = 0; i < POLY; i++)
                        {
                            if ((plug->chordID->note_active[i]) && (plug->chordID->rnote[i] == cmdnote))
                            {
                                plug->chordID->note_active[i] = 0;
                                plug->chordID->gate[i] = 0;
                                plug->chordID->cc = 1;  // mark chord changed
                                break;
                            }
                        }

                    break;
                    }
                    default: break;
                }
                // Pass through all MIDI events directly to output
                lv2_atom_sequence_append_event( plug->atom_out_p, out_capacity, ev);
            }
        }
    }

    // we are good to run now

    //check and set changed parameters
    int val;
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(i)
        {
            // Normal processing
            case Sharm_L_Chroma:
            case Sharm_R_Chroma:
            case Sharm_LR_Cross:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(i) != val)
                {
                    plug->sharm->changepar(i,val);
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Sharm_DryWet:
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->sharm->getpar(Sharm_DryWet) != val)
                {
                    plug->sharm->changepar(Sharm_DryWet,val);
                }
            }
            break;

            // Offset 64
            case Sharm_L_Gain:
            case Sharm_R_Gain:
            {
                val = (int)*plug->param_p[i] + 64;  // offset 64
                if(plug->sharm->getpar(i) != val)
                {
                    plug->sharm->changepar(i,val);
                }
            }
            break;

            // Offset 12
            case Sharm_L_Interval:
            case Sharm_R_Interval:
            {
                val = (int)*plug->param_p[i] + 12;  // offset 12
                if(plug->sharm->getpar(i) != val)
                {
                    plug->sharm->changepar(i,val);
                }
            }
            break;

            // SELECT
            case Sharm_Select:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(Sharm_Select) != val)
                {
                    plug->sharm->changepar(Sharm_Select,val);
                    plug->chordID->cleanup();
                    if(!val)
                    {
                        plug->sharm->changepar(Sharm_L_Chroma,plug->sharm->getpar(Sharm_L_Chroma));
                        plug->sharm->changepar(Sharm_R_Chroma,plug->sharm->getpar(Sharm_R_Chroma));
                    }

                    plug->chordID->cc = 1;  // mark chord changed
                }
            }
            break;

            // Note/Chord
            case Sharm_Note:
            case Sharm_Chord:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(i) != val)
                {
                    plug->sharm->changepar(i,val);
                    plug->chordID->cc = 1;  // mark chord changed
                }
            }
            break;

            // MIDI
            case Sharm_MIDI:
            {
                val = (int)*plug->param_p[i];
                if(plug->sharm->getpar(i) != val)
                {
                    plug->sharm->changepar(i,val);
                    plug->chordID->cleanup();
                    if(!val)
                    {
                        plug->sharm->changepar(Sharm_L_Chroma,plug->sharm->getpar(Sharm_L_Chroma));
                        plug->sharm->changepar(Sharm_R_Chroma,plug->sharm->getpar(Sharm_R_Chroma));
                    }
                    plug->chordID->cc = 1;  // mark chord changed to update after cleanup
                }
            }
            break;
        }
    }

    if(have_signal(plug->output_l_p, plug->output_r_p, nframes))
    {
        if(plug->sharm->mira)
        {
            if(plug->sharm->PSELECT || plug->sharm->PMIDI)
            {
                plug->noteID->schmittFloat(plug->output_l_p,plug->output_r_p);
                if((plug->noteID->reconota != -1) && (plug->noteID->reconota != plug->noteID->last))
                {
                    if(plug->noteID->afreq > 0.0)
                    {
                        plug->chordID->Vamos(1,plug->sharm->Pintervall - 12,plug->noteID->reconota);
                        plug->chordID->Vamos(2,plug->sharm->Pintervalr - 12,plug->noteID->reconota);
                        plug->sharm->r_ratiol = plug->chordID->r__ratio[1]; // pass the found ratio
                        plug->sharm->r_ratior = plug->chordID->r__ratio[2]; // pass the found ratio
                        plug->noteID->last = plug->noteID->reconota;
                    }
                }
            }
        }
    }

    if(plug->sharm->PSELECT || plug->sharm->PMIDI)
    {
        if (plug->chordID->cc) 
        {   
            plug->chordID->cc = 0;
            
            if(plug->sharm->PSELECT || bypass)
            {
                bypass = 0;
                plug->chordID->ctipo = plug->sharm->getpar(Sharm_Chord);    // set chord type
                plug->chordID->fundi = plug->sharm->getpar(Sharm_Note);     // set root note
            }
            plug->chordID->Vamos(1,plug->sharm->Pintervall - 12,plug->noteID->reconota);
            plug->chordID->Vamos(2,plug->sharm->Pintervalr - 12,plug->noteID->reconota);
            plug->sharm->r_ratiol = plug->chordID->r__ratio[1];
            plug->sharm->r_ratior = plug->chordID->r__ratio[2];
        }
        plug->comp->out(plug->output_l_p,plug->output_r_p);
    }

    //now run
    plug->sharm->out(plug->output_l_p,plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->sharm->outvolume, nframes);

    xfade_check(plug,nframes);

    return;
}

///// ResSolution /////////
LV2_Handle init_ressollv2(const LV2_Descriptor* /* descriptor */,double sample_freq, const char* /* bundle_path */,const LV2_Feature * const* host_features)
{
    RKRLV2* plug = (RKRLV2*)malloc(sizeof(RKRLV2));

    if(plug == NULL)
        return NULL;

    plug->nparams = C_RESSOL_PARAMETERS - 2; // -2 for LFO_Random and LFO_Type
    plug->effectindex = IRESSOLUTION;
    plug->prev_bypass = 1;
    
    getFeatures(plug,host_features);    // for period_max
    
    plug->ressol = new ResSolution(sample_freq, plug->period_max);
    
    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void run_ressollv2(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRLV2* plug = (RKRLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    check_shared_buf(plug,nframes);
    inline_check(plug, nframes);
    
    // are we bypassing
    if(*plug->bypass_p && plug->prev_bypass)
    {
        return;
    }
 
    /* adjust for possible variable nframes */
    if(plug->period_max != nframes)
    {
        plug->period_max = nframes;
        plug->ressol->lv2_update_params(nframes);
    }
    
    // we are good to run now
    
    //check and set changed parameters
    int val = 0;
    int param_case_offset = 0;
    
    for(int i = 0; i < plug->nparams; i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Ressol_Distortion:
            case Ressol_LFO_Stereo:
            case Ressol_Width:
            case Ressol_Stages:
            case Ressol_Mismatch:
            case Ressol_Subtract:
            case Ressol_Depth:
            case Ressol_Hyper:
            {
                val = (int)*plug->param_p[i];
                if(plug->ressol->getpar(param_case_offset) != val)
                {
                    plug->ressol->changepar(param_case_offset,val);
                }
            }
            break;
            
            // Special cases
            // skip 2 after this one
            case Ressol_LFO_Tempo:      // This is labeled 'Shift' for the parameter
            {
                val = (int)*plug->param_p[i];
                if(plug->ressol->getpar(param_case_offset) != val)
                {
                    plug->ressol->changepar(param_case_offset,val);
                }
                // increment for skipped parameters, LFO_Random and LFO_Type
                param_case_offset += 2;
            }
            break;
            
            // wet/dry -> dry/wet reversal
            case Ressol_DryWet: 
            {
                val = Dry_Wet((int)*plug->param_p[i]);
                if(plug->ressol->getpar(Ressol_DryWet) != val)
                {
                    plug->ressol->changepar(Ressol_DryWet,val);
                }
            }
            break;
            
            // Offset
            case Ressol_Feedback:
            {
                val = (int)*plug->param_p[i] + 64;  // offset
                if(plug->ressol->getpar(Ressol_Feedback) != val)
                {
                    plug->ressol->changepar(Ressol_Feedback,val);
                }
            }
            break;
        }
        param_case_offset++;
    }

    //now run
    plug->ressol->out(plug->output_l_p, plug->output_r_p);

    //and for whatever reason we have to do the wet/dry mix ourselves
    wetdry_mix(plug, plug->ressol->outvolume, nframes);

    xfade_check(plug,nframes);

    if(plug->prev_bypass)
    {
        plug->ressol->cleanup();
    }

    return;
}

/////////////////////////////////
//       END OF FX
/////////////////////////////////



void cleanup_rkrlv2(LV2_Handle handle)
{
    RKRLV2* plug = (RKRLV2*)handle;
    switch(plug->effectindex)
    {
    case IEQ:
        delete plug->eq;
        break;
    case IEQP:
        delete plug->peq;
        break;
    case ICOMP:
        delete plug->comp;
        break;
    case IDIST:
        delete plug->dist;
        break;
    case IECHO:
        delete plug->echo;
        break;
    case ICHORUS:
        delete plug->chorus;
        break;
    case IAPHASE:
        delete plug->aphase;
        break;
    case IHARM:
    case IHARM_NM:
        delete plug->harm;
        delete plug->noteID;
        delete plug->chordID;
        delete plug->comp;
        break;
    case IEXCITER:
        delete plug->exciter;
        break;
    case IPAN:
        delete plug->pan;
        break;
    case IAWAH:
        delete plug->alien;
        break;
    case IREV:
        delete plug->reve;
        break;
    case ICAB:
        delete plug->cab;
        break;
    case IMDEL:
        delete plug->mdel;
        break;
    case IWAH:
        delete plug->wah;
        break;
    case IDERE:
        delete plug->dere;
        break;
    case IVALVE:
        delete plug->valve;
        break;
    case IDFLANGE:
        delete plug->dflange;
        break;
    case IRING:
        delete plug->noteID;
        delete plug->ring;
        break;
    case IDISTBAND:
        delete plug->distband;
        break;
    case IARPIE:
        delete plug->arp;
        break;
    case IEXPAND:
        delete plug->expand;
        break;
    case ISHUFF:
        delete plug->shuf;
        break;
    case ISYNTH:
        delete plug->synth;
        break;
    case IVBAND:
        delete plug->varyband;
        break;
    case IMUTRO:
        delete plug->mutro;
        break;
    case IECHOVERSE:
        delete plug->echoverse;
        break;
    case ICOIL:
        delete plug->coil;
        break;
    case ISHELF:
        delete plug->shelf;
        break;
    case IVOC:
        delete plug->voc;
        break;
    case ISUS:
        delete plug->sus;
        break;
    case ISEQ:
        delete plug->seq;
        break;
    case ISHIFT:
        delete plug->shift;
        break;
    case ISTOMP:
        delete plug->stomp;
        break;
    case IREVTRON:
        delete plug->revtron;
        break;
    case IECHOTRON:
        delete plug->echotron;
        break;
    case ISTEREOHARM:
    case ISHARM_NM:
    	delete plug->sharm;
        delete plug->noteID;
        delete plug->chordID;
        delete plug->comp;
    	break;
    case IMBCOMP:
    	delete plug->mbcomp;
    	break;
    case IOPTTREM:
    	delete plug->otrem;
    	break;
    case IVIBE:
    	delete plug->vibe;
    	break;
    case IINF:
    	delete plug->inf;
    	break;
    case IPHASE:
        delete plug->phase;
        break;
    case IGATE:
        delete plug->gate;
        break;
    case IMIDIC:
        delete plug->midic;
        break;
    case ICONVO:
        delete plug->convol;
     //   delete plug->wavfile;
        break;
    case IFLANGE:
        delete plug->flanger;
        break;
    case IOVERDRIVE:
        delete plug->overdrive;
        break;
    case IRESSOLUTION:
        delete plug->ressol;
        break;
    }
    free(plug);
}

void connect_rkrlv2_ports_w_atom(LV2_Handle handle, uint32_t port, void *data)
{
    RKRLV2* plug = (RKRLV2*)handle;
    switch(port)
    {
    case INL:
        plug->input_l_p = (float*)data;
        break;
    case INR:
        plug->input_r_p = (float*)data;
        break;
    case OUTL:
        plug->output_l_p = (float*)data;
        break;
    case OUTR:
        plug->output_r_p = (float*)data;
        break;
    case BYPASS:
        plug->atom_in_p = (const LV2_Atom_Sequence*)data;
        break;
    case PARAM0:
        plug->atom_out_p = (LV2_Atom_Sequence*)data;
        break;
    case PARAM1:
        plug->bypass_p = (float*)data;
        break;
    case PARAM2:
        plug->param_p[0] = (float*)data;
        break;
    case PARAM3:
        plug->param_p[1] = (float*)data;
        break;
    case PARAM4:
        plug->param_p[2] = (float*)data;
        break;
    case PARAM5:
        plug->param_p[3] = (float*)data;
        break;
    case PARAM6:
        plug->param_p[4] = (float*)data;
        break;
    case PARAM7:
        plug->param_p[5] = (float*)data;
        break;
    case PARAM8:
        plug->param_p[6] = (float*)data;
        break;
    case PARAM9:
        plug->param_p[7] = (float*)data;
        break;
    case PARAM10:
        plug->param_p[8] = (float*)data;
        break;
    case PARAM11:
        plug->param_p[9] = (float*)data;
        break;
    case PARAM12:
        plug->param_p[10] = (float*)data;
        break;
    case PARAM13:
        plug->param_p[11] = (float*)data;
        break;
    case PARAM14:
        plug->param_p[12] = (float*)data;
        break;
    case PARAM15:
        plug->param_p[13] = (float*)data;
        break;
    case PARAM16:
        plug->param_p[14] = (float*)data;
        break;
    case PARAM17:
        plug->param_p[15] = (float*)data;
        break;
    case PARAM18:
        plug->param_p[16] = (float*)data;
        break;
    case DBG:
        plug->param_p[17] = (float*)data;
        break;
    case EXTRA:
        plug->param_p[18] = (float*)data;
        break;
    default:
        puts("UNKNOWN PORT YO!!");
    }
}

void connect_rkrlv2_ports(LV2_Handle handle, uint32_t port, void *data)
{
    RKRLV2* plug = (RKRLV2*)handle;
    switch(port)
    {
    case INL:
        plug->input_l_p = (float*)data;
        break;
    case INR:
        plug->input_r_p = (float*)data;
        break;
    case OUTL:
        plug->output_l_p = (float*)data;
        break;
    case OUTR:
        plug->output_r_p = (float*)data;
        break;
    case BYPASS:
        plug->bypass_p = (float*)data;
        break;
    case PARAM0:
        plug->param_p[0] = (float*)data;
        break;
    case PARAM1:
        plug->param_p[1] = (float*)data;
        break;
    case PARAM2:
        plug->param_p[2] = (float*)data;
        break;
    case PARAM3:
        plug->param_p[3] = (float*)data;
        break;
    case PARAM4:
        plug->param_p[4] = (float*)data;
        break;
    case PARAM5:
        plug->param_p[5] = (float*)data;
        break;
    case PARAM6:
        plug->param_p[6] = (float*)data;
        break;
    case PARAM7:
        plug->param_p[7] = (float*)data;
        break;
    case PARAM8:
        plug->param_p[8] = (float*)data;
        break;
    case PARAM9:
        plug->param_p[9] = (float*)data;
        break;
    case PARAM10:
        plug->param_p[10] = (float*)data;
        break;
    case PARAM11:
        plug->param_p[11] = (float*)data;
        break;
    case PARAM12:
        plug->param_p[12] = (float*)data;
        break;
    case PARAM13:
        plug->param_p[13] = (float*)data;
        break;
    case PARAM14:
        plug->param_p[14] = (float*)data;
        break;
    case PARAM15:
        plug->param_p[15] = (float*)data;
        break;
    case PARAM16:
        plug->param_p[16] = (float*)data;
        break;
    case PARAM17:
        plug->param_p[17] = (float*)data;
        break;
    case PARAM18:
        plug->param_p[18] = (float*)data;
        break;
    case DBG:
        plug->dbg_p = (float*)data;
        break;
    default:
        puts("UNKNOWN PORT YO!!");
    }
}


/////////////////////////////////
//    Plugin Descriptors
////////////////////////////////

static const LV2_Descriptor eqlv2_descriptor=
{
    EQLV2_URI,
    init_eqlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_eqlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor complv2_descriptor=
{
    COMPLV2_URI,
    init_complv2,
    connect_rkrlv2_ports,
    0,//activate
    run_complv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor distlv2_descriptor=
{
    DISTLV2_URI,
    init_distlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_distlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor echolv2_descriptor=
{
    ECHOLV2_URI,
    init_echolv2,
    connect_rkrlv2_ports,
    0,//activate
    run_echolv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor choruslv2_descriptor=
{
    CHORUSLV2_URI,
    init_choruslv2,
    connect_rkrlv2_ports,
    0,//activate
    run_choruslv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor aphaselv2_descriptor=
{
    APHASELV2_URI,
    init_aphaselv2,
    connect_rkrlv2_ports,
    0,//activate
    run_aphaselv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor harmnomidlv2_descriptor=
{
    HARMNOMIDLV2_URI,
    init_harmnomidlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_harmnomidlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor exciterlv2_descriptor=
{
    EXCITERLV2_URI,
    init_exciterlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_exciterlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor panlv2_descriptor=
{
    PANLV2_URI,
    init_panlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_panlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor alienlv2_descriptor=
{
    ALIENLV2_URI,
    init_alienlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_alienlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor revelv2_descriptor=
{
    REVELV2_URI,
    init_revelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_revelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor eqplv2_descriptor=
{
    EQPLV2_URI,
    init_eqplv2,
    connect_rkrlv2_ports,
    0,//activate
    run_eqplv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor cablv2_descriptor=
{
    CABLV2_URI,
    init_cablv2,
    connect_rkrlv2_ports,
    0,//activate
    run_cablv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor mdellv2_descriptor=
{
    MDELLV2_URI,
    init_mdellv2,
    connect_rkrlv2_ports,
    0,//activate
    run_mdellv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor wahlv2_descriptor=
{
    WAHLV2_URI,
    init_wahlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_wahlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor derelv2_descriptor=
{
    DERELV2_URI,
    init_derelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_derelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor valvelv2_descriptor=
{
    VALVELV2_URI,
    init_valvelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_valvelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor dflangelv2_descriptor=
{
    DFLANGELV2_URI,
    init_dflangelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_dflangelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor ringlv2_descriptor=
{
    RINGLV2_URI,
    init_ringlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_ringlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor distbandlv2_descriptor=
{
    DISTBANDLV2_URI,
    init_distbandlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_distbandlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor arplv2_descriptor=
{
    ARPIELV2_URI,
    init_arplv2,
    connect_rkrlv2_ports,
    0,//activate
    run_arplv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor expandlv2_descriptor=
{
    EXPANDLV2_URI,
    init_expandlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_expandlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor shuflv2_descriptor=
{
    SHUFFLELV2_URI,
    init_shuflv2,
    connect_rkrlv2_ports,
    0,//activate
    run_shuflv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor synthlv2_descriptor=
{
    SYNTHLV2_URI,
    init_synthlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_synthlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor varybandlv2_descriptor=
{
    VARYBANDLV2_URI,
    init_varybandlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_varybandlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor mutrolv2_descriptor=
{
    MUTROLV2_URI,
    init_mutrolv2,
    connect_rkrlv2_ports,
    0,//activate
    run_mutrolv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor echoverselv2_descriptor=
{
    ECHOVERSELV2_URI,
    init_echoverselv2,
    connect_rkrlv2_ports,
    0,//activate
    run_echoverselv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor coillv2_descriptor=
{
    COILLV2_URI,
    init_coillv2,
    connect_rkrlv2_ports,
    0,//activate
    run_coillv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor shelflv2_descriptor=
{
    SHELFLV2_URI,
    init_shelflv2,
    connect_rkrlv2_ports,
    0,//activate
    run_shelflv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor voclv2_descriptor=
{
    VOCODERLV2_URI,
    init_voclv2,
    connect_rkrlv2_ports,
    0,//activate
    run_voclv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor suslv2_descriptor=
{
    SUSTAINLV2_URI,
    init_suslv2,
    connect_rkrlv2_ports,
    0,//activate
    run_suslv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor seqlv2_descriptor=
{
    SEQUENCELV2_URI,
    init_seqlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_seqlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor shiftlv2_descriptor=
{
    SHIFTERLV2_URI,
    init_shiftlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_shiftlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor stomplv2_descriptor=
{
    STOMPLV2_URI,
    init_stomplv2,
    connect_rkrlv2_ports,
    0,//activate
    run_stomplv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor revtronlv2_descriptor=
{
    REVTRONLV2_URI,
    init_revtronlv2,
    connect_rkrlv2_ports_w_atom,
    0,//activate
    run_revtronlv2,
    0,//deactivate
    cleanup_rkrlv2,
    revtron_extension_data
};

static const LV2_Descriptor echotronlv2_descriptor=
{
    ECHOTRONLV2_URI,
    init_echotronlv2,
    connect_rkrlv2_ports_w_atom,
    0,//activate
    run_echotronlv2,
    0,//deactivate
    cleanup_rkrlv2,
    echotron_extension_data
};

static const LV2_Descriptor sharmnomidlv2_descriptor=
{
    SHARMNOMIDLV2_URI,
    init_sharmnomidlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_sharmnomidlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor mbcomplv2_descriptor=
{
    MBCOMPLV2_URI,
    init_mbcomplv2,
    connect_rkrlv2_ports,
    0,//activate
    run_mbcomplv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor otremlv2_descriptor=
{
    OPTTREMLV2_URI,
    init_otremlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_otremlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor vibelv2_descriptor=
{
    VIBELV2_URI,
    init_vibelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_vibelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor inflv2_descriptor=
{
    INFLV2_URI,
    init_inflv2,
    connect_rkrlv2_ports,
    0,//activate
    run_inflv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor phaselv2_descriptor=
{
    PHASELV2_URI,
    init_phaselv2,
    connect_rkrlv2_ports,
    0,//activate
    run_phaselv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor gatelv2_descriptor=
{
    GATELV2_URI,
    init_gatelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_gatelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor midiclv2_descriptor=
{
    MIDICLV2_URI,
    init_midiclv2,
    connect_rkrlv2_ports_w_atom,
    0,//activate
    run_midiclv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor convollv2_descriptor=
{
    CONVOLOTRONLV2_URI,
    init_convollv2,
    connect_rkrlv2_ports_w_atom,
    0,//activate
    run_convollv2,
    0,//deactivate
    cleanup_rkrlv2,
    convol_extension_data
};

static const LV2_Descriptor flangerlv2_descriptor=
{
    FLANGELV2_URI,
    init_flangerlv2,
    connect_rkrlv2_ports,
    0,//activate
    run_flangerlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor overdrivelv2_descriptor=
{
    OVERDRIVELV2_URI,
    init_overdrivelv2,
    connect_rkrlv2_ports,
    0,//activate
    run_overdrivelv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor harmonizerlv2_descriptor=
{
    HARMLV2_URI,
    init_harmonizerlv2,
    connect_rkrlv2_ports_w_atom,
    0,//activate
    run_harmonizerlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor stereoharmlv2_descriptor=
{
    STEROHARMLV2_URI,
    init_stereoharmlv2,
    connect_rkrlv2_ports_w_atom,
    0,//activate
    run_stereoharmlv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

static const LV2_Descriptor ressollv2_descriptor=
{
    RESSOLLV2_URI,
    init_ressollv2,
    connect_rkrlv2_ports,
    0,//activate
    run_ressollv2,
    0,//deactivate
    cleanup_rkrlv2,
    0//extension
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    switch (index)
    {
    case IEQ:
        return &eqlv2_descriptor ;
    case ICOMP:
        return &complv2_descriptor ;
    case IDIST:
        return &distlv2_descriptor ;
    case IECHO:
        return &echolv2_descriptor ;
    case ICHORUS:
        return &choruslv2_descriptor ;
    case IAPHASE:
        return &aphaselv2_descriptor ;
    case IHARM_NM:
        return &harmnomidlv2_descriptor ;
    case IEXCITER:
        return &exciterlv2_descriptor ;
    case IPAN:
        return &panlv2_descriptor ;
    case IAWAH:
        return &alienlv2_descriptor ;
    case IREV:
        return &revelv2_descriptor ;
    case IEQP:
        return &eqplv2_descriptor ;
    case ICAB:
        return &cablv2_descriptor ;
    case IMDEL:
        return &mdellv2_descriptor ;
    case IWAH:
        return &wahlv2_descriptor ;
    case IDERE:
        return &derelv2_descriptor ;
    case IVALVE:
        return &valvelv2_descriptor ;
    case IDFLANGE:
        return &dflangelv2_descriptor ;
    case IRING:
        return &ringlv2_descriptor ;
    case IDISTBAND:
        return &distbandlv2_descriptor ;
    case IARPIE:
        return &arplv2_descriptor ;
    case IEXPAND:
        return &expandlv2_descriptor ;
    case ISHUFF:
        return &shuflv2_descriptor ;
    case ISYNTH:
        return &synthlv2_descriptor ;
    case IVBAND:
        return &varybandlv2_descriptor ;
    case IMUTRO:
        return &mutrolv2_descriptor ;
    case IECHOVERSE:
        return &echoverselv2_descriptor ;
    case ICOIL:
        return &coillv2_descriptor ;
    case ISHELF:
        return &shelflv2_descriptor ;
    case IVOC:
        return &voclv2_descriptor ;
    case ISUS:
        return &suslv2_descriptor ;
    case ISEQ:
        return &seqlv2_descriptor ;
    case ISHIFT:
        return &shiftlv2_descriptor ;
    case ISTOMP:
        return &stomplv2_descriptor ;
    case IREVTRON:
        return &revtronlv2_descriptor ;
    case IECHOTRON:
        return &echotronlv2_descriptor ;
    case ISHARM_NM:
    	return &sharmnomidlv2_descriptor ;
    case IMBCOMP:
    	return &mbcomplv2_descriptor ;
    case IOPTTREM:
    	return &otremlv2_descriptor ;
    case IVIBE:
    	return &vibelv2_descriptor ;
    case IINF:
    	return &inflv2_descriptor ;
    case IPHASE:
        return &phaselv2_descriptor ;
    case IGATE:
        return &gatelv2_descriptor ;
    case IMIDIC:
        return &midiclv2_descriptor ;
    case ICONVO:
        return &convollv2_descriptor ;
    case IFLANGE:
        return &flangerlv2_descriptor ;
    case IOVERDRIVE:
        return &overdrivelv2_descriptor ;
    case IHARM:
        return &harmonizerlv2_descriptor ;
    case ISTEREOHARM:
    	return &stereoharmlv2_descriptor ;
    case IRESSOLUTION:
        return &ressollv2_descriptor ;
    default:
        return 0;
    }
}
