/*
  rakarrack - a guitar effects software

 rkrMIDI.C  -  MIDI functions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

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


#include "process.h"
#include <FL/fl_ask.H>  // for error pop up
#include <FL/Fl_Preferences.H>

/* MIDI control defines (Max - Min) / 127 - parameter ranges  */
const float C_MC_7_RANGE        = 0.05511811f;      /* 7 / 127 = 0.055118110236 */
const float C_MC_8_RANGE        = 0.06299f;         /* 8 / 127 = 0.062992125984252 */
const float C_MC_11_RANGE       = 0.0866142f;       /* 11 / 127 = 0.0866141732283465*/
const float C_MC_12_RANGE       = 0.094488189f;     /* 12 / 127 = 0.094488188976 */
const float C_MC_23_RANGE       = 0.18110236f;      /* 23 / 127 = 0.1811023622047 */
const float C_MC_24_RANGE       = 0.18897638f;      /* 24 / 127 = 0.188976377953  */
const float C_MC_30_RANGE       = 0.236220472441f;  /* 30 / 127 = 0.2362204724409449 */
const float C_MC_32_RANGE       = 0.25196850393701f;  /* 32 / 127 = 0.2519685039370079 */
const float C_MC_33_RANGE       = 0.25984252f;      /* 33 / 127 = 0.2598425196850 */
const float C_MC_40_RANGE       = 0.31496063f;      /* (42 - 2) / 127 = 0.314960629921 */
const float C_MC_49_RANGE       = 0.385827f;        /* (50 - 1) / 127 = 0.385826771654 */
const float C_MC_55_RANGE       = 0.43307087f;      /* (65 - 10) / 127 = 0.433070866142 */
const float C_MC_57_RANGE       = 0.448818898f;     /* (-60 - -3) / 127 = 0.448818897638 */
const float C_MC_63_RANGE       = 0.49606299f;      /* (127 - 64) / 127 = 0.4960629921259843 */
const float C_MC_80_RANGE       = 0.62992126f;      /* 80 / 127 = 0.629921259843 */
const float C_MC_90_RANGE       = 0.708661417f;     /* 90 / 127 = 0.7086614173228 */
const float C_MC_94_RANGE       = 0.74015748f;      /* (-70 + 24) / 127 = 0.740157480315 */
const float C_MC_100_RANGE      = 0.7874016f;       /* 100 / 127 = 0.787401574803 */
const float C_MC_126_RANGE      = 0.99212598f;      /* (127 - 1) / 127 = 0.992125984252 */
const float C_MC_128_RANGE      = 1.007874015748f;  /* 128 / 127 = 1.007874015748 */
const float C_MC_130_RANGE      = 1.023622f;        /* (170 - 40) / 127 = 1.0236220472441 */
const float C_MC_240_RANGE      = 1.890f;           /* (250 - 10) / 127 = 1.889763779528 */
const float C_MC_245_RANGE      = 1.9291339f;       /* (250 - 5) / 127 = 1.929133858268 */
const float C_MC_248_RANGE      = 1.9527559055f;    /* (250 - 2) / 127 = 1.9527559055118 */
const float C_MC_249_RANGE      = 1.9606299212598f; /* (250 - 1) / 127 = 1.9606299212598 */
const float C_MC_360_RANGE      = 2.83464567f;      /* (380 - 20) / 127 = 2.8346456692913 */
const float C_MC_470_RANGE      = 3.7007874f;       /* (480 - 10) / 127 = 3.7007874015748 */
const float C_MC_490_RANGE      = 3.85826772f;      /* (500 - 10) / 127 = 3.858267716535 */
const float C_MC_495_RANGE      = 3.8976378f;       /* (500 - 5) / 127 = 3.8976377952756 */
const float C_MC_498_RANGE      = 3.92125984f;      /* (500 - 2) / 127 = 3.921259842519685 */
const float C_MC_500_RANGE      = 3.9370079f;       /* 500 / 127 = 3.937007874 */
const float C_MC_600_RANGE      = 4.724f;           /* 600  / 127 = 4.724409448819 */
const float C_MC_770_RANGE      = 6.062992126f;     /* (800 - 30) / 127 = 6.062992125984 */
const float C_MC_980_RANGE      = 7.716535433f;     /* (1000 - 20) / 127 = 7.716535433071 */
const float C_MC_990_RANGE      = 7.7952756f;       /* (1000 - 10) / 127 = 7.795275590551 */
const float C_MC_995_RANGE      = 7.8346457f;       /* (1000 - 5) / 127 = 7.8346456692913 */
const float C_MC_1480_RANGE     = 11.653543f;       /* (1500 - 20) / 127 = 11.6535433071 */
const float C_MC_1900_RANGE     = 14.96063f;        /* (4500 - 2600) / 127 = 14.96062992126 */
const float C_MC_1980_RANGE     = 15.59055118f;     /* (2000 - 20) / 127 = 15.5905511811 */
const float C_MC_1999_RANGE     = 15.748031f;       /* (2000 - 1) / 127 = 15.740157480315 */
const float C_MC_2000_RANGE     = 15.7480315f;      /* 2000 / 127 = 15.748031496063 */
const float C_MC_2480_RANGE     = 19.52756f;        /* (2500 - 20) / 127 =  19.527559055118 */
const float C_MC_3600_RANGE     = 28.34645669f;     /* (4000 - 400) / 127 = 28.3464566929134 */
const float C_MC_4000_RANGE     = 31.496063f;       /* 4000 / 127 = 31.496062992126 */
const float C_MC_4380_RANGE     = 34.488189f;       /* (4400 - 20) / 127 = 34.488188976378 */
const float C_MC_4999_RANGE     = 39.362205f;       /* (5000 - 1) / 127 = 39.362204724409 */
const float C_MC_5990_RANGE     = 47.165354f;       /* (6000 - 10) / 127 = 47.1653543307087 */
const float C_MC_6000_RANGE     = 47.2441f;         /* 6000 / 127 = 47.244094488189 */
const float C_MC_6800_RANGE     = 53.54330709f;     /* (8000 - 1200) / 127 = 53.5433070866142 */
const float C_MC_7000_RANGE     = 55.11811f;        /* (8000 - 1000) / 127 = 55.11811023622 */
const float C_MC_7200_RANGE     = 56.6929134f;      /* (8000 - 800) / 127 = 56.6929133858 */
const float C_MC_11200_RANGE    = 88.18898f;        /* (12000 - 800) / 127 = 88.188976377953 */
const float C_MC_15780_RANGE    = 124.25197f;       /* (16000 - 220) / 127 = 124.251968503937 */
const float C_MC_19980_RANGE    = 157.322835f;      /* (20000 - 20) / 127 = 157.322834645669 */
const float C_MC_19999_RANGE    = 157.472441f;      /* (20000 - 1) / 127 = 157.4724409448819 */
const float C_MC_20000_RANGE    = 157.480315f;      /* (26000 - 6000) / 127 = 157.480314961 */
const float C_MC_24000_RANGE    = 188.97638f;       /* (26000 - 2000) / 127 = 188.976377952756 */
const float C_MC_25980_RANGE    = 204.566929f;      /* (26000 - 20) / 127 = 204.5669291338583 */


/**
 *  The MIDI control parameters. This includes the default MIDI control
 *  parameters (1 - 127) as well as MIDI learn.
 *
 *  The los_params[] array is the order used by the MIDI learn window.
 * 
 *  The items are copied to Effects_Params struct:
 * 
 *      char Description[32]    = MC Parameter Description - MIDI Learn Window
 *      int MC_params_index     = Parameter MIDI control number
 *      int Effect_index        = Rack Effect number
 *      int Efx_param_index     = Effect parameter number
 *      int MC_offset           = MIDI control parameter offset
 *      double MC_range         = MIDI control parameter range multiplier
 * 
 * If any additional parameters are added, then the constant
 * C_MC_PARAMETER_SIZE must be adjusted.
 */
