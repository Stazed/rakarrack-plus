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
