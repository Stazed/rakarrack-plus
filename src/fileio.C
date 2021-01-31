/*  rakarrack - Audio effects software

  fileio.C  -  File Input/Output functions
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

#include <errno.h>
#include "process.h"
#include <FL/fl_ask.H> // for error pop up


const int presets_default[C_NUMBER_EFFECTS][C_NUMBER_PARAMETERS] = {
    //EQ1
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0},
    //Compressor
    {-30, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Distortion
    {0, 64, 0, 87, 14, 6, 0, 3134, 157, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //Overdrive
    {84, 64, 35, 56, 40, 0, 0, 6703, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Echo
    {67, 64, 565, 64, 30, 59, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Chorus
    {64, 64, 33, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Phaser
    {64, 64, 11, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0},
    //Flanger
    {64, 64, 39, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Reverb
    {80, 64, 63, 24, 0, 0, 0, 4002, 27, 83, 1, 64, 0, 0, 0, 0, 0, 0, 0},
    //EQ2 Parametric
    {200, 64, 64, 800, 64, 64, 12000, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //WahWah
    {64, 64, 80, 0, 0, 64, 70, 90, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //AlienWah
    {64, 64, 80, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //Cabinet
    {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Pan
    {64, 64, 26, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Harmonizer
    {64, 64, 64, 12, 6000, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //MusicDelay
    {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0, 0, 0, 0},
    //NoiseGate
    {0, 0, 1, 2, 6703, 76, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Derelict
    {0, 64, 64, 83, 65, 15, 0, 2437, 169, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //APhaser
    {64, 20, 14, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0, 0, 0, 0},
    //Valve
    {0, 64, 64, 127, 64, 0, 5841, 61, 1, 0, 69, 1, 84, 0, 0, 0, 0, 0, 0},
    //Dual Flange
    {32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0, 0, 0, 0, 0},
    //Ring
    {0, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0, 0, 0, 0, 0},
    //Exciter
    {127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20, 0, 0, 0, 0, 0, 0},
    //DistBand
    {0, 64, 64, 56, 40, 0, 0, 0, 29, 35, 100, 0, 450, 1500, 1, 0, 0, 0, 0},
    //Arpie
    {0, 64, 126, 64, 30, 59, 0, 127, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0},
    //Expander
    {-50, 20, 50, 50, 3134, 76, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Shuffle
    {64, 10, 0, 0, 0, 600, 1200, 2000, 6000, -14, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //Synthfilter
    {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0, 0, 0, 0},
    //VaryBand
    {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Convolotron
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Looper
    {64, 0, 1, 0, 1, 0, 64, 1, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //MuTroMojo
    {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450, 1, 1, 0},
    //Echoverse
    {64, 64, 90, 64, 64, 64, 64, 0, 1, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //CoilCrafter
    {32, 6, 1, 3300, 16, 4400, 42, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //ShelfBoost
    {127, 64, 16000, 1, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Vocoder
    {0, 64, 10, 70, 70, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Systainer
    {40, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Sequence
    {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3, 0, 0, 0, 0},
    //Shifter
    {0, 64, 64, 200, 200, -20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //StompBox
    {48, 32, 0, 32, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Reverbtron
    {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64, 0, 0, 20000, 0, 0, 0, 0},
    //Echotron
    {64, 45, 34, 4, 0, 76, 3, 41, 0, 96, -13, 64, 1, 1, 1, 1, 0, 0, 0},
    //StereoHarm
    {64, 64, 12, 0, 64, 12, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0},
    //CompBand
    {0, 4, 4, 4, 4, 0, 0, 0, 0, 200, 2000, 15000, 88, 0, 0, 0, 0, 0, 0},
    //Opticaltrem
    {127, 260, 10, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Vibe
    {35, 120, 10, 0, 64, 64, 64, 64, 3, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Infinity
    {64, 64, -64, 64, -64, 64, -64, 64, -64, 0, 0, 0, 14, 0, 0, 0, 0, 4, 0}
};

/**
 *  Individual preset file loading.
 * 
 * @param buf
 *  Buffer to load effect parameters.
 * 
 * @param j
 *  The effect number to load.
 */
