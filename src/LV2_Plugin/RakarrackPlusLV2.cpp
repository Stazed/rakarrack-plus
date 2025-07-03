/*
  rakarrack-plus - a guitar effects software

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include "RakarrackPlusLV2.h"
#include "RakarrackPlusLV2UI.h"


// externs
int global_gui_show = 0;
std::string nsm_preferences_file = "";

//this is the default hopefully hosts don't use periods of more than this, or they will communicate the max bufsize
#define INTERMEDIATE_BUFSIZE 1024
#define RAKARRACK_PLUS_STATE_URI "https://github.com/Stazed/rakarrack-plus#state"


void getFeatures(RKRPLUSLV2* plug, const LV2_Feature * const* host_features)
{
    uint8_t i,j;
    uint32_t nomBufSize = 0;
    plug->nparams = 0;
    plug->effectindex = 0;
    plug->period_max = INTERMEDIATE_BUFSIZE;
    plug->period_default = INTERMEDIATE_BUFSIZE;
    plug->tmp_l = 0;
    plug->tmp_r = 0;
    plug->input_l_p = 0;
    plug->input_r_p = 0;
    plug->output_l_p = 0;
    plug->output_r_p = 0;
    plug->urid_map = 0;
    plug->URIDs.midi_MidiEvent = 0;
    plug->URIDs.atom_Float = 0;
    plug->URIDs.atom_Int = 0;
    plug->URIDs.atom_long = 0;
    plug->URIDs.atom_string_id = 0;
    plug->URIDs.atom_Object = 0;
    plug->URIDs.atom_URID = 0;
    plug->URIDs.bufsz_max = 0;
    plug->URIDs.bufsz_min = 0;
    plug->URIDs.bufsz_nom = 0;
    plug->URIDs.rkrplus_state_id = 0;
    plug->URIDs.atom_blank = 0;
    plug->URIDs.atom_position = 0;
    plug->URIDs.atom_bpb = 0;
    plug->URIDs.atom_bar = 0;
    plug->URIDs.atom_bar_beat = 0;
    plug->URIDs.atom_bpm = 0;
    plug->URIDs.atom_beatUnit = 0;

    for(i=0; host_features[i]; i++)
    {
        if(!strcmp(host_features[i]->URI,LV2_OPTIONS__options))
        {
            LV2_Options_Option* option;
            option = (LV2_Options_Option*)host_features[i]->data;
            for(j=0; option[j].key; j++)
            {
                if(option[j].key == plug->URIDs.bufsz_max || option[j].key == plug->URIDs.bufsz_min)
                {
                    if(option[j].type == plug->URIDs.atom_Int)
                    {
                        uint32_t bufSz = *(const int*)option[j].value;
                        if(plug->period_max < bufSz)
                            plug->period_max = bufSz;

                    }
                    //other types?
                }
                if(option[j].key == plug->URIDs.bufsz_nom && option[j].type == plug->URIDs.atom_Int)
                {
                    nomBufSize =  *(const uint32_t*)option[j].value;
                }
            }
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
                plug->URIDs.atom_URID = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__URID);
                plug->URIDs.bufsz_max = plug->urid_map->map(plug->urid_map->handle,LV2_BUF_SIZE__maxBlockLength);
                plug->URIDs.bufsz_min = plug->urid_map->map(plug->urid_map->handle,LV2_BUF_SIZE__minBlockLength);
                plug->URIDs.bufsz_nom = plug->urid_map->map(plug->urid_map->handle,LV2_BUF_SIZE__nominalBlockLength);
                plug->URIDs.rkrplus_state_id = plug->urid_map->map(plug->urid_map->handle,RAKARRACK_PLUS_STATE_URI);
                plug->URIDs.atom_string_id = plug->urid_map->map(plug->urid_map->handle,LV2_ATOM__String);
                plug->URIDs.atom_position = plug->urid_map->map(plug->urid_map->handle, LV2_TIME__Position);
                plug->URIDs.atom_blank = plug->urid_map->map(plug->urid_map->handle, LV2_ATOM__Blank);
                plug->URIDs.atom_bpb = plug->urid_map->map(plug->urid_map->handle, LV2_TIME__beatsPerBar);
                plug->URIDs.atom_bar = plug->urid_map->map(plug->urid_map->handle, LV2_TIME__bar);
                plug->URIDs.atom_bar_beat = plug->urid_map->map(plug->urid_map->handle, LV2_TIME__barBeat);
                plug->URIDs.atom_bpm = plug->urid_map->map(plug->urid_map->handle, LV2_TIME__beatsPerMinute);
                plug->URIDs.atom_long = plug->urid_map->map(plug->urid_map->handle, LV2_ATOM__Long);
                plug->URIDs.atom_beatUnit = plug->urid_map->map(plug->urid_map->handle, LV2_TIME__beatUnit);
            }
        }
    }

    if (nomBufSize > 0)
        plug->period_max = nomBufSize;
    else if (plug->period_max == 0)
        plug->period_max = INTERMEDIATE_BUFSIZE;

    plug->period_default = plug->period_max;

}

//checks if input and output buffers are shared and copies it in a temp buffer so wet/dry works
static void
check_shared_buf(RKRPLUSLV2* plug, uint32_t nframes)
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
inline_check(RKRPLUSLV2* plug, uint32_t nframes)
{
    // copy only if needed. memcpy() src/dest memory areas must not overlap.
    if (plug->output_l_p != plug->input_l_p) {
        memcpy(plug->output_l_p,plug->input_l_p,sizeof(float)*nframes);
    }
    if (plug->output_r_p != plug->input_r_p) {
        memcpy(plug->output_r_p,plug->input_r_p,sizeof(float)*nframes);
    }
}

/**
 * Add a midi message to the output port.
 * This is used by MIDIConverter
 */
