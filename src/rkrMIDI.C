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

template <typename T>
std::string NumberToString ( T Number )
{
    std::stringstream ss;
    ss << Number;
    return ss.str();
}

/**
 *  The MIDI control parameters. This includes the default MIDI control
 *  parameters (1 - 127) as well as MIDI learn.
 *
 *  The los_params[] array is the order used by the MIDI learn window.
 * 
 *  The items are copied to Effects_Params struct:
 *      char Nom[32] = Parameter Name
 *      int Ato      = Parameter MIDI control number
 *      int Effect   = Effect number
 * 
 * If any additional parameters are added, then the constant
 * C_MC_PARAMETER_SIZE must be adjusted.
 */
void
RKR::MIDI_control()
{
    static const char *los_params[] =
    {
        "Balance FX%", ITS(MC_Balance_FX).c_str(), ITS(EFX_MASTER).c_str(),
        "Input Volume", ITS(MC_Input_Volume).c_str(), ITS(EFX_MASTER).c_str(),
        "Output Volume", ITS(MC_Output_Volume).c_str(), ITS(EFX_MASTER).c_str(),
        "Multi On/Off", ITS(MC_Multi_On_Off).c_str(), ITS(EFX_MASTER).c_str(),

        "Alienwah Dry/Wet", ITS(MC_Alien_DryWet).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Pan", ITS(MC_Alien_Pan).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Tempo", ITS(MC_Alien_LFO_Tempo).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Random", ITS(MC_Alien_LFO_Random).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah LFO Type", ITS(MC_Alien_LFO_Type).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Phase", ITS(MC_Alien_Phase).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Stereo Df.", ITS(MC_Alien_LFO_Stereo).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Depth", ITS(MC_Alien_Depth).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Delay", ITS(MC_Alien_Delay).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah Feedback", ITS(MC_Alien_Feedback).c_str(), ITS(EFX_ALIENWAH).c_str(),
        "Alienwah L/R Cross", ITS(MC_Alien_LR_Cross).c_str(), ITS(EFX_ALIENWAH).c_str(),

        "Analog Phaser Dry/Wet", ITS(MC_APhase_DryWet).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser LFO Type", ITS(MC_APhase_LFO_Type).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Tempo", ITS(MC_APhase_LFO_Tempo).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser P. Depth", ITS(MC_APhase_Depth).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Width", ITS(MC_APhase_Width).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Feedback", ITS(MC_APhase_Feedback).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Mismatch", ITS(MC_APhase_Mismatch).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Distortion", ITS(MC_APhase_Distortion).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Random", ITS(MC_APhase_LFO_Random).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),
        "Analog Phaser Stereo Df.", ITS(MC_APhase_LFO_Stereo).c_str(), ITS(EFX_ANALOG_PHASER).c_str(),

        "Arpie Dry/Wet", ITS(MC_Arpie_DryWet).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie Arpe's", ITS(MC_Arpie_ArpeWD).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie Pan", ITS(MC_Arpie_Pan).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie Tempo", ITS(MC_Arpie_Tempo).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie L/R Delay", ITS(MC_Arpie_LR_Delay).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie L/R Cross", ITS(MC_Arpie_LR_Cross).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie Feedback", ITS(MC_Arpie_Feedback).c_str(), ITS(EFX_ARPIE).c_str(),
        "Arpie Damp", ITS(MC_Arpie_Damp).c_str(), ITS(EFX_ARPIE).c_str(),

        "Cabinet Gain",ITS(MC_Cabinet_Gain).c_str(), ITS(EFX_CABINET).c_str(),

        "Chorus Dry/Wet", ITS(MC_Chorus_DryWet).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Pan", ITS(MC_Chorus_Pan).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Tempo", ITS(MC_Chorus_LFO_Tempo).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Random", ITS(MC_Chorus_LFO_Random).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus LFO Type", ITS(MC_Chorus_LFO_Type).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Stereo Df", ITS(MC_Chorus_LFO_Stereo).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Depth", ITS(MC_Chorus_Depth).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Delay", ITS(MC_Chorus_Delay).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus Feedback", ITS(MC_Chorus_Feedback).c_str(), ITS(EFX_CHORUS).c_str(),
        "Chorus L/R Cross", ITS(MC_Chorus_LR_Cross).c_str(), ITS(EFX_CHORUS).c_str(),

        "CoilCrafter Gain", ITS(MC_Coil_Gain).c_str(), ITS(EFX_COILCRAFTER).c_str(),
        "CoilCrafter Tone", ITS(MC_Coil_Tone).c_str(), ITS(EFX_COILCRAFTER).c_str(),
        "CoilCrafter Freq 1", ITS(MC_Coil_Freq_1).c_str(), ITS(EFX_COILCRAFTER).c_str(),
        "CoilCrafter Q 1", ITS(MC_Coil_Q_1).c_str(), ITS(EFX_COILCRAFTER).c_str(),
        "CoilCrafter Freq 2", ITS(MC_Coil_Freq_2).c_str(), ITS(EFX_COILCRAFTER).c_str(),
        "CoilCrafter Q 2", ITS(MC_Coil_Q_2).c_str(), ITS(EFX_COILCRAFTER).c_str(),

        "CompBand Dry/Wet", ITS(MC_CompBand_DryWet).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand Gain", ITS(MC_CompBand_Gain).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand L Ratio", ITS(MC_CompBand_Low_Ratio).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand ML Ratio", ITS(MC_CompBand_Mid_1_Ratio).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand MH Ratio", ITS(MC_CompBand_Mid_2_Ratio).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand H Ratio", ITS(MC_CompBand_High_Ratio).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand L Thres", ITS(MC_CompBand_Low_Thresh).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand ML Thres", ITS(MC_CompBand_Mid_1_Thresh).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand MH Thres", ITS(MC_CompBand_Mid_2_Thresh).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand H Thres", ITS(MC_CompBand_High_Thresh).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand Cross 1", ITS(MC_CompBand_Cross_1).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand Cross 2", ITS(MC_CompBand_Cross_2).c_str(), ITS(EFX_COMPBAND).c_str(),
        "CompBand Cross 3", ITS(MC_CompBand_Cross_3).c_str(), ITS(EFX_COMPBAND).c_str(),

        "Compressor A.Time", ITS(MC_Compress_Attack).c_str(), ITS(EFX_COMPRESSOR).c_str(),
        "Compressor Knee", ITS(MC_Compress_Knee).c_str(), ITS(EFX_COMPRESSOR).c_str(),
        "Compressor Output", ITS(MC_Compress_Output).c_str(), ITS(EFX_COMPRESSOR).c_str(),
        "Compressor Ratio", ITS(MC_Compress_Ratio).c_str(), ITS(EFX_COMPRESSOR).c_str(),
        "Compressor R.Time", ITS(MC_Compress_Release).c_str(), ITS(EFX_COMPRESSOR).c_str(),
        "Compressor Threshold", ITS(MC_Compress_Threshold).c_str(), ITS(EFX_COMPRESSOR).c_str(),

        "Convolotron Dry/Wet", ITS(MC_Convo_DryWet).c_str(), ITS(EFX_CONVOLOTRON).c_str(),
        "Convolotron Pan", ITS(MC_Convo_Pan).c_str(), ITS(EFX_CONVOLOTRON).c_str(),
        "Convolotron Level", ITS(MC_Convo_Level).c_str(), ITS(EFX_CONVOLOTRON).c_str(),
        "Convolotron Damp", ITS(MC_Convo_Damp).c_str(), ITS(EFX_CONVOLOTRON).c_str(),
        "Convolotron Feedback", ITS(MC_Convo_Feedback).c_str(), ITS(EFX_CONVOLOTRON).c_str(),
        "Convolotron Length", ITS(MC_Convo_Length).c_str(), ITS(EFX_CONVOLOTRON).c_str(),

        "Derelict Dry/Wet", ITS(MC_Dere_DryWet).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict L/R Cross", ITS(MC_Dere_LR_Cross).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict Drive", ITS(MC_Dere_Drive).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict Level", ITS(MC_Dere_Level).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict Type", ITS(MC_Dere_Type).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict Color", ITS(MC_Dere_Color).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict Sub Octave", ITS(MC_Dere_Suboctave).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict Pan", ITS(MC_Dere_Pan).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict LPF", ITS(MC_Dere_LPF).c_str(), ITS(EFX_DERELICT).c_str(),
        "Derelict HPF", ITS(MC_Dere_HPF).c_str(), ITS(EFX_DERELICT).c_str(),

        "DistBand Dry/Wet", ITS(MC_DistBand_DryWet).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand L/R Cross", ITS(MC_DistBand_LR_Cross).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Drive", ITS(MC_DistBand_Drive).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Level", ITS(MC_DistBand_Level).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand L. Gain", ITS(MC_DistBand_Gain_Low).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand M. Gain", ITS(MC_DistBand_Gain_Mid).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand H. Gain", ITS(MC_DistBand_Gain_Hi).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Cross 1", ITS(MC_DistBand_Cross_1).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Cross 2", ITS(MC_DistBand_Cross_2).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Type Low", ITS(MC_DistBand_Type_Low).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Type Mid", ITS(MC_DistBand_Type_Mid).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Type High", ITS(MC_DistBand_Type_Hi).c_str(), ITS(EFX_DISTBAND).c_str(),
        "DistBand Pan", ITS(MC_DistBand_Pan).c_str(), ITS(EFX_DISTBAND).c_str(),

        "Distortion Dry/Wet", ITS(MC_Dist_DryWet).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion L/R Cross", ITS(MC_Dist_LR_Cross).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion Drive", ITS(MC_Dist_Drive).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion Level", ITS(MC_Dist_Level).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion Type", ITS(MC_Dist_Type).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion Pan", ITS(MC_Dist_Pan).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion Sub Octave", ITS(MC_Dist_Suboctave).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion LPF", ITS(MC_Dist_LPF).c_str(), ITS(EFX_DISTORTION).c_str(),
        "Distortion HPF", ITS(MC_Dist_HPF).c_str(), ITS(EFX_DISTORTION).c_str(),

        "Dual Flange Dry/Wet", ITS(MC_DFlange_DryWet).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Pan", ITS(MC_DFlange_Pan).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange L/R Cross", ITS(MC_DFlange_LR_Cross).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Depth", ITS(MC_DFlange_Depth).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Width", ITS(MC_DFlange_Width).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Offset", ITS(MC_DFlange_Offset).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Feedback", ITS(MC_DFlange_Feedback).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange LPF", ITS(MC_DFlange_LPF).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Tempo", ITS(MC_DFlange_LFO_Tempo).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Stereo Df", ITS(MC_DFlange_LFO_Stereo).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange LFO Type", ITS(MC_DFlange_LFO_Type).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),
        "Dual Flange Random", ITS(MC_DFlange_LFO_Random).c_str(), ITS(EFX_DUAL_FLANGE).c_str(),

        "Echo Dry/Wet", ITS(MC_Echo_DryWet).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo Reverse", ITS(MC_Echo_Reverse).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo Pan", ITS(MC_Echo_Pan).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo Delay", ITS(MC_Echo_Delay).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo L/R Delay", ITS(MC_Echo_LR_Delay).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo L/R Cross", ITS(MC_Echo_LR_Cross).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo Feedback", ITS(MC_Echo_Feedback).c_str(), ITS(EFX_ECHO).c_str(),
        "Echo Damp", ITS(MC_Echo_Damp).c_str(), ITS(EFX_ECHO).c_str(),

        "Echotron Dry/Wet", ITS(MC_Echotron_DryWet).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Pan", ITS(MC_Echotron_Pan).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Tempo", ITS(MC_Echotron_Tempo).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Damp", ITS(MC_Echotron_Damp).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Feedback", ITS(MC_Echotron_Feedback).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron L/R Cross", ITS(MC_Echotron_LR_Cross).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Width", ITS(MC_Echotron_LFO_Width).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Depth", ITS(MC_Echotron_Depth).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron Stereo Df", ITS(MC_Echotron_LFO_Stereo).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron LFO Type", ITS(MC_Echotron_LFO_Type).c_str(), ITS(EFX_ECHOTRON).c_str(),
        "Echotron #", ITS(MC_Echotron_Taps).c_str(), ITS(EFX_ECHOTRON).c_str(),

        "Echoverse Dry/Wet", ITS(MC_Echoverse_DryWet).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Reverse", ITS(MC_Echoverse_Reverse).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Pan", ITS(MC_Echoverse_Pan).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Tempo", ITS(MC_Echoverse_Tempo).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse L/R Delay", ITS(MC_Echoverse_LR_Delay).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Feedback", ITS(MC_Echoverse_Feedback).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Damp", ITS(MC_Echoverse_Damp).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Ex Stereo", ITS(MC_Echoverse_Ext_Stereo).c_str(), ITS(EFX_ECHOVERSE).c_str(),
        "Echoverse Angle", ITS(MC_Echoverse_Angle).c_str(), ITS(EFX_ECHOVERSE).c_str(),

        "EQ Gain", ITS(MC_EQ_Gain).c_str(), ITS(EFX_EQ).c_str(),
        "EQ Q", ITS(MC_EQ_Q).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 31 Hz", ITS(MC_EQ_31_HZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 63 Hz", ITS(MC_EQ_63_HZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 125 Hz", ITS(MC_EQ_125_HZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 250 Hz", ITS(MC_EQ_250_HZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 500 Hz", ITS(MC_EQ_500_HZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 1 Khz", ITS(MC_EQ_1_KHZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 2 Khz", ITS(MC_EQ_2_KHZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 4 Khz", ITS(MC_EQ_4_KHZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 8 Khz", ITS(MC_EQ_8_KHZ).c_str(), ITS(EFX_EQ).c_str(),
        "EQ 16 Khz", ITS(MC_EQ_16_KHZ).c_str(), ITS(EFX_EQ).c_str(),

        "Exciter Gain", ITS(MC_Exciter_Gain).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter LPF", ITS(MC_Exciter_LPF).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter HPF", ITS(MC_Exciter_HPF).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 1", ITS(MC_Exciter_Harm_1).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 2", ITS(MC_Exciter_Harm_2).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 3", ITS(MC_Exciter_Harm_3).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 4", ITS(MC_Exciter_Harm_4).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 5", ITS(MC_Exciter_Harm_5).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 6", ITS(MC_Exciter_Harm_6).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 7", ITS(MC_Exciter_Harm_7).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 8", ITS(MC_Exciter_Harm_8).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 9", ITS(MC_Exciter_Harm_9).c_str(), ITS(EFX_EXCITER).c_str(),
        "Exciter Har 10", ITS(MC_Exciter_Harm_10).c_str(), ITS(EFX_EXCITER).c_str(),

        "Expander A.Time", ITS(MC_Expander_Attack).c_str(), ITS(EFX_EXPANDER).c_str(),
        "Expander R.Time", ITS(MC_Expander_Release).c_str(), ITS(EFX_EXPANDER).c_str(),
        "Expander Shape", ITS(MC_Expander_Shape).c_str(), ITS(EFX_EXPANDER).c_str(),
        "Expander Threshold", ITS(MC_Expander_Threshold).c_str(), ITS(EFX_EXPANDER).c_str(),
        "Expander Out Gain", ITS(MC_Expander_Gain).c_str(), ITS(EFX_EXPANDER).c_str(),
        "Expander LPF", ITS(MC_Expander_LPF).c_str(), ITS(EFX_EXPANDER).c_str(),
        "Expander HPF", ITS(MC_Expander_HPF).c_str(), ITS(EFX_EXPANDER).c_str(),

        "Flanger Dry/Wet", ITS(MC_Flanger_DryWet).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Pan", ITS(MC_Flanger_Pan).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Tempo", ITS(MC_Flanger_LFO_Tempo).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Random", ITS(MC_Flanger_LFO_Random).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger LFO Type", ITS(MC_Flanger_LFO_Type).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Stereo Df", ITS(MC_Flanger_LFO_Stereo).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Depth", ITS(MC_Flanger_Depth).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Delay", ITS(MC_Flanger_Delay).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger Feedback", ITS(MC_Flanger_Feedback).c_str(), ITS(EFX_FLANGER).c_str(),
        "Flanger L/R Cross", ITS(MC_Flanger_LR_Cross).c_str(), ITS(EFX_FLANGER).c_str(),

        "Harmonizer Dry/Wet", ITS(MC_Harm_DryWet).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Interval", ITS(MC_Harm_Interval).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Gain", ITS(MC_Harm_Gain).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Pan", ITS(MC_Harm_Pan).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Freq", ITS(MC_Harm_Filter_Freq).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Filter Gain", ITS(MC_Harm_Filter_Gain).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Filter Q", ITS(MC_Harm_Filter_Q).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer SELECT", ITS(MC_Harm_Select).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Note", ITS(MC_Harm_Note).c_str(), ITS(EFX_HARMONIZER).c_str(),
        "Harmonizer Chord", ITS(MC_Harm_Chord).c_str(), ITS(EFX_HARMONIZER).c_str(),

        "Infinity Dry/Wet", ITS(MC_Infinity_DryWet).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Res", ITS(MC_Infinity_Resonance).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 1", ITS(MC_Infinity_Band_1).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 2", ITS(MC_Infinity_Band_2).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 3", ITS(MC_Infinity_Band_3).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 4", ITS(MC_Infinity_Band_4).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 5", ITS(MC_Infinity_Band_5).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 6", ITS(MC_Infinity_Band_6).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 7", ITS(MC_Infinity_Band_7).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Filter Band 8", ITS(MC_Infinity_Band_8).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity AutoPan", ITS(MC_Infinity_AutoPan).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Stereo Df", ITS(MC_Infinity_LR_Delay).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Start", ITS(MC_Infinity_Start).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity End", ITS(MC_Infinity_End).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Tempo", ITS(MC_Infinity_Tempo).c_str(), ITS(EFX_INFINITY).c_str(),
        "Infinity Subdiv", ITS(MC_Infinity_Subdivision).c_str(), ITS(EFX_INFINITY).c_str(),

        "Looper Dry/Wet", ITS(MC_Looper_DryWet).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Level 1", ITS(MC_Looper_Level_1).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Level 2", ITS(MC_Looper_Level_2).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Tempo", ITS(MC_Looper_Tempo).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Reverse", ITS(MC_Looper_Reverse).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Auto Play", ITS(MC_Looper_AutoPlay).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Play", ITS(MC_Looper_Play).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Pause", ITS(MC_Looper_Stop).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Record", ITS(MC_Looper_Record).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper R1", ITS(MC_Looper_Rec_1).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper R2", ITS(MC_Looper_Rec_2).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Track 1", ITS(MC_Looper_Track_1).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Track 2", ITS(MC_Looper_Track_2).c_str(), ITS(EFX_LOOPER).c_str(),
        "Looper Clear", ITS(MC_Looper_Clear).c_str(), ITS(EFX_LOOPER).c_str(),

        "Musical Delay Dry/Wet", ITS(MC_Music_DryWet).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay L/R Cross", ITS(MC_Music_LR_Cross).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay Pan 1", ITS(MC_Music_Pan_1).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay Pan 2", ITS(MC_Music_Pan_2).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay Tempo", ITS(MC_Music_Tempo).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay Gain 1", ITS(MC_Music_Gain_1).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay Gain 2", ITS(MC_Music_Gain_2).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay FB 1", ITS(MC_Music_Feedback_1).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay FB 2", ITS(MC_Music_Feedback_2).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),
        "Musical Delay Damp", ITS(MC_Music_Damp).c_str(), ITS(EFX_MUSICAL_DELAY).c_str(),

        "MuTroMojo Dry/Wet", ITS(MC_MuTro_DryWet).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo LP", ITS(MC_MuTro_LowPass).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo BP", ITS(MC_MuTro_BandPass).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo HP", ITS(MC_MuTro_HighPass).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo LFO Type", ITS(MC_MuTro_LFO_Type).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Depth", ITS(MC_MuTro_Depth).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Tempo", ITS(MC_MuTro_LFO_Tempo).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Res", ITS(MC_MuTro_Resonance).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Range", ITS(MC_MuTro_Range).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Wah", ITS(MC_MuTro_Wah).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo E. Sens", ITS(MC_MuTro_Env_Sens).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Smooth", ITS(MC_MuTro_Env_Smooth).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Random", ITS(MC_MuTro_LFO_Random).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo Stereo Df", ITS(MC_MuTro_LFO_Stereo).c_str(), ITS(EFX_MUTROMOJO).c_str(),
        "MuTroMojo St. Freq", ITS(MC_MuTro_St_Freq).c_str(), ITS(EFX_MUTROMOJO).c_str(),

        "NoiseGate A. Time", ITS(MC_Gate_Attack).c_str(), ITS(EFX_NOISEGATE).c_str(),
        "NoiseGate R. Time", ITS(MC_Gate_Release).c_str(), ITS(EFX_NOISEGATE).c_str(),
        "NoiseGate Range", ITS(MC_Gate_Range).c_str(), ITS(EFX_NOISEGATE).c_str(),
        "NoiseGate Threshold", ITS(MC_Gate_Threshold).c_str(), ITS(EFX_NOISEGATE).c_str(),
        "NoiseGate Hold", ITS(MC_Gate_Hold).c_str(), ITS(EFX_NOISEGATE).c_str(),
        "NoiseGate LPF", ITS(MC_Gate_LPF).c_str(), ITS(EFX_NOISEGATE).c_str(),
        "NoiseGate HPF", ITS(MC_Gate_HPF).c_str(), ITS(EFX_NOISEGATE).c_str(),

        "Opticaltrem Depth", ITS(MC_Optical_Depth).c_str(), ITS(EFX_OPTICALTREM).c_str(),
        "Opticaltrem Tempo", ITS(MC_Optical_LFO_Tempo).c_str(), ITS(EFX_OPTICALTREM).c_str(),
        "Opticaltrem Random", ITS(MC_Optical_LFO_Random).c_str(), ITS(EFX_OPTICALTREM).c_str(),
        "Opticaltrem LFO Type", ITS(MC_Optical_LFO_Type).c_str(), ITS(EFX_OPTICALTREM).c_str(),
        "Opticaltrem Stereo Df", ITS(MC_Optical_LFO_Stereo).c_str(), ITS(EFX_OPTICALTREM).c_str(),
        "Opticaltrem Pan", ITS(MC_Optical_Pan).c_str(), ITS(EFX_OPTICALTREM).c_str(),

        "Overdrive Dry/Wet", ITS(MC_Overdrive_DryWet).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive L/R Cross", ITS(MC_Overdrive_LR_Cross).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive Drive", ITS(MC_Overdrive_Drive).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive Level", ITS(MC_Overdrive_Level).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive Type", ITS(MC_Overdrive_Type).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive Pan", ITS(MC_Overdrive_Pan).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive LPF", ITS(MC_Overdrive_LPF).c_str(), ITS(EFX_OVERDRIVE).c_str(),
        "Overdrive HPF", ITS(MC_Overdrive_HPF).c_str(), ITS(EFX_OVERDRIVE).c_str(),

        "Pan Dry/Wet", ITS(MC_Pan_DryWet).c_str(), ITS(EFX_PAN).c_str(),
        "Pan Pan", ITS(MC_Pan_Pan).c_str(), ITS(EFX_PAN).c_str(),
        "Pan Tempo", ITS(MC_Pan_LFO_Tempo).c_str(), ITS(EFX_PAN).c_str(),
        "Pan Random", ITS(MC_Pan_LFO_Random).c_str(), ITS(EFX_PAN).c_str(),
        "Pan LFO Type", ITS(MC_Pan_LFO_Type).c_str(), ITS(EFX_PAN).c_str(),
        "Pan Stereo Df", ITS(MC_Pan_LFO_Stereo).c_str(), ITS(EFX_PAN).c_str(),
        "Pan E. Stereo", ITS(MC_Pan_Ex_St_Amt).c_str(), ITS(EFX_PAN).c_str(),

        "Parametric EQ Gain", ITS(MC_Parametric_Gain).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ Low Freq", ITS(MC_Parametric_Low_Freq).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ Low Gain", ITS(MC_Parametric_Low_Gain).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ Low Q", ITS(MC_Parametric_Low_Q).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ Mid Freq", ITS(MC_Parametric_Mid_Freq).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ Mid Gain", ITS(MC_Parametric_Mid_Gain).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ Mid Q", ITS(MC_Parametric_Mid_Q).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ High Freq", ITS(MC_Parametric_High_Freq).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ High Gain", ITS(MC_Parametric_High_Gain).c_str(), ITS(EFX_PARAMETRIC).c_str(),
        "Parametric EQ High Q", ITS(MC_Parametric_High_Q).c_str(), ITS(EFX_PARAMETRIC).c_str(),

        "Phaser Dry/Wet", ITS(MC_Phaser_DryWet).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Pan", ITS(MC_Phaser_Pan).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Tempo", ITS(MC_Phaser_LFO_Tempo).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Random", ITS(MC_Phaser_LFO_Random).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser LFO Type", ITS(MC_Phaser_LFO_Type).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Phase", ITS(MC_Phaser_Phase).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Stereo Df", ITS(MC_Phaser_LFO_Stereo).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Depth", ITS(MC_Phaser_Depth).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser Feedback", ITS(MC_Phaser_Feedback).c_str(), ITS(EFX_PHASER).c_str(),
        "Phaser L/R Cross", ITS(MC_Phaser_LR_Cross).c_str(), ITS(EFX_PHASER).c_str(),

        "Reverb Dry/Wet", ITS(MC_Reverb_DryWet).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb Pan", ITS(MC_Reverb_Pan).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb Time", ITS(MC_Reverb_Time).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb Initial Delay", ITS(MC_Reverb_I_Delay).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb Del. E/R", ITS(MC_Reverb_Delay_FB).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb Room Size", ITS(MC_Reverb_Room).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb LPF", ITS(MC_Reverb_LPF).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb HPF", ITS(MC_Reverb_HPF).c_str(), ITS(EFX_REVERB).c_str(),
        "Reverb Damping", ITS(MC_Reverb_Damp).c_str(), ITS(EFX_REVERB).c_str(),

        "Reverbtron Dry/Wet", ITS(MC_Revtron_DryWet).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Pan", ITS(MC_Revtron_Pan).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Level", ITS(MC_Revtron_Level).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Damp", ITS(MC_Revtron_Damp).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Feedback", ITS(MC_Revtron_Feedback).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Length", ITS(MC_Revtron_Length).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Stretch", ITS(MC_Revtron_Stretch).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Initial Delay", ITS(MC_Revtron_I_Delay).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Fade", ITS(MC_Revtron_Fade).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron Diffusion", ITS(MC_Revtron_Diffusion).c_str(), ITS(EFX_REVERBTRON).c_str(),
        "Reverbtron LPF", ITS(MC_Revtron_LPF).c_str(), ITS(EFX_REVERBTRON).c_str(),

        "Ring Dry/Wet", ITS(MC_Ring_DryWet).c_str(), ITS(EFX_RING).c_str(),
        "Ring L/R Cross", ITS(MC_Ring_LR_Cross).c_str(), ITS(EFX_RING).c_str(),
        "Ring Input", ITS(MC_Ring_Input).c_str(), ITS(EFX_RING).c_str(),
        "Ring Level", ITS(MC_Ring_Level).c_str(), ITS(EFX_RING).c_str(),
        "Ring Pan", ITS(MC_Ring_Pan).c_str(), ITS(EFX_RING).c_str(),
        "Ring Depth", ITS(MC_Ring_Depth).c_str(), ITS(EFX_RING).c_str(),
        "Ring Freq", ITS(MC_Ring_Freq).c_str(), ITS(EFX_RING).c_str(),
        "Ring Sine", ITS(MC_Ring_Sine).c_str(), ITS(EFX_RING).c_str(),
        "Ring Triangle", ITS(MC_Ring_Triangle).c_str(), ITS(EFX_RING).c_str(),
        "Ring Sawtooth", ITS(MC_Ring_Saw).c_str(), ITS(EFX_RING).c_str(),
        "Ring Square", ITS(MC_Ring_Square).c_str(), ITS(EFX_RING).c_str(),

        "Sequence Dry/Wet", ITS(MC_Sequence_DryWet).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 1", ITS(MC_Sequence_Step_1).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 2", ITS(MC_Sequence_Step_2).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 3", ITS(MC_Sequence_Step_3).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 4", ITS(MC_Sequence_Step_4).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 5", ITS(MC_Sequence_Step_5).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 6", ITS(MC_Sequence_Step_6).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 7", ITS(MC_Sequence_Step_7).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence 8", ITS(MC_Sequence_Step_8).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence Tempo", ITS(MC_Sequence_Tempo).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence Q", ITS(MC_Sequence_Resonance).c_str(), ITS(EFX_SEQUENCE).c_str(),
        "Sequence Stereo Df", ITS(MC_Sequence_Stdf).c_str(), ITS(EFX_SEQUENCE).c_str(),

        "ShelfBoost Gain", ITS(MC_Shelf_Gain).c_str(), ITS(EFX_SHELFBOOST).c_str(),
        "ShelfBoost Level", ITS(MC_Shelf_Level).c_str(), ITS(EFX_SHELFBOOST).c_str(),
        "ShelfBoost Tone", ITS(MC_Shelf_Tone).c_str(), ITS(EFX_SHELFBOOST).c_str(),
        "ShelfBoost Presence", ITS(MC_Shelf_Presence).c_str(), ITS(EFX_SHELFBOOST).c_str(),

        "Shifter Dry/Wet", ITS(MC_Shifter_DryWet).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Interval", ITS(MC_Shifter_Interval).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Gain", ITS(MC_Shifter_Gain).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Pan", ITS(MC_Shifter_Pan).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Attack", ITS(MC_Shifter_Attack).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Decay", ITS(MC_Shifter_Decay).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Threshold", ITS(MC_Shifter_Threshold).c_str(), ITS(EFX_SHIFTER).c_str(),
        "Shifter Whammy", ITS(MC_Shifter_Whammy).c_str(), ITS(EFX_SHIFTER).c_str(),

        "Shuffle Dry/Wet", ITS(MC_Shuffle_DryWet).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle Low Freq", ITS(MC_Shuffle_Freq_L).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle Low Gain", ITS(MC_Shuffle_Gain_L).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle M.L. Freq", ITS(MC_Shuffle_Freq_ML).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle M.L. Gain", ITS(MC_Shuffle_Gain_ML).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle M.H. Freq", ITS(MC_Shuffle_Freq_MH).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle M.H. Gain", ITS(MC_Shuffle_Gain_MH).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle High Freq", ITS(MC_Shuffle_Freq_H).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle High Gain", ITS(MC_Shuffle_Gain_H).c_str(), ITS(EFX_SHUFFLE).c_str(),
        "Shuffle Q", ITS(MC_Shuffle_Width).c_str(), ITS(EFX_SHUFFLE).c_str(),

        "StereoHarm Dry/Wet", ITS(MC_Sharm_DryWet).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Int L", ITS(MC_Sharm_L_Interval).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Chrm L", ITS(MC_Sharm_L_Chroma).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Gain L", ITS(MC_Sharm_L_Gain).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Int R", ITS(MC_Sharm_R_Interval).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Chrm R", ITS(MC_Sharm_R_Chroma).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Gain R", ITS(MC_Sharm_R_Gain).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm L/R Cross", ITS(MC_Sharm_LR_Cross).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm SELECT", ITS(MC_Sharm_Select).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Note", ITS(MC_Sharm_Note).c_str(), ITS(EFX_STEREOHARM).c_str(),
        "StereoHarm Chord", ITS(MC_Sharm_Chord).c_str(), ITS(EFX_STEREOHARM).c_str(),

        "StompBox Level", ITS(MC_Stomp_Level).c_str(), ITS(EFX_STOMPBOX).c_str(),
        "StompBox Gain", ITS(MC_Stomp_Gain).c_str(), ITS(EFX_STOMPBOX).c_str(),
        "StompBox Bias", ITS(MC_Stomp_Bias).c_str(), ITS(EFX_STOMPBOX).c_str(),
        "StompBox Mid", ITS(MC_Stomp_Mid).c_str(), ITS(EFX_STOMPBOX).c_str(),
        "StompBox Tone", ITS(MC_Stomp_Tone).c_str(), ITS(EFX_STOMPBOX).c_str(),
        "StompBox Mode", ITS(MC_Stomp_Mode).c_str(), ITS(EFX_STOMPBOX).c_str(),

        "Sustainer Gain", ITS(MC_Sustain_Gain).c_str(), ITS(EFX_SUSTAINER).c_str(),
        "Sustainer Sustain", ITS(MC_Sustain_Sustain).c_str(), ITS(EFX_SUSTAINER).c_str(),

        "Synthfilter Dry/Wet", ITS(MC_Synthfilter_DryWet).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Distort", ITS(MC_Synthfilter_Distort).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Tempo", ITS(MC_Synthfilter_LFO_Tempo).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Random", ITS(MC_Synthfilter_LFO_Random).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter LFO Type", ITS(MC_Synthfilter_LFO_Type).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Stereo Df", ITS(MC_Synthfilter_LFO_Stereo).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Width", ITS(MC_Synthfilter_Width).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Feedback", ITS(MC_Synthfilter_Feedback).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Depth", ITS(MC_Synthfilter_Depth).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter E.Sens", ITS(MC_Synthfilter_Env_Sens).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter A.Time", ITS(MC_Synthfilter_Attack).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter R.Time", ITS(MC_Synthfilter_Release).c_str(), ITS(EFX_SYNTHFILTER).c_str(),
        "Synthfilter Offset", ITS(MC_Synthfilter_Offset).c_str(), ITS(EFX_SYNTHFILTER).c_str(),

        "Valve Dry/Wet", ITS(MC_Valve_DryWet).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve L/R Cross", ITS(MC_Valve_LR_Cross).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve Pan", ITS(MC_Valve_Pan).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve Level", ITS(MC_Valve_Level).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve Drive", ITS(MC_Valve_Drive).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve Dist", ITS(MC_Valve_Distortion).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve Presence", ITS(MC_Valve_Presence).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve LPF", ITS(MC_Valve_LPF).c_str(), ITS(EFX_VALVE).c_str(),
        "Valve HPF", ITS(MC_Valve_HPF).c_str(), ITS(EFX_VALVE).c_str(),

        "VaryBand Dry/Wet", ITS(MC_VaryBand_DryWet).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand Tempo 1", ITS(MC_VaryBand_LFO_Tempo_1).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand LFO 1 Type", ITS(MC_VaryBand_LFO_Type_1).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand St.df 1", ITS(MC_VaryBand_LFO_Stereo_1).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand Tempo 2", ITS(MC_VaryBand_LFO_Tempo_2).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand LFO 2 Type", ITS(MC_VaryBand_LFO_Type_2).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand St.df 2", ITS(MC_VaryBand_LFO_Stereo_2).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand Cross 1", ITS(MC_VaryBand_Cross_1).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand Cross 2", ITS(MC_VaryBand_Cross_2).c_str(), ITS(EFX_VARYBAND).c_str(),
        "VaryBand Cross 3", ITS(MC_VaryBand_Cross_3).c_str(), ITS(EFX_VARYBAND).c_str(),

        "Vibe Dry/Wet", ITS(MC_Vibe_DryWet).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Width", ITS(MC_Vibe_Width).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Depth", ITS(MC_Vibe_Depth).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Tempo", ITS(MC_Vibe_LFO_Tempo).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Random", ITS(MC_Vibe_LFO_Random).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe LFO Type", ITS(MC_Vibe_LFO_Type).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Stereo Df", ITS(MC_Vibe_LFO_Stereo).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Feedback", ITS(MC_Vibe_Feedback).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe L/R Cross", ITS(MC_Vibe_LR_Cross).c_str(), ITS(EFX_VIBE).c_str(),
        "Vibe Pan", ITS(MC_Vibe_Pan).c_str(), ITS(EFX_VIBE).c_str(),

        "Vocoder Dry/Wet", ITS(MC_Vocoder_DryWet).c_str(), ITS(EFX_VOCODER).c_str(),
        "Vocoder Pan", ITS(MC_Vocoder_Pan).c_str(), ITS(EFX_VOCODER).c_str(),
        "Vocoder Input", ITS(MC_Vocoder_Input).c_str(), ITS(EFX_VOCODER).c_str(),
        "Vocoder Smear", ITS(MC_Vocoder_Smear).c_str(), ITS(EFX_VOCODER).c_str(),
        "Vocoder Q", ITS(MC_Vocoder_Q).c_str(), ITS(EFX_VOCODER).c_str(),
        "Vocoder Ring", ITS(MC_Vocoder_Ring).c_str(), ITS(EFX_VOCODER).c_str(),
        "Vocoder Level", ITS(MC_Vocoder_Level).c_str(), ITS(EFX_VOCODER).c_str(),

        "WahWah Dry/Wet", ITS(MC_WahWah_DryWet).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Pan", ITS(MC_WahWah_Pan).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Tempo", ITS(MC_WahWah_LFO_Tempo).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Random", ITS(MC_WahWah_LFO_Random).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah LFO Type", ITS(MC_WahWah_LFO_Type).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Stereo Df", ITS(MC_WahWah_LFO_Stereo).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Depth", ITS(MC_WahWah_Depth).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Amp S.", ITS(MC_WahWah_Sense).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Amp S.I.", ITS(MC_WahWah_ASI).c_str(), ITS(EFX_WAHWAH).c_str(),
        "WahWah Smooth", ITS(MC_WahWah_Smooth).c_str(), ITS(EFX_WAHWAH).c_str(),
    };

    // If any additional parameters are added, then the constant
    // C_MC_PARAMETER_SIZE must be adjusted.
    for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
    {
        strcpy(efx_params[i].Nom, los_params[i * 3]);
        sscanf(los_params[i * 3 + 1], "%d", &efx_params[i].Ato);
        sscanf(los_params[i * 3 + 2], "%d", &efx_params[i].Effect);
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
            stecla = 5;
        }

        if (midievent->type == SND_SEQ_EVENT_STOP)
        {
            Rack_Effects[EFX_LOOPER]->changepar(Looper_Stop, 1);
            stecla = 5;
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
                    preset = midievent->data.control.value;

                if (midievent->data.control.value == 81)
                    if (Selected_Preset > 1) preset = Selected_Preset - 1;

                if (midievent->data.control.value == 82)
                    if (Selected_Preset < 60) preset = Selected_Preset + 1;
            }
            else
                preset = midievent->data.control.value;
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

    switch (miraque)
    {
    case EFX_EQ:
        if (inoff) EFX_Bypass[EFX_EQ] = 1;
        else EFX_Bypass[EFX_EQ] = 0;
        break;
    case EFX_COMPRESSOR:
        if (inoff) EFX_Bypass[EFX_COMPRESSOR] = 1;
        else EFX_Bypass[EFX_COMPRESSOR] = 0;
        break;
    case EFX_DISTORTION:
        if (inoff) EFX_Bypass[EFX_DISTORTION] = 1;
        else EFX_Bypass[EFX_DISTORTION] = 0;
        break;
    case EFX_OVERDRIVE:
        if (inoff) EFX_Bypass[EFX_OVERDRIVE] = 1;
        else EFX_Bypass[EFX_OVERDRIVE] = 0;
        break;
    case EFX_ECHO:
        if (inoff) EFX_Bypass[EFX_ECHO] = 1;
        else EFX_Bypass[EFX_ECHO] = 0;
        break;
    case EFX_CHORUS:
        if (inoff) EFX_Bypass[EFX_CHORUS] = 1;
        else EFX_Bypass[EFX_CHORUS] = 0;
        break;
    case EFX_PHASER:
        if (inoff) EFX_Bypass[EFX_PHASER] = 1;
        else EFX_Bypass[EFX_PHASER] = 0;
        break;
    case EFX_FLANGER:
        if (inoff) EFX_Bypass[EFX_FLANGER] = 1;
        else EFX_Bypass[EFX_FLANGER] = 0;
        break;
    case EFX_REVERB:
        if (inoff) EFX_Bypass[EFX_REVERB] = 1;
        else EFX_Bypass[EFX_REVERB] = 0;
        break;
    case EFX_PARAMETRIC:
        if (inoff) EFX_Bypass[EFX_PARAMETRIC] = 1;
        else EFX_Bypass[EFX_PARAMETRIC] = 0;
        break;
    case EFX_WAHWAH:
        if (inoff) EFX_Bypass[EFX_WAHWAH] = 1;
        else EFX_Bypass[EFX_WAHWAH] = 0;
        break;
    case EFX_ALIENWAH:
        if (inoff) EFX_Bypass[EFX_ALIENWAH] = 1;
        else EFX_Bypass[EFX_ALIENWAH] = 0;
        break;
    case EFX_CABINET:
        if (inoff) EFX_Bypass[EFX_CABINET] = 1;
        else EFX_Bypass[EFX_CABINET] = 0;
        break;
    case EFX_PAN:
        if (inoff) EFX_Bypass[EFX_PAN] = 1;
        else EFX_Bypass[EFX_PAN] = 0;
        break;
    case EFX_HARMONIZER:
        if (inoff) EFX_Bypass[EFX_HARMONIZER] = 1;
        else EFX_Bypass[EFX_HARMONIZER] = 0;
        break;
    case EFX_MUSICAL_DELAY:
        if (inoff) EFX_Bypass[EFX_MUSICAL_DELAY] = 1;
        else EFX_Bypass[EFX_MUSICAL_DELAY] = 0;
        break;
    case EFX_NOISEGATE:
        if (inoff) EFX_Bypass[EFX_NOISEGATE] = 1;
        else EFX_Bypass[EFX_NOISEGATE] = 0;
        break;
    case EFX_DERELICT:
        if (inoff) EFX_Bypass[EFX_DERELICT] = 1;
        else EFX_Bypass[EFX_DERELICT] = 0;
        break;
    case EFX_ANALOG_PHASER:
        if (inoff) EFX_Bypass[EFX_ANALOG_PHASER] = 1;
        else EFX_Bypass[EFX_ANALOG_PHASER] = 0;
        break;
    case EFX_VALVE:
        if (inoff) EFX_Bypass[EFX_VALVE] = 1;
        else EFX_Bypass[EFX_VALVE] = 0;
        break;
    case EFX_DUAL_FLANGE:
        if (inoff) EFX_Bypass[EFX_DUAL_FLANGE] = 1;
        else EFX_Bypass[EFX_DUAL_FLANGE] = 0;
        break;
    case EFX_RING:
        if (inoff) EFX_Bypass[EFX_RING] = 1;
        else EFX_Bypass[EFX_RING] = 0;
        break;
    case EFX_EXCITER:
        if (inoff) EFX_Bypass[EFX_EXCITER] = 1;
        else EFX_Bypass[EFX_EXCITER] = 0;
        break;
    case EFX_DISTBAND:
        if (inoff) EFX_Bypass[EFX_DISTBAND] = 1;
        else EFX_Bypass[EFX_DISTBAND] = 0;
        break;
    case EFX_ARPIE:
        if (inoff) EFX_Bypass[EFX_ARPIE] = 1;
        else EFX_Bypass[EFX_ARPIE] = 0;
        break;
    case EFX_EXPANDER:
        if (inoff) EFX_Bypass[EFX_EXPANDER] = 1;
        else EFX_Bypass[EFX_EXPANDER] = 0;
        break;
    case EFX_SHUFFLE:
        if (inoff) EFX_Bypass[EFX_SHUFFLE] = 1;
        else EFX_Bypass[EFX_SHUFFLE] = 0;
        break;
    case EFX_SYNTHFILTER:
        if (inoff) EFX_Bypass[EFX_SYNTHFILTER] = 1;
        else EFX_Bypass[EFX_SYNTHFILTER] = 0;
        break;
    case EFX_VARYBAND:
        if (inoff) EFX_Bypass[EFX_VARYBAND] = 1;
        else EFX_Bypass[EFX_VARYBAND] = 0;
        break;
    case EFX_CONVOLOTRON:
        if (inoff) EFX_Bypass[EFX_CONVOLOTRON] = 1;
        else EFX_Bypass[EFX_CONVOLOTRON] = 0;
        break;
    case EFX_LOOPER:
        if (inoff) EFX_Bypass[EFX_LOOPER] = 1;
        else EFX_Bypass[EFX_LOOPER] = 0;
        break;
    case EFX_MUTROMOJO:
        if (inoff) EFX_Bypass[EFX_MUTROMOJO] = 1;
        else EFX_Bypass[EFX_MUTROMOJO] = 0;
        break;
    case EFX_ECHOVERSE:
        if (inoff) EFX_Bypass[EFX_ECHOVERSE] = 1;
        else EFX_Bypass[EFX_ECHOVERSE] = 0;
        break;
    case EFX_COILCRAFTER:
        if (inoff) EFX_Bypass[EFX_COILCRAFTER] = 1;
        else EFX_Bypass[EFX_COILCRAFTER] = 0;
        break;
    case EFX_SHELFBOOST:
        if (inoff) EFX_Bypass[EFX_SHELFBOOST] = 1;
        else EFX_Bypass[EFX_SHELFBOOST] = 0;
        break;
    case EFX_VOCODER:
        if (inoff) EFX_Bypass[EFX_VOCODER] = 1;
        else EFX_Bypass[EFX_VOCODER] = 0;
        break;
    case EFX_SUSTAINER:
        if (inoff) EFX_Bypass[EFX_SUSTAINER] = 1;
        else EFX_Bypass[EFX_SUSTAINER] = 0;
        break;
    case EFX_SEQUENCE:
        if (inoff) EFX_Bypass[EFX_SEQUENCE] = 1;
        else EFX_Bypass[EFX_SEQUENCE] = 0;
        break;
    case EFX_SHIFTER:
        if (inoff) EFX_Bypass[EFX_SHIFTER] = 1;
        else EFX_Bypass[EFX_SHIFTER] = 0;
        break;
    case EFX_STOMPBOX:
        if (inoff) EFX_Bypass[EFX_STOMPBOX] = 1;
        else EFX_Bypass[EFX_STOMPBOX] = 0;
        break;
    case EFX_REVERBTRON:
        if (inoff) EFX_Bypass[EFX_REVERBTRON] = 1;
        else EFX_Bypass[EFX_REVERBTRON] = 0;
        break;
    case EFX_ECHOTRON:
        if (inoff) EFX_Bypass[EFX_ECHOTRON] = 1;
        else EFX_Bypass[EFX_ECHOTRON] = 0;
        break;
    case EFX_STEREOHARM:
        if (inoff) EFX_Bypass[EFX_STEREOHARM] = 1;
        else EFX_Bypass[EFX_STEREOHARM] = 0;
        break;
    case EFX_COMPBAND:
        if (inoff) EFX_Bypass[EFX_COMPBAND] = 1;
        else EFX_Bypass[EFX_COMPBAND] = 0;
        break;
    case EFX_OPTICALTREM:
        if (inoff) EFX_Bypass[EFX_OPTICALTREM] = 1;
        else EFX_Bypass[EFX_OPTICALTREM] = 0;
        break;
    case EFX_VIBE:
        if (inoff) EFX_Bypass[EFX_VIBE] = 1;
        else EFX_Bypass[EFX_VIBE] = 0;
        break;
    case EFX_INFINITY:
        if (inoff) EFX_Bypass[EFX_INFINITY] = 1;
        else EFX_Bypass[EFX_INFINITY] = 0;
        break;


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

int
RKR::checkonoff(int miraque)
{
    switch (miraque)
    {
    case EFX_EQ:
        if (EFX_Bypass[EFX_EQ]) return (0);
        break;
    case EFX_COMPRESSOR:
        if (EFX_Bypass[EFX_COMPRESSOR]) return (0);
        break;
    case EFX_DISTORTION:
        if (EFX_Bypass[EFX_DISTORTION]) return (0);
        break;
    case EFX_OVERDRIVE:
        if (EFX_Bypass[EFX_OVERDRIVE])return (0);
        break;
    case EFX_ECHO:
        if (EFX_Bypass[EFX_ECHO])return (0);
        break;
    case EFX_CHORUS:
        if (EFX_Bypass[EFX_CHORUS]) return (0);
        break;
    case EFX_PHASER:
        if (EFX_Bypass[EFX_PHASER]) return (0);
        break;
    case EFX_FLANGER:
        if (EFX_Bypass[EFX_FLANGER]) return (0);
        break;
    case EFX_REVERB:
        if (EFX_Bypass[EFX_REVERB]) return (0);
        break;
    case EFX_PARAMETRIC:
        if (EFX_Bypass[EFX_PARAMETRIC]) return (0);
        break;
    case EFX_WAHWAH:
        if (EFX_Bypass[EFX_WAHWAH]) return (0);
        break;
    case EFX_ALIENWAH:
        if (EFX_Bypass[EFX_ALIENWAH]) return (0);
        break;
    case EFX_CABINET:
        if (EFX_Bypass[EFX_CABINET]) return (0);
        break;
    case EFX_PAN:
        if (EFX_Bypass[EFX_PAN]) return (0);
        break;
    case EFX_HARMONIZER:
        if (EFX_Bypass[EFX_HARMONIZER]) return (0);
        break;
    case EFX_MUSICAL_DELAY:
        if (EFX_Bypass[EFX_MUSICAL_DELAY]) return (0);
        break;
    case EFX_NOISEGATE:
        if (EFX_Bypass[EFX_NOISEGATE]) return (0);
        break;
    case EFX_DERELICT:
        if (EFX_Bypass[EFX_DERELICT]) return (0);
        break;
    case EFX_ANALOG_PHASER:
        if (EFX_Bypass[EFX_ANALOG_PHASER]) return (0);
        break;
    case EFX_VALVE:
        if (EFX_Bypass[EFX_VALVE]) return (0);
        break;
    case EFX_DUAL_FLANGE:
        if (EFX_Bypass[EFX_DUAL_FLANGE]) return (0);
        break;
    case EFX_RING:
        if (EFX_Bypass[EFX_RING]) return (0);
        break;
    case EFX_EXCITER:
        if (EFX_Bypass[EFX_EXCITER]) return (0);
        break;
    case EFX_DISTBAND:
        if (EFX_Bypass[EFX_DISTBAND]) return (0);
        break;
    case EFX_ARPIE:
        if (EFX_Bypass[EFX_ARPIE]) return (0);
        break;
    case EFX_EXPANDER:
        if (EFX_Bypass[EFX_EXPANDER]) return (0);
        break;
    case EFX_SHUFFLE:
        if (EFX_Bypass[EFX_SHUFFLE]) return (0);
        break;
    case EFX_SYNTHFILTER:
        if (EFX_Bypass[EFX_SYNTHFILTER]) return (0);
        break;
    case EFX_VARYBAND:
        if (EFX_Bypass[EFX_VARYBAND]) return (0);
        break;
    case EFX_CONVOLOTRON:
        if (EFX_Bypass[EFX_CONVOLOTRON]) return (0);
        break;
    case EFX_LOOPER:
        if (EFX_Bypass[EFX_LOOPER]) return (0);
        break;
    case EFX_MUTROMOJO:
        if (EFX_Bypass[EFX_MUTROMOJO]) return (0);
        break;
    case EFX_ECHOVERSE:
        if (EFX_Bypass[EFX_ECHOVERSE]) return (0);
        break;
    case EFX_COILCRAFTER:
        if (EFX_Bypass[EFX_COILCRAFTER]) return (0);
        break;
    case EFX_SHELFBOOST:
        if (EFX_Bypass[EFX_SHELFBOOST]) return (0);
        break;
    case EFX_VOCODER:
        if (EFX_Bypass[EFX_VOCODER]) return (0);
        break;
    case EFX_SUSTAINER:
        if (EFX_Bypass[EFX_SUSTAINER]) return (0);
        break;
    case EFX_SEQUENCE:
        if (EFX_Bypass[EFX_SEQUENCE]) return (0);
        break;
    case EFX_SHIFTER:
        if (EFX_Bypass[EFX_SHIFTER]) return (0);
        break;
    case EFX_STOMPBOX:
        if (EFX_Bypass[EFX_STOMPBOX]) return (0);
        break;
    case EFX_REVERBTRON:
        if (EFX_Bypass[EFX_REVERBTRON]) return (0);
        break;
    case EFX_ECHOTRON:
        if (EFX_Bypass[EFX_ECHOTRON]) return (0);
        break;
    case EFX_STEREOHARM:
        if (EFX_Bypass[EFX_STEREOHARM]) return (0);
        break;
    case EFX_COMPBAND:
        if (EFX_Bypass[EFX_COMPBAND]) return (0);
        break;
    case EFX_OPTICALTREM:
        if (EFX_Bypass[EFX_OPTICALTREM]) return (0);
        break;
    case EFX_VIBE:
        if (EFX_Bypass[EFX_VIBE]) return (0);
        break;
    case EFX_INFINITY:
        if (EFX_Bypass[EFX_INFINITY]) return (0);
        break;

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
                    preset = cmdvalue;

                if (cmdvalue == 81) if (Selected_Preset > 1) preset = Selected_Preset - 1;
                
                if (cmdvalue == 82) if (Selected_Preset < 60) preset = Selected_Preset + 1;
            }
            else
                preset = cmdvalue;
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
        efx_Derelict->changepar(Dere_Drive, value);
        break;

    case MC_Dere_Level:
        efx_Derelict->changepar(Dere_Level, value);
        break;

    case MC_Dere_LPF:
        efx_Derelict->changepar(Dere_LPF, ret_LPF(value));
        break;

    case MC_Dere_HPF:
        efx_Derelict->changepar(Dere_HPF, ret_HPF(value));
        break;

    case MC_Dere_Color:
        efx_Derelict->changepar(Dere_Color, value);
        break;

    case MC_Output_Volume:
        Master_Volume = (float) value / 127.0f;
        calculavol(2);
        break;

    case MC_Dere_Suboctave:
        efx_Derelict->changepar(Dere_Suboctave, value);
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
        efx_MusDelay->changepar(Music_Gain_1, value);
        break;

    case MC_Music_Gain_2:
        efx_MusDelay->changepar(Music_Gain_2, value);
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
        efx_MusDelay->changepar(Music_DryWet, Dry_Wet(value));
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
        efx_MusDelay->changepar(Music_Pan_1, value);
        break;

    case MC_Reverb_Pan:
        Rack_Effects[EFX_REVERB]->changepar(Reverb_Pan, value);
        break;

    case MC_Unused_64:
        break;

    case MC_Music_Pan_2:
        efx_MusDelay->changepar(Music_Pan_2, value);
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
        efx_MusDelay->changepar(Music_Feedback_1, value);
        break;

    case MC_Music_Feedback_2:
        efx_MusDelay->changepar(Music_Feedback_2, value);
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
        efx_MusDelay->changepar(Music_LR_Cross, value);
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
        efx_Derelict->changepar(Dere_DryWet, Dry_Wet(value));
        break;

    case MC_Dere_Pan:
        efx_Derelict->changepar(Dere_Pan, value);
        break;

    case MC_Dere_LR_Cross:
        efx_Derelict->changepar(Dere_LR_Cross, value);
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
        EQ *Efx_Equalizer = static_cast<EQ*>(Rack_Effects[EFX_EQ]);
        Efx_Equalizer->changepar_Q(EQ_Q, value);
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
        efx_Compressor->changepar(Compress_Attack, 10 + (int) ((float) value * C_MC_240_RANGE));
        break;

    case MC_Compress_Release:
        efx_Compressor->changepar(Compress_Release, 10 + (int) ((float) value * C_MC_490_RANGE));
        break;

    case MC_Compress_Ratio:
        efx_Compressor->changepar(Compress_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_Compress_Knee:
        efx_Compressor->changepar(Compress_Knee, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Compress_Threshold:
        efx_Compressor->changepar(Compress_Threshold, -60 + (int) ((float) value * C_MC_57_RANGE));
        break;

    case MC_Compress_Output:
        efx_Compressor->changepar(Compress_Output, -40 + (int) ((float) value * C_MC_40_RANGE));
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
        efx_DFlange->changepar(DFlange_DryWet, Dry_Wet(value));
        break;

    case MC_DFlange_Pan:
        efx_DFlange->changepar(DFlange_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_DFlange_LR_Cross:
        efx_DFlange->changepar(DFlange_LR_Cross, value);
        break;

    case MC_DFlange_Depth:
        efx_DFlange->changepar(DFlange_Depth, 20 + (int) ((float) value * C_MC_2480_RANGE));
        break;

    case MC_DFlange_Width:
        efx_DFlange->changepar(DFlange_Width, (int) ((float) value * C_MC_6000_RANGE));
        break;

    case MC_DFlange_Offset:
        efx_DFlange->changepar(DFlange_Offset, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DFlange_Feedback:
        efx_DFlange->changepar(DFlange_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_DFlange_LPF:
        /* This is labeled LPF but uses same range as HPF - FIXME check DSP */
        efx_DFlange->changepar(DFlange_LPF, 20 + (int) ((float) value * C_MC_19980_RANGE));
        break;

    case MC_DFlange_LFO_Tempo:
        efx_DFlange->changepar(DFlange_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_DFlange_LFO_Stereo:
        efx_DFlange->changepar(DFlange_LFO_Stereo, value);
        break;

    case MC_DFlange_LFO_Random:
        efx_DFlange->changepar(DFlange_LFO_Random, value);
        break;

    case MC_Valve_DryWet:
        efx_Valve->changepar(Valve_DryWet, Dry_Wet(value));
        break;

    case MC_Valve_LR_Cross:
        efx_Valve->changepar(Valve_LR_Cross, value);
        break;

    case MC_Valve_Pan:
        efx_Valve->changepar(Valve_Pan, value);
        break;

    case MC_Valve_Level:
        efx_Valve->changepar(Valve_Level, value);
        break;

    case MC_Valve_Drive:
        efx_Valve->changepar(Valve_Drive, value);
        break;

    case MC_Valve_Distortion:
        efx_Valve->changepar(Valve_Distortion, value);
        break;

    case MC_Valve_Presence:
        efx_Valve->changepar(Valve_Presence, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Valve_LPF:
        efx_Valve->changepar(Valve_LPF, ret_LPF(value));
        break;

    case MC_Valve_HPF:
        efx_Valve->changepar(Valve_HPF, ret_HPF(value));
        break;

    case MC_Ring_DryWet:
        efx_Ring->changepar(Ring_DryWet, Dry_Wet(value));
        break;

    case MC_Ring_LR_Cross:
        efx_Ring->changepar(Ring_LR_Cross, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Ring_Input:
        efx_Ring->changepar(Ring_Input, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Ring_Level:
        efx_Ring->changepar(Ring_Level, value);
        break;

    case MC_Ring_Pan:
        efx_Ring->changepar(Ring_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Ring_Depth:
        efx_Ring->changepar(Ring_Depth, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Freq:
        efx_Ring->changepar(Ring_Freq, 1 + (int) ((float) value * C_MC_19999_RANGE));
        break;

    case MC_Ring_Sine:
        efx_Ring->changepar(Ring_Sine, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Triangle:
        efx_Ring->changepar(Ring_Triangle, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Saw:
        efx_Ring->changepar(Ring_Saw, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Ring_Square:
        efx_Ring->changepar(Ring_Square, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_Exciter_Gain:
        efx_Exciter->changepar(Exciter_Gain, value);
        break;

    case MC_Exciter_LPF:
        efx_Exciter->changepar(Exciter_LPF, ret_LPF(value));
        break;

    case MC_Exciter_HPF:
        efx_Exciter->changepar(Exciter_HPF, ret_HPF(value));
        break;

    case MC_Exciter_Harm_1:
        efx_Exciter->changepar(Exciter_Harm_1, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_2:
        efx_Exciter->changepar(Exciter_Harm_2, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_3:
        efx_Exciter->changepar(Exciter_Harm_3, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_4:
        efx_Exciter->changepar(Exciter_Harm_4, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_5:
        efx_Exciter->changepar(Exciter_Harm_5, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_6:
        efx_Exciter->changepar(Exciter_Harm_6, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_7:
        efx_Exciter->changepar(Exciter_Harm_7, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_8:
        efx_Exciter->changepar(Exciter_Harm_8, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_9:
        efx_Exciter->changepar(Exciter_Harm_9, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Exciter_Harm_10:
        efx_Exciter->changepar(Exciter_Harm_10, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_DistBand_DryWet:
        efx_DistBand->changepar(DistBand_DryWet, Dry_Wet(value));
        break;

    case MC_DistBand_LR_Cross:
        efx_DistBand->changepar(DistBand_LR_Cross, value);
        break;

    case MC_DistBand_Drive:
        efx_DistBand->changepar(DistBand_Drive, value);
        break;

    case MC_DistBand_Level:
        efx_DistBand->changepar(DistBand_Level, value);
        break;

    case MC_DistBand_Gain_Low:
        efx_DistBand->changepar(DistBand_Gain_Low, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DistBand_Gain_Mid:
        efx_DistBand->changepar(DistBand_Gain_Mid, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DistBand_Gain_Hi:
        efx_DistBand->changepar(DistBand_Gain_Hi, (int) ((float) value * C_MC_100_RANGE));
        break;

    case MC_DistBand_Cross_1:
        efx_DistBand->changepar(DistBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_DistBand_Cross_2:
        efx_DistBand->changepar(DistBand_Cross_2, 800 + (int) ((float) value * C_MC_11200_RANGE));
        break;

    case MC_DistBand_Pan:
        efx_DistBand->changepar(DistBand_Pan, value);
        break;

    case MC_Arpie_DryWet:
        efx_Arpie->changepar(Arpie_DryWet, Dry_Wet(value));
        break;

    case MC_Arpie_ArpeWD:
        efx_Arpie->changepar(Arpie_ArpeWD, value);
        break;

    case MC_Arpie_Pan:
        efx_Arpie->changepar(Arpie_Pan, value);
        break;

    case MC_Arpie_Tempo:
        efx_Arpie->changepar(Arpie_Tempo, ret_Tempo(value));
        break;

    case MC_Arpie_LR_Delay:
        efx_Arpie->changepar(Arpie_LR_Delay, value);
        break;

    case MC_Arpie_LR_Cross:
        efx_Arpie->changepar(Arpie_LR_Cross, value);
        break;

    case MC_Arpie_Feedback:
        efx_Arpie->changepar(Arpie_Feedback, value);
        break;

    case MC_Arpie_Damp:
        efx_Arpie->changepar(Arpie_Damp, value);
        break;

    case MC_Expander_Attack:
        efx_Expander->changepar(Expander_Attack, 1 + (int) ((float) value * C_MC_4999_RANGE));
        break;

    case MC_Expander_Release:
        efx_Expander->changepar(Expander_Release, 10 + (int) ((float) value * C_MC_990_RANGE));
        break;

    case MC_Expander_Shape:
        efx_Expander->changepar(Expander_Shape, 1 + (int) ((float) value * C_MC_49_RANGE));
        break;

    case MC_Expander_Threshold:
        efx_Expander->changepar(Expander_Threshold, (int) ((float) value * -C_MC_80_RANGE));
        break;

    case MC_Expander_Gain:
        efx_Expander->changepar(Expander_Gain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Expander_LPF:
        efx_Expander->changepar(Expander_LPF, ret_LPF(value));
        break;

    case MC_Expander_HPF:
        efx_Expander->changepar(Expander_HPF, ret_HPF(value));
        break;

    case MC_Shuffle_DryWet:
        efx_Shuffle->changepar(Shuffle_DryWet, Dry_Wet(value));
        break;

    case MC_Shuffle_Freq_L:
        efx_Shuffle->changepar(Shuffle_Freq_L, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_Shuffle_Gain_L:
        efx_Shuffle->changepar(Shuffle_Gain_L, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Freq_ML:
        efx_Shuffle->changepar(Shuffle_Freq_ML, 400 + (int) ((float) value * C_MC_3600_RANGE));
        break;

    case MC_Shuffle_Gain_ML:
        efx_Shuffle->changepar(Shuffle_Gain_ML, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Freq_MH:
        efx_Shuffle->changepar(Shuffle_Freq_MH, 1200 + (int) ((float) value * C_MC_6800_RANGE));
        break;

    case MC_Shuffle_Gain_MH:
        efx_Shuffle->changepar(Shuffle_Gain_MH, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Freq_H:
        efx_Shuffle->changepar(Shuffle_Freq_H, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case MC_Shuffle_Gain_H:
        efx_Shuffle->changepar(Shuffle_Gain_H, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Shuffle_Width:
        efx_Shuffle->changepar(Shuffle_Width, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Synthfilter_DryWet:
        efx_Synthfilter->changepar(Synthfilter_DryWet, Dry_Wet(value));
        break;

    case MC_Synthfilter_Distort:
        efx_Synthfilter->changepar(Synthfilter_Distort, value);
        break;

    case MC_Synthfilter_LFO_Tempo:
        efx_Synthfilter->changepar(Synthfilter_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Synthfilter_LFO_Stereo:
        efx_Synthfilter->changepar(Synthfilter_LFO_Stereo, value);
        break;

    case MC_Synthfilter_Width:
        efx_Synthfilter->changepar(Synthfilter_Width, value);
        break;

    case MC_Synthfilter_Feedback:
        efx_Synthfilter->changepar(Synthfilter_Feedback, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Synthfilter_Depth:
        efx_Synthfilter->changepar(Synthfilter_Depth, value);
        break;

    case MC_Synthfilter_Env_Sens:
        efx_Synthfilter->changepar(Synthfilter_Env_Sens, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case MC_Synthfilter_Attack:
        efx_Synthfilter->changepar(Synthfilter_Attack, 5 + (int) ((float) value * C_MC_995_RANGE));
        break;

    case MC_Synthfilter_Release:
        efx_Synthfilter->changepar(Synthfilter_Release, 5 + (int) ((float) value * C_MC_495_RANGE));
        break;

    case MC_Synthfilter_Offset:
        efx_Synthfilter->changepar(Synthfilter_Offset, value);
        break;

    case MC_VaryBand_DryWet:
        efx_VaryBand->changepar(VaryBand_DryWet, Dry_Wet(value));
        break;

    case MC_VaryBand_LFO_Tempo_1:
        efx_VaryBand->changepar(VaryBand_LFO_Tempo_1, ret_Tempo(value));
        break;

    case MC_VaryBand_LFO_Stereo_1:
        efx_VaryBand->changepar(VaryBand_LFO_Stereo_1, value);
        break;

    case MC_VaryBand_LFO_Tempo_2:
        efx_VaryBand->changepar(VaryBand_LFO_Tempo_2, ret_Tempo(value));
        break;

    case MC_VaryBand_LFO_Stereo_2:
        efx_VaryBand->changepar(VaryBand_LFO_Stereo_2, value);
        break;

    case MC_VaryBand_Cross_1:
        efx_VaryBand->changepar(VaryBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_VaryBand_Cross_2:
        efx_VaryBand->changepar(VaryBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case MC_VaryBand_Cross_3:
        efx_VaryBand->changepar(VaryBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case MC_MuTro_DryWet:
        efx_MuTroMojo->changepar(MuTro_DryWet, Dry_Wet(value));
        break;

    case MC_MuTro_LowPass:
        efx_MuTroMojo->changepar(MuTro_LowPass, value - 64);
        break;

    case MC_MuTro_BandPass:
        efx_MuTroMojo->changepar(MuTro_BandPass, value - 64);
        break;

    case MC_MuTro_HighPass:
        efx_MuTroMojo->changepar(MuTro_HighPass, value - 64);
        break;

    case MC_MuTro_Depth:
        efx_MuTroMojo->changepar(MuTro_Depth, value);
        break;

    case MC_MuTro_LFO_Tempo:
        efx_MuTroMojo->changepar(MuTro_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_MuTro_Resonance:
        efx_MuTroMojo->changepar(MuTro_Resonance, value);
        break;

    case MC_MuTro_Range:
        efx_MuTroMojo->changepar(MuTro_Range, 10 + (int) ((float) value * C_MC_5990_RANGE));
        break;

    case MC_MuTro_Wah:
        efx_MuTroMojo->changepar(MuTro_Wah, value);
        break;

    case MC_MuTro_Env_Sens:
        efx_MuTroMojo->changepar(MuTro_Env_Sens, value - 64);
        break;

    case MC_MuTro_Env_Smooth:
        efx_MuTroMojo->changepar(MuTro_Env_Smooth, value);
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
        efx_CoilCrafter->changepar(Coil_Gain, value);
        break;

    case MC_Coil_Tone:
        efx_CoilCrafter->changepar(Coil_Tone, 20 + (int) ((float) value * C_MC_4380_RANGE));
        break;

    case MC_Coil_Freq_1:
        efx_CoilCrafter->changepar(Coil_Freq_1, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case MC_Coil_Q_1:
        efx_CoilCrafter->changepar(Coil_Q_1, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case MC_Coil_Freq_2:
        efx_CoilCrafter->changepar(Coil_Freq_2, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case MC_Coil_Q_2:
        efx_CoilCrafter->changepar(Coil_Q_2, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case MC_Shelf_Gain:
        efx_ShelfBoost->changepar(Shelf_Gain, value);
        break;

    case MC_Shelf_Level:
        efx_ShelfBoost->changepar(Shelf_Level, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Shelf_Tone:
        efx_ShelfBoost->changepar(Shelf_Tone, 220 + (int) ((float) value * C_MC_15780_RANGE));
        break;

    case MC_Shelf_Presence:
        efx_ShelfBoost->changepar(Shelf_Presence, ((int) (float) value * C_MC_128_RANGE)  - 64);
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
        efx_Echoverse->changepar(Echoverse_DryWet, Dry_Wet(value));
        break;

    case MC_Echoverse_Reverse:
        efx_Echoverse->changepar(Echoverse_Reverse, value);
        break;

    case MC_Echoverse_Pan:
        efx_Echoverse->changepar(Echoverse_Pan, value);
        break;

    case MC_Echoverse_Tempo:
        efx_Echoverse->changepar(Echoverse_Tempo, ret_Tempo(value));
        break;

    case MC_Echoverse_LR_Delay:
        efx_Echoverse->changepar(Echoverse_LR_Delay, value);
        break;

    case MC_Echoverse_Feedback:
        efx_Echoverse->changepar(Echoverse_Feedback, value);
        break;

    case MC_Echoverse_Damp:
        efx_Echoverse->changepar(Echoverse_Damp, value);
        break;

    case MC_Echoverse_Ext_Stereo:
        efx_Echoverse->changepar(Echoverse_Ext_Stereo, value);
        break;

    case MC_Echoverse_Angle:
        efx_Echoverse->changepar(Echoverse_Angle, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sustain_Gain:
        efx_Sustainer->changepar(Sustain_Gain, value);
        break;

    case MC_Sustain_Sustain:
        efx_Sustainer->changepar(Sustain_Sustain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case MC_Sequence_DryWet:
        efx_Sequence->changepar(Sequence_DryWet, Dry_Wet(value));
        break;

    case MC_Sequence_Step_1:
        efx_Sequence->changepar(Sequence_Step_1, value);
        break;

    case MC_Sequence_Step_2:
        efx_Sequence->changepar(Sequence_Step_2, value);
        break;

    case MC_Sequence_Step_3:
        efx_Sequence->changepar(Sequence_Step_3, value);
        break;

    case MC_Sequence_Step_4:
        efx_Sequence->changepar(Sequence_Step_4, value);
        break;

    case MC_Sequence_Step_5:
        efx_Sequence->changepar(Sequence_Step_5, value);
        break;

    case MC_Sequence_Step_6:
        efx_Sequence->changepar(Sequence_Step_6, value);
        break;

    case MC_Sequence_Step_7:
        efx_Sequence->changepar(Sequence_Step_7, value);
        break;

    case MC_Sequence_Step_8:
        efx_Sequence->changepar(Sequence_Step_8, value);
        break;

    case MC_Sequence_Tempo:
        efx_Sequence->changepar(Sequence_Tempo, ret_Tempo(value));
        break;

    case MC_Sequence_Resonance:
        efx_Sequence->changepar(Sequence_Resonance, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sequence_Stdf:
        efx_Sequence->changepar(Sequence_Stdf, (int) ((float) value * C_MC_7_RANGE));
        break;

    case MC_Shifter_DryWet:
        efx_Shifter->changepar(Shifter_DryWet, Dry_Wet(value));
        break;

    case MC_Shifter_Interval:
        efx_Shifter->changepar(Shifter_Interval, (int) ((float) value * C_MC_12_RANGE));
        break;

    case MC_Shifter_Gain:
        efx_Shifter->changepar(Shifter_Gain, value);
        break;

    case MC_Shifter_Pan:
        efx_Shifter->changepar(Shifter_Pan, value);
        break;

    case MC_Shifter_Attack:
        efx_Shifter->changepar(Shifter_Attack, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case MC_Shifter_Decay:
        efx_Shifter->changepar(Shifter_Decay, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case MC_Shifter_Threshold:
        efx_Shifter->changepar(Shifter_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case MC_Shifter_Whammy:
        efx_Shifter->changepar(Shifter_Whammy, value);
        break;

    case MC_Stomp_Level:
        efx_StompBox->changepar(Stomp_Level, value);
        break;

    case MC_Stomp_Gain:
        efx_StompBox->changepar(Stomp_Gain, value);
        break;

    case MC_Stomp_Bias:
        efx_StompBox->changepar(Stomp_Bias, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Stomp_Mid:
        efx_StompBox->changepar(Stomp_Mid, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Stomp_Tone:
        efx_StompBox->changepar(Stomp_Tone, (int) ((float) value * C_MC_128_RANGE) - 64);
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
        efx_StereoHarm->changepar(Sharm_DryWet, Dry_Wet(value));
        break;

    case MC_Sharm_L_Interval:
        efx_StereoHarm->changepar(Sharm_L_Interval, (int) ((float) value * C_MC_24_RANGE));
        break;

    case MC_Sharm_L_Chroma:
        efx_StereoHarm->changepar(Sharm_L_Chroma, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case MC_Sharm_L_Gain:
        efx_StereoHarm->changepar(Sharm_L_Gain, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sharm_R_Interval:
        efx_StereoHarm->changepar(Sharm_R_Interval, (int) ((float) value * C_MC_24_RANGE));
        break;

    case MC_Sharm_R_Chroma:
        efx_StereoHarm->changepar(Sharm_R_Chroma, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case MC_Sharm_R_Gain:
        efx_StereoHarm->changepar(Sharm_R_Gain, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Sharm_LR_Cross:
        efx_StereoHarm->changepar(Sharm_LR_Cross, value);
        break;

    case MC_Sharm_Note:
        efx_StereoHarm->changepar(Sharm_Note, (int) ((float) value * C_MC_23_RANGE));
        break;

    case MC_Sharm_Chord:
        efx_StereoHarm->changepar(Sharm_Chord, (int) ((float) value * C_MC_33_RANGE));
        break;

    case MC_CompBand_DryWet:
        efx_CompBand->changepar(CompBand_DryWet, Dry_Wet(value));
        break;

    case MC_CompBand_Gain:
        efx_CompBand->changepar(CompBand_Gain, value);
        break;

    case MC_CompBand_Low_Ratio:
        efx_CompBand->changepar(CompBand_Low_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_Mid_1_Ratio:
        efx_CompBand->changepar(CompBand_Mid_1_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_Mid_2_Ratio:
        efx_CompBand->changepar(CompBand_Mid_2_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_High_Ratio:
        efx_CompBand->changepar(CompBand_High_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case MC_CompBand_Low_Thresh:
        efx_CompBand->changepar(CompBand_Low_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_Mid_1_Thresh:
        efx_CompBand->changepar(CompBand_Mid_1_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_Mid_2_Thresh:
        efx_CompBand->changepar(CompBand_Mid_2_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_High_Thresh:
        efx_CompBand->changepar(CompBand_High_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case MC_CompBand_Cross_1:
        efx_CompBand->changepar(CompBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_CompBand_Cross_2:
        efx_CompBand->changepar(CompBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case MC_CompBand_Cross_3:
        efx_CompBand->changepar(CompBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case MC_Optical_Depth:
        efx_Opticaltrem->changepar(Optical_Depth, value);
        break;

    case MC_Optical_LFO_Tempo:
        efx_Opticaltrem->changepar(Optical_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Optical_LFO_Random:
        efx_Opticaltrem->changepar(Optical_LFO_Random, value);
        break;

    case MC_Optical_LFO_Stereo:
        efx_Opticaltrem->changepar(Optical_LFO_Stereo, value);
        break;

    case MC_Optical_Pan:
        efx_Opticaltrem->changepar(Optical_Pan, value);
        break;

    case MC_Vibe_DryWet:
        efx_Vibe->changepar(Vibe_DryWet, Dry_Wet(value));
        break;

    case MC_Vibe_Width:
        efx_Vibe->changepar(Vibe_Width, value);
        break;

    case MC_Vibe_Depth:
        efx_Vibe->changepar(Vibe_Depth, value);
        break;

    case MC_Vibe_LFO_Tempo:
        efx_Vibe->changepar(Vibe_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Vibe_LFO_Random:
        efx_Vibe->changepar(Vibe_LFO_Random, value);
        break;

    case MC_Vibe_LFO_Stereo:
        efx_Vibe->changepar(Vibe_LFO_Stereo, value);
        break;

    case MC_Vibe_Feedback:
        efx_Vibe->changepar(Vibe_Feedback, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Vibe_LR_Cross:
        efx_Vibe->changepar(Vibe_LR_Cross, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Vibe_Pan:
        efx_Vibe->changepar(Vibe_Pan, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_Infinity_DryWet:
        efx_Infinity->changepar(Infinity_DryWet, Dry_Wet(value));
        break;

    case MC_Infinity_Resonance:
        efx_Infinity->changepar(Infinity_Resonance, -1000 + (int) ((float) value * C_MC_2000_RANGE));
        break;

    case MC_Infinity_AutoPan:
        efx_Infinity->changepar(Infinity_AutoPan, value);
        break;

    case MC_Infinity_LR_Delay:
        efx_Infinity->changepar(Infinity_LR_Delay, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Start:
        efx_Infinity->changepar(Infinity_Start, value);
        break;

    case MC_Infinity_End:
        efx_Infinity->changepar(Infinity_End, value);
        break;

    case MC_Infinity_Tempo:
        efx_Infinity->changepar(Infinity_Tempo, ret_Tempo(value));
        break;

    case MC_Infinity_Subdivision:
        efx_Infinity->changepar(Infinity_Subdivision, -16 + (int) ((float) value * C_MC_32_RANGE));
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
        efx_Infinity->changepar(Infinity_Band_1, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_2:
        efx_Infinity->changepar(Infinity_Band_2, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_3:
        efx_Infinity->changepar(Infinity_Band_3, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_4:
        efx_Infinity->changepar(Infinity_Band_4, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_5:
        efx_Infinity->changepar(Infinity_Band_5, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_6:
        efx_Infinity->changepar(Infinity_Band_6, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_7:
        efx_Infinity->changepar(Infinity_Band_7, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Infinity_Band_8:
        efx_Infinity->changepar(Infinity_Band_8, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case MC_Looper_Tempo:
        Rack_Effects[EFX_LOOPER]->changepar(Looper_Tempo, 20 + (int) ((float) value * C_MC_360_RANGE));
        break;

    case MC_Music_Tempo:
        efx_MusDelay->changepar(Music_Tempo, 10 + (int) ((float) value * C_MC_470_RANGE));
        break;

    case MC_Music_Damp:
        efx_MusDelay->changepar(Music_Damp, value);
        break;

    case MC_MuTro_LFO_Random:
        efx_MuTroMojo->changepar(MuTro_LFO_Random, value);
        break;

    case MC_MuTro_LFO_Stereo:
        efx_MuTroMojo->changepar(MuTro_LFO_Stereo, value);
        break;

    case MC_MuTro_St_Freq:
        efx_MuTroMojo->changepar(MuTro_St_Freq, 30 + (int) ((float) value * C_MC_770_RANGE));
        break;

    case MC_Gate_Attack:
        efx_Gate->changepar(Gate_Attack, 1 + (int) ((float) value * C_MC_249_RANGE));
        break;

    case MC_Gate_Release:
        efx_Gate->changepar(Gate_Release, 2 + (int) ((float) value * C_MC_248_RANGE));
        break;

    case MC_Gate_Range:
        efx_Gate->changepar(Gate_Range, -90 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case MC_Gate_Threshold:
        efx_Gate->changepar(Gate_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case MC_Gate_Hold:
        efx_Gate->changepar(Gate_Hold, 2 + (int) ((float) value * C_MC_498_RANGE));
        break;

    case MC_Gate_LPF:
        efx_Gate->changepar(Gate_LPF, ret_LPF(value));
        break;

    case MC_Gate_HPF:
        efx_Gate->changepar(Gate_HPF, ret_HPF(value));
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
        efx_Synthfilter->changepar(Synthfilter_LFO_Random, value);
        break;

    case MC_Harm_Select:
        Rack_Effects[EFX_HARMONIZER]->changepar(Harm_Select, value);
        break;

    case MC_Sharm_Select:
        efx_StereoHarm->changepar(Sharm_Select, value);
        break;

    case MC_Dist_Type:
        Rack_Effects[EFX_DISTORTION]->changepar(Dist_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_Overdrive_Type:
        Rack_Effects[EFX_OVERDRIVE]->changepar(Overdrive_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_Dere_Type:
        efx_Derelict->changepar(Dere_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_DistBand_Type_Low:
        efx_DistBand->changepar(DistBand_Type_Low, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_DistBand_Type_Mid:
        efx_DistBand->changepar(DistBand_Type_Mid, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_DistBand_Type_Hi:
        efx_DistBand->changepar(DistBand_Type_Hi, (int) ((float) value * C_MC_30_RANGE));
        break;

    case MC_Stomp_Mode:
        efx_StompBox->changepar(Stomp_Mode, (int) ((float) value * C_MC_8_RANGE));
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
        efx_DFlange->changepar(DFlange_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Echotron_LFO_Type:
        Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_MuTro_LFO_Type:
        efx_MuTroMojo->changepar(MuTro_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Optical_LFO_Type:
        efx_Opticaltrem->changepar(Optical_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Pan_LFO_Type:
        Rack_Effects[EFX_PAN]->changepar(Pan_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Phaser_LFO_Type:
        Rack_Effects[EFX_PHASER]->changepar(Phaser_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Synthfilter_LFO_Type:
        efx_Synthfilter->changepar(Synthfilter_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_VaryBand_LFO_Type_1:
        efx_VaryBand->changepar(VaryBand_LFO_Type_1, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_VaryBand_LFO_Type_2:
        efx_VaryBand->changepar(VaryBand_LFO_Type_2, (int) ((float) value * C_MC_11_RANGE));
        break;

    case MC_Vibe_LFO_Type:
        efx_Vibe->changepar(Vibe_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
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