void
RKR::MIDI_control()
{
    static const char *los_params[] =
    {
        "Balance FX%",              NTS(MC_Balance_FX).c_str(),             NTS(EFX_MASTER).c_str(),        "0",                                 "0",     "1.0",
        "Input Volume",             NTS(MC_Input_Volume).c_str(),           NTS(EFX_MASTER).c_str(),        "0",                                 "0",     "1.0",
        "Output Volume",            NTS(MC_Output_Volume).c_str(),          NTS(EFX_MASTER).c_str(),        "0",                                 "0",     "1.0",
        "Multi On/Off",             NTS(MC_Multi_On_Off).c_str(),           NTS(EFX_MASTER).c_str(),        "0",                                 "0",     "1.0",

        "Alienwah Dry/Wet",         NTS(MC_Alien_DryWet).c_str(),           NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_DryWet).c_str(),           "0",     "1.0",
        "Alienwah Pan",             NTS(MC_Alien_Pan).c_str(),              NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_Pan).c_str(),              "0",     "1.0",
        "Alienwah Tempo",           NTS(MC_Alien_LFO_Tempo).c_str(),        NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_LFO_Tempo).c_str(),        "0",     "1.0",
        "Alienwah Random",          NTS(MC_Alien_LFO_Random).c_str(),       NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_LFO_Random).c_str(),       "0",     "1.0",
        "Alienwah LFO Type",        NTS(MC_Alien_LFO_Type).c_str(),         NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_LFO_Type).c_str(),         "0",     "1.0",
        "Alienwah Phase",           NTS(MC_Alien_Phase).c_str(),            NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_Phase).c_str(),            "0",     "1.0",
        "Alienwah Stereo Df.",      NTS(MC_Alien_LFO_Stereo).c_str(),       NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_LFO_Stereo).c_str(),       "0",     "1.0",
        "Alienwah Depth",           NTS(MC_Alien_Depth).c_str(),            NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_Depth).c_str(),            "0",     "1.0",
        "Alienwah Delay",           NTS(MC_Alien_Delay).c_str(),            NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_Delay).c_str(),            "0",     "1.0",
        "Alienwah Feedback",        NTS(MC_Alien_Feedback).c_str(),         NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_Feedback).c_str(),         "0",     "1.0",
        "Alienwah L/R Cross",       NTS(MC_Alien_LR_Cross).c_str(),         NTS(EFX_ALIENWAH).c_str(),      NTS(Alien_LR_Cross).c_str(),         "0",     "1.0",

        "Analog Phaser Dry/Wet",    NTS(MC_APhase_DryWet).c_str(),          NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_DryWet).c_str(),          "0",     "1.0",
        "Analog Phaser LFO Type",   NTS(MC_APhase_LFO_Type).c_str(),        NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_LFO_Type).c_str(),        "0",     "1.0",
        "Analog Phaser Tempo",      NTS(MC_APhase_LFO_Tempo).c_str(),       NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_LFO_Tempo).c_str(),       "0",     "1.0",
        "Analog Phaser P. Depth",   NTS(MC_APhase_Depth).c_str(),           NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_Depth).c_str(),           "0",     "1.0",
        "Analog Phaser Width",      NTS(MC_APhase_Width).c_str(),           NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_Width).c_str(),           "0",     "1.0",
        "Analog Phaser Feedback",   NTS(MC_APhase_Feedback).c_str(),        NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_Feedback).c_str(),        "0",     "1.0",
        "Analog Phaser Mismatch",   NTS(MC_APhase_Mismatch).c_str(),        NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_Mismatch).c_str(),        "0",     "1.0",
        "Analog Phaser Distortion", NTS(MC_APhase_Distortion).c_str(),      NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_Distortion).c_str(),      "0",     "1.0",
        "Analog Phaser Random",     NTS(MC_APhase_LFO_Random).c_str(),      NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_LFO_Random).c_str(),      "0",     "1.0",
        "Analog Phaser Stereo Df.", NTS(MC_APhase_LFO_Stereo).c_str(),      NTS(EFX_ANALOG_PHASER).c_str(), NTS(APhase_LFO_Stereo).c_str(),      "0",     "1.0",

        "Arpie Dry/Wet",            NTS(MC_Arpie_DryWet).c_str(),           NTS(EFX_ARPIE).c_str(),         NTS(Arpie_DryWet).c_str(),           "0",     "1.0",
        "Arpie Arpe's",             NTS(MC_Arpie_ArpeWD).c_str(),           NTS(EFX_ARPIE).c_str(),         NTS(Arpie_ArpeWD).c_str(),           "0",     "1.0",
        "Arpie Pan",                NTS(MC_Arpie_Pan).c_str(),              NTS(EFX_ARPIE).c_str(),         NTS(Arpie_Pan).c_str(),              "0",     "1.0",
        "Arpie Tempo",              NTS(MC_Arpie_Tempo).c_str(),            NTS(EFX_ARPIE).c_str(),         NTS(Arpie_Tempo).c_str(),            "0",     "1.0",
        "Arpie L/R Delay",          NTS(MC_Arpie_LR_Delay).c_str(),         NTS(EFX_ARPIE).c_str(),         NTS(Arpie_LR_Delay).c_str(),         "0",     "1.0",
        "Arpie L/R Cross",          NTS(MC_Arpie_LR_Cross).c_str(),         NTS(EFX_ARPIE).c_str(),         NTS(Arpie_LR_Cross).c_str(),         "0",     "1.0",
        "Arpie Feedback",           NTS(MC_Arpie_Feedback).c_str(),         NTS(EFX_ARPIE).c_str(),         NTS(Arpie_Feedback).c_str(),         "0",     "1.0",
        "Arpie Damp",               NTS(MC_Arpie_Damp).c_str(),             NTS(EFX_ARPIE).c_str(),         NTS(Arpie_Damp).c_str(),             "0",     "1.0",

        "Cabinet Gain",             NTS(MC_Cabinet_Gain).c_str(),           NTS(EFX_CABINET).c_str(),       NTS(Cabinet_Gain).c_str(),           "0",     "1.0",

        "Chorus Dry/Wet",           NTS(MC_Chorus_DryWet).c_str(),          NTS(EFX_CHORUS).c_str(),        NTS(Chorus_DryWet).c_str(),          "0",     "1.0",
        "Chorus Pan",               NTS(MC_Chorus_Pan).c_str(),             NTS(EFX_CHORUS).c_str(),        NTS(Chorus_Pan).c_str(),             "0",     "1.0",
        "Chorus Tempo",             NTS(MC_Chorus_LFO_Tempo).c_str(),       NTS(EFX_CHORUS).c_str(),        NTS(Chorus_LFO_Tempo).c_str(),       "0",     "1.0",
        "Chorus Random",            NTS(MC_Chorus_LFO_Random).c_str(),      NTS(EFX_CHORUS).c_str(),        NTS(Chorus_LFO_Random).c_str(),      "0",     "1.0",
        "Chorus LFO Type",          NTS(MC_Chorus_LFO_Type).c_str(),        NTS(EFX_CHORUS).c_str(),        NTS(Chorus_LFO_Type).c_str(),        "0",     "1.0",
        "Chorus Stereo Df",         NTS(MC_Chorus_LFO_Stereo).c_str(),      NTS(EFX_CHORUS).c_str(),        NTS(Chorus_LFO_Stereo).c_str(),      "0",     "1.0",
        "Chorus Depth",             NTS(MC_Chorus_Depth).c_str(),           NTS(EFX_CHORUS).c_str(),        NTS(Chorus_Depth).c_str(),           "0",     "1.0",
        "Chorus Delay",             NTS(MC_Chorus_Delay).c_str(),           NTS(EFX_CHORUS).c_str(),        NTS(Chorus_Delay).c_str(),           "0",     "1.0",
        "Chorus Feedback",          NTS(MC_Chorus_Feedback).c_str(),        NTS(EFX_CHORUS).c_str(),        NTS(Chorus_Feedback).c_str(),        "0",     "1.0",
        "Chorus L/R Cross",         NTS(MC_Chorus_LR_Cross).c_str(),        NTS(EFX_CHORUS).c_str(),        NTS(Chorus_LR_Cross).c_str(),        "0",     "1.0",

        "CoilCrafter Gain",         NTS(MC_Coil_Gain).c_str(),              NTS(EFX_COILCRAFTER).c_str(),   NTS(Coil_Gain).c_str(),              "0",     "1.0",
        "CoilCrafter Tone",         NTS(MC_Coil_Tone).c_str(),              NTS(EFX_COILCRAFTER).c_str(),   NTS(Coil_Tone).c_str(),              "0",     "1.0",
        "CoilCrafter Freq 1",       NTS(MC_Coil_Freq_1).c_str(),            NTS(EFX_COILCRAFTER).c_str(),   NTS(Coil_Freq_1).c_str(),            "0",     "1.0",
        "CoilCrafter Q 1",          NTS(MC_Coil_Q_1).c_str(),               NTS(EFX_COILCRAFTER).c_str(),   NTS(Coil_Q_1).c_str(),               "0",     "1.0",
        "CoilCrafter Freq 2",       NTS(MC_Coil_Freq_2).c_str(),            NTS(EFX_COILCRAFTER).c_str(),   NTS(Coil_Freq_2).c_str(),            "0",     "1.0",
        "CoilCrafter Q 2",          NTS(MC_Coil_Q_2).c_str(),               NTS(EFX_COILCRAFTER).c_str(),   NTS(Coil_Q_2).c_str(),               "0",     "1.0",

        "CompBand Dry/Wet",         NTS(MC_CompBand_DryWet).c_str(),        NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_DryWet).c_str(),        "0",     "1.0",
        "CompBand Gain",            NTS(MC_CompBand_Gain).c_str(),          NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Gain).c_str(),          "0",     "1.0",
        "CompBand L Ratio",         NTS(MC_CompBand_Low_Ratio).c_str(),     NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Low_Ratio).c_str(),     "0",     "1.0",
        "CompBand ML Ratio",        NTS(MC_CompBand_Mid_1_Ratio).c_str(),   NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Mid_1_Ratio).c_str(),   "0",     "1.0",
        "CompBand MH Ratio",        NTS(MC_CompBand_Mid_2_Ratio).c_str(),   NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Mid_2_Ratio).c_str(),   "0",     "1.0",
        "CompBand H Ratio",         NTS(MC_CompBand_High_Ratio).c_str(),    NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_High_Ratio).c_str(),    "0",     "1.0",
        "CompBand L Thres",         NTS(MC_CompBand_Low_Thresh).c_str(),    NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Low_Thresh).c_str(),    "0",     "1.0",
        "CompBand ML Thres",        NTS(MC_CompBand_Mid_1_Thresh).c_str(),  NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Mid_1_Thresh).c_str(),  "0",     "1.0",
        "CompBand MH Thres",        NTS(MC_CompBand_Mid_2_Thresh).c_str(),  NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Mid_2_Thresh).c_str(),  "0",     "1.0",
        "CompBand H Thres",         NTS(MC_CompBand_High_Thresh).c_str(),   NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_High_Thresh).c_str(),   "0",     "1.0",
        "CompBand Cross 1",         NTS(MC_CompBand_Cross_1).c_str(),       NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Cross_1).c_str(),       "0",     "1.0",
        "CompBand Cross 2",         NTS(MC_CompBand_Cross_2).c_str(),       NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Cross_2).c_str(),       "0",     "1.0",
        "CompBand Cross 3",         NTS(MC_CompBand_Cross_3).c_str(),       NTS(EFX_COMPBAND).c_str(),      NTS(CompBand_Cross_3).c_str(),       "0",     "1.0",

        "Compressor A.Time",        NTS(MC_Compress_Attack).c_str(),        NTS(EFX_COMPRESSOR).c_str(),    NTS(Compress_Attack).c_str(),        "0",     "1.0",
        "Compressor Knee",          NTS(MC_Compress_Knee).c_str(),          NTS(EFX_COMPRESSOR).c_str(),    NTS(Compress_Knee).c_str(),          "0",     "1.0",
        "Compressor Output",        NTS(MC_Compress_Output).c_str(),        NTS(EFX_COMPRESSOR).c_str(),    NTS(Compress_Output).c_str(),        "0",     "1.0",
        "Compressor Ratio",         NTS(MC_Compress_Ratio).c_str(),         NTS(EFX_COMPRESSOR).c_str(),    NTS(Compress_Ratio).c_str(),         "0",     "1.0",
        "Compressor R.Time",        NTS(MC_Compress_Release).c_str(),       NTS(EFX_COMPRESSOR).c_str(),    NTS(Compress_Release).c_str(),       "0",     "1.0",
        "Compressor Threshold",     NTS(MC_Compress_Threshold).c_str(),     NTS(EFX_COMPRESSOR).c_str(),    NTS(Compress_Threshold).c_str(),     "0",     "1.0",

        "Convolotron Dry/Wet",      NTS(MC_Convo_DryWet).c_str(),           NTS(EFX_CONVOLOTRON).c_str(),   NTS(Convo_DryWet).c_str(),           "0",     "1.0",
        "Convolotron Pan",          NTS(MC_Convo_Pan).c_str(),              NTS(EFX_CONVOLOTRON).c_str(),   NTS(Convo_Pan).c_str(),              "0",     "1.0",
        "Convolotron Level",        NTS(MC_Convo_Level).c_str(),            NTS(EFX_CONVOLOTRON).c_str(),   NTS(Convo_Level).c_str(),            "0",     "1.0",
        "Convolotron Damp",         NTS(MC_Convo_Damp).c_str(),             NTS(EFX_CONVOLOTRON).c_str(),   NTS(Convo_Damp).c_str(),             "0",     "1.0",
        "Convolotron Feedback",     NTS(MC_Convo_Feedback).c_str(),         NTS(EFX_CONVOLOTRON).c_str(),   NTS(Convo_Feedback).c_str(),         "0",     "1.0",
        "Convolotron Length",       NTS(MC_Convo_Length).c_str(),           NTS(EFX_CONVOLOTRON).c_str(),   NTS(Convo_Length).c_str(),           "0",     "1.0",

        "Derelict Dry/Wet",         NTS(MC_Dere_DryWet).c_str(),            NTS(EFX_DERELICT).c_str(),      NTS(Dere_DryWet).c_str(),            "0",     "1.0",
        "Derelict L/R Cross",       NTS(MC_Dere_LR_Cross).c_str(),          NTS(EFX_DERELICT).c_str(),      NTS(Dere_LR_Cross).c_str(),          "0",     "1.0",
        "Derelict Drive",           NTS(MC_Dere_Drive).c_str(),             NTS(EFX_DERELICT).c_str(),      NTS(Dere_Drive).c_str(),             "0",     "1.0",
        "Derelict Level",           NTS(MC_Dere_Level).c_str(),             NTS(EFX_DERELICT).c_str(),      NTS(Dere_Level).c_str(),             "0",     "1.0",
        "Derelict Type",            NTS(MC_Dere_Type).c_str(),              NTS(EFX_DERELICT).c_str(),      NTS(Dere_Type).c_str(),              "0",     "1.0",
        "Derelict Color",           NTS(MC_Dere_Color).c_str(),             NTS(EFX_DERELICT).c_str(),      NTS(Dere_Color).c_str(),             "0",     "1.0",
        "Derelict Sub Octave",      NTS(MC_Dere_Suboctave).c_str(),         NTS(EFX_DERELICT).c_str(),      NTS(Dere_Suboctave).c_str(),         "0",     "1.0",
        "Derelict Pan",             NTS(MC_Dere_Pan).c_str(),               NTS(EFX_DERELICT).c_str(),      NTS(Dere_Pan).c_str(),               "0",     "1.0",
        "Derelict LPF",             NTS(MC_Dere_LPF).c_str(),               NTS(EFX_DERELICT).c_str(),      NTS(Dere_LPF).c_str(),               "0",     "1.0",
        "Derelict HPF",             NTS(MC_Dere_HPF).c_str(),               NTS(EFX_DERELICT).c_str(),      NTS(Dere_HPF).c_str(),               "0",     "1.0",

        "DistBand Dry/Wet",         NTS(MC_DistBand_DryWet).c_str(),        NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_DryWet).c_str(),        "0",     "1.0",
        "DistBand L/R Cross",       NTS(MC_DistBand_LR_Cross).c_str(),      NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_LR_Cross).c_str(),      "0",     "1.0",
        "DistBand Drive",           NTS(MC_DistBand_Drive).c_str(),         NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Drive).c_str(),         "0",     "1.0",
        "DistBand Level",           NTS(MC_DistBand_Level).c_str(),         NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Level).c_str(),         "0",     "1.0",
        "DistBand L. Gain",         NTS(MC_DistBand_Gain_Low).c_str(),      NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Gain_Low).c_str(),      "0",     "1.0",
        "DistBand M. Gain",         NTS(MC_DistBand_Gain_Mid).c_str(),      NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Gain_Mid).c_str(),      "0",     "1.0",
        "DistBand H. Gain",         NTS(MC_DistBand_Gain_Hi).c_str(),       NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Gain_Hi).c_str(),       "0",     "1.0",
        "DistBand Cross 1",         NTS(MC_DistBand_Cross_1).c_str(),       NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Cross_1).c_str(),       "0",     "1.0",
        "DistBand Cross 2",         NTS(MC_DistBand_Cross_2).c_str(),       NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Cross_2).c_str(),       "0",     "1.0",
        "DistBand Type Low",        NTS(MC_DistBand_Type_Low).c_str(),      NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Type_Low).c_str(),      "0",     "1.0",
        "DistBand Type Mid",        NTS(MC_DistBand_Type_Mid).c_str(),      NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Type_Mid).c_str(),      "0",     "1.0",
        "DistBand Type High",       NTS(MC_DistBand_Type_Hi).c_str(),       NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Type_Hi).c_str(),       "0",     "1.0",
        "DistBand Pan",             NTS(MC_DistBand_Pan).c_str(),           NTS(EFX_DISTBAND).c_str(),      NTS(DistBand_Pan).c_str(),           "0",     "1.0",

        "Distortion Dry/Wet",       NTS(MC_Dist_DryWet).c_str(),            NTS(EFX_DISTORTION).c_str(),    NTS(Dist_DryWet).c_str(),            "0",     "1.0",
        "Distortion L/R Cross",     NTS(MC_Dist_LR_Cross).c_str(),          NTS(EFX_DISTORTION).c_str(),    NTS(Dist_LR_Cross).c_str(),          "0",     "1.0",
        "Distortion Drive",         NTS(MC_Dist_Drive).c_str(),             NTS(EFX_DISTORTION).c_str(),    NTS(Dist_Drive).c_str(),             "0",     "1.0",
        "Distortion Level",         NTS(MC_Dist_Level).c_str(),             NTS(EFX_DISTORTION).c_str(),    NTS(Dist_Level).c_str(),             "0",     "1.0",
        "Distortion Type",          NTS(MC_Dist_Type).c_str(),              NTS(EFX_DISTORTION).c_str(),    NTS(Dist_Type).c_str(),              "0",     "1.0",
        "Distortion Pan",           NTS(MC_Dist_Pan).c_str(),               NTS(EFX_DISTORTION).c_str(),    NTS(Dist_Pan).c_str(),               "0",     "1.0",
        "Distortion Sub Octave",    NTS(MC_Dist_Suboctave).c_str(),         NTS(EFX_DISTORTION).c_str(),    NTS(Dist_Suboctave).c_str(),         "0",     "1.0",
        "Distortion LPF",           NTS(MC_Dist_LPF).c_str(),               NTS(EFX_DISTORTION).c_str(),    NTS(Dist_LPF).c_str(),               "0",     "1.0",
        "Distortion HPF",           NTS(MC_Dist_HPF).c_str(),               NTS(EFX_DISTORTION).c_str(),    NTS(Dist_HPF).c_str(),               "0",     "1.0",

        "Dual Flange Dry/Wet",      NTS(MC_DFlange_DryWet).c_str(),         NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_DryWet).c_str(),         "0",     "1.0",
        "Dual Flange Pan",          NTS(MC_DFlange_Pan).c_str(),            NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_Pan).c_str(),            "0",     "1.0",
        "Dual Flange L/R Cross",    NTS(MC_DFlange_LR_Cross).c_str(),       NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_LR_Cross).c_str(),       "0",     "1.0",
        "Dual Flange Depth",        NTS(MC_DFlange_Depth).c_str(),          NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_Depth).c_str(),          "0",     "1.0",
        "Dual Flange Width",        NTS(MC_DFlange_Width).c_str(),          NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_Width).c_str(),          "0",     "1.0",
        "Dual Flange Offset",       NTS(MC_DFlange_Offset).c_str(),         NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_Offset).c_str(),         "0",     "1.0",
        "Dual Flange Feedback",     NTS(MC_DFlange_Feedback).c_str(),       NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_Feedback).c_str(),       "0",     "1.0",
        "Dual Flange LPF",          NTS(MC_DFlange_LPF).c_str(),            NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_LPF).c_str(),            "0",     "1.0",
        "Dual Flange Tempo",        NTS(MC_DFlange_LFO_Tempo).c_str(),      NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_LFO_Tempo).c_str(),      "0",     "1.0",
        "Dual Flange Stereo Df",    NTS(MC_DFlange_LFO_Stereo).c_str(),     NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_LFO_Stereo).c_str(),     "0",     "1.0",
        "Dual Flange LFO Type",     NTS(MC_DFlange_LFO_Type).c_str(),       NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_LFO_Type).c_str(),       "0",     "1.0",
        "Dual Flange Random",       NTS(MC_DFlange_LFO_Random).c_str(),     NTS(EFX_DUAL_FLANGE).c_str(),   NTS(DFlange_LFO_Random).c_str(),     "0",     "1.0",

        "Echo Dry/Wet",             NTS(MC_Echo_DryWet).c_str(),            NTS(EFX_ECHO).c_str(),          NTS(Echo_DryWet).c_str(),            "0",     "1.0",
        "Echo Reverse",             NTS(MC_Echo_Reverse).c_str(),           NTS(EFX_ECHO).c_str(),          NTS(Echo_Reverse).c_str(),           "0",     "1.0",
        "Echo Pan",                 NTS(MC_Echo_Pan).c_str(),               NTS(EFX_ECHO).c_str(),          NTS(Echo_Pan).c_str(),               "0",     "1.0",
        "Echo Delay",               NTS(MC_Echo_Delay).c_str(),             NTS(EFX_ECHO).c_str(),          NTS(Echo_Delay).c_str(),             "0",     "1.0",
        "Echo L/R Delay",           NTS(MC_Echo_LR_Delay).c_str(),          NTS(EFX_ECHO).c_str(),          NTS(Echo_LR_Delay).c_str(),          "0",     "1.0",
        "Echo L/R Cross",           NTS(MC_Echo_LR_Cross).c_str(),          NTS(EFX_ECHO).c_str(),          NTS(Echo_LR_Cross).c_str(),          "0",     "1.0",
        "Echo Feedback",            NTS(MC_Echo_Feedback).c_str(),          NTS(EFX_ECHO).c_str(),          NTS(Echo_Feedback).c_str(),          "0",     "1.0",
        "Echo Damp",                NTS(MC_Echo_Damp).c_str(),              NTS(EFX_ECHO).c_str(),          NTS(Echo_Damp).c_str(),              "0",     "1.0",

        "Echotron Dry/Wet",         NTS(MC_Echotron_DryWet).c_str(),        NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_DryWet).c_str(),        "0",     "1.0",
        "Echotron Pan",             NTS(MC_Echotron_Pan).c_str(),           NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_Pan).c_str(),           "0",     "1.0",
        "Echotron Tempo",           NTS(MC_Echotron_Tempo).c_str(),         NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_Tempo).c_str(),         "0",     "1.0",
        "Echotron Damp",            NTS(MC_Echotron_Damp).c_str(),          NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_Damp).c_str(),          "0",     "1.0",
        "Echotron Feedback",        NTS(MC_Echotron_Feedback).c_str(),      NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_Feedback).c_str(),      "0",     "1.0",
        "Echotron L/R Cross",       NTS(MC_Echotron_LR_Cross).c_str(),      NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_LR_Cross).c_str(),      "0",     "1.0",
        "Echotron Width",           NTS(MC_Echotron_LFO_Width).c_str(),     NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_LFO_Width).c_str(),     "0",     "1.0",
        "Echotron Depth",           NTS(MC_Echotron_Depth).c_str(),         NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_Depth).c_str(),         "0",     "1.0",
        "Echotron Stereo Df",       NTS(MC_Echotron_LFO_Stereo).c_str(),    NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_LFO_Stereo).c_str(),    "0",     "1.0",
        "Echotron LFO Type",        NTS(MC_Echotron_LFO_Type).c_str(),      NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_LFO_Type).c_str(),      "0",     "1.0",
        "Echotron #",               NTS(MC_Echotron_Taps).c_str(),          NTS(EFX_ECHOTRON).c_str(),      NTS(Echotron_Taps).c_str(),          "0",     "1.0",

        "Echoverse Dry/Wet",        NTS(MC_Echoverse_DryWet).c_str(),       NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_DryWet).c_str(),       "0",     "1.0",
        "Echoverse Reverse",        NTS(MC_Echoverse_Reverse).c_str(),      NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Reverse).c_str(),      "0",     "1.0",
        "Echoverse Pan",            NTS(MC_Echoverse_Pan).c_str(),          NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Pan).c_str(),          "0",     "1.0",
        "Echoverse Tempo",          NTS(MC_Echoverse_Tempo).c_str(),        NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Tempo).c_str(),        "0",     "1.0",
        "Echoverse L/R Delay",      NTS(MC_Echoverse_LR_Delay).c_str(),     NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_LR_Delay).c_str(),     "0",     "1.0",
        "Echoverse Feedback",       NTS(MC_Echoverse_Feedback).c_str(),     NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Feedback).c_str(),     "0",     "1.0",
        "Echoverse Damp",           NTS(MC_Echoverse_Damp).c_str(),         NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Damp).c_str(),         "0",     "1.0",
        "Echoverse Ex Stereo",      NTS(MC_Echoverse_Ext_Stereo).c_str(),   NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Ext_Stereo).c_str(),   "0",     "1.0",
        "Echoverse Angle",          NTS(MC_Echoverse_Angle).c_str(),        NTS(EFX_ECHOVERSE).c_str(),     NTS(Echoverse_Angle).c_str(),        "0",     "1.0",

        "EQ Gain",                  NTS(MC_EQ_Gain).c_str(),                NTS(EFX_EQ).c_str(),            NTS(EQ_Gain).c_str(),                "0",     "1.0",
        "EQ Q",                     NTS(MC_EQ_Q).c_str(),                   NTS(EFX_EQ).c_str(),            NTS(EQ_Q).c_str(),                   "0",     "1.0",
        "EQ 31 Hz",                 NTS(MC_EQ_31_HZ).c_str(),               NTS(EFX_EQ).c_str(),            NTS(EQ_31_HZ).c_str(),               "0",     "1.0",
        "EQ 63 Hz",                 NTS(MC_EQ_63_HZ).c_str(),               NTS(EFX_EQ).c_str(),            NTS(EQ_63_HZ).c_str(),               "0",     "1.0",
        "EQ 125 Hz",                NTS(MC_EQ_125_HZ).c_str(),              NTS(EFX_EQ).c_str(),            NTS(EQ_125_HZ).c_str(),              "0",     "1.0",
        "EQ 250 Hz",                NTS(MC_EQ_250_HZ).c_str(),              NTS(EFX_EQ).c_str(),            NTS(EQ_250_HZ).c_str(),              "0",     "1.0",
        "EQ 500 Hz",                NTS(MC_EQ_500_HZ).c_str(),              NTS(EFX_EQ).c_str(),            NTS(EQ_500_HZ).c_str(),              "0",     "1.0",
        "EQ 1 Khz",                 NTS(MC_EQ_1_KHZ).c_str(),               NTS(EFX_EQ).c_str(),            NTS(EQ_1_KHZ).c_str(),               "0",     "1.0",
        "EQ 2 Khz",                 NTS(MC_EQ_2_KHZ).c_str(),               NTS(EFX_EQ).c_str(),            NTS(EQ_2_KHZ).c_str(),               "0",     "1.0",
        "EQ 4 Khz",                 NTS(MC_EQ_4_KHZ).c_str(),               NTS(EFX_EQ).c_str(),            NTS(EQ_4_KHZ).c_str(),               "0",     "1.0",
        "EQ 8 Khz",                 NTS(MC_EQ_8_KHZ).c_str(),               NTS(EFX_EQ).c_str(),            NTS(EQ_8_KHZ).c_str(),               "0",     "1.0",
        "EQ 16 Khz",                NTS(MC_EQ_16_KHZ).c_str(),              NTS(EFX_EQ).c_str(),            NTS(EQ_16_KHZ).c_str(),              "0",     "1.0",

        "Exciter Gain",             NTS(MC_Exciter_Gain).c_str(),           NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Gain).c_str(),           "0",     "1.0",
        "Exciter LPF",              NTS(MC_Exciter_LPF).c_str(),            NTS(EFX_EXCITER).c_str(),       NTS(Exciter_LPF).c_str(),            "0",     "1.0",
        "Exciter HPF",              NTS(MC_Exciter_HPF).c_str(),            NTS(EFX_EXCITER).c_str(),       NTS(Exciter_HPF).c_str(),            "0",     "1.0",
        "Exciter Har 1",            NTS(MC_Exciter_Harm_1).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_1).c_str(),         "0",     "1.0",
        "Exciter Har 2",            NTS(MC_Exciter_Harm_2).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_2).c_str(),         "0",     "1.0",
        "Exciter Har 3",            NTS(MC_Exciter_Harm_3).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_3).c_str(),         "0",     "1.0",
        "Exciter Har 4",            NTS(MC_Exciter_Harm_4).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_4).c_str(),         "0",     "1.0",
        "Exciter Har 5",            NTS(MC_Exciter_Harm_5).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_5).c_str(),         "0",     "1.0",
        "Exciter Har 6",            NTS(MC_Exciter_Harm_6).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_6).c_str(),         "0",     "1.0",
        "Exciter Har 7",            NTS(MC_Exciter_Harm_7).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_7).c_str(),         "0",     "1.0",
        "Exciter Har 8",            NTS(MC_Exciter_Harm_8).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_8).c_str(),         "0",     "1.0",
        "Exciter Har 9",            NTS(MC_Exciter_Harm_9).c_str(),         NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_9).c_str(),         "0",     "1.0",
        "Exciter Har 10",           NTS(MC_Exciter_Harm_10).c_str(),        NTS(EFX_EXCITER).c_str(),       NTS(Exciter_Harm_10).c_str(),        "0",     "1.0",

        "Expander A.Time",          NTS(MC_Expander_Attack).c_str(),        NTS(EFX_EXPANDER).c_str(),      NTS(Expander_Attack).c_str(),        "0",     "1.0",
        "Expander R.Time",          NTS(MC_Expander_Release).c_str(),       NTS(EFX_EXPANDER).c_str(),      NTS(Expander_Release).c_str(),       "0",     "1.0",
        "Expander Shape",           NTS(MC_Expander_Shape).c_str(),         NTS(EFX_EXPANDER).c_str(),      NTS(Expander_Shape).c_str(),         "0",     "1.0",
        "Expander Threshold",       NTS(MC_Expander_Threshold).c_str(),     NTS(EFX_EXPANDER).c_str(),      NTS(Expander_Threshold).c_str(),     "0",     "1.0",
        "Expander Out Gain",        NTS(MC_Expander_Gain).c_str(),          NTS(EFX_EXPANDER).c_str(),      NTS(Expander_Gain).c_str(),          "0",     "1.0",
        "Expander LPF",             NTS(MC_Expander_LPF).c_str(),           NTS(EFX_EXPANDER).c_str(),      NTS(Expander_LPF).c_str(),           "0",     "1.0",
        "Expander HPF",             NTS(MC_Expander_HPF).c_str(),           NTS(EFX_EXPANDER).c_str(),      NTS(Expander_HPF).c_str(),           "0",     "1.0",

        "Flanger Dry/Wet",          NTS(MC_Flanger_DryWet).c_str(),         NTS(EFX_FLANGER).c_str(),       NTS(Flanger_DryWet).c_str(),         "0",     "1.0",
        "Flanger Pan",              NTS(MC_Flanger_Pan).c_str(),            NTS(EFX_FLANGER).c_str(),       NTS(Flanger_Pan).c_str(),            "0",     "1.0",
        "Flanger Tempo",            NTS(MC_Flanger_LFO_Tempo).c_str(),      NTS(EFX_FLANGER).c_str(),       NTS(Flanger_LFO_Tempo).c_str(),      "0",     "1.0",
        "Flanger Random",           NTS(MC_Flanger_LFO_Random).c_str(),     NTS(EFX_FLANGER).c_str(),       NTS(Flanger_LFO_Random).c_str(),     "0",     "1.0",
        "Flanger LFO Type",         NTS(MC_Flanger_LFO_Type).c_str(),       NTS(EFX_FLANGER).c_str(),       NTS(Flanger_LFO_Type).c_str(),       "0",     "1.0",
        "Flanger Stereo Df",        NTS(MC_Flanger_LFO_Stereo).c_str(),     NTS(EFX_FLANGER).c_str(),       NTS(Flanger_LFO_Stereo).c_str(),     "0",     "1.0",
        "Flanger Depth",            NTS(MC_Flanger_Depth).c_str(),          NTS(EFX_FLANGER).c_str(),       NTS(Flanger_Depth).c_str(),          "0",     "1.0",
        "Flanger Delay",            NTS(MC_Flanger_Delay).c_str(),          NTS(EFX_FLANGER).c_str(),       NTS(Flanger_Delay).c_str(),          "0",     "1.0",
        "Flanger Feedback",         NTS(MC_Flanger_Feedback).c_str(),       NTS(EFX_FLANGER).c_str(),       NTS(Flanger_Feedback).c_str(),       "0",     "1.0",
        "Flanger L/R Cross",        NTS(MC_Flanger_LR_Cross).c_str(),       NTS(EFX_FLANGER).c_str(),       NTS(Flanger_LR_Cross).c_str(),       "0",     "1.0",

        "Harmonizer Dry/Wet",       NTS(MC_Harm_DryWet).c_str(),            NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_DryWet).c_str(),            "0",     "1.0",
        "Harmonizer Interval",      NTS(MC_Harm_Interval).c_str(),          NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Interval).c_str(),          "0",     "1.0",
        "Harmonizer Gain",          NTS(MC_Harm_Gain).c_str(),              NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Gain).c_str(),              "0",     "1.0",
        "Harmonizer Pan",           NTS(MC_Harm_Pan).c_str(),               NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Pan).c_str(),               "0",     "1.0",
        "Harmonizer Freq",          NTS(MC_Harm_Filter_Freq).c_str(),       NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Filter_Freq).c_str(),       "0",     "1.0",
        "Harmonizer Filter Gain",   NTS(MC_Harm_Filter_Gain).c_str(),       NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Filter_Gain).c_str(),       "0",     "1.0",
        "Harmonizer Filter Q",      NTS(MC_Harm_Filter_Q).c_str(),          NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Filter_Q).c_str(),          "0",     "1.0",
        "Harmonizer SELECT",        NTS(MC_Harm_Select).c_str(),            NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Select).c_str(),            "0",     "1.0",
        "Harmonizer Note",          NTS(MC_Harm_Note).c_str(),              NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Note).c_str(),              "0",     "1.0",
        "Harmonizer Chord",         NTS(MC_Harm_Chord).c_str(),             NTS(EFX_HARMONIZER).c_str(),    NTS(Harm_Chord).c_str(),             "0",     "1.0",

        "Infinity Dry/Wet",         NTS(MC_Infinity_DryWet).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_DryWet).c_str(),        "0",     "1.0",
        "Infinity Res",             NTS(MC_Infinity_Resonance).c_str(),     NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Resonance).c_str(),     "0",     "1.0",
        "Infinity Filter Band 1",   NTS(MC_Infinity_Band_1).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_1).c_str(),        "0",     "1.0",
        "Infinity Filter Band 2",   NTS(MC_Infinity_Band_2).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_2).c_str(),        "0",     "1.0",
        "Infinity Filter Band 3",   NTS(MC_Infinity_Band_3).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_3).c_str(),        "0",     "1.0",
        "Infinity Filter Band 4",   NTS(MC_Infinity_Band_4).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_4).c_str(),        "0",     "1.0",
        "Infinity Filter Band 5",   NTS(MC_Infinity_Band_5).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_5).c_str(),        "0",     "1.0",
        "Infinity Filter Band 6",   NTS(MC_Infinity_Band_6).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_6).c_str(),        "0",     "1.0",
        "Infinity Filter Band 7",   NTS(MC_Infinity_Band_7).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_7).c_str(),        "0",     "1.0",
        "Infinity Filter Band 8",   NTS(MC_Infinity_Band_8).c_str(),        NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Band_8).c_str(),        "0",     "1.0",
        "Infinity AutoPan",         NTS(MC_Infinity_AutoPan).c_str(),       NTS(EFX_INFINITY).c_str(),      NTS(Infinity_AutoPan).c_str(),       "0",     "1.0",
        "Infinity Stereo Df",       NTS(MC_Infinity_LR_Delay).c_str(),      NTS(EFX_INFINITY).c_str(),      NTS(Infinity_LR_Delay).c_str(),      "0",     "1.0",
        "Infinity Start",           NTS(MC_Infinity_Start).c_str(),         NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Start).c_str(),         "0",     "1.0",
        "Infinity End",             NTS(MC_Infinity_End).c_str(),           NTS(EFX_INFINITY).c_str(),      NTS(Infinity_End).c_str(),           "0",     "1.0",
        "Infinity Tempo",           NTS(MC_Infinity_Tempo).c_str(),         NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Tempo).c_str(),         "0",     "1.0",
        "Infinity Subdiv",          NTS(MC_Infinity_Subdivision).c_str(),   NTS(EFX_INFINITY).c_str(),      NTS(Infinity_Subdivision).c_str(),   "0",     "1.0",

        "Looper Dry/Wet",           NTS(MC_Looper_DryWet).c_str(),          NTS(EFX_LOOPER).c_str(),        NTS(Looper_DryWet).c_str(),          "0",     "1.0",
        "Looper Level 1",           NTS(MC_Looper_Level_1).c_str(),         NTS(EFX_LOOPER).c_str(),        NTS(Looper_Level_1).c_str(),         "0",     "1.0",
        "Looper Level 2",           NTS(MC_Looper_Level_2).c_str(),         NTS(EFX_LOOPER).c_str(),        NTS(Looper_Level_2).c_str(),         "0",     "1.0",
        "Looper Tempo",             NTS(MC_Looper_Tempo).c_str(),           NTS(EFX_LOOPER).c_str(),        NTS(Looper_Tempo).c_str(),           "0",     "1.0",
        "Looper Reverse",           NTS(MC_Looper_Reverse).c_str(),         NTS(EFX_LOOPER).c_str(),        NTS(Looper_Reverse).c_str(),         "0",     "1.0",
        "Looper Auto Play",         NTS(MC_Looper_AutoPlay).c_str(),        NTS(EFX_LOOPER).c_str(),        NTS(Looper_AutoPlay).c_str(),        "0",     "1.0",
        "Looper Play",              NTS(MC_Looper_Play).c_str(),            NTS(EFX_LOOPER).c_str(),        NTS(Looper_Play).c_str(),            "0",     "1.0",
        "Looper Pause",             NTS(MC_Looper_Stop).c_str(),            NTS(EFX_LOOPER).c_str(),        NTS(Looper_Stop).c_str(),            "0",     "1.0",
        "Looper Record",            NTS(MC_Looper_Record).c_str(),          NTS(EFX_LOOPER).c_str(),        NTS(Looper_Record).c_str(),          "0",     "1.0",
        "Looper R1",                NTS(MC_Looper_Rec_1).c_str(),           NTS(EFX_LOOPER).c_str(),        NTS(Looper_Rec_1).c_str(),           "0",     "1.0",
        "Looper R2",                NTS(MC_Looper_Rec_2).c_str(),           NTS(EFX_LOOPER).c_str(),        NTS(Looper_Rec_2).c_str(),           "0",     "1.0",
        "Looper Track 1",           NTS(MC_Looper_Track_1).c_str(),         NTS(EFX_LOOPER).c_str(),        NTS(Looper_Track_1).c_str(),         "0",     "1.0",
        "Looper Track 2",           NTS(MC_Looper_Track_2).c_str(),         NTS(EFX_LOOPER).c_str(),        NTS(Looper_Track_2).c_str(),         "0",     "1.0",
        "Looper Clear",             NTS(MC_Looper_Clear).c_str(),           NTS(EFX_LOOPER).c_str(),        NTS(Looper_Clear).c_str(),           "0",     "1.0",

        "Musical Delay Dry/Wet",    NTS(MC_Music_DryWet).c_str(),           NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_DryWet).c_str(),           "0",     "1.0",
        "Musical Delay L/R Cross",  NTS(MC_Music_LR_Cross).c_str(),         NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_LR_Cross).c_str(),         "0",     "1.0",
        "Musical Delay Pan 1",      NTS(MC_Music_Pan_1).c_str(),            NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Pan_1).c_str(),            "0",     "1.0",
        "Musical Delay Pan 2",      NTS(MC_Music_Pan_2).c_str(),            NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Pan_2).c_str(),            "0",     "1.0",
        "Musical Delay Tempo",      NTS(MC_Music_Tempo).c_str(),            NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Tempo).c_str(),            "0",     "1.0",
        "Musical Delay Gain 1",     NTS(MC_Music_Gain_1).c_str(),           NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Gain_1).c_str(),           "0",     "1.0",
        "Musical Delay Gain 2",     NTS(MC_Music_Gain_2).c_str(),           NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Gain_2).c_str(),           "0",     "1.0",
        "Musical Delay FB 1",       NTS(MC_Music_Feedback_1).c_str(),       NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Feedback_1).c_str(),       "0",     "1.0",
        "Musical Delay FB 2",       NTS(MC_Music_Feedback_2).c_str(),       NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Feedback_2).c_str(),       "0",     "1.0",
        "Musical Delay Damp",       NTS(MC_Music_Damp).c_str(),             NTS(EFX_MUSICAL_DELAY).c_str(), NTS(Music_Damp).c_str(),             "0",     "1.0",

        "MuTroMojo Dry/Wet",        NTS(MC_MuTro_DryWet).c_str(),           NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_DryWet).c_str(),           "0",     "1.0",
        "MuTroMojo LP",             NTS(MC_MuTro_LowPass).c_str(),          NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_LowPass).c_str(),          "0",     "1.0",
        "MuTroMojo BP",             NTS(MC_MuTro_BandPass).c_str(),         NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_BandPass).c_str(),         "0",     "1.0",
        "MuTroMojo HP",             NTS(MC_MuTro_HighPass).c_str(),         NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_HighPass).c_str(),         "0",     "1.0",
        "MuTroMojo LFO Type",       NTS(MC_MuTro_LFO_Type).c_str(),         NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_LFO_Type).c_str(),         "0",     "1.0",
        "MuTroMojo Depth",          NTS(MC_MuTro_Depth).c_str(),            NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_Depth).c_str(),            "0",     "1.0",
        "MuTroMojo Tempo",          NTS(MC_MuTro_LFO_Tempo).c_str(),        NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_LFO_Tempo).c_str(),        "0",     "1.0",
        "MuTroMojo Res",            NTS(MC_MuTro_Resonance).c_str(),        NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_Resonance).c_str(),        "0",     "1.0",
        "MuTroMojo Range",          NTS(MC_MuTro_Range).c_str(),            NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_Range).c_str(),            "0",     "1.0",
        "MuTroMojo Wah",            NTS(MC_MuTro_Wah).c_str(),              NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_Wah).c_str(),              "0",     "1.0",
        "MuTroMojo E. Sens",        NTS(MC_MuTro_Env_Sens).c_str(),         NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_Env_Sens).c_str(),         "0",     "1.0",
        "MuTroMojo Smooth",         NTS(MC_MuTro_Env_Smooth).c_str(),       NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_Env_Smooth).c_str(),       "0",     "1.0",
        "MuTroMojo Random",         NTS(MC_MuTro_LFO_Random).c_str(),       NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_LFO_Random).c_str(),       "0",     "1.0",
        "MuTroMojo Stereo Df",      NTS(MC_MuTro_LFO_Stereo).c_str(),       NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_LFO_Stereo).c_str(),       "0",     "1.0",
        "MuTroMojo St. Freq",       NTS(MC_MuTro_St_Freq).c_str(),          NTS(EFX_MUTROMOJO).c_str(),     NTS(MuTro_St_Freq).c_str(),          "0",     "1.0",

        "NoiseGate A. Time",        NTS(MC_Gate_Attack).c_str(),            NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_Attack).c_str(),            "0",     "1.0",
        "NoiseGate R. Time",        NTS(MC_Gate_Release).c_str(),           NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_Release).c_str(),           "0",     "1.0",
        "NoiseGate Range",          NTS(MC_Gate_Range).c_str(),             NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_Range).c_str(),             "0",     "1.0",
        "NoiseGate Threshold",      NTS(MC_Gate_Threshold).c_str(),         NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_Threshold).c_str(),         "0",     "1.0",
        "NoiseGate Hold",           NTS(MC_Gate_Hold).c_str(),              NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_Hold).c_str(),              "0",     "1.0",
        "NoiseGate LPF",            NTS(MC_Gate_LPF).c_str(),               NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_LPF).c_str(),               "0",     "1.0",
        "NoiseGate HPF",            NTS(MC_Gate_HPF).c_str(),               NTS(EFX_NOISEGATE).c_str(),     NTS(Gate_HPF).c_str(),               "0",     "1.0",

        "Opticaltrem Depth",        NTS(MC_Optical_Depth).c_str(),          NTS(EFX_OPTICALTREM).c_str(),   NTS(Optical_Depth).c_str(),          "0",     "1.0",
        "Opticaltrem Tempo",        NTS(MC_Optical_LFO_Tempo).c_str(),      NTS(EFX_OPTICALTREM).c_str(),   NTS(Optical_LFO_Tempo).c_str(),      "0",     "1.0",
        "Opticaltrem Random",       NTS(MC_Optical_LFO_Random).c_str(),     NTS(EFX_OPTICALTREM).c_str(),   NTS(Optical_LFO_Random).c_str(),     "0",     "1.0",
        "Opticaltrem LFO Type",     NTS(MC_Optical_LFO_Type).c_str(),       NTS(EFX_OPTICALTREM).c_str(),   NTS(Optical_LFO_Type).c_str(),       "0",     "1.0",
        "Opticaltrem Stereo Df",    NTS(MC_Optical_LFO_Stereo).c_str(),     NTS(EFX_OPTICALTREM).c_str(),   NTS(Optical_LFO_Stereo).c_str(),     "0",     "1.0",
        "Opticaltrem Pan",          NTS(MC_Optical_Pan).c_str(),            NTS(EFX_OPTICALTREM).c_str(),   NTS(Optical_Pan).c_str(),            "0",     "1.0",

        "Overdrive Dry/Wet",        NTS(MC_Overdrive_DryWet).c_str(),       NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_DryWet).c_str(),       "0",     "1.0",
        "Overdrive L/R Cross",      NTS(MC_Overdrive_LR_Cross).c_str(),     NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_LR_Cross).c_str(),     "0",     "1.0",
        "Overdrive Drive",          NTS(MC_Overdrive_Drive).c_str(),        NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_Drive).c_str(),        "0",     "1.0",
        "Overdrive Level",          NTS(MC_Overdrive_Level).c_str(),        NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_Level).c_str(),        "0",     "1.0",
        "Overdrive Type",           NTS(MC_Overdrive_Type).c_str(),         NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_Type).c_str(),         "0",     "1.0",
        "Overdrive Pan",            NTS(MC_Overdrive_Pan).c_str(),          NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_Pan).c_str(),          "0",     "1.0",
        "Overdrive LPF",            NTS(MC_Overdrive_LPF).c_str(),          NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_LPF).c_str(),          "0",     "1.0",
        "Overdrive HPF",            NTS(MC_Overdrive_HPF).c_str(),          NTS(EFX_OVERDRIVE).c_str(),     NTS(Overdrive_HPF).c_str(),          "0",     "1.0",

        "Pan Dry/Wet",              NTS(MC_Pan_DryWet).c_str(),             NTS(EFX_PAN).c_str(),           NTS(Pan_DryWet).c_str(),             "0",     "1.0",
        "Pan Pan",                  NTS(MC_Pan_Pan).c_str(),                NTS(EFX_PAN).c_str(),           NTS(Pan_Pan).c_str(),                "0",     "1.0",
        "Pan Tempo",                NTS(MC_Pan_LFO_Tempo).c_str(),          NTS(EFX_PAN).c_str(),           NTS(Pan_LFO_Tempo).c_str(),          "0",     "1.0",
        "Pan Random",               NTS(MC_Pan_LFO_Random).c_str(),         NTS(EFX_PAN).c_str(),           NTS(Pan_LFO_Random).c_str(),         "0",     "1.0",
        "Pan LFO Type",             NTS(MC_Pan_LFO_Type).c_str(),           NTS(EFX_PAN).c_str(),           NTS(Pan_LFO_Type).c_str(),           "0",     "1.0",
        "Pan Stereo Df",            NTS(MC_Pan_LFO_Stereo).c_str(),         NTS(EFX_PAN).c_str(),           NTS(Pan_LFO_Stereo).c_str(),         "0",     "1.0",
        "Pan E. Stereo",            NTS(MC_Pan_Ex_St_Amt).c_str(),          NTS(EFX_PAN).c_str(),           NTS(Pan_Ex_St_Amt).c_str(),          "0",     "1.0",

        "Parametric EQ Gain",       NTS(MC_Parametric_Gain).c_str(),        NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Gain).c_str(),        "0",     "1.0",
        "Parametric EQ Low Freq",   NTS(MC_Parametric_Low_Freq).c_str(),    NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Low_Freq).c_str(),    "0",     "1.0",
        "Parametric EQ Low Gain",   NTS(MC_Parametric_Low_Gain).c_str(),    NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Low_Gain).c_str(),    "0",     "1.0",
        "Parametric EQ Low Q",      NTS(MC_Parametric_Low_Q).c_str(),       NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Low_Q).c_str(),       "0",     "1.0",
        "Parametric EQ Mid Freq",   NTS(MC_Parametric_Mid_Freq).c_str(),    NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Mid_Freq).c_str(),    "0",     "1.0",
        "Parametric EQ Mid Gain",   NTS(MC_Parametric_Mid_Gain).c_str(),    NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Mid_Gain).c_str(),    "0",     "1.0",
        "Parametric EQ Mid Q",      NTS(MC_Parametric_Mid_Q).c_str(),       NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_Mid_Q).c_str(),       "0",     "1.0",
        "Parametric EQ High Freq",  NTS(MC_Parametric_High_Freq).c_str(),   NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_High_Freq).c_str(),   "0",     "1.0",
        "Parametric EQ High Gain",  NTS(MC_Parametric_High_Gain).c_str(),   NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_High_Gain).c_str(),   "0",     "1.0",
        "Parametric EQ High Q",     NTS(MC_Parametric_High_Q).c_str(),      NTS(EFX_PARAMETRIC).c_str(),    NTS(Parametric_High_Q).c_str(),      "0",     "1.0",

        "Phaser Dry/Wet",           NTS(MC_Phaser_DryWet).c_str(),          NTS(EFX_PHASER).c_str(),        NTS(Phaser_DryWet).c_str(),          "0",     "1.0",
        "Phaser Pan",               NTS(MC_Phaser_Pan).c_str(),             NTS(EFX_PHASER).c_str(),        NTS(Phaser_Pan).c_str(),             "0",     "1.0",
        "Phaser Tempo",             NTS(MC_Phaser_LFO_Tempo).c_str(),       NTS(EFX_PHASER).c_str(),        NTS(Phaser_LFO_Tempo).c_str(),       "0",     "1.0",
        "Phaser Random",            NTS(MC_Phaser_LFO_Random).c_str(),      NTS(EFX_PHASER).c_str(),        NTS(Phaser_LFO_Random).c_str(),      "0",     "1.0",
        "Phaser LFO Type",          NTS(MC_Phaser_LFO_Type).c_str(),        NTS(EFX_PHASER).c_str(),        NTS(Phaser_LFO_Type).c_str(),        "0",     "1.0",
        "Phaser Phase",             NTS(MC_Phaser_Phase).c_str(),           NTS(EFX_PHASER).c_str(),        NTS(Phaser_Phase).c_str(),           "0",     "1.0",
        "Phaser Stereo Df",         NTS(MC_Phaser_LFO_Stereo).c_str(),      NTS(EFX_PHASER).c_str(),        NTS(Phaser_LFO_Stereo).c_str(),      "0",     "1.0",
        "Phaser Depth",             NTS(MC_Phaser_Depth).c_str(),           NTS(EFX_PHASER).c_str(),        NTS(Phaser_Depth).c_str(),           "0",     "1.0",
        "Phaser Feedback",          NTS(MC_Phaser_Feedback).c_str(),        NTS(EFX_PHASER).c_str(),        NTS(Phaser_Feedback).c_str(),        "0",     "1.0",
        "Phaser L/R Cross",         NTS(MC_Phaser_LR_Cross).c_str(),        NTS(EFX_PHASER).c_str(),        NTS(Phaser_LR_Cross).c_str(),        "0",     "1.0",

        "Reverb Dry/Wet",           NTS(MC_Reverb_DryWet).c_str(),          NTS(EFX_REVERB).c_str(),        NTS(Reverb_DryWet).c_str(),          "0",     "1.0",
        "Reverb Pan",               NTS(MC_Reverb_Pan).c_str(),             NTS(EFX_REVERB).c_str(),        NTS(Reverb_Pan).c_str(),             "0",     "1.0",
        "Reverb Time",              NTS(MC_Reverb_Time).c_str(),            NTS(EFX_REVERB).c_str(),        NTS(Reverb_Time).c_str(),            "0",     "1.0",
        "Reverb Initial Delay",     NTS(MC_Reverb_I_Delay).c_str(),         NTS(EFX_REVERB).c_str(),        NTS(Reverb_I_Delay).c_str(),         "0",     "1.0",
        "Reverb Del. E/R",          NTS(MC_Reverb_Delay_FB).c_str(),        NTS(EFX_REVERB).c_str(),        NTS(Reverb_Delay_FB).c_str(),        "0",     "1.0",
        "Reverb Room Size",         NTS(MC_Reverb_Room).c_str(),            NTS(EFX_REVERB).c_str(),        NTS(Reverb_Room).c_str(),            "0",     "1.0",
        "Reverb LPF",               NTS(MC_Reverb_LPF).c_str(),             NTS(EFX_REVERB).c_str(),        NTS(Reverb_LPF).c_str(),             "0",     "1.0",
        "Reverb HPF",               NTS(MC_Reverb_HPF).c_str(),             NTS(EFX_REVERB).c_str(),        NTS(Reverb_HPF).c_str(),             "0",     "1.0",
        "Reverb Damping",           NTS(MC_Reverb_Damp).c_str(),            NTS(EFX_REVERB).c_str(),        NTS(Reverb_Damp).c_str(),            "0",     "1.0",

        "Reverbtron Dry/Wet",       NTS(MC_Revtron_DryWet).c_str(),         NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_DryWet).c_str(),         "0",     "1.0",
        "Reverbtron Pan",           NTS(MC_Revtron_Pan).c_str(),            NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Pan).c_str(),            "0",     "1.0",
        "Reverbtron Level",         NTS(MC_Revtron_Level).c_str(),          NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Level).c_str(),          "0",     "1.0",
        "Reverbtron Damp",          NTS(MC_Revtron_Damp).c_str(),           NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Damp).c_str(),           "0",     "1.0",
        "Reverbtron Feedback",      NTS(MC_Revtron_Feedback).c_str(),       NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Feedback).c_str(),       "0",     "1.0",
        "Reverbtron Length",        NTS(MC_Revtron_Length).c_str(),         NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Length).c_str(),         "0",     "1.0",
        "Reverbtron Stretch",       NTS(MC_Revtron_Stretch).c_str(),        NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Stretch).c_str(),        "0",     "1.0",
        "Reverbtron Initial Delay", NTS(MC_Revtron_I_Delay).c_str(),        NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_I_Delay).c_str(),        "0",     "1.0",
        "Reverbtron Fade",          NTS(MC_Revtron_Fade).c_str(),           NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Fade).c_str(),           "0",     "1.0",
        "Reverbtron Diffusion",     NTS(MC_Revtron_Diffusion).c_str(),      NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_Diffusion).c_str(),      "0",     "1.0",
        "Reverbtron LPF",           NTS(MC_Revtron_LPF).c_str(),            NTS(EFX_REVERBTRON).c_str(),    NTS(Revtron_LPF).c_str(),            "0",     "1.0",

        "Ring Dry/Wet",             NTS(MC_Ring_DryWet).c_str(),            NTS(EFX_RING).c_str(),          NTS(Ring_DryWet).c_str(),            "0",     "1.0",
        "Ring L/R Cross",           NTS(MC_Ring_LR_Cross).c_str(),          NTS(EFX_RING).c_str(),          NTS(Ring_LR_Cross).c_str(),          "0",     "1.0",
        "Ring Input",               NTS(MC_Ring_Input).c_str(),             NTS(EFX_RING).c_str(),          NTS(Ring_Input).c_str(),             "0",     "1.0",
        "Ring Level",               NTS(MC_Ring_Level).c_str(),             NTS(EFX_RING).c_str(),          NTS(Ring_Level).c_str(),             "0",     "1.0",
        "Ring Pan",                 NTS(MC_Ring_Pan).c_str(),               NTS(EFX_RING).c_str(),          NTS(Ring_Pan).c_str(),               "0",     "1.0",
        "Ring Depth",               NTS(MC_Ring_Depth).c_str(),             NTS(EFX_RING).c_str(),          NTS(Ring_Depth).c_str(),             "0",     "1.0",
        "Ring Freq",                NTS(MC_Ring_Freq).c_str(),              NTS(EFX_RING).c_str(),          NTS(Ring_Freq).c_str(),              "0",     "1.0",
        "Ring Sine",                NTS(MC_Ring_Sine).c_str(),              NTS(EFX_RING).c_str(),          NTS(Ring_Sine).c_str(),              "0",     "1.0",
        "Ring Triangle",            NTS(MC_Ring_Triangle).c_str(),          NTS(EFX_RING).c_str(),          NTS(Ring_Triangle).c_str(),          "0",     "1.0",
        "Ring Sawtooth",            NTS(MC_Ring_Saw).c_str(),               NTS(EFX_RING).c_str(),          NTS(Ring_Saw).c_str(),               "0",     "1.0",
        "Ring Square",              NTS(MC_Ring_Square).c_str(),            NTS(EFX_RING).c_str(),          NTS(Ring_Square).c_str(),            "0",     "1.0",

        "Sequence Dry/Wet",         NTS(MC_Sequence_DryWet).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_DryWet).c_str(),        "0",     "1.0",
        "Sequence 1",               NTS(MC_Sequence_Step_1).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_1).c_str(),        "0",     "1.0",
        "Sequence 2",               NTS(MC_Sequence_Step_2).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_2).c_str(),        "0",     "1.0",
        "Sequence 3",               NTS(MC_Sequence_Step_3).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_3).c_str(),        "0",     "1.0",
        "Sequence 4",               NTS(MC_Sequence_Step_4).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_4).c_str(),        "0",     "1.0",
        "Sequence 5",               NTS(MC_Sequence_Step_5).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_5).c_str(),        "0",     "1.0",
        "Sequence 6",               NTS(MC_Sequence_Step_6).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_6).c_str(),        "0",     "1.0",
        "Sequence 7",               NTS(MC_Sequence_Step_7).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_7).c_str(),        "0",     "1.0",
        "Sequence 8",               NTS(MC_Sequence_Step_8).c_str(),        NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Step_8).c_str(),        "0",     "1.0",
        "Sequence Tempo",           NTS(MC_Sequence_Tempo).c_str(),         NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Tempo).c_str(),         "0",     "1.0",
        "Sequence Q",               NTS(MC_Sequence_Resonance).c_str(),     NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Resonance).c_str(),     "0",     "1.0",
        "Sequence Stereo Df",       NTS(MC_Sequence_Stdf).c_str(),          NTS(EFX_SEQUENCE).c_str(),      NTS(Sequence_Stdf).c_str(),          "0",     "1.0",

        "ShelfBoost Gain",          NTS(MC_Shelf_Gain).c_str(),             NTS(EFX_SHELFBOOST).c_str(),    NTS(Shelf_Gain).c_str(),             "0",     "1.0",
        "ShelfBoost Level",         NTS(MC_Shelf_Level).c_str(),            NTS(EFX_SHELFBOOST).c_str(),    NTS(Shelf_Level).c_str(),            "0",     "1.0",
        "ShelfBoost Tone",          NTS(MC_Shelf_Tone).c_str(),             NTS(EFX_SHELFBOOST).c_str(),    NTS(Shelf_Tone).c_str(),             "0",     "1.0",
        "ShelfBoost Presence",      NTS(MC_Shelf_Presence).c_str(),         NTS(EFX_SHELFBOOST).c_str(),    NTS(Shelf_Presence).c_str(),         "0",     "1.0",

        "Shifter Dry/Wet",          NTS(MC_Shifter_DryWet).c_str(),         NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_DryWet).c_str(),         "0",     "1.0",
        "Shifter Interval",         NTS(MC_Shifter_Interval).c_str(),       NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Interval).c_str(),       "0",     "1.0",
        "Shifter Gain",             NTS(MC_Shifter_Gain).c_str(),           NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Gain).c_str(),           "0",     "1.0",
        "Shifter Pan",              NTS(MC_Shifter_Pan).c_str(),            NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Pan).c_str(),            "0",     "1.0",
        "Shifter Attack",           NTS(MC_Shifter_Attack).c_str(),         NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Attack).c_str(),         "0",     "1.0",
        "Shifter Decay",            NTS(MC_Shifter_Decay).c_str(),          NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Decay).c_str(),          "0",     "1.0",
        "Shifter Threshold",        NTS(MC_Shifter_Threshold).c_str(),      NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Threshold).c_str(),      "0",     "1.0",
        "Shifter Whammy",           NTS(MC_Shifter_Whammy).c_str(),         NTS(EFX_SHIFTER).c_str(),       NTS(Shifter_Whammy).c_str(),         "0",     "1.0",

        "Shuffle Dry/Wet",          NTS(MC_Shuffle_DryWet).c_str(),         NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_DryWet).c_str(),         "0",     "1.0",
        "Shuffle Low Freq",         NTS(MC_Shuffle_Freq_L).c_str(),         NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Freq_L).c_str(),         "0",     "1.0",
        "Shuffle Low Gain",         NTS(MC_Shuffle_Gain_L).c_str(),         NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Gain_L).c_str(),         "0",     "1.0",
        "Shuffle M.L. Freq",        NTS(MC_Shuffle_Freq_ML).c_str(),        NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Freq_ML).c_str(),        "0",     "1.0",
        "Shuffle M.L. Gain",        NTS(MC_Shuffle_Gain_ML).c_str(),        NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Gain_ML).c_str(),        "0",     "1.0",
        "Shuffle M.H. Freq",        NTS(MC_Shuffle_Freq_MH).c_str(),        NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Freq_MH).c_str(),        "0",     "1.0",
        "Shuffle M.H. Gain",        NTS(MC_Shuffle_Gain_MH).c_str(),        NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Gain_MH).c_str(),        "0",     "1.0",
        "Shuffle High Freq",        NTS(MC_Shuffle_Freq_H).c_str(),         NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Freq_H).c_str(),         "0",     "1.0",
        "Shuffle High Gain",        NTS(MC_Shuffle_Gain_H).c_str(),         NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Gain_H).c_str(),         "0",     "1.0",
        "Shuffle Q",                NTS(MC_Shuffle_Width).c_str(),          NTS(EFX_SHUFFLE).c_str(),       NTS(Shuffle_Width).c_str(),          "0",     "1.0",

        "StereoHarm Dry/Wet",       NTS(MC_Sharm_DryWet).c_str(),           NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_DryWet).c_str(),           "0",     "1.0",
        "StereoHarm Int L",         NTS(MC_Sharm_L_Interval).c_str(),       NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_L_Interval).c_str(),       "0",     "1.0",
        "StereoHarm Chrm L",        NTS(MC_Sharm_L_Chroma).c_str(),         NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_L_Chroma).c_str(),         "0",     "1.0",
        "StereoHarm Gain L",        NTS(MC_Sharm_L_Gain).c_str(),           NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_L_Gain).c_str(),           "0",     "1.0",
        "StereoHarm Int R",         NTS(MC_Sharm_R_Interval).c_str(),       NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_R_Interval).c_str(),       "0",     "1.0",
        "StereoHarm Chrm R",        NTS(MC_Sharm_R_Chroma).c_str(),         NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_R_Chroma).c_str(),         "0",     "1.0",
        "StereoHarm Gain R",        NTS(MC_Sharm_R_Gain).c_str(),           NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_R_Gain).c_str(),           "0",     "1.0",
        "StereoHarm L/R Cross",     NTS(MC_Sharm_LR_Cross).c_str(),         NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_LR_Cross).c_str(),         "0",     "1.0",
        "StereoHarm SELECT",        NTS(MC_Sharm_Select).c_str(),           NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_Select).c_str(),           "0",     "1.0",
        "StereoHarm Note",          NTS(MC_Sharm_Note).c_str(),             NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_Note).c_str(),             "0",     "1.0",
        "StereoHarm Chord",         NTS(MC_Sharm_Chord).c_str(),            NTS(EFX_STEREOHARM).c_str(),    NTS(Sharm_Chord).c_str(),            "0",     "1.0",

        "StompBox Level",           NTS(MC_Stomp_Level).c_str(),            NTS(EFX_STOMPBOX).c_str(),      NTS(Stomp_Level).c_str(),            "0",     "1.0",
        "StompBox Gain",            NTS(MC_Stomp_Gain).c_str(),             NTS(EFX_STOMPBOX).c_str(),      NTS(Stomp_Gain).c_str(),             "0",     "1.0",
        "StompBox Bias",            NTS(MC_Stomp_Bias).c_str(),             NTS(EFX_STOMPBOX).c_str(),      NTS(Stomp_Bias).c_str(),             "0",     "1.0",
        "StompBox Mid",             NTS(MC_Stomp_Mid).c_str(),              NTS(EFX_STOMPBOX).c_str(),      NTS(Stomp_Mid).c_str(),              "0",     "1.0",
        "StompBox Tone",            NTS(MC_Stomp_Tone).c_str(),             NTS(EFX_STOMPBOX).c_str(),      NTS(Stomp_Tone).c_str(),             "0",     "1.0",
        "StompBox Mode",            NTS(MC_Stomp_Mode).c_str(),             NTS(EFX_STOMPBOX).c_str(),      NTS(Stomp_Mode).c_str(),             "0",     "1.0",

        "Sustainer Gain",           NTS(MC_Sustain_Gain).c_str(),           NTS(EFX_SUSTAINER).c_str(),     NTS(Sustain_Gain).c_str(),           "0",     "1.0",
        "Sustainer Sustain",        NTS(MC_Sustain_Sustain).c_str(),        NTS(EFX_SUSTAINER).c_str(),     NTS(Sustain_Sustain).c_str(),        "0",     "1.0",

        "Synthfilter Dry/Wet",      NTS(MC_Synthfilter_DryWet).c_str(),     NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_DryWet).c_str(),     "0",     "1.0",
        "Synthfilter Distort",      NTS(MC_Synthfilter_Distort).c_str(),    NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Distort).c_str(),    "0",     "1.0",
        "Synthfilter Tempo",        NTS(MC_Synthfilter_LFO_Tempo).c_str(),  NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_LFO_Tempo).c_str(),  "0",     "1.0",
        "Synthfilter Random",       NTS(MC_Synthfilter_LFO_Random).c_str(), NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_LFO_Random).c_str(), "0",     "1.0",
        "Synthfilter LFO Type",     NTS(MC_Synthfilter_LFO_Type).c_str(),   NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_LFO_Type).c_str(),   "0",     "1.0",
        "Synthfilter Stereo Df",    NTS(MC_Synthfilter_LFO_Stereo).c_str(), NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_LFO_Stereo).c_str(), "0",     "1.0",
        "Synthfilter Width",        NTS(MC_Synthfilter_Width).c_str(),      NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Width).c_str(),      "0",     "1.0",
        "Synthfilter Feedback",     NTS(MC_Synthfilter_Feedback).c_str(),   NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Feedback).c_str(),   "0",     "1.0",
        "Synthfilter Depth",        NTS(MC_Synthfilter_Depth).c_str(),      NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Depth).c_str(),      "0",     "1.0",
        "Synthfilter E.Sens",       NTS(MC_Synthfilter_Env_Sens).c_str(),   NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Env_Sens).c_str(),   "0",     "1.0",
        "Synthfilter A.Time",       NTS(MC_Synthfilter_Attack).c_str(),     NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Attack).c_str(),     "0",     "1.0",
        "Synthfilter R.Time",       NTS(MC_Synthfilter_Release).c_str(),    NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Release).c_str(),    "0",     "1.0",
        "Synthfilter Offset",       NTS(MC_Synthfilter_Offset).c_str(),     NTS(EFX_SYNTHFILTER).c_str(),   NTS(Synthfilter_Offset).c_str(),     "0",     "1.0",

        "Valve Dry/Wet",            NTS(MC_Valve_DryWet).c_str(),           NTS(EFX_VALVE).c_str(),         NTS(Valve_DryWet).c_str(),           "0",     "1.0",
        "Valve L/R Cross",          NTS(MC_Valve_LR_Cross).c_str(),         NTS(EFX_VALVE).c_str(),         NTS(Valve_LR_Cross).c_str(),         "0",     "1.0",
        "Valve Pan",                NTS(MC_Valve_Pan).c_str(),              NTS(EFX_VALVE).c_str(),         NTS(Valve_Pan).c_str(),              "0",     "1.0",
        "Valve Level",              NTS(MC_Valve_Level).c_str(),            NTS(EFX_VALVE).c_str(),         NTS(Valve_Level).c_str(),            "0",     "1.0",
        "Valve Drive",              NTS(MC_Valve_Drive).c_str(),            NTS(EFX_VALVE).c_str(),         NTS(Valve_Drive).c_str(),            "0",     "1.0",
        "Valve Dist",               NTS(MC_Valve_Distortion).c_str(),       NTS(EFX_VALVE).c_str(),         NTS(Valve_Distortion).c_str(),       "0",     "1.0",
        "Valve Presence",           NTS(MC_Valve_Presence).c_str(),         NTS(EFX_VALVE).c_str(),         NTS(Valve_Presence).c_str(),         "0",     "1.0",
        "Valve LPF",                NTS(MC_Valve_LPF).c_str(),              NTS(EFX_VALVE).c_str(),         NTS(Valve_LPF).c_str(),              "0",     "1.0",
        "Valve HPF",                NTS(MC_Valve_HPF).c_str(),              NTS(EFX_VALVE).c_str(),         NTS(Valve_HPF).c_str(),              "0",     "1.0",

        "VaryBand Dry/Wet",         NTS(MC_VaryBand_DryWet).c_str(),        NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_DryWet).c_str(),        "0",     "1.0",
        "VaryBand Tempo 1",         NTS(MC_VaryBand_LFO_Tempo_1).c_str(),   NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_LFO_Tempo_1).c_str(),   "0",     "1.0",
        "VaryBand LFO 1 Type",      NTS(MC_VaryBand_LFO_Type_1).c_str(),    NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_LFO_Type_1).c_str(),    "0",     "1.0",
        "VaryBand St.df 1",         NTS(MC_VaryBand_LFO_Stereo_1).c_str(),  NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_LFO_Stereo_1).c_str(),  "0",     "1.0",
        "VaryBand Tempo 2",         NTS(MC_VaryBand_LFO_Tempo_2).c_str(),   NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_LFO_Tempo_2).c_str(),   "0",     "1.0",
        "VaryBand LFO 2 Type",      NTS(MC_VaryBand_LFO_Type_2).c_str(),    NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_LFO_Type_2).c_str(),    "0",     "1.0",
        "VaryBand St.df 2",         NTS(MC_VaryBand_LFO_Stereo_2).c_str(),  NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_LFO_Stereo_2).c_str(),  "0",     "1.0",
        "VaryBand Cross 1",         NTS(MC_VaryBand_Cross_1).c_str(),       NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_Cross_1).c_str(),       "0",     "1.0",
        "VaryBand Cross 2",         NTS(MC_VaryBand_Cross_2).c_str(),       NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_Cross_2).c_str(),       "0",     "1.0",
        "VaryBand Cross 3",         NTS(MC_VaryBand_Cross_3).c_str(),       NTS(EFX_VARYBAND).c_str(),      NTS(VaryBand_Cross_3).c_str(),       "0",     "1.0",

        "Vibe Dry/Wet",             NTS(MC_Vibe_DryWet).c_str(),            NTS(EFX_VIBE).c_str(),          NTS(Vibe_DryWet).c_str(),            "0",     "1.0",
        "Vibe Width",               NTS(MC_Vibe_Width).c_str(),             NTS(EFX_VIBE).c_str(),          NTS(Vibe_Width).c_str(),             "0",     "1.0",
        "Vibe Depth",               NTS(MC_Vibe_Depth).c_str(),             NTS(EFX_VIBE).c_str(),          NTS(Vibe_Depth).c_str(),             "0",     "1.0",
        "Vibe Tempo",               NTS(MC_Vibe_LFO_Tempo).c_str(),         NTS(EFX_VIBE).c_str(),          NTS(Vibe_LFO_Tempo).c_str(),         "0",     "1.0",
        "Vibe Random",              NTS(MC_Vibe_LFO_Random).c_str(),        NTS(EFX_VIBE).c_str(),          NTS(Vibe_LFO_Random).c_str(),        "0",     "1.0",
        "Vibe LFO Type",            NTS(MC_Vibe_LFO_Type).c_str(),          NTS(EFX_VIBE).c_str(),          NTS(Vibe_LFO_Type).c_str(),          "0",     "1.0",
        "Vibe Stereo Df",           NTS(MC_Vibe_LFO_Stereo).c_str(),        NTS(EFX_VIBE).c_str(),          NTS(Vibe_LFO_Stereo).c_str(),        "0",     "1.0",
        "Vibe Feedback",            NTS(MC_Vibe_Feedback).c_str(),          NTS(EFX_VIBE).c_str(),          NTS(Vibe_Feedback).c_str(),          "0",     "1.0",
        "Vibe L/R Cross",           NTS(MC_Vibe_LR_Cross).c_str(),          NTS(EFX_VIBE).c_str(),          NTS(Vibe_LR_Cross).c_str(),          "0",     "1.0",
        "Vibe Pan",                 NTS(MC_Vibe_Pan).c_str(),               NTS(EFX_VIBE).c_str(),          NTS(Vibe_Pan).c_str(),               "0",     "1.0",

        "Vocoder Dry/Wet",          NTS(MC_Vocoder_DryWet).c_str(),         NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_DryWet).c_str(),         "0",     "1.0",
        "Vocoder Pan",              NTS(MC_Vocoder_Pan).c_str(),            NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_Pan).c_str(),            "0",     "1.0",
        "Vocoder Input",            NTS(MC_Vocoder_Input).c_str(),          NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_Input).c_str(),          "0",     "1.0",
        "Vocoder Smear",            NTS(MC_Vocoder_Smear).c_str(),          NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_Smear).c_str(),          "0",     "1.0",
        "Vocoder Q",                NTS(MC_Vocoder_Q).c_str(),              NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_Q).c_str(),              "0",     "1.0",
        "Vocoder Ring",             NTS(MC_Vocoder_Ring).c_str(),           NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_Ring).c_str(),           "0",     "1.0",
        "Vocoder Level",            NTS(MC_Vocoder_Level).c_str(),          NTS(EFX_VOCODER).c_str(),       NTS(Vocoder_Level).c_str(),          "0",     "1.0",

        "WahWah Dry/Wet",           NTS(MC_WahWah_DryWet).c_str(),          NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_DryWet).c_str(),          "0",     "1.0",
        "WahWah Pan",               NTS(MC_WahWah_Pan).c_str(),             NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_Pan).c_str(),             "0",     "1.0",
        "WahWah Tempo",             NTS(MC_WahWah_LFO_Tempo).c_str(),       NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_LFO_Tempo).c_str(),       "0",     "1.0",
        "WahWah Random",            NTS(MC_WahWah_LFO_Random).c_str(),      NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_LFO_Random).c_str(),      "0",     "1.0",
        "WahWah LFO Type",          NTS(MC_WahWah_LFO_Type).c_str(),        NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_LFO_Type).c_str(),        "0",     "1.0",
        "WahWah Stereo Df",         NTS(MC_WahWah_LFO_Stereo).c_str(),      NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_LFO_Stereo).c_str(),      "0",     "1.0",
        "WahWah Depth",             NTS(MC_WahWah_Depth).c_str(),           NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_Depth).c_str(),           "0",     "1.0",
        "WahWah Amp S.",            NTS(MC_WahWah_Sense).c_str(),           NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_Sense).c_str(),           "0",     "1.0",
        "WahWah Amp S.I.",          NTS(MC_WahWah_ASI).c_str(),             NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_ASI).c_str(),             "0",     "1.0",
        "WahWah Smooth",            NTS(MC_WahWah_Smooth).c_str(),          NTS(EFX_WAHWAH).c_str(),        NTS(WahWah_Smooth).c_str(),          "0",     "1.0",
    };

    // If any additional parameters are added, then the constant
    // C_MC_PARAMETER_SIZE must be adjusted.
    for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
    {
        strcpy(mc_efx_params[i].Description, los_params[i * 6]);
        sscanf(los_params[i * 6 + 1], "%d", &mc_efx_params[i].MC_params_index);
        sscanf(los_params[i * 6 + 2], "%d", &mc_efx_params[i].Effect_index);
        sscanf(los_params[i * 6 + 3], "%d", &mc_efx_params[i].Efx_param_index);
        sscanf(los_params[i * 6 + 4], "%d", &mc_efx_params[i].MC_offset);
        sscanf(los_params[i * 6 + 5], "%lf", &mc_efx_params[i].MC_range);
    }
}