void
forge_midimessage(_RKRPLUSLV2* plug,
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

///// Rakarrack-plus /////////
LV2_Handle init_rkrplus(const LV2_Descriptor */*descriptor*/,
    double sample_freq,
    const char */*bundle_path*/,
    const LV2_Feature * const* host_features)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)malloc(sizeof(RKRPLUSLV2));
    
//    plug->nparams = 2;
    plug->effectindex = IRKRPLUS;
//    plug->prev_bypass = 1;

    getFeatures(plug,host_features);

    if(!plug->urid_map)
    {
    //a required feature was not supported by host
    	free(plug);
    	return 0;
    }

    lv2_atom_forge_init(&plug->forge, plug->urid_map);

    plug->rkrplus = new RKR(sample_freq, plug->period_max, true);

    // This pref-name is used by the FLTK preferences for configure lookup
    // specifically for LV2, so we don't clobber the standalone configure.
    std::string pref_name(PACKAGE);
    pref_name += "-LV2";
    plug->rkrplus->set_client_name(pref_name);
    plug->rkrplus->lv2_is_active = true;

    plug->rkrplus->initialize();
    plug->rkrplus->calculavol(1);
    plug->rkrplus->calculavol(2);

    if(plug->rkrplus->need_bogomips_message)
        plug->rkrplus->handle_bogomips_message = true;

    // initialize for shared in/out buffer
    plug->tmp_l = (float*)malloc(sizeof(float)*plug->period_max);
    plug->tmp_r = (float*)malloc(sizeof(float)*plug->period_max);

    return plug;
}

void activate_rkrplus(LV2_Handle handle)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)handle;
 //   printf("Activate Called\n");

    plug->rkrplus->FX_Master_Active_Reset = 1;
    plug->rkrplus->Mvalue = 1;

    // Uses MIDI control to set/unset master on/off
    plug->rkrplus->Mcontrol[MC_Multi_On_Off] = 1;
    plug->rkrplus->OnOffC = 0;
    plug->rkrplus->ActiveUn(EFX_MASTER_ON_OFF);
    plug->rkrplus->Active_Preset.FX_Master_Active = 1;
}

void deactivate_rkrplus(LV2_Handle handle)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)handle;
//    printf("DE-Activate Called\n");

    plug->rkrplus->FX_Master_Active_Reset = 0;

    // Uses MIDI control to set/unset master on/off
    plug->rkrplus->Mvalue = 1;
    plug->rkrplus->Mcontrol[MC_Multi_On_Off] = 1;
    plug->rkrplus->OnOffC = 0;
    plug->rkrplus->ActiveUn(EFX_MASTER_ON_OFF);
    plug->rkrplus->Active_Preset.FX_Master_Active = 0;
}