void RKR::putbuf(char *buf, int j)
{
    char *cfilename;
    cfilename = (char *) malloc(sizeof (char) * 128);

    switch (j)
    {

    case EFX_REVERB:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_REVERB][0], &lv[EFX_REVERB][1], &lv[EFX_REVERB][2], &lv[EFX_REVERB][3], &lv[EFX_REVERB][4],
               &lv[EFX_REVERB][5], &lv[EFX_REVERB][6], &lv[EFX_REVERB][7], &lv[EFX_REVERB][8], &lv[EFX_REVERB][9],
               &lv[EFX_REVERB][10], &lv[EFX_REVERB][11], &EFX_Bank_Bypass[EFX_REVERB]);
        break;

    case EFX_ECHO:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_ECHO][0], &lv[EFX_ECHO][1], &lv[EFX_ECHO][2], &lv[EFX_ECHO][3], &lv[EFX_ECHO][4],
               &lv[EFX_ECHO][5], &lv[EFX_ECHO][6], &lv[EFX_ECHO][7], &lv[EFX_ECHO][8], &EFX_Bank_Bypass[EFX_ECHO]);
        break;

    case EFX_CHORUS:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_CHORUS][0], &lv[EFX_CHORUS][1], &lv[EFX_CHORUS][2], &lv[EFX_CHORUS][3], &lv[EFX_CHORUS][4],
               &lv[EFX_CHORUS][5], &lv[EFX_CHORUS][6], &lv[EFX_CHORUS][7], &lv[EFX_CHORUS][8], &lv[EFX_CHORUS][9],
               &lv[EFX_CHORUS][10], &lv[EFX_CHORUS][11], &lv[EFX_CHORUS][12], &EFX_Bank_Bypass[EFX_CHORUS]);
        break;

    case EFX_FLANGER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_FLANGER][0], &lv[EFX_FLANGER][1], &lv[EFX_FLANGER][2], &lv[EFX_FLANGER][3], &lv[EFX_FLANGER][4],
               &lv[EFX_FLANGER][5], &lv[EFX_FLANGER][6], &lv[EFX_FLANGER][7], &lv[EFX_FLANGER][8], &lv[EFX_FLANGER][9],
               &lv[EFX_FLANGER][10], &lv[EFX_FLANGER][11], &lv[EFX_FLANGER][12], &EFX_Bank_Bypass[EFX_FLANGER]);
        break;

    case EFX_PHASER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_PHASER][0], &lv[EFX_PHASER][1], &lv[EFX_PHASER][2], &lv[EFX_PHASER][3], &lv[EFX_PHASER][4],
               &lv[EFX_PHASER][5], &lv[EFX_PHASER][6], &lv[EFX_PHASER][7], &lv[EFX_PHASER][8], &lv[EFX_PHASER][9],
               &lv[EFX_PHASER][10], &lv[EFX_PHASER][11], &EFX_Bank_Bypass[EFX_PHASER]);
        break;

    case EFX_OVERDRIVE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_OVERDRIVE][0], &lv[EFX_OVERDRIVE][1], &lv[EFX_OVERDRIVE][2], &lv[EFX_OVERDRIVE][3], &lv[EFX_OVERDRIVE][4],
               &lv[EFX_OVERDRIVE][5], &lv[EFX_OVERDRIVE][6], &lv[EFX_OVERDRIVE][7], &lv[EFX_OVERDRIVE][8], &lv[EFX_OVERDRIVE][9],
               &lv[EFX_OVERDRIVE][10], &lv[EFX_OVERDRIVE][11], &lv[EFX_OVERDRIVE][12], &EFX_Bank_Bypass[EFX_OVERDRIVE]);
        break;

    case EFX_DISTORTION:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_DISTORTION][0], &lv[EFX_DISTORTION][1], &lv[EFX_DISTORTION][2], &lv[EFX_DISTORTION][3], &lv[EFX_DISTORTION][4],
               &lv[EFX_DISTORTION][5], &lv[EFX_DISTORTION][6], &lv[EFX_DISTORTION][7], &lv[EFX_DISTORTION][8], &lv[EFX_DISTORTION][9],
               &lv[EFX_DISTORTION][10], &lv[EFX_DISTORTION][11], &lv[EFX_DISTORTION][12], &EFX_Bank_Bypass[EFX_DISTORTION]);
        break;

    case EFX_EQ:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_EQ][0], &lv[EFX_EQ][1], &lv[EFX_EQ][2], &lv[EFX_EQ][3], &lv[EFX_EQ][4],
               &lv[EFX_EQ][5], &lv[EFX_EQ][6], &lv[EFX_EQ][7], &lv[EFX_EQ][8], &lv[EFX_EQ][9],
               &lv[EFX_EQ][10], &lv[EFX_EQ][11], &EFX_Bank_Bypass[EFX_EQ]);
        break;

    case EFX_PARAMETRIC:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_PARAMETRIC][0], &lv[EFX_PARAMETRIC][1], &lv[EFX_PARAMETRIC][2], &lv[EFX_PARAMETRIC][3], &lv[EFX_PARAMETRIC][4],
               &lv[EFX_PARAMETRIC][5], &lv[EFX_PARAMETRIC][6], &lv[EFX_PARAMETRIC][7], &lv[EFX_PARAMETRIC][8], &lv[EFX_PARAMETRIC][9],
               &EFX_Bank_Bypass[EFX_PARAMETRIC]);
        break;

    case EFX_COMPRESSOR:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_COMPRESSOR][0], &lv[EFX_COMPRESSOR][1], &lv[EFX_COMPRESSOR][2], &lv[EFX_COMPRESSOR][3], &lv[EFX_COMPRESSOR][4],
               &lv[EFX_COMPRESSOR][5], &lv[EFX_COMPRESSOR][6], &lv[EFX_COMPRESSOR][7], &lv[EFX_COMPRESSOR][8], &EFX_Bank_Bypass[EFX_COMPRESSOR]);
        break;

    case EFX_WAHWAH:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_WAHWAH][0], &lv[EFX_WAHWAH][1], &lv[EFX_WAHWAH][2], &lv[EFX_WAHWAH][3], &lv[EFX_WAHWAH][4],
               &lv[EFX_WAHWAH][5], &lv[EFX_WAHWAH][6], &lv[EFX_WAHWAH][7], &lv[EFX_WAHWAH][8], &lv[EFX_WAHWAH][9],
               &lv[EFX_WAHWAH][10], &EFX_Bank_Bypass[EFX_WAHWAH]);
        break;

    case EFX_ALIENWAH:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_ALIENWAH][0], &lv[EFX_ALIENWAH][1], &lv[EFX_ALIENWAH][2], &lv[EFX_ALIENWAH][3], &lv[EFX_ALIENWAH][4],
               &lv[EFX_ALIENWAH][5], &lv[EFX_ALIENWAH][6], &lv[EFX_ALIENWAH][7], &lv[EFX_ALIENWAH][8], &lv[EFX_ALIENWAH][9],
               &lv[EFX_ALIENWAH][10], &EFX_Bank_Bypass[EFX_ALIENWAH]);
        break;

    case EFX_CABINET:
        sscanf(buf, "%d,%d,%d\n", &lv[EFX_CABINET][0], &lv[EFX_CABINET][1], &EFX_Bank_Bypass[EFX_CABINET]);
        break;

    case EFX_PAN:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_PAN][0], &lv[EFX_PAN][1], &lv[EFX_PAN][2], &lv[EFX_PAN][3], &lv[EFX_PAN][4],
               &lv[EFX_PAN][5], &lv[EFX_PAN][6], &lv[EFX_PAN][7], &lv[EFX_PAN][8], &EFX_Bank_Bypass[EFX_PAN]);
        break;

    case EFX_HARMONIZER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_HARMONIZER][0], &lv[EFX_HARMONIZER][1], &lv[EFX_HARMONIZER][2], &lv[EFX_HARMONIZER][3], &lv[EFX_HARMONIZER][4],
               &lv[EFX_HARMONIZER][5], &lv[EFX_HARMONIZER][6], &lv[EFX_HARMONIZER][7], &lv[EFX_HARMONIZER][8], &lv[EFX_HARMONIZER][9],
               &lv[EFX_HARMONIZER][10], &EFX_Bank_Bypass[EFX_HARMONIZER]);
        break;

    case EFX_MUSICAL_DELAY:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_MUSICAL_DELAY][0], &lv[EFX_MUSICAL_DELAY][1], &lv[EFX_MUSICAL_DELAY][2], &lv[EFX_MUSICAL_DELAY][3], &lv[EFX_MUSICAL_DELAY][4],
               &lv[EFX_MUSICAL_DELAY][5], &lv[EFX_MUSICAL_DELAY][6], &lv[EFX_MUSICAL_DELAY][7], &lv[EFX_MUSICAL_DELAY][8], &lv[EFX_MUSICAL_DELAY][9],
               &lv[EFX_MUSICAL_DELAY][10], &lv[EFX_MUSICAL_DELAY][11], &lv[EFX_MUSICAL_DELAY][12], &EFX_Bank_Bypass[EFX_MUSICAL_DELAY]);
        break;

    case EFX_NOISEGATE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_NOISEGATE][0], &lv[EFX_NOISEGATE][1], &lv[EFX_NOISEGATE][2], &lv[EFX_NOISEGATE][3], &lv[EFX_NOISEGATE][4],
               &lv[EFX_NOISEGATE][5], &lv[EFX_NOISEGATE][6], &EFX_Bank_Bypass[EFX_NOISEGATE]);

        break;

    case EFX_DERELICT:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_DERELICT][0], &lv[EFX_DERELICT][1], &lv[EFX_DERELICT][2], &lv[EFX_DERELICT][3], &lv[EFX_DERELICT][4],
               &lv[EFX_DERELICT][5], &lv[EFX_DERELICT][6], &lv[EFX_DERELICT][7], &lv[EFX_DERELICT][8], &lv[EFX_DERELICT][9],
               &lv[EFX_DERELICT][10], &lv[EFX_DERELICT][11], &EFX_Bank_Bypass[EFX_DERELICT]);
        break;

    case EFX_ANALOG_PHASER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_ANALOG_PHASER][0], &lv[EFX_ANALOG_PHASER][1], &lv[EFX_ANALOG_PHASER][2], &lv[EFX_ANALOG_PHASER][3], &lv[EFX_ANALOG_PHASER][4],
               &lv[EFX_ANALOG_PHASER][5], &lv[EFX_ANALOG_PHASER][6], &lv[EFX_ANALOG_PHASER][7], &lv[EFX_ANALOG_PHASER][8], &lv[EFX_ANALOG_PHASER][9],
               &lv[EFX_ANALOG_PHASER][10], &lv[EFX_ANALOG_PHASER][11], &EFX_Bank_Bypass[EFX_ANALOG_PHASER]);
        break;

    case EFX_VALVE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_VALVE][0], &lv[EFX_VALVE][1], &lv[EFX_VALVE][2], &lv[EFX_VALVE][3], &lv[EFX_VALVE][4],
               &lv[EFX_VALVE][5], &lv[EFX_VALVE][6], &lv[EFX_VALVE][7], &lv[EFX_VALVE][8], &lv[EFX_VALVE][9],
               &lv[EFX_VALVE][10], &lv[EFX_VALVE][11], &lv[EFX_VALVE][12], &EFX_Bank_Bypass[EFX_VALVE]);
        break;

    case EFX_DUAL_FLANGE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_DUAL_FLANGE][0], &lv[EFX_DUAL_FLANGE][1], &lv[EFX_DUAL_FLANGE][2], &lv[EFX_DUAL_FLANGE][3], &lv[EFX_DUAL_FLANGE][4],
               &lv[EFX_DUAL_FLANGE][5], &lv[EFX_DUAL_FLANGE][6], &lv[EFX_DUAL_FLANGE][7], &lv[EFX_DUAL_FLANGE][8], &lv[EFX_DUAL_FLANGE][9],
               &lv[EFX_DUAL_FLANGE][10], &lv[EFX_DUAL_FLANGE][11], &lv[EFX_DUAL_FLANGE][12], &lv[EFX_DUAL_FLANGE][13], &lv[EFX_DUAL_FLANGE][14],
               &EFX_Bank_Bypass[EFX_DUAL_FLANGE]);
        break;

    case EFX_RING:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_RING][0], &lv[EFX_RING][1], &lv[EFX_RING][2], &lv[EFX_RING][3], &lv[EFX_RING][4],
               &lv[EFX_RING][5], &lv[EFX_RING][6], &lv[EFX_RING][7], &lv[EFX_RING][8], &lv[EFX_RING][9],
               &lv[EFX_RING][10], &lv[EFX_RING][11], &lv[EFX_RING][12], &EFX_Bank_Bypass[EFX_RING]);
        break;

    case EFX_EXCITER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_EXCITER][0], &lv[EFX_EXCITER][1], &lv[EFX_EXCITER][2], &lv[EFX_EXCITER][3], &lv[EFX_EXCITER][4],
               &lv[EFX_EXCITER][5], &lv[EFX_EXCITER][6], &lv[EFX_EXCITER][7], &lv[EFX_EXCITER][8], &lv[EFX_EXCITER][9],
               &lv[EFX_EXCITER][10], &lv[EFX_EXCITER][11], &lv[EFX_EXCITER][12], &EFX_Bank_Bypass[EFX_EXCITER]);
        break;

    case EFX_DISTBAND:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_DISTBAND][0], &lv[EFX_DISTBAND][1], &lv[EFX_DISTBAND][2], &lv[EFX_DISTBAND][3], &lv[EFX_DISTBAND][4],
               &lv[EFX_DISTBAND][5], &lv[EFX_DISTBAND][6], &lv[EFX_DISTBAND][7], &lv[EFX_DISTBAND][8], &lv[EFX_DISTBAND][9],
               &lv[EFX_DISTBAND][10], &lv[EFX_DISTBAND][11], &lv[EFX_DISTBAND][12], &lv[EFX_DISTBAND][13], &lv[EFX_DISTBAND][14],
               &EFX_Bank_Bypass[EFX_DISTBAND]);
        break;

    case EFX_ARPIE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_ARPIE][0], &lv[EFX_ARPIE][1], &lv[EFX_ARPIE][2], &lv[EFX_ARPIE][3], &lv[EFX_ARPIE][4],
               &lv[EFX_ARPIE][5], &lv[EFX_ARPIE][6], &lv[EFX_ARPIE][7], &lv[EFX_ARPIE][8], &lv[EFX_ARPIE][9],
               &lv[EFX_ARPIE][10], &EFX_Bank_Bypass[EFX_ARPIE]);
        break;

    case EFX_EXPANDER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_EXPANDER][0], &lv[EFX_EXPANDER][1], &lv[EFX_EXPANDER][2], &lv[EFX_EXPANDER][3], &lv[EFX_EXPANDER][4],
               &lv[EFX_EXPANDER][5], &lv[EFX_EXPANDER][6], &EFX_Bank_Bypass[EFX_EXPANDER]);

        break;

    case EFX_SHUFFLE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_SHUFFLE][0], &lv[EFX_SHUFFLE][1], &lv[EFX_SHUFFLE][2], &lv[EFX_SHUFFLE][3], &lv[EFX_SHUFFLE][4],
               &lv[EFX_SHUFFLE][5], &lv[EFX_SHUFFLE][6], &lv[EFX_SHUFFLE][7], &lv[EFX_SHUFFLE][8], &lv[EFX_SHUFFLE][9],
               &lv[EFX_SHUFFLE][10], &EFX_Bank_Bypass[EFX_SHUFFLE]);
        break;

    case EFX_SYNTHFILTER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_SYNTHFILTER][0], &lv[EFX_SYNTHFILTER][1], &lv[EFX_SYNTHFILTER][2], &lv[EFX_SYNTHFILTER][3], &lv[EFX_SYNTHFILTER][4],
               &lv[EFX_SYNTHFILTER][5], &lv[EFX_SYNTHFILTER][6], &lv[EFX_SYNTHFILTER][7], &lv[EFX_SYNTHFILTER][8], &lv[EFX_SYNTHFILTER][9],
               &lv[EFX_SYNTHFILTER][10], &lv[EFX_SYNTHFILTER][11], &lv[EFX_SYNTHFILTER][12], &lv[EFX_SYNTHFILTER][13], &lv[EFX_SYNTHFILTER][14],
               &lv[EFX_SYNTHFILTER][15], &EFX_Bank_Bypass[EFX_SYNTHFILTER]);
        break;

    case EFX_VARYBAND:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_VARYBAND][0], &lv[EFX_VARYBAND][1], &lv[EFX_VARYBAND][2], &lv[EFX_VARYBAND][3], &lv[EFX_VARYBAND][4],
               &lv[EFX_VARYBAND][5], &lv[EFX_VARYBAND][6], &lv[EFX_VARYBAND][7], &lv[EFX_VARYBAND][8], &lv[EFX_VARYBAND][9],
               &lv[EFX_VARYBAND][10], &EFX_Bank_Bypass[EFX_VARYBAND]);
        break;

    case EFX_CONVOLOTRON:
    {
        Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
        memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));
        memset(cfilename, 0, sizeof (char) * 128);
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[EFX_CONVOLOTRON][0], &lv[EFX_CONVOLOTRON][1], &lv[EFX_CONVOLOTRON][2], &lv[EFX_CONVOLOTRON][3], &lv[EFX_CONVOLOTRON][4],
               &lv[EFX_CONVOLOTRON][5], &lv[EFX_CONVOLOTRON][6], &lv[EFX_CONVOLOTRON][7], &lv[EFX_CONVOLOTRON][8], &lv[EFX_CONVOLOTRON][9],
               &lv[EFX_CONVOLOTRON][10], &EFX_Bank_Bypass[EFX_CONVOLOTRON], cfilename);
        strcpy(Efx_Convolotron->Filename, cfilename);
        break;
    }
    case EFX_LOOPER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_LOOPER][0], &lv[EFX_LOOPER][1], &lv[EFX_LOOPER][2], &lv[EFX_LOOPER][3], &lv[EFX_LOOPER][4],
               &lv[EFX_LOOPER][5], &lv[EFX_LOOPER][6], &lv[EFX_LOOPER][7], &lv[EFX_LOOPER][8], &lv[EFX_LOOPER][9],
               &lv[EFX_LOOPER][10], &lv[EFX_LOOPER][11], &lv[EFX_LOOPER][12], &lv[EFX_LOOPER][13], &EFX_Bank_Bypass[EFX_LOOPER]);
        break;

    case EFX_MUTROMOJO:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_MUTROMOJO][0], &lv[EFX_MUTROMOJO][1], &lv[EFX_MUTROMOJO][2], &lv[EFX_MUTROMOJO][3], &lv[EFX_MUTROMOJO][4],
               &lv[EFX_MUTROMOJO][5], &lv[EFX_MUTROMOJO][6], &lv[EFX_MUTROMOJO][7], &lv[EFX_MUTROMOJO][8], &lv[EFX_MUTROMOJO][9],
               &lv[EFX_MUTROMOJO][10], &lv[EFX_MUTROMOJO][11], &lv[EFX_MUTROMOJO][12], &lv[EFX_MUTROMOJO][13], &lv[EFX_MUTROMOJO][14],
               &lv[EFX_MUTROMOJO][15], &lv[EFX_MUTROMOJO][16], &lv[EFX_MUTROMOJO][17], &EFX_Bank_Bypass[EFX_MUTROMOJO]);
        break;

    case EFX_ECHOVERSE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_ECHOVERSE][0], &lv[EFX_ECHOVERSE][1], &lv[EFX_ECHOVERSE][2], &lv[EFX_ECHOVERSE][3], &lv[EFX_ECHOVERSE][4],
               &lv[EFX_ECHOVERSE][5], &lv[EFX_ECHOVERSE][6], &lv[EFX_ECHOVERSE][7], &lv[EFX_ECHOVERSE][8], &lv[EFX_ECHOVERSE][9], &EFX_Bank_Bypass[EFX_ECHOVERSE]);
        break;

    case EFX_COILCRAFTER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_COILCRAFTER][0], &lv[EFX_COILCRAFTER][1], &lv[EFX_COILCRAFTER][2], &lv[EFX_COILCRAFTER][3], &lv[EFX_COILCRAFTER][4],
               &lv[EFX_COILCRAFTER][5], &lv[EFX_COILCRAFTER][6], &lv[EFX_COILCRAFTER][7], &lv[EFX_COILCRAFTER][8], &EFX_Bank_Bypass[EFX_COILCRAFTER]);
        break;

    case EFX_SHELFBOOST:
        sscanf(buf, "%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_SHELFBOOST][0], &lv[EFX_SHELFBOOST][1], &lv[EFX_SHELFBOOST][2], &lv[EFX_SHELFBOOST][3], &lv[EFX_SHELFBOOST][4],
               &EFX_Bank_Bypass[EFX_SHELFBOOST]);
        break;

    case EFX_VOCODER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_VOCODER][0], &lv[EFX_VOCODER][1], &lv[EFX_VOCODER][2], &lv[EFX_VOCODER][3], &lv[EFX_VOCODER][4],
               &lv[EFX_VOCODER][5], &lv[EFX_VOCODER][6], &EFX_Bank_Bypass[EFX_VOCODER]);
        break;

    case EFX_SUSTAINER:
        sscanf(buf, "%d,%d,%d\n",
               &lv[EFX_SUSTAINER][0], &lv[EFX_SUSTAINER][1], &EFX_Bank_Bypass[EFX_SUSTAINER]);
        break;

    case EFX_SEQUENCE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_SEQUENCE][0], &lv[EFX_SEQUENCE][1], &lv[EFX_SEQUENCE][2], &lv[EFX_SEQUENCE][3], &lv[EFX_SEQUENCE][4],
               &lv[EFX_SEQUENCE][5], &lv[EFX_SEQUENCE][6], &lv[EFX_SEQUENCE][7], &lv[EFX_SEQUENCE][8], &lv[EFX_SEQUENCE][9],
               &lv[EFX_SEQUENCE][10], &lv[EFX_SEQUENCE][11], &lv[EFX_SEQUENCE][12], &lv[EFX_SEQUENCE][13], &lv[EFX_SEQUENCE][14], &EFX_Bank_Bypass[EFX_SEQUENCE]);
        break;

    case EFX_SHIFTER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_SHIFTER][0], &lv[EFX_SHIFTER][1], &lv[EFX_SHIFTER][2], &lv[EFX_SHIFTER][3], &lv[EFX_SHIFTER][4],
               &lv[EFX_SHIFTER][5], &lv[EFX_SHIFTER][6], &lv[EFX_SHIFTER][7], &lv[EFX_SHIFTER][8], &lv[EFX_SHIFTER][9], &EFX_Bank_Bypass[EFX_SHIFTER]);
        break;

    case EFX_STOMPBOX:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_STOMPBOX][0], &lv[EFX_STOMPBOX][1], &lv[EFX_STOMPBOX][2], &lv[EFX_STOMPBOX][3], &lv[EFX_STOMPBOX][4],
               &lv[EFX_STOMPBOX][5], &EFX_Bank_Bypass[EFX_STOMPBOX]);
        break;

    case EFX_REVERBTRON:
    {
        Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
        memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));
        memset(cfilename, 0, sizeof (char) * 128);
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[EFX_REVERBTRON][0], &lv[EFX_REVERBTRON][1], &lv[EFX_REVERBTRON][2], &lv[EFX_REVERBTRON][3], &lv[EFX_REVERBTRON][4],
               &lv[EFX_REVERBTRON][5], &lv[EFX_REVERBTRON][6], &lv[EFX_REVERBTRON][7], &lv[EFX_REVERBTRON][8], &lv[EFX_REVERBTRON][9],
               &lv[EFX_REVERBTRON][10], &lv[EFX_REVERBTRON][11], &lv[EFX_REVERBTRON][12], &lv[EFX_REVERBTRON][13], &lv[EFX_REVERBTRON][14], &lv[EFX_REVERBTRON][15],
               &EFX_Bank_Bypass[EFX_REVERBTRON],
               cfilename);
        strcpy(Efx_Reverbtron->Filename, cfilename);
        break;
    }
    case EFX_ECHOTRON:
    {
        Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
        memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));
        memset(cfilename, 0, sizeof (char) * 128);
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[EFX_ECHOTRON][0], &lv[EFX_ECHOTRON][1], &lv[EFX_ECHOTRON][2], &lv[EFX_ECHOTRON][3], &lv[EFX_ECHOTRON][4],
               &lv[EFX_ECHOTRON][5], &lv[EFX_ECHOTRON][6], &lv[EFX_ECHOTRON][7], &lv[EFX_ECHOTRON][8], &lv[EFX_ECHOTRON][9],
               &lv[EFX_ECHOTRON][10], &lv[EFX_ECHOTRON][11], &lv[EFX_ECHOTRON][12], &lv[EFX_ECHOTRON][13], &lv[EFX_ECHOTRON][14], &lv[EFX_ECHOTRON][15],
               &EFX_Bank_Bypass[EFX_ECHOTRON],
               cfilename);
        strcpy(Efx_Echotron->Filename, cfilename);
        break;
    }
    case EFX_STEREOHARM:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_STEREOHARM][0], &lv[EFX_STEREOHARM][1], &lv[EFX_STEREOHARM][2], &lv[EFX_STEREOHARM][3], &lv[EFX_STEREOHARM][4],
               &lv[EFX_STEREOHARM][5], &lv[EFX_STEREOHARM][6], &lv[EFX_STEREOHARM][7], &lv[EFX_STEREOHARM][8], &lv[EFX_STEREOHARM][9],
               &lv[EFX_STEREOHARM][10], &lv[EFX_STEREOHARM][11], &EFX_Bank_Bypass[EFX_STEREOHARM]);
        break;

    case EFX_COMPBAND:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_COMPBAND][0], &lv[EFX_COMPBAND][1], &lv[EFX_COMPBAND][2], &lv[EFX_COMPBAND][3], &lv[EFX_COMPBAND][4],
               &lv[EFX_COMPBAND][5], &lv[EFX_COMPBAND][6], &lv[EFX_COMPBAND][7], &lv[EFX_COMPBAND][8], &lv[EFX_COMPBAND][9],
               &lv[EFX_COMPBAND][10], &lv[EFX_COMPBAND][11], &lv[EFX_COMPBAND][12], &EFX_Bank_Bypass[EFX_COMPBAND]);
        break;

    case EFX_OPTICALTREM:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_OPTICALTREM][0], &lv[EFX_OPTICALTREM][1], &lv[EFX_OPTICALTREM][2], &lv[EFX_OPTICALTREM][3], &lv[EFX_OPTICALTREM][4],
               &lv[EFX_OPTICALTREM][5], &EFX_Bank_Bypass[EFX_OPTICALTREM]);
        break;

    case EFX_VIBE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_VIBE][0], &lv[EFX_VIBE][1], &lv[EFX_VIBE][2], &lv[EFX_VIBE][3], &lv[EFX_VIBE][4],
               &lv[EFX_VIBE][5], &lv[EFX_VIBE][6], &lv[EFX_VIBE][7], &lv[EFX_VIBE][8], &lv[EFX_VIBE][9], &lv[EFX_VIBE][10],
               &EFX_Bank_Bypass[EFX_VIBE]);
        break;

    case EFX_INFINITY:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[EFX_INFINITY][0], &lv[EFX_INFINITY][1], &lv[EFX_INFINITY][2], &lv[EFX_INFINITY][3], &lv[EFX_INFINITY][4],
               &lv[EFX_INFINITY][5], &lv[EFX_INFINITY][6], &lv[EFX_INFINITY][7], &lv[EFX_INFINITY][8], &lv[EFX_INFINITY][9],
               &lv[EFX_INFINITY][10], &lv[EFX_INFINITY][11], &lv[EFX_INFINITY][12], &lv[EFX_INFINITY][13], &lv[EFX_INFINITY][14],
               &lv[EFX_INFINITY][15], &lv[EFX_INFINITY][16], &lv[EFX_INFINITY][17], &EFX_Bank_Bypass[EFX_INFINITY]);
        break;
    }

    free(cfilename);
}