/**
 *  Get MIDI connection information from user preferences.
 *  If auto connect is requested, then make the connections.
 */
void
RKR::ConnectMIDI()
{
    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    rakarrack.get(PrefNom("Auto Connect MIDI IN"), aconnect_MI, 0);
    rakarrack.get(PrefNom("MIDI IN Device"), MID, "", 40);
    
    if (aconnect_MI)
        Conecta();
}

void
RKR::InitMIDI()
{
    // Open Alsa Seq
    int err = snd_seq_open(&midi_in, "default", SND_SEQ_OPEN_INPUT, 0);
    if (err < 0)
        printf("Cannot activate ALSA seq client\n");
    snd_seq_set_client_name(midi_in, jackcliname);
    snd_config_update_free_global();

    char portname[70];

    // Create Alsa Seq Client
    sprintf(portname, "%s IN", jackcliname);
    snd_seq_create_simple_port(midi_in, portname,
                               SND_SEQ_PORT_CAP_WRITE |
                               SND_SEQ_PORT_CAP_SUBS_WRITE,
                               SND_SEQ_PORT_TYPE_SYNTH);

}

void
RKR::miramidi()
{
    if (snd_seq_event_input_pending(midi_in, 1))
    {
        do
        {
            midievents();
        }
        while (snd_seq_event_input_pending(midi_in, 0));
    }
}

