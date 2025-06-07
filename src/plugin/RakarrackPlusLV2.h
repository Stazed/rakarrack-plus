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


#define RAKARRACK_PLUS_LV2_URI "https://github.com/Stazed/rakarrack-plus#rkrplus"
#define RAKARRACK_PLUS_LV2_UI_URI "https://github.com/Stazed/rakarrack-plus#rkrplus_ui"
#define RAKARRACK_PLUS_STATE_URI "https://github.com/Stazed/rakarrack-plus#state"


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


#endif
