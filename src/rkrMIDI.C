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
        "Alienwah Dry/Wet",         strdup( NTS(MC_Alien_DryWet).c_str()),           strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_DryWet).c_str()),         "127",     "-1.0",
        "Alienwah Pan",             strdup( NTS(MC_Alien_Pan).c_str()),              strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Pan).c_str()),              "0",     "1.0",
        "Alienwah Tempo",           strdup( NTS(MC_Alien_LFO_Tempo).c_str()),        strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Tempo).c_str()),        "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Alienwah Random",          strdup( NTS(MC_Alien_LFO_Random).c_str()),       strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Random).c_str()),       "0",     "1.0",
        "Alienwah LFO Type",        strdup( NTS(MC_Alien_LFO_Type).c_str()),         strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Type).c_str()),         "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Alienwah Phase",           strdup( NTS(MC_Alien_Phase).c_str()),            strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Phase).c_str()),            "0",     "1.0",
        "Alienwah Stereo Df.",      strdup( NTS(MC_Alien_LFO_Stereo).c_str()),       strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LFO_Stereo).c_str()),       "0",     "1.0",
        "Alienwah Depth",           strdup( NTS(MC_Alien_Depth).c_str()),            strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Depth).c_str()),            "0",     "1.0",
        "Alienwah Delay",           strdup( NTS(MC_Alien_Delay).c_str()),            strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Delay).c_str()),            "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Alienwah Feedback",        strdup( NTS(MC_Alien_Feedback).c_str()),         strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_Feedback).c_str()),         "0",     "1.0",
        "Alienwah L/R Cross",       strdup( NTS(MC_Alien_LR_Cross).c_str()),         strdup( NTS(EFX_ALIENWAH).c_str()),      strdup( NTS(Alien_LR_Cross).c_str()),         "0",     "1.0",

        "Analog Phaser Dry/Wet",    strdup( NTS(MC_APhase_DryWet).c_str()),          strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_DryWet).c_str()),        "127",     "-1.0",
        "Analog Phaser LFO Type",   strdup( NTS(MC_APhase_LFO_Type).c_str()),        strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Analog Phaser Tempo",      strdup( NTS(MC_APhase_LFO_Tempo).c_str()),       strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Analog Phaser P. Depth",   strdup( NTS(MC_APhase_Depth).c_str()),           strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Depth).c_str()),           "0",     "1.0",
        "Analog Phaser Width",      strdup( NTS(MC_APhase_Width).c_str()),           strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Width).c_str()),           "0",     "1.0",
        "Analog Phaser Feedback",   strdup( NTS(MC_APhase_Feedback).c_str()),        strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Feedback).c_str()),        "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Analog Phaser Mismatch",   strdup( NTS(MC_APhase_Mismatch).c_str()),        strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Mismatch).c_str()),        "0",     "1.0",
        "Analog Phaser Distortion", strdup( NTS(MC_APhase_Distortion).c_str()),      strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_Distortion).c_str()),      "0",     "1.0",
        "Analog Phaser Random",     strdup( NTS(MC_APhase_LFO_Random).c_str()),      strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Random).c_str()),      "0",     "1.0",
        "Analog Phaser Stereo Df.", strdup( NTS(MC_APhase_LFO_Stereo).c_str()),      strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(APhase_LFO_Stereo).c_str()),      "0",     "1.0",

        "Arpie Dry/Wet",            strdup( NTS(MC_Arpie_DryWet).c_str()),           strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_DryWet).c_str()),         "127",     "-1.0",
        "Arpie Arpe's",             strdup( NTS(MC_Arpie_ArpeWD).c_str()),           strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_ArpeWD).c_str()),           "0",     "1.0",
        "Arpie Pan",                strdup( NTS(MC_Arpie_Pan).c_str()),              strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Pan).c_str()),              "0",     "1.0",
        "Arpie Tempo",              strdup( NTS(MC_Arpie_Tempo).c_str()),            strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Tempo).c_str()),            "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Arpie L/R Delay",          strdup( NTS(MC_Arpie_LR_Delay).c_str()),         strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_LR_Delay).c_str()),         "0",     "1.0",
        "Arpie L/R Cross",          strdup( NTS(MC_Arpie_LR_Cross).c_str()),         strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_LR_Cross).c_str()),         "0",     "1.0",
        "Arpie Feedback",           strdup( NTS(MC_Arpie_Feedback).c_str()),         strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Feedback).c_str()),         "0",     "1.0",
        "Arpie Damp",               strdup( NTS(MC_Arpie_Damp).c_str()),             strdup( NTS(EFX_ARPIE).c_str()),         strdup( NTS(Arpie_Damp).c_str()),             "0",     "1.0",

        "Balance FX%",              strdup( NTS(MC_Balance_FX).c_str()),             strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "Cabinet Gain",             strdup( NTS(MC_Cabinet_Gain).c_str()),           strdup( NTS(EFX_CABINET).c_str()),       strdup( NTS(Cabinet_Gain).c_str()),           "0",     "1.0",

        "Chorus Dry/Wet",           strdup( NTS(MC_Chorus_DryWet).c_str()),          strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_DryWet).c_str()),        "127",     "-1.0",
        "Chorus Pan",               strdup( NTS(MC_Chorus_Pan).c_str()),             strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Pan).c_str()),             "0",     "1.0",
        "Chorus Tempo",             strdup( NTS(MC_Chorus_LFO_Tempo).c_str()),       strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Chorus Random",            strdup( NTS(MC_Chorus_LFO_Random).c_str()),      strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Random).c_str()),      "0",     "1.0",
        "Chorus LFO Type",          strdup( NTS(MC_Chorus_LFO_Type).c_str()),        strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Chorus Stereo Df",         strdup( NTS(MC_Chorus_LFO_Stereo).c_str()),      strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LFO_Stereo).c_str()),      "0",     "1.0",
        "Chorus Depth",             strdup( NTS(MC_Chorus_Depth).c_str()),           strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Depth).c_str()),           "0",     "1.0",
        "Chorus Delay",             strdup( NTS(MC_Chorus_Delay).c_str()),           strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Delay).c_str()),           "0",     "1.0",
        "Chorus Feedback",          strdup( NTS(MC_Chorus_Feedback).c_str()),        strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_Feedback).c_str()),        "0",     "1.0",
        "Chorus L/R Cross",         strdup( NTS(MC_Chorus_LR_Cross).c_str()),        strdup( NTS(EFX_CHORUS).c_str()),        strdup( NTS(Chorus_LR_Cross).c_str()),        "0",     "1.0",

        "CoilCrafter Gain",         strdup( NTS(MC_Coil_Gain).c_str()),              strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Gain).c_str()),              "0",     "1.0",
        "CoilCrafter Tone",         strdup( NTS(MC_Coil_Tone).c_str()),              strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Tone).c_str()),             "20",     strdup( NTS(C_MC_4380_RANGE).c_str()),
        "CoilCrafter Freq 1",       strdup( NTS(MC_Coil_Freq_1).c_str()),            strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Freq_1).c_str()),         "2600",     strdup( NTS(C_MC_1900_RANGE).c_str()),
        "CoilCrafter Q 1",          strdup( NTS(MC_Coil_Q_1).c_str()),               strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Q_1).c_str()),              "10",     strdup( NTS(C_MC_55_RANGE).c_str()),
        "CoilCrafter Freq 2",       strdup( NTS(MC_Coil_Freq_2).c_str()),            strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Freq_2).c_str()),         "2600",     strdup( NTS(C_MC_1900_RANGE).c_str()),
        "CoilCrafter Q 2",          strdup( NTS(MC_Coil_Q_2).c_str()),               strdup( NTS(EFX_COILCRAFTER).c_str()),   strdup( NTS(Coil_Q_2).c_str()),              "10",     strdup( NTS(C_MC_55_RANGE).c_str()),

        "CompBand Dry/Wet",         strdup( NTS(MC_CompBand_DryWet).c_str()),        strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_DryWet).c_str()),      "127",     "-1.0",
        "CompBand Gain",            strdup( NTS(MC_CompBand_Gain).c_str()),          strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Gain).c_str()),          "0",     "1.0",
        "CompBand L Ratio",         strdup( NTS(MC_CompBand_Low_Ratio).c_str()),     strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Low_Ratio).c_str()),     "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "CompBand ML Ratio",        strdup( NTS(MC_CompBand_Mid_1_Ratio).c_str()),   strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_1_Ratio).c_str()),   "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "CompBand MH Ratio",        strdup( NTS(MC_CompBand_Mid_2_Ratio).c_str()),   strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_2_Ratio).c_str()),   "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "CompBand H Ratio",         strdup( NTS(MC_CompBand_High_Ratio).c_str()),    strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_High_Ratio).c_str()),    "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "CompBand L Thres",         strdup( NTS(MC_CompBand_Low_Thresh).c_str()),    strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Low_Thresh).c_str()),  "-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "CompBand ML Thres",        strdup( NTS(MC_CompBand_Mid_1_Thresh).c_str()),  strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_1_Thresh).c_str()),"-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "CompBand MH Thres",        strdup( NTS(MC_CompBand_Mid_2_Thresh).c_str()),  strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Mid_2_Thresh).c_str()),"-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "CompBand H Thres",         strdup( NTS(MC_CompBand_High_Thresh).c_str()),   strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_High_Thresh).c_str()), "-70",     strdup( NTS(C_MC_94_RANGE).c_str()),
        "CompBand Cross 1",         strdup( NTS(MC_CompBand_Cross_1).c_str()),       strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Cross_1).c_str()),      "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "CompBand Cross 2",         strdup( NTS(MC_CompBand_Cross_2).c_str()),       strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Cross_2).c_str()),    "1000",     strdup( NTS(C_MC_7000_RANGE).c_str()),
        "CompBand Cross 3",         strdup( NTS(MC_CompBand_Cross_3).c_str()),       strdup( NTS(EFX_COMPBAND).c_str()),      strdup( NTS(CompBand_Cross_3).c_str()),    "2000",     strdup( NTS(C_MC_24000_RANGE).c_str()),

        "Compressor A.Time",        strdup( NTS(MC_Compress_Attack).c_str()),        strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Attack).c_str()),       "10",     strdup( NTS(C_MC_240_RANGE).c_str()),
        "Compressor Knee",          strdup( NTS(MC_Compress_Knee).c_str()),          strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Knee).c_str()),          "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Compressor Output",        strdup( NTS(MC_Compress_Output).c_str()),        strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Output).c_str()),      "-40",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "Compressor Ratio",         strdup( NTS(MC_Compress_Ratio).c_str()),         strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Ratio).c_str()),         "2",     strdup( NTS(C_MC_40_RANGE).c_str()),
        "Compressor R.Time",        strdup( NTS(MC_Compress_Release).c_str()),       strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Release).c_str()),      "10",     strdup( NTS(C_MC_490_RANGE).c_str()),
        "Compressor Threshold",     strdup( NTS(MC_Compress_Threshold).c_str()),     strdup( NTS(EFX_COMPRESSOR).c_str()),    strdup( NTS(Compress_Threshold).c_str()),   "-60",     strdup( NTS(C_MC_57_RANGE).c_str()),

        "Convolotron Dry/Wet",      strdup( NTS(MC_Convo_DryWet).c_str()),           strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_DryWet).c_str()),         "127",     "-1.0",
        "Convolotron Pan",          strdup( NTS(MC_Convo_Pan).c_str()),              strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Pan).c_str()),              "0",     "1.0",
        "Convolotron Level",        strdup( NTS(MC_Convo_Level).c_str()),            strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Level).c_str()),            "0",     "1.0",
        "Convolotron Damp",         strdup( NTS(MC_Convo_Damp).c_str()),             strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Damp).c_str()),             "0",     "1.0",
        "Convolotron Feedback",     strdup( NTS(MC_Convo_Feedback).c_str()),         strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Feedback).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Convolotron Length",       strdup( NTS(MC_Convo_Length).c_str()),           strdup( NTS(EFX_CONVOLOTRON).c_str()),   strdup( NTS(Convo_Length).c_str()),           "5",     strdup( NTS(C_MC_245_RANGE).c_str()),

        "Derelict Dry/Wet",         strdup( NTS(MC_Dere_DryWet).c_str()),            strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_DryWet).c_str()),          "127",     "-1.0",
        "Derelict L/R Cross",       strdup( NTS(MC_Dere_LR_Cross).c_str()),          strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_LR_Cross).c_str()),          "0",     "1.0",
        "Derelict Drive",           strdup( NTS(MC_Dere_Drive).c_str()),             strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Drive).c_str()),             "0",     "1.0",
        "Derelict Level",           strdup( NTS(MC_Dere_Level).c_str()),             strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Level).c_str()),             "0",     "1.0",
        "Derelict Type",            strdup( NTS(MC_Dere_Type).c_str()),              strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Type).c_str()),              "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "Derelict Color",           strdup( NTS(MC_Dere_Color).c_str()),             strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Color).c_str()),             "0",     "1.0",
        "Derelict Sub Octave",      strdup( NTS(MC_Dere_Suboctave).c_str()),         strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Suboctave).c_str()),         "0",     "1.0",
        "Derelict Pan",             strdup( NTS(MC_Dere_Pan).c_str()),               strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_Pan).c_str()),               "0",     "1.0",
        "Derelict LPF",             strdup( NTS(MC_Dere_LPF).c_str()),               strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_LPF).c_str()),              "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Derelict HPF",             strdup( NTS(MC_Dere_HPF).c_str()),               strdup( NTS(EFX_DERELICT).c_str()),      strdup( NTS(Dere_HPF).c_str()),              "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "DistBand Dry/Wet",         strdup( NTS(MC_DistBand_DryWet).c_str()),        strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_DryWet).c_str()),      "127",     "-1.0",
        "DistBand L/R Cross",       strdup( NTS(MC_DistBand_LR_Cross).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_LR_Cross).c_str()),      "0",     "1.0",
        "DistBand Drive",           strdup( NTS(MC_DistBand_Drive).c_str()),         strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Drive).c_str()),         "0",     "1.0",
        "DistBand Level",           strdup( NTS(MC_DistBand_Level).c_str()),         strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Level).c_str()),         "0",     "1.0",
        "DistBand L. Gain",         strdup( NTS(MC_DistBand_Gain_Low).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Gain_Low).c_str()),      "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "DistBand M. Gain",         strdup( NTS(MC_DistBand_Gain_Mid).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Gain_Mid).c_str()),      "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "DistBand H. Gain",         strdup( NTS(MC_DistBand_Gain_Hi).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Gain_Hi).c_str()),       "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "DistBand Cross 1",         strdup( NTS(MC_DistBand_Cross_1).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Cross_1).c_str()),      "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "DistBand Cross 2",         strdup( NTS(MC_DistBand_Cross_2).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Cross_2).c_str()),     "800",     strdup( NTS(C_MC_11200_RANGE).c_str()),
        "DistBand Type Low",        strdup( NTS(MC_DistBand_Type_Low).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Type_Low).c_str()),      "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "DistBand Type Mid",        strdup( NTS(MC_DistBand_Type_Mid).c_str()),      strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Type_Mid).c_str()),      "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "DistBand Type High",       strdup( NTS(MC_DistBand_Type_Hi).c_str()),       strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Type_Hi).c_str()),       "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "DistBand Pan",             strdup( NTS(MC_DistBand_Pan).c_str()),           strdup( NTS(EFX_DISTBAND).c_str()),      strdup( NTS(DistBand_Pan).c_str()),           "0",     "1.0",

        "Distortion Dry/Wet",       strdup( NTS(MC_Dist_DryWet).c_str()),            strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_DryWet).c_str()),          "127",     "-1.0",
        "Distortion L/R Cross",     strdup( NTS(MC_Dist_LR_Cross).c_str()),          strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_LR_Cross).c_str()),          "0",     "1.0",
        "Distortion Drive",         strdup( NTS(MC_Dist_Drive).c_str()),             strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Drive).c_str()),             "0",     "1.0",
        "Distortion Level",         strdup( NTS(MC_Dist_Level).c_str()),             strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Level).c_str()),             "0",     "1.0",
        "Distortion Type",          strdup( NTS(MC_Dist_Type).c_str()),              strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Type).c_str()),              "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "Distortion Pan",           strdup( NTS(MC_Dist_Pan).c_str()),               strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Pan).c_str()),               "0",     "1.0",
        "Distortion Sub Octave",    strdup( NTS(MC_Dist_Suboctave).c_str()),         strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_Suboctave).c_str()),         "0",     "1.0",
        "Distortion LPF",           strdup( NTS(MC_Dist_LPF).c_str()),               strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_LPF).c_str()),              "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Distortion HPF",           strdup( NTS(MC_Dist_HPF).c_str()),               strdup( NTS(EFX_DISTORTION).c_str()),    strdup( NTS(Dist_HPF).c_str()),              "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "Dual Flange Dry/Wet",      strdup( NTS(MC_DFlange_DryWet).c_str()),         strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_DryWet).c_str()),       "127",     "-1.0",
        "Dual Flange Pan",          strdup( NTS(MC_DFlange_Pan).c_str()),            strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Pan).c_str()),          "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Dual Flange L/R Cross",    strdup( NTS(MC_DFlange_LR_Cross).c_str()),       strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LR_Cross).c_str()),       "0",     "1.0",
        "Dual Flange Depth",        strdup( NTS(MC_DFlange_Depth).c_str()),          strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Depth).c_str()),         "20",     strdup( NTS(C_MC_2480_RANGE).c_str()),
        "Dual Flange Width",        strdup( NTS(MC_DFlange_Width).c_str()),          strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Width).c_str()),          "0",     strdup( NTS(C_MC_6000_RANGE).c_str()),
        "Dual Flange Offset",       strdup( NTS(MC_DFlange_Offset).c_str()),         strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Offset).c_str()),         "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Dual Flange Feedback",     strdup( NTS(MC_DFlange_Feedback).c_str()),       strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_Feedback).c_str()),     "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Dual Flange LPF",          strdup( NTS(MC_DFlange_LPF).c_str()),            strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LPF).c_str()),           "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),
        "Dual Flange Tempo",        strdup( NTS(MC_DFlange_LFO_Tempo).c_str()),      strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Tempo).c_str()),      "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Dual Flange Stereo Df",    strdup( NTS(MC_DFlange_LFO_Stereo).c_str()),     strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Stereo).c_str()),     "0",     "1.0",
        "Dual Flange LFO Type",     strdup( NTS(MC_DFlange_LFO_Type).c_str()),       strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Type).c_str()),       "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Dual Flange Random",       strdup( NTS(MC_DFlange_LFO_Random).c_str()),     strdup( NTS(EFX_DUAL_FLANGE).c_str()),   strdup( NTS(DFlange_LFO_Random).c_str()),     "0",     "1.0",

        "Echo Dry/Wet",             strdup( NTS(MC_Echo_DryWet).c_str()),            strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_DryWet).c_str()),          "127",     "-1.0",
        "Echo Reverse",             strdup( NTS(MC_Echo_Reverse).c_str()),           strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Reverse).c_str()),           "0",     "1.0",
        "Echo Pan",                 strdup( NTS(MC_Echo_Pan).c_str()),               strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Pan).c_str()),               "0",     "1.0",
        "Echo Delay",               strdup( NTS(MC_Echo_Delay).c_str()),             strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Delay).c_str()),            "20",     strdup( NTS(C_MC_1980_RANGE).c_str()),
        "Echo L/R Delay",           strdup( NTS(MC_Echo_LR_Delay).c_str()),          strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_LR_Delay).c_str()),          "0",     "1.0",
        "Echo L/R Cross",           strdup( NTS(MC_Echo_LR_Cross).c_str()),          strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_LR_Cross).c_str()),          "0",     "1.0",
        "Echo Feedback",            strdup( NTS(MC_Echo_Feedback).c_str()),          strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Feedback).c_str()),          "0",     "1.0",
        "Echo Damp",                strdup( NTS(MC_Echo_Damp).c_str()),              strdup( NTS(EFX_ECHO).c_str()),          strdup( NTS(Echo_Damp).c_str()),              "0",     "1.0",

        "Echotron Dry/Wet",         strdup( NTS(MC_Echotron_DryWet).c_str()),        strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_DryWet).c_str()),      "127",     "-1.0",
        "Echotron Pan",             strdup( NTS(MC_Echotron_Pan).c_str()),           strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Pan).c_str()),           "0",     "1.0",
        "Echotron Tempo",           strdup( NTS(MC_Echotron_Tempo).c_str()),         strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Echotron Damp",            strdup( NTS(MC_Echotron_Damp).c_str()),          strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Damp).c_str()),          "0",     "1.0",
        "Echotron Feedback",        strdup( NTS(MC_Echotron_Feedback).c_str()),      strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Feedback).c_str()),    "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Echotron L/R Cross",       strdup( NTS(MC_Echotron_LR_Cross).c_str()),      strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LR_Cross).c_str()),      "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Echotron Width",           strdup( NTS(MC_Echotron_LFO_Width).c_str()),     strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LFO_Width).c_str()),     "0",     "1.0",
        "Echotron Depth",           strdup( NTS(MC_Echotron_Depth).c_str()),         strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Depth).c_str()),         "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Echotron Stereo Df",       strdup( NTS(MC_Echotron_LFO_Stereo).c_str()),    strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LFO_Stereo).c_str()),    "0",     "1.0",
        "Echotron LFO Type",        strdup( NTS(MC_Echotron_LFO_Type).c_str()),      strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_LFO_Type).c_str()),      "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Echotron #",               strdup( NTS(MC_Echotron_Taps).c_str()),          strdup( NTS(EFX_ECHOTRON).c_str()),      strdup( NTS(Echotron_Taps).c_str()),          "1",     strdup( NTS(C_MC_126_RANGE).c_str()),

        "Echoverse Dry/Wet",        strdup( NTS(MC_Echoverse_DryWet).c_str()),       strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_DryWet).c_str()),     "127",     "-1.0",
        "Echoverse Reverse",        strdup( NTS(MC_Echoverse_Reverse).c_str()),      strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Reverse).c_str()),      "0",     "1.0",
        "Echoverse Pan",            strdup( NTS(MC_Echoverse_Pan).c_str()),          strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Pan).c_str()),          "0",     "1.0",
        "Echoverse Tempo",          strdup( NTS(MC_Echoverse_Tempo).c_str()),        strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Tempo).c_str()),        "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Echoverse L/R Delay",      strdup( NTS(MC_Echoverse_LR_Delay).c_str()),     strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_LR_Delay).c_str()),     "0",     "1.0",
        "Echoverse Feedback",       strdup( NTS(MC_Echoverse_Feedback).c_str()),     strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Feedback).c_str()),     "0",     "1.0",
        "Echoverse Damp",           strdup( NTS(MC_Echoverse_Damp).c_str()),         strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Damp).c_str()),         "0",     "1.0",
        "Echoverse Ex Stereo",      strdup( NTS(MC_Echoverse_Ext_Stereo).c_str()),   strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Ext_Stereo).c_str()),   "0",     "1.0",
        "Echoverse Angle",          strdup( NTS(MC_Echoverse_Angle).c_str()),        strdup( NTS(EFX_ECHOVERSE).c_str()),     strdup( NTS(Echoverse_Angle).c_str()),        "0",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "EQ Gain",                  strdup( NTS(MC_EQ_Gain).c_str()),                strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_Gain).c_str()),                "0",     "1.0",
        "EQ Q",                     strdup( NTS(MC_EQ_Q).c_str()),                   strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_Q).c_str()),                   "0",     "1.0",
        "EQ 31 Hz",                 strdup( NTS(MC_EQ_31_HZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_31_HZ).c_str()),               "0",     "1.0",
        "EQ 63 Hz",                 strdup( NTS(MC_EQ_63_HZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_63_HZ).c_str()),               "0",     "1.0",
        "EQ 125 Hz",                strdup( NTS(MC_EQ_125_HZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_125_HZ).c_str()),              "0",     "1.0",
        "EQ 250 Hz",                strdup( NTS(MC_EQ_250_HZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_250_HZ).c_str()),              "0",     "1.0",
        "EQ 500 Hz",                strdup( NTS(MC_EQ_500_HZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_500_HZ).c_str()),              "0",     "1.0",
        "EQ 1 Khz",                 strdup( NTS(MC_EQ_1_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_1_KHZ).c_str()),               "0",     "1.0",
        "EQ 2 Khz",                 strdup( NTS(MC_EQ_2_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_2_KHZ).c_str()),               "0",     "1.0",
        "EQ 4 Khz",                 strdup( NTS(MC_EQ_4_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_4_KHZ).c_str()),               "0",     "1.0",
        "EQ 8 Khz",                 strdup( NTS(MC_EQ_8_KHZ).c_str()),               strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_8_KHZ).c_str()),               "0",     "1.0",
        "EQ 16 Khz",                strdup( NTS(MC_EQ_16_KHZ).c_str()),              strdup( NTS(EFX_EQ).c_str()),            strdup( NTS(EQ_16_KHZ).c_str()),              "0",     "1.0",

        "Exciter Gain",             strdup( NTS(MC_Exciter_Gain).c_str()),           strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Gain).c_str()),           "0",     "1.0",
        "Exciter LPF",              strdup( NTS(MC_Exciter_LPF).c_str()),            strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_LPF).c_str()),           "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Exciter HPF",              strdup( NTS(MC_Exciter_HPF).c_str()),            strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_HPF).c_str()),           "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),
        "Exciter Har 1",            strdup( NTS(MC_Exciter_Harm_1).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_1).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 2",            strdup( NTS(MC_Exciter_Harm_2).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_2).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 3",            strdup( NTS(MC_Exciter_Harm_3).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_3).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 4",            strdup( NTS(MC_Exciter_Harm_4).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_4).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 5",            strdup( NTS(MC_Exciter_Harm_5).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_5).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 6",            strdup( NTS(MC_Exciter_Harm_6).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_6).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 7",            strdup( NTS(MC_Exciter_Harm_7).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_7).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 8",            strdup( NTS(MC_Exciter_Harm_8).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_8).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 9",            strdup( NTS(MC_Exciter_Harm_9).c_str()),         strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_9).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Exciter Har 10",           strdup( NTS(MC_Exciter_Harm_10).c_str()),        strdup( NTS(EFX_EXCITER).c_str()),       strdup( NTS(Exciter_Harm_10).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "Expander A.Time",          strdup( NTS(MC_Expander_Attack).c_str()),        strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Attack).c_str()),        "1",     strdup( NTS(C_MC_4999_RANGE).c_str()),
        "Expander R.Time",          strdup( NTS(MC_Expander_Release).c_str()),       strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Release).c_str()),      "10",     strdup( NTS(C_MC_990_RANGE).c_str()),
        "Expander Shape",           strdup( NTS(MC_Expander_Shape).c_str()),         strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Shape).c_str()),         "1",     strdup( NTS(C_MC_49_RANGE).c_str()),
        "Expander Threshold",       strdup( NTS(MC_Expander_Threshold).c_str()),     strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Threshold).c_str()),     "0",     strdup( NTS(-C_MC_80_RANGE).c_str()),
        "Expander Out Gain",        strdup( NTS(MC_Expander_Gain).c_str()),          strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_Gain).c_str()),          "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "Expander LPF",             strdup( NTS(MC_Expander_LPF).c_str()),           strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_LPF).c_str()),          "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Expander HPF",             strdup( NTS(MC_Expander_HPF).c_str()),           strdup( NTS(EFX_EXPANDER).c_str()),      strdup( NTS(Expander_HPF).c_str()),          "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "Flanger Dry/Wet",          strdup( NTS(MC_Flanger_DryWet).c_str()),         strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_DryWet).c_str()),       "127",     "-1.0",
        "Flanger Pan",              strdup( NTS(MC_Flanger_Pan).c_str()),            strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Pan).c_str()),            "0",     "1.0",
        "Flanger Tempo",            strdup( NTS(MC_Flanger_LFO_Tempo).c_str()),      strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Tempo).c_str()),      "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Flanger Random",           strdup( NTS(MC_Flanger_LFO_Random).c_str()),     strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Random).c_str()),     "0",     "1.0",
        "Flanger LFO Type",         strdup( NTS(MC_Flanger_LFO_Type).c_str()),       strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Type).c_str()),       "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Flanger Stereo Df",        strdup( NTS(MC_Flanger_LFO_Stereo).c_str()),     strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LFO_Stereo).c_str()),     "0",     "1.0",
        "Flanger Depth",            strdup( NTS(MC_Flanger_Depth).c_str()),          strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Depth).c_str()),          "0",     "1.0",
        "Flanger Delay",            strdup( NTS(MC_Flanger_Delay).c_str()),          strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Delay).c_str()),          "0",     "1.0",
        "Flanger Feedback",         strdup( NTS(MC_Flanger_Feedback).c_str()),       strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_Feedback).c_str()),       "0",     "1.0",
        "Flanger L/R Cross",        strdup( NTS(MC_Flanger_LR_Cross).c_str()),       strdup( NTS(EFX_FLANGER).c_str()),       strdup( NTS(Flanger_LR_Cross).c_str()),       "0",     "1.0",

        "Harmonizer Dry/Wet",       strdup( NTS(MC_Harm_DryWet).c_str()),            strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_DryWet).c_str()),          "127",     "-1.0",
        "Harmonizer Interval",      strdup( NTS(MC_Harm_Interval).c_str()),          strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Interval).c_str()),          "0",     strdup( NTS(C_MC_24_RANGE).c_str()),
        "Harmonizer Gain",          strdup( NTS(MC_Harm_Gain).c_str()),              strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Gain).c_str()),              "0",     "1.0",
        "Harmonizer Pan",           strdup( NTS(MC_Harm_Pan).c_str()),               strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Pan).c_str()),               "0",     "1.0",
        "Harmonizer Freq",          strdup( NTS(MC_Harm_Filter_Freq).c_str()),       strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Filter_Freq).c_str()),      "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Harmonizer Filter Gain",   strdup( NTS(MC_Harm_Filter_Gain).c_str()),       strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Filter_Gain).c_str()),       "0",     "1.0",
        "Harmonizer Filter Q",      strdup( NTS(MC_Harm_Filter_Q).c_str()),          strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Filter_Q).c_str()),          "0",     "1.0",
        "Harmonizer SELECT",        strdup( NTS(MC_Harm_Select).c_str()),            strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Select).c_str()),            "0",     "1.0",
        "Harmonizer Note",          strdup( NTS(MC_Harm_Note).c_str()),              strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Note).c_str()),              "0",     strdup( NTS(C_MC_23_RANGE).c_str()),
        "Harmonizer Chord",         strdup( NTS(MC_Harm_Chord).c_str()),             strdup( NTS(EFX_HARMONIZER).c_str()),    strdup( NTS(Harm_Chord).c_str()),             "0",     strdup( NTS(C_MC_33_RANGE).c_str()),

        "Infinity Dry/Wet",         strdup( NTS(MC_Infinity_DryWet).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_DryWet).c_str()),      "127",     "-1.0",
        "Infinity Res",             strdup( NTS(MC_Infinity_Resonance).c_str()),     strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Resonance).c_str()), "-1000",     strdup( NTS(C_MC_2000_RANGE).c_str()),
        "Infinity Filter Band 1",   strdup( NTS(MC_Infinity_Band_1).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_1).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 2",   strdup( NTS(MC_Infinity_Band_2).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_2).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 3",   strdup( NTS(MC_Infinity_Band_3).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_3).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 4",   strdup( NTS(MC_Infinity_Band_4).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_4).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 5",   strdup( NTS(MC_Infinity_Band_5).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_5).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 6",   strdup( NTS(MC_Infinity_Band_6).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_6).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 7",   strdup( NTS(MC_Infinity_Band_7).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_7).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Filter Band 8",   strdup( NTS(MC_Infinity_Band_8).c_str()),        strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Band_8).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity AutoPan",         strdup( NTS(MC_Infinity_AutoPan).c_str()),       strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_AutoPan).c_str()),       "0",     "1.0",
        "Infinity Stereo Df",       strdup( NTS(MC_Infinity_LR_Delay).c_str()),      strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_LR_Delay).c_str()),    "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Infinity Start",           strdup( NTS(MC_Infinity_Start).c_str()),         strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Start).c_str()),         "0",     "1.0",
        "Infinity End",             strdup( NTS(MC_Infinity_End).c_str()),           strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_End).c_str()),           "0",     "1.0",
        "Infinity Tempo",           strdup( NTS(MC_Infinity_Tempo).c_str()),         strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Infinity Subdiv",          strdup( NTS(MC_Infinity_Subdivision).c_str()),   strdup( NTS(EFX_INFINITY).c_str()),      strdup( NTS(Infinity_Subdivision).c_str()), "-16",     strdup( NTS(C_MC_32_RANGE).c_str()),

        "Input Volume",             strdup( NTS(MC_Input_Volume).c_str()),           strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "Looper Dry/Wet",           strdup( NTS(MC_Looper_DryWet).c_str()),          strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_DryWet).c_str()),        "127",     "-1.0",
        "Looper Level 1",           strdup( NTS(MC_Looper_Level_1).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Level_1).c_str()),         "0",     "1.0",
        "Looper Level 2",           strdup( NTS(MC_Looper_Level_2).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Level_2).c_str()),         "0",     "1.0",
        "Looper Tempo",             strdup( NTS(MC_Looper_Tempo).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Tempo).c_str()),          "20",     strdup( NTS(C_MC_360_RANGE).c_str()),
        "Looper Reverse",           strdup( NTS(MC_Looper_Reverse).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Reverse).c_str()),         "0",     "1.0",
        "Looper Auto Play",         strdup( NTS(MC_Looper_AutoPlay).c_str()),        strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_AutoPlay).c_str()),        "0",     "1.0",
        "Looper Play",              strdup( NTS(MC_Looper_Play).c_str()),            strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Play).c_str()),            "0",     "1.0",
        "Looper Pause",             strdup( NTS(MC_Looper_Stop).c_str()),            strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Stop).c_str()),            "0",     "1.0",
        "Looper Record",            strdup( NTS(MC_Looper_Record).c_str()),          strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Record).c_str()),          "0",     "1.0",
        "Looper R1",                strdup( NTS(MC_Looper_Rec_1).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Rec_1).c_str()),           "0",     "1.0",
        "Looper R2",                strdup( NTS(MC_Looper_Rec_2).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Rec_2).c_str()),           "0",     "1.0",
        "Looper Track 1",           strdup( NTS(MC_Looper_Track_1).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Track_1).c_str()),         "0",     "1.0",
        "Looper Track 2",           strdup( NTS(MC_Looper_Track_2).c_str()),         strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Track_2).c_str()),         "0",     "1.0",
        "Looper Clear",             strdup( NTS(MC_Looper_Clear).c_str()),           strdup( NTS(EFX_LOOPER).c_str()),        strdup( NTS(Looper_Clear).c_str()),           "0",     "1.0",

        "Multi On/Off",             strdup( NTS(MC_Multi_On_Off).c_str()),           strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "Musical Delay Dry/Wet",    strdup( NTS(MC_Music_DryWet).c_str()),           strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_DryWet).c_str()),         "127",     "-1.0",
        "Musical Delay L/R Cross",  strdup( NTS(MC_Music_LR_Cross).c_str()),         strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_LR_Cross).c_str()),         "0",     "1.0",
        "Musical Delay Pan 1",      strdup( NTS(MC_Music_Pan_1).c_str()),            strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Pan_1).c_str()),            "0",     "1.0",
        "Musical Delay Pan 2",      strdup( NTS(MC_Music_Pan_2).c_str()),            strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Pan_2).c_str()),            "0",     "1.0",
        "Musical Delay Tempo",      strdup( NTS(MC_Music_Tempo).c_str()),            strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Tempo).c_str()),           "10",     strdup( NTS(C_MC_470_RANGE).c_str()),
        "Musical Delay Gain 1",     strdup( NTS(MC_Music_Gain_1).c_str()),           strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Gain_1).c_str()),           "0",     "1.0",
        "Musical Delay Gain 2",     strdup( NTS(MC_Music_Gain_2).c_str()),           strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Gain_2).c_str()),           "0",     "1.0",
        "Musical Delay FB 1",       strdup( NTS(MC_Music_Feedback_1).c_str()),       strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Feedback_1).c_str()),       "0",     "1.0",
        "Musical Delay FB 2",       strdup( NTS(MC_Music_Feedback_2).c_str()),       strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Feedback_2).c_str()),       "0",     "1.0",
        "Musical Delay Damp",       strdup( NTS(MC_Music_Damp).c_str()),             strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Music_Damp).c_str()),             "0",     "1.0",

        "MuTroMojo Dry/Wet",        strdup( NTS(MC_MuTro_DryWet).c_str()),           strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_DryWet).c_str()),         "127",     "-1.0",
        "MuTroMojo LP",             strdup( NTS(MC_MuTro_LowPass).c_str()),          strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LowPass).c_str()),        "-64",     "1.0",
        "MuTroMojo BP",             strdup( NTS(MC_MuTro_BandPass).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_BandPass).c_str()),       "-64",     "1.0",
        "MuTroMojo HP",             strdup( NTS(MC_MuTro_HighPass).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_HighPass).c_str()),       "-64",     "1.0",
        "MuTroMojo LFO Type",       strdup( NTS(MC_MuTro_LFO_Type).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Type).c_str()),         "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "MuTroMojo Depth",          strdup( NTS(MC_MuTro_Depth).c_str()),            strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Depth).c_str()),            "0",     "1.0",
        "MuTroMojo Tempo",          strdup( NTS(MC_MuTro_LFO_Tempo).c_str()),        strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Tempo).c_str()),        "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "MuTroMojo Res",            strdup( NTS(MC_MuTro_Resonance).c_str()),        strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Resonance).c_str()),        "0",     "1.0",
        "MuTroMojo Range",          strdup( NTS(MC_MuTro_Range).c_str()),            strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Range).c_str()),           "10",     strdup( NTS(C_MC_5990_RANGE).c_str()),
        "MuTroMojo Wah",            strdup( NTS(MC_MuTro_Wah).c_str()),              strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Wah).c_str()),              "0",     "1.0",
        "MuTroMojo E. Sens",        strdup( NTS(MC_MuTro_Env_Sens).c_str()),         strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Env_Sens).c_str()),       "-64",     "1.0",
        "MuTroMojo Smooth",         strdup( NTS(MC_MuTro_Env_Smooth).c_str()),       strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_Env_Smooth).c_str()),       "0",     "1.0",
        "MuTroMojo Random",         strdup( NTS(MC_MuTro_LFO_Random).c_str()),       strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Random).c_str()),       "0",     "1.0",
        "MuTroMojo Stereo Df",      strdup( NTS(MC_MuTro_LFO_Stereo).c_str()),       strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_LFO_Stereo).c_str()),       "0",     "1.0",
        "MuTroMojo St. Freq",       strdup( NTS(MC_MuTro_St_Freq).c_str()),          strdup( NTS(EFX_MUTROMOJO).c_str()),     strdup( NTS(MuTro_St_Freq).c_str()),         "30",     strdup( NTS(C_MC_770_RANGE).c_str()),

        "NoiseGate A. Time",        strdup( NTS(MC_Gate_Attack).c_str()),            strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Attack).c_str()),            "1",     strdup( NTS(C_MC_249_RANGE).c_str()),
        "NoiseGate R. Time",        strdup( NTS(MC_Gate_Release).c_str()),           strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Release).c_str()),           "2",     strdup( NTS(C_MC_248_RANGE).c_str()),
        "NoiseGate Range",          strdup( NTS(MC_Gate_Range).c_str()),             strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Range).c_str()),           "-90",     strdup( NTS(C_MC_90_RANGE).c_str()),
        "NoiseGate Threshold",      strdup( NTS(MC_Gate_Threshold).c_str()),         strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Threshold).c_str()),       "-70",     strdup( NTS(C_MC_90_RANGE).c_str()),
        "NoiseGate Hold",           strdup( NTS(MC_Gate_Hold).c_str()),              strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_Hold).c_str()),              "2",     strdup( NTS(C_MC_498_RANGE).c_str()),
        "NoiseGate LPF",            strdup( NTS(MC_Gate_LPF).c_str()),               strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_LPF).c_str()),              "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "NoiseGate HPF",            strdup( NTS(MC_Gate_HPF).c_str()),               strdup( NTS(EFX_NOISEGATE).c_str()),     strdup( NTS(Gate_HPF).c_str()),              "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "Opticaltrem Depth",        strdup( NTS(MC_Optical_Depth).c_str()),          strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_Depth).c_str()),          "0",     "1.0",
        "Opticaltrem Tempo",        strdup( NTS(MC_Optical_LFO_Tempo).c_str()),      strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Tempo).c_str()),      "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Opticaltrem Random",       strdup( NTS(MC_Optical_LFO_Random).c_str()),     strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Random).c_str()),     "0",     "1.0",
        "Opticaltrem LFO Type",     strdup( NTS(MC_Optical_LFO_Type).c_str()),       strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Type).c_str()),       "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Opticaltrem Stereo Df",    strdup( NTS(MC_Optical_LFO_Stereo).c_str()),     strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_LFO_Stereo).c_str()),     "0",     "1.0",
        "Opticaltrem Pan",          strdup( NTS(MC_Optical_Pan).c_str()),            strdup( NTS(EFX_OPTICALTREM).c_str()),   strdup( NTS(Optical_Pan).c_str()),            "0",     "1.0",

        "Output Volume",            strdup( NTS(MC_Output_Volume).c_str()),          strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "Overdrive Dry/Wet",        strdup( NTS(MC_Overdrive_DryWet).c_str()),       strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_DryWet).c_str()),     "127",     "-1.0",
        "Overdrive L/R Cross",      strdup( NTS(MC_Overdrive_LR_Cross).c_str()),     strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_LR_Cross).c_str()),     "0",     "1.0",
        "Overdrive Drive",          strdup( NTS(MC_Overdrive_Drive).c_str()),        strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Drive).c_str()),        "0",     "1.0",
        "Overdrive Level",          strdup( NTS(MC_Overdrive_Level).c_str()),        strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Level).c_str()),        "0",     "1.0",
        "Overdrive Type",           strdup( NTS(MC_Overdrive_Type).c_str()),         strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Type).c_str()),         "0",     strdup( NTS(C_MC_30_RANGE).c_str()),
        "Overdrive Pan",            strdup( NTS(MC_Overdrive_Pan).c_str()),          strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_Pan).c_str()),          "0",     "1.0",
        "Overdrive LPF",            strdup( NTS(MC_Overdrive_LPF).c_str()),          strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_LPF).c_str()),         "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Overdrive HPF",            strdup( NTS(MC_Overdrive_HPF).c_str()),          strdup( NTS(EFX_OVERDRIVE).c_str()),     strdup( NTS(Overdrive_HPF).c_str()),         "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "Pan Dry/Wet",              strdup( NTS(MC_Pan_DryWet).c_str()),             strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_DryWet).c_str()),           "127",     "-1.0",
        "Pan Pan",                  strdup( NTS(MC_Pan_Pan).c_str()),                strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_Pan).c_str()),                "0",     "1.0",
        "Pan Tempo",                strdup( NTS(MC_Pan_LFO_Tempo).c_str()),          strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Tempo).c_str()),          "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Pan Random",               strdup( NTS(MC_Pan_LFO_Random).c_str()),         strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Random).c_str()),         "0",     "1.0",
        "Pan LFO Type",             strdup( NTS(MC_Pan_LFO_Type).c_str()),           strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Type).c_str()),           "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Pan Stereo Df",            strdup( NTS(MC_Pan_LFO_Stereo).c_str()),         strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_LFO_Stereo).c_str()),         "0",     "1.0",
        "Pan E. Stereo",            strdup( NTS(MC_Pan_Ex_St_Amt).c_str()),          strdup( NTS(EFX_PAN).c_str()),           strdup( NTS(Pan_Ex_St_Amt).c_str()),          "0",     "1.0",

        "Parametric EQ Gain",       strdup( NTS(MC_Parametric_Gain).c_str()),        strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Gain).c_str()),        "0",     "1.0",
        "Parametric EQ Low Freq",   strdup( NTS(MC_Parametric_Low_Freq).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Low_Freq).c_str()),   "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "Parametric EQ Low Gain",   strdup( NTS(MC_Parametric_Low_Gain).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Low_Gain).c_str()),    "0",     "1.0",
        "Parametric EQ Low Q",      strdup( NTS(MC_Parametric_Low_Q).c_str()),       strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Low_Q).c_str()),       "0",     "1.0",
        "Parametric EQ Mid Freq",   strdup( NTS(MC_Parametric_Mid_Freq).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Mid_Freq).c_str()),  "800",     strdup( NTS(C_MC_7200_RANGE).c_str()),
        "Parametric EQ Mid Gain",   strdup( NTS(MC_Parametric_Mid_Gain).c_str()),    strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Mid_Gain).c_str()),    "0",     "1.0",
        "Parametric EQ Mid Q",      strdup( NTS(MC_Parametric_Mid_Q).c_str()),       strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_Mid_Q).c_str()),       "0",     "1.0",
        "Parametric EQ High Freq",  strdup( NTS(MC_Parametric_High_Freq).c_str()),   strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_High_Freq).c_str()),"6000",     strdup( NTS(C_MC_20000_RANGE).c_str()),
        "Parametric EQ High Gain",  strdup( NTS(MC_Parametric_High_Gain).c_str()),   strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_High_Gain).c_str()),   "0",     "1.0",
        "Parametric EQ High Q",     strdup( NTS(MC_Parametric_High_Q).c_str()),      strdup( NTS(EFX_PARAMETRIC).c_str()),    strdup( NTS(Parametric_High_Q).c_str()),      "0",     "1.0",

        "Phaser Dry/Wet",           strdup( NTS(MC_Phaser_DryWet).c_str()),          strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_DryWet).c_str()),        "127",     "-1.0",
        "Phaser Pan",               strdup( NTS(MC_Phaser_Pan).c_str()),             strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Pan).c_str()),             "0",     "1.0",
        "Phaser Tempo",             strdup( NTS(MC_Phaser_LFO_Tempo).c_str()),       strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Phaser Random",            strdup( NTS(MC_Phaser_LFO_Random).c_str()),      strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Random).c_str()),      "0",     "1.0",
        "Phaser LFO Type",          strdup( NTS(MC_Phaser_LFO_Type).c_str()),        strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Phaser Phase",             strdup( NTS(MC_Phaser_Phase).c_str()),           strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Phase).c_str()),           "0",     "1.0",
        "Phaser Stereo Df",         strdup( NTS(MC_Phaser_LFO_Stereo).c_str()),      strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LFO_Stereo).c_str()),      "0",     "1.0",
        "Phaser Depth",             strdup( NTS(MC_Phaser_Depth).c_str()),           strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Depth).c_str()),           "0",     "1.0",
        "Phaser Feedback",          strdup( NTS(MC_Phaser_Feedback).c_str()),        strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_Feedback).c_str()),        "0",     "1.0",
        "Phaser L/R Cross",         strdup( NTS(MC_Phaser_LR_Cross).c_str()),        strdup( NTS(EFX_PHASER).c_str()),        strdup( NTS(Phaser_LR_Cross).c_str()),        "0",     "1.0",

        "Program Change Table",     strdup( NTS(MC_Program_Table).c_str()),          strdup( NTS(EFX_MASTER).c_str()),        "0",                                 "0",     "1.0",

        "Reverb Dry/Wet",           strdup( NTS(MC_Reverb_DryWet).c_str()),          strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_DryWet).c_str()),        "127",     "-1.0",
        "Reverb Pan",               strdup( NTS(MC_Reverb_Pan).c_str()),             strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Pan).c_str()),             "0",     "1.0",
        "Reverb Time",              strdup( NTS(MC_Reverb_Time).c_str()),            strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Time).c_str()),            "0",     "1.0",
        "Reverb Initial Delay",     strdup( NTS(MC_Reverb_I_Delay).c_str()),         strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_I_Delay).c_str()),         "0",     "1.0",
        "Reverb Del. E/R",          strdup( NTS(MC_Reverb_Delay_FB).c_str()),        strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Delay_FB).c_str()),        "0",     "1.0",
        "Reverb Room Size",         strdup( NTS(MC_Reverb_Room).c_str()),            strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Room).c_str()),            "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "Reverb LPF",               strdup( NTS(MC_Reverb_LPF).c_str()),             strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_LPF).c_str()),            "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Reverb HPF",               strdup( NTS(MC_Reverb_HPF).c_str()),             strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_HPF).c_str()),            "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),
        "Reverb Damping",           strdup( NTS(MC_Reverb_Damp).c_str()),            strdup( NTS(EFX_REVERB).c_str()),        strdup( NTS(Reverb_Damp).c_str()),           "64",     strdup( NTS(C_MC_63_RANGE).c_str()),

        "Reverbtron Dry/Wet",       strdup( NTS(MC_Revtron_DryWet).c_str()),         strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_DryWet).c_str()),       "127",     "-1.0",
        "Reverbtron Pan",           strdup( NTS(MC_Revtron_Pan).c_str()),            strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Pan).c_str()),            "0",     "1.0",
        "Reverbtron Level",         strdup( NTS(MC_Revtron_Level).c_str()),          strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Level).c_str()),          "0",     "1.0",
        "Reverbtron Damp",          strdup( NTS(MC_Revtron_Damp).c_str()),           strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Damp).c_str()),           "0",     "1.0",
        "Reverbtron Feedback",      strdup( NTS(MC_Revtron_Feedback).c_str()),       strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Feedback).c_str()),     "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Reverbtron Length",        strdup( NTS(MC_Revtron_Length).c_str()),         strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Length).c_str()),        "20",     strdup( NTS(C_MC_1480_RANGE).c_str()),
        "Reverbtron Stretch",       strdup( NTS(MC_Revtron_Stretch).c_str()),        strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Stretch).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Reverbtron Initial Delay", strdup( NTS(MC_Revtron_I_Delay).c_str()),        strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_I_Delay).c_str()),        "0",     strdup( NTS(C_MC_500_RANGE).c_str()),
        "Reverbtron Fade",          strdup( NTS(MC_Revtron_Fade).c_str()),           strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Fade).c_str()),           "0",     "1.0",
        "Reverbtron Diffusion",     strdup( NTS(MC_Revtron_Diffusion).c_str()),      strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_Diffusion).c_str()),      "0",     "1.0",
        "Reverbtron LPF",           strdup( NTS(MC_Revtron_LPF).c_str()),            strdup( NTS(EFX_REVERBTRON).c_str()),    strdup( NTS(Revtron_LPF).c_str()),           "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),

        "Ring Dry/Wet",             strdup( NTS(MC_Ring_DryWet).c_str()),            strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_DryWet).c_str()),          "127",     "-1.0",
        "Ring L/R Cross",           strdup( NTS(MC_Ring_LR_Cross).c_str()),          strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_LR_Cross).c_str()),        "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Ring Input",               strdup( NTS(MC_Ring_Input).c_str()),             strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Input).c_str()),             "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "Ring Level",               strdup( NTS(MC_Ring_Level).c_str()),             strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Level).c_str()),             "0",     "1.0",
        "Ring Pan",                 strdup( NTS(MC_Ring_Pan).c_str()),               strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Pan).c_str()),             "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Ring Depth",               strdup( NTS(MC_Ring_Depth).c_str()),             strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Depth).c_str()),             "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Ring Freq",                strdup( NTS(MC_Ring_Freq).c_str()),              strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Freq).c_str()),              "1",     strdup( NTS(C_MC_19999_RANGE).c_str()),
        "Ring Sine",                strdup( NTS(MC_Ring_Sine).c_str()),              strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Sine).c_str()),              "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Ring Triangle",            strdup( NTS(MC_Ring_Triangle).c_str()),          strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Triangle).c_str()),          "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Ring Sawtooth",            strdup( NTS(MC_Ring_Saw).c_str()),               strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Saw).c_str()),               "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Ring Square",              strdup( NTS(MC_Ring_Square).c_str()),            strdup( NTS(EFX_RING).c_str()),          strdup( NTS(Ring_Square).c_str()),            "0",     strdup( NTS(C_MC_100_RANGE).c_str()),

        "Sequence Dry/Wet",         strdup( NTS(MC_Sequence_DryWet).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_DryWet).c_str()),      "127",     "-1.0",
        "Sequence 1",               strdup( NTS(MC_Sequence_Step_1).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_1).c_str()),        "0",     "1.0",
        "Sequence 2",               strdup( NTS(MC_Sequence_Step_2).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_2).c_str()),        "0",     "1.0",
        "Sequence 3",               strdup( NTS(MC_Sequence_Step_3).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_3).c_str()),        "0",     "1.0",
        "Sequence 4",               strdup( NTS(MC_Sequence_Step_4).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_4).c_str()),        "0",     "1.0",
        "Sequence 5",               strdup( NTS(MC_Sequence_Step_5).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_5).c_str()),        "0",     "1.0",
        "Sequence 6",               strdup( NTS(MC_Sequence_Step_6).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_6).c_str()),        "0",     "1.0",
        "Sequence 7",               strdup( NTS(MC_Sequence_Step_7).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_7).c_str()),        "0",     "1.0",
        "Sequence 8",               strdup( NTS(MC_Sequence_Step_8).c_str()),        strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Step_8).c_str()),        "0",     "1.0",
        "Sequence Tempo",           strdup( NTS(MC_Sequence_Tempo).c_str()),         strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Sequence Q",               strdup( NTS(MC_Sequence_Resonance).c_str()),     strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Resonance).c_str()),     "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Sequence Stereo Df",       strdup( NTS(MC_Sequence_Stdf).c_str()),          strdup( NTS(EFX_SEQUENCE).c_str()),      strdup( NTS(Sequence_Stdf).c_str()),          "0",     strdup( NTS(C_MC_7_RANGE).c_str()),

        "ShelfBoost Gain",          strdup( NTS(MC_Shelf_Gain).c_str()),             strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Gain).c_str()),             "0",     "1.0",
        "ShelfBoost Level",         strdup( NTS(MC_Shelf_Level).c_str()),            strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Level).c_str()),            "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "ShelfBoost Tone",          strdup( NTS(MC_Shelf_Tone).c_str()),             strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Tone).c_str()),           "220",     strdup( NTS(C_MC_15780_RANGE).c_str()),
        "ShelfBoost Presence",      strdup( NTS(MC_Shelf_Presence).c_str()),         strdup( NTS(EFX_SHELFBOOST).c_str()),    strdup( NTS(Shelf_Presence).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "Shifter Dry/Wet",          strdup( NTS(MC_Shifter_DryWet).c_str()),         strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_DryWet).c_str()),       "127",     "-1.0",
        "Shifter Interval",         strdup( NTS(MC_Shifter_Interval).c_str()),       strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Interval).c_str()),       "0",     strdup( NTS(C_MC_12_RANGE).c_str()),
        "Shifter Gain",             strdup( NTS(MC_Shifter_Gain).c_str()),           strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Gain).c_str()),           "0",     "1.0",
        "Shifter Pan",              strdup( NTS(MC_Shifter_Pan).c_str()),            strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Pan).c_str()),            "0",     "1.0",
        "Shifter Attack",           strdup( NTS(MC_Shifter_Attack).c_str()),         strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Attack).c_str()),         "1",     strdup( NTS(C_MC_1999_RANGE).c_str()),
        "Shifter Decay",            strdup( NTS(MC_Shifter_Decay).c_str()),          strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Decay).c_str()),          "1",     strdup( NTS(C_MC_1999_RANGE).c_str()),
        "Shifter Threshold",        strdup( NTS(MC_Shifter_Threshold).c_str()),      strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Threshold).c_str()),    "-70",     strdup( NTS(C_MC_90_RANGE).c_str()),
        "Shifter Whammy",           strdup( NTS(MC_Shifter_Whammy).c_str()),         strdup( NTS(EFX_SHIFTER).c_str()),       strdup( NTS(Shifter_Whammy).c_str()),         "0",     "1.0",

        "Shuffle Dry/Wet",          strdup( NTS(MC_Shuffle_DryWet).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_DryWet).c_str()),       "127",     "-1.0",
        "Shuffle Low Freq",         strdup( NTS(MC_Shuffle_Freq_L).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_L).c_str()),        "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "Shuffle Low Gain",         strdup( NTS(MC_Shuffle_Gain_L).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_L).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Shuffle M.L. Freq",        strdup( NTS(MC_Shuffle_Freq_ML).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_ML).c_str()),      "400",     strdup( NTS(C_MC_3600_RANGE).c_str()),
        "Shuffle M.L. Gain",        strdup( NTS(MC_Shuffle_Gain_ML).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_ML).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Shuffle M.H. Freq",        strdup( NTS(MC_Shuffle_Freq_MH).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_MH).c_str()),     "1200",     strdup( NTS(C_MC_6800_RANGE).c_str()),
        "Shuffle M.H. Gain",        strdup( NTS(MC_Shuffle_Gain_MH).c_str()),        strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_MH).c_str()),      "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Shuffle High Freq",        strdup( NTS(MC_Shuffle_Freq_H).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Freq_H).c_str()),      "6000",     strdup( NTS(C_MC_20000_RANGE).c_str()),
        "Shuffle High Gain",        strdup( NTS(MC_Shuffle_Gain_H).c_str()),         strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Gain_H).c_str()),       "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Shuffle Q",                strdup( NTS(MC_Shuffle_Width).c_str()),          strdup( NTS(EFX_SHUFFLE).c_str()),       strdup( NTS(Shuffle_Width).c_str()),        "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "StereoHarm Dry/Wet",       strdup( NTS(MC_Sharm_DryWet).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_DryWet).c_str()),         "127",     "-1.0",
        "StereoHarm Int L",         strdup( NTS(MC_Sharm_L_Interval).c_str()),       strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_L_Interval).c_str()),       "0",     strdup( NTS(C_MC_24_RANGE).c_str()),
        "StereoHarm Chrm L",        strdup( NTS(MC_Sharm_L_Chroma).c_str()),         strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_L_Chroma).c_str()),     "-2000",     strdup( NTS(C_MC_4000_RANGE).c_str()),
        "StereoHarm Gain L",        strdup( NTS(MC_Sharm_L_Gain).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_L_Gain).c_str()),           "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "StereoHarm Int R",         strdup( NTS(MC_Sharm_R_Interval).c_str()),       strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_R_Interval).c_str()),       "0",     strdup( NTS(C_MC_24_RANGE).c_str()),
        "StereoHarm Chrm R",        strdup( NTS(MC_Sharm_R_Chroma).c_str()),         strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_R_Chroma).c_str()),     "-2000",     strdup( NTS(C_MC_4000_RANGE).c_str()),
        "StereoHarm Gain R",        strdup( NTS(MC_Sharm_R_Gain).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_R_Gain).c_str()),           "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "StereoHarm L/R Cross",     strdup( NTS(MC_Sharm_LR_Cross).c_str()),         strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_LR_Cross).c_str()),         "0",     "1.0",
        "StereoHarm SELECT",        strdup( NTS(MC_Sharm_Select).c_str()),           strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_Select).c_str()),           "0",     "1.0",
        "StereoHarm Note",          strdup( NTS(MC_Sharm_Note).c_str()),             strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_Note).c_str()),             "0",     strdup( NTS(C_MC_23_RANGE).c_str()),
        "StereoHarm Chord",         strdup( NTS(MC_Sharm_Chord).c_str()),            strdup( NTS(EFX_STEREOHARM).c_str()),    strdup( NTS(Sharm_Chord).c_str()),            "0",     strdup( NTS(C_MC_33_RANGE).c_str()),

        "StompBox Level",           strdup( NTS(MC_Stomp_Level).c_str()),            strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Level).c_str()),            "0",     "1.0",
        "StompBox Gain",            strdup( NTS(MC_Stomp_Gain).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Gain).c_str()),             "0",     "1.0",
        "StompBox Bias",            strdup( NTS(MC_Stomp_Bias).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Bias).c_str()),           "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "StompBox Mid",             strdup( NTS(MC_Stomp_Mid).c_str()),              strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Mid).c_str()),            "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "StompBox Tone",            strdup( NTS(MC_Stomp_Tone).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Tone).c_str()),           "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "StompBox Mode",            strdup( NTS(MC_Stomp_Mode).c_str()),             strdup( NTS(EFX_STOMPBOX).c_str()),      strdup( NTS(Stomp_Mode).c_str()),             "0",     strdup( NTS(C_MC_8_RANGE).c_str()),

        "Sustainer Gain",           strdup( NTS(MC_Sustain_Gain).c_str()),           strdup( NTS(EFX_SUSTAINER).c_str()),     strdup( NTS(Sustain_Gain).c_str()),           "0",     "1.0",
        "Sustainer Sustain",        strdup( NTS(MC_Sustain_Sustain).c_str()),        strdup( NTS(EFX_SUSTAINER).c_str()),     strdup( NTS(Sustain_Sustain).c_str()),        "1",     strdup( NTS(C_MC_126_RANGE).c_str()),

        "Synthfilter Dry/Wet",      strdup( NTS(MC_Synthfilter_DryWet).c_str()),     strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_DryWet).c_str()),   "127",     "-1.0",
        "Synthfilter Distort",      strdup( NTS(MC_Synthfilter_Distort).c_str()),    strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Distort).c_str()),    "0",     "1.0",
        "Synthfilter Tempo",        strdup( NTS(MC_Synthfilter_LFO_Tempo).c_str()),  strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Tempo).c_str()),  "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Synthfilter Random",       strdup( NTS(MC_Synthfilter_LFO_Random).c_str()), strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Random).c_str()), "0",     "1.0",
        "Synthfilter LFO Type",     strdup( NTS(MC_Synthfilter_LFO_Type).c_str()),   strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Type).c_str()),   "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Synthfilter Stereo Df",    strdup( NTS(MC_Synthfilter_LFO_Stereo).c_str()), strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_LFO_Stereo).c_str()), "0",     "1.0",
        "Synthfilter Width",        strdup( NTS(MC_Synthfilter_Width).c_str()),      strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Width).c_str()),      "0",     "1.0",
        "Synthfilter Feedback",     strdup( NTS(MC_Synthfilter_Feedback).c_str()),   strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Feedback).c_str()), "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Synthfilter Depth",        strdup( NTS(MC_Synthfilter_Depth).c_str()),      strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Depth).c_str()),      "0",     "1.0",
        "Synthfilter E.Sens",       strdup( NTS(MC_Synthfilter_Env_Sens).c_str()),   strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Env_Sens).c_str()), "-64",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Synthfilter A.Time",       strdup( NTS(MC_Synthfilter_Attack).c_str()),     strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Attack).c_str()),     "5",     strdup( NTS(C_MC_995_RANGE).c_str()),
        "Synthfilter R.Time",       strdup( NTS(MC_Synthfilter_Release).c_str()),    strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Release).c_str()),    "5",     strdup( NTS(C_MC_495_RANGE).c_str()),
        "Synthfilter Offset",       strdup( NTS(MC_Synthfilter_Offset).c_str()),     strdup( NTS(EFX_SYNTHFILTER).c_str()),   strdup( NTS(Synthfilter_Offset).c_str()),     "0",     "1.0",

        "Valve Dry/Wet",            strdup( NTS(MC_Valve_DryWet).c_str()),           strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_DryWet).c_str()),         "127",     "-1.0",
        "Valve L/R Cross",          strdup( NTS(MC_Valve_LR_Cross).c_str()),         strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_LR_Cross).c_str()),         "0",     "1.0",
        "Valve Pan",                strdup( NTS(MC_Valve_Pan).c_str()),              strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Pan).c_str()),              "0",     "1.0",
        "Valve Level",              strdup( NTS(MC_Valve_Level).c_str()),            strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Level).c_str()),            "0",     "1.0",
        "Valve Drive",              strdup( NTS(MC_Valve_Drive).c_str()),            strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Drive).c_str()),            "0",     "1.0",
        "Valve Dist",               strdup( NTS(MC_Valve_Distortion).c_str()),       strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Distortion).c_str()),       "0",     "1.0",
        "Valve Presence",           strdup( NTS(MC_Valve_Presence).c_str()),         strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_Presence).c_str()),         "0",     strdup( NTS(C_MC_100_RANGE).c_str()),
        "Valve LPF",                strdup( NTS(MC_Valve_LPF).c_str()),              strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_LPF).c_str()),             "20",     strdup( NTS(C_MC_25980_RANGE).c_str()),
        "Valve HPF",                strdup( NTS(MC_Valve_HPF).c_str()),              strdup( NTS(EFX_VALVE).c_str()),         strdup( NTS(Valve_HPF).c_str()),             "20",     strdup( NTS(C_MC_19980_RANGE).c_str()),

        "VaryBand Dry/Wet",         strdup( NTS(MC_VaryBand_DryWet).c_str()),        strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_DryWet).c_str()),      "127",     "-1.0",
        "VaryBand Tempo 1",         strdup( NTS(MC_VaryBand_LFO_Tempo_1).c_str()),   strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Tempo_1).c_str()),   "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "VaryBand LFO 1 Type",      strdup( NTS(MC_VaryBand_LFO_Type_1).c_str()),    strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Type_1).c_str()),    "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "VaryBand St.df 1",         strdup( NTS(MC_VaryBand_LFO_Stereo_1).c_str()),  strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Stereo_1).c_str()),  "0",     "1.0",
        "VaryBand Tempo 2",         strdup( NTS(MC_VaryBand_LFO_Tempo_2).c_str()),   strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Tempo_2).c_str()),   "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "VaryBand LFO 2 Type",      strdup( NTS(MC_VaryBand_LFO_Type_2).c_str()),    strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Type_2).c_str()),    "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "VaryBand St.df 2",         strdup( NTS(MC_VaryBand_LFO_Stereo_2).c_str()),  strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_LFO_Stereo_2).c_str()),  "0",     "1.0",
        "VaryBand Cross 1",         strdup( NTS(MC_VaryBand_Cross_1).c_str()),       strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_Cross_1).c_str()),      "20",     strdup( NTS(C_MC_980_RANGE).c_str()),
        "VaryBand Cross 2",         strdup( NTS(MC_VaryBand_Cross_2).c_str()),       strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_Cross_2).c_str()),    "1000",     strdup( NTS(C_MC_7000_RANGE).c_str()),
        "VaryBand Cross 3",         strdup( NTS(MC_VaryBand_Cross_3).c_str()),       strdup( NTS(EFX_VARYBAND).c_str()),      strdup( NTS(VaryBand_Cross_3).c_str()),    "2000",     strdup( NTS(C_MC_24000_RANGE).c_str()),

        "Vibe Dry/Wet",             strdup( NTS(MC_Vibe_DryWet).c_str()),            strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_DryWet).c_str()),          "127",     "-1.0",
        "Vibe Width",               strdup( NTS(MC_Vibe_Width).c_str()),             strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Width).c_str()),             "0",     "1.0",
        "Vibe Depth",               strdup( NTS(MC_Vibe_Depth).c_str()),             strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Depth).c_str()),             "0",     "1.0",
        "Vibe Tempo",               strdup( NTS(MC_Vibe_LFO_Tempo).c_str()),         strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Tempo).c_str()),         "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "Vibe Random",              strdup( NTS(MC_Vibe_LFO_Random).c_str()),        strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Random).c_str()),        "0",     "1.0",
        "Vibe LFO Type",            strdup( NTS(MC_Vibe_LFO_Type).c_str()),          strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Type).c_str()),          "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "Vibe Stereo Df",           strdup( NTS(MC_Vibe_LFO_Stereo).c_str()),        strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LFO_Stereo).c_str()),        "0",     "1.0",
        "Vibe Feedback",            strdup( NTS(MC_Vibe_Feedback).c_str()),          strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Feedback).c_str()),          "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Vibe L/R Cross",           strdup( NTS(MC_Vibe_LR_Cross).c_str()),          strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_LR_Cross).c_str()),          "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Vibe Pan",                 strdup( NTS(MC_Vibe_Pan).c_str()),               strdup( NTS(EFX_VIBE).c_str()),          strdup( NTS(Vibe_Pan).c_str()),               "0",     strdup( NTS(C_MC_128_RANGE).c_str()),

        "Vocoder Dry/Wet",          strdup( NTS(MC_Vocoder_DryWet).c_str()),         strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_DryWet).c_str()),       "127",     "-1.0",
        "Vocoder Pan",              strdup( NTS(MC_Vocoder_Pan).c_str()),            strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Pan).c_str()),            "0",     strdup( NTS(C_MC_128_RANGE).c_str()),
        "Vocoder Input",            strdup( NTS(MC_Vocoder_Input).c_str()),          strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Input).c_str()),          "0",     "1.0",
        "Vocoder Smear",            strdup( NTS(MC_Vocoder_Smear).c_str()),          strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Smear).c_str()),          "1",     strdup( NTS(C_MC_126_RANGE).c_str()),
        "Vocoder Q",                strdup( NTS(MC_Vocoder_Q).c_str()),              strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Q).c_str()),             "40",     strdup( NTS(C_MC_130_RANGE).c_str()),
        "Vocoder Ring",             strdup( NTS(MC_Vocoder_Ring).c_str()),           strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Ring).c_str()),           "0",     "1.0",
        "Vocoder Level",            strdup( NTS(MC_Vocoder_Level).c_str()),          strdup( NTS(EFX_VOCODER).c_str()),       strdup( NTS(Vocoder_Level).c_str()),          "0",     "1.0",

        "WahWah Dry/Wet",           strdup( NTS(MC_WahWah_DryWet).c_str()),          strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_DryWet).c_str()),        "127",     "-1.0",
        "WahWah Pan",               strdup( NTS(MC_WahWah_Pan).c_str()),             strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Pan).c_str()),             "0",     "1.0",
        "WahWah Tempo",             strdup( NTS(MC_WahWah_LFO_Tempo).c_str()),       strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Tempo).c_str()),       "1",     strdup( NTS(C_MC_600_RANGE).c_str()),
        "WahWah Random",            strdup( NTS(MC_WahWah_LFO_Random).c_str()),      strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Random).c_str()),      "0",     "1.0",
        "WahWah LFO Type",          strdup( NTS(MC_WahWah_LFO_Type).c_str()),        strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Type).c_str()),        "0",     strdup( NTS(C_MC_11_RANGE).c_str()),
        "WahWah Stereo Df",         strdup( NTS(MC_WahWah_LFO_Stereo).c_str()),      strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_LFO_Stereo).c_str()),      "0",     "1.0",
        "WahWah Depth",             strdup( NTS(MC_WahWah_Depth).c_str()),           strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Depth).c_str()),           "0",     "1.0",
        "WahWah Amp S.",            strdup( NTS(MC_WahWah_Sense).c_str()),           strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Sense).c_str()),           "0",     "1.0",
        "WahWah Amp S.I.",          strdup( NTS(MC_WahWah_ASI).c_str()),             strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_ASI).c_str()),             "0",     "1.0",
        "WahWah Smooth",            strdup( NTS(MC_WahWah_Smooth).c_str()),          strdup( NTS(EFX_WAHWAH).c_str()),        strdup( NTS(WahWah_Smooth).c_str()),          "0",     "1.0",
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
 *  Alsa MIDI connection.
 *  If auto connect is requested, then make the connections.
 */