/**
 *  Individual preset file saving and user defined Inserted presets.
 * 
 * @param buf
 *  Buffer to hold efx parameters
 * 
 * @param j
 *  The effect number to load
 */
void RKR::getbuf(char *buf, int j)
{
    switch (j)
    {
    case EFX_REVERB:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_REVERB]->getpar(Reverb_DryWet), Rack_Effects[EFX_REVERB]->getpar(Reverb_Pan),
                Rack_Effects[EFX_REVERB]->getpar(Reverb_Time), Rack_Effects[EFX_REVERB]->getpar(Reverb_I_Delay),
                Rack_Effects[EFX_REVERB]->getpar(Reverb_Delay_FB), Rack_Effects[EFX_REVERB]->getpar(Reverb_SKIP_5),
                Rack_Effects[EFX_REVERB]->getpar(Reverb_SKIP_6), Rack_Effects[EFX_REVERB]->getpar(Reverb_LPF),
                Rack_Effects[EFX_REVERB]->getpar(Reverb_HPF), Rack_Effects[EFX_REVERB]->getpar(Reverb_Damp),
                Rack_Effects[EFX_REVERB]->getpar(Reverb_Type), Rack_Effects[EFX_REVERB]->getpar(Reverb_Room), EFX_Bypass[EFX_REVERB]);
        break;

    case EFX_ECHO:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_ECHO]->getpar(Echo_DryWet), Rack_Effects[EFX_ECHO]->getpar(Echo_Pan),
                Rack_Effects[EFX_ECHO]->getpar(Echo_Delay), Rack_Effects[EFX_ECHO]->getpar(Echo_LR_Delay),
                Rack_Effects[EFX_ECHO]->getpar(Echo_LR_Cross), Rack_Effects[EFX_ECHO]->getpar(Echo_Feedback),
                Rack_Effects[EFX_ECHO]->getpar(Echo_Damp), Rack_Effects[EFX_ECHO]->getpar(Echo_Reverse),
                Rack_Effects[EFX_ECHO]->getpar(Echo_Direct), EFX_Bypass[EFX_ECHO]);
        break;

    case EFX_CHORUS:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_DryWet), Rack_Effects[EFX_CHORUS]->getpar(Chorus_Pan),
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_LFO_Tempo), Rack_Effects[EFX_CHORUS]->getpar(Chorus_LFO_Random),
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_LFO_Type), Rack_Effects[EFX_CHORUS]->getpar(Chorus_LFO_Stereo),
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_Depth), Rack_Effects[EFX_CHORUS]->getpar(Chorus_Delay),
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_Feedback), Rack_Effects[EFX_CHORUS]->getpar(Chorus_LR_Cross),
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_SKIP_Flange_10), Rack_Effects[EFX_CHORUS]->getpar(Chorus_Subtract),
                Rack_Effects[EFX_CHORUS]->getpar(Chorus_Intense), EFX_Bypass[EFX_CHORUS]);
        break;

    case EFX_FLANGER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_DryWet), Rack_Effects[EFX_FLANGER]->getpar(Flanger_Pan),
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_LFO_Tempo), Rack_Effects[EFX_FLANGER]->getpar(Flanger_LFO_Random),
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_LFO_Type), Rack_Effects[EFX_FLANGER]->getpar(Flanger_LFO_Stereo),
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_Depth), Rack_Effects[EFX_FLANGER]->getpar(Flanger_Delay),
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_Feedback), Rack_Effects[EFX_FLANGER]->getpar(Flanger_LR_Cross),
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_SKIP_Flange_10), Rack_Effects[EFX_FLANGER]->getpar(Flanger_Subtract),
                Rack_Effects[EFX_FLANGER]->getpar(Flanger_Intense), EFX_Bypass[EFX_FLANGER]);
        break;

    case EFX_PHASER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_PHASER]->getpar(Phaser_DryWet), Rack_Effects[EFX_PHASER]->getpar(Phaser_Pan),
                Rack_Effects[EFX_PHASER]->getpar(Phaser_LFO_Tempo), Rack_Effects[EFX_PHASER]->getpar(Phaser_LFO_Random),
                Rack_Effects[EFX_PHASER]->getpar(Phaser_LFO_Type), Rack_Effects[EFX_PHASER]->getpar(Phaser_LFO_Stereo),
                Rack_Effects[EFX_PHASER]->getpar(Phaser_Depth), Rack_Effects[EFX_PHASER]->getpar(Phaser_Feedback),
                Rack_Effects[EFX_PHASER]->getpar(Phaser_Stages), Rack_Effects[EFX_PHASER]->getpar(Phaser_LR_Cross),
                Rack_Effects[EFX_PHASER]->getpar(Phaser_Subtract), Rack_Effects[EFX_PHASER]->getpar(Phaser_Phase),
                EFX_Bypass[EFX_PHASER]);
        break;

    case EFX_OVERDRIVE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_DryWet), Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Pan),
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_LR_Cross), Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Drive),
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Level), Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Type),
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Negate), Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_LPF),
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_HPF), Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Stereo),
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Prefilter), Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_SKIP_11),
                Rack_Effects[EFX_OVERDRIVE]->getpar(Overdrive_Suboctave), EFX_Bypass[EFX_OVERDRIVE]);
        break;

    case EFX_DISTORTION:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_DryWet), Rack_Effects[EFX_DISTORTION]->getpar(Dist_Pan),
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_LR_Cross), Rack_Effects[EFX_DISTORTION]->getpar(Dist_Drive),
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_Level), Rack_Effects[EFX_DISTORTION]->getpar(Dist_Type),
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_Negate), Rack_Effects[EFX_DISTORTION]->getpar(Dist_LPF),
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_HPF), Rack_Effects[EFX_DISTORTION]->getpar(Dist_Stereo),
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_Prefilter), Rack_Effects[EFX_DISTORTION]->getpar(Dist_SKIP_11),
                Rack_Effects[EFX_DISTORTION]->getpar(Dist_Suboctave), EFX_Bypass[EFX_DISTORTION]);
        break;

    case EFX_EQ:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_EQ]->getpar(EQ_31_HZ), Rack_Effects[EFX_EQ]->getpar(EQ_63_HZ),
                Rack_Effects[EFX_EQ]->getpar(EQ_125_HZ), Rack_Effects[EFX_EQ]->getpar(EQ_250_HZ),
                Rack_Effects[EFX_EQ]->getpar(EQ_500_HZ), Rack_Effects[EFX_EQ]->getpar(EQ_1_KHZ),
                Rack_Effects[EFX_EQ]->getpar(EQ_2_KHZ), Rack_Effects[EFX_EQ]->getpar(EQ_4_KHZ),
                Rack_Effects[EFX_EQ]->getpar(EQ_8_KHZ), Rack_Effects[EFX_EQ]->getpar(EQ_16_KHZ),
                Rack_Effects[EFX_EQ]->getpar(EQ_Gain), Rack_Effects[EFX_EQ]->getpar(EQ_Q), EFX_Bypass[EFX_EQ]);
        break;

    case EFX_PARAMETRIC:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Low_Freq), Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Low_Gain),
                Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Low_Q), Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Mid_Freq),
                Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Mid_Gain), Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Mid_Q),
                Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_High_Freq), Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_High_Gain),
                Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_High_Q), Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Gain),
                EFX_Bypass[EFX_PARAMETRIC]);
        break;

    case EFX_COMPRESSOR:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Threshold), Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Ratio),
                Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Output), Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Attack),
                Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Release), Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Auto_Out),
                Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Knee), Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Stereo),
                Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Peak), EFX_Bypass[EFX_COMPRESSOR]);
        break;


    case EFX_WAHWAH:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_WAHWAH]->getpar(WahWah_DryWet), Rack_Effects[EFX_WAHWAH]->getpar(WahWah_Pan),
                Rack_Effects[EFX_WAHWAH]->getpar(WahWah_LFO_Tempo), Rack_Effects[EFX_WAHWAH]->getpar(WahWah_LFO_Random),
                Rack_Effects[EFX_WAHWAH]->getpar(WahWah_LFO_Type), Rack_Effects[EFX_WAHWAH]->getpar(WahWah_LFO_Stereo),
                Rack_Effects[EFX_WAHWAH]->getpar(WahWah_Depth), Rack_Effects[EFX_WAHWAH]->getpar(WahWah_Sense),
                Rack_Effects[EFX_WAHWAH]->getpar(WahWah_ASI), Rack_Effects[EFX_WAHWAH]->getpar(WahWah_Smooth),
                Rack_Effects[EFX_WAHWAH]->getpar(WahWah_Mode), EFX_Bypass[EFX_WAHWAH]);
        break;

    case EFX_ALIENWAH:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_ALIENWAH]->getpar(Alien_DryWet), Rack_Effects[EFX_ALIENWAH]->getpar(Alien_Pan),
                Rack_Effects[EFX_ALIENWAH]->getpar(Alien_LFO_Tempo), Rack_Effects[EFX_ALIENWAH]->getpar(Alien_LFO_Random),
                Rack_Effects[EFX_ALIENWAH]->getpar(Alien_LFO_Type), Rack_Effects[EFX_ALIENWAH]->getpar(Alien_LFO_Stereo),
                Rack_Effects[EFX_ALIENWAH]->getpar(Alien_Depth), Rack_Effects[EFX_ALIENWAH]->getpar(Alien_Feedback),
                Rack_Effects[EFX_ALIENWAH]->getpar(Alien_Delay), Rack_Effects[EFX_ALIENWAH]->getpar(Alien_LR_Cross),
                Rack_Effects[EFX_ALIENWAH]->getpar(Alien_Phase), EFX_Bypass[EFX_ALIENWAH]);
        break;

    case EFX_CABINET:
        sprintf(buf, "%d,%d,%d\n",
                Rack_Effects[EFX_CABINET]->Ppreset, Rack_Effects[EFX_CABINET]->getpar(Cabinet_Gain), EFX_Bypass[EFX_CABINET]);
        break;

    case EFX_PAN:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_PAN]->getpar(Pan_DryWet), Rack_Effects[EFX_PAN]->getpar(Pan_Pan),
                Rack_Effects[EFX_PAN]->getpar(Pan_LFO_Tempo), Rack_Effects[EFX_PAN]->getpar(Pan_LFO_Random),
                Rack_Effects[EFX_PAN]->getpar(Pan_LFO_Type), Rack_Effects[EFX_PAN]->getpar(Pan_LFO_Stereo),
                Rack_Effects[EFX_PAN]->getpar(Pan_Ex_St_Amt), Rack_Effects[EFX_PAN]->getpar(Pan_AutoPan),
                Rack_Effects[EFX_PAN]->getpar(Pan_Enable_Extra), EFX_Bypass[EFX_PAN]);
        break;

    case EFX_HARMONIZER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_HARMONIZER]->getpar(Harm_DryWet), Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Pan),
                Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Gain), Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Interval),
                Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Filter_Freq), Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Select),
                Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Note), Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Chord),
                Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Filter_Gain), Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Filter_Q),
                Rack_Effects[EFX_HARMONIZER]->getpar(Harm_MIDI), EFX_Bypass[EFX_HARMONIZER]);
        break;

    case EFX_MUSICAL_DELAY:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_DryWet), Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Pan_1),
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Delay_1), Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Del_Offset),
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_LR_Cross), Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Feedback_1),
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Damp), Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Pan_2),
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Delay_2), Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Feedback_2),
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Tempo), Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Gain_1),
                Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Gain_2), EFX_Bypass[EFX_MUSICAL_DELAY]);
        break;

    case EFX_NOISEGATE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_NOISEGATE]->getpar(Gate_Threshold), Rack_Effects[EFX_NOISEGATE]->getpar(Gate_Range),
                Rack_Effects[EFX_NOISEGATE]->getpar(Gate_Attack), Rack_Effects[EFX_NOISEGATE]->getpar(Gate_Release),
                Rack_Effects[EFX_NOISEGATE]->getpar(Gate_LPF), Rack_Effects[EFX_NOISEGATE]->getpar(Gate_HPF),
                Rack_Effects[EFX_NOISEGATE]->getpar(Gate_Hold), EFX_Bypass[EFX_NOISEGATE]);
        break;

    case EFX_DERELICT:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_DERELICT]->getpar(Dere_DryWet), Rack_Effects[EFX_DERELICT]->getpar(Dere_Pan),
                Rack_Effects[EFX_DERELICT]->getpar(Dere_LR_Cross), Rack_Effects[EFX_DERELICT]->getpar(Dere_Drive),
                Rack_Effects[EFX_DERELICT]->getpar(Dere_Level), Rack_Effects[EFX_DERELICT]->getpar(Dere_Type),
                Rack_Effects[EFX_DERELICT]->getpar(Dere_Negate), Rack_Effects[EFX_DERELICT]->getpar(Dere_LPF),
                Rack_Effects[EFX_DERELICT]->getpar(Dere_HPF), Rack_Effects[EFX_DERELICT]->getpar(Dere_Color),
                Rack_Effects[EFX_DERELICT]->getpar(Dere_Prefilter), Rack_Effects[EFX_DERELICT]->getpar(Dere_Suboctave),
                EFX_Bypass[EFX_DERELICT]);
        break;

    case EFX_ANALOG_PHASER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_DryWet), Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Distortion),
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Tempo), Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Random),
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Type), Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Stereo),
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Width), Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Feedback),
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Stages), Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Mismatch),
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Subtract), Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Depth),
                Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Hyper), EFX_Bypass[EFX_ANALOG_PHASER]);
        break;

    case EFX_VALVE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_VALVE]->getpar(Valve_DryWet), Rack_Effects[EFX_VALVE]->getpar(Valve_Pan),
                Rack_Effects[EFX_VALVE]->getpar(Valve_LR_Cross), Rack_Effects[EFX_VALVE]->getpar(Valve_Drive),
                Rack_Effects[EFX_VALVE]->getpar(Valve_Level), Rack_Effects[EFX_VALVE]->getpar(Valve_Negate),
                Rack_Effects[EFX_VALVE]->getpar(Valve_LPF), Rack_Effects[EFX_VALVE]->getpar(Valve_HPF),
                Rack_Effects[EFX_VALVE]->getpar(Valve_Stereo), Rack_Effects[EFX_VALVE]->getpar(Valve_Prefilter),
                Rack_Effects[EFX_VALVE]->getpar(Valve_Distortion), Rack_Effects[EFX_VALVE]->getpar(Valve_Ex_Dist),
                Rack_Effects[EFX_VALVE]->getpar(Valve_Presence), EFX_Bypass[EFX_VALVE]);
        break;

    case EFX_DUAL_FLANGE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_DryWet), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Pan),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LR_Cross), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Depth),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Width), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Offset),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Feedback), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LPF),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Subtract), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Zero),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LFO_Tempo), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LFO_Stereo),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LFO_Type), Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LFO_Random),
                Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_Intense), EFX_Bypass[EFX_DUAL_FLANGE]);
        break;

    case EFX_RING:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_RING]->getpar(Ring_DryWet), Rack_Effects[EFX_RING]->getpar(Ring_Pan),
                Rack_Effects[EFX_RING]->getpar(Ring_LR_Cross), Rack_Effects[EFX_RING]->getpar(Ring_Level),
                Rack_Effects[EFX_RING]->getpar(Ring_Depth), Rack_Effects[EFX_RING]->getpar(Ring_Freq),
                Rack_Effects[EFX_RING]->getpar(Ring_Stereo), Rack_Effects[EFX_RING]->getpar(Ring_Sine),
                Rack_Effects[EFX_RING]->getpar(Ring_Triangle), Rack_Effects[EFX_RING]->getpar(Ring_Saw),
                Rack_Effects[EFX_RING]->getpar(Ring_Square), Rack_Effects[EFX_RING]->getpar(Ring_Input),
                Rack_Effects[EFX_RING]->getpar(Ring_Auto_Freq), EFX_Bypass[EFX_RING]);
        break;

    case EFX_EXCITER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_Gain), Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_1),
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_2), Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_3),
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_4), Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_5),
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_6), Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_7),
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_8), Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_9),
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_Harm_10), Rack_Effects[EFX_EXCITER]->getpar(Exciter_LPF),
                Rack_Effects[EFX_EXCITER]->getpar(Exciter_HPF), EFX_Bypass[EFX_EXCITER]);
        break;

    case EFX_DISTBAND:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_DryWet), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Pan),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_LR_Cross), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Drive),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Level), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Type_Low),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Type_Mid), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Type_Hi),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Gain_Low), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Gain_Mid),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Gain_Hi), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Negate),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Cross_1), Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Cross_2),
                Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Stereo), EFX_Bypass[EFX_DISTBAND]);
        break;

    case EFX_ARPIE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_ARPIE]->getpar(Arpie_DryWet), Rack_Effects[EFX_ARPIE]->getpar(Arpie_Pan),
                Rack_Effects[EFX_ARPIE]->getpar(Arpie_Tempo), Rack_Effects[EFX_ARPIE]->getpar(Arpie_LR_Delay),
                Rack_Effects[EFX_ARPIE]->getpar(Arpie_LR_Cross), Rack_Effects[EFX_ARPIE]->getpar(Arpie_Feedback),
                Rack_Effects[EFX_ARPIE]->getpar(Arpie_Damp), Rack_Effects[EFX_ARPIE]->getpar(Arpie_ArpeWD),
                Rack_Effects[EFX_ARPIE]->getpar(Arpie_Harm), Rack_Effects[EFX_ARPIE]->getpar(Arpie_Pattern),
                Rack_Effects[EFX_ARPIE]->getpar(Arpie_Subdivision), EFX_Bypass[EFX_ARPIE]);
        break;

    case EFX_EXPANDER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_EXPANDER]->getpar(Expander_Threshold), Rack_Effects[EFX_EXPANDER]->getpar(Expander_Shape),
                Rack_Effects[EFX_EXPANDER]->getpar(Expander_Attack), Rack_Effects[EFX_EXPANDER]->getpar(Expander_Release),
                Rack_Effects[EFX_EXPANDER]->getpar(Expander_LPF), Rack_Effects[EFX_EXPANDER]->getpar(Expander_HPF),
                Rack_Effects[EFX_EXPANDER]->getpar(Expander_Gain), EFX_Bypass[EFX_EXPANDER]);
        break;

    case EFX_SHUFFLE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_DryWet), Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Gain_L),
                Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Gain_ML), Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Gain_MH),
                Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Gain_H), Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Freq_L),
                Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Freq_ML), Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Freq_MH),
                Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Freq_H), Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_Width),
                Rack_Effects[EFX_SHUFFLE]->getpar(Shuffle_F_Band), EFX_Bypass[EFX_SHUFFLE]);
        break;

    case EFX_SYNTHFILTER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_DryWet), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Distort),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_LFO_Tempo), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_LFO_Random),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_LFO_Type), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_LFO_Stereo),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Width), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Feedback),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_LPF_Stages), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_HPF_Stages),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Subtract), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Depth),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Env_Sens), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Attack),
                Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Release), Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_Offset),
                EFX_Bypass[EFX_SYNTHFILTER]);
        break;

    case EFX_VARYBAND:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_DryWet), Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Tempo_1),
                Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Type_1), Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Stereo_1),
                Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Tempo_2), Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Type_2),
                Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Stereo_2), Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_Cross_1),
                Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_Cross_2), Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_Cross_3),
                Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_Combination), EFX_Bypass[EFX_VARYBAND]);
        break;

    case EFX_CONVOLOTRON:
    {
        Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_DryWet), Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Pan),
                Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Safe), Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Length),
                Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_User_File), Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_SKIP_5),
                Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Damp), Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Level),
                Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Set_File), Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_SKIP_9),
                Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Feedback), EFX_Bypass[EFX_CONVOLOTRON], Efx_Convolotron->Filename);
        break;
    }
    case EFX_LOOPER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_LOOPER]->getpar(Looper_DryWet), Rack_Effects[EFX_LOOPER]->getpar(Looper_Play),
                Rack_Effects[EFX_LOOPER]->getpar(Looper_Stop), Rack_Effects[EFX_LOOPER]->getpar(Looper_Record),
                Rack_Effects[EFX_LOOPER]->getpar(Looper_Clear), Rack_Effects[EFX_LOOPER]->getpar(Looper_Reverse),
                Rack_Effects[EFX_LOOPER]->getpar(Looper_Level_1), Rack_Effects[EFX_LOOPER]->getpar(Looper_Track_1),
                Rack_Effects[EFX_LOOPER]->getpar(Looper_Track_2), Rack_Effects[EFX_LOOPER]->getpar(Looper_AutoPlay),
                Rack_Effects[EFX_LOOPER]->getpar(Looper_Level_2), Rack_Effects[EFX_LOOPER]->getpar(Looper_Rec_1),
                Rack_Effects[EFX_LOOPER]->getpar(Looper_Rec_2), Rack_Effects[EFX_LOOPER]->getpar(Looper_Link), EFX_Bypass[EFX_LOOPER]);
        break;

    case EFX_MUTROMOJO:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_DryWet), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Resonance),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_LFO_Tempo), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_LFO_Random),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_LFO_Type), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_LFO_Stereo),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Depth), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Env_Sens),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Wah), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Env_Smooth),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_LowPass), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_BandPass),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_HighPass), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Stages),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Range), Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_St_Freq),
                Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_Mod_Res), Rack_Effects[EFX_MUTROMOJO]->getpar(Mutro_Mode_Legacy),
                Rack_Effects[EFX_MUTROMOJO]->getpar(Mutro_Preset), EFX_Bypass[EFX_MUTROMOJO]);

        break;

    case EFX_ECHOVERSE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_DryWet), Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Pan),
                Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Tempo), Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_LR_Delay),
                Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Angle), Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Feedback),
                Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Damp), Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Reverse),
                Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Subdivision), Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Ext_Stereo),
                EFX_Bypass[EFX_ECHOVERSE]);
        break;

    case EFX_COILCRAFTER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Gain), Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Origin),
                Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Destiny), Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Freq_1),
                Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Q_1), Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Freq_2),
                Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Q_2), Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_Tone),
                Rack_Effects[EFX_COILCRAFTER]->getpar(Coil_NeckMode), EFX_Bypass[EFX_COILCRAFTER]);
        break;

    case EFX_SHELFBOOST:
        sprintf(buf, "%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_SHELFBOOST]->getpar(Shelf_Gain), Rack_Effects[EFX_SHELFBOOST]->getpar(Shelf_Presence),
                Rack_Effects[EFX_SHELFBOOST]->getpar(Shelf_Tone), Rack_Effects[EFX_SHELFBOOST]->getpar(Shelf_Stereo),
                Rack_Effects[EFX_SHELFBOOST]->getpar(Shelf_Level), EFX_Bypass[EFX_SHELFBOOST]);
        break;

    case EFX_VOCODER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_VOCODER]->getpar(Vocoder_DryWet), Rack_Effects[EFX_VOCODER]->getpar(Vocoder_Pan),
                Rack_Effects[EFX_VOCODER]->getpar(Vocoder_Smear), Rack_Effects[EFX_VOCODER]->getpar(Vocoder_Q),
                Rack_Effects[EFX_VOCODER]->getpar(Vocoder_Input), Rack_Effects[EFX_VOCODER]->getpar(Vocoder_Level),
                Rack_Effects[EFX_VOCODER]->getpar(Vocoder_Ring), EFX_Bypass[EFX_VOCODER]);
        break;

    case EFX_SUSTAINER:
        sprintf(buf, "%d,%d,%d\n",
                Rack_Effects[EFX_SUSTAINER]->getpar(Sustain_Gain), Rack_Effects[EFX_SUSTAINER]->getpar(Sustain_Sustain),
                EFX_Bypass[EFX_SUSTAINER]);
        break;

    case EFX_SEQUENCE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_1), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_2),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_3), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_4),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_5), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_6),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_7), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Step_8),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_DryWet), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Tempo),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Resonance), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Amp),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Stdf), Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Mode),
                Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Range), EFX_Bypass[EFX_SEQUENCE]);
        break;

    case EFX_SHIFTER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_SHIFTER]->getpar(Shifter_DryWet), Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Pan),
                Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Gain), Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Attack),
                Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Decay), Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Threshold),
                Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Interval), Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Shift),
                Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Mode), Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Whammy), EFX_Bypass[EFX_SHIFTER]);
        break;


    case EFX_STOMPBOX:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Level), Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Tone),
                Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Mid), Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Bias),
                Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Gain), Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Mode),
                EFX_Bypass[EFX_STOMPBOX]);
        break;

    case EFX_REVERBTRON:
    {
        Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_DryWet), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Fade),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Safe), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Length),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_User_File), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_I_Delay),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Damp), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Level),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Set_File), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Stretch),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Feedback), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Pan),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Ex_Stereo), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Shuffle),
                Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_LPF), Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Diffusion),
                EFX_Bypass[EFX_REVERBTRON], Efx_Reverbtron->Filename);
        break;
    }
    case EFX_ECHOTRON:
    {
        Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_DryWet), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Depth),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_LFO_Width), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Taps),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_User_File), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Tempo),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Damp), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_LR_Cross),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Set_File), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_LFO_Stereo),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Feedback), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Pan),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Mod_Delay), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Mod_Filter),
                Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_LFO_Type), Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Filters),
                EFX_Bypass[EFX_ECHOTRON], Efx_Echotron->Filename);
        break;
    }
    case EFX_STEREOHARM:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_DryWet), Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_L_Gain),
                Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_L_Interval), Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_L_Chroma),
                Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_R_Gain), Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_R_Interval),
                Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_R_Chroma), Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Select),
                Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Note), Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Chord),
                Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_MIDI), Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_LR_Cross),
                EFX_Bypass[EFX_STEREOHARM]);
        break;

    case EFX_COMPBAND:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_DryWet), Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Low_Ratio),
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Mid_1_Ratio), Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Mid_2_Ratio),
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_High_Ratio), Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Low_Thresh),
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Mid_1_Thresh), Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Mid_2_Thresh),
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_High_Thresh), Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Cross_1),
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Cross_2), Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Cross_3),
                Rack_Effects[EFX_COMPBAND]->getpar(CompBand_Gain), EFX_Bypass[EFX_COMPBAND]);
        break;

    case EFX_OPTICALTREM:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_Depth), Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_LFO_Tempo),
                Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_LFO_Random), Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_LFO_Type),
                Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_LFO_Stereo), Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_Pan),
                Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_Invert), EFX_Bypass[EFX_OPTICALTREM]);
        break;


    case EFX_VIBE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_VIBE]->getpar(Vibe_Width), Rack_Effects[EFX_VIBE]->getpar(Vibe_LFO_Tempo),
                Rack_Effects[EFX_VIBE]->getpar(Vibe_LFO_Random), Rack_Effects[EFX_VIBE]->getpar(Vibe_LFO_Type),
                Rack_Effects[EFX_VIBE]->getpar(Vibe_LFO_Stereo), Rack_Effects[EFX_VIBE]->getpar(Vibe_Pan),
                Rack_Effects[EFX_VIBE]->getpar(Vibe_DryWet), Rack_Effects[EFX_VIBE]->getpar(Vibe_Feedback),
                Rack_Effects[EFX_VIBE]->getpar(Vibe_Depth), Rack_Effects[EFX_VIBE]->getpar(Vibe_LR_Cross),
                Rack_Effects[EFX_VIBE]->getpar(Vibe_Stereo), EFX_Bypass[EFX_VIBE]);
        break;

    case EFX_INFINITY:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_DryWet), Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_1),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_2), Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_3),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_4), Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_5),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_6), Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_7),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Band_8), Rack_Effects[EFX_INFINITY]->getpar(Infinity_Resonance),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Start), Rack_Effects[EFX_INFINITY]->getpar(Infinity_End),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Tempo), Rack_Effects[EFX_INFINITY]->getpar(Infinity_LR_Delay),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Subdivision), Rack_Effects[EFX_INFINITY]->getpar(Infinity_AutoPan),
                Rack_Effects[EFX_INFINITY]->getpar(Infinity_Reverse), Rack_Effects[EFX_INFINITY]->getpar(Infinity_Stages),
                EFX_Bypass[EFX_INFINITY]);
        break;
    }
}