void run_rkrplus(LV2_Handle handle, uint32_t nframes)
{
    if( nframes == 0)
        return;

    RKRPLUSLV2* plug = (RKRPLUSLV2*)handle;

    /* Sanity check */
    if( nframes > plug->period_default )
    {
        fprintf(stderr, "The Host sent NFRAFES > MAX of %d, so we is bailing out...!!!\n", nframes);
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    if(plug->rkrplus->quality_update)
    {
        inline_check(plug, nframes);    // pass input to output if needed
        return;
    }

    plug->rkrplus->efx_MIDIConverter->plug = plug;       // for MIDIConverter direct access to lv2 

    check_shared_buf(plug,nframes);

    /* Adjust for possible change in nframes from period_max on init.
     * We cannot adjust upwardly unless it is <= period_default.
     * Thus the reason for the sanity check above. */
    if( plug->period_max != nframes ) 
    {
        plug->period_max = nframes;
        plug->rkrplus->lv2_update_params(nframes);
    }

    /* Process incoming MIDI messages */
    LV2_ATOM_SEQUENCE_FOREACH(plug->atom_in_p , ev)
    {
    /*    printf("GOT atoM %d: Blank = %d: Object = %d: MIDIEvent = %d: Position = %d\n",
                ev->body.type, plug->URIDs.atom_blank,
                plug->URIDs.atom_Object,
                plug->URIDs.midi_MidiEvent,
                plug->URIDs.atom_position);
    */
        if (ev->body.type ==  plug->URIDs.midi_MidiEvent)
        {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            plug->rkrplus->lv2_process_midievents(msg);
        }
        else if ((plug->rkrplus->Tap_Active) && (plug->rkrplus->Tap_Selection == 2))
        {
            if (ev->body.type == plug->URIDs.atom_blank || ev->body.type == plug->URIDs.atom_Object)
            {
                LV2_Atom_Object *obj = (LV2_Atom_Object *)&ev->body;
                if (obj->body.otype != plug->URIDs.atom_position)
                    continue;

                LV2_Atom *bpb = NULL;
                LV2_Atom *bar = NULL;
                LV2_Atom *barBeat = NULL;
                LV2_Atom *bpm = NULL;
                LV2_Atom *beatUnit = NULL;
                lv2_atom_object_get(obj,
                                    plug->URIDs.atom_bpb, &bpb,
                                    plug->URIDs.atom_bar, &bar,
                                    plug->URIDs.atom_bar_beat, &barBeat,
                                    plug->URIDs.atom_bpm, &bpm,
                                    plug->URIDs.atom_beatUnit, &beatUnit,
                                    NULL);

                if (bpm && bpm->type == plug->URIDs.atom_Float)
                {
                    float BPM = ((LV2_Atom_Float *)bpm)->body;

                    if (beatUnit && beatUnit->type == plug->URIDs.atom_Int)
                    {
                        // In DAWs, Beats Per Minute really mean Quarter Beats Per
                        // Minute. Therefore we need to divide by four first, to
                        // get a whole beat, and then multiply that according to
                        // the time signature denominator. See this link for some
                        // background: https://music.stackexchange.com/a/109743
                        BPM = BPM / 4 * ((LV2_Atom_Int *)beatUnit)->body;
                    }
                    plug->rkrplus->lv2_set_bpm(BPM);
                }
            }
        }
    }

    // Process MIDI out - MIDIConverter
    const uint32_t out_capacity = plug->atom_out_p->atom.size;

    lv2_atom_forge_set_buffer(&plug->forge, (uint8_t*)plug->atom_out_p, out_capacity);
    lv2_atom_forge_sequence_head(&plug->forge, &plug->atom_frame, 0);
    
    // Audio - we are good to run now
    //inline copy input to process output
    memcpy(plug->rkrplus->efxoutl, plug->input_l_p, sizeof(float)*nframes);
    memcpy(plug->rkrplus->efxoutr, plug->input_r_p, sizeof(float)*nframes);

    //now run, the input here is used for wet/dry
    plug->rkrplus->process_effects(plug->input_l_p,plug->input_r_p, 0);

    // copy processed output to LV2 output
    memcpy(plug->output_l_p, plug->rkrplus->efxoutl, sizeof(float)*nframes);
    memcpy(plug->output_r_p, plug->rkrplus->efxoutr, sizeof(float)*nframes);

    return;
}

void cleanup_rkrplus(LV2_Handle handle)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)handle;
    switch(plug->effectindex)
    {
    case IRKRPLUS:
        delete plug->rkrplus;
        break;

    }
    free(plug);
}