void
RKR::ConnectMIDI()
{
    if (Config.aconnect_MI)
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

    if ((Tap_Active) && (Tap_Selection == 3) && (midievent->type == SND_SEQ_EVENT_CLOCK))
    {
        mtc_counter++;
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    if ((EFX_Active[EFX_LOOPER]) && (Tap_Selection == 3))
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

        if ((Tap_Active) && (Tap_Selection == 1) && (midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
            Tap_TempoSet = TapTempo();

        if (midievent->data.note.channel == Config.Harmonizer_MIDI_Channel)
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

        if (midievent->data.note.channel == Config.StereoHarm_MIDI_Channel)
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
        if (midievent->data.control.channel == Config.MIDI_In_Channel)
        {
            if (!Config.custom_midi_table)
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
            {
                int bank = MIDI_Table[midievent->data.control.value].bank;
                int preset = MIDI_Table[midievent->data.control.value].preset + 1;
                process_midi_controller_events(0, bank, preset);  // 0 is CC 0 Bank Select
            }
        }
    }

    if (midievent->type == SND_SEQ_EVENT_CONTROLLER)
    {
        if (midievent->data.control.channel == Config.MIDI_In_Channel)
        {
            if (RControl)
            {
                ControlGet = (int) midievent->data.control.param;
                return;
            }
            
            // Bank_Select = CC 0
            if(midievent->data.control.param == 0)
            {
                process_midi_controller_events((int) midievent->data.control.param,
                                               (int) midievent->data.control.value);
            }
            else if (Config.MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (Active_Preset.XUserMIDI[(int) midievent->data.control.param][i])
                        process_midi_controller_events(Active_Preset.XUserMIDI[(int) midievent->data.control.param][i],
                                                       (int) midievent->data.control.value);
                    else break;
                }
            }
            else
                process_midi_controller_events((int) midievent->data.control.param,
                                               (int) midievent->data.control.value);
        }
    }
#ifdef SYSEX_SUPPORT
    if (midievent->type == SND_SEQ_EVENT_SYSEX)
    {
        /* temp for midi data */
        unsigned char buffer[0x1000];
        bool sysex = false;
        
        snd_midi_event_t *midi_ev;
        snd_midi_event_new(sizeof(buffer), &midi_ev);
        long bytes = snd_midi_event_decode(midi_ev, buffer, sizeof(buffer), midievent);

        if (bytes <= 0)
        {
            snd_midi_event_free( midi_ev );
            return;
        }

        start_sysex( );
        sysex = append_sysex( buffer, bytes );

        /* sysex messages might be more than one message */
        while (sysex)
        {
            snd_seq_event_input(midi_in, &midievent);

            bytes = snd_midi_event_decode(midi_ev, buffer, sizeof(buffer), midievent);

            if (bytes > 0)
                sysex = append_sysex( buffer, bytes );
            else
                sysex = false;
        }

        snd_midi_event_free( midi_ev );

        parse_sysex();
    }
#endif // SYSEX_SUPPORT
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
                if (inoff) EFX_Active[i] = 1;
                else EFX_Active[i] = 0;
                return;
            }
        }
    }
    else
    {
        switch (miraque)
        {
            case EFX_TAP_TEMPO_ON_OFF:
                if (inoff) Tap_Active = 1;
                else Tap_Active = 0;
                break;
            case EFX_MIDI_CONVERTER_ON_OFF:
                if (inoff) MIDIConverter_Active = 1;
                else MIDIConverter_Active = 0;
                break;
            case EFX_TUNER_ON_OFF:
                if (inoff) Tuner_Active = 1;
                else Tuner_Active = 0;
                break;
            case EFX_MASTER_ON_OFF:
                if (inoff) Active_Preset.FX_Master_Active = 1;
                else Active_Preset.FX_Master_Active = 0;
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
                if (EFX_Active[i])
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
                if (Tap_Active) return 0;
                break;
            case EFX_MIDI_CONVERTER_ON_OFF:
                if (MIDIConverter_Active) return 0;
                break;
            case EFX_TUNER_ON_OFF:
                if (Tuner_Active) return 0;
                break;
            case EFX_MASTER_ON_OFF:
                if (Active_Preset.FX_Master_Active) return 0;
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

    if (IsCoIn)
        disconectaaconnect();

    if ((fp = fopen("/proc/asound/seq/clients", "r")) != NULL)
    {
        char temp[128];
        char temp1[128];
        char temp2[128];
        char *nume;
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
                
                std::string s_name = jackcliname;
                s_name + " IN";
                
                if (strstr(temp, s_name.c_str()) != 0)
                {
                    Cyoin = client;
                    Pyoin = puerto;
                }
                
                if (strstr(temp, Config.MID) != 0)
                {
                    Ccin = client;
                    Pcin = puerto;
                }
            }
        }

        fclose(fp);
    }

    conectaaconnect();
};