/**
 *  Save individual presets from menu, File/Save Preset.
 * 
 * @param filename
 *      The user defined file name for the preset to be saved.
 */
void
RKR::savefile(char *filename)
{
    FILE *fn;
    char buf[256];
    fn = fopen(filename, "w");

    if (errno == EACCES)
    {
        Error_Handle(3);
        fclose(fn);
        return;
    }

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%s\n", VERSION);
    fputs(buf, fn);


    //Autor

    memset(buf, 0, sizeof (buf));

    if (strlen(Author) != 0)
    {
        sprintf(buf, "%s\n", Author);
    }
    else
    {
        if (UserRealName != NULL)
        {
            sprintf(buf, "%s\n", UserRealName);
        }
        else
        {
            sprintf(buf, "%s\n", getenv("USER"));
        }
    }

    fputs(buf, fn);

    //Preset Name

    memset(buf, 0, sizeof (buf));
    fputs(Preset_Name, fn);
    fputs("\n", fn);


    //General
    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%f,%f,%f,%d\n", Input_Gain, Master_Volume, Fraction_Bypass, Bypass);
    fputs(buf, fn);


    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        int j = efx_order[i];
        memset(buf, 0, sizeof (buf));
        getbuf(buf, j);
        fputs(buf, fn);
    }

    // Order
    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            efx_order[0], efx_order[1], efx_order[2], efx_order[3],
            efx_order[4], efx_order[5], efx_order[6], efx_order[7],
            efx_order[8], efx_order[9]);

    fputs(buf, fn);


    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                XUserMIDI[i][0], XUserMIDI[i][1], XUserMIDI[i][2], XUserMIDI[i][3], XUserMIDI[i][4],
                XUserMIDI[i][5], XUserMIDI[i][6], XUserMIDI[i][7], XUserMIDI[i][8], XUserMIDI[i][9],
                XUserMIDI[i][10], XUserMIDI[i][10], XUserMIDI[i][12], XUserMIDI[i][13], XUserMIDI[i][14],
                XUserMIDI[i][15], XUserMIDI[i][16], XUserMIDI[i][17], XUserMIDI[i][18], XUserMIDI[i][19]);

        fputs(buf, fn);
    }

    fclose(fn);
}

/**
 *  This parses the preset files *.rkr types, from the menu  File/Load Preset
 * 
 * @param filename
 *      The user selected filename.
 */
