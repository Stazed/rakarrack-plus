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

#ifndef RAKARRACKPLUSLV2_H
#define RAKARRACKPLUSLV2_H

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
// We are including our own atom-forge.h, etc because of a name clash with Bool and one
// of the FLTK included shared libraries definition by the same name.
#include "atom-forge.h"

#include "../process.h"


#define RAKARRACK_PLUS_LV2_URI "https://github.com/Stazed/rakarrack-plus#rkrplus"
#define RAKARRACK_PLUS_LV2_UI_URI "https://github.com/Stazed/rakarrack-plus#rkrplus_ui"

typedef struct _RKRPLUSLV2
{
    uint8_t nparams;
    uint8_t effectindex;//index of effect
    uint16_t period_max;
    uint16_t period_default;
    float	*tmp_l;//temporary buffers for wet/dry mixing for hosts with shared in/out buffers(Ardour)
    float 	*tmp_r;

    //ports
    float *input_l_p;
    float *input_r_p;
    float *output_l_p;
    float *output_r_p;
    const LV2_Atom_Sequence* atom_in_p;
    LV2_Atom_Sequence* atom_out_p;
    float *param_p[20];

    //various "advanced" lv2 stuffs
    LV2_Atom_Forge	forge;
    LV2_Atom_Forge_Frame atom_frame;
    LV2_URID_Map *urid_map;

    struct urids
    {
        LV2_URID    midi_MidiEvent;
        LV2_URID    atom_Float;
        LV2_URID    atom_Int;
        LV2_URID    atom_long;
        LV2_URID    atom_string_id;
        LV2_URID    atom_Object;
        LV2_URID    atom_URID;
        LV2_URID    bufsz_max;
        LV2_URID    bufsz_min;
        LV2_URID    bufsz_nom;
        LV2_URID    rkrplus_state_id;
        LV2_URID    atom_blank;
        LV2_URID    atom_position;
        LV2_URID    atom_bpb;
        LV2_URID    atom_bar;
        LV2_URID    atom_bar_beat;
        LV2_URID    atom_bpm;
        LV2_URID    atom_beatUnit;
        
    } URIDs;

    RKR* rkrplus;

} RKRPLUSLV2;

enum RKRLV2_ports_
{
    INL =0,
    INR,
    OUTL,
    OUTR,
    ATOM_IN,    // MIDI IN
    ATOM_OUT,   // MIDI OUT
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
    PARAM19
};

/* These need to be in the same order as manifest.ttl, start with 0 for multiple plugins */
enum RKRLV2_effects_
{
    IRKRPLUS = 0
};

void
forge_midimessage(_RKRPLUSLV2* plug,
		uint32_t tme,
		const uint8_t* const buffer,
		uint32_t size);


#endif
