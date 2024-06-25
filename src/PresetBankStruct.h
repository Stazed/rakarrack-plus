/*

  PresetBankStruct.h

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

/* 
 * File:   PresetBankStruct.h
 * Author: sspresto
 *
 * Created on March 22, 2021, 7:29 PM
 */

#ifndef PRESETBANKSTRUCT_H
#define PRESETBANKSTRUCT_H

#include "global.h"

/**
 * The maximum number of effect parameters for the audio class
 */
const int C_NUMBER_PARAMETERS = 19;

/**
 * The number of rack effects - Main window, Orderwindow.
 * saved_order[] : efx_order[] : new_order[];
 */
const int C_NUMBER_ORDERED_EFFECTS = 10;

/**
 * The array sizes for Effect_Params[70][20] and Bank.Effect_Params[70][20]:
 * These sizes are saved to file for bank files. The max effects include 
 * one index for effect order Effect_Params[69][xx]. The max parameters includes the one index for
 * effect bypass Effect_Params[xx][19].
 */
const int C_MAX_EFFECTS = 70;
const int C_MAX_PARAMETERS = C_NUMBER_PARAMETERS + 1;  // 20
const int C_BYPASS = 19;

/**
 * The default settings for rack audio effect.
 */
const int presets_default[C_MAX_EFFECTS][C_MAX_PARAMETERS] = {
    //EQ1
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //Compressor
    {-30, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Distortion
    {0, 64, 0, 87, 14, 6, 0, 3134, 157, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Overdrive
    {84, 64, 35, 56, 40, 0, 0, 6703, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Echo
    {67, 64, 565, 64, 30, 59, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Chorus
    {64, 64, 33, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Phaser
    {64, 64, 11, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0},
    //Flanger
    {64, 64, 39, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Reverb
    {80, 64, 63, 24, 0, 0, 0, 4002, 27, 83, 1, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //EQ2 Parametric
    {200, 64, 64, 800, 64, 64, 12000, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //WahWah
    {64, 64, 80, 0, 0, 64, 70, 90, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //AlienWah
    {64, 64, 80, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Cabinet
    {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Pan
    {64, 64, 26, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Harmonizer
    {64, 64, 64, 12, 6000, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //MusicDelay
    {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0, 0, 0, 0, 0},
    //NoiseGate
    {0, 0, 1, 2, 6703, 76, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Derelict
    {0, 64, 64, 83, 65, 15, 0, 2437, 169, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //APhaser
    {64, 20, 14, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0, 0, 0, 0, 0},
    //Valve
    {0, 64, 64, 127, 64, 0, 5841, 61, 1, 0, 69, 1, 84, 0, 0, 0, 0, 0, 0, 0},
    //Dual Flange
    {32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0, 0, 0, 0, 0, 0},
    //Ring
    {0, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0, 0, 0, 0, 0, 0},
    //Exciter
    {127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20, 0, 0, 0, 0, 0, 0, 0},
    //DistBand
    {0, 64, 64, 56, 40, 0, 0, 0, 29, 35, 100, 0, 450, 1500, 1, 0, 0, 0, 0, 0},
    //Arpie
    {0, 64, 126, 64, 30, 59, 0, 127, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Expander
    {-50, 20, 50, 50, 3134, 76, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Shuffle
    {64, 10, 0, 0, 0, 600, 1200, 2000, 6000, -14, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Synthfilter
    {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0, 0, 0, 0, 0},
    //VaryBand
    {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Convolotron
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Looper
    {64, 0, 1, 0, 1, 0, 64, 1, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //MuTroMojo
    {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450, 1, 1, 0, 0},
    //Echoverse
    {64, 64, 90, 64, 64, 64, 64, 0, 1, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //CoilCrafter
    {32, 6, 1, 3300, 16, 4400, 42, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //ShelfBoost
    {127, 64, 16000, 1, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Vocoder
    {0, 64, 10, 70, 70, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Systainer
    {40, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Sequence
    {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3, 0, 0, 0, 0, 0},
    //Shifter
    {0, 64, 64, 200, 200, -20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //StompBox
    {48, 32, 0, 32, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Reverbtron
    {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64, 0, 0, 20000, 0, 0, 0, 0, 0},
    //Echotron
    {64, 45, 34, 4, 0, 76, 3, 41, 0, 96, -13, 64, 1, 1, 1, 1, 0, 0, 0, 0},
    //StereoHarm
    {64, 64, 12, 0, 64, 12, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //CompBand
    {0, 4, 4, 4, 4, 0, 0, 0, 0, 200, 2000, 15000, 88, 0, 0, 0, 0, 0, 0, 0},
    //Opticaltrem
    {127, 260, 10, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Vibe
    {35, 120, 10, 0, 64, 64, 64, 64, 3, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Infinity
    {64, 64, -64, 64, -64, 64, -64, 64, -64, 0, 0, 0, 14, 0, 0, 0, 0, 4, 0, 0},
    //ResSolution
    {64, 0, 268, 0, 12, 64, 76, 64, 4, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0},      // 47
    // Unused
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},       // 50
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},       // 60
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},       // 68

    // Order - only 0 through 9 are currently used
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}        // 69
};




struct PresetBankStruct
{
public:
    PresetBankStruct();

    void new_preset();
    
    // The order and size of these variables cannot be changed
    // since they are used for file saving.
    char Preset_Name[64];
    char Author[64];
    char Classe[36];            // Not used
    char Type[4];               // Not used
    char ConvoFiname[128];
    char cInput_Gain[64];
    char cMaster_Volume[64];
    char cBalance[64];
    float Input_Gain;
    float Master_Volume;
    float Fraction_Bypass;      // Balance
    int FX_Master_Active;       // Active
    char RevFiname[128];
    char EchoFiname[128];
    
    int Effect_Params[C_MAX_EFFECTS][C_MAX_PARAMETERS];

    /**
     * Array for holding the user MIDI learn settings. CC from 0 to 127, each CC can
     * be mapped to 20 effects.
     */
    int XUserMIDI[128][20];

    int XMIDIrangeMin[128];     // Not used
    int XMIDIrangeMax[128];     // Not used

};

#endif /* PRESETBANKSTRUCT_H */