void
RKR::midievents()
{
    int i;
    snd_seq_event_t *midievent;
    midievent = NULL;
    snd_seq_event_input(midi_in, &midievent);
    
    if (midievent == NULL)
        return;
    
    if (midievent->type == 42)
        return;

    if ((Tap_Bypass) && (Tap_Selection == 3) && (midievent->type == SND_SEQ_EVENT_CLOCK))
    {
        mtc_counter++;
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    if ((EFX_Bypass[EFX_LOOPER]) && (Tap_Selection == 3))
    {
        if (midievent->type == SND_SEQ_EVENT_START)
        {
            Rack_Effects[EFX_LOOPER]->changepar(Looper_Play, 1);
            Gui_Refresh = GUI_Refresh_Looper;
        }

        if (midievent->type == SND_SEQ_EVENT_STOP)
        {
            Rack_Effects[EFX_LOOPER]->changepar(Looper_Stop, 1);
            Gui_Refresh = GUI_Refresh_Looper;
        }
    }

    if ((midievent->type == SND_SEQ_EVENT_NOTEON)
        || (midievent->type == SND_SEQ_EVENT_NOTEOFF))
    {
        int cmdnote = midievent->data.note.note;
        int cmdvelo = midievent->data.note.velocity;

        if ((Tap_Bypass) && (Tap_Selection == 1) && (midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
            Tap_TempoSet = TapTempo();

        if (midievent->data.note.channel == HarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
                {
                    if (RC_Harm->note_active[i] == 0)
                    {
                        RC_Harm->note_active[i] = 1;
                        RC_Harm->rnote[i] = cmdnote;
                        RC_Harm->gate[i] = 1;
                        RC_Harm->MiraChord();
                        break;
                    }
                }

                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }

        if (midievent->data.note.channel == StereoHarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
                {
                    if (RC_Stereo_Harm->note_active[i] == 0)
                    {
                        RC_Stereo_Harm->note_active[i] = 1;
                        RC_Stereo_Harm->rnote[i] = cmdnote;
                        RC_Stereo_Harm->gate[i] = 1;
                        RC_Stereo_Harm->MiraChord();
                        break;
                    }
                }

                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }
    }


    if (midievent->type == SND_SEQ_EVENT_PGMCHANGE)
    {
        if (midievent->data.control.channel == MidiCh)
        {
            if (!midi_table)
            {
                if ((midievent->data.control.value > 0)
                    && (midievent->data.control.value < 61))
                    Change_Preset = midievent->data.control.value;

                if (midievent->data.control.value == 81)
                    if (Selected_Preset > 1) Change_Preset = Selected_Preset - 1;

                if (midievent->data.control.value == 82)
                    if (Selected_Preset < 60) Change_Preset = Selected_Preset + 1;
            }
            else
                Change_Preset = midievent->data.control.value;
        }
    }

    if (midievent->type == SND_SEQ_EVENT_CONTROLLER)
    {
        if (midievent->data.control.channel == MidiCh)
        {
            if (RControl)
            {
                ControlGet = (int) midievent->data.control.param;
                return;
            }

            if (MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (XUserMIDI[(int) midievent->data.control.param][i])
                        process_midi_controller_events(XUserMIDI[(int) midievent->data.control.param][i],
                                                       (int) midievent->data.control.value);
                    else break;
                }
            }
            else
                process_midi_controller_events((int) midievent->data.control.param,
                                               (int) midievent->data.control.value);
        }
    }
}

void
RKR::ActOnOff()
{
    if (OnOffC < 63) OnOffC++;
}

void
RKR::ActiveUn(int value)
{
    int numef;
    int inoff = 0;
    int miraque = 0;

    if (value < 20)
    {
        numef = value / 2;
        inoff = checkonoff(efx_order[numef]); // value % 2;
        miraque = efx_order[numef];
        ActOnOff();
        Mnumeff[OnOffC] = numef;
    }
    else if (value < 121)
    {
        numef = value - 20;
        inoff = checkonoff(numef);
        miraque = numef;
        ActOnOff();
        Mnumeff[OnOffC] = 1000 + numef;
    }
    else
    {
        numef = value;
        inoff = checkonoff(numef);
        miraque = numef;
        ActOnOff();
        Mnumeff[OnOffC] = 2000 + numef;
    }
    
    if(miraque < C_NUMBER_EFFECTS)
    {
        for(int i = 0; i < C_NUMBER_EFFECTS; i++)
        {
            if(miraque == i)
            {
                if (inoff) EFX_Bypass[i] = 1;
                else EFX_Bypass[i] = 0;
                return;
            }
        }
    }
    else
    {
        switch (miraque)
        {
            case EFX_TAP_TEMPO_ON_OFF:
                if (inoff) Tap_Bypass = 1;
                else Tap_Bypass = 0;
                break;
            case EFX_MIDI_CONVERTER_ON_OFF:
                if (inoff) MIDIConverter_Bypass = 1;
                else MIDIConverter_Bypass = 0;
                break;
            case EFX_TUNER_ON_OFF:
                if (inoff) Tuner_Bypass = 1;
                else Tuner_Bypass = 0;
                break;
            case EFX_MASTER_ON_OFF:
                if (inoff) Bypass = 1;
                else Bypass = 0;
            break;
        }
    }
}

int
RKR::checkonoff(int miraque)
{
    if(miraque < C_NUMBER_EFFECTS)
    {
        for(int i = 0; i < C_NUMBER_EFFECTS; i++)
        {
            if(miraque == i)
            {
                if (EFX_Bypass[i])
                {
                    return (0);
                }
                else
                    return (1);
            }
        }
    }
    else
    {
        switch (miraque)
        {
            case EFX_TAP_TEMPO_ON_OFF:
                if (Tap_Bypass) return 0;
                break;
            case EFX_MIDI_CONVERTER_ON_OFF:
                if (MIDIConverter_Bypass) return 0;
                break;
            case EFX_TUNER_ON_OFF:
                if (Tuner_Bypass) return 0;
                break;
            case EFX_MASTER_ON_OFF:
                if (Bypass) return 0;
            break;
        }
    }

    return (1);
}

void
RKR::Conecta()
{
    FILE *fp;

    int client = 0;
    int puerto = 0;
    char temp[128];
    char temp1[128];
    char temp2[128];
    char *nume;

    if (IsCoIn)
        disconectaaconnect();

    if ((fp = fopen("/proc/asound/seq/clients", "r")) != NULL)
    {
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "Client") != NULL)
            {
                strcpy(temp1, temp);
                strtok(temp1, " ");
                nume = strtok(NULL, "\"");
                sscanf(nume, "%d", &client);
            }

            if (strstr(temp, "Port") != NULL)
            {
                strcpy(temp2, temp);
                strtok(temp2, " ");
                nume = strtok(NULL, "  ");
                sscanf(nume, "%d", &puerto);
                
                if (strstr(temp, "rakarrack IN") != 0)
                {
                    Cyoin = client;
                    Pyoin = puerto;
                }
                
                if (strstr(temp, MID) != 0)
                {
                    Ccin = client;
                    Pcin = puerto;
                }
            }
        }
    }

    fclose(fp);
    conectaaconnect();
};