void
RKR::conectaaconnect()
{
    if (strlen(Config.MID) != 0)
    {
        char tempi[128];
        memset(tempi, 0, sizeof (tempi));
        sprintf(tempi, "aconnect %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        
        if (system(tempi) == -1)
        {
            Handle_Message(29);
        }
        else
            IsCoIn = 1;
    }
}

void
RKR::disconectaaconnect()
{
    if (strlen(Config.MID) != 0)
    {
        char tempi[128];
        memset(tempi, 0, sizeof (tempi));
        sprintf(tempi, "aconnect -d %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        if (system(tempi) == -1)
        {
            Handle_Message(29);
        }
        else
            IsCoIn = 0;
    }
}

void
RKR::jack_process_midievents(jack_midi_event_t *midievent)
{
    int i;
    int type = midievent->buffer[0] >> 4;

    if ((Tap_Active) && (Tap_Selection == 3) && (midievent->buffer[0] == 0xf8))
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

        if ((Tap_Active) && (Tap_Selection == 1) && (type == 9) && (cmdvelo != 0)) Tap_TempoSet = TapTempo();

        if (cmdchan == Config.Harmonizer_MIDI_Channel)
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

        if (cmdchan == Config.StereoHarm_MIDI_Channel)
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

        if (cmdchan == Config.MIDI_In_Channel)
        {
            if (!Config.custom_midi_table)
            {
                if ((cmdvalue > 0)
                    && (cmdvalue < 61))
                    Change_Preset = cmdvalue;

                if (cmdvalue == 81) if (Selected_Preset > 1) Change_Preset = Selected_Preset - 1;
                
                if (cmdvalue == 82) if (Selected_Preset < 60) Change_Preset = Selected_Preset + 1;
            }
            else
            {
                int bank = MIDI_Table[cmdvalue].bank;
                int preset = MIDI_Table[cmdvalue].preset + 1;
                process_midi_controller_events(0, bank, preset);    // 0 is CC 0 Bank Select
            }
        }
    }

    if (type == 11)
    {
        int cmdcontrol = midievent->buffer[1];
        int cmdvalue = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == Config.MIDI_In_Channel)
        {
            if (RControl)
            {
                ControlGet = cmdcontrol;
                return;
            }
            
            // Bank_Select = CC 0
            if(cmdcontrol == 0)
            {
                process_midi_controller_events(cmdcontrol, cmdvalue);
            }
            else if (Config.MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (Active_Preset.XUserMIDI[cmdcontrol][i])
                        process_midi_controller_events(Active_Preset.XUserMIDI[cmdcontrol][i], cmdvalue);
                    else break;
                }
            }
            else
                process_midi_controller_events(cmdcontrol, cmdvalue);
        }
    }