void connect_rkrplus_ports(LV2_Handle handle, uint32_t port, void *data)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)handle;
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
    case ATOM_IN:
        plug->atom_in_p = (const LV2_Atom_Sequence*)data;
        break;
    case ATOM_OUT:
        plug->atom_out_p = (LV2_Atom_Sequence*)data;
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
    case PARAM19:
        plug->param_p[19] = (float*)data;
        break;
    default:
        puts("UNKNOWN PORT YO!!");
    }
}


LV2_State_Status stateSave(LV2_Handle h,
        LV2_State_Store_Function store,
        LV2_State_Handle handle,
        uint32_t flags,
        const LV2_Feature * const *features)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)h;
    uint32_t a = flags; flags = a;
    const LV2_Feature * const *feat = features;
    features = feat;
    // suppress warnings - may use later

    std::string str;

    plug->rkrplus->rkr_save_state(str);
    int sz = plug->rkrplus->LV2_save_preferences(str);
    char *data = new char[sz]; // Allocate memory
    strcpy(data, str.c_str()); // Copy the string

    store(handle, plug->URIDs.rkrplus_state_id, data, sz, plug->URIDs.atom_string_id, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);
    delete[] data; // Deallocate the memory

    return LV2_STATE_SUCCESS;
}


LV2_State_Status stateRestore(LV2_Handle h,
        LV2_State_Retrieve_Function retrieve,
        LV2_State_Handle handle,
        uint32_t flags,
        const LV2_Feature * const *features)
{
    RKRPLUSLV2* plug = (RKRPLUSLV2*)h;
    uint32_t a = flags; flags = a;
    const LV2_Feature * const *feat = features;
    features = feat;
    // lines above suppress warnings - may use later

    size_t sz = 0;
    LV2_URID type = 0;
    uint32_t new_flags;

    const char *data = (const char *)retrieve(handle, plug->URIDs.rkrplus_state_id, &sz, &type, &new_flags);
    
    if(sz > 0)
    {
        std::vector<int>v_default;
        std::vector<int>v_restore;
        plug->rkrplus->check_preferences_changed(v_default);

        std::string s_buf(data);
        plug->rkrplus->LV2_restore_preferences(s_buf);

        plug->rkrplus->check_preferences_changed(v_restore);

        if (v_default != v_restore)
        {
            printf("WE Are Re-initializing\n");

            plug->rkrplus->quality_update = true;
            plug->rkrplus->reset_all_effects(false);    // false - don't need state restore since we do it next
            plug->rkrplus->initialize(true);
            plug->rkrplus->quality_update = false;
        }

        plug->rkrplus->rkr_restore_state(s_buf);

        // These need to be set as if the GUI is not loaded.
        plug->rkrplus->calculavol(1);
        plug->rkrplus->calculavol(2);

        if(plug->rkrplus->Config.booster == 1.0)
            plug->rkrplus->booster = 1.0f;
        else
            plug->rkrplus->booster = dB2rap(10);
    }

    if(plug->rkrplus->need_bogomips_message)
        plug->rkrplus->handle_bogomips_message = true;

    return LV2_STATE_SUCCESS;
}


static LV2_State_Status callback_stateSave(LV2_Handle h, LV2_State_Store_Function store, LV2_State_Handle state, uint32_t flags, const LV2_Feature * const *features)
{
    return stateSave(h, store, state, flags, features);
}


static LV2_State_Status callback_stateRestore(LV2_Handle h, LV2_State_Retrieve_Function retrieve, LV2_State_Handle state, uint32_t flags, const LV2_Feature * const *features)
{
    return stateRestore(h, retrieve, state, flags, features);
}

static const void *extension_data(const char *uri)
{
    static const LV2_State_Interface state_iface = { callback_stateSave, callback_stateRestore };
    if (!strcmp(uri, LV2_STATE__interface))
    {
        return static_cast<const void *>(&state_iface);

    }

    return NULL;
}

static const LV2_Descriptor rkrplus_descriptor=
{
    RAKARRACK_PLUS_LV2_URI,
    init_rkrplus,
    connect_rkrplus_ports,
    activate_rkrplus,
    run_rkrplus,
    deactivate_rkrplus,
    cleanup_rkrplus,
    extension_data

};

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    switch (index)
    {
    case IRKRPLUS:
        return &rkrplus_descriptor ;
    default:
        return 0;
    }
}