void
RKR::conectaaconnect()
{
    char tempi[128];

    if (MID != NULL)
    {
        memset(tempi, 0, sizeof (tempi));
        sprintf(tempi, "aconnect %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        
        if (system(tempi) == -1)
            fl_alert("Error running aconnect!");
        else
            IsCoIn = 1;
    }
}

void
RKR::disconectaaconnect()
{
    char tempi[128];

    if (MID != NULL)
    {
        memset(tempi, 0, sizeof (tempi));
        sprintf(tempi, "aconnect -d %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        if (system(tempi) == -1)
            fl_alert("Error running aconnect!");
        else
            IsCoIn = 0;
    }
}

void
RKR::jack_process_midievents(jack_midi_event_t *midievent)
{
    int i;
    int type = midievent->buffer[0] >> 4;

    if ((Tap_Bypass) && (Tap_Selection == 3) && (midievent->buffer[0] == 0xf8))
    {
        mtc_counter++;
        
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    if ((type == 8) || (type == 9))
    {
        int cmdnote = midievent->buffer[1];
        int cmdvelo = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if ((Tap_Bypass) && (Tap_Selection == 1) && (type == 9) && (cmdvelo != 0)) Tap_TempoSet = TapTempo();

        if (cmdchan == HarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((type == 9) && (cmdvelo != 0))
                {
                    if (RC_Harm->note_active[i] == 0)
                    {
                        RC_Harm->note_active[i] = 1;
                        RC_Harm->rnote[i] = cmdnote;
                        RC_Harm->gate[i] = 1;
                        RC_Harm->MiraChord();
                        break;
                    }
                }

                if ((type == 9) && (cmdvelo == 0))
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (type == 8)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }

        if (cmdchan == StereoHarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((type == 9) && (cmdvelo != 0))
                {
                    if (RC_Stereo_Harm->note_active[i] == 0)
                    {
                        RC_Stereo_Harm->note_active[i] = 1;
                        RC_Stereo_Harm->rnote[i] = cmdnote;
                        RC_Stereo_Harm->gate[i] = 1;
                        RC_Stereo_Harm->MiraChord();
                        break;
                    }
                }

                if ((type == 9) && (cmdvelo == 0))
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (type == 8)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }
    }

    if (type == 12)
    {
        int cmdvalue = midievent->buffer[1];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == MidiCh)
        {
            if (!midi_table)
            {
                if ((cmdvalue > 0)
                    && (cmdvalue < 61))
                    Change_Preset = cmdvalue;

                if (cmdvalue == 81) if (Selected_Preset > 1) Change_Preset = Selected_Preset - 1;
                
                if (cmdvalue == 82) if (Selected_Preset < 60) Change_Preset = Selected_Preset + 1;
            }
            else
                Change_Preset = cmdvalue;
        }
    }

    if (type == 11)
    {
        int cmdcontrol = midievent->buffer[1];
        int cmdvalue = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == MidiCh)
        {
            if (RControl)
            {
                ControlGet = cmdcontrol;
                return;
            }

            if (MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (XUserMIDI[cmdcontrol][i])
                        process_midi_controller_events(XUserMIDI[cmdcontrol][i], cmdvalue);
                    else break;
                }
            }
            else
                process_midi_controller_events(cmdcontrol, cmdvalue);
        }
    }
}