#ifdef SYSEX_SUPPORT
    if(type == 15)
    {
        start_sysex();
        append_sysex(midievent->buffer, midievent->size);
        parse_sysex();
    }
#endif
}

/*
 * process MIDI controller events
 */
void
RKR::process_midi_controller_events(int parameter, int value, int preset)
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

    // Special cases
    switch (parameter)
    {
        // Bank Select is Hard coded to CC 0
        case MC_Bank_Select:    // CC 0
        {
            if(value < (int) Bank_Vector.size())
            {
                if(active_bank != value)
                {
                    copy_bank(Bank, Bank_Vector[value].Bank);
                    active_bank = Change_Bank = value;      // Change_Bank is for GUI update
                }
            }
            
            // From custom program change table.
            // We must set the preset after the bank change.
            if(preset != C_CHANGE_PRESET_OFF)
            {
                Change_Preset = preset;
            }
            return;
        }

        case MC_Unused_10:
        case MC_Unused_11:
        case MC_Unused_13:
        case MC_Unused_15:
        case MC_Unused_16:
        case MC_Unused_17:
        case MC_Unused_18:
        case MC_Unused_19:

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

        case MC_Unused_64:

        case MC_Unused_128:
        case MC_Unused_129:
            return;

        case MC_Program_Table:
        {
            if(set_midi_table (value))
                Change_MIDI_Table = value;  // GUI update

            return;
        }

        case MC_Output_Volume:
            Active_Preset.Master_Volume = (float) value / 127.0f;
            calculavol(2);
            return;

        case MC_Balance_FX:
            Active_Preset.Fraction_Bypass = (float) value / 127.0f;
            return;

        case MC_Input_Volume:
            Active_Preset.Input_Gain = (float) value / 127.0f;
            calculavol(1);
            return;

        case MC_Multi_On_Off:
            ActiveUn(value);
            return;
    }

    // Normal MIDI processing of rack effects
    for (int param_index = 0; param_index < C_MC_PARAMETER_SIZE; param_index++)
    {
        if(mc_efx_params[param_index].MC_params_index == parameter)
        {
            // Get the MIDI control item to change
            int effect_index = mc_efx_params[param_index].Effect_index;
            int efx_param_index = mc_efx_params[param_index].Efx_param_index;
            int mc_offset = mc_efx_params[param_index].MC_offset;
            double range = mc_efx_params[param_index].MC_range;
            
            // Apply the change to the effect
            Rack_Effects[effect_index]->changepar(efx_param_index, mc_offset + ( (int) ((float) value * range)));
            return;
        }
    }
}