void
RKR::loadfile(char *filename)
{
    FILE *fn;
    char buf[256];

    if ((fn = fopen(filename, "r")) == NULL)
    {
        filetoload = NULL; // bad file name
        fl_alert("Error loading %s file!\n", filename);
        return;
    }

    New();

    for (int i = 0; i < 14; i++)
    {
        memset(buf, 0, sizeof (buf));
        
        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            fl_alert("Error loading %s file!\n", filename);
            file_error(fn);
            return;
        }
    }

    //Order
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error loading %s file Order!\n", filename);
        file_error(fn);
        return;
    }

    int l[10];
    sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           &l[0], &l[1], &l[2], &l[3], &l[4], &l[5], &l[6], &l[7], &l[8],
           &l[9]);

    fclose(fn);

    if ((fn = fopen(filename, "r")) == NULL)
    {
        return;
    }

    //Version
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error loading %s file Version!\n", filename);
        file_error(fn);
        return;
    }

    //Author
    memset(Author, 0, 64);
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error loading %s file Author!\n", filename);
        file_error(fn);
        return;
    }

    for (int i = 0; i < 64; i++)
    {
        if (buf[i] > 20)
        {
            Author[i] = buf[i];
        }
    }


    // Preset Name
    memset(Preset_Name, 0, 64);
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error loading %s file Preset Name!\n", filename);
        file_error(fn);
        return;
    }

    for (int i = 0; i < 64; i++)
    {
        if (buf[i] > 20)
        {
            Preset_Name[i] = buf[i];
        }
    }

    //General
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error loading %s file General!\n", filename);
        file_error(fn);
        return;
    }

    float in_vol, out_vol; in_vol = out_vol = 0.0;
    float balance = 1.0f;
    
    sscanf(buf, "%f,%f,%f,%d\n", &in_vol, &out_vol, &balance, &Bypass_B);

    if ((actuvol == 0) || (needtoloadstate))
    {
        Fraction_Bypass = balance;
        Input_Gain = in_vol;
        Master_Volume = out_vol;
    }

    for (int i = 0; i < 10; i++)
    {
        int j = l[i];

        memset(buf, 0, sizeof (buf));

        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            fl_alert("Error loading %s file General!\n", filename);
            file_error(fn);
            return;
        }
        putbuf(buf, j);
    }

    //Order
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error loading %s file Order!\n", filename);
        file_error(fn);
        return;
    }

    sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           &lv[EFX_ORDER][0], &lv[EFX_ORDER][1], &lv[EFX_ORDER][2], &lv[EFX_ORDER][3], &lv[EFX_ORDER][4],
           &lv[EFX_ORDER][5], &lv[EFX_ORDER][6], &lv[EFX_ORDER][7], &lv[EFX_ORDER][8], &lv[EFX_ORDER][9]);

    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));

        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            fl_alert("Error loading %s file MIDI!\n", filename);
            file_error(fn);
            return;
        }

        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &XUserMIDI[i][0], &XUserMIDI[i][1], &XUserMIDI[i][2], &XUserMIDI[i][3], &XUserMIDI[i][4],
               &XUserMIDI[i][5], &XUserMIDI[i][6], &XUserMIDI[i][7], &XUserMIDI[i][8], &XUserMIDI[i][9],
               &XUserMIDI[i][10], &XUserMIDI[i][10], &XUserMIDI[i][12], &XUserMIDI[i][13], &XUserMIDI[i][14],
               &XUserMIDI[i][15], &XUserMIDI[i][16], &XUserMIDI[i][17], &XUserMIDI[i][18], &XUserMIDI[i][19]);
    }

    fclose(fn);
    Actualizar_Audio();
}

void
RKR::file_error(FILE *fn)
{
    printf("File Error!\n");
    
    fclose(fn);
    Actualizar_Audio();
}

void
RKR::Actualizar_Audio()
{
    Bypass = 0;

    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        efx_order[i] = lv[EFX_ORDER][i];
    }

    EFX_Bypass[EFX_HARMONIZER] = 0;
    EFX_Bypass[EFX_RING] = 0;
    EFX_Bypass[EFX_STEREOHARM] = 0;

    /*  Original set and cleared for only the ten current from efx_order[] which did not
        clear other efx from leftover settings. The new behavior will reset all efx to default
        if not used. The reset will occur when the New button is pressed or a new bank is selected.
        If an existing bank repositions an effect, by replacement and then moving it to a new location
        then the settings are kept from previous.
        Legacy:
        for (j=0; j<10; j++) {
            switch(efx_order[j]) {
     */

    int i = 0;

    for (int j = 0; j < C_NUMBER_EFFECTS; j++)
    {
        switch (j)
        {

        case EFX_EQ:
// Special case
            EFX_Bypass[EFX_EQ] = 0;
            Rack_Effects[EFX_EQ]->cleanup();
            for (i = 0; i < C_EQ_PARAMETERS; i++)
                Rack_Effects[EFX_EQ]->changepar(i, lv[EFX_EQ][i]);
#if 0
            for (i = 0; i < 10; i++)
            {
                Rack_Effects[EFX_EQ]->changepar(i * 5 + 12, lv[EFX_EQ][i]);
                Rack_Effects[EFX_EQ]->changepar(i * 5 + 13, lv[EFX_EQ][11]);
            }
            Rack_Effects[EFX_EQ]->changepar(EQ_Gain, lv[EFX_EQ][10]);
#endif // 0
            
            EFX_Bypass[EFX_EQ] = EFX_Bank_Bypass[EFX_EQ];
            break;

        case EFX_COMPRESSOR:
            EFX_Bypass[EFX_COMPRESSOR] = 0;
            Rack_Effects[EFX_COMPRESSOR]->cleanup();
            for (i = 0; i < C_COMPRESS_PARAMETERS; i++)
                Rack_Effects[EFX_COMPRESSOR]->changepar(i, lv[EFX_COMPRESSOR][i]);
            EFX_Bypass[EFX_COMPRESSOR] = EFX_Bank_Bypass[EFX_COMPRESSOR];
            break;

        case EFX_DISTORTION:
            EFX_Bypass[EFX_DISTORTION] = 0;
            Rack_Effects[EFX_DISTORTION]->cleanup();
            for (i = 0; i < C_DIST_PARAMETERS; i++)
                Rack_Effects[EFX_DISTORTION]->changepar(i, lv[EFX_DISTORTION][i]);
            EFX_Bypass[EFX_DISTORTION] = EFX_Bank_Bypass[EFX_DISTORTION];
            break;

        case EFX_OVERDRIVE:
            EFX_Bypass[EFX_OVERDRIVE] = 0;
            Rack_Effects[EFX_OVERDRIVE]->cleanup();
            for (i = 0; i < C_OVERDRIVE_PARAMETERS; i++)
                Rack_Effects[EFX_OVERDRIVE]->changepar(i, lv[EFX_OVERDRIVE][i]);
            EFX_Bypass[EFX_OVERDRIVE] = EFX_Bank_Bypass[EFX_OVERDRIVE];
            break;

        case EFX_ECHO:
            EFX_Bypass[EFX_ECHO] = 0;
            Rack_Effects[EFX_ECHO]->cleanup();
            for (i = 0; i < C_ECHO_PARAMETERS; i++)
                Rack_Effects[EFX_ECHO]->changepar(i, lv[EFX_ECHO][i]);
            EFX_Bypass[EFX_ECHO] = EFX_Bank_Bypass[EFX_ECHO];
            break;

        case EFX_CHORUS:
            EFX_Bypass[EFX_CHORUS] = 0;
            Rack_Effects[EFX_CHORUS]->cleanup();
            for (i = 0; i < C_CHORUS_PARAMETERS; i++)
                Rack_Effects[EFX_CHORUS]->changepar(i, lv[EFX_CHORUS][i]);
            EFX_Bypass[EFX_CHORUS] = EFX_Bank_Bypass[EFX_CHORUS];
            break;

        case EFX_PHASER:
            EFX_Bypass[EFX_PHASER] = 0;
            Rack_Effects[EFX_PHASER]->cleanup();
            for (i = 0; i < C_PHASER_PARAMETERS; i++)
                Rack_Effects[EFX_PHASER]->changepar(i, lv[EFX_PHASER][i]);
            EFX_Bypass[EFX_PHASER] = EFX_Bank_Bypass[EFX_PHASER];
            break;

        case EFX_FLANGER:
            EFX_Bypass[EFX_FLANGER] = 0;
            Rack_Effects[EFX_FLANGER]->cleanup();
            for (i = 0; i < C_FLANGER_PARAMETERS; i++)
                Rack_Effects[EFX_FLANGER]->changepar(i, lv[EFX_FLANGER][i]);
            EFX_Bypass[EFX_FLANGER] = EFX_Bank_Bypass[EFX_FLANGER];
            break;

        case EFX_REVERB:
            EFX_Bypass[EFX_REVERB] = 0;
            Rack_Effects[EFX_REVERB]->cleanup();
            for (i = 0; i < C_REVERB_PARAMETERS; i++)
                Rack_Effects[EFX_REVERB]->changepar(i, lv[EFX_REVERB][i]);
            EFX_Bypass[EFX_REVERB] = EFX_Bank_Bypass[EFX_REVERB];
            break;

        case EFX_PARAMETRIC:
// Special case
            EFX_Bypass[EFX_PARAMETRIC] = 0;
            Rack_Effects[EFX_PARAMETRIC]->cleanup();
            for (i = 0; i < 3; i++)
            {
                Rack_Effects[EFX_PARAMETRIC]->changepar(i * 5 + 11, lv[EFX_PARAMETRIC][0 + i * 3]);
                Rack_Effects[EFX_PARAMETRIC]->changepar(i * 5 + 12, lv[EFX_PARAMETRIC][1 + i * 3]);
                Rack_Effects[EFX_PARAMETRIC]->changepar(i * 5 + 13, lv[EFX_PARAMETRIC][2 + i * 3]);
            }
            Rack_Effects[EFX_PARAMETRIC]->changepar(Parametric_Gain, lv[EFX_PARAMETRIC][9]);
            EFX_Bypass[EFX_PARAMETRIC] = EFX_Bank_Bypass[EFX_PARAMETRIC];
            break;

        case EFX_WAHWAH:
            EFX_Bypass[EFX_WAHWAH] = 0;
            Rack_Effects[EFX_WAHWAH]->cleanup();
            for (i = 0; i < C_WAHWAH_PARAMETERS; i++)
                Rack_Effects[EFX_WAHWAH]->changepar(i, lv[EFX_WAHWAH][i]);
            EFX_Bypass[EFX_WAHWAH] = EFX_Bank_Bypass[EFX_WAHWAH];
            break;

        case EFX_ALIENWAH:
            EFX_Bypass[EFX_ALIENWAH] = 0;
            Rack_Effects[EFX_ALIENWAH]->cleanup();
            for (i = 0; i < C_ALIENWAH_PARAMETERS; i++)
                Rack_Effects[EFX_ALIENWAH]->changepar(i, lv[EFX_ALIENWAH][i]);
            EFX_Bypass[EFX_ALIENWAH] = EFX_Bank_Bypass[EFX_ALIENWAH];
            break;

        case EFX_CABINET:
// Special case
            EFX_Bypass[EFX_CABINET] = 0;
            Rack_Effects[EFX_CABINET]->cleanup();
            for(i = 0; i < C_CABINET_PARAMETERS; i++)
                Rack_Effects[EFX_CABINET]->changepar(i, lv[EFX_CABINET][i]);
#if 0
            Rack_Effects[EFX_CABINET]->setpreset(lv[EFX_CABINET][0]);
            Rack_Effects[EFX_CABINET]->changepar(Cabinet_Gain, lv[EFX_CABINET][1]);
#endif
            EFX_Bypass[EFX_CABINET] = EFX_Bank_Bypass[EFX_CABINET];
            break;

        case EFX_PAN:
            EFX_Bypass[EFX_PAN] = 0;
            Rack_Effects[EFX_PAN]->cleanup();
            for (i = 0; i < C_PAN_PARAMETERS; i++)
                Rack_Effects[EFX_PAN]->changepar(i, lv[EFX_PAN][i]);
            EFX_Bypass[EFX_PAN] = EFX_Bank_Bypass[EFX_PAN];
            break;

        case EFX_HARMONIZER:
            EFX_Bypass[EFX_HARMONIZER] = 0;
            Rack_Effects[EFX_HARMONIZER]->cleanup();
// Maybe special - see stereo harm
            for (i = 0; i < C_HARM_PARAMETERS; i++)
                Rack_Effects[EFX_HARMONIZER]->changepar(i, lv[EFX_HARMONIZER][i]);
            EFX_Bypass[EFX_HARMONIZER] = EFX_Bank_Bypass[EFX_HARMONIZER];
            break;

        case EFX_MUSICAL_DELAY:
            EFX_Bypass[EFX_MUSICAL_DELAY] = 0;
            Rack_Effects[EFX_MUSICAL_DELAY]->cleanup();
            for (i = 0; i < C_MUSIC_PARAMETERS; i++)
                Rack_Effects[EFX_MUSICAL_DELAY]->changepar(i, lv[EFX_MUSICAL_DELAY][i]);
            EFX_Bypass[EFX_MUSICAL_DELAY] = EFX_Bank_Bypass[EFX_MUSICAL_DELAY];
            break;

        case EFX_NOISEGATE:
            EFX_Bypass[EFX_NOISEGATE] = 0;
            Rack_Effects[EFX_NOISEGATE]->cleanup();
            for (i = 0; i < C_GATE_PARAMETERS; i++)
                Rack_Effects[EFX_NOISEGATE]->changepar(i, lv[EFX_NOISEGATE][i]);
            EFX_Bypass[EFX_NOISEGATE] = EFX_Bank_Bypass[EFX_NOISEGATE];
            break;

        case EFX_DERELICT:
            EFX_Bypass[EFX_DERELICT] = 0;
            Rack_Effects[EFX_DERELICT]->cleanup();
            for (i = 0; i < C_DERE_PARAMETERS; i++)
                Rack_Effects[EFX_DERELICT]->changepar(i, lv[EFX_DERELICT][i]);
            EFX_Bypass[EFX_DERELICT] = EFX_Bank_Bypass[EFX_DERELICT];
            break;

        case EFX_ANALOG_PHASER:
            EFX_Bypass[EFX_ANALOG_PHASER] = 0;
            Rack_Effects[EFX_ANALOG_PHASER]->cleanup();
            for (i = 0; i < C_APHASER_PARAMETERS; i++)
                Rack_Effects[EFX_ANALOG_PHASER]->changepar(i, lv[EFX_ANALOG_PHASER][i]);
            EFX_Bypass[EFX_ANALOG_PHASER] = EFX_Bank_Bypass[EFX_ANALOG_PHASER];
            break;

        case EFX_VALVE:
            EFX_Bypass[EFX_VALVE] = 0;
            Rack_Effects[EFX_VALVE]->cleanup();
            for (i = 0; i < C_VALVE_PARAMETERS; i++)
                Rack_Effects[EFX_VALVE]->changepar(i, lv[EFX_VALVE][i]);
            EFX_Bypass[EFX_VALVE] = EFX_Bank_Bypass[EFX_VALVE];
            break;

        case EFX_DUAL_FLANGE:
            EFX_Bypass[EFX_DUAL_FLANGE] = 0;
            Rack_Effects[EFX_DUAL_FLANGE]->cleanup();
            for (i = 0; i < C_DFLANGE_PARAMETERS; i++)
                Rack_Effects[EFX_DUAL_FLANGE]->changepar(i, lv[EFX_DUAL_FLANGE][i]);
            EFX_Bypass[EFX_DUAL_FLANGE] = EFX_Bank_Bypass[EFX_DUAL_FLANGE];
            break;

        case EFX_RING:
            EFX_Bypass[EFX_RING] = 0;
            Rack_Effects[EFX_RING]->cleanup();
            for (i = 0; i < C_RING_PARAMETERS; i++)
                Rack_Effects[EFX_RING]->changepar(i, lv[EFX_RING][i]);
            EFX_Bypass[EFX_RING] = EFX_Bank_Bypass[EFX_RING];
            break;

        case EFX_EXCITER:
            EFX_Bypass[EFX_EXCITER] = 0;
            Rack_Effects[EFX_EXCITER]->cleanup();
            for (i = 0; i < C_EXCITER_PARAMETERS; i++)
                Rack_Effects[EFX_EXCITER]->changepar(i, lv[EFX_EXCITER][i]);
            EFX_Bypass[EFX_EXCITER] = EFX_Bank_Bypass[EFX_EXCITER];
            break;

        case EFX_DISTBAND:
            EFX_Bypass[EFX_DISTBAND] = 0;
            Rack_Effects[EFX_DISTBAND]->cleanup();
            for (i = 0; i < C_DBAND_PARAMETERS; i++)
                Rack_Effects[EFX_DISTBAND]->changepar(i, lv[EFX_DISTBAND][i]);
            EFX_Bypass[EFX_DISTBAND] = EFX_Bank_Bypass[EFX_DISTBAND];
            break;

        case EFX_ARPIE:
            EFX_Bypass[EFX_ARPIE] = 0;
            Rack_Effects[EFX_ARPIE]->cleanup();
            for (i = 0; i < C_ARPIE_PARAMETERS; i++)
                Rack_Effects[EFX_ARPIE]->changepar(i, lv[EFX_ARPIE][i]);
            EFX_Bypass[EFX_ARPIE] = EFX_Bank_Bypass[EFX_ARPIE];
            break;

        case EFX_EXPANDER:
            EFX_Bypass[EFX_EXPANDER] = 0;
            Rack_Effects[EFX_EXPANDER]->cleanup();
            for (i = 0; i < C_EXPANDER_PARAMETERS; i++)
                Rack_Effects[EFX_EXPANDER]->changepar(i, lv[EFX_EXPANDER][i]);
            EFX_Bypass[EFX_EXPANDER] = EFX_Bank_Bypass[EFX_EXPANDER];
            break;

        case EFX_SHUFFLE:
            EFX_Bypass[EFX_SHUFFLE] = 0;
            Rack_Effects[EFX_SHUFFLE]->cleanup();
            for (i = 0; i < C_SHUFFLE_PARAMETERS; i++)
                Rack_Effects[EFX_SHUFFLE]->changepar(i, lv[EFX_SHUFFLE][i]);
            EFX_Bypass[EFX_SHUFFLE] = EFX_Bank_Bypass[EFX_SHUFFLE];
            break;

        case EFX_SYNTHFILTER:
            EFX_Bypass[EFX_SYNTHFILTER] = 0;
            Rack_Effects[EFX_SYNTHFILTER]->cleanup();
            for (i = 0; i < C_SYNTHFILTER_PARAMETERS; i++)
                Rack_Effects[EFX_SYNTHFILTER]->changepar(i, lv[EFX_SYNTHFILTER][i]);
            EFX_Bypass[EFX_SYNTHFILTER] = EFX_Bank_Bypass[EFX_SYNTHFILTER];
            break;

        case EFX_VARYBAND:
            EFX_Bypass[EFX_VARYBAND] = 0;
            Rack_Effects[EFX_VARYBAND]->cleanup();
            for (i = 0; i < C_VARYBAND_PARAMETERS; i++)
                Rack_Effects[EFX_VARYBAND]->changepar(i, lv[EFX_VARYBAND][i]);
            EFX_Bypass[EFX_VARYBAND] = EFX_Bank_Bypass[EFX_VARYBAND];
            break;

        case EFX_CONVOLOTRON:
            EFX_Bypass[EFX_CONVOLOTRON] = 0;
            Rack_Effects[EFX_CONVOLOTRON]->cleanup();
            for (i = 0; i < C_CONVO_PARAMETERS; i++)
                Rack_Effects[EFX_CONVOLOTRON]->changepar(i, lv[EFX_CONVOLOTRON][i]);
            EFX_Bypass[EFX_CONVOLOTRON] = EFX_Bank_Bypass[EFX_CONVOLOTRON];
            break;

        case EFX_LOOPER:
            EFX_Bypass[EFX_LOOPER] = 0;
// Special Case
            // Rack_Effects[EFX_LOOPER]->cleanup();
            for (i = 0; i < C_LOOPER_PARAMETERS; i++)
                Rack_Effects[EFX_LOOPER]->changepar(i, lv[EFX_LOOPER][i]);
            EFX_Bypass[EFX_LOOPER] = EFX_Bank_Bypass[EFX_LOOPER];
            break;

        case EFX_MUTROMOJO:
            EFX_Bypass[EFX_MUTROMOJO] = 0;
            Rack_Effects[EFX_MUTROMOJO]->cleanup();
            for (i = 0; i < C_MUTRO_PARAMETERS; i++)
                Rack_Effects[EFX_MUTROMOJO]->changepar(i, lv[EFX_MUTROMOJO][i]);
            EFX_Bypass[EFX_MUTROMOJO] = EFX_Bank_Bypass[EFX_MUTROMOJO];
            break;

        case EFX_ECHOVERSE:
            EFX_Bypass[EFX_ECHOVERSE] = 0;
            Rack_Effects[EFX_ECHOVERSE]->cleanup();
            for (i = 0; i < C_ECHOVERSE_PARAMETERS; i++)
                Rack_Effects[EFX_ECHOVERSE]->changepar(i, lv[EFX_ECHOVERSE][i]);
            EFX_Bypass[EFX_ECHOVERSE] = EFX_Bank_Bypass[EFX_ECHOVERSE];
            break;

        case EFX_COILCRAFTER:
            EFX_Bypass[EFX_COILCRAFTER] = 0;
            Rack_Effects[EFX_COILCRAFTER]->cleanup();
            for (i = 0; i < C_COIL_PARAMETERS; i++)
                Rack_Effects[EFX_COILCRAFTER]->changepar(i, lv[EFX_COILCRAFTER][i]);
            EFX_Bypass[EFX_COILCRAFTER] = EFX_Bank_Bypass[EFX_COILCRAFTER];
            break;

        case EFX_SHELFBOOST:
            EFX_Bypass[EFX_SHELFBOOST] = 0;
            Rack_Effects[EFX_SHELFBOOST]->cleanup();
            for (i = 0; i < C_SHELF_PARAMETERS; i++)
                Rack_Effects[EFX_SHELFBOOST]->changepar(i, lv[EFX_SHELFBOOST][i]);
            EFX_Bypass[EFX_SHELFBOOST] = EFX_Bank_Bypass[EFX_SHELFBOOST];
            break;

        case EFX_VOCODER:
            EFX_Bypass[EFX_VOCODER] = 0;
            Rack_Effects[EFX_VOCODER]->cleanup();
            for (i = 0; i < C_VOCODER_PARAMETERS; i++)
                Rack_Effects[EFX_VOCODER]->changepar(i, lv[EFX_VOCODER][i]);
            EFX_Bypass[EFX_VOCODER] = EFX_Bank_Bypass[EFX_VOCODER];
            break;

        case EFX_SUSTAINER:
            EFX_Bypass[EFX_SUSTAINER] = 0;
            Rack_Effects[EFX_SUSTAINER]->cleanup();
            for (i = 0; i < C_SUSTAIN_PARAMETERS; i++)
                Rack_Effects[EFX_SUSTAINER]->changepar(i, lv[EFX_SUSTAINER][i]);
            EFX_Bypass[EFX_SUSTAINER] = EFX_Bank_Bypass[EFX_SUSTAINER];
            break;

        case EFX_SEQUENCE:
            EFX_Bypass[EFX_SEQUENCE] = 0;
            Rack_Effects[EFX_SEQUENCE]->cleanup();
            for (i = 0; i < C_SEQUENCE_PARAMETERS; i++)
                Rack_Effects[EFX_SEQUENCE]->changepar(i, lv[EFX_SEQUENCE][i]);
            EFX_Bypass[EFX_SEQUENCE] = EFX_Bank_Bypass[EFX_SEQUENCE];
            break;

        case EFX_SHIFTER:
            EFX_Bypass[EFX_SHIFTER] = 0;
            Rack_Effects[EFX_SHIFTER]->cleanup();
            for (i = 0; i < C_SHIFTER_PARAMETERS; i++)
                Rack_Effects[EFX_SHIFTER]->changepar(i, lv[EFX_SHIFTER][i]);
            EFX_Bypass[EFX_SHIFTER] = EFX_Bank_Bypass[EFX_SHIFTER];
            break;

        case EFX_STOMPBOX:
            EFX_Bypass[EFX_STOMPBOX] = 0;
            Rack_Effects[EFX_STOMPBOX]->cleanup();
            for (i = 0; i < C_STOMP_PARAMETERS; i++)
                Rack_Effects[EFX_STOMPBOX]->changepar(i, lv[EFX_STOMPBOX][i]);
            EFX_Bypass[EFX_STOMPBOX] = EFX_Bank_Bypass[EFX_STOMPBOX];
            break;

        case EFX_REVERBTRON:
            EFX_Bypass[EFX_REVERBTRON] = 0;
            Rack_Effects[EFX_REVERBTRON]->cleanup();
            for (i = 0; i < C_REVTRON_PARAMETERS; i++)
                Rack_Effects[EFX_REVERBTRON]->changepar(i, lv[EFX_REVERBTRON][i]);
            EFX_Bypass[EFX_REVERBTRON] = EFX_Bank_Bypass[EFX_REVERBTRON];
            break;

        case EFX_ECHOTRON:
            EFX_Bypass[EFX_ECHOTRON] = 0;
            Rack_Effects[EFX_ECHOTRON]->cleanup();
            for (i = 0; i < C_ECHOTRON_PARAMETERS; i++)
                Rack_Effects[EFX_ECHOTRON]->changepar(i, lv[EFX_ECHOTRON][i]);
            EFX_Bypass[EFX_ECHOTRON] = EFX_Bank_Bypass[EFX_ECHOTRON];
            break;

        case EFX_STEREOHARM:
            EFX_Bypass[EFX_STEREOHARM] = 0;
            Rack_Effects[EFX_STEREOHARM]->cleanup();
            for (i = 0; i < C_SHARM_PARAMETERS; i++)
                Rack_Effects[EFX_STEREOHARM]->changepar(i, lv[EFX_STEREOHARM][i]);
// Special Case
            if (lv[EFX_STEREOHARM][10]) RC_Stereo_Harm->cleanup();
            
            EFX_Bypass[EFX_STEREOHARM] = EFX_Bank_Bypass[EFX_STEREOHARM];
            break;

        case EFX_COMPBAND:
            EFX_Bypass[EFX_COMPBAND] = 0;
            Rack_Effects[EFX_COMPBAND]->cleanup();
            for (i = 0; i < C_COMPBAND_PARAMETERS; i++)
                Rack_Effects[EFX_COMPBAND]->changepar(i, lv[EFX_COMPBAND][i]);
            EFX_Bypass[EFX_COMPBAND] = EFX_Bank_Bypass[EFX_COMPBAND];
            break;

        case EFX_OPTICALTREM:
            EFX_Bypass[EFX_OPTICALTREM] = 0;
            Rack_Effects[EFX_OPTICALTREM]->cleanup();
            for (i = 0; i < C_OPTICAL_PARAMETERS; i++)
                Rack_Effects[EFX_OPTICALTREM]->changepar(i, lv[EFX_OPTICALTREM][i]);
            EFX_Bypass[EFX_OPTICALTREM] = EFX_Bank_Bypass[EFX_OPTICALTREM];
            break;

        case EFX_VIBE:
            EFX_Bypass[EFX_VIBE] = 0;
            Rack_Effects[EFX_VIBE]->cleanup();
            for (i = 0; i < C_VIBE_PARAMETERS; i++)
                Rack_Effects[EFX_VIBE]->changepar(i, lv[EFX_VIBE][i]);
            EFX_Bypass[EFX_VIBE] = EFX_Bank_Bypass[EFX_VIBE];
            break;

        case EFX_INFINITY:
            EFX_Bypass[EFX_INFINITY] = 0;
            Rack_Effects[EFX_INFINITY]->cleanup();
            for (i = 0; i < C_INFINITY_PARAMETERS; i++)
                Rack_Effects[EFX_INFINITY]->changepar(i, lv[EFX_INFINITY][i]);
            EFX_Bypass[EFX_INFINITY] = EFX_Bank_Bypass[EFX_INFINITY];
            break;

        }
    }


    Bypass = Bypass_B;

    if (needtoloadstate)
    {
        calculavol(1);
        calculavol(2);
    }
}