/*
 * process MIDI controller events
 */
void
RKR::process_midi_controller_events(int parameter, int value)
{
    // Don't process MIDI control when updating quality since
    // the efx may be deleted
    if(quality_update)
        return;
    
    // Flags used for Gui update from MIDI control - used by RKRGUI::MIDI_control_gui_refresh()
    if (parameter > 0)
    {
        Mcontrol[parameter] = 1;    // The parameter number that needs updating
        Mvalue = 1;                 // Flag to indicate at least one parameter needs update
    }

    switch (parameter)
    {
    case MC_Unused_0:
        break;
        
// Start MIDI Implementation Chart range

    case MC_WahWah_Depth:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_Depth, value);
        break;

    case MC_Dere_Drive:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_Drive, value);
        break;

    case MC_Dere_Level:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_Level, value);
        break;

    case MC_Dere_LPF:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_LPF, ret_LPF(value));
        break;

    case MC_Dere_HPF:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_HPF, ret_HPF(value));
        break;

    case MC_Dere_Color:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_Color, value);
        break;

    case MC_Output_Volume:
        Master_Volume = (float) value / 127.0f;
        calculavol(2);
        break;

    case MC_Dere_Suboctave:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_Suboctave, value);
        break;

    case MC_Dist_Suboctave:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_Suboctave, value);
        break;

    case MC_Unused_10:
    case MC_Unused_11:
        break;

    case MC_Balance_FX:
        Fraction_Bypass = (float) value / 127.0f;
        break;

    case MC_Unused_13:
        break;

    case MC_Input_Volume:
        Input_Gain = (float) value / 127.0f;
        calculavol(1);
        break;

    case MC_Unused_15:
    case MC_Unused_16:
    case MC_Unused_17:
    case MC_Unused_18:
    case MC_Unused_19:
        break;
                        
    case MC_Alien_Depth:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_Depth, value);
        break;

    case MC_Phaser_Depth:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_Depth, value);
        break;

    case MC_Flanger_Depth:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_Depth, value);
        break;

    case MC_Chorus_Depth:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_Depth, value);
        break;

    case MC_Music_Gain_1:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Gain_1, value);
        break;

    case MC_Music_Gain_2:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Gain_2, value);
        break;

    case MC_Harm_Filter_Freq:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Filter_Freq, ret_LPF(value));
        break;

    case MC_Harm_Interval:
        if (EFX_Bypass[EFX_HARMONIZER])
        {
            EFX_Bypass[EFX_HARMONIZER] = 0;
            Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Interval, (int) ((float) value * C_MC_24_RANGE));
            EFX_Bypass[EFX_HARMONIZER] = 1;
        }
        break;

    case MC_WahWah_DryWet:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_DryWet, Dry_Wet(value));
        break;

    case MC_Overdrive_DryWet:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_DryWet, Dry_Wet(value));
        break;

    case MC_Dist_DryWet:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_DryWet, Dry_Wet(value));
        break;

    case MC_Harm_DryWet:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_DryWet, Dry_Wet(value));
        break;

    case MC_Unused_32:
    case MC_Unused_33:
    case MC_Unused_34:
    case MC_Unused_35:
    case MC_Unused_36:
    case MC_Unused_37:
    case MC_Unused_38:
    case MC_Unused_39:
    case MC_Unused_40:
    case MC_Unused_41:
    case MC_Unused_42:
    case MC_Unused_43:
    case MC_Unused_44:
    case MC_Unused_45:
        break;

    case MC_Echo_Pan:
        Rack_Effects[EFX_ECHO]->changepar(Echo_Pan, value);
        break;

    case MC_Overdrive_Pan:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_Pan, value);
        break;

    case MC_Dist_Pan:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_Pan, value);
        break;

    case MC_Harm_Pan:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Pan, value);
        break;

    case MC_Chorus_Pan:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_Pan, value);
        break;

    case MC_Flanger_Pan:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_Pan, value);
        break;

    case MC_Chorus_DryWet:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_DryWet, Dry_Wet(value));
        break;

    case MC_Flanger_DryWet:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_DryWet, Dry_Wet(value));
        break;

    case MC_Phaser_DryWet:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_DryWet, Dry_Wet(value));
        break;

    case MC_Alien_DryWet:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_DryWet, Dry_Wet(value));
        break;

    case MC_Music_DryWet:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_DryWet, Dry_Wet(value));
        break;

    case MC_Reverb_DryWet:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_DryWet, Dry_Wet(value));
        break;

    case MC_Pan_DryWet:
        Rack_Effects[EFX_PAN]->changepar(Pan_DryWet, Dry_Wet(value));
        break;

    case MC_Echo_DryWet:
        Rack_Effects[EFX_ECHO]->changepar(Echo_DryWet, Dry_Wet(value));
        break;

    case MC_Phaser_Pan:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_Pan, value);
        break;

    case MC_Alien_Pan:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_Pan, value);
        break;

    case MC_Music_Pan_1:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Pan_1, value);
        break;

    case MC_Reverb_Pan:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_Pan, value);
        break;

    case MC_Unused_64:
        break;

    case MC_Music_Pan_2:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Pan_2, value);
        break;

    case MC_WahWah_Pan:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_Pan, value);
        break;

    case MC_Pan_Pan:
        Rack_Effects[EFX_PAN]->changepar(Pan_Pan, value);
        break;

    case MC_Overdrive_Drive:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_Drive, value);
        break;

    case MC_Dist_Drive:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_Drive, value);
        break;

    case MC_Overdrive_Level:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_Level, value);
        break;

    case MC_Dist_Level:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_Level, value);
        break;

    case MC_Chorus_LFO_Tempo:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Flanger_LFO_Tempo:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Phaser_LFO_Tempo:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_WahWah_LFO_Tempo:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Alien_LFO_Tempo:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Pan_LFO_Tempo:
        Rack_Effects[EFX_PAN]->changepar(Pan_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Echo_Feedback:
        Rack_Effects[EFX_ECHO]->changepar(Echo_Feedback, value);
        break;

    case MC_Chorus_Feedback:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_Feedback, value);
        break;

    case MC_Flanger_Feedback:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_Feedback, value);
        break;

    case MC_Phaser_Feedback:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_Feedback, value);
        break;

    case MC_Alien_Feedback:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_Feedback, value);
        break;

    case MC_Music_Feedback_1:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Feedback_1, value);
        break;

    case MC_Music_Feedback_2:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Feedback_2, value);
        break;

    case MC_Overdrive_LPF:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_LPF, ret_LPF(value));
        break;

    case MC_Dist_LPF:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_LPF, ret_LPF(value));
        break;

    case MC_Reverb_LPF:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_LPF, ret_LPF(value));
        break;

    case MC_Overdrive_HPF:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_HPF, ret_HPF(value));
        break;

    case MC_Dist_HPF:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_HPF, ret_HPF(value));
        break;

    case MC_Reverb_HPF:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_HPF, ret_HPF(value));
        break;

    case MC_Chorus_LR_Cross:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_LR_Cross, value);
        break;

    case MC_Flanger_LR_Cross:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_LR_Cross, value);
        break;

    case MC_Phaser_LR_Cross:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_LR_Cross, value);
        break;

    case MC_Overdrive_LR_Cross:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_LR_Cross, value);
        break;

    case MC_Dist_LR_Cross:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_LR_Cross, value);
        break;

    case MC_Alien_LR_Cross:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_LR_Cross, value);
        break;

    case MC_Echo_LR_Cross:
        Rack_Effects[EFX_ECHO]->changepar(Echo_LR_Cross, value);
        break;

    case MC_Music_LR_Cross:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_LR_Cross, value);
        break;

    case MC_Chorus_LFO_Stereo:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_LFO_Stereo, value);
        break;

    case MC_Flanger_LFO_Stereo:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_LFO_Stereo, value);
        break;

    case MC_Phaser_LFO_Stereo:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_LFO_Stereo, value);
        break;

    case MC_WahWah_LFO_Stereo:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_LFO_Stereo, value);
        break;

    case MC_Alien_LFO_Stereo:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_LFO_Stereo, value);
        break;

    case MC_Pan_LFO_Stereo:
        Rack_Effects[EFX_PAN]->changepar(Pan_LFO_Stereo, value);
        break;

    case MC_Chorus_LFO_Random:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_LFO_Random, value);
        break;

    case MC_Flanger_LFO_Random:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_LFO_Random, value);
        break;

    case MC_Phaser_LFO_Random:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_LFO_Random, value);
        break;

    case MC_WahWah_LFO_Random:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_LFO_Random, value);
        break;

    case MC_Alien_LFO_Random:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_LFO_Random, value);
        break;

    case MC_Pan_LFO_Random:
        Rack_Effects[EFX_PAN]->changepar(Pan_LFO_Random, value);
        break;

    case MC_WahWah_Sense:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_Sense, value);
        break;

    case MC_WahWah_ASI:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_ASI, value);
        break;

    case MC_WahWah_Smooth:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_Smooth, value);
        break;

    case MC_Phaser_Phase:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_Phase, value);
        break;

    case MC_Alien_Phase:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_Phase, value);
        break;

    case MC_Multi_On_Off:
        ActiveUn(value);
        break;

    case MC_APhase_DryWet:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_DryWet, Dry_Wet(value));
        break;

    case MC_APhase_Distortion:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Distortion, value);
        break;

    case MC_APhase_LFO_Tempo:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_APhase_Depth:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Depth, value);
        break;

    case MC_APhase_Width:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Width, value);
        break;

    case MC_APhase_Feedback:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Feedback, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_APhase_Mismatch:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Mismatch, value);
        break;

    case MC_APhase_LFO_Stereo:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Stereo, value);
        break;

    case MC_Dere_DryWet:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_DryWet, Dry_Wet(value));
        break;

    case MC_Dere_Pan:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_Pan, value);
        break;

    case MC_Dere_LR_Cross:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_LR_Cross, value);
        break;