#ifdef SYSEX_SUPPORT
void
RKR::start_sysex( void  )
{
    m_sysex.clear();
}

bool 
RKR::append_sysex( unsigned char *a_data, long a_size )
{
    bool ret = true;

    for ( int i=0; i<a_size; i++ )
    {

        m_sysex.push_back( a_data[i] );
        if ( a_data[i] == EVENT_SYSEX_END )
            ret = false;
    }

    return ret;
}

/**
 * http://www.indiana.edu/~emusic/etext/MIDI/chapter3_MIDI9.shtml
 * A System Exclusive code set begins with 11110000 (240 decimal or F0 hex),
 * followed by the manufacturer ID#, then by an unspecified number of
 * data bytes of any ranges from 0-127) and ends with 11110111
 * (decimal 247 or F7 hex), meaning End of SysEx message. No other coded
 * are to be transmitted during a SysEx message (except a system real time
 * message). Normally, after the manufacturer ID, each maker will have its
 * own instrument model subcode, so a Yamaha DX7 will ignore a Yamaha SY77's
 * patch dump. In addition, most instruments have a SysEx ID # setting so
 * more than one of the same instruments can be on a network but not
 * necessarily respond to a patch dump not intended for it.
 * 
 * Dec 125 = hex 7D is the Universal Non-Commercial identification byte,
 * designed for use by Universities, researchers, etc.
 *
 * 
 * * * * * *  Format of Rakarrack-plus sysex save message * * * * * *
    EVENT_SYSEX                                             // byte 0 0xF0
    const unsigned char c_non_commercial_ID = 0x7D;         // byte 1
    const unsigned long c_RKRP_subcode      = 0x524B5250;   // bytes 2 - 5
    const unsigned char c_sysex_type        = 0x01;         // byte 6 - (For sysex type - 0x01)
    Bank save location (Dec 3 - 127)                        // byte 7
    Preset location (Dec 1 - 60)                            // byte 8
    Preset name (ascii hex max 22 characters)               // bytes 9 to ??? max 31 variable size
    end sysex 0xF7                                          // byte ??  (End message)
 */