/**
 *  Loads the default banks preset names and information for the bank manager window.
 */
void
RKR::loadnames()
{
    FILE *fn;
    char temp[128];

    memset(B_Names, 0, sizeof (B_Names));

    for (int k = 0; k < 4; k++)
    {
        switch (k)
        {

        case 0:
            memset(temp, 0, sizeof (temp));
            sprintf(temp, "%s/Default.rkrb", DATADIR);
            break;

        case 1:
            memset(temp, 0, sizeof (temp));
            sprintf(temp, "%s/Extra.rkrb", DATADIR);
            break;

        case 2:
            memset(temp, 0, sizeof (temp));
            sprintf(temp, "%s/Extra1.rkrb", DATADIR);
            break;

        case 3:
            memset(temp, 0, sizeof (temp));
            sprintf(temp, "%s", BankFilename);
            break;
        }

        if ((fn = fopen(temp, "rb")) != NULL)
        {
            New_Bank();
            while (1)
            {
                size_t ret = fread(&Bank, sizeof (Bank), 1, fn);
                
                if (feof(fn))
                    break;
                
                if (ret != 1)
                {
                    fl_alert("fread error in loadnames()");
                    break;
                }

                for (int j = 1; j <= 60; j++)
                {
                    strcpy(B_Names[k][j].Preset_Name, Bank[j].Preset_Name);
                }
            }
            fclose(fn);
        }
    }
}

/**
 * Reverts the file ordering to program Bank[].lv[][] array ordering.
 * The Bank files (.rkrb types) effects indexing is saved in the order
 * of LV_File_Index. The program uses EFX_Index.
 * 
 * @param lv_revert
 *      A single Bank[].lv[][] array to be converted. This will alter the array.
 * 
 * @param size
 *      The size of the array sizeof().
 */
void
RKR::revert_file_to_bank(int lv_revert[C_MAX_EFFECTS][C_MAX_PARAMETERS], int size)
{
    // Table to revert LV_File_Index to EFX_Index.
    const int reversion[C_MAX_EFFECTS] =
    {
        EFX_REVERB,
        EFX_ECHO,
        EFX_CHORUS,
        EFX_FLANGER,
        EFX_PHASER,
        EFX_OVERDRIVE,
        EFX_DISTORTION,
        EFX_EQ,
        EFX_PARAMETRIC,
        EFX_COMPRESSOR,
        EFX_ORDER,           // 10
        EFX_WAHWAH,
        EFX_ALIENWAH,
        EFX_CABINET,
        EFX_PAN,
        EFX_HARMONIZER,
        EFX_MUSICAL_DELAY,
        EFX_NOISEGATE,
        EFX_DERELICT,
        EFX_ANALOG_PHASER,
        EFX_VALVE,           // 20
        EFX_DUAL_FLANGE,
        EFX_RING,
        EFX_EXCITER,
        EFX_DISTBAND,
        EFX_ARPIE,
        EFX_EXPANDER,
        EFX_SHUFFLE,
        EFX_SYNTHFILTER,
        EFX_VARYBAND,
        EFX_CONVOLOTRON,     // 30
        EFX_LOOPER,
        EFX_MUTROMOJO,
        EFX_ECHOVERSE,
        EFX_COILCRAFTER,
        EFX_SHELFBOOST,
        EFX_VOCODER,
        EFX_SUSTAINER,
        EFX_SEQUENCE,
        EFX_SHIFTER,
        EFX_STOMPBOX,        // 40
        EFX_REVERBTRON,
        EFX_ECHOTRON,
        EFX_STEREOHARM,
        EFX_COMPBAND,
        EFX_OPTICALTREM,
        EFX_VIBE,
        EFX_INFINITY,       // 47

        // offset by -1 for order moved to 69
        47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65, 66, 67, 68
    };

    // Temporary array to hold reverted 
    int temp[C_MAX_EFFECTS][C_MAX_PARAMETERS];
    memset(temp, 0, sizeof (temp));
    
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        // Find the reversion row of the bank
        int row = reversion[i];
        
        // Put the bank row into the temp by reversion row.
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            temp[row][j] = lv_revert[i][j];
        }
    }
    
    // Clear the bank array.
    memset(lv_revert, 0, sizeof (size));
    
    // Put the reverted temp array into the bank array.
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            lv_revert[i][j] = temp[i][j];
        }
    }
}

/**
 * Converts the program Bank[].lv[][] array ordering to file ordering.
 * The Bank files (.rkrb types) effects indexing is saved in the order
 * of LV_File_Index. This was originally part of legacy rakarrack which kept 
 * the lv[][] indexing the same for the program. The program otherwise
 * used the indexing of EFX_Index. This resulted in extra code having 
 * to individually map each effect when copying bank to preset. A
 * for() loop would be much easier if the two were aligned.
 * Originally this was all done with magic numbers. The conversion/reversion
 * is simply to align the lv[][] arrays with the program without having to 
 * change the file structure.
 * 
 * @param lv_convert
 *      A single Bank[].lv[][] array to be converted. This will alter the array.
 * 
 * @param size
 *      The size of the array sizeof().
 */
