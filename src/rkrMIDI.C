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
 * Cast Integer To String (ITS). 
 */
#define ITS( x )   dynamic_cast< std::ostringstream & >( \
                    ( std::ostringstream() << std::dec << x ) ).str()

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
        "Balance FX%", ITS(MC_Balance_FX).c_str(), "50",
        "Input Volume", ITS(MC_Input_Volume).c_str(), "50",
        "Output Volume", ITS(MC_Output_Volume).c_str(), "50",
        "Multi On/Off", ITS(MC_Multi_On_Off).c_str(), "50",

        "Alienwah Dry/Wet", ITS(MC_Alien_DryWet).c_str(), "11",
        "Alienwah Pan", ITS(MC_Alien_Pan).c_str(), "11",
        "Alienwah Tempo", ITS(MC_Alien_LFO_Tempo).c_str(), "11",
        "Alienwah Random", ITS(MC_Alien_LFO_Random).c_str(), "11",
        "Alienwah LFO Type", ITS(MC_Alien_LFO_Type).c_str(), "11",
        "Alienwah Phase", ITS(MC_Alien_Phase).c_str(), "11",
        "Alienwah Stereo Df.", ITS(MC_Alien_LFO_Stereo).c_str(), "11",
        "Alienwah Depth", ITS(MC_Alien_Depth).c_str(), "11",
        "Alienwah Delay", ITS(MC_Alien_Delay).c_str(), "11",
        "Alienwah Feedback", ITS(MC_Alien_Feedback).c_str(), "11",
        "Alienwah L/R Cross", ITS(MC_Alien_LR_Cross).c_str(), "11",

        "Analog Phaser Dry/Wet", ITS(MC_APhase_DryWet).c_str(), "18",
        "Analog Phaser LFO Type", ITS(MC_APhase_LFO_Type).c_str(), "18",
        "Analog Phaser Tempo", ITS(MC_APhase_LFO_Tempo).c_str(), "18",
        "Analog Phaser P. Depth", ITS(MC_APhase_Depth).c_str(), "18",
        "Analog Phaser Width", ITS(MC_APhase_Width).c_str(), "18",
        "Analog Phaser Feedback", ITS(MC_APhase_Feedback).c_str(), "18",
        "Analog Phaser Mismatch", ITS(MC_APhase_Mismatch).c_str(), "18",
        "Analog Phaser Distortion", ITS(MC_APhase_Distortion).c_str(), "18",
        "Analog Phaser Random", ITS(MC_APhase_LFO_Random).c_str(), "18",
        "Analog Phaser Stereo Df.", ITS(MC_APhase_LFO_Stereo).c_str(), "18",

        "Arpie Dry/Wet", ITS(MC_Arpie_DryWet).c_str(), "24",
        "Arpie Arpe's", ITS(MC_Arpie_ArpeWD).c_str(), "24",
        "Arpie Pan", ITS(MC_Arpie_Pan).c_str(), "24",
        "Arpie Tempo", ITS(MC_Arpie_Tempo).c_str(), "24",
        "Arpie L/R Delay", ITS(MC_Arpie_LR_Delay).c_str(), "24",
        "Arpie L/R Cross", ITS(MC_Arpie_LR_Cross).c_str(), "24",
        "Arpie Feedback", ITS(MC_Arpie_Feedback).c_str(), "24",
        "Arpie Damp", ITS(MC_Arpie_Damp).c_str(), "24",

        "Cabinet Gain",ITS(MC_Cabinet_Gain).c_str(),"12",

        "Chorus Dry/Wet", ITS(MC_Chorus_DryWet).c_str(), "5",
        "Chorus Pan", ITS(MC_Chorus_Pan).c_str(), "5",
        "Chorus Tempo", ITS(MC_Chorus_LFO_Tempo).c_str(), "5",
        "Chorus Random", ITS(MC_Chorus_LFO_Random).c_str(), "5",
        "Chorus LFO Type", ITS(MC_Chorus_LFO_Type).c_str(), "5",
        "Chorus Stereo Df", ITS(MC_Chorus_LFO_Stereo).c_str(), "5",
        "Chorus Depth", ITS(MC_Chorus_Depth).c_str(), "5",
        "Chorus Delay", ITS(MC_Chorus_Delay).c_str(), "5",
        "Chorus Feedback", ITS(MC_Chorus_Feedback).c_str(), "5",
        "Chorus L/R Cross", ITS(MC_Chorus_LR_Cross).c_str(), "5",

        "CoilCrafter Gain", ITS(MC_Coil_Gain).c_str(), "33",
        "CoilCrafter Tone", ITS(MC_Coil_Tone).c_str(), "33",
        "CoilCrafter Freq 1", ITS(MC_Coil_Freq_1).c_str(), "33",
        "CoilCrafter Q 1", ITS(MC_Coil_Q_1).c_str(), "33",
        "CoilCrafter Freq 2", ITS(MC_Coil_Freq_2).c_str(), "33",
        "CoilCrafter Q 2", ITS(MC_Coil_Q_2).c_str(), "33",

        "CompBand Dry/Wet", ITS(MC_CompBand_DryWet).c_str(), "43",
        "CompBand Gain", ITS(MC_CompBand_Gain).c_str(), "43",
        "CompBand L Ratio", ITS(MC_CompBand_Low_Ratio).c_str(), "43",
        "CompBand ML Ratio", ITS(MC_CompBand_Mid_1_Ratio).c_str(), "43",
        "CompBand MH Ratio", ITS(MC_CompBand_Mid_2_Ratio).c_str(), "43",
        "CompBand H Ratio", ITS(MC_CompBand_High_Ratio).c_str(), "43",
        "CompBand L Thres", ITS(MC_CompBand_Low_Thresh).c_str(), "43",
        "CompBand ML Thres", ITS(MC_CompBand_Mid_1_Thresh).c_str(), "43",
        "CompBand MH Thres", ITS(MC_CompBand_Mid_2_Thresh).c_str(), "43",
        "CompBand H Thres", ITS(MC_CompBand_High_Thresh).c_str(), "43",
        "CompBand Cross 1", ITS(MC_CompBand_Cross_1).c_str(), "43",
        "CompBand Cross 2", ITS(MC_CompBand_Cross_2).c_str(), "43",
        "CompBand Cross 3", ITS(MC_CompBand_Cross_3).c_str(), "43",

        "Compressor A.Time", ITS(MC_Compress_Attack).c_str(), "1",
        "Compressor Knee", ITS(MC_Compress_Knee).c_str(), "1",
        "Compressor Output", ITS(MC_Compress_Output).c_str(), "1",
        "Compressor Ratio", ITS(MC_Compress_Ratio).c_str(), "1",
        "Compressor R.Time", ITS(MC_Compress_Release).c_str(), "1",
        "Compressor Threshold", ITS(MC_Compress_Threshold).c_str(), "1",

        "Convolotron Dry/Wet", ITS(MC_Convo_DryWet).c_str(), "29",
        "Convolotron Pan", ITS(MC_Convo_Pan).c_str(), "29",
        "Convolotron Level", ITS(MC_Convo_Level).c_str(), "29",
        "Convolotron Damp", ITS(MC_Convo_Damp).c_str(), "29",
        "Convolotron Feedback", ITS(MC_Convo_Feedback).c_str(), "29",
        "Convolotron Length", ITS(MC_Convo_Length).c_str(), "29",

        "Derelict Dry/Wet", ITS(MC_Dere_DryWet).c_str(), "17",
        "Derelict L/R Cross", ITS(MC_Dere_LR_Cross).c_str(), "17",
        "Derelict Drive", ITS(MC_Dere_Drive).c_str(), "17",
        "Derelict Level", ITS(MC_Dere_Level).c_str(), "17",
        "Derelict Type", ITS(MC_Dere_Type).c_str(), "17",
        "Derelict Color", ITS(MC_Dere_Color).c_str(), "17",
        "Derelict Sub Octave", ITS(MC_Dere_Suboctave).c_str(), "17",
        "Derelict Pan", ITS(MC_Dere_Pan).c_str(), "17",
        "Derelict LPF", ITS(MC_Dere_LPF).c_str(), "17",
        "Derelict HPF", ITS(MC_Dere_HPF).c_str(), "17",

        "DistBand Dry/Wet", ITS(MC_DistBand_DryWet).c_str(), "23",
        "DistBand L/R Cross", ITS(MC_DistBand_LR_Cross).c_str(), "23",
        "DistBand Drive", ITS(MC_DistBand_Drive).c_str(), "23",
        "DistBand Level", ITS(MC_DistBand_Level).c_str(), "23",
        "DistBand L. Gain", ITS(MC_DistBand_Gain_Low).c_str(), "23",
        "DistBand M. Gain", ITS(MC_DistBand_Gain_Mid).c_str(), "23",
        "DistBand H. Gain", ITS(MC_DistBand_Gain_Hi).c_str(), "23",
        "DistBand Cross 1", ITS(MC_DistBand_Cross_1).c_str(), "23",
        "DistBand Cross 2", ITS(MC_DistBand_Cross_2).c_str(), "23",
        "DistBand Type Low", ITS(MC_DistBand_Type_Low).c_str(), "23",
        "DistBand Type Mid", ITS(MC_DistBand_Type_Mid).c_str(), "23",
        "DistBand Type High", ITS(MC_DistBand_Type_Hi).c_str(), "23",
        "DistBand Pan", ITS(MC_DistBand_Pan).c_str(), "23",

        "Distortion Dry/Wet", ITS(MC_Dist_DryWet).c_str(), "2",
        "Distortion L/R Cross", ITS(MC_Dist_LR_Cross).c_str(), "2",
        "Distortion Drive", ITS(MC_Dist_Drive).c_str(), "2",
        "Distortion Level", ITS(MC_Dist_Level).c_str(), "2",
        "Distortion Type", ITS(MC_Dist_Type).c_str(), "2",
        "Distortion Pan", ITS(MC_Dist_Pan).c_str(), "2",
        "Distortion Sub Octave", ITS(MC_Dist_Suboctave).c_str(), "2",
        "Distortion LPF", ITS(MC_Dist_LPF).c_str(), "2",
        "Distortion HPF", ITS(MC_Dist_HPF).c_str(), "2",

        "Dual Flange Dry/Wet", ITS(MC_DFlange_DryWet).c_str(), "20",
        "Dual Flange Pan", ITS(MC_DFlange_Pan).c_str(), "20",
        "Dual Flange L/R Cross", ITS(MC_DFlange_LR_Cross).c_str(), "20",
        "Dual Flange Depth", ITS(MC_DFlange_Depth).c_str(), "20",
        "Dual Flange Width", ITS(MC_DFlange_Width).c_str(), "20",
        "Dual Flange Offset", ITS(MC_DFlange_Offset).c_str(), "20",
        "Dual Flange Feedback", ITS(MC_DFlange_Feedback).c_str(), "20",
        "Dual Flange LPF", ITS(MC_DFlange_LPF).c_str(), "20",
        "Dual Flange Tempo", ITS(MC_DFlange_LFO_Tempo).c_str(), "20",
        "Dual Flange Stereo Df", ITS(MC_DFlange_LFO_Stereo).c_str(), "20",
        "Dual Flange LFO Type", ITS(MC_DFlange_LFO_Type).c_str(), "20",
        "Dual Flange Random", ITS(MC_DFlange_LFO_Random).c_str(), "20",

        "Echo Dry/Wet", ITS(MC_Echo_DryWet).c_str(), "4",
        "Echo Reverse", ITS(MC_Echo_Reverse).c_str(), "4",
        "Echo Pan", ITS(MC_Echo_Pan).c_str(), "4",
        "Echo Delay", ITS(MC_Echo_Delay).c_str(), "4",
        "Echo L/R Delay", ITS(MC_Echo_LR_Delay).c_str(), "4",
        "Echo L/R Cross", ITS(MC_Echo_LR_Cross).c_str(), "4",
        "Echo Feedback", ITS(MC_Echo_Feedback).c_str(), "4",
        "Echo Damp", ITS(MC_Echo_Damp).c_str(), "4",

        "Echotron Dry/Wet", ITS(MC_Echotron_DryWet).c_str(), "41",
        "Echotron Pan", ITS(MC_Echotron_Pan).c_str(), "41",
        "Echotron Tempo", ITS(MC_Echotron_Tempo).c_str(), "41",
        "Echotron Damp", ITS(MC_Echotron_Damp).c_str(), "41",
        "Echotron Feedback", ITS(MC_Echotron_Feedback).c_str(), "41",
        "Echotron L/R Cross", ITS(MC_Echotron_LR_Cross).c_str(), "41",
        "Echotron Width", ITS(MC_Echotron_LFO_Width).c_str(), "41",
        "Echotron Depth", ITS(MC_Echotron_Depth).c_str(), "41",
        "Echotron Stereo Df", ITS(MC_Echotron_LFO_Stereo).c_str(), "41",
        "Echotron LFO Type", ITS(MC_Echotron_LFO_Type).c_str(), "41",
        "Echotron #", ITS(MC_Echotron_Taps).c_str(), "41",

        "Echoverse Dry/Wet", ITS(MC_Echoverse_DryWet).c_str(), "32",
        "Echoverse Reverse", ITS(MC_Echoverse_Reverse).c_str(), "32",
        "Echoverse Pan", ITS(MC_Echoverse_Pan).c_str(), "32",
        "Echoverse Tempo", ITS(MC_Echoverse_Tempo).c_str(), "32",
        "Echoverse L/R Delay", ITS(MC_Echoverse_LR_Delay).c_str(), "32",
        "Echoverse Feedback", ITS(MC_Echoverse_Feedback).c_str(), "32",
        "Echoverse Damp", ITS(MC_Echoverse_Damp).c_str(), "32",
        "Echoverse Ex Stereo", ITS(MC_Echoverse_Ext_Stereo).c_str(), "32",
        "Echoverse Angle", ITS(MC_Echoverse_Angle).c_str(), "32",

        "EQ Gain", ITS(MC_EQ_Gain).c_str(), "0",
        "EQ Q", ITS(MC_EQ_Q).c_str(), "0",
        "EQ 31 Hz", ITS(MC_EQ_31_HZ).c_str(), "0",
        "EQ 63 Hz", ITS(MC_EQ_63_HZ).c_str(), "0",
        "EQ 125 Hz", ITS(MC_EQ_125_HZ).c_str(), "0",
        "EQ 250 Hz", ITS(MC_EQ_250_HZ).c_str(), "0",
        "EQ 500 Hz", ITS(MC_EQ_500_HZ).c_str(), "0",
        "EQ 1 Khz", ITS(MC_EQ_1_KHZ).c_str(), "0",
        "EQ 2 Khz", ITS(MC_EQ_2_KHZ).c_str(), "0",
        "EQ 4 Khz", ITS(MC_EQ_4_KHZ).c_str(), "0",
        "EQ 8 Khz", ITS(MC_EQ_8_KHZ).c_str(), "0",
        "EQ 16 Khz", ITS(MC_EQ_16_KHZ).c_str(), "0",

        "Exciter Gain", ITS(MC_Exciter_Gain).c_str(), "22",
        "Exciter LPF", ITS(MC_Exciter_LPF).c_str(), "22",
        "Exciter HPF", ITS(MC_Exciter_HPF).c_str(), "22",
        "Exciter Har 1", ITS(MC_Exciter_Harm_1).c_str(), "22",
        "Exciter Har 2", ITS(MC_Exciter_Harm_2).c_str(), "22",
        "Exciter Har 3", ITS(MC_Exciter_Harm_3).c_str(), "22",
        "Exciter Har 4", ITS(MC_Exciter_Harm_4).c_str(), "22",
        "Exciter Har 5", ITS(MC_Exciter_Harm_5).c_str(), "22",
        "Exciter Har 6", ITS(MC_Exciter_Harm_6).c_str(), "22",
        "Exciter Har 7", ITS(MC_Exciter_Harm_7).c_str(), "22",
        "Exciter Har 8", ITS(MC_Exciter_Harm_8).c_str(), "22",
        "Exciter Har 9", ITS(MC_Exciter_Harm_9).c_str(), "22",
        "Exciter Har 10", ITS(MC_Exciter_Harm_10).c_str(), "22",

        "Expander A.Time", ITS(MC_Expander_Attack).c_str(), "25",
        "Expander R.Time", ITS(MC_Expander_Release).c_str(), "25",
        "Expander Shape", ITS(MC_Expander_Shape).c_str(), "25",
        "Expander Threshold", ITS(MC_Expander_Threshold).c_str(), "25",
        "Expander Out Gain", ITS(MC_Expander_Gain).c_str(), "25",
        "Expander LPF", ITS(MC_Expander_LPF).c_str(), "25",
        "Expander HPF", ITS(MC_Expander_HPF).c_str(), "25",

        "Flanger Dry/Wet", ITS(MC_Flanger_DryWet).c_str(), "7",
        "Flanger Pan", ITS(MC_Flanger_Pan).c_str(), "7",
        "Flanger Tempo", ITS(MC_Flanger_LFO_Tempo).c_str(), "7",
        "Flanger Random", ITS(MC_Flanger_LFO_Random).c_str(), "7",
        "Flanger LFO Type", ITS(MC_Flanger_LFO_Type).c_str(), "7",
        "Flanger Stereo Df", ITS(MC_Flanger_LFO_Stereo).c_str(), "7",
        "Flanger Depth", ITS(MC_Flanger_Depth).c_str(), "7",
        "Flanger Delay", ITS(MC_Flanger_Delay).c_str(), "7",
        "Flanger Feedback", ITS(MC_Flanger_Feedback).c_str(), "7",
        "Flanger L/R Cross", ITS(MC_Flanger_LR_Cross).c_str(), "7",

        "Harmonizer Dry/Wet", ITS(MC_Harm_DryWet).c_str(), "14",
        "Harmonizer Interval", ITS(MC_Harm_Interval).c_str(), "14",
        "Harmonizer Gain", ITS(MC_Harm_Gain).c_str(), "14",
        "Harmonizer Pan", ITS(MC_Harm_Pan).c_str(), "14",
        "Harmonizer Freq", ITS(MC_Harm_Filter_Freq).c_str(), "14",
        "Harmonizer Filter Gain", ITS(MC_Harm_Filter_Gain).c_str(), "14",
        "Harmonizer Filter Q", ITS(MC_Harm_Filter_Q).c_str(), "14",
        "Harmonizer SELECT", ITS(MC_Harm_Select).c_str(), "14",
        "Harmonizer Note", ITS(MC_Harm_Note).c_str(), "14",
        "Harmonizer Chord", ITS(MC_Harm_Chord).c_str(), "14",

        "Infinity Dry/Wet", ITS(MC_Infinity_DryWet).c_str(), "46",
        "Infinity Res", ITS(MC_Infinity_Resonance).c_str(), "46",
        "Infinity Filter Band 1", ITS(MC_Infinity_Band_1).c_str(), "46",
        "Infinity Filter Band 2", ITS(MC_Infinity_Band_2).c_str(), "46",
        "Infinity Filter Band 3", ITS(MC_Infinity_Band_3).c_str(), "46",
        "Infinity Filter Band 4", ITS(MC_Infinity_Band_4).c_str(), "46",
        "Infinity Filter Band 5", ITS(MC_Infinity_Band_5).c_str(), "46",
        "Infinity Filter Band 6", ITS(MC_Infinity_Band_6).c_str(), "46",
        "Infinity Filter Band 7", ITS(MC_Infinity_Band_7).c_str(), "46",
        "Infinity Filter Band 8", ITS(MC_Infinity_Band_8).c_str(), "46",
        "Infinity AutoPan", ITS(MC_Infinity_AutoPan).c_str(), "46",
        "Infinity Stereo Df", ITS(MC_Infinity_LR_Delay).c_str(), "46",
        "Infinity Start", ITS(MC_Infinity_Start).c_str(), "46",
        "Infinity End", ITS(MC_Infinity_End).c_str(), "46",
        "Infinity Tempo", ITS(MC_Infinity_Tempo).c_str(), "46",
        "Infinity Subdiv", ITS(MC_Infinity_Subdivision).c_str(), "46",

        "Looper Dry/Wet", ITS(MC_Looper_DryWet).c_str(), "30",
        "Looper Level 1", ITS(MC_Looper_Level_1).c_str(), "30",
        "Looper Level 2", ITS(MC_Looper_Level_2).c_str(), "30",
        "Looper Tempo", ITS(MC_Looper_Tempo).c_str(), "30",
        "Looper Reverse", ITS(MC_Looper_Reverse).c_str(), "30",
        "Looper Auto Play", ITS(MC_Looper_AutoPlay).c_str(), "30",
        "Looper Play", ITS(MC_Looper_Play).c_str(), "30",
        "Looper Pause", ITS(MC_Looper_Stop).c_str(), "30",
        "Looper Record", ITS(MC_Looper_Record).c_str(), "30",
        "Looper R1", ITS(MC_Looper_Rec_1).c_str(), "30",
        "Looper R2", ITS(MC_Looper_Rec_2).c_str(), "30",
        "Looper Track 1", ITS(MC_Looper_Track_1).c_str(), "30",
        "Looper Track 2", ITS(MC_Looper_Track_2).c_str(), "30",
        "Looper Clear", ITS(MC_Looper_Clear).c_str(), "30",

        "Musical Delay Dry/Wet", ITS(MC_Music_DryWet).c_str(), "15",
        "Musical Delay L/R Cross", ITS(MC_Music_LR_Cross).c_str(), "15",
        "Musical Delay Pan 1", ITS(MC_Music_Pan_1).c_str(), "15",
        "Musical Delay Pan 2", ITS(MC_Music_Pan_2).c_str(), "15",
        "Musical Delay Tempo", ITS(MC_Music_Tempo).c_str(), "15",
        "Musical Delay Gain 1", ITS(MC_Music_Gain_1).c_str(), "15",
        "Musical Delay Gain 2", ITS(MC_Music_Gain_2).c_str(), "15",
        "Musical Delay FB 1", ITS(MC_Music_Feedback_1).c_str(), "15",
        "Musical Delay FB 2", ITS(MC_Music_Feedback_2).c_str(), "15",
        "Musical Delay Damp", ITS(MC_Music_Damp).c_str(), "15",

        "MuTroMojo Dry/Wet", ITS(MC_MuTro_DryWet).c_str(), "31",
        "MuTroMojo LP", ITS(MC_MuTro_LowPass).c_str(), "31",
        "MuTroMojo BP", ITS(MC_MuTro_BandPass).c_str(), "31",
        "MuTroMojo HP", ITS(MC_MuTro_HighPass).c_str(), "31",
        "MuTroMojo LFO Type", ITS(MC_MuTro_LFO_Type).c_str(), "31",
        "MuTroMojo Depth", ITS(MC_MuTro_Depth).c_str(), "31",
        "MuTroMojo Tempo", ITS(MC_MuTro_LFO_Tempo).c_str(), "31",
        "MuTroMojo Res", ITS(MC_MuTro_Resonance).c_str(), "31",
        "MuTroMojo Range", ITS(MC_MuTro_Range).c_str(), "31",
        "MuTroMojo Wah", ITS(MC_MuTro_Wah).c_str(), "31",
        "MuTroMojo E. Sens", ITS(MC_MuTro_Env_Sens).c_str(), "31",
        "MuTroMojo Smooth", ITS(MC_MuTro_Env_Smooth).c_str(), "31",
        "MuTroMojo Random", ITS(MC_MuTro_LFO_Random).c_str(), "31",
        "MuTroMojo Stereo Df", ITS(MC_MuTro_LFO_Stereo).c_str(), "31",
        "MuTroMojo St. Freq", ITS(MC_MuTro_St_Freq).c_str(), "31",

        "NoiseGate A. Time", ITS(MC_Gate_Attack).c_str(), "16",
        "NoiseGate R. Time", ITS(MC_Gate_Release).c_str(), "16",
        "NoiseGate Range", ITS(MC_Gate_Range).c_str(), "16",
        "NoiseGate Threshold", ITS(MC_Gate_Threshold).c_str(), "16",
        "NoiseGate Hold", ITS(MC_Gate_Hold).c_str(), "16",
        "NoiseGate LPF", ITS(MC_Gate_LPF).c_str(), "16",
        "NoiseGate HPF", ITS(MC_Gate_HPF).c_str(), "16",

        "Opticaltrem Depth", ITS(MC_Optical_Depth).c_str(), "44",
        "Opticaltrem Tempo", ITS(MC_Optical_LFO_Tempo).c_str(), "44",
        "Opticaltrem Random", ITS(MC_Optical_LFO_Random).c_str(), "44",
        "Opticaltrem LFO Type", ITS(MC_Optical_LFO_Type).c_str(), "44",
        "Opticaltrem Stereo Df", ITS(MC_Optical_LFO_Stereo).c_str(), "44",
        "Opticaltrem Pan", ITS(MC_Optical_Pan).c_str(), "44",

        "Overdrive Dry/Wet", ITS(MC_Overdrive_DryWet).c_str(), "3",
        "Overdrive L/R Cross", ITS(MC_Overdrive_LR_Cross).c_str(), "3",
        "Overdrive Drive", ITS(MC_Overdrive_Drive).c_str(), "3",
        "Overdrive Level", ITS(MC_Overdrive_Level).c_str(), "3",
        "Overdrive Type", ITS(MC_Overdrive_Type).c_str(), "3",
        "Overdrive Pan", ITS(MC_Overdrive_Pan).c_str(), "3",
        "Overdrive LPF", ITS(MC_Overdrive_LPF).c_str(), "3",
        "Overdrive HPF", ITS(MC_Overdrive_HPF).c_str(), "3",

        "Pan Dry/Wet", ITS(MC_Pan_DryWet).c_str(), "13",
        "Pan Pan", ITS(MC_Pan_Pan).c_str(), "13",
        "Pan Tempo", ITS(MC_Pan_LFO_Tempo).c_str(), "13",
        "Pan Random", ITS(MC_Pan_LFO_Random).c_str(), "13",
        "Pan LFO Type", ITS(MC_Pan_LFO_Type).c_str(), "13",
        "Pan Stereo Df", ITS(MC_Pan_LFO_Stereo).c_str(), "13",
        "Pan E. Stereo", ITS(MC_Pan_Ex_St_Amt).c_str(), "13",

        "Parametric EQ Gain", ITS(MC_Parametric_Gain).c_str(), "9",
        "Parametric EQ Low Freq", ITS(MC_Parametric_Low_Freq).c_str(), "9",
        "Parametric EQ Low Gain", ITS(MC_Parametric_Low_Gain).c_str(), "9",
        "Parametric EQ Low Q", ITS(MC_Parametric_Low_Q).c_str(), "9",
        "Parametric EQ Mid Freq", ITS(MC_Parametric_Mid_Freq).c_str(), "9",
        "Parametric EQ Mid Gain", ITS(MC_Parametric_Mid_Gain).c_str(), "9",
        "Parametric EQ Mid Q", ITS(MC_Parametric_Mid_Q).c_str(), "9",
        "Parametric EQ High Freq", ITS(MC_Parametric_High_Freq).c_str(), "9",
        "Parametric EQ High Gain", ITS(MC_Parametric_High_Gain).c_str(), "9",
        "Parametric EQ High Q", ITS(MC_Parametric_High_Q).c_str(), "9",

        "Phaser Dry/Wet", ITS(MC_Phaser_DryWet).c_str(), "6",
        "Phaser Pan", ITS(MC_Phaser_Pan).c_str(), "6",
        "Phaser Tempo", ITS(MC_Phaser_LFO_Tempo).c_str(), "6",
        "Phaser Random", ITS(MC_Phaser_LFO_Random).c_str(), "6",
        "Phaser LFO Type", ITS(MC_Phaser_LFO_Type).c_str(), "6",
        "Phaser Phase", ITS(MC_Phaser_Phase).c_str(), "6",
        "Phaser Stereo Df", ITS(MC_Phaser_LFO_Stereo).c_str(), "6",
        "Phaser Depth", ITS(MC_Phaser_Depth).c_str(), "6",
        "Phaser Feedback", ITS(MC_Phaser_Feedback).c_str(), "6",
        "Phaser L/R Cross", ITS(MC_Phaser_LR_Cross).c_str(), "6",

        "Reverb Dry/Wet", ITS(MC_Reverb_DryWet).c_str(), "8",
        "Reverb Pan", ITS(MC_Reverb_Pan).c_str(), "8",
        "Reverb Time", ITS(MC_Reverb_Time).c_str(), "8",
        "Reverb Initial Delay", ITS(MC_Reverb_I_Delay).c_str(), "8",
        "Reverb Del. E/R", ITS(MC_Reverb_Delay_FB).c_str(), "8",
        "Reverb Room Size", ITS(MC_Reverb_Room).c_str(), "8",
        "Reverb LPF", ITS(MC_Reverb_LPF).c_str(), "8",
        "Reverb HPF", ITS(MC_Reverb_HPF).c_str(), "8",
        "Reverb Damping", ITS(MC_Reverb_Damp).c_str(), "8",

        "Reverbtron Dry/Wet", ITS(MC_Revtron_DryWet).c_str(), "40",
        "Reverbtron Pan", ITS(MC_Revtron_Pan).c_str(), "40",
        "Reverbtron Level", ITS(MC_Revtron_Level).c_str(), "40",
        "Reverbtron Damp", ITS(MC_Revtron_Damp).c_str(), "40",
        "Reverbtron Feedback", ITS(MC_Revtron_Feedback).c_str(), "40",
        "Reverbtron Length", ITS(MC_Revtron_Length).c_str(), "40",
        "Reverbtron Stretch", ITS(MC_Revtron_Stretch).c_str(), "40",
        "Reverbtron Initial Delay", ITS(MC_Revtron_I_Delay).c_str(), "40",
        "Reverbtron Fade", ITS(MC_Revtron_Fade).c_str(), "40",
        "Reverbtron Diffusion", ITS(MC_Revtron_Diffusion).c_str(), "40",
        "Reverbtron LPF", ITS(MC_Revtron_LPF).c_str(), "40",

        "Ring Dry/Wet", ITS(MC_Ring_DryWet).c_str(), "21",
        "Ring L/R Cross", ITS(MC_Ring_LR_Cross).c_str(), "21",
        "Ring Input", ITS(MC_Ring_Input).c_str(), "21",
        "Ring Level", ITS(MC_Ring_Level).c_str(), "21",
        "Ring Pan", ITS(MC_Ring_Pan).c_str(), "21",
        "Ring Depth", ITS(MC_Ring_Depth).c_str(), "21",
        "Ring Freq", ITS(MC_Ring_Freq).c_str(), "21",
        "Ring Sine", ITS(MC_Ring_Sine).c_str(), "21",
        "Ring Triangle", ITS(MC_Ring_Triangle).c_str(), "21",
        "Ring Sawtooth", ITS(MC_Ring_Saw).c_str(), "21",
        "Ring Square", ITS(MC_Ring_Square).c_str(), "21",

        "Sequence Dry/Wet", ITS(MC_Sequence_DryWet).c_str(), "37",
        "Sequence 1", ITS(MC_Sequence_Step_1).c_str(), "37",
        "Sequence 2", ITS(MC_Sequence_Step_2).c_str(), "37",
        "Sequence 3", ITS(MC_Sequence_Step_3).c_str(), "37",
        "Sequence 4", ITS(MC_Sequence_Step_4).c_str(), "37",
        "Sequence 5", ITS(MC_Sequence_Step_5).c_str(), "37",
        "Sequence 6", ITS(MC_Sequence_Step_6).c_str(), "37",
        "Sequence 7", ITS(MC_Sequence_Step_7).c_str(), "37",
        "Sequence 8", ITS(MC_Sequence_Step_8).c_str(), "37",
        "Sequence Tempo", ITS(MC_Sequence_Tempo).c_str(), "37",
        "Sequence Q", ITS(MC_Sequence_Resonance).c_str(), "37",
        "Sequence Stereo Df", ITS(MC_Sequence_Stdf).c_str(), "37",

        "ShelfBoost Gain", ITS(MC_Shelf_Gain).c_str(), "34",
        "ShelfBoost Level", ITS(MC_Shelf_Level).c_str(), "34",
        "ShelfBoost Tone", ITS(MC_Shelf_Tone).c_str(), "34",
        "ShelfBoost Presence", ITS(MC_Shelf_Presence).c_str(), "34",

        "Shifter Dry/Wet", ITS(MC_Shifter_DryWet).c_str(), "38",
        "Shifter Interval", ITS(MC_Shifter_Interval).c_str(), "38",
        "Shifter Gain", ITS(MC_Shifter_Gain).c_str(), "38",
        "Shifter Pan", ITS(MC_Shifter_Pan).c_str(), "38",
        "Shifter Attack", ITS(MC_Shifter_Attack).c_str(), "38",
        "Shifter Decay", ITS(MC_Shifter_Decay).c_str(), "38",
        "Shifter Threshold", ITS(MC_Shifter_Threshold).c_str(), "38",
        "Shifter Whammy", ITS(MC_Shifter_Whammy).c_str(), "38",

        "Shuffle Dry/Wet", ITS(MC_Shuffle_DryWet).c_str(), "26",
        "Shuffle Low Freq", ITS(MC_Shuffle_Freq_L).c_str(), "26",
        "Shuffle Low Gain", ITS(MC_Shuffle_Gain_L).c_str(), "26",
        "Shuffle M.L. Freq", ITS(MC_Shuffle_Freq_ML).c_str(), "26",
        "Shuffle M.L. Gain", ITS(MC_Shuffle_Gain_ML).c_str(), "26",
        "Shuffle M.H. Freq", ITS(MC_Shuffle_Freq_MH).c_str(), "26",
        "Shuffle M.H. Gain", ITS(MC_Shuffle_Gain_MH).c_str(), "26",
        "Shuffle High Freq", ITS(MC_Shuffle_Freq_H).c_str(), "26",
        "Shuffle High Gain", ITS(MC_Shuffle_Gain_H).c_str(), "26",
        "Shuffle Q", ITS(MC_Shuffle_Width).c_str(), "26",

        "StereoHarm Dry/Wet", ITS(MC_Sharm_DryWet).c_str(), "42",
        "StereoHarm Int L", ITS(MC_Sharm_L_Interval).c_str(), "42",
        "StereoHarm Chrm L", ITS(MC_Sharm_L_Chroma).c_str(), "42",
        "StereoHarm Gain L", ITS(MC_Sharm_L_Gain).c_str(), "42",
        "StereoHarm Int R", ITS(MC_Sharm_R_Interval).c_str(), "42",
        "StereoHarm Chrm R", ITS(MC_Sharm_R_Chroma).c_str(), "42",
        "StereoHarm Gain R", ITS(MC_Sharm_R_Gain).c_str(), "42",
        "StereoHarm L/R Cross", ITS(MC_Sharm_LR_Cross).c_str(), "42",
        "StereoHarm SELECT", ITS(MC_Sharm_Select).c_str(), "42",
        "StereoHarm Note", ITS(MC_Sharm_Note).c_str(), "42",
        "StereoHarm Chord", ITS(MC_Sharm_Chord).c_str(), "42",

        "StompBox Level", ITS(MC_Stomp_Level).c_str(), "39",
        "StompBox Gain", ITS(MC_Stomp_Gain).c_str(), "39",
        "StompBox Bias", ITS(MC_Stomp_Bias).c_str(), "39",
        "StompBox Mid", ITS(MC_Stomp_Mid).c_str(), "39",
        "StompBox Tone", ITS(MC_Stomp_Tone).c_str(), "39",
        "StompBox Mode", ITS(MC_Stomp_Mode).c_str(), "39",

        "Sustainer Gain", ITS(MC_Sustain_Gain).c_str(), "36",
        "Sustainer Sustain", ITS(MC_Sustain_Sustain).c_str(), "36",

        "Synthfilter Dry/Wet", ITS(MC_Synthfilter_DryWet).c_str(), "27",
        "Synthfilter Distort", ITS(MC_Synthfilter_Distort).c_str(), "27",
        "Synthfilter Tempo", ITS(MC_Synthfilter_LFO_Tempo).c_str(), "27",
        "Synthfilter Random", ITS(MC_Synthfilter_LFO_Random).c_str(), "27",
        "Synthfilter LFO Type", ITS(MC_Synthfilter_LFO_Type).c_str(), "27",
        "Synthfilter Stereo Df", ITS(MC_Synthfilter_LFO_Stereo).c_str(), "27",
        "Synthfilter Width", ITS(MC_Synthfilter_Width).c_str(), "27",
        "Synthfilter Feedback", ITS(MC_Synthfilter_Feedback).c_str(), "27",
        "Synthfilter Depth", ITS(MC_Synthfilter_Depth).c_str(), "27",
        "Synthfilter E.Sens", ITS(MC_Synthfilter_Env_Sens).c_str(), "27",
        "Synthfilter A.Time", ITS(MC_Synthfilter_Attack).c_str(), "27",
        "Synthfilter R.Time", ITS(MC_Synthfilter_Release).c_str(), "27",
        "Synthfilter Offset", ITS(MC_Synthfilter_Offset).c_str(), "27",

        "Valve Dry/Wet", ITS(MC_Valve_DryWet).c_str(), "19",
        "Valve L/R Cross", ITS(MC_Valve_LR_Cross).c_str(), "19",
        "Valve Pan", ITS(MC_Valve_Pan).c_str(), "19",
        "Valve Level", ITS(MC_Valve_Level).c_str(), "19",
        "Valve Drive", ITS(MC_Valve_Drive).c_str(), "19",
        "Valve Dist", ITS(MC_Valve_Distortion).c_str(), "19",
        "Valve Presence", ITS(MC_Valve_Presence).c_str(), "19",
        "Valve LPF", ITS(MC_Valve_LPF).c_str(), "19",
        "Valve HPF", ITS(MC_Valve_HPF).c_str(), "19",

        "VaryBand Dry/Wet", ITS(MC_VaryBand_DryWet).c_str(), "28",
        "VaryBand Tempo 1", ITS(MC_VaryBand_LFO_Tempo_1).c_str(), "28",
        "VaryBand LFO 1 Type", ITS(MC_VaryBand_LFO_Type_1).c_str(), "28",
        "VaryBand St.df 1", ITS(MC_VaryBand_LFO_Stereo_1).c_str(), "28",
        "VaryBand Tempo 2", ITS(MC_VaryBand_LFO_Tempo_2).c_str(), "28",
        "VaryBand LFO 2 Type", ITS(MC_VaryBand_LFO_Type_2).c_str(), "28",
        "VaryBand St.df 2", ITS(MC_VaryBand_LFO_Stereo_2).c_str(), "28",
        "VaryBand Cross 1", ITS(MC_VaryBand_Cross_1).c_str(), "28",
        "VaryBand Cross 2", ITS(MC_VaryBand_Cross_2).c_str(), "28",
        "VaryBand Cross 3", ITS(MC_VaryBand_Cross_3).c_str(), "28",

        "Vibe Dry/Wet", ITS(MC_Vibe_DryWet).c_str(), "45",
        "Vibe Width", ITS(MC_Vibe_Width).c_str(), "45",
        "Vibe Depth", ITS(MC_Vibe_Depth).c_str(), "45",
        "Vibe Tempo", ITS(MC_Vibe_LFO_Tempo).c_str(), "45",
        "Vibe Random", ITS(MC_Vibe_LFO_Random).c_str(), "45",
        "Vibe LFO Type", ITS(MC_Vibe_LFO_Type).c_str(), "45",
        "Vibe Stereo Df", ITS(MC_Vibe_LFO_Stereo).c_str(), "45",
        "Vibe Feedback", ITS(MC_Vibe_Feedback).c_str(), "45",
        "Vibe L/R Cross", ITS(MC_Vibe_LR_Cross).c_str(), "45",
        "Vibe Pan", ITS(MC_Vibe_Pan).c_str(), "45",

        "Vocoder Dry/Wet", ITS(MC_Vocoder_DryWet).c_str(), "35",
        "Vocoder Pan", ITS(MC_Vocoder_Pan).c_str(), "35",
        "Vocoder Input", ITS(MC_Vocoder_Input).c_str(), "35",
        "Vocoder Smear", ITS(MC_Vocoder_Smear).c_str(), "35",
        "Vocoder Q", ITS(MC_Vocoder_Q).c_str(), "35",
        "Vocoder Ring", ITS(MC_Vocoder_Ring).c_str(), "35",
        "Vocoder Level", ITS(MC_Vocoder_Level).c_str(), "35",

        "WahWah Dry/Wet", ITS(MC_WahWah_DryWet).c_str(), "10",
        "WahWah Pan", ITS(MC_WahWah_Pan).c_str(), "10",
        "WahWah Tempo", ITS(MC_WahWah_LFO_Tempo).c_str(), "10",
        "WahWah Random", ITS(MC_WahWah_LFO_Random).c_str(), "10",
        "WahWah LFO Type", ITS(MC_WahWah_LFO_Type).c_str(), "10",
        "WahWah Stereo Df", ITS(MC_WahWah_LFO_Stereo).c_str(), "10",
        "WahWah Depth", ITS(MC_WahWah_Depth).c_str(), "10",
        "WahWah Amp S.", ITS(MC_WahWah_Sense).c_str(), "10",
        "WahWah Amp S.I.", ITS(MC_WahWah_ASI).c_str(), "10",
        "WahWah Smooth", ITS(MC_WahWah_Smooth).c_str(), "10",
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

    if ((Looper_Bypass) && (Tap_Selection == 3))
    {
        if (midievent->type == SND_SEQ_EVENT_START)
        {
            efx_Looper->changepar(Looper_Play, 1);
            stecla = 5;
        }

        if (midievent->type == SND_SEQ_EVENT_STOP)
        {
            efx_Looper->changepar(Looper_Stop, 1);
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
    case 0:
        if (inoff) EQ1_Bypass = 1;
        else EQ1_Bypass = 0;
        break;
    case 1:
        if (inoff) Compressor_Bypass = 1;
        else Compressor_Bypass = 0;
        break;
    case 2:
        if (inoff) Distorsion_Bypass = 1;
        else Distorsion_Bypass = 0;
        break;
    case 3:
        if (inoff) Overdrive_Bypass = 1;
        else Overdrive_Bypass = 0;
        break;
    case 4:
        if (inoff) Echo_Bypass = 1;
        else Echo_Bypass = 0;
        break;
    case 5:
        if (inoff) Chorus_Bypass = 1;
        else Chorus_Bypass = 0;
        break;
    case 6:
        if (inoff) Phaser_Bypass = 1;
        else Phaser_Bypass = 0;
        break;
    case 7:
        if (inoff) Flanger_Bypass = 1;
        else Flanger_Bypass = 0;
        break;
    case 8:
        if (inoff) Reverb_Bypass = 1;
        else Reverb_Bypass = 0;
        break;
    case 9:
        if (inoff) EQ2_Bypass = 1;
        else EQ2_Bypass = 0;
        break;
    case 10:
        if (inoff) WahWah_Bypass = 1;
        else WahWah_Bypass = 0;
        break;
    case 11:
        if (inoff) Alienwah_Bypass = 1;
        else Alienwah_Bypass = 0;
        break;
    case 12:
        if (inoff) Cabinet_Bypass = 1;
        else Cabinet_Bypass = 0;
        break;
    case 13:
        if (inoff) Pan_Bypass = 1;
        else Pan_Bypass = 0;
        break;
    case 14:
        if (inoff) Harmonizer_Bypass = 1;
        else Harmonizer_Bypass = 0;
        break;
    case 15:
        if (inoff) MusDelay_Bypass = 1;
        else MusDelay_Bypass = 0;
        break;
    case 16:
        if (inoff) Gate_Bypass = 1;
        else Gate_Bypass = 0;
        break;
    case 17:
        if (inoff) Derelict_Bypass = 1;
        else Derelict_Bypass = 0;
        break;
    case 18:
        if (inoff) APhaser_Bypass = 1;
        else APhaser_Bypass = 0;
        break;
    case 19:
        if (inoff) Valve_Bypass = 1;
        else Valve_Bypass = 0;
        break;
    case 20:
        if (inoff) DFlange_Bypass = 1;
        else DFlange_Bypass = 0;
        break;
    case 21:
        if (inoff) Ring_Bypass = 1;
        else Ring_Bypass = 0;
        break;
    case 22:
        if (inoff) Exciter_Bypass = 1;
        else Exciter_Bypass = 0;
        break;
    case 23:
        if (inoff) DistBand_Bypass = 1;
        else DistBand_Bypass = 0;
        break;
    case 24:
        if (inoff) Arpie_Bypass = 1;
        else Arpie_Bypass = 0;
        break;
    case 25:
        if (inoff) Expander_Bypass = 1;
        else Expander_Bypass = 0;
        break;
    case 26:
        if (inoff) Shuffle_Bypass = 1;
        else Shuffle_Bypass = 0;
        break;
    case 27:
        if (inoff) Synthfilter_Bypass = 1;
        else Synthfilter_Bypass = 0;
        break;
    case 28:
        if (inoff) VaryBand_Bypass = 1;
        else VaryBand_Bypass = 0;
        break;
    case 29:
        if (inoff) Convol_Bypass = 1;
        else Convol_Bypass = 0;
        break;
    case 30:
        if (inoff) Looper_Bypass = 1;
        else Looper_Bypass = 0;
        break;
    case 31:
        if (inoff) MuTroMojo_Bypass = 1;
        else MuTroMojo_Bypass = 0;
        break;
    case 32:
        if (inoff) Echoverse_Bypass = 1;
        else Echoverse_Bypass = 0;
        break;
    case 33:
        if (inoff) CoilCrafter_Bypass = 1;
        else CoilCrafter_Bypass = 0;
        break;
    case 34:
        if (inoff) ShelfBoost_Bypass = 1;
        else ShelfBoost_Bypass = 0;
        break;
    case 35:
        if (inoff) Vocoder_Bypass = 1;
        else Vocoder_Bypass = 0;
        break;
    case 36:
        if (inoff) Sustainer_Bypass = 1;
        else Sustainer_Bypass = 0;
        break;
    case 37:
        if (inoff) Sequence_Bypass = 1;
        else Sequence_Bypass = 0;
        break;
    case 38:
        if (inoff) Shifter_Bypass = 1;
        else Shifter_Bypass = 0;
        break;
    case 39:
        if (inoff) StompBox_Bypass = 1;
        else StompBox_Bypass = 0;
        break;
    case 40:
        if (inoff) Reverbtron_Bypass = 1;
        else Reverbtron_Bypass = 0;
        break;
    case 41:
        if (inoff) Echotron_Bypass = 1;
        else Echotron_Bypass = 0;
        break;
    case 42:
        if (inoff) StereoHarm_Bypass = 1;
        else StereoHarm_Bypass = 0;
        break;
    case 43:
        if (inoff) CompBand_Bypass = 1;
        else CompBand_Bypass = 0;
        break;
    case 44:
        if (inoff) Opticaltrem_Bypass = 1;
        else Opticaltrem_Bypass = 0;
        break;
    case 45:
        if (inoff) Vibe_Bypass = 1;
        else Vibe_Bypass = 0;
        break;
    case 46:
        if (inoff) Infinity_Bypass = 1;
        else Infinity_Bypass = 0;
        break;


    case 121:
        if (inoff) Tap_Bypass = 1;
        else Tap_Bypass = 0;
        break;
    case 122:
        if (inoff) MIDIConverter_Bypass = 1;
        else MIDIConverter_Bypass = 0;
        break;
    case 123:
        if (inoff) Tuner_Bypass = 1;
        else Tuner_Bypass = 0;
        break;
    case 124:
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
    case 0:
        if (EQ1_Bypass) return (0);
        break;
    case 1:
        if (Compressor_Bypass) return (0);
        break;
    case 2:
        if (Distorsion_Bypass) return (0);
        break;
    case 3:
        if (Overdrive_Bypass)return (0);
        break;
    case 4:
        if (Echo_Bypass)return (0);
        break;
    case 5:
        if (Chorus_Bypass) return (0);
        break;
    case 6:
        if (Phaser_Bypass) return (0);
        break;
    case 7:
        if (Flanger_Bypass) return (0);
        break;
    case 8:
        if (Reverb_Bypass) return (0);
        break;
    case 9:
        if (EQ2_Bypass) return (0);
        break;
    case 10:
        if (WahWah_Bypass) return (0);
        break;
    case 11:
        if (Alienwah_Bypass) return (0);
        break;
    case 12:
        if (Cabinet_Bypass) return (0);
        break;
    case 13:
        if (Pan_Bypass) return (0);
        break;
    case 14:
        if (Harmonizer_Bypass) return (0);
        break;
    case 15:
        if (MusDelay_Bypass) return (0);
        break;
    case 16:
        if (Gate_Bypass) return (0);
        break;
    case 17:
        if (Derelict_Bypass) return (0);
        break;
    case 18:
        if (APhaser_Bypass) return (0);
        break;
    case 19:
        if (Valve_Bypass) return (0);
        break;
    case 20:
        if (DFlange_Bypass) return (0);
        break;
    case 21:
        if (Ring_Bypass) return (0);
        break;
    case 22:
        if (Exciter_Bypass) return (0);
        break;
    case 23:
        if (DistBand_Bypass) return (0);
        break;
    case 24:
        if (Arpie_Bypass) return (0);
        break;
    case 25:
        if (Expander_Bypass) return (0);
        break;
    case 26:
        if (Shuffle_Bypass) return (0);
        break;
    case 27:
        if (Synthfilter_Bypass) return (0);
        break;
    case 28:
        if (VaryBand_Bypass) return (0);
        break;
    case 29:
        if (Convol_Bypass) return (0);
        break;
    case 30:
        if (Looper_Bypass) return (0);
        break;
    case 31:
        if (MuTroMojo_Bypass) return (0);
        break;
    case 32:
        if (Echoverse_Bypass) return (0);
        break;
    case 33:
        if (CoilCrafter_Bypass) return (0);
        break;
    case 34:
        if (ShelfBoost_Bypass) return (0);
        break;
    case 35:
        if (Vocoder_Bypass) return (0);
        break;
    case 36:
        if (Sustainer_Bypass) return (0);
        break;
    case 37:
        if (Sequence_Bypass) return (0);
        break;
    case 38:
        if (Shifter_Bypass) return (0);
        break;
    case 39:
        if (StompBox_Bypass) return (0);
        break;
    case 40:
        if (Reverbtron_Bypass) return (0);
        break;
    case 41:
        if (Echotron_Bypass) return (0);
        break;
    case 42:
        if (StereoHarm_Bypass) return (0);
        break;
    case 43:
        if (CompBand_Bypass) return (0);
        break;
    case 44:
        if (Opticaltrem_Bypass) return (0);
        break;
    case 45:
        if (Vibe_Bypass) return (0);
        break;
    case 46:
        if (Infinity_Bypass) return (0);
        break;

    case 121:
        if (Tap_Bypass) return 0;
        break;
    case 122:
        if (MIDIConverter_Bypass) return 0;
        break;
    case 123:
        if (Tuner_Bypass) return 0;
        break;
    case 124:
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
#if 0
    for(int i = 0; i < C_MC_PARAMETER_SIZE; i++)
    {
        if(parameter == efx_params[i].Ato)
        {
            printf("Effect Name = %s: parameter = %d: Effect = %d\n",
                    efx_params[i].Nom, efx_params[i].Ato, efx_params[i].Effect);
            break;
        }
    }
#endif // 0
    
    /* Don't process MIDI control when updating quality since
       the efx may be deleted */
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
    case 0: // Unused
        break;
        
// Start MIDI Implementation Chart range

    case MC_WahWah_Depth:
        efx_WahWah->changepar(WahWah_Depth, value);
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
        efx_Distorsion->changepar(Dist_Suboctave, value);
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
        efx_Alienwah->changepar(Alien_Depth, value);
        break;

    case MC_Phaser_Depth:
        efx_Phaser->changepar(Phaser_Depth, value);
        break;

    case MC_Flanger_Depth:
        efx_Flanger->changepar(Flanger_Depth, value);
        break;

    case MC_Chorus_Depth:
        efx_Chorus->changepar(Chorus_Depth, value);
        break;

    case MC_Music_Gain_1:
        efx_MusDelay->changepar(Music_Gain_1, value);
        break;

    case MC_Music_Gain_2:
        efx_MusDelay->changepar(Music_Gain_2, value);
        break;

    case MC_Harm_Filter_Freq:
        efx_Har->changepar(Harm_Filter_Freq, ret_LPF(value));
        break;

    case MC_Harm_Interval:
        if (Harmonizer_Bypass)
        {
            Harmonizer_Bypass = 0;
            efx_Har->changepar(Harm_Interval, (int) ((float) value * C_MC_24_RANGE));
            Harmonizer_Bypass = 1;
        }
        break;

    case MC_WahWah_DryWet:
        efx_WahWah->changepar(WahWah_DryWet, Dry_Wet(value));
        break;

    case MC_Overdrive_DryWet:
        efx_Overdrive->changepar(Overdrive_DryWet, Dry_Wet(value));
        break;

    case MC_Dist_DryWet:
        efx_Distorsion->changepar(Dist_DryWet, Dry_Wet(value));
        break;

    case MC_Harm_DryWet:
        efx_Har->changepar(Harm_DryWet, Dry_Wet(value));
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
        efx_Echo->changepar(Echo_Pan, value);
        break;

    case MC_Overdrive_Pan:
        efx_Overdrive->changepar(Overdrive_Pan, value);
        break;

    case MC_Dist_Pan:
        efx_Distorsion->changepar(Dist_Pan, value);
        break;

    case MC_Harm_Pan:
        efx_Har->changepar(Harm_Pan, value);
        break;

    case MC_Chorus_Pan:
        efx_Chorus->changepar(Chorus_Pan, value);
        break;

    case MC_Flanger_Pan:
        efx_Flanger->changepar(Flanger_Pan, value);
        break;

    case MC_Chorus_DryWet:
        efx_Chorus->changepar(Chorus_DryWet, Dry_Wet(value));
        break;

    case MC_Flanger_DryWet:
        efx_Flanger->changepar(Flanger_DryWet, Dry_Wet(value));
        break;

    case MC_Phaser_DryWet:
        efx_Phaser->changepar(Phaser_DryWet, Dry_Wet(value));
        break;

    case MC_Alien_DryWet:
        efx_Alienwah->changepar(Alien_DryWet, Dry_Wet(value));
        break;

    case MC_Music_DryWet:
        efx_MusDelay->changepar(Music_DryWet, Dry_Wet(value));
        break;

    case MC_Reverb_DryWet:
        efx_Rev->changepar(Reverb_DryWet, Dry_Wet(value));
        break;

    case MC_Pan_DryWet:
        efx_Pan->changepar(Pan_DryWet, Dry_Wet(value));
        break;

    case MC_Echo_DryWet:
        efx_Echo->changepar(Echo_DryWet, Dry_Wet(value));
        break;

    case MC_Phaser_Pan:
        efx_Phaser->changepar(Phaser_Pan, value);
        break;

    case MC_Alien_Pan:
        efx_Alienwah->changepar(Alien_Pan, value);
        break;

    case MC_Music_Pan_1:
        efx_MusDelay->changepar(Music_Pan_1, value);
        break;

    case MC_Reverb_Pan:
        efx_Rev->changepar(Reverb_Pan, value);
        break;

    case MC_Unused_64:
        break;

    case MC_Music_Pan_2:
        efx_MusDelay->changepar(Music_Pan_2, value);
        break;

    case MC_WahWah_Pan:
        efx_WahWah->changepar(WahWah_Pan, value);
        break;

    case MC_Pan_Pan:
        efx_Pan->changepar(Pan_Pan, value);
        break;

    case MC_Overdrive_Drive:
        efx_Overdrive->changepar(Overdrive_Drive, value);
        break;

    case MC_Dist_Drive:
        efx_Distorsion->changepar(Dist_Drive, value);
        break;

    case MC_Overdrive_Level:
        efx_Overdrive->changepar(Overdrive_Level, value);
        break;

    case MC_Dist_Level:
        efx_Distorsion->changepar(Dist_Level, value);
        break;

    case MC_Chorus_LFO_Tempo:
        efx_Chorus->changepar(Chorus_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Flanger_LFO_Tempo:
        efx_Flanger->changepar(Flanger_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Phaser_LFO_Tempo:
        efx_Phaser->changepar(Phaser_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_WahWah_LFO_Tempo:
        efx_WahWah->changepar(WahWah_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Alien_LFO_Tempo:
        efx_Alienwah->changepar(Alien_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Pan_LFO_Tempo:
        efx_Pan->changepar(Pan_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_Echo_Feedback:
        efx_Echo->changepar(Echo_Feedback, value);
        break;

    case MC_Chorus_Feedback:
        efx_Chorus->changepar(Chorus_Feedback, value);
        break;

    case MC_Flanger_Feedback:
        efx_Flanger->changepar(Flanger_Feedback, value);
        break;

    case MC_Phaser_Feedback:
        efx_Phaser->changepar(Phaser_Feedback, value);
        break;

    case MC_Alien_Feedback:
        efx_Alienwah->changepar(Alien_Feedback, value);
        break;

    case MC_Music_Feedback_1:
        efx_MusDelay->changepar(Music_Feedback_1, value);
        break;

    case MC_Music_Feedback_2:
        efx_MusDelay->changepar(Music_Feedback_2, value);
        break;

    case MC_Overdrive_LPF:
        efx_Overdrive->changepar(Overdrive_LPF, ret_LPF(value));
        break;

    case MC_Dist_LPF:
        efx_Distorsion->changepar(Dist_LPF, ret_LPF(value));
        break;

    case MC_Reverb_LPF:
        efx_Rev->changepar(Reverb_LPF, ret_LPF(value));
        break;

    case MC_Overdrive_HPF:
        efx_Overdrive->changepar(Overdrive_HPF, ret_HPF(value));
        break;

    case MC_Dist_HPF:
        efx_Distorsion->changepar(Dist_HPF, ret_HPF(value));
        break;

    case MC_Reverb_HPF:
        efx_Rev->changepar(Reverb_HPF, ret_HPF(value));
        break;

    case MC_Chorus_LR_Cross:
        efx_Chorus->changepar(Chorus_LR_Cross, value);
        break;

    case MC_Flanger_LR_Cross:
        efx_Flanger->changepar(Flanger_LR_Cross, value);
        break;

    case MC_Phaser_LR_Cross:
        efx_Phaser->changepar(Phaser_LR_Cross, value);
        break;

    case MC_Overdrive_LR_Cross:
        efx_Overdrive->changepar(Overdrive_LR_Cross, value);
        break;

    case MC_Dist_LR_Cross:
        efx_Distorsion->changepar(Dist_LR_Cross, value);
        break;

    case MC_Alien_LR_Cross:
        efx_Alienwah->changepar(Alien_LR_Cross, value);
        break;

    case MC_Echo_LR_Cross:
        efx_Echo->changepar(Echo_LR_Cross, value);
        break;

    case MC_Music_LR_Cross:
        efx_MusDelay->changepar(Music_LR_Cross, value);
        break;

    case MC_Chorus_LFO_Stereo:
        efx_Chorus->changepar(Chorus_LFO_Stereo, value);
        break;

    case MC_Flanger_LFO_Stereo:
        efx_Flanger->changepar(Flanger_LFO_Stereo, value);
        break;

    case MC_Phaser_LFO_Stereo:
        efx_Phaser->changepar(Phaser_LFO_Stereo, value);
        break;

    case MC_WahWah_LFO_Stereo:
        efx_WahWah->changepar(WahWah_LFO_Stereo, value);
        break;

    case MC_Alien_LFO_Stereo:
        efx_Alienwah->changepar(Alien_LFO_Stereo, value);
        break;

    case MC_Pan_LFO_Stereo:
        efx_Pan->changepar(Pan_LFO_Stereo, value);
        break;

    case MC_Chorus_LFO_Random:
        efx_Chorus->changepar(Chorus_LFO_Random, value);
        break;

    case MC_Flanger_LFO_Random:
        efx_Flanger->changepar(Flanger_LFO_Random, value);
        break;

    case MC_Phaser_LFO_Random:
        efx_Phaser->changepar(Phaser_LFO_Random, value);
        break;

    case MC_WahWah_LFO_Random:
        efx_WahWah->changepar(WahWah_LFO_Random, value);
        break;

    case MC_Alien_LFO_Random:
        efx_Alienwah->changepar(Alien_LFO_Random, value);
        break;

    case MC_Pan_LFO_Random:
        efx_Pan->changepar(Pan_LFO_Random, value);
        break;

    case MC_WahWah_Sense:
        efx_WahWah->changepar(WahWah_Sense, value);
        break;

    case MC_WahWah_ASI:
        efx_WahWah->changepar(WahWah_ASI, value);
        break;

    case MC_WahWah_Smooth:
        efx_WahWah->changepar(WahWah_Smooth, value);
        break;

    case MC_Phaser_Phase:
        efx_Phaser->changepar(Phaser_Phase, value);
        break;

    case MC_Alien_Phase:
        efx_Alienwah->changepar(Alien_Phase, value);
        break;

    case MC_Multi_On_Off:
        ActiveUn(value);
        break;

    case MC_APhase_DryWet:
        efx_APhaser->changepar(APhase_DryWet, Dry_Wet(value));
        break;

    case MC_APhase_Distortion:
        efx_APhaser->changepar(APhase_Distortion, value);
        break;

    case MC_APhase_LFO_Tempo:
        efx_APhaser->changepar(APhase_LFO_Tempo, ret_Tempo(value));
        break;

    case MC_APhase_Depth:
        efx_APhaser->changepar(APhase_Depth, value);
        break;

    case MC_APhase_Width:
        efx_APhaser->changepar(APhase_Width, value);
        break;

    case MC_APhase_Feedback:
        efx_APhaser->changepar(APhase_Feedback, (int) ((float) value * C_MC_128_RANGE));
        break;

    case MC_APhase_Mismatch:
        efx_APhaser->changepar(APhase_Mismatch, value);
        break;

    case MC_APhase_LFO_Stereo:
        efx_APhaser->changepar(APhase_LFO_Stereo, value);
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
        efx_EQ1->changepar(EQ_Gain, value);
        break;

    case MC_EQ_Q:
        efx_EQ1->changepar_Q(EQ_Q, value);
        break;

    case MC_EQ_31_HZ:
        efx_EQ1->changepar(EQ_31_HZ, value);
        break;

    case MC_EQ_63_HZ:
        efx_EQ1->changepar(EQ_63_HZ, value);
        break;

    case MC_EQ_125_HZ:
        efx_EQ1->changepar(EQ_125_HZ, value);
        break;

    case MC_EQ_250_HZ:
        efx_EQ1->changepar(EQ_250_HZ, value);
        break;

    case MC_EQ_500_HZ:
        efx_EQ1->changepar(EQ_500_HZ, value);
        break;

    case MC_EQ_1_KHZ:
        efx_EQ1->changepar(EQ_1_KHZ, value);
        break;

    case MC_EQ_2_KHZ:
        efx_EQ1->changepar(EQ_2_KHZ, value);
        break;

    case MC_EQ_4_KHZ:
        efx_EQ1->changepar(EQ_4_KHZ, value);
        break;

    case MC_EQ_8_KHZ:
        efx_EQ1->changepar(EQ_8_KHZ, value);
        break;

    case MC_EQ_16_KHZ:
        efx_EQ1->changepar(EQ_16_KHZ, value);
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
        efx_EQ2->changepar(Parametric_Gain, value);
        break;

    case MC_Parametric_Low_Freq:
        efx_EQ2->changepar(Parametric_Low_Freq, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case MC_Parametric_Low_Gain:
        efx_EQ2->changepar(Parametric_Low_Gain, value);
        break;

    case MC_Parametric_Low_Q:
        efx_EQ2->changepar(Parametric_Low_Q, value);
        break;

    case MC_Parametric_Mid_Freq:
        efx_EQ2->changepar(Parametric_Mid_Freq, 800 + (int) ((float) value * C_MC_7200_RANGE));
        break;

    case MC_Parametric_Mid_Gain:
        efx_EQ2->changepar(Parametric_Mid_Gain, value);
        break;

    case MC_Parametric_Mid_Q:
        efx_EQ2->changepar(Parametric_Mid_Q, value);
        break;

    case MC_Parametric_High_Freq:
        efx_EQ2->changepar(Parametric_High_Freq, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case MC_Parametric_High_Gain:
        efx_EQ2->changepar(Parametric_High_Gain, value);
        break;

    case MC_Parametric_High_Q:
        efx_EQ2->changepar(Parametric_High_Q, value);
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

    case 202:
        efx_DistBand->changepar(DistBand_DryWet, Dry_Wet(value));
        break;

    case 203:
        efx_DistBand->changepar(DistBand_LR_Cross, value);
        break;

    case 204:
        efx_DistBand->changepar(DistBand_Drive, value);
        break;

    case 205:
        efx_DistBand->changepar(DistBand_Level, value);
        break;

    case 206:
        efx_DistBand->changepar(DistBand_Gain_Low, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 207:
        efx_DistBand->changepar(DistBand_Gain_Mid, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 208:
        efx_DistBand->changepar(DistBand_Gain_Hi, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 209:
        efx_DistBand->changepar(DistBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 210:
        efx_DistBand->changepar(DistBand_Cross_2, 800 + (int) ((float) value * C_MC_11200_RANGE));
        break;

    case 211:
        efx_DistBand->changepar(DistBand_Pan, value);
        break;

    case 212:
        efx_Arpie->changepar(Arpie_DryWet, Dry_Wet(value));
        break;

    case 213:
        efx_Arpie->changepar(Arpie_ArpeWD, value);
        break;

    case 214:
        efx_Arpie->changepar(Arpie_Pan, value);
        break;

    case 215:
        efx_Arpie->changepar(Arpie_Tempo, ret_Tempo(value));
        break;

    case 216:
        efx_Arpie->changepar(Arpie_LR_Delay, value);
        break;

    case 217:
        efx_Arpie->changepar(Arpie_LR_Cross, value);
        break;

    case 218:
        efx_Arpie->changepar(Arpie_Feedback, value);
        break;

    case 219:
        efx_Arpie->changepar(Arpie_Damp, value);
        break;

    case 220:
        efx_Expander->changepar(Expander_Attack, 1 + (int) ((float) value * C_MC_4999_RANGE));
        break;

    case 221:
        efx_Expander->changepar(Expander_Release, 10 + (int) ((float) value * C_MC_990_RANGE));
        break;

    case 222:
        efx_Expander->changepar(Expander_Shape, 1 + (int) ((float) value * C_MC_49_RANGE));
        break;

    case 223:
        efx_Expander->changepar(Expander_Threshold, (int) ((float) value * -C_MC_80_RANGE));
        break;

    case 224:
        efx_Expander->changepar(Expander_Gain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 225:
        efx_Expander->changepar(Expander_LPF, ret_LPF(value));
        break;

    case 226:
        efx_Expander->changepar(Expander_HPF, ret_HPF(value));
        break;

    case 227:
        efx_Shuffle->changepar(Shuffle_DryWet, Dry_Wet(value));
        break;

    case 228:
        efx_Shuffle->changepar(Shuffle_Freq_L, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 229:
        efx_Shuffle->changepar(Shuffle_Gain_L, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 230:
        efx_Shuffle->changepar(Shuffle_Freq_ML, 400 + (int) ((float) value * C_MC_3600_RANGE));
        break;

    case 231:
        efx_Shuffle->changepar(Shuffle_Gain_ML, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 232:
        efx_Shuffle->changepar(Shuffle_Freq_MH, 1200 + (int) ((float) value * C_MC_6800_RANGE));
        break;

    case 233:
        efx_Shuffle->changepar(Shuffle_Gain_MH, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 234:
        efx_Shuffle->changepar(Shuffle_Freq_H, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case 235:
        efx_Shuffle->changepar(Shuffle_Gain_H, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 236:
        efx_Shuffle->changepar(Shuffle_Width, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 237:
        efx_Synthfilter->changepar(Synthfilter_DryWet, Dry_Wet(value));
        break;

    case 238:
        efx_Synthfilter->changepar(Synthfilter_Distort, value);
        break;

    case 239:
        efx_Synthfilter->changepar(Synthfilter_LFO_Tempo, ret_Tempo(value));
        break;

    case 240:
        efx_Synthfilter->changepar(Synthfilter_LFO_Stereo, value);
        break;

    case 241:
        efx_Synthfilter->changepar(Synthfilter_Width, value);
        break;

    case 242:
        efx_Synthfilter->changepar(Synthfilter_Feedback, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 243:
        efx_Synthfilter->changepar(Synthfilter_Depth, value);
        break;

    case 244:
        efx_Synthfilter->changepar(Synthfilter_Env_Sens, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 245:
        efx_Synthfilter->changepar(Synthfilter_Attack, 5 + (int) ((float) value * C_MC_995_RANGE));
        break;

    case 246:
        efx_Synthfilter->changepar(Synthfilter_Release, 5 + (int) ((float) value * C_MC_495_RANGE));
        break;

    case 247:
        efx_Synthfilter->changepar(Synthfilter_Offset, value);
        break;

    case 248:
        efx_VaryBand->changepar(VaryBand_DryWet, Dry_Wet(value));
        break;

    case 249:
        efx_VaryBand->changepar(VaryBand_LFO_Tempo_1, ret_Tempo(value));
        break;

    case 250:
        efx_VaryBand->changepar(VaryBand_LFO_Stereo_1, value);
        break;

    case 251:
        efx_VaryBand->changepar(VaryBand_LFO_Tempo_2, ret_Tempo(value));
        break;

    case 252:
        efx_VaryBand->changepar(VaryBand_LFO_Stereo_2, value);
        break;

    case 253:
        efx_VaryBand->changepar(VaryBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 254:
        efx_VaryBand->changepar(VaryBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case 255:
        efx_VaryBand->changepar(VaryBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case 256:
        efx_MuTroMojo->changepar(MuTro_DryWet, Dry_Wet(value));
        break;

    case 257:
        efx_MuTroMojo->changepar(MuTro_LowPass, value - 64);
        break;

    case 258:
        efx_MuTroMojo->changepar(MuTro_BandPass, value - 64);
        break;

    case 259:
        efx_MuTroMojo->changepar(MuTro_HighPass, value - 64);
        break;

    case 260:
        efx_MuTroMojo->changepar(MuTro_Depth, value);
        break;

    case 261:
        efx_MuTroMojo->changepar(MuTro_LFO_Tempo, ret_Tempo(value));
        break;

    case 262:
        efx_MuTroMojo->changepar(MuTro_Resonance, value);
        break;

    case 263:
        efx_MuTroMojo->changepar(MuTro_Range, 10 + (int) ((float) value * C_MC_5990_RANGE));
        break;

    case 264:
        efx_MuTroMojo->changepar(MuTro_Wah, value);
        break;

    case 265:
        efx_MuTroMojo->changepar(MuTro_Env_Sens, value - 64);
        break;

    case 266:
        efx_MuTroMojo->changepar(MuTro_Env_Smooth, value);
        break;

    case 267:
        efx_Looper->changepar(Looper_DryWet, Dry_Wet(value));
        break;

    case 268:
        efx_Looper->changepar(Looper_Level_1, value);
        break;

    case 269:
        efx_Looper->changepar(Looper_Level_2, value);
        break;

    case 270:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Reverse, i);
        break;
    }
    case 271:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_AutoPlay, i);
        break;
    }
    case 272:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Play, i);
        break;
    }
    case 273:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Stop, i);
        break;
    }
    case 274:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Record, i);
        break;
    }
    case 275:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Rec_1, i);
        break;
    }
    case 276:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Rec_2, i);
        break;
    }
    case 277:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Track_1, i);
        break;
    }
    case 278:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Track_2, i);
        break;
    }
    case 279:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Clear, i);
        break;
    }
    case 280:
        efx_Convol->changepar(Convo_DryWet, Dry_Wet(value));
        break;

    case 281:
        efx_Convol->changepar(Convo_Pan, value);
        break;

    case 282:
        efx_Convol->changepar(Convo_Level, value);
        break;

    case 283:
        efx_Convol->changepar(Convo_Damp, value);
        break;

    case 284:
        efx_Convol->changepar(Convo_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        break;

    case 285:
        efx_Convol->changepar(Convo_Length, 5 + (int) ((float) value * C_MC_245_RANGE));
        break;

    case 286:
        efx_CoilCrafter->changepar(Coil_Gain, value);
        break;

    case 287:
        efx_CoilCrafter->changepar(Coil_Tone, 20 + (int) ((float) value * C_MC_4380_RANGE));
        break;

    case 288:
        efx_CoilCrafter->changepar(Coil_Freq_1, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case 289:
        efx_CoilCrafter->changepar(Coil_Q_1, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case 290:
        efx_CoilCrafter->changepar(Coil_Freq_2, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case 291:
        efx_CoilCrafter->changepar(Coil_Q_2, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case 292:
        efx_ShelfBoost->changepar(Shelf_Gain, value);
        break;

    case 293:
        efx_ShelfBoost->changepar(Shelf_Level, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 294:
        efx_ShelfBoost->changepar(Shelf_Tone, 220 + (int) ((float) value * C_MC_15780_RANGE));
        break;

    case 295:
        efx_ShelfBoost->changepar(Shelf_Presence, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 296:
        efx_Vocoder->changepar(Vocoder_DryWet, Dry_Wet(value));
        break;

    case 297:
        efx_Vocoder->changepar(Vocoder_Pan, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 298:
        efx_Vocoder->changepar(Vocoder_Input, value);
        break;

    case 299:
        efx_Vocoder->changepar(Vocoder_Smear, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 300:
        efx_Vocoder->changepar(Vocoder_Q, 40 + (int) ((float) value * C_MC_130_RANGE));
        break;

    case 301:
        efx_Vocoder->changepar(Vocoder_Ring, value);
        break;

    case 302:
        efx_Vocoder->changepar(Vocoder_Level, value);
        break;

    case 303:
        efx_Echoverse->changepar(Echoverse_DryWet, Dry_Wet(value));
        break;

    case 304:
        efx_Echoverse->changepar(Echoverse_Reverse, value);
        break;

    case 305:
        efx_Echoverse->changepar(Echoverse_Pan, value);
        break;

    case 306:
        efx_Echoverse->changepar(Echoverse_Tempo, ret_Tempo(value));
        break;

    case 307:
        efx_Echoverse->changepar(Echoverse_LR_Delay, value);
        break;

    case 308:
        efx_Echoverse->changepar(Echoverse_Feedback, value);
        break;

    case 309:
        efx_Echoverse->changepar(Echoverse_Damp, value);
        break;

    case 310:
        efx_Echoverse->changepar(Echoverse_Ext_Stereo, value);
        break;

    case 311:
        efx_Echoverse->changepar(Echoverse_Angle, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 312:
        efx_Sustainer->changepar(Sustain_Gain, value);
        break;

    case 313:
        efx_Sustainer->changepar(Sustain_Sustain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 314:
        efx_Sequence->changepar(Sequence_DryWet, Dry_Wet(value));
        break;

    case 315:
        efx_Sequence->changepar(Sequence_Step_1, value);
        break;

    case 316:
        efx_Sequence->changepar(Sequence_Step_2, value);
        break;

    case 317:
        efx_Sequence->changepar(Sequence_Step_3, value);
        break;

    case 318:
        efx_Sequence->changepar(Sequence_Step_4, value);
        break;

    case 319:
        efx_Sequence->changepar(Sequence_Step_5, value);
        break;

    case 320:
        efx_Sequence->changepar(Sequence_Step_6, value);
        break;

    case 321:
        efx_Sequence->changepar(Sequence_Step_7, value);
        break;

    case 322:
        efx_Sequence->changepar(Sequence_Step_8, value);
        break;

    case 323:
        efx_Sequence->changepar(Sequence_Tempo, ret_Tempo(value));
        break;

    case 324:
        efx_Sequence->changepar(Sequence_Resonance, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 325:
        efx_Sequence->changepar(Sequence_Stdf, (int) ((float) value * C_MC_7_RANGE));
        break;

    case 326:
        efx_Shifter->changepar(Shifter_DryWet, Dry_Wet(value));
        break;

    case 327:
        efx_Shifter->changepar(Shifter_Interval, (int) ((float) value * C_MC_12_RANGE));
        break;

    case 328:
        efx_Shifter->changepar(Shifter_Gain, value);
        break;

    case 329:
        efx_Shifter->changepar(Shifter_Pan, value);
        break;

    case 330:
        efx_Shifter->changepar(Shifter_Attack, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case 331:
        efx_Shifter->changepar(Shifter_Decay, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case 332:
        efx_Shifter->changepar(Shifter_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case 333:
        efx_Shifter->changepar(Shifter_Whammy, value);
        break;

    case 334:
        efx_StompBox->changepar(Stomp_Level, value);
        break;

    case 335:
        efx_StompBox->changepar(Stomp_Gain, value);
        break;

    case 336:
        efx_StompBox->changepar(Stomp_Bias, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 337:
        efx_StompBox->changepar(Stomp_Mid, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 338:
        efx_StompBox->changepar(Stomp_Tone, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 339:
        efx_Reverbtron->changepar(Revtron_DryWet, Dry_Wet(value));
        break;

    case 340:
        efx_Reverbtron->changepar(Revtron_Pan, value);
        break;

    case 341:
        efx_Reverbtron->changepar(Revtron_Level, value);
        break;

    case 342:
        efx_Reverbtron->changepar(Revtron_Damp, value);
        break;

    case 343:
        efx_Reverbtron->changepar(Revtron_Feedback, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 344:
        efx_Reverbtron->changepar(Revtron_Length, 20 + (int) ((float) value * C_MC_1480_RANGE));
        break;

    case 345:
        efx_Reverbtron->changepar(Revtron_Stretch, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 346:
        efx_Reverbtron->changepar(Revtron_I_Delay, (int) ((float) value * C_MC_500_RANGE));
        break;

    case 347:
        efx_Reverbtron->changepar(Revtron_Fade, value);
        break;

    case 348:
        efx_Echotron->changepar(Echotron_DryWet, Dry_Wet(value));
        break;

    case 349:
        efx_Echotron->changepar(Echotron_Pan, value);
        break;

    case 350:
        efx_Echotron->changepar(Echotron_Tempo, ret_Tempo(value));
        break;

    case 351:
        efx_Echotron->changepar(Echotron_Damp, value);
        break;

    case 352:
        efx_Echotron->changepar(Echotron_Feedback, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 353:
        efx_Echotron->changepar(Echotron_LR_Cross, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 354:
        efx_Echotron->changepar(Echotron_LFO_Width, value);
        break;

    case 355:
        efx_Echotron->changepar(Echotron_Depth, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 356:
        efx_Echotron->changepar(Echotron_LFO_Stereo, value);
        break;

    case 357:
    {
        /* There is a minor problem with this approach. If the user sets the delay
           file after setting the taps, then the taps will not get adjusted upward. 
           The user will have to move the MIDI control slider to set taps upward.
           The downward limit is checked and limited always on file loading. */
        int number_taps = 1 + (int) ((float) value * C_MC_126_RANGE);
        if(number_taps > efx_Echotron->File.fLength)
        {
            number_taps = efx_Echotron->File.fLength;
        }
        efx_Echotron->changepar(Echotron_Taps, number_taps);
        break;
    }
    case 358:
        efx_StereoHarm->changepar(Sharm_DryWet, Dry_Wet(value));
        break;

    case 359:
        efx_StereoHarm->changepar(Sharm_L_Interval, (int) ((float) value * C_MC_24_RANGE));
        break;

    case 360:
        efx_StereoHarm->changepar(Sharm_L_Chroma, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case 361:
        efx_StereoHarm->changepar(Sharm_L_Gain, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 362:
        efx_StereoHarm->changepar(Sharm_R_Interval, (int) ((float) value * C_MC_24_RANGE));
        break;

    case 363:
        efx_StereoHarm->changepar(Sharm_R_Chroma, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case 364:
        efx_StereoHarm->changepar(Sharm_R_Gain, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 365:
        efx_StereoHarm->changepar(Sharm_LR_Cross, value);
        break;

    case 366:
        efx_StereoHarm->changepar(Sharm_Note, (int) ((float) value * C_MC_23_RANGE));
        break;

    case 367:
        efx_StereoHarm->changepar(Sharm_Chord, (int) ((float) value * C_MC_33_RANGE));
        break;

    case 368:
        efx_CompBand->changepar(CompBand_DryWet, Dry_Wet(value));
        break;

    case 369:
        efx_CompBand->changepar(CompBand_Gain, value);
        break;

    case 370:
        efx_CompBand->changepar(CompBand_Low_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 371:
        efx_CompBand->changepar(CompBand_Mid_1_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 372:
        efx_CompBand->changepar(CompBand_Mid_2_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 373:
        efx_CompBand->changepar(CompBand_High_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 374:
        efx_CompBand->changepar(CompBand_Low_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 375:
        efx_CompBand->changepar(CompBand_Mid_1_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 376:
        efx_CompBand->changepar(CompBand_Mid_2_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 377:
        efx_CompBand->changepar(CompBand_High_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 378:
        efx_CompBand->changepar(CompBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 379:
        efx_CompBand->changepar(CompBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case 380:
        efx_CompBand->changepar(CompBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case 381:
        efx_Opticaltrem->changepar(Optical_Depth, value);
        break;

    case 382:
        efx_Opticaltrem->changepar(Optical_LFO_Tempo, ret_Tempo(value));
        break;

    case 383:
        efx_Opticaltrem->changepar(Optical_LFO_Random, value);
        break;

    case 384:
        efx_Opticaltrem->changepar(Optical_LFO_Stereo, value);
        break;

    case 385:
        efx_Opticaltrem->changepar(Optical_Pan, value);
        break;

    case 386:
        efx_Vibe->changepar(Vibe_DryWet, Dry_Wet(value));
        break;

    case 387:
        efx_Vibe->changepar(Vibe_Width, value);
        break;

    case 388:
        efx_Vibe->changepar(Vibe_Depth, value);
        break;

    case 389:
        efx_Vibe->changepar(Vibe_LFO_Tempo, ret_Tempo(value));
        break;

    case 390:
        efx_Vibe->changepar(Vibe_LFO_Random, value);
        break;

    case 391:
        efx_Vibe->changepar(Vibe_LFO_Stereo, value);
        break;

    case 392:
        efx_Vibe->changepar(Vibe_Feedback, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 393:
        efx_Vibe->changepar(Vibe_LR_Cross, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 394:
        efx_Vibe->changepar(Vibe_Pan, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 395:
        efx_Infinity->changepar(Infinity_DryWet, Dry_Wet(value));
        break;

    case 396:
        efx_Infinity->changepar(Infinity_Resonance, -1000 + (int) ((float) value * C_MC_2000_RANGE));
        break;

    case 397:
        efx_Infinity->changepar(Infinity_AutoPan, value);
        break;

    case 398:
        efx_Infinity->changepar(Infinity_LR_Delay, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 399:
        efx_Infinity->changepar(Infinity_Start, value);
        break;

    case 400:
        efx_Infinity->changepar(Infinity_End, value);
        break;

    case 401:
        efx_Infinity->changepar(Infinity_Tempo, ret_Tempo(value));
        break;

    case 402:
        efx_Infinity->changepar(Infinity_Subdivision, -16 + (int) ((float) value * C_MC_32_RANGE));
        break;

    case 403:
        efx_Alienwah->changepar(Alien_Delay, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 404:
        efx_APhaser->changepar(APhase_LFO_Random, value);
        break;

    case 405:
        efx_Cabinet->changepar(Cabinet_Gain, value);
        break;

    case 406:
        efx_Chorus->changepar(Chorus_Delay, value);
        break;
    
    case 407:
        efx_Echo->changepar(Echo_Reverse, value);
        break;

    case 408:
        efx_Echo->changepar(Echo_Delay, 20 + (int) ((float) value * C_MC_1980_RANGE));
        break;

    case 409:
        efx_Echo->changepar(Echo_LR_Delay, value);
        break;

    case 410:
        efx_Echo->changepar(Echo_Damp, value);
        break;

    case 411:
        efx_Flanger->changepar(Flanger_Delay, value);
        break;
        
    case 412:
        efx_Har->changepar(Harm_Gain, value);
        break;

    case 413:
        efx_Har->changepar(Harm_Filter_Gain, value);
        break;

    case 414:
        efx_Har->changepar(Harm_Filter_Q, value);
        break;

    case 415:
        efx_Infinity->changepar(Infinity_Band_1, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 416:
        efx_Infinity->changepar(Infinity_Band_2, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 417:
        efx_Infinity->changepar(Infinity_Band_3, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 418:
        efx_Infinity->changepar(Infinity_Band_4, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 419:
        efx_Infinity->changepar(Infinity_Band_5, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 420:
        efx_Infinity->changepar(Infinity_Band_6, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 421:
        efx_Infinity->changepar(Infinity_Band_7, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 422:
        efx_Infinity->changepar(Infinity_Band_8, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 423:
        efx_Looper->changepar(Looper_Tempo, 20 + (int) ((float) value * C_MC_360_RANGE));
        break;

    case 424:
        efx_MusDelay->changepar(Music_Tempo, 10 + (int) ((float) value * C_MC_470_RANGE));
        break;

    case 425:
        efx_MusDelay->changepar(Music_Damp, value);
        break;

    case 426:
        efx_MuTroMojo->changepar(MuTro_LFO_Random, value);
        break;

    case 427:
        efx_MuTroMojo->changepar(MuTro_LFO_Stereo, value);
        break;

    case 428:
        efx_MuTroMojo->changepar(MuTro_St_Freq, 30 + (int) ((float) value * C_MC_770_RANGE));
        break;

    case 429:
        efx_Gate->changepar(Gate_Attack, 1 + (int) ((float) value * C_MC_249_RANGE));
        break;

    case 430:
        efx_Gate->changepar(Gate_Release, 2 + (int) ((float) value * C_MC_248_RANGE));
        break;

    case 431:
        efx_Gate->changepar(Gate_Range, -90 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case 432:
        efx_Gate->changepar(Gate_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case 433:
        efx_Gate->changepar(Gate_Hold, 2 + (int) ((float) value * C_MC_498_RANGE));
        break;

    case 434:
        efx_Gate->changepar(Gate_LPF, ret_LPF(value));
        break;

    case 435:
        efx_Gate->changepar(Gate_HPF, ret_HPF(value));
        break;

    case 436:
        efx_Pan->changepar(Pan_Ex_St_Amt, value);
        break;

    case 437:
        efx_Rev->changepar(Reverb_Time, value);
        break;

    case 438:
        efx_Rev->changepar(Reverb_I_Delay, value);
        break;

    case 439:
        efx_Rev->changepar(Reverb_Delay_FB, value);
        break;

    case 440:
        efx_Rev->changepar(Reverb_Room, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 441:
        efx_Rev->changepar(Reverb_Damp, 64 + (int) ((float) value * C_MC_63_RANGE));
        break;

    case 442:
        efx_Reverbtron->changepar(Revtron_Diffusion, value);
        break;

    case 443:
        efx_Reverbtron->changepar(Revtron_LPF, ret_LPF(value));
        break;

    case 444:
        efx_Har->changepar(Harm_Note, (int) ((float) value * C_MC_23_RANGE));
        break;

    case 445:
        efx_Har->changepar(Harm_Chord, (int) ((float) value * C_MC_33_RANGE));
        break;

    case 446:
        efx_Synthfilter->changepar(Synthfilter_LFO_Random, value);
        break;

    case 447:
        efx_Har->changepar(Harm_Select, value);
        break;

    case 448:
        efx_StereoHarm->changepar(Sharm_Select, value);
        break;

    case 449:
        efx_Distorsion->changepar(Dist_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 450:
        efx_Overdrive->changepar(Overdrive_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 451:
        efx_Derelict->changepar(Dere_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 452:
        efx_DistBand->changepar(DistBand_Type_Low, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 453:
        efx_DistBand->changepar(DistBand_Type_Mid, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 454:
        efx_DistBand->changepar(DistBand_Type_Hi, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 455:
        efx_StompBox->changepar(Stomp_Mode, (int) ((float) value * C_MC_8_RANGE));
        break;

    case 456:
        efx_Alienwah->changepar(Alien_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 457:
        efx_APhaser->changepar(APhase_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 458:
        efx_Chorus->changepar(Chorus_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 459:
        efx_Flanger->changepar(Flanger_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 460:
        efx_DFlange->changepar(DFlange_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 461:
        efx_Echotron->changepar(Echotron_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 462:
        efx_MuTroMojo->changepar(MuTro_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 463:
        efx_Opticaltrem->changepar(Optical_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 464:
        efx_Pan->changepar(Pan_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 465:
        efx_Phaser->changepar(Phaser_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 466:
        efx_Synthfilter->changepar(Synthfilter_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 467:
        efx_VaryBand->changepar(VaryBand_LFO_Type_1, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 468:
        efx_VaryBand->changepar(VaryBand_LFO_Type_2, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 469:
        efx_Vibe->changepar(Vibe_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 470:
        efx_WahWah->changepar(WahWah_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
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