void RKR::parse_sysex()
{
    unsigned char *data = m_sysex.data();

    // Check the manufacturer ID
    const unsigned char c_non_commercial_ID = 0x7D;         // byte 1
    if(data[1] != c_non_commercial_ID)
        return;

    // Check the subcode
    const unsigned long c_RKRP_subcode      = 0x524B5250;   // bytes 2 - 5
    unsigned long subcode = 0;

    subcode += (data[2] << 24);
    subcode += (data[3] << 16);
    subcode += (data[4] << 8);
    subcode += (data[5]);

    if(subcode != c_RKRP_subcode)
        return;

    // We are good to go 
    std::string preset_name = "";
    unsigned bank_number = data[7];
    unsigned preset_number = data[8];

    for(unsigned i = 9; i < (m_sysex.size() - 1); i++)
    {
        preset_name += (char) m_sysex[i];
    }

    //printf("Preset name = %s: bank = %d: p_num = %d\n", preset_name.c_str(), bank_number, preset_number);

    m_preset_name = preset_name;
    m_bank_number = bank_number;
    m_preset_number = preset_number;
    m_have_sysex_message = data[6];
}

void RKR::sysex_save_preset()
{
    if((m_bank_number >= Bank_Vector.size()) || (m_bank_number < 3))
    {
        fprintf(stderr, "Invalid Bank save request!\n");
        return;
    }

    if((m_preset_number < 1) || (m_preset_number > 60))
    {
        fprintf(stderr, "Invalid preset number save request!\n");
        return;
    }
    
    if(m_preset_name.size() > 22 || m_preset_name.empty())
    {
        fprintf(stderr, "Invalid preset name size!\n");
        return;
    }

    PresetBankStruct Save_Bank[62];
    
    // Copy the requested bank to be saved
    copy_bank(Save_Bank, Bank_Vector[m_bank_number].Bank);
    
    // Update active preset for any user changes
    refresh_active_preset();
    
    // Set the preset name for the active preset
    memset(Active_Preset.Preset_Name, 0, sizeof (char) * 64);
    strcpy(Active_Preset.Preset_Name, m_preset_name.c_str());
    
    // Copy the active preset to the save bank
    Save_Bank[m_preset_number] = Active_Preset;
    
    // Get the filename for the requested bank
    std::string filename = Bank_Vector[m_bank_number].Bank_File_Name;
    
    // Save the bank
    save_bank(filename, Save_Bank);
    
    // Update the Bank Vector
    load_bank_vector();
}
#endif  // SYSEX_SUPPORT