void
RKR::convert_bank_to_file(int lv_convert[C_MAX_EFFECTS][C_MAX_PARAMETERS], int size)
{
    // The effect order used for file saving lv[70][20] and Preset_Bank_Struct.lv[70][20].
    enum LV_File_Index
    {
        LV_REVERB = 0,
        LV_ECHO,
        LV_CHORUS,
        LV_FLANGER,
        LV_PHASER,
        LV_OVERDRIVE,
        LV_DISTORTION,
        LV_EQ,
        LV_PARAMETRIC,
        LV_COMPRESSOR,
        LV_ORDER,           // 10 - the order saved is EFX_Index order
        LV_WAHWAH,
        LV_ALIENWAH,
        LV_CABINET,
        LV_PAN,
        LV_HARMONIZER,
        LV_MUSICAL_DELAY,
        LV_NOISEGATE,
        LV_DERELICT,
        LV_ANALOG_PHASER,
        LV_VALVE,           // 20
        LV_DUAL_FLANGE,
        LV_RING,
        LV_EXCITER,
        LV_DISTBAND,
        LV_ARPIE,
        LV_EXPANDER,
        LV_SHUFFLE,
        LV_SYNTHFILTER,
        LV_VARYBAND,
        LV_CONVOLOTRON,     // 30
        LV_LOOPER,
        LV_MUTROMOJO,
        LV_ECHOVERSE,
        LV_COILCRAFTER,
        LV_SHELFBOOST,
        LV_VOCODER,
        LV_SUSTAINER,
        LV_SEQUENCE,
        LV_SHIFTER,
        LV_STOMPBOX,        // 40
        LV_REVERBTRON,
        LV_ECHOTRON,
        LV_STEREOHARM,
        LV_COMPBAND,
        LV_OPTICALTREM,
        LV_VIBE,
        LV_INFINITY        // 47
    };

    // Table to convert program EFX_Index to LV_File_Index.
    const int conversion[C_MAX_EFFECTS] = 
    {
        LV_EQ,             // 0
        LV_COMPRESSOR,
        LV_DISTORTION,
        LV_OVERDRIVE,
        LV_ECHO,
        LV_CHORUS,
        LV_PHASER,
        LV_FLANGER,
        LV_REVERB,
        LV_PARAMETRIC,
        LV_WAHWAH,         // 10
        LV_ALIENWAH,
        LV_CABINET,
        LV_PAN,
        LV_HARMONIZER,
        LV_MUSICAL_DELAY,
        LV_NOISEGATE,
        LV_DERELICT,
        LV_ANALOG_PHASER,
        LV_VALVE,
        LV_DUAL_FLANGE,    // 20
        LV_RING,
        LV_EXCITER,
        LV_DISTBAND,
        LV_ARPIE,
        LV_EXPANDER,
        LV_SHUFFLE,
        LV_SYNTHFILTER,
        LV_VARYBAND,
        LV_CONVOLOTRON,
        LV_LOOPER,         // 30
        LV_MUTROMOJO,
        LV_ECHOVERSE,
        LV_COILCRAFTER,
        LV_SHELFBOOST,
        LV_VOCODER,
        LV_SUSTAINER,
        LV_SEQUENCE,
        LV_SHIFTER,
        LV_STOMPBOX,
        LV_REVERBTRON,     // 40
        LV_ECHOTRON,
        LV_STEREOHARM,
        LV_COMPBAND,
        LV_OPTICALTREM,
        LV_VIBE,
        LV_INFINITY,       // 46

        // Offset by +1 for order inserted at 10
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
        59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,

        LV_ORDER           // 69
    };

    // Temporary array to hold converted
    int temp[C_MAX_EFFECTS][C_MAX_PARAMETERS];
    memset(temp, 0, sizeof (temp));
    
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        // Find the conversion row of the bank
        int row = conversion[i];
        
        // Put the bank row into the temp by conversion row.
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            temp[row][j] = lv_convert[i][j];
        }
    }
    
    // Clear the bank array.
    memset(lv_convert, 0, sizeof (size));
    
    // Put the converted temp array into the bank array.
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            lv_convert[i][j] = temp[i][j];
        }
    }
}

/**
 *  This parses the rakarrack bank files *.rkrb types.
 * @param filename
 *      The bank file to parse.
 * 
 * @return 
 *      Zero (0) if an error occurs.
 *      One (1) if successful parsing.
 */
int
RKR::loadbank(char *filename)
{
    int err_message = 1;
    char meslabel[70];
    FILE *fn;

    memset(meslabel, 0, sizeof (meslabel));
    sprintf(meslabel, "%s %s", jackcliname, VERSION);

    err_message = CheckOldBank(filename);

    switch (err_message)
    {
    case 0:
        break;
    case 1:
        Message(1, meslabel, "Can not load this Bank file because is from a old rakarrack version,\n please use 'Convert Old Bank' menu entry in the Bank window.");
        return (0);
        break;
    case 2:
        Message(1, meslabel, "Can not load this Bank file\n");
        return (0);
        break;
    case 3:
        Message(1, meslabel, "Can not load this Bank file because is from a old rakarrack git version,\n please use rakgit2new utility to convert.");
        return (0);
        break;
    }

    if ((fn = fopen(filename, "rb")) != NULL)
    {
        New_Bank();

        while (1)
        {
            size_t ret = fread(&Bank, sizeof (Bank), 1, fn);

            if (feof(fn))
                break;

            if (ret != 1)
            {
                fl_alert("fread error in loadbank()");
                break;
            }
        }

        fclose(fn);

        if (BigEndian())
        {
            fix_endianess();
        }

        convert_IO();

        for(int i = 0; i < 62; i++)
        {
            revert_file_to_bank(Bank[i].lv, sizeof(Bank[i].lv));
        }

        modified = 0;
        new_bank_loaded = 1;
        return (1);
    }
    return (0);
};

int
RKR::savebank(char *filename)
{
    FILE *fn;

    if ((fn = fopen(filename, "wb")) != NULL)
    {
        copy_IO();

        for(int i = 0; i < 62; i++)
        {
            convert_bank_to_file(Bank[i].lv, sizeof(Bank[i].lv));
        }

        if (BigEndian())
        {
            fix_endianess();
        }
        
        fwrite(&Bank, sizeof (Bank), 1, fn);
        
        if (BigEndian())
        {
            fix_endianess();
        }

        fclose(fn);

        for(int i = 0; i < 62; i++)
        {
            revert_file_to_bank(Bank[i].lv, sizeof(Bank[i].lv));
        }

        modified = 0;
        return (1);
    }

    if (errno == EACCES)
    {
        Error_Handle(3);
    }
    
    return (0);
}

void
RKR::New()
{
    // Set all main rack off
    for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
    {
        active[j] = 0;
    }

    memset(Preset_Name, 0, sizeof (char) * 64);
    
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));

    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));

    memset(Author, 0, sizeof (char) * 64);
    strcpy(Author, UserRealName);
    Input_Gain = .5f;
    Master_Volume = .5f;
    Fraction_Bypass = 1.0f;
    Bypass = 0;
    memset(lv, 0, sizeof (lv));

    // Set the default presets
    for (int j = 0; j < C_NUMBER_EFFECTS; j++)
    {
        for (int k = 0; k < C_NUMBER_PARAMETERS; k++)
        {
            lv[j][k] = presets_default[j][k];
        }

        lv[j][C_BYPASS] = 0;
    }
    // Set the default order
    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        // Order from 0 to 10
        lv[EFX_ORDER][k] = efx_order[k] = k;
    }
    
    // Set all effects bypass to off
    for(int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        EFX_Bank_Bypass[i] = 0;
    }

    Bypass_B = 0;

    memset(XUserMIDI, 0, sizeof (XUserMIDI));

    Actualizar_Audio();
}

void
RKR::New_Bank()
{
    for (int i = 0; i < 62; i++)
    {
        memset(Bank[i].Preset_Name, 0, sizeof (Bank[i].Preset_Name));
        memset(Bank[i].Author, 0, sizeof (Bank[i].Author));
        strcpy(Bank[i].Author, UserRealName);
        memset(Bank[i].ConvoFiname, 0, sizeof (Bank[i].ConvoFiname));
        memset(Bank[i].RevFiname, 0, sizeof (Bank[i].RevFiname));
        memset(Bank[i].EchoFiname, 0, sizeof (Bank[i].EchoFiname));

        Bank[i].Input_Gain = .5f;
        Bank[i].Master_Volume = .5f;
        Bank[i].Balance = 1.0f;
        Bank[i].Bypass = 0;
        memset(Bank[i].lv, 0, sizeof (Bank[i].lv));

        // Set the default presets
        for (int j = 0; j < C_NUMBER_EFFECTS; j++)
        {
            for (int k = 0; k < C_NUMBER_PARAMETERS; k++)
            {
                Bank[i].lv[j][k] = presets_default[j][k];
            }
            Bank[i].lv[j][C_BYPASS] = 0;
        }
        
        // Set the default order
        for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
        {
            Bank[i].lv[EFX_ORDER][k] = k;   // Order from 0 to 10
        }

        memset(Bank[i].XUserMIDI, 0, sizeof (Bank[i].XUserMIDI));
    }
};

void
RKR::Bank_to_Preset(int i)
{
    memset(Preset_Name, 0, sizeof (char) * 64);
    strcpy(Preset_Name, Bank[i].Preset_Name);
    memset(Author, 0, sizeof (char) * 64);
    strcpy(Author, Bank[i].Author);
    
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));
    strcpy(Efx_Convolotron->Filename, Bank[i].ConvoFiname);

    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));
    strcpy(Efx_Reverbtron->Filename, Bank[i].RevFiname);

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));
    strcpy(Efx_Echotron->Filename, Bank[i].EchoFiname);

    for (int j = 0; j < C_MAX_EFFECTS; j++)
    {
        for (int k = 0; k < C_MAX_PARAMETERS; k++)
        {
            lv[j][k] = Bank[i].lv[j][k];
        }
    }

    // Set the main rack effect order
    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        efx_order[k] = Bank[i].lv[EFX_ORDER][k];
    }
    
    // Set the bypass state for each effect
    for(int k = 0; k < C_NUMBER_EFFECTS; k++)
    {
        EFX_Bank_Bypass[k] = Bank[i].lv[k][C_BYPASS];
    }

    Bypass_B = Bypass;

    memcpy(XUserMIDI, Bank[i].XUserMIDI, sizeof (XUserMIDI));

    Actualizar_Audio();

    if (actuvol == 0)
    {
        Input_Gain = Bank[i].Input_Gain;
        Master_Volume = Bank[i].Master_Volume;
        Fraction_Bypass = Bank[i].Balance;
    }

    if ((Tap_Updated) && (Tap_Bypass) && (Tap_TempoSet > 0) && (Tap_TempoSet < 601))
    {
        Update_tempo();
    }
}