// End MIDI Implementation Chart range
// Start of MIDI learn extras

    case MC_Unused_128:
    case MC_Unused_129:
        break;

    case MC_EQ_Gain:
        Rack_Effects[EFX_EQ]->changepar(EQ_Gain, value);
        break;

    case MC_EQ_Q:
    {
        Rack_Effects[EFX_EQ]->changepar(EQ_Q, value);
        break;
    }
    case MC_EQ_31_HZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_31_HZ, value);
        break;

    case MC_EQ_63_HZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_63_HZ, value);
        break;

    case MC_EQ_125_HZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_125_HZ, value);
        break;

    case MC_EQ_250_HZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_250_HZ, value);
        break;

    case MC_EQ_500_HZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_500_HZ, value);
        break;

    case MC_EQ_1_KHZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_1_KHZ, value);
        break;

    case MC_EQ_2_KHZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_2_KHZ, value);
        break;

    case MC_EQ_4_KHZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_4_KHZ, value);
        break;

    case MC_EQ_8_KHZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_8_KHZ, value);
        break;

    case MC_EQ_16_KHZ:
        Rack_Effects[EFX_EQ]->changepar(EQ_16_KHZ, value);
        break;

    case MC_Compress_Attack:
        Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Attack, 10 + (int) ((float) value * C_MC_240_RANGE));
        break;

    case MC_Compress_Release:
        Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Release, 10 + (int) ((float) value * C_MC_490_RANGE));
        break;

    case MC_Compress_Ratio:
        Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_Compress_Knee:
        Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Knee, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Compress_Threshold:
        Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Threshold, -60 + (int) ((float) value * C_MC_57_RANGE));
        break;

    case MC_Compress_Output:
        Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Output, -40 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_Parametric_Gain:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Gain, value);
        break;

    case MC_Parametric_Low_Freq:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Low_Freq, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_Parametric_Low_Gain:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Low_Gain, value);
        break;

    case MC_Parametric_Low_Q:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Low_Q, value);
        break;

    case MC_Parametric_Mid_Freq:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Mid_Freq, 800 + (int) ((float) value * C_MC_7200_RANGE));
        break;

    case MC_Parametric_Mid_Gain:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Mid_Gain, value);
        break;

    case MC_Parametric_Mid_Q:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Mid_Q, value);
        break;

    case MC_Parametric_High_Freq:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_High_Freq, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case MC_Parametric_High_Gain:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_High_Gain, value);
        break;

    case MC_Parametric_High_Q:
        Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_High_Q, value);
        break;

    case MC_DFlange_DryWet:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_DryWet, Dry_Wet(value));
        break;

    case MC_DFlange_Pan:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_DFlange_LR_Cross:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LR_Cross, value);
        break;

    case MC_DFlange_Depth:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_Depth, 20 + (int) ((float) value * C_MC_2480_RANGE));
        break;

    case MC_DFlange_Width:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_Width, (int) ((float) value * C_MC_6000_RANGE));
        break;

    case MC_DFlange_Offset:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_Offset, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DFlange_Feedback:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_DFlange_LPF:
        /* This is labeled LPF but uses same range as HPF - FIXME check DSP */
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LPF, 20 + (int) ((float) value * C_MC_19980_RANGE));
        break;

    case MC_DFlange_LFO_Tempo:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_DFlange_LFO_Stereo:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LFO_Stereo, value);
        break;

    case MC_DFlange_LFO_Random:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LFO_Random, value);
        break;

    case MC_Valve_DryWet:
        Rack_Effects[EFX_VALVE]->changepar(Valve_DryWet, Dry_Wet(value));
        break;

    case MC_Valve_LR_Cross:
        Rack_Effects[EFX_VALVE]->changepar(Valve_LR_Cross, value);
        break;

    case MC_Valve_Pan:
        Rack_Effects[EFX_VALVE]->changepar(Valve_Pan, value);
        break;

    case MC_Valve_Level:
        Rack_Effects[EFX_VALVE]->changepar(Valve_Level, value);
        break;

    case MC_Valve_Drive:
        Rack_Effects[EFX_VALVE]->changepar(Valve_Drive, value);
        break;

    case MC_Valve_Distortion:
        Rack_Effects[EFX_VALVE]->changepar(Valve_Distortion, value);
        break;

    case MC_Valve_Presence:
        Rack_Effects[EFX_VALVE]->changepar(Valve_Presence, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Valve_LPF:
        Rack_Effects[EFX_VALVE]->changepar(Valve_LPF, ret_LPF(value));
        break;

    case MC_Valve_HPF:
        Rack_Effects[EFX_VALVE]->changepar(Valve_HPF, ret_HPF(value));
        break;

    case MC_Ring_DryWet:
        Rack_Effects[EFX_RING]->changepar(Ring_DryWet, Dry_Wet(value));
        break;

    case MC_Ring_LR_Cross:
        Rack_Effects[EFX_RING]->changepar(Ring_LR_Cross, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Ring_Input:
        Rack_Effects[EFX_RING]->changepar(Ring_Input, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Ring_Level:
        Rack_Effects[EFX_RING]->changepar(Ring_Level, value);
        break;

    case MC_Ring_Pan:
        Rack_Effects[EFX_RING]->changepar(Ring_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Ring_Depth:
        Rack_Effects[EFX_RING]->changepar(Ring_Depth, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Freq:
        Rack_Effects[EFX_RING]->changepar(Ring_Freq, 1 + (int) ((float) value * C_MC_19999_RANGE));
        break;

    case MC_Ring_Sine:
        Rack_Effects[EFX_RING]->changepar(Ring_Sine, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Triangle:
        Rack_Effects[EFX_RING]->changepar(Ring_Triangle, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Saw:
        Rack_Effects[EFX_RING]->changepar(Ring_Saw, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Square:
        Rack_Effects[EFX_RING]->changepar(Ring_Square, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Exciter_Gain:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Gain, value);
        break;

    case MC_Exciter_LPF:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_LPF, ret_LPF(value));
        break;

    case MC_Exciter_HPF:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_HPF, ret_HPF(value));
        break;

    case MC_Exciter_Harm_1:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_1, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_2:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_2, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_3:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_3, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_4:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_4, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_5:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_5, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_6:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_6, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_7:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_7, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_8:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_8, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_9:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_9, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_10:
        Rack_Effects[EFX_EXCITER]->changepar(Exciter_Harm_10, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_DistBand_DryWet:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_DryWet, Dry_Wet(value));
        break;

    case MC_DistBand_LR_Cross:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_LR_Cross, value);
        break;

    case MC_DistBand_Drive:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Drive, value);
        break;

    case MC_DistBand_Level:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Level, value);
        break;

    case MC_DistBand_Gain_Low:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Gain_Low, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DistBand_Gain_Mid:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Gain_Mid, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DistBand_Gain_Hi:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Gain_Hi, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DistBand_Cross_1:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_DistBand_Cross_2:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Cross_2, 800 + (int) ((float) value * C_MC_11200_RANGE));
        break;

    case MC_DistBand_Pan:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Pan, value);
        break;

    case MC_Arpie_DryWet:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_DryWet, Dry_Wet(value));
        break;

    case MC_Arpie_ArpeWD:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_ArpeWD, value);
        break;

    case MC_Arpie_Pan:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_Pan, value);
        break;

    case MC_Arpie_Tempo:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_Tempo, ret_Tempo(value));
        break;

    case MC_Arpie_LR_Delay:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_LR_Delay, value);
        break;

    case MC_Arpie_LR_Cross:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_LR_Cross, value);
        break;

    case MC_Arpie_Feedback:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_Feedback, value);
        break;

    case MC_Arpie_Damp:
        Rack_Effects[EFX_ARPIE]->changepar(Arpie_Damp, value);
        break;

    case MC_Expander_Attack:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_Attack, 1 + (int) ((float) value * C_MC_4999_RANGE));
        break;

    case MC_Expander_Release:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_Release, 10 + (int) ((float) value * C_MC_990_RANGE));
        break;

    case MC_Expander_Shape:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_Shape, 1 + (int) ((float) value * C_MC_49_RANGE));
        break;

    case MC_Expander_Threshold:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_Threshold, (int) ((float) value * -C_MC_80_RANGE));
        break;

    case MC_Expander_Gain:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_Gain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Expander_LPF:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_LPF, ret_LPF(value));
        break;

    case MC_Expander_HPF:
        Rack_Effects[EFX_EXPANDER]->changepar(Expander_HPF, ret_HPF(value));
        break;

    case MC_Shuffle_DryWet:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_DryWet, Dry_Wet(value));
        break;

    case MC_Shuffle_Freq_L:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Freq_L, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_Shuffle_Gain_L:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Gain_L, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Freq_ML:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Freq_ML, 400 + (int) ((float) value * C_MC_3600_RANGE));
        break;

    case MC_Shuffle_Gain_ML:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Gain_ML, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Freq_MH:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Freq_MH, 1200 + (int) ((float) value * C_MC_6800_RANGE));
        break;

    case MC_Shuffle_Gain_MH:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Gain_MH, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Freq_H:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Freq_H, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case MC_Shuffle_Gain_H:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Gain_H, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Width:
        Rack_Effects[EFX_SHUFFLE]->changepar(Shuffle_Width, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Synthfilter_DryWet:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_DryWet, Dry_Wet(value));
        break;

    case MC_Synthfilter_Distort:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Distort, value);
        break;

    case MC_Synthfilter_LFO_Tempo:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Synthfilter_LFO_Stereo:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_LFO_Stereo, value);
        break;

    case MC_Synthfilter_Width:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Width, value);
        break;

    case MC_Synthfilter_Feedback:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Feedback, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Synthfilter_Depth:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Depth, value);
        break;

    case MC_Synthfilter_Env_Sens:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Env_Sens, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Synthfilter_Attack:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Attack, 5 + (int) ((float) value * C_MC_995_RANGE));
        break;

    case MC_Synthfilter_Release:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Release, 5 + (int) ((float) value * C_MC_495_RANGE));
        break;

    case MC_Synthfilter_Offset:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_Offset, value);
        break;

    case MC_VaryBand_DryWet:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_DryWet, Dry_Wet(value));
        break;

    case MC_VaryBand_LFO_Tempo_1:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Tempo_1, ret_Tempo(value));
        break;

    case MC_VaryBand_LFO_Stereo_1:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Stereo_1, value);
        break;

    case MC_VaryBand_LFO_Tempo_2:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Tempo_2, ret_Tempo(value));
        break;

    case MC_VaryBand_LFO_Stereo_2:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Stereo_2, value);
        break;

    case MC_VaryBand_Cross_1:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_VaryBand_Cross_2:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case MC_VaryBand_Cross_3:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case MC_MuTro_DryWet:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_DryWet, Dry_Wet(value));
        break;

    case MC_MuTro_LowPass:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_LowPass, value - 64);
        break;

    case MC_MuTro_BandPass:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_BandPass, value - 64);
        break;

    case MC_MuTro_HighPass:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_HighPass, value - 64);
        break;

    case MC_MuTro_Depth:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_Depth, value);
        break;

    case MC_MuTro_LFO_Tempo:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_MuTro_Resonance:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_Resonance, value);
        break;

    case MC_MuTro_Range:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_Range, 10 + (int) ((float) value * C_MC_5990_RANGE));
        break;

    case MC_MuTro_Wah:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_Wah, value);
        break;

    case MC_MuTro_Env_Sens:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_Env_Sens, value - 64);
        break;

    case MC_MuTro_Env_Smooth:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_Env_Smooth, value);
        break;

    case MC_Looper_DryWet:
        Rack_Effects[EFX_LOOPER]->changepar(Looper_DryWet, Dry_Wet(value));
        break;

    case MC_Looper_Level_1:
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Level_1, value);
        break;

    case MC_Looper_Level_2:
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Level_2, value);
        break;

    case MC_Looper_Reverse:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Reverse, i);
        break;
    }
    case MC_Looper_AutoPlay:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_AutoPlay, i);
        break;
    }
    case MC_Looper_Play:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Play, i);
        break;
    }
    case MC_Looper_Stop:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Stop, i);
        break;
    }
    case MC_Looper_Record:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Record, i);
        break;
    }
    case MC_Looper_Rec_1:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Rec_1, i);
        break;
    }
    case MC_Looper_Rec_2:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Rec_2, i);
        break;
    }
    case MC_Looper_Track_1:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Track_1, i);
        break;
    }
    case MC_Looper_Track_2:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Track_2, i);
        break;
    }
    case MC_Looper_Clear:
    {
        int i = 0;
        if (value) i = 1;
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Clear, i);
        break;
    }
    case MC_Convo_DryWet:
        Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_DryWet, Dry_Wet(value));
        break;

    case MC_Convo_Pan:
        Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Pan, value);
        break;

    case MC_Convo_Level:
        Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Level, value);
        break;

    case MC_Convo_Damp:
        Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Damp, value);
        break;

    case MC_Convo_Feedback:
        Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Convo_Length:
        Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Length, 5 + (int) ((float) value * C_MC_245_RANGE));
        break;

    case MC_Coil_Gain:
        Rack_Effects[EFX_COILCRAFTER]->changepar(Coil_Gain, value);
        break;

    case MC_Coil_Tone:
        Rack_Effects[EFX_COILCRAFTER]->changepar(Coil_Tone, 20 + (int) ((float) value * C_MC_4380_RANGE));
        break;

    case MC_Coil_Freq_1:
        Rack_Effects[EFX_COILCRAFTER]->changepar(Coil_Freq_1, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case MC_Coil_Q_1:
        Rack_Effects[EFX_COILCRAFTER]->changepar(Coil_Q_1, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case MC_Coil_Freq_2:
        Rack_Effects[EFX_COILCRAFTER]->changepar(Coil_Freq_2, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case MC_Coil_Q_2:
        Rack_Effects[EFX_COILCRAFTER]->changepar(Coil_Q_2, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case MC_Shelf_Gain:
        Rack_Effects[EFX_SHELFBOOST]->changepar(Shelf_Gain, value);
        break;

    case MC_Shelf_Level:
        Rack_Effects[EFX_SHELFBOOST]->changepar(Shelf_Level, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Shelf_Tone:
        Rack_Effects[EFX_SHELFBOOST]->changepar(Shelf_Tone, 220 + (int) ((float) value * C_MC_15780_RANGE));
        break;

    case MC_Shelf_Presence:
        Rack_Effects[EFX_SHELFBOOST]->changepar(Shelf_Presence, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Vocoder_DryWet:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_DryWet, Dry_Wet(value));
        break;

    case MC_Vocoder_Pan:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_Pan, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Vocoder_Input:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_Input, value);
        break;

    case MC_Vocoder_Smear:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_Smear, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Vocoder_Q:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_Q, 40 + (int) ((float) value * C_MC_130_RANGE));
        break;

    case MC_Vocoder_Ring:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_Ring, value);
        break;

    case MC_Vocoder_Level:
        Rack_Effects[EFX_VOCODER]->changepar(Vocoder_Level, value);
        break;

    case MC_Echoverse_DryWet:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_DryWet, Dry_Wet(value));
        break;

    case MC_Echoverse_Reverse:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Reverse, value);
        break;

    case MC_Echoverse_Pan:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Pan, value);
        break;

    case MC_Echoverse_Tempo:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Tempo, ret_Tempo(value));
        break;

    case MC_Echoverse_LR_Delay:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_LR_Delay, value);
        break;

    case MC_Echoverse_Feedback:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Feedback, value);
        break;

    case MC_Echoverse_Damp:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Damp, value);
        break;

    case MC_Echoverse_Ext_Stereo:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Ext_Stereo, value);
        break;

    case MC_Echoverse_Angle:
        Rack_Effects[EFX_ECHOVERSE]->changepar(Echoverse_Angle, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sustain_Gain:
        Rack_Effects[EFX_SUSTAINER]->changepar(Sustain_Gain, value);
        break;

    case MC_Sustain_Sustain:
        Rack_Effects[EFX_SUSTAINER]->changepar(Sustain_Sustain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Sequence_DryWet:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_DryWet, Dry_Wet(value));
        break;

    case MC_Sequence_Step_1:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_1, value);
        break;

    case MC_Sequence_Step_2:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_2, value);
        break;

    case MC_Sequence_Step_3:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_3, value);
        break;

    case MC_Sequence_Step_4:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_4, value);
        break;

    case MC_Sequence_Step_5:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_5, value);
        break;

    case MC_Sequence_Step_6:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_6, value);
        break;

    case MC_Sequence_Step_7:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_7, value);
        break;

    case MC_Sequence_Step_8:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Step_8, value);
        break;

    case MC_Sequence_Tempo:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Tempo, ret_Tempo(value));
        break;

    case MC_Sequence_Resonance:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Resonance, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sequence_Stdf:
        Rack_Effects[EFX_SEQUENCE]->changepar(Sequence_Stdf, (int) ((float) value * C_MC_7_RANGE));
        break;

    case MC_Shifter_DryWet:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_DryWet, Dry_Wet(value));
        break;

    case MC_Shifter_Interval:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Interval, (int) ((float) value * C_MC_12_RANGE));
        break;

    case MC_Shifter_Gain:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Gain, value);
        break;

    case MC_Shifter_Pan:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Pan, value);
        break;

    case MC_Shifter_Attack:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Attack, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case MC_Shifter_Decay:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Decay, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case MC_Shifter_Threshold:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case MC_Shifter_Whammy:
        Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Whammy, value);
        break;

    case MC_Stomp_Level:
        Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Level, value);
        break;

    case MC_Stomp_Gain:
        Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Gain, value);
        break;

    case MC_Stomp_Bias:
        Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Bias, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Stomp_Mid:
        Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Mid, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Stomp_Tone:
        Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Tone, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Revtron_DryWet:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_DryWet, Dry_Wet(value));
        break;

    case MC_Revtron_Pan:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Pan, value);
        break;

    case MC_Revtron_Level:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Level, value);
        break;

    case MC_Revtron_Damp:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Damp, value);
        break;

    case MC_Revtron_Feedback:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Feedback, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Revtron_Length:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Length, 20 + (int) ((float) value * C_MC_1480_RANGE));
        break;

    case MC_Revtron_Stretch:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Stretch, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Revtron_I_Delay:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_I_Delay, (int) ((float) value * C_MC_500_RANGE));
        break;

    case MC_Revtron_Fade:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Fade, value);
        break;

    case MC_Echotron_DryWet:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_DryWet, Dry_Wet(value));
        break;

    case MC_Echotron_Pan:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Pan, value);
        break;

    case MC_Echotron_Tempo:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Tempo, ret_Tempo(value));
        break;

    case MC_Echotron_Damp:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Damp, value);
        break;

    case MC_Echotron_Feedback:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Feedback, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Echotron_LR_Cross:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_LR_Cross, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Echotron_LFO_Width:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_LFO_Width, value);
        break;

    case MC_Echotron_Depth:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Depth, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Echotron_LFO_Stereo:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_LFO_Stereo, value);
        break;

    case MC_Echotron_Taps:
    {
        /* There is a minor problem with this approach. If the user sets the delay
           file after setting the taps, then the taps will not get adjusted upward. 
           The user will have to move the MIDI control slider to set taps upward.
           The downward limit is checked and limited always on file loading. */
        int number_taps = 1 + (int) ((float) value * C_MC_126_RANGE);
        
        Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
        if(number_taps > Efx_Echotron->get_file_length())
        {
            number_taps = Efx_Echotron->get_file_length();
        }
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Taps, number_taps);
        break;
    }
    case MC_Sharm_DryWet:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_DryWet, Dry_Wet(value));
        break;

    case MC_Sharm_L_Interval:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_L_Interval, (int) ((float) value * C_MC_24_RANGE));
        break;

    case MC_Sharm_L_Chroma:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_L_Chroma, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case MC_Sharm_L_Gain:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_L_Gain, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sharm_R_Interval:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_R_Interval, (int) ((float) value * C_MC_24_RANGE));
        break;

    case MC_Sharm_R_Chroma:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_R_Chroma, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case MC_Sharm_R_Gain:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_R_Gain, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sharm_LR_Cross:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_LR_Cross, value);
        break;

    case MC_Sharm_Note:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_Note, (int) ((float) value * C_MC_23_RANGE));
        break;

    case MC_Sharm_Chord:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_Chord, (int) ((float) value * C_MC_33_RANGE));
        break;

    case MC_CompBand_DryWet:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_DryWet, Dry_Wet(value));
        break;

    case MC_CompBand_Gain:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Gain, value);
        break;

    case MC_CompBand_Low_Ratio:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Low_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_Mid_1_Ratio:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Mid_1_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_Mid_2_Ratio:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Mid_2_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_High_Ratio:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_High_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_Low_Thresh:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Low_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_Mid_1_Thresh:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Mid_1_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_Mid_2_Thresh:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Mid_2_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_High_Thresh:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_High_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_Cross_1:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_CompBand_Cross_2:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case MC_CompBand_Cross_3:
        Rack_Effects[EFX_COMPBAND]->changepar(CompBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case MC_Optical_Depth:
        Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_Depth, value);
        break;

    case MC_Optical_LFO_Tempo:
        Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Optical_LFO_Random:
        Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_LFO_Random, value);
        break;

    case MC_Optical_LFO_Stereo:
        Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_LFO_Stereo, value);
        break;

    case MC_Optical_Pan:
        Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_Pan, value);
        break;

    case MC_Vibe_DryWet:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_DryWet, Dry_Wet(value));
        break;

    case MC_Vibe_Width:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_Width, value);
        break;

    case MC_Vibe_Depth:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_Depth, value);
        break;

    case MC_Vibe_LFO_Tempo:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Vibe_LFO_Random:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_LFO_Random, value);
        break;

    case MC_Vibe_LFO_Stereo:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_LFO_Stereo, value);
        break;

    case MC_Vibe_Feedback:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_Feedback, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Vibe_LR_Cross:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_LR_Cross, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Vibe_Pan:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_Pan, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Infinity_DryWet:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_DryWet, Dry_Wet(value));
        break;

    case MC_Infinity_Resonance:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Resonance, -1000 + (int) ((float) value * C_MC_2000_RANGE));
        break;

    case MC_Infinity_AutoPan:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_AutoPan, value);
        break;

    case MC_Infinity_LR_Delay:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_LR_Delay, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Start:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Start, value);
        break;

    case MC_Infinity_End:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_End, value);
        break;

    case MC_Infinity_Tempo:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Tempo, ret_Tempo(value));
        break;

    case MC_Infinity_Subdivision:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Subdivision, -16 + (int) ((float) value * C_MC_32_RANGE));
        break;

    case MC_Alien_Delay:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_Delay, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_APhase_LFO_Random:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Random, value);
        break;

    case MC_Cabinet_Gain:
        Rack_Effects[EFX_CABINET]->changepar(Cabinet_Gain, value);
        break;

    case MC_Chorus_Delay:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_Delay, value);
        break;
    
    case MC_Echo_Reverse:
        Rack_Effects[EFX_ECHO]->changepar(Echo_Reverse, value);
        break;

    case MC_Echo_Delay:
        Rack_Effects[EFX_ECHO]->changepar(Echo_Delay, 20 + (int) ((float) value * C_MC_1980_RANGE));
        break;

    case MC_Echo_LR_Delay:
        Rack_Effects[EFX_ECHO]->changepar(Echo_LR_Delay, value);
        break;

    case MC_Echo_Damp:
        Rack_Effects[EFX_ECHO]->changepar(Echo_Damp, value);
        break;

    case MC_Flanger_Delay:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_Delay, value);
        break;
        
    case MC_Harm_Gain:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Gain, value);
        break;

    case MC_Harm_Filter_Gain:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Filter_Gain, value);
        break;

    case MC_Harm_Filter_Q:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Filter_Q, value);
        break;

    case MC_Infinity_Band_1:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_1, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_2:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_2, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_3:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_3, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_4:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_4, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_5:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_5, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_6:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_6, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_7:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_7, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_8:
        Rack_Effects[EFX_INFINITY]->changepar(Infinity_Band_8, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Looper_Tempo:
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Tempo, 20 + (int) ((float) value * C_MC_360_RANGE));
        break;

    case MC_Music_Tempo:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Tempo, 10 + (int) ((float) value * C_MC_470_RANGE));
        break;

    case MC_Music_Damp:
        Rack_Effects[EFX_MUSICAL_DELAY]->changepar(Music_Damp, value);
        break;

    case MC_MuTro_LFO_Random:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_LFO_Random, value);
        break;

    case MC_MuTro_LFO_Stereo:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_LFO_Stereo, value);
        break;

    case MC_MuTro_St_Freq:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_St_Freq, 30 + (int) ((float) value * C_MC_770_RANGE));
        break;

    case MC_Gate_Attack:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_Attack, 1 + (int) ((float) value * C_MC_249_RANGE));
        break;

    case MC_Gate_Release:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_Release, 2 + (int) ((float) value * C_MC_248_RANGE));
        break;

    case MC_Gate_Range:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_Range, -90 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case MC_Gate_Threshold:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case MC_Gate_Hold:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_Hold, 2 + (int) ((float) value * C_MC_498_RANGE));
        break;

    case MC_Gate_LPF:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_LPF, ret_LPF(value));
        break;

    case MC_Gate_HPF:
        Rack_Effects[EFX_NOISEGATE]->changepar(Gate_HPF, ret_HPF(value));
        break;

    case MC_Pan_Ex_St_Amt:
        Rack_Effects[EFX_PAN]->changepar(Pan_Ex_St_Amt, value);
        break;

    case MC_Reverb_Time:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_Time, value);
        break;

    case MC_Reverb_I_Delay:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_I_Delay, value);
        break;

    case MC_Reverb_Delay_FB:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_Delay_FB, value);
        break;

    case MC_Reverb_Room:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_Room, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Reverb_Damp:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_Damp, 64 + (int) ((float) value * C_MC_63_RANGE));
        break;

    case MC_Revtron_Diffusion:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Diffusion, value);
        break;

    case MC_Revtron_LPF:
        Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_LPF, ret_LPF(value));
        break;

    case MC_Harm_Note:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Note, (int) ((float) value * C_MC_23_RANGE));
        break;

    case MC_Harm_Chord:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Chord, (int) ((float) value * C_MC_33_RANGE));
        break;

    case MC_Synthfilter_LFO_Random:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_LFO_Random, value);
        break;

    case MC_Harm_Select:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Select, value);
        break;

    case MC_Sharm_Select:
        Rack_Effects[EFX_STEREOHARM]->changepar(Sharm_Select, value);
        break;

    case MC_Dist_Type:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_Overdrive_Type:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_Dere_Type:
        Rack_Effects[EFX_DERELICT]->changepar(Dere_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_DistBand_Type_Low:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Type_Low, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_DistBand_Type_Mid:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Type_Mid, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_DistBand_Type_Hi:
        Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Type_Hi, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_Stomp_Mode:
        Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Mode, (int) ((float) value * C_MC_8_RANGE));
        break;

    case MC_Alien_LFO_Type:
        Rack_Effects[EFX_ALIENWAH]->changepar(Alien_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_APhase_LFO_Type:
        Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Chorus_LFO_Type:
        Rack_Effects[EFX_CHORUS]->changepar(Chorus_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Flanger_LFO_Type:
        Rack_Effects[EFX_FLANGER]->changepar(Flanger_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_DFlange_LFO_Type:
        Rack_Effects[EFX_DUAL_FLANGE]->changepar(DFlange_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Echotron_LFO_Type:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_MuTro_LFO_Type:
        Rack_Effects[EFX_MUTROMOJO]->changepar(MuTro_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Optical_LFO_Type:
        Rack_Effects[EFX_OPTICALTREM]->changepar(Optical_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Pan_LFO_Type:
        Rack_Effects[EFX_PAN]->changepar(Pan_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Phaser_LFO_Type:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Synthfilter_LFO_Type:
        Rack_Effects[EFX_SYNTHFILTER]->changepar(Synthfilter_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_VaryBand_LFO_Type_1:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Type_1, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_VaryBand_LFO_Type_2:
        Rack_Effects[EFX_VARYBAND]->changepar(VaryBand_LFO_Type_2, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Vibe_LFO_Type:
        Rack_Effects[EFX_VIBE]->changepar(Vibe_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_WahWah_LFO_Type:
        Rack_Effects[EFX_WAHWAH]->changepar(WahWah_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;
    }
}

int
RKR::ret_Tempo(int value)
{
    return ( 1 + (int) ((float) value * C_MC_600_RANGE));
}

int
RKR::ret_LPF(int value)
{
    return ( 20 + (int) ((float) value * C_MC_25980_RANGE));
}

int
RKR::ret_HPF(int value)
{
    return ( 20 + (int) ((float) value * C_MC_19980_RANGE));
}