void
RKR::Preset_to_Bank(int i)
{
    // Main window information
    memset(Bank[i].Preset_Name, 0, sizeof (Bank[i].Preset_Name));
    strcpy(Bank[i].Preset_Name, Preset_Name);
    memset(Bank[i].Author, 0, sizeof (Bank[i].Author));
    strcpy(Bank[i].Author, Author);
    
    // Special cases filenames for Convolotron, Echotron, Reverbtron
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Bank[i].ConvoFiname, 0, sizeof (Bank[i].ConvoFiname));    
    strcpy(Bank[i].ConvoFiname, Efx_Convolotron->Filename);
    
    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Bank[i].RevFiname, 0, sizeof (Bank[i].RevFiname));
    strcpy(Bank[i].RevFiname, Efx_Reverbtron->Filename);

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Bank[i].EchoFiname, 0, sizeof (Bank[i].EchoFiname));
    strcpy(Bank[i].EchoFiname, Efx_Echotron->Filename);

    // Master effect
    Bank[i].Input_Gain = Input_Gain;
    Bank[i].Master_Volume = Master_Volume;
    Bank[i].Balance = Fraction_Bypass;

    // Load all effect parameters into the lv[][] array for current preset (main window)
    int j = 0;

    for (j = 0; j < C_REVERB_PARAMETERS; j++)
        lv[EFX_REVERB][j] = Rack_Effects[EFX_REVERB]->getpar(j);
    for (j = 0; j < C_ECHO_PARAMETERS; j++)
        lv[EFX_ECHO][j] = Rack_Effects[EFX_ECHO]->getpar(j);
    for (j = 0; j < C_CHORUS_PARAMETERS; j++)
        lv[EFX_CHORUS][j] = Rack_Effects[EFX_CHORUS]->getpar(j);
    for (j = 0; j < C_FLANGER_PARAMETERS; j++)
        lv[EFX_FLANGER][j] = Rack_Effects[EFX_FLANGER]->getpar(j);
    for (j = 0; j < C_PHASER_PARAMETERS; j++)
        lv[EFX_PHASER][j] = Rack_Effects[EFX_PHASER]->getpar(j);
    for (j = 0; j < C_OVERDRIVE_PARAMETERS; j++)
        lv[EFX_OVERDRIVE][j] = Rack_Effects[EFX_OVERDRIVE]->getpar(j);
    for (j = 0; j < C_DIST_PARAMETERS; j++)
        lv[EFX_DISTORTION][j] = Rack_Effects[EFX_DISTORTION]->getpar(j);
    for (j = 0; j < C_COMPRESS_PARAMETERS; j++)
        lv[EFX_COMPRESSOR][j] = Rack_Effects[EFX_COMPRESSOR]->getpar(j);
    for (j = 0; j < C_WAHWAH_PARAMETERS; j++)
        lv[EFX_WAHWAH][j] = Rack_Effects[EFX_WAHWAH]->getpar(j);
    for (j = 0; j < C_ALIENWAH_PARAMETERS; j++)
        lv[EFX_ALIENWAH][j] = Rack_Effects[EFX_ALIENWAH]->getpar(j);
    for (j = 0; j < C_PAN_PARAMETERS; j++)
        lv[EFX_PAN][j] = Rack_Effects[EFX_PAN]->getpar(j);
    for (j = 0; j < C_HARM_PARAMETERS; j++)
        lv[EFX_HARMONIZER][j] = Rack_Effects[EFX_HARMONIZER]->getpar(j);
    for (j = 0; j < C_MUSIC_PARAMETERS; j++)
        lv[EFX_MUSICAL_DELAY][j] = Rack_Effects[EFX_MUSICAL_DELAY]->getpar(j);
    for (j = 0; j < C_GATE_PARAMETERS; j++)
        lv[EFX_NOISEGATE][j] = Rack_Effects[EFX_NOISEGATE]->getpar(j);
    for (j = 0; j < C_DERE_PARAMETERS; j++)
        lv[EFX_DERELICT][j] = Rack_Effects[EFX_DERELICT]->getpar(j);
    for (j = 0; j < C_APHASER_PARAMETERS; j++)
        lv[EFX_ANALOG_PHASER][j] = Rack_Effects[EFX_ANALOG_PHASER]->getpar(j);
    for (j = 0; j < C_VALVE_PARAMETERS; j++)
        lv[EFX_VALVE][j] = Rack_Effects[EFX_VALVE]->getpar(j);
    for (j = 0; j < C_DFLANGE_PARAMETERS; j++)
        lv[EFX_DUAL_FLANGE][j] = Rack_Effects[EFX_DUAL_FLANGE]->getpar(j);
    for (j = 0; j < C_RING_PARAMETERS; j++)
        lv[EFX_RING][j] = Rack_Effects[EFX_RING]->getpar(j);
    for (j = 0; j < C_EXCITER_PARAMETERS; j++)
        lv[EFX_EXCITER][j] = Rack_Effects[EFX_EXCITER]->getpar(j);
    for (j = 0; j < C_DBAND_PARAMETERS; j++)
        lv[EFX_DISTBAND][j] = Rack_Effects[EFX_DISTBAND]->getpar(j);
    for (j = 0; j < C_ARPIE_PARAMETERS; j++)
        lv[EFX_ARPIE][j] = Rack_Effects[EFX_ARPIE]->getpar(j);
    for (j = 0; j < C_EXPANDER_PARAMETERS; j++)
        lv[EFX_EXPANDER][j] = Rack_Effects[EFX_EXPANDER]->getpar(j);
    for (j = 0; j < C_SHUFFLE_PARAMETERS; j++)
        lv[EFX_SHUFFLE][j] = Rack_Effects[EFX_SHUFFLE]->getpar(j);
    for (j = 0; j < C_SYNTHFILTER_PARAMETERS; j++)
        lv[EFX_SYNTHFILTER][j] = Rack_Effects[EFX_SYNTHFILTER]->getpar(j);
    for (j = 0; j < C_VARYBAND_PARAMETERS; j++)
        lv[EFX_VARYBAND][j] = Rack_Effects[EFX_VARYBAND]->getpar(j);
    for (j = 0; j < C_CONVO_PARAMETERS; j++)
        lv[EFX_CONVOLOTRON][j] = Rack_Effects[EFX_CONVOLOTRON]->getpar(j);
    for (j = 0; j < C_LOOPER_PARAMETERS; j++)
        lv[EFX_LOOPER][j] = Rack_Effects[EFX_LOOPER]->getpar(j);
    for (j = 0; j < C_MUTRO_PARAMETERS; j++)
        lv[EFX_MUTROMOJO][j] = Rack_Effects[EFX_MUTROMOJO]->getpar(j);
    for (j = 0; j < C_ECHOVERSE_PARAMETERS; j++)
        lv[EFX_ECHOVERSE][j] = Rack_Effects[EFX_ECHOVERSE]->getpar(j);
    for (j = 0; j < C_COIL_PARAMETERS; j++)
        lv[EFX_COILCRAFTER][j] = Rack_Effects[EFX_COILCRAFTER]->getpar(j);
    for (j = 0; j < C_SHELF_PARAMETERS; j++)
        lv[EFX_SHELFBOOST][j] = Rack_Effects[EFX_SHELFBOOST]->getpar(j);
    for (j = 0; j < C_VOCODER_PARAMETERS; j++)
        lv[EFX_VOCODER][j] = Rack_Effects[EFX_VOCODER]->getpar(j);
    for (j = 0; j < C_SUSTAIN_PARAMETERS; j++)
        lv[EFX_SUSTAINER][j] = Rack_Effects[EFX_SUSTAINER]->getpar(j);
    for (j = 0; j < C_SEQUENCE_PARAMETERS; j++)
        lv[EFX_SEQUENCE][j] = Rack_Effects[EFX_SEQUENCE]->getpar(j);
    for (j = 0; j < C_SHIFTER_PARAMETERS; j++)
        lv[EFX_SHIFTER][j] = Rack_Effects[EFX_SHIFTER]->getpar(j);
    for (j = 0; j < C_STOMP_PARAMETERS; j++)
        lv[EFX_STOMPBOX][j] = Rack_Effects[EFX_STOMPBOX]->getpar(j);
    for (j = 0; j < C_REVTRON_PARAMETERS; j++)
        lv[EFX_REVERBTRON][j] = Rack_Effects[EFX_REVERBTRON]->getpar(j);
    for (j = 0; j < C_ECHOTRON_PARAMETERS; j++)
        lv[EFX_ECHOTRON][j] = Rack_Effects[EFX_ECHOTRON]->getpar(j);
    for (j = 0; j < C_SHARM_PARAMETERS; j++)
        lv[EFX_STEREOHARM][j] = Rack_Effects[EFX_STEREOHARM]->getpar(j);
    for (j = 0; j < C_COMPBAND_PARAMETERS; j++)
        lv[EFX_COMPBAND][j] = Rack_Effects[EFX_COMPBAND]->getpar(j);
    for (j = 0; j < C_OPTICAL_PARAMETERS; j++)
        lv[EFX_OPTICALTREM][j] = Rack_Effects[EFX_OPTICALTREM]->getpar(j);
    for (j = 0; j < C_VIBE_PARAMETERS; j++)
        lv[EFX_VIBE][j] = Rack_Effects[EFX_VIBE]->getpar(j);
    for (j = 0; j < C_INFINITY_PARAMETERS; j++)
        lv[EFX_INFINITY][j] = Rack_Effects[EFX_INFINITY]->getpar(j);


    for (j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
        lv[EFX_ORDER][j] = efx_order[j];

    for (j = 0; j < C_EQ_PARAMETERS; j++)
        lv[EFX_EQ][j] = Rack_Effects[EFX_EQ]->getpar(j);
#if 0
    for (j = 0; j < 10; j++)
        lv[EFX_EQ][j] = Rack_Effects[EFX_EQ]->getpar(j * 5 + 12);

    lv[EFX_EQ][10] = Rack_Effects[EFX_EQ]->getpar(EQ_Gain);
    lv[EFX_EQ][11] = Rack_Effects[EFX_EQ]->getpar(EQ_Q);
#endif // 0

    for (j = 0; j < 3; j++)
    {
        lv[EFX_PARAMETRIC][0 + j * 3] = Rack_Effects[EFX_PARAMETRIC]->getpar(j * 5 + 11);
        lv[EFX_PARAMETRIC][1 + j * 3] = Rack_Effects[EFX_PARAMETRIC]->getpar(j * 5 + 12);
        lv[EFX_PARAMETRIC][2 + j * 3] = Rack_Effects[EFX_PARAMETRIC]->getpar(j * 5 + 13);
    }
    lv[EFX_PARAMETRIC][9] = Rack_Effects[EFX_PARAMETRIC]->getpar(Parametric_Gain);
    
    for (j = 0; j < C_CABINET_PARAMETERS; j++)
        lv[EFX_CABINET][j] = Rack_Effects[EFX_CABINET]->getpar(j);
#if 0
    lv[EFX_CABINET][0] = Rack_Effects[EFX_CABINET]->Ppreset;
    lv[EFX_CABINET][1] = Rack_Effects[EFX_CABINET]->getpar(Cabinet_Gain);
#endif
    // Copy the lv[][] parameters to the Bank[].lv[][] structure
    for (j = 0; j < C_MAX_EFFECTS; j++)
    {
        for (int k = 0; k < C_NUMBER_PARAMETERS; k++)    // bypass is not copied here
        {
            Bank[i].lv[j][k] = lv[j][k];
        }
    }

    // Copy the current bypass state to the Bank
    for(j = 0; j < C_NUMBER_EFFECTS; j++)
    {
        Bank[i].lv[j][C_BYPASS] = EFX_Bypass[j];
    }

    // Copy MIDI learn to the Bank
    memcpy(Bank[i].XUserMIDI, XUserMIDI, sizeof (XUserMIDI));
}

int
RKR::BigEndian()
{
    long one = 1;
    return !(*((char *) (&one)));
}

void
RKR::copy_IO()
{
    for (int i = 0; i < 62; i++)
    {
        memset(Bank[i].cInput_Gain, 0, sizeof (Bank[i].cInput_Gain));
        sprintf(Bank[i].cInput_Gain, "%f", Bank[i].Input_Gain);
        memset(Bank[i].cMaster_Volume, 0, sizeof (Bank[i].cMaster_Volume));
        sprintf(Bank[i].cMaster_Volume, "%f", Bank[i].Master_Volume);
        memset(Bank[i].cBalance, 0, sizeof (Bank[i].cBalance));
        sprintf(Bank[i].cBalance, "%f", Bank[i].Balance);
    }
}

void
RKR::convert_IO()
{
    for (int i = 0; i < 62; i++)
    {
        sscanf(Bank[i].cInput_Gain, "%f", &Bank[i].Input_Gain);
        if (Bank[i].Input_Gain == 0.0) Bank[i].Input_Gain = 0.5f;

        sscanf(Bank[i].cMaster_Volume, "%f", &Bank[i].Master_Volume);
        if (Bank[i].Master_Volume == 0.0) Bank[i].Master_Volume = 0.5f;

        sscanf(Bank[i].cBalance, "%f", &Bank[i].Balance);
        if (Bank[i].Balance == 0.0) Bank[i].Balance = 1.0f;
    }
}

void
RKR::fix_endianess()
{
    unsigned int data;

    for (int i = 0; i < 62; i++)
    {
        data = Bank[i].Bypass;
        data = SwapFourBytes(data);
        Bank[i].Bypass = data;

        for (int j = 0; j < 70; j++)
        {
            for (int k = 0; k < 20; k++)
            {
                data = Bank[i].lv[j][k];
                data = SwapFourBytes(data);
                Bank[i].lv[j][k] = data;
            }
        }

        for (int j = 0; j < 128; j++)
        {
            for (int k = 0; k < 20; k++)
            {
                data = Bank[i].XUserMIDI[j][k];
                data = SwapFourBytes(data);
                Bank[i].XUserMIDI[j][k] = data;
            }
        }
    }
}

void
RKR::saveskin(char *filename)
{
    FILE *fn;
    char buf[256];
    fn = fopen(filename, "w");
    if (errno == EACCES)
    {
        Error_Handle(3);
        fclose(fn);
        return;
    }

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d\n", swidth, sheight);
    fputs(buf, fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d,%d,%d\n", sback_color, sfore_color, slabel_color, sleds_color);
    fputs(buf, fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%s", BackgroundImage);
    fputs(buf, fn);
    fputs("\n", fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d\n", fontsize, font);
    fputs(buf, fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d\n", sschema);
    fputs(buf, fn);

    fclose(fn);

}

bool
RKR::loadskin(char *filename)
{
    char buf[256];
    FILE *fn;

    if ((fn = fopen(filename, "r")) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        return 0;
    }

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d,%d\n", &swidth, &sheight);

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d,%d,%d,%d\n", &sback_color, &sfore_color, &slabel_color, &sleds_color);

    memset(BackgroundImage, 0, sizeof (BackgroundImage));
    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        load_skin_error(fn);
        return 0;
    }
    for (unsigned int i = 0; i < 256; i++)
    {
        if (buf[i] > 20)
        {
            BackgroundImage[i] = buf[i];
        }
    }

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d,%d\n", &fontsize, &font);

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d\n", &sschema);

    fclose(fn);
    
    return 1; // no errors
}

void
RKR::load_skin_error(FILE *fn)
{
    printf("Load Skin File error\n");
    fclose(fn);
}

void
RKR::dump_preset_names(void)
{
    for (int i = 0; i < 62; i++)
    {
        fprintf
        (
            stderr,
            "RKR_BANK_NAME:%d:%s\n",
            i,
            Bank[i].Preset_Name
        );
    }
}

int
RKR::CheckOldBank(char *filename)
{
    long Length = 0;
    FILE *fs;

    if ((fs = fopen(filename, "r")) != NULL)
    {
        fseek(fs, 0L, SEEK_END);
        Length = ftell(fs);
        fclose(fs);
        if (Length == 993488) return (3);
        if (Length != 1092688) return (1);
        else return (0);
    }

    return (2);
}

void
RKR::ConvertOldFile(char * filename)
{
    char buff[255];
    memset(buff, 0, sizeof (buff));
    sprintf(buff, "rakconvert -c '%s'", filename);
    if (system(buff) == -1)
        fl_alert("Error running rakconvert...\n");
}

void
RKR::ConvertReverbFile(char * filename)
{
    char buff[255];
    memset(buff, 0, sizeof (buff));
    sprintf(buff, "rakverb -i '%s'", filename);
    printf("%s\n", buff);
    if (system(buff) == -1)
        fl_alert("Error running rakverb...\n");
}

void
RKR::SaveIntPreset(int num, char *name)
{
    FILE *fn;
    char tempfile[256];
    char buf[256];
    char sbuf[260];
    memset(tempfile, 0, sizeof (tempfile));
    sprintf(tempfile, "%s%s", getenv("HOME"), "/.rkrintpreset");

    if ((fn = fopen(tempfile, "a")) != NULL)
    {
        memset(buf, 0, sizeof (buf));
        getbuf(buf, num);
        memset(sbuf, 0, sizeof (sbuf));
        sprintf(sbuf, "%d,%s,%s", num, name, buf);
        fputs(sbuf, fn);
        fclose(fn);
    }
}

void
RKR::DelIntPreset(int num, char *name)
{
    FILE *fn;
    FILE *fs;
    char *rname;
    int eff = 0;
    char orden[1024];
    char tempfile[256];
    char tempfile2[256];
    char buf[256];
    char rbuf[256];

    char *sbuf;
    memset(tempfile, 0, sizeof (tempfile));
    memset(tempfile2, 0, sizeof (tempfile2));
    memset(orden, 0, sizeof (orden));

    sprintf(tempfile, "%s%s", getenv("HOME"), "/.rkrintpreset");

    if ((fs = fopen(tempfile, "r")) == NULL) return;

    sprintf(tempfile2, "%s%s", getenv("HOME"), "/.rkrtemp");

    if ((fn = fopen(tempfile2, "w")) != NULL)
    {
        memset(buf, 0, sizeof (buf));
        while (fgets(buf, sizeof buf, fs) != NULL)
        {
            sbuf = buf;
            memset(rbuf, 0, sizeof (rbuf));
            sprintf(rbuf, "%s", buf);
            sscanf(buf, "%d", &eff);
            rname = strsep(&sbuf, ",");
            rname = strsep(&sbuf, ",");
            if ((eff == num)&&(strcmp(rname, name) == 0))
            {
                continue;
            }
            else fputs(rbuf, fn);
            memset(buf, 0, sizeof (buf));
        }
    }

    fclose(fs);
    fclose(fn);

    sprintf(orden, "mv %s %s\n", tempfile2, tempfile);

    if (system(orden) == -1)
        fl_alert("Error removing internal preset");
}

/**
 *  This adds to the list of internal presets that are user defined. It can result in
 *  duplicate preset names for the same effect. FIXME
 * 
 * @param filename
 *      The file that holds the internal presets to be added.
 * 
 * @return 
 *      Zero(0) if error.
 *      One(1) on success.
 */
bool
RKR::MergeIntPreset(char *filename)
{
    FILE *fn;
    if ((fn = fopen(filename, "r")) == NULL)
    {
        fl_alert("Error reading %s file!\n", filename);
        return 0;
    }
    
    fclose(fn);
        
    char orden[1024];
    char tempfile[256];
    char tempfile2[256];

    memset(tempfile, 0, sizeof (tempfile));
    memset(tempfile2, 0, sizeof (tempfile2));
    memset(orden, 0, sizeof (orden));

    sprintf(tempfile, "%s%s", getenv("HOME"), "/.rkrintpreset");
    sprintf(tempfile2, "%s%s", getenv("HOME"), "/.rkrtemp");

    sprintf(orden, "cat %s '%s' > %s\n", tempfile, filename, tempfile2);
    
    if (system(orden) == -1)
    {
        fl_alert("Error merging internal presets!");
        return 0;
    }
    
    memset(orden, 0, sizeof (orden));

    sprintf(orden, "mv %s %s\n", tempfile2, tempfile);
    if (system(orden) == -1)
    {
        fl_alert("Error merging internal presets!");
        return 0;
    }

    return 1;
}

void
RKR::savemiditable(char *filename)
{
    FILE *fn;
    char buf[256];
    fn = fopen(filename, "w");

    if (errno == EACCES)
    {
        Error_Handle(3);
        fclose(fn);
        return;
    }

    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        sprintf(buf, "%d,%d\n", M_table[i].bank, M_table[i].preset);
        fputs(buf, fn);
    }

    fclose(fn);
}

void
RKR::loadmiditable(char *filename)
{
    char buf[256];
    FILE *fn;

    if ((fn = fopen(filename, "r")) == NULL)
    {
        fl_alert("Error loading %s file!\n", filename);
        return;
    }

    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            fl_alert("Error loading %s file!\n", filename);
            break;
        }

        sscanf(buf, "%d,%d\n", &M_table[i].bank, &M_table[i].preset);
    }

    fclose(fn);
}

