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


const int presets_default[C_NUM_EFFECTS_PLUS_ORDER][C_MAX_PARAMETER_SIZE] = {
    //Reverb
    {80, 64, 63, 24, 0, 0, 0, 4002, 27, 83, 1, 64, 0, 0, 0, 0, 0, 0, 0},
    //Echo
    {67, 64, 565, 64, 30, 59, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Chorus
    {64, 64, 33, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Flanger
    {64, 64, 39, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Phaser
    {64, 64, 11, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0},
    //Overdrive
    {84, 64, 35, 56, 40, 0, 0, 6703, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Distorsion
    {0, 64, 0, 87, 14, 6, 0, 3134, 157, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //EQ1
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0},
    //EQ2 Parametric
    {200, 64, 64, 800, 64, 64, 12000, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Compressor
    {-30, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Order
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}, // EFX_Index!!!
    //WahWah
    {64, 64, 80, 0, 0, 64, 70, 90, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //AlienWah1
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
    //Shuffle 1
    {64, 10, 0, 0, 0, 600, 1200, 2000, 6000, -14, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //Synthfilter
    {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0, 0, 0, 0},
    //VaryBand
    {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Convolotron 1
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
               &lv[LV_REVERB][0], &lv[LV_REVERB][1], &lv[LV_REVERB][2], &lv[LV_REVERB][3], &lv[LV_REVERB][4],
               &lv[LV_REVERB][5], &lv[LV_REVERB][6], &lv[LV_REVERB][7], &lv[LV_REVERB][8], &lv[LV_REVERB][9],
               &lv[LV_REVERB][10], &lv[LV_REVERB][11], &Reverb_B);
        break;

    case EFX_ECHO:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_ECHO][0], &lv[LV_ECHO][1], &lv[LV_ECHO][2], &lv[LV_ECHO][3], &lv[LV_ECHO][4],
               &lv[LV_ECHO][5], &lv[LV_ECHO][6], &lv[LV_ECHO][7], &lv[LV_ECHO][8], &Echo_B);
        break;

    case EFX_CHORUS:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_CHORUS][0], &lv[LV_CHORUS][1], &lv[LV_CHORUS][2], &lv[LV_CHORUS][3], &lv[LV_CHORUS][4],
               &lv[LV_CHORUS][5], &lv[LV_CHORUS][6], &lv[LV_CHORUS][7], &lv[LV_CHORUS][8], &lv[LV_CHORUS][9],
               &lv[LV_CHORUS][10], &lv[LV_CHORUS][11], &lv[LV_CHORUS][12], &Chorus_B);
        break;

    case EFX_FLANGER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_FLANGER][0], &lv[LV_FLANGER][1], &lv[LV_FLANGER][2], &lv[LV_FLANGER][3], &lv[LV_FLANGER][4],
               &lv[LV_FLANGER][5], &lv[LV_FLANGER][6], &lv[LV_FLANGER][7], &lv[LV_FLANGER][8], &lv[LV_FLANGER][9],
               &lv[LV_FLANGER][10], &lv[LV_FLANGER][11], &lv[LV_FLANGER][12], &Flanger_B);
        break;

    case EFX_PHASER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_PHASER][0], &lv[LV_PHASER][1], &lv[LV_PHASER][2], &lv[LV_PHASER][3], &lv[LV_PHASER][4],
               &lv[LV_PHASER][5], &lv[LV_PHASER][6], &lv[LV_PHASER][7], &lv[LV_PHASER][8], &lv[LV_PHASER][9],
               &lv[LV_PHASER][10], &lv[LV_PHASER][11], &Phaser_B);
        break;

    case EFX_OVERDRIVE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_OVERDRIVE][0], &lv[LV_OVERDRIVE][1], &lv[LV_OVERDRIVE][2], &lv[LV_OVERDRIVE][3], &lv[LV_OVERDRIVE][4],
               &lv[LV_OVERDRIVE][5], &lv[LV_OVERDRIVE][6], &lv[LV_OVERDRIVE][7], &lv[LV_OVERDRIVE][8], &lv[LV_OVERDRIVE][9],
               &lv[LV_OVERDRIVE][10], &lv[LV_OVERDRIVE][11], &lv[LV_OVERDRIVE][12], &Overdrive_B);
        break;

    case EFX_DISTORTION:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_DISTORTION][0], &lv[LV_DISTORTION][1], &lv[LV_DISTORTION][2], &lv[LV_DISTORTION][3], &lv[LV_DISTORTION][4],
               &lv[LV_DISTORTION][5], &lv[LV_DISTORTION][6], &lv[LV_DISTORTION][7], &lv[LV_DISTORTION][8], &lv[LV_DISTORTION][9],
               &lv[LV_DISTORTION][10], &lv[LV_DISTORTION][11], &lv[LV_DISTORTION][12], &Distorsion_B);
        break;

    case EFX_EQ:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_EQ][0], &lv[LV_EQ][1], &lv[LV_EQ][2], &lv[LV_EQ][3], &lv[LV_EQ][4],
               &lv[LV_EQ][5], &lv[LV_EQ][6], &lv[LV_EQ][7], &lv[LV_EQ][8], &lv[LV_EQ][9],
               &lv[LV_EQ][10], &lv[LV_EQ][11], &EQ1_B);
        break;

    case EFX_PARAMETRIC:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_PARAMETRIC][0], &lv[LV_PARAMETRIC][1], &lv[LV_PARAMETRIC][2], &lv[LV_PARAMETRIC][3], &lv[LV_PARAMETRIC][4],
               &lv[LV_PARAMETRIC][5], &lv[LV_PARAMETRIC][6], &lv[LV_PARAMETRIC][7], &lv[LV_PARAMETRIC][8], &lv[LV_PARAMETRIC][9],
               &EQ2_B);
        break;

    case EFX_COMPRESSOR:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_COMPRESSOR][0], &lv[LV_COMPRESSOR][1], &lv[LV_COMPRESSOR][2], &lv[LV_COMPRESSOR][3], &lv[LV_COMPRESSOR][4],
               &lv[LV_COMPRESSOR][5], &lv[LV_COMPRESSOR][6], &lv[LV_COMPRESSOR][7], &lv[LV_COMPRESSOR][8], &Compressor_B);
        break;

    case EFX_WAHWAH:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_WAHWAH][0], &lv[LV_WAHWAH][1], &lv[LV_WAHWAH][2], &lv[LV_WAHWAH][3], &lv[LV_WAHWAH][4],
               &lv[LV_WAHWAH][5], &lv[LV_WAHWAH][6], &lv[LV_WAHWAH][7], &lv[LV_WAHWAH][8], &lv[LV_WAHWAH][9],
               &lv[LV_WAHWAH][10], &WahWah_B);
        break;

    case EFX_ALIENWAH:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_ALIENWAH][0], &lv[LV_ALIENWAH][1], &lv[LV_ALIENWAH][2], &lv[LV_ALIENWAH][3], &lv[LV_ALIENWAH][4],
               &lv[LV_ALIENWAH][5], &lv[LV_ALIENWAH][6], &lv[LV_ALIENWAH][7], &lv[LV_ALIENWAH][8], &lv[LV_ALIENWAH][9],
               &lv[LV_ALIENWAH][10], &Alienwah_B);
        break;

    case EFX_CABINET:
        sscanf(buf, "%d,%d,%d\n", &lv[LV_CABINET][0], &lv[LV_CABINET][1], &Cabinet_B);
        break;

    case EFX_PAN:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_PAN][0], &lv[LV_PAN][1], &lv[LV_PAN][2], &lv[LV_PAN][3], &lv[LV_PAN][4],
               &lv[LV_PAN][5], &lv[LV_PAN][6], &lv[LV_PAN][7], &lv[LV_PAN][8], &Pan_B);
        break;

    case EFX_HARMONIZER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_HARMONIZER][0], &lv[LV_HARMONIZER][1], &lv[LV_HARMONIZER][2], &lv[LV_HARMONIZER][3], &lv[LV_HARMONIZER][4],
               &lv[LV_HARMONIZER][5], &lv[LV_HARMONIZER][6], &lv[LV_HARMONIZER][7], &lv[LV_HARMONIZER][8], &lv[LV_HARMONIZER][9],
               &lv[LV_HARMONIZER][10], &Harmonizer_B);
        break;

    case EFX_MUSICAL_DELAY:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_MUSICAL_DELAY][0], &lv[LV_MUSICAL_DELAY][1], &lv[LV_MUSICAL_DELAY][2], &lv[LV_MUSICAL_DELAY][3], &lv[LV_MUSICAL_DELAY][4],
               &lv[LV_MUSICAL_DELAY][5], &lv[LV_MUSICAL_DELAY][6], &lv[LV_MUSICAL_DELAY][7], &lv[LV_MUSICAL_DELAY][8], &lv[LV_MUSICAL_DELAY][9],
               &lv[LV_MUSICAL_DELAY][10], &lv[LV_MUSICAL_DELAY][11], &lv[LV_MUSICAL_DELAY][12], &MusDelay_B);
        break;

    case EFX_NOISEGATE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_NOISEGATE][0], &lv[LV_NOISEGATE][1], &lv[LV_NOISEGATE][2], &lv[LV_NOISEGATE][3], &lv[LV_NOISEGATE][4],
               &lv[LV_NOISEGATE][5], &lv[LV_NOISEGATE][6], &Gate_B);

        break;

    case EFX_DERELICT:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_DERELICT][0], &lv[LV_DERELICT][1], &lv[LV_DERELICT][2], &lv[LV_DERELICT][3], &lv[LV_DERELICT][4],
               &lv[LV_DERELICT][5], &lv[LV_DERELICT][6], &lv[LV_DERELICT][7], &lv[LV_DERELICT][8], &lv[LV_DERELICT][9],
               &lv[LV_DERELICT][10], &lv[LV_DERELICT][11], &Derelict_B);
        break;

    case EFX_ANALOG_PHASER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_ANALOG_PHASER][0], &lv[LV_ANALOG_PHASER][1], &lv[LV_ANALOG_PHASER][2], &lv[LV_ANALOG_PHASER][3], &lv[LV_ANALOG_PHASER][4],
               &lv[LV_ANALOG_PHASER][5], &lv[LV_ANALOG_PHASER][6], &lv[LV_ANALOG_PHASER][7], &lv[LV_ANALOG_PHASER][8], &lv[LV_ANALOG_PHASER][9],
               &lv[LV_ANALOG_PHASER][10], &lv[LV_ANALOG_PHASER][11], &APhaser_B);
        break;

    case EFX_VALVE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_VALVE][0], &lv[LV_VALVE][1], &lv[LV_VALVE][2], &lv[LV_VALVE][3], &lv[LV_VALVE][4],
               &lv[LV_VALVE][5], &lv[LV_VALVE][6], &lv[LV_VALVE][7], &lv[LV_VALVE][8], &lv[LV_VALVE][9],
               &lv[LV_VALVE][10], &lv[LV_VALVE][11], &lv[LV_VALVE][12], &Valve_B);
        break;

    case EFX_DUAL_FLANGE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_DUAL_FLANGE][0], &lv[LV_DUAL_FLANGE][1], &lv[LV_DUAL_FLANGE][2], &lv[LV_DUAL_FLANGE][3], &lv[LV_DUAL_FLANGE][4],
               &lv[LV_DUAL_FLANGE][5], &lv[LV_DUAL_FLANGE][6], &lv[LV_DUAL_FLANGE][7], &lv[LV_DUAL_FLANGE][8], &lv[LV_DUAL_FLANGE][9],
               &lv[LV_DUAL_FLANGE][10], &lv[LV_DUAL_FLANGE][11], &lv[LV_DUAL_FLANGE][12], &lv[LV_DUAL_FLANGE][13], &lv[LV_DUAL_FLANGE][14],
               &DFlange_B);
        break;

    case EFX_RING:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_RING][0], &lv[LV_RING][1], &lv[LV_RING][2], &lv[LV_RING][3], &lv[LV_RING][4],
               &lv[LV_RING][5], &lv[LV_RING][6], &lv[LV_RING][7], &lv[LV_RING][8], &lv[LV_RING][9],
               &lv[LV_RING][10], &lv[LV_RING][11], &lv[LV_RING][12], &Ring_B);
        break;

    case EFX_EXCITER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_EXCITER][0], &lv[LV_EXCITER][1], &lv[LV_EXCITER][2], &lv[LV_EXCITER][3], &lv[LV_EXCITER][4],
               &lv[LV_EXCITER][5], &lv[LV_EXCITER][6], &lv[LV_EXCITER][7], &lv[LV_EXCITER][8], &lv[LV_EXCITER][9],
               &lv[LV_EXCITER][10], &lv[LV_EXCITER][11], &lv[LV_EXCITER][12], &Exciter_B);
        break;

    case EFX_DISTBAND:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_DISTBAND][0], &lv[LV_DISTBAND][1], &lv[LV_DISTBAND][2], &lv[LV_DISTBAND][3], &lv[LV_DISTBAND][4],
               &lv[LV_DISTBAND][5], &lv[LV_DISTBAND][6], &lv[LV_DISTBAND][7], &lv[LV_DISTBAND][8], &lv[LV_DISTBAND][9],
               &lv[LV_DISTBAND][10], &lv[LV_DISTBAND][11], &lv[LV_DISTBAND][12], &lv[LV_DISTBAND][13], &lv[LV_DISTBAND][14],
               &DistBand_B);
        break;

    case EFX_ARPIE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_ARPIE][0], &lv[LV_ARPIE][1], &lv[LV_ARPIE][2], &lv[LV_ARPIE][3], &lv[LV_ARPIE][4],
               &lv[LV_ARPIE][5], &lv[LV_ARPIE][6], &lv[LV_ARPIE][7], &lv[LV_ARPIE][8], &lv[LV_ARPIE][9],
               &lv[LV_ARPIE][10], &Arpie_B);
        break;

    case EFX_EXPANDER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_EXPANDER][0], &lv[LV_EXPANDER][1], &lv[LV_EXPANDER][2], &lv[LV_EXPANDER][3], &lv[LV_EXPANDER][4],
               &lv[LV_EXPANDER][5], &lv[LV_EXPANDER][6], &Expander_B);

        break;

    case EFX_SHUFFLE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_SHUFFLE][0], &lv[LV_SHUFFLE][1], &lv[LV_SHUFFLE][2], &lv[LV_SHUFFLE][3], &lv[LV_SHUFFLE][4],
               &lv[LV_SHUFFLE][5], &lv[LV_SHUFFLE][6], &lv[LV_SHUFFLE][7], &lv[LV_SHUFFLE][8], &lv[LV_SHUFFLE][9],
               &lv[LV_SHUFFLE][10], &Shuffle_B);
        break;

    case EFX_SYNTHFILTER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_SYNTHFILTER][0], &lv[LV_SYNTHFILTER][1], &lv[LV_SYNTHFILTER][2], &lv[LV_SYNTHFILTER][3], &lv[LV_SYNTHFILTER][4],
               &lv[LV_SYNTHFILTER][5], &lv[LV_SYNTHFILTER][6], &lv[LV_SYNTHFILTER][7], &lv[LV_SYNTHFILTER][8], &lv[LV_SYNTHFILTER][9],
               &lv[LV_SYNTHFILTER][10], &lv[LV_SYNTHFILTER][11], &lv[LV_SYNTHFILTER][12], &lv[LV_SYNTHFILTER][13], &lv[LV_SYNTHFILTER][14],
               &lv[LV_SYNTHFILTER][15], &Synthfilter_B);
        break;

    case EFX_VARYBAND:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_VARYBAND][0], &lv[LV_VARYBAND][1], &lv[LV_VARYBAND][2], &lv[LV_VARYBAND][3], &lv[LV_VARYBAND][4],
               &lv[LV_VARYBAND][5], &lv[LV_VARYBAND][6], &lv[LV_VARYBAND][7], &lv[LV_VARYBAND][8], &lv[LV_VARYBAND][9],
               &lv[LV_VARYBAND][10], &VaryBand_B);
        break;

    case EFX_CONVOLOTRON:
        memset(efx_Convol->Filename, 0, sizeof (efx_Convol->Filename));
        memset(cfilename, 0, sizeof (char) * 128);
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[LV_CONVOLOTRON][0], &lv[LV_CONVOLOTRON][1], &lv[LV_CONVOLOTRON][2], &lv[LV_CONVOLOTRON][3], &lv[LV_CONVOLOTRON][4],
               &lv[LV_CONVOLOTRON][5], &lv[LV_CONVOLOTRON][6], &lv[LV_CONVOLOTRON][7], &lv[LV_CONVOLOTRON][8], &lv[LV_CONVOLOTRON][9],
               &lv[LV_CONVOLOTRON][10], &Convol_B, cfilename);
        strcpy(efx_Convol->Filename, cfilename);
        break;

    case EFX_LOOPER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_LOOPER][0], &lv[LV_LOOPER][1], &lv[LV_LOOPER][2], &lv[LV_LOOPER][3], &lv[LV_LOOPER][4],
               &lv[LV_LOOPER][5], &lv[LV_LOOPER][6], &lv[LV_LOOPER][7], &lv[LV_LOOPER][8], &lv[LV_LOOPER][9],
               &lv[LV_LOOPER][10], &lv[LV_LOOPER][11], &lv[LV_LOOPER][12], &lv[LV_LOOPER][13], &Looper_B);
        break;

    case EFX_MUTROMOJO:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_MUTROMOJO][0], &lv[LV_MUTROMOJO][1], &lv[LV_MUTROMOJO][2], &lv[LV_MUTROMOJO][3], &lv[LV_MUTROMOJO][4],
               &lv[LV_MUTROMOJO][5], &lv[LV_MUTROMOJO][6], &lv[LV_MUTROMOJO][7], &lv[LV_MUTROMOJO][8], &lv[LV_MUTROMOJO][9],
               &lv[LV_MUTROMOJO][10], &lv[LV_MUTROMOJO][11], &lv[LV_MUTROMOJO][12], &lv[LV_MUTROMOJO][13], &lv[LV_MUTROMOJO][14],
               &lv[LV_MUTROMOJO][15], &lv[LV_MUTROMOJO][16], &lv[LV_MUTROMOJO][17], &MuTroMojo_B);
        break;

    case EFX_ECHOVERSE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_ECHOVERSE][0], &lv[LV_ECHOVERSE][1], &lv[LV_ECHOVERSE][2], &lv[LV_ECHOVERSE][3], &lv[LV_ECHOVERSE][4],
               &lv[LV_ECHOVERSE][5], &lv[LV_ECHOVERSE][6], &lv[LV_ECHOVERSE][7], &lv[LV_ECHOVERSE][8], &lv[LV_ECHOVERSE][9], &Echoverse_B);
        break;

    case EFX_COILCRAFTER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_COILCRAFTER][0], &lv[LV_COILCRAFTER][1], &lv[LV_COILCRAFTER][2], &lv[LV_COILCRAFTER][3], &lv[LV_COILCRAFTER][4],
               &lv[LV_COILCRAFTER][5], &lv[LV_COILCRAFTER][6], &lv[LV_COILCRAFTER][7], &lv[LV_COILCRAFTER][8], &CoilCrafter_B);
        break;

    case EFX_SHELFBOOST:
        sscanf(buf, "%d,%d,%d,%d,%d,%d\n",
               &lv[LV_SHELFBOOST][0], &lv[LV_SHELFBOOST][1], &lv[LV_SHELFBOOST][2], &lv[LV_SHELFBOOST][3], &lv[LV_SHELFBOOST][4],
               &ShelfBoost_B);
        break;

    case EFX_VOCODER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_VOCODER][0], &lv[LV_VOCODER][1], &lv[LV_VOCODER][2], &lv[LV_VOCODER][3], &lv[LV_VOCODER][4],
               &lv[LV_VOCODER][5], &lv[LV_VOCODER][6], &Vocoder_B);
        break;

    case EFX_SUSTAINER:
        sscanf(buf, "%d,%d,%d\n",
               &lv[LV_SUSTAINER][0], &lv[LV_SUSTAINER][1], &Sustainer_B);
        break;

    case EFX_SEQUENCE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_SEQUENCE][0], &lv[LV_SEQUENCE][1], &lv[LV_SEQUENCE][2], &lv[LV_SEQUENCE][3], &lv[LV_SEQUENCE][4],
               &lv[LV_SEQUENCE][5], &lv[LV_SEQUENCE][6], &lv[LV_SEQUENCE][7], &lv[LV_SEQUENCE][8], &lv[LV_SEQUENCE][9],
               &lv[LV_SEQUENCE][10], &lv[LV_SEQUENCE][11], &lv[LV_SEQUENCE][12], &lv[LV_SEQUENCE][13], &lv[LV_SEQUENCE][14], &Sequence_B);
        break;

    case EFX_SHIFTER:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_SHIFTER][0], &lv[LV_SHIFTER][1], &lv[LV_SHIFTER][2], &lv[LV_SHIFTER][3], &lv[LV_SHIFTER][4],
               &lv[LV_SHIFTER][5], &lv[LV_SHIFTER][6], &lv[LV_SHIFTER][7], &lv[LV_SHIFTER][8], &lv[LV_SHIFTER][9], &Shifter_B);
        break;

    case EFX_STOMPBOX:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_STOMPBOX][0], &lv[LV_STOMPBOX][1], &lv[LV_STOMPBOX][2], &lv[LV_STOMPBOX][3], &lv[LV_STOMPBOX][4],
               &lv[LV_STOMPBOX][5], &StompBox_B);
        break;

    case EFX_REVERBTRON:
        memset(efx_Reverbtron->Filename, 0, sizeof (efx_Reverbtron->Filename));
        memset(cfilename, 0, sizeof (char) * 128);
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[LV_REVERBTRON][0], &lv[LV_REVERBTRON][1], &lv[LV_REVERBTRON][2], &lv[LV_REVERBTRON][3], &lv[LV_REVERBTRON][4],
               &lv[LV_REVERBTRON][5], &lv[LV_REVERBTRON][6], &lv[LV_REVERBTRON][7], &lv[LV_REVERBTRON][8], &lv[LV_REVERBTRON][9],
               &lv[LV_REVERBTRON][10], &lv[LV_REVERBTRON][11], &lv[LV_REVERBTRON][12], &lv[LV_REVERBTRON][13], &lv[LV_REVERBTRON][14], &lv[LV_REVERBTRON][15],
               &Reverbtron_B,
               cfilename);
        strcpy(efx_Reverbtron->Filename, cfilename);
        break;

    case EFX_ECHOTRON:
        memset(efx_Echotron->Filename, 0, sizeof (efx_Echotron->Filename));
        memset(cfilename, 0, sizeof (char) * 128);
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[LV_ECHOTRON][0], &lv[LV_ECHOTRON][1], &lv[LV_ECHOTRON][2], &lv[LV_ECHOTRON][3], &lv[LV_ECHOTRON][4],
               &lv[LV_ECHOTRON][5], &lv[LV_ECHOTRON][6], &lv[LV_ECHOTRON][7], &lv[LV_ECHOTRON][8], &lv[LV_ECHOTRON][9],
               &lv[LV_ECHOTRON][10], &lv[LV_ECHOTRON][11], &lv[LV_ECHOTRON][12], &lv[LV_ECHOTRON][13], &lv[LV_ECHOTRON][14], &lv[LV_ECHOTRON][15],
               &Echotron_B,
               cfilename);
        strcpy(efx_Echotron->Filename, cfilename);
        break;

    case EFX_STEREOHARM:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_STEREOHARM][0], &lv[LV_STEREOHARM][1], &lv[LV_STEREOHARM][2], &lv[LV_STEREOHARM][3], &lv[LV_STEREOHARM][4],
               &lv[LV_STEREOHARM][5], &lv[LV_STEREOHARM][6], &lv[LV_STEREOHARM][7], &lv[LV_STEREOHARM][8], &lv[LV_STEREOHARM][9],
               &lv[LV_STEREOHARM][10], &lv[LV_STEREOHARM][11], &StereoHarm_B);
        break;

    case EFX_COMPBAND:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_COMPBAND][0], &lv[LV_COMPBAND][1], &lv[LV_COMPBAND][2], &lv[LV_COMPBAND][3], &lv[LV_COMPBAND][4],
               &lv[LV_COMPBAND][5], &lv[LV_COMPBAND][6], &lv[LV_COMPBAND][7], &lv[LV_COMPBAND][8], &lv[LV_COMPBAND][9],
               &lv[LV_COMPBAND][10], &lv[LV_COMPBAND][11], &lv[LV_COMPBAND][12], &CompBand_B);
        break;

    case EFX_OPTICALTREM:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_OPTICALTREM][0], &lv[LV_OPTICALTREM][1], &lv[LV_OPTICALTREM][2], &lv[LV_OPTICALTREM][3], &lv[LV_OPTICALTREM][4],
               &lv[LV_OPTICALTREM][5], &Opticaltrem_B);
        break;

    case EFX_VIBE:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_VIBE][0], &lv[LV_VIBE][1], &lv[LV_VIBE][2], &lv[LV_VIBE][3], &lv[LV_VIBE][4],
               &lv[LV_VIBE][5], &lv[LV_VIBE][6], &lv[LV_VIBE][7], &lv[LV_VIBE][8], &lv[LV_VIBE][9], &lv[LV_VIBE][10],
               &Vibe_B);
        break;

    case EFX_INFINITY:
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[LV_INFINITY][0], &lv[LV_INFINITY][1], &lv[LV_INFINITY][2], &lv[LV_INFINITY][3], &lv[LV_INFINITY][4],
               &lv[LV_INFINITY][5], &lv[LV_INFINITY][6], &lv[LV_INFINITY][7], &lv[LV_INFINITY][8], &lv[LV_INFINITY][9],
               &lv[LV_INFINITY][10], &lv[LV_INFINITY][11], &lv[LV_INFINITY][12], &lv[LV_INFINITY][13], &lv[LV_INFINITY][14],
               &lv[LV_INFINITY][15], &lv[LV_INFINITY][16], &lv[LV_INFINITY][17], &Infinity_B);
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
                efx_Rev->getpar(Reverb_DryWet), efx_Rev->getpar(Reverb_Pan),
                efx_Rev->getpar(Reverb_Time), efx_Rev->getpar(Reverb_I_Delay),
                efx_Rev->getpar(Reverb_Delay_FB), efx_Rev->getpar(Reverb_SKIP_5),
                efx_Rev->getpar(Reverb_SKIP_6), efx_Rev->getpar(Reverb_LPF),
                efx_Rev->getpar(Reverb_HPF), efx_Rev->getpar(Reverb_Damp),
                efx_Rev->getpar(Reverb_Type), efx_Rev->getpar(Reverb_Room), Reverb_Bypass);
        break;

    case EFX_ECHO:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Echo->getpar(Echo_DryWet), efx_Echo->getpar(Echo_Pan),
                efx_Echo->getpar(Echo_Delay), efx_Echo->getpar(Echo_LR_Delay),
                efx_Echo->getpar(Echo_LR_Cross), efx_Echo->getpar(Echo_Feedback),
                efx_Echo->getpar(Echo_Damp), efx_Echo->getpar(Echo_Reverse),
                efx_Echo->getpar(Echo_Direct), Echo_Bypass);
        break;

    case EFX_CHORUS:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Chorus->getpar(Chorus_DryWet), efx_Chorus->getpar(Chorus_Pan),
                efx_Chorus->getpar(Chorus_LFO_Tempo), efx_Chorus->getpar(Chorus_LFO_Random),
                efx_Chorus->getpar(Chorus_LFO_Type), efx_Chorus->getpar(Chorus_LFO_Stereo),
                efx_Chorus->getpar(Chorus_Depth), efx_Chorus->getpar(Chorus_Delay),
                efx_Chorus->getpar(Chorus_Feedback), efx_Chorus->getpar(Chorus_LR_Cross),
                efx_Chorus->getpar(Chorus_SKIP_Flange_10), efx_Chorus->getpar(Chorus_Subtract),
                efx_Chorus->getpar(Chorus_Intense), Chorus_Bypass);
        break;

    case EFX_FLANGER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Flanger->getpar(Flanger_DryWet), efx_Flanger->getpar(Flanger_Pan),
                efx_Flanger->getpar(Flanger_LFO_Tempo), efx_Flanger->getpar(Flanger_LFO_Random),
                efx_Flanger->getpar(Flanger_LFO_Type), efx_Flanger->getpar(Flanger_LFO_Stereo),
                efx_Flanger->getpar(Flanger_Depth), efx_Flanger->getpar(Flanger_Delay),
                efx_Flanger->getpar(Flanger_Feedback), efx_Flanger->getpar(Flanger_LR_Cross),
                efx_Flanger->getpar(Flanger_SKIP_Flange_10), efx_Flanger->getpar(Flanger_Subtract),
                efx_Flanger->getpar(Flanger_Intense), Flanger_Bypass);
        break;

    case EFX_PHASER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Phaser->getpar(Phaser_DryWet), efx_Phaser->getpar(Phaser_Pan),
                efx_Phaser->getpar(Phaser_LFO_Tempo), efx_Phaser->getpar(Phaser_LFO_Random),
                efx_Phaser->getpar(Phaser_LFO_Type), efx_Phaser->getpar(Phaser_LFO_Stereo),
                efx_Phaser->getpar(Phaser_Depth), efx_Phaser->getpar(Phaser_Feedback),
                efx_Phaser->getpar(Phaser_Stages), efx_Phaser->getpar(Phaser_LR_Cross),
                efx_Phaser->getpar(Phaser_Subtract), efx_Phaser->getpar(Phaser_Phase),
                Phaser_Bypass);
        break;

    case EFX_OVERDRIVE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Overdrive->getpar(Overdrive_DryWet), efx_Overdrive->getpar(Overdrive_Pan),
                efx_Overdrive->getpar(Overdrive_LR_Cross), efx_Overdrive->getpar(Overdrive_Drive),
                efx_Overdrive->getpar(Overdrive_Level), efx_Overdrive->getpar(Overdrive_Type),
                efx_Overdrive->getpar(Overdrive_Negate), efx_Overdrive->getpar(Overdrive_LPF),
                efx_Overdrive->getpar(Overdrive_HPF), efx_Overdrive->getpar(Overdrive_Stereo),
                efx_Overdrive->getpar(Overdrive_Prefilter), efx_Overdrive->getpar(Overdrive_SKIP_11),
                efx_Overdrive->getpar(Overdrive_Suboctave), Overdrive_Bypass);
        break;

    case EFX_DISTORTION:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Distorsion->getpar(Dist_DryWet), efx_Distorsion->getpar(Dist_Pan),
                efx_Distorsion->getpar(Dist_LR_Cross), efx_Distorsion->getpar(Dist_Drive),
                efx_Distorsion->getpar(Dist_Level), efx_Distorsion->getpar(Dist_Type),
                efx_Distorsion->getpar(Dist_Negate), efx_Distorsion->getpar(Dist_LPF),
                efx_Distorsion->getpar(Dist_HPF), efx_Distorsion->getpar(Dist_Stereo),
                efx_Distorsion->getpar(Dist_Prefilter), efx_Distorsion->getpar(Dist_SKIP_11),
                efx_Distorsion->getpar(Dist_Suboctave), Distorsion_Bypass);
        break;

    case EFX_EQ:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_EQ1->getpar(EQ_31_HZ), efx_EQ1->getpar(EQ_63_HZ),
                efx_EQ1->getpar(EQ_125_HZ), efx_EQ1->getpar(EQ_250_HZ),
                efx_EQ1->getpar(EQ_500_HZ), efx_EQ1->getpar(EQ_1_KHZ),
                efx_EQ1->getpar(EQ_2_KHZ), efx_EQ1->getpar(EQ_4_KHZ),
                efx_EQ1->getpar(EQ_8_KHZ), efx_EQ1->getpar(EQ_16_KHZ),
                efx_EQ1->getpar(EQ_Gain), efx_EQ1->getpar(EQ_Q), EQ1_Bypass);
        break;

    case EFX_PARAMETRIC:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_EQ2->getpar(Parametric_Low_Freq), efx_EQ2->getpar(Parametric_Low_Gain),
                efx_EQ2->getpar(Parametric_Low_Q), efx_EQ2->getpar(Parametric_Mid_Freq),
                efx_EQ2->getpar(Parametric_Mid_Gain), efx_EQ2->getpar(Parametric_Mid_Q),
                efx_EQ2->getpar(Parametric_High_Freq), efx_EQ2->getpar(Parametric_High_Gain),
                efx_EQ2->getpar(Parametric_High_Q), efx_EQ2->getpar(Parametric_Gain),
                EQ2_Bypass);
        break;

    case EFX_COMPRESSOR:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Compressor->getpar(Compress_Threshold), efx_Compressor->getpar(Compress_Ratio),
                efx_Compressor->getpar(Compress_Output), efx_Compressor->getpar(Compress_Attack),
                efx_Compressor->getpar(Compress_Release), efx_Compressor->getpar(Compress_Auto_Out),
                efx_Compressor->getpar(Compress_Knee), efx_Compressor->getpar(Compress_Stereo),
                efx_Compressor->getpar(Compress_Peak), Compressor_Bypass);
        break;


    case EFX_WAHWAH:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_WahWah->getpar(WahWah_DryWet), efx_WahWah->getpar(WahWah_Pan),
                efx_WahWah->getpar(WahWah_LFO_Tempo), efx_WahWah->getpar(WahWah_LFO_Random),
                efx_WahWah->getpar(WahWah_LFO_Type), efx_WahWah->getpar(WahWah_LFO_Stereo),
                efx_WahWah->getpar(WahWah_Depth), efx_WahWah->getpar(WahWah_Sense),
                efx_WahWah->getpar(WahWah_ASI), efx_WahWah->getpar(WahWah_Smooth),
                efx_WahWah->getpar(WahWah_Mode), WahWah_Bypass);
        break;

    case EFX_ALIENWAH:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Alienwah->getpar(Alien_DryWet), efx_Alienwah->getpar(Alien_Pan),
                efx_Alienwah->getpar(Alien_LFO_Tempo), efx_Alienwah->getpar(Alien_LFO_Random),
                efx_Alienwah->getpar(Alien_LFO_Type), efx_Alienwah->getpar(Alien_LFO_Stereo),
                efx_Alienwah->getpar(Alien_Depth), efx_Alienwah->getpar(Alien_Feedback),
                efx_Alienwah->getpar(Alien_Delay), efx_Alienwah->getpar(Alien_LR_Cross),
                efx_Alienwah->getpar(Alien_Phase), Alienwah_Bypass);
        break;

    case EFX_CABINET:
        sprintf(buf, "%d,%d,%d\n",
                efx_Cabinet->Cabinet_Preset, efx_Cabinet->getpar(Cabinet_Gain), Cabinet_Bypass);
        break;

    case EFX_PAN:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Pan->getpar(Pan_DryWet), efx_Pan->getpar(Pan_Pan),
                efx_Pan->getpar(Pan_LFO_Tempo), efx_Pan->getpar(Pan_LFO_Random),
                efx_Pan->getpar(Pan_LFO_Type), efx_Pan->getpar(Pan_LFO_Stereo),
                efx_Pan->getpar(Pan_Ex_St_Amt), efx_Pan->getpar(Pan_AutoPan),
                efx_Pan->getpar(Pan_Enable_Extra), Pan_Bypass);
        break;

    case EFX_HARMONIZER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Har->getpar(Harm_DryWet), efx_Har->getpar(Harm_Pan),
                efx_Har->getpar(Harm_Gain), efx_Har->getpar(Harm_Interval),
                efx_Har->getpar(Harm_Filter_Freq), efx_Har->getpar(Harm_Select),
                efx_Har->getpar(Harm_Note), efx_Har->getpar(Harm_Chord),
                efx_Har->getpar(Harm_Filter_Gain), efx_Har->getpar(Harm_Filter_Q),
                efx_Har->getpar(Harm_MIDI), Harmonizer_Bypass);
        break;

    case EFX_MUSICAL_DELAY:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_MusDelay->getpar(Music_DryWet), efx_MusDelay->getpar(Music_Pan_1),
                efx_MusDelay->getpar(Music_Delay_1), efx_MusDelay->getpar(Music_Del_Offset),
                efx_MusDelay->getpar(Music_LR_Cross), efx_MusDelay->getpar(Music_Feedback_1),
                efx_MusDelay->getpar(Music_Damp), efx_MusDelay->getpar(Music_Pan_2),
                efx_MusDelay->getpar(Music_Delay_2), efx_MusDelay->getpar(Music_Feedback_2),
                efx_MusDelay->getpar(Music_Tempo), efx_MusDelay->getpar(Music_Gain_1),
                efx_MusDelay->getpar(Music_Gain_2), MusDelay_Bypass);
        break;

    case EFX_NOISEGATE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Gate->getpar(Gate_Threshold), efx_Gate->getpar(Gate_Range),
                efx_Gate->getpar(Gate_Attack), efx_Gate->getpar(Gate_Release),
                efx_Gate->getpar(Gate_LPF), efx_Gate->getpar(Gate_HPF),
                efx_Gate->getpar(Gate_Hold), Gate_Bypass);
        break;

    case EFX_DERELICT:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Derelict->getpar(Dere_DryWet), efx_Derelict->getpar(Dere_Pan),
                efx_Derelict->getpar(Dere_LR_Cross), efx_Derelict->getpar(Dere_Drive),
                efx_Derelict->getpar(Dere_Level), efx_Derelict->getpar(Dere_Type),
                efx_Derelict->getpar(Dere_Negate), efx_Derelict->getpar(Dere_LPF),
                efx_Derelict->getpar(Dere_HPF), efx_Derelict->getpar(Dere_Color),
                efx_Derelict->getpar(Dere_Prefilter), efx_Derelict->getpar(Dere_Suboctave),
                Derelict_Bypass);
        break;

    case EFX_ANALOG_PHASER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_APhaser->getpar(APhase_DryWet), efx_APhaser->getpar(APhase_Distortion),
                efx_APhaser->getpar(APhase_LFO_Tempo), efx_APhaser->getpar(APhase_LFO_Random),
                efx_APhaser->getpar(APhase_LFO_Type), efx_APhaser->getpar(APhase_LFO_Stereo),
                efx_APhaser->getpar(APhase_Width), efx_APhaser->getpar(APhase_Feedback),
                efx_APhaser->getpar(APhase_Stages), efx_APhaser->getpar(APhase_Mismatch),
                efx_APhaser->getpar(APhase_Subtract), efx_APhaser->getpar(APhase_Depth),
                efx_APhaser->getpar(APhase_Hyper), APhaser_Bypass);
        break;

    case EFX_VALVE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Valve->getpar(Valve_DryWet), efx_Valve->getpar(Valve_Pan),
                efx_Valve->getpar(Valve_LR_Cross), efx_Valve->getpar(Valve_Drive),
                efx_Valve->getpar(Valve_Level), efx_Valve->getpar(Valve_Negate),
                efx_Valve->getpar(Valve_LPF), efx_Valve->getpar(Valve_HPF),
                efx_Valve->getpar(Valve_Stereo), efx_Valve->getpar(Valve_Prefilter),
                efx_Valve->getpar(Valve_Distortion), efx_Valve->getpar(Valve_Ex_Dist),
                efx_Valve->getpar(Valve_Presence), Valve_Bypass);
        break;

    case EFX_DUAL_FLANGE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_DFlange->getpar(DFlange_DryWet), efx_DFlange->getpar(DFlange_Pan),
                efx_DFlange->getpar(DFlange_LR_Cross), efx_DFlange->getpar(DFlange_Depth),
                efx_DFlange->getpar(DFlange_Width), efx_DFlange->getpar(DFlange_Offset),
                efx_DFlange->getpar(DFlange_Feedback), efx_DFlange->getpar(DFlange_LPF),
                efx_DFlange->getpar(DFlange_Subtract), efx_DFlange->getpar(DFlange_Zero),
                efx_DFlange->getpar(DFlange_LFO_Tempo), efx_DFlange->getpar(DFlange_LFO_Stereo),
                efx_DFlange->getpar(DFlange_LFO_Type), efx_DFlange->getpar(DFlange_LFO_Random),
                efx_DFlange->getpar(DFlange_Intense), DFlange_Bypass);
        break;

    case EFX_RING:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Ring->getpar(Ring_DryWet), efx_Ring->getpar(Ring_Pan),
                efx_Ring->getpar(Ring_LR_Cross), efx_Ring->getpar(Ring_Level),
                efx_Ring->getpar(Ring_Depth), efx_Ring->getpar(Ring_Freq),
                efx_Ring->getpar(Ring_Stereo), efx_Ring->getpar(Ring_Sine),
                efx_Ring->getpar(Ring_Triangle), efx_Ring->getpar(Ring_Saw),
                efx_Ring->getpar(Ring_Square), efx_Ring->getpar(Ring_Input),
                efx_Ring->getpar(Ring_Auto_Freq), Ring_Bypass);
        break;

    case EFX_EXCITER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Exciter->getpar(Exciter_Gain), efx_Exciter->getpar(Exciter_Harm_1),
                efx_Exciter->getpar(Exciter_Harm_2), efx_Exciter->getpar(Exciter_Harm_3),
                efx_Exciter->getpar(Exciter_Harm_4), efx_Exciter->getpar(Exciter_Harm_5),
                efx_Exciter->getpar(Exciter_Harm_6), efx_Exciter->getpar(Exciter_Harm_7),
                efx_Exciter->getpar(Exciter_Harm_8), efx_Exciter->getpar(Exciter_Harm_9),
                efx_Exciter->getpar(Exciter_Harm_10), efx_Exciter->getpar(Exciter_LPF),
                efx_Exciter->getpar(Exciter_HPF), Exciter_Bypass);
        break;

    case EFX_DISTBAND:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_DistBand->getpar(DistBand_DryWet), efx_DistBand->getpar(DistBand_Pan),
                efx_DistBand->getpar(DistBand_LR_Cross), efx_DistBand->getpar(DistBand_Drive),
                efx_DistBand->getpar(DistBand_Level), efx_DistBand->getpar(DistBand_Type_Low),
                efx_DistBand->getpar(DistBand_Type_Mid), efx_DistBand->getpar(DistBand_Type_Hi),
                efx_DistBand->getpar(DistBand_Gain_Low), efx_DistBand->getpar(DistBand_Gain_Mid),
                efx_DistBand->getpar(DistBand_Gain_Hi), efx_DistBand->getpar(DistBand_Negate),
                efx_DistBand->getpar(DistBand_Cross_1), efx_DistBand->getpar(DistBand_Cross_2),
                efx_DistBand->getpar(DistBand_Stereo), DistBand_Bypass);
        break;

    case EFX_ARPIE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Arpie->getpar(Arpie_DryWet), efx_Arpie->getpar(Arpie_Pan),
                efx_Arpie->getpar(Arpie_Tempo), efx_Arpie->getpar(Arpie_LR_Delay),
                efx_Arpie->getpar(Arpie_LR_Cross), efx_Arpie->getpar(Arpie_Feedback),
                efx_Arpie->getpar(Arpie_Damp), efx_Arpie->getpar(Arpie_ArpeWD),
                efx_Arpie->getpar(Arpie_Harm), efx_Arpie->getpar(Arpie_Pattern),
                efx_Arpie->getpar(Arpie_Subdivision), Arpie_Bypass);
        break;

    case EFX_EXPANDER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Expander->getpar(Expander_Threshold), efx_Expander->getpar(Expander_Shape),
                efx_Expander->getpar(Expander_Attack), efx_Expander->getpar(Expander_Release),
                efx_Expander->getpar(Expander_LPF), efx_Expander->getpar(Expander_HPF),
                efx_Expander->getpar(Expander_Gain), Expander_Bypass);
        break;

    case EFX_SHUFFLE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Shuffle->getpar(Shuffle_DryWet), efx_Shuffle->getpar(Shuffle_Gain_L),
                efx_Shuffle->getpar(Shuffle_Gain_ML), efx_Shuffle->getpar(Shuffle_Gain_MH),
                efx_Shuffle->getpar(Shuffle_Gain_H), efx_Shuffle->getpar(Shuffle_Freq_L),
                efx_Shuffle->getpar(Shuffle_Freq_ML), efx_Shuffle->getpar(Shuffle_Freq_MH),
                efx_Shuffle->getpar(Shuffle_Freq_H), efx_Shuffle->getpar(Shuffle_Width),
                efx_Shuffle->getpar(Shuffle_F_Band), Shuffle_Bypass);
        break;

    case EFX_SYNTHFILTER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Synthfilter->getpar(Synthfilter_DryWet), efx_Synthfilter->getpar(Synthfilter_Distort),
                efx_Synthfilter->getpar(Synthfilter_LFO_Tempo), efx_Synthfilter->getpar(Synthfilter_LFO_Random),
                efx_Synthfilter->getpar(Synthfilter_LFO_Type), efx_Synthfilter->getpar(Synthfilter_LFO_Stereo),
                efx_Synthfilter->getpar(Synthfilter_Width), efx_Synthfilter->getpar(Synthfilter_Feedback),
                efx_Synthfilter->getpar(Synthfilter_LPF_Stages), efx_Synthfilter->getpar(Synthfilter_HPF_Stages),
                efx_Synthfilter->getpar(Synthfilter_Subtract), efx_Synthfilter->getpar(Synthfilter_Depth),
                efx_Synthfilter->getpar(Synthfilter_Env_Sens), efx_Synthfilter->getpar(Synthfilter_Attack),
                efx_Synthfilter->getpar(Synthfilter_Release), efx_Synthfilter->getpar(Synthfilter_Offset),
                Synthfilter_Bypass);
        break;

    case EFX_VARYBAND:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_VaryBand->getpar(VaryBand_DryWet), efx_VaryBand->getpar(VaryBand_LFO_Tempo_1),
                efx_VaryBand->getpar(VaryBand_LFO_Type_1), efx_VaryBand->getpar(VaryBand_LFO_Stereo_1),
                efx_VaryBand->getpar(VaryBand_LFO_Tempo_2), efx_VaryBand->getpar(VaryBand_LFO_Type_2),
                efx_VaryBand->getpar(VaryBand_LFO_Stereo_2), efx_VaryBand->getpar(VaryBand_Cross_1),
                efx_VaryBand->getpar(VaryBand_Cross_2), efx_VaryBand->getpar(VaryBand_Cross_3),
                efx_VaryBand->getpar(VaryBand_Combination), VaryBand_Bypass);
        break;

    case EFX_CONVOLOTRON:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                efx_Convol->getpar(Convo_DryWet), efx_Convol->getpar(Convo_Pan),
                efx_Convol->getpar(Convo_Safe), efx_Convol->getpar(Convo_Length),
                efx_Convol->getpar(Convo_User_File), efx_Convol->getpar(Convo_SKIP_5),
                efx_Convol->getpar(Convo_Damp), efx_Convol->getpar(Convo_Level),
                efx_Convol->getpar(Convo_Set_File), efx_Convol->getpar(Convo_SKIP_9),
                efx_Convol->getpar(Convo_Feedback), Convol_Bypass, efx_Convol->Filename);
        break;

    case EFX_LOOPER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Looper->getpar(Looper_DryWet), efx_Looper->getpar(Looper_Play),
                efx_Looper->getpar(Looper_Stop), efx_Looper->getpar(Looper_Record),
                efx_Looper->getpar(Looper_Clear), efx_Looper->getpar(Looper_Reverse),
                efx_Looper->getpar(Looper_Level_1), efx_Looper->getpar(Looper_Track_1),
                efx_Looper->getpar(Looper_Track_2), efx_Looper->getpar(Looper_AutoPlay),
                efx_Looper->getpar(Looper_Level_2), efx_Looper->getpar(Looper_Rec_1),
                efx_Looper->getpar(Looper_Rec_2), efx_Looper->getpar(Looper_Link), Looper_Bypass);
        break;

    case EFX_MUTROMOJO:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_MuTroMojo->getpar(MuTro_DryWet), efx_MuTroMojo->getpar(MuTro_Resonance),
                efx_MuTroMojo->getpar(MuTro_LFO_Tempo), efx_MuTroMojo->getpar(MuTro_LFO_Random),
                efx_MuTroMojo->getpar(MuTro_LFO_Type), efx_MuTroMojo->getpar(MuTro_LFO_Stereo),
                efx_MuTroMojo->getpar(MuTro_Depth), efx_MuTroMojo->getpar(MuTro_Env_Sens),
                efx_MuTroMojo->getpar(MuTro_Wah), efx_MuTroMojo->getpar(MuTro_Env_Smooth),
                efx_MuTroMojo->getpar(MuTro_LowPass), efx_MuTroMojo->getpar(MuTro_BandPass),
                efx_MuTroMojo->getpar(MuTro_HighPass), efx_MuTroMojo->getpar(MuTro_Stages),
                efx_MuTroMojo->getpar(MuTro_Range), efx_MuTroMojo->getpar(MuTro_St_Freq),
                efx_MuTroMojo->getpar(MuTro_Mod_Res), efx_MuTroMojo->getpar(Mutro_Mode_Legacy),
                efx_MuTroMojo->getpar(Mutro_Preset), MuTroMojo_Bypass);

        break;

    case EFX_ECHOVERSE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Echoverse->getpar(Echoverse_DryWet), efx_Echoverse->getpar(Echoverse_Pan),
                efx_Echoverse->getpar(Echoverse_Tempo), efx_Echoverse->getpar(Echoverse_LR_Delay),
                efx_Echoverse->getpar(Echoverse_Angle), efx_Echoverse->getpar(Echoverse_Feedback),
                efx_Echoverse->getpar(Echoverse_Damp), efx_Echoverse->getpar(Echoverse_Reverse),
                efx_Echoverse->getpar(Echoverse_Subdivision), efx_Echoverse->getpar(Echoverse_Ext_Stereo),
                Echoverse_Bypass);
        break;

    case EFX_COILCRAFTER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_CoilCrafter->getpar(Coil_Gain), efx_CoilCrafter->getpar(Coil_Origin),
                efx_CoilCrafter->getpar(Coil_Destiny), efx_CoilCrafter->getpar(Coil_Freq_1),
                efx_CoilCrafter->getpar(Coil_Q_1), efx_CoilCrafter->getpar(Coil_Freq_2),
                efx_CoilCrafter->getpar(Coil_Q_2), efx_CoilCrafter->getpar(Coil_Tone),
                efx_CoilCrafter->getpar(Coil_NeckMode), CoilCrafter_Bypass);
        break;

    case EFX_SHELFBOOST:
        sprintf(buf, "%d,%d,%d,%d,%d,%d\n",
                efx_ShelfBoost->getpar(Shelf_Gain), efx_ShelfBoost->getpar(Shelf_Presence),
                efx_ShelfBoost->getpar(Shelf_Tone), efx_ShelfBoost->getpar(Shelf_Stereo),
                efx_ShelfBoost->getpar(Shelf_Level), ShelfBoost_Bypass);
        break;

    case EFX_VOCODER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Vocoder->getpar(Vocoder_DryWet), efx_Vocoder->getpar(Vocoder_Pan),
                efx_Vocoder->getpar(Vocoder_Smear), efx_Vocoder->getpar(Vocoder_Q),
                efx_Vocoder->getpar(Vocoder_Input), efx_Vocoder->getpar(Vocoder_Level),
                efx_Vocoder->getpar(Vocoder_Ring), Vocoder_Bypass);
        break;

    case EFX_SUSTAINER:
        sprintf(buf, "%d,%d,%d\n",
                efx_Sustainer->getpar(Sustain_Gain), efx_Sustainer->getpar(Sustain_Sustain),
                Sustainer_Bypass);
        break;

    case EFX_SEQUENCE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Sequence->getpar(Sequence_Step_1), efx_Sequence->getpar(Sequence_Step_2),
                efx_Sequence->getpar(Sequence_Step_3), efx_Sequence->getpar(Sequence_Step_4),
                efx_Sequence->getpar(Sequence_Step_5), efx_Sequence->getpar(Sequence_Step_6),
                efx_Sequence->getpar(Sequence_Step_7), efx_Sequence->getpar(Sequence_Step_8),
                efx_Sequence->getpar(Sequence_DryWet), efx_Sequence->getpar(Sequence_Tempo),
                efx_Sequence->getpar(Sequence_Resonance), efx_Sequence->getpar(Sequence_Amp),
                efx_Sequence->getpar(Sequence_Stdf), efx_Sequence->getpar(Sequence_Mode),
                efx_Sequence->getpar(Sequence_Range), Sequence_Bypass);
        break;

    case EFX_SHIFTER:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Shifter->getpar(Shifter_DryWet), efx_Shifter->getpar(Shifter_Pan),
                efx_Shifter->getpar(Shifter_Gain), efx_Shifter->getpar(Shifter_Attack),
                efx_Shifter->getpar(Shifter_Decay), efx_Shifter->getpar(Shifter_Threshold),
                efx_Shifter->getpar(Shifter_Interval), efx_Shifter->getpar(Shifter_Shift),
                efx_Shifter->getpar(Shifter_Mode), efx_Shifter->getpar(Shifter_Whammy), Shifter_Bypass);
        break;


    case EFX_STOMPBOX:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
                efx_StompBox->getpar(Stomp_Level), efx_StompBox->getpar(Stomp_Tone),
                efx_StompBox->getpar(Stomp_Mid), efx_StompBox->getpar(Stomp_Bias),
                efx_StompBox->getpar(Stomp_Gain), efx_StompBox->getpar(Stomp_Mode),
                StompBox_Bypass);
        break;

    case EFX_REVERBTRON:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                efx_Reverbtron->getpar(Revtron_DryWet), efx_Reverbtron->getpar(Revtron_Fade),
                efx_Reverbtron->getpar(Revtron_Safe), efx_Reverbtron->getpar(Revtron_Length),
                efx_Reverbtron->getpar(Revtron_User_File), efx_Reverbtron->getpar(Revtron_I_Delay),
                efx_Reverbtron->getpar(Revtron_Damp), efx_Reverbtron->getpar(Revtron_Level),
                efx_Reverbtron->getpar(Revtron_Set_File), efx_Reverbtron->getpar(Revtron_Stretch),
                efx_Reverbtron->getpar(Revtron_Feedback), efx_Reverbtron->getpar(Revtron_Pan),
                efx_Reverbtron->getpar(Revtron_Ex_Stereo), efx_Reverbtron->getpar(Revtron_Shuffle),
                efx_Reverbtron->getpar(Revtron_LPF), efx_Reverbtron->getpar(Revtron_Diffusion),
                Reverbtron_Bypass, efx_Reverbtron->Filename);
        break;

    case EFX_ECHOTRON:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                efx_Echotron->getpar(Echotron_DryWet), efx_Echotron->getpar(Echotron_Depth),
                efx_Echotron->getpar(Echotron_LFO_Width), efx_Echotron->getpar(Echotron_Taps),
                efx_Echotron->getpar(Echotron_User_File), efx_Echotron->getpar(Echotron_Tempo),
                efx_Echotron->getpar(Echotron_Damp), efx_Echotron->getpar(Echotron_LR_Cross),
                efx_Echotron->getpar(Echotron_Set_File), efx_Echotron->getpar(Echotron_LFO_Stereo),
                efx_Echotron->getpar(Echotron_Feedback), efx_Echotron->getpar(Echotron_Pan),
                efx_Echotron->getpar(Echotron_Mod_Delay), efx_Echotron->getpar(Echotron_Mod_Filter),
                efx_Echotron->getpar(Echotron_LFO_Type), efx_Echotron->getpar(Echotron_Filters),
                Echotron_Bypass, efx_Echotron->Filename);
        break;

    case EFX_STEREOHARM:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_StereoHarm->getpar(Sharm_DryWet), efx_StereoHarm->getpar(Sharm_L_Gain),
                efx_StereoHarm->getpar(Sharm_L_Interval), efx_StereoHarm->getpar(Sharm_L_Chroma),
                efx_StereoHarm->getpar(Sharm_R_Gain), efx_StereoHarm->getpar(Sharm_R_Interval),
                efx_StereoHarm->getpar(Sharm_R_Chroma), efx_StereoHarm->getpar(Sharm_Select),
                efx_StereoHarm->getpar(Sharm_Note), efx_StereoHarm->getpar(Sharm_Chord),
                efx_StereoHarm->getpar(Sharm_MIDI), efx_StereoHarm->getpar(Sharm_LR_Cross),
                StereoHarm_Bypass);
        break;

    case EFX_COMPBAND:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_CompBand->getpar(CompBand_DryWet), efx_CompBand->getpar(CompBand_Low_Ratio),
                efx_CompBand->getpar(CompBand_Mid_1_Ratio), efx_CompBand->getpar(CompBand_Mid_2_Ratio),
                efx_CompBand->getpar(CompBand_High_Ratio), efx_CompBand->getpar(CompBand_Low_Thresh),
                efx_CompBand->getpar(CompBand_Mid_1_Thresh), efx_CompBand->getpar(CompBand_Mid_2_Thresh),
                efx_CompBand->getpar(CompBand_High_Thresh), efx_CompBand->getpar(CompBand_Cross_1),
                efx_CompBand->getpar(CompBand_Cross_2), efx_CompBand->getpar(CompBand_Cross_3),
                efx_CompBand->getpar(CompBand_Gain), CompBand_Bypass);
        break;

    case EFX_OPTICALTREM:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Opticaltrem->getpar(Optical_Depth), efx_Opticaltrem->getpar(Optical_LFO_Tempo),
                efx_Opticaltrem->getpar(Optical_LFO_Random), efx_Opticaltrem->getpar(Optical_LFO_Type),
                efx_Opticaltrem->getpar(Optical_LFO_Stereo), efx_Opticaltrem->getpar(Optical_Pan),
                efx_Opticaltrem->getpar(Optical_Invert), Opticaltrem_Bypass);
        break;


    case EFX_VIBE:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Vibe->getpar(Vibe_Width), efx_Vibe->getpar(Vibe_LFO_Tempo),
                efx_Vibe->getpar(Vibe_LFO_Random), efx_Vibe->getpar(Vibe_LFO_Type),
                efx_Vibe->getpar(Vibe_LFO_Stereo), efx_Vibe->getpar(Vibe_Pan),
                efx_Vibe->getpar(Vibe_DryWet), efx_Vibe->getpar(Vibe_Feedback),
                efx_Vibe->getpar(Vibe_Depth), efx_Vibe->getpar(Vibe_LR_Cross),
                efx_Vibe->getpar(Vibe_Stereo), Vibe_Bypass);
        break;

    case EFX_INFINITY:
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Infinity->getpar(Infinity_DryWet), efx_Infinity->getpar(Infinity_Band_1),
                efx_Infinity->getpar(Infinity_Band_2), efx_Infinity->getpar(Infinity_Band_3),
                efx_Infinity->getpar(Infinity_Band_4), efx_Infinity->getpar(Infinity_Band_5),
                efx_Infinity->getpar(Infinity_Band_6), efx_Infinity->getpar(Infinity_Band_7),
                efx_Infinity->getpar(Infinity_Band_8), efx_Infinity->getpar(Infinity_Resonance),
                efx_Infinity->getpar(Infinity_Start), efx_Infinity->getpar(Infinity_End),
                efx_Infinity->getpar(Infinity_Tempo), efx_Infinity->getpar(Infinity_LR_Delay),
                efx_Infinity->getpar(Infinity_Subdivision), efx_Infinity->getpar(Infinity_AutoPan),
                efx_Infinity->getpar(Infinity_Reverse), efx_Infinity->getpar(Infinity_Stages),
                Infinity_Bypass);
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
           &lv[LV_ORDER][0], &lv[LV_ORDER][1], &lv[LV_ORDER][2], &lv[LV_ORDER][3], &lv[LV_ORDER][4],
           &lv[LV_ORDER][5], &lv[LV_ORDER][6], &lv[LV_ORDER][7], &lv[LV_ORDER][8], &lv[LV_ORDER][9]);

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
        efx_order[i] = lv[LV_ORDER][i];
    }

    Harmonizer_Bypass = 0;
    Ring_Bypass = 0;
    StereoHarm_Bypass = 0;

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
            EQ1_Bypass = 0;
            efx_EQ1->cleanup();
            for (i = 0; i < 10; i++)
            {
                efx_EQ1->changepar(i * 5 + 12, lv[LV_EQ][i]);
                efx_EQ1->changepar(i * 5 + 13, lv[LV_EQ][11]);
            }
            efx_EQ1->changepar(EQ_Gain, lv[LV_EQ][10]);
            EQ1_Bypass = EQ1_B;
            break;

        case EFX_COMPRESSOR:
            Compressor_Bypass = 0;
            efx_Compressor->cleanup();
            for (i = 0; i < C_COMPRESS_PARAMETERS; i++)
                efx_Compressor->changepar(i, lv[LV_COMPRESSOR][i]);
            Compressor_Bypass = Compressor_B;
            break;

        case EFX_DISTORTION:
            Distorsion_Bypass = 0;
            efx_Distorsion->cleanup();
            for (i = 0; i < C_DIST_PARAMETERS; i++)
                efx_Distorsion->changepar(i, lv[LV_DISTORTION][i]);
            Distorsion_Bypass = Distorsion_B;
            break;

        case EFX_OVERDRIVE:
            Overdrive_Bypass = 0;
            efx_Overdrive->cleanup();
            for (i = 0; i < C_OVERDRIVE_PARAMETERS; i++)
                efx_Overdrive->changepar(i, lv[LV_OVERDRIVE][i]);
            Overdrive_Bypass = Overdrive_B;
            break;

        case EFX_ECHO:
            Echo_Bypass = 0;
            efx_Echo->cleanup();
            for (i = 0; i < C_ECHO_PARAMETERS; i++)
                efx_Echo->changepar(i, lv[LV_ECHO][i]);
            Echo_Bypass = Echo_B;
            break;

        case EFX_CHORUS:
            Chorus_Bypass = 0;
            efx_Chorus->cleanup();
            for (i = 0; i < C_CHORUS_PARAMETERS; i++)
                efx_Chorus->changepar(i, lv[LV_CHORUS][i]);
            Chorus_Bypass = Chorus_B;
            break;

        case EFX_PHASER:
            Phaser_Bypass = 0;
            efx_Phaser->cleanup();
            for (i = 0; i < C_PHASER_PARAMETERS; i++)
                efx_Phaser->changepar(i, lv[LV_PHASER][i]);
            Phaser_Bypass = Phaser_B;
            break;

        case EFX_FLANGER:
            Flanger_Bypass = 0;
            efx_Flanger->cleanup();
            for (i = 0; i < C_FLANGER_PARAMETERS; i++)
                efx_Flanger->changepar(i, lv[LV_FLANGER][i]);
            Flanger_Bypass = Flanger_B;
            break;

        case EFX_REVERB:
            Reverb_Bypass = 0;
            efx_Rev->cleanup();
            for (i = 0; i < C_REVERB_PARAMETERS; i++)
                efx_Rev->changepar(i, lv[LV_REVERB][i]);
            Reverb_Bypass = Reverb_B;
            break;

        case EFX_PARAMETRIC:
            EQ2_Bypass = 0;
            efx_EQ2->cleanup();
            for (i = 0; i < 3; i++)
            {
                efx_EQ2->changepar(i * 5 + 11, lv[LV_PARAMETRIC][0 + i * 3]);
                efx_EQ2->changepar(i * 5 + 12, lv[LV_PARAMETRIC][1 + i * 3]);
                efx_EQ2->changepar(i * 5 + 13, lv[LV_PARAMETRIC][2 + i * 3]);
            }
            efx_EQ2->changepar(Parametric_Gain, lv[LV_PARAMETRIC][9]);
            EQ2_Bypass = EQ2_B;
            break;

        case EFX_WAHWAH:
            WahWah_Bypass = 0;
            efx_WahWah->cleanup();
            for (i = 0; i < C_WAHWAH_PARAMETERS; i++)
                efx_WahWah->changepar(i, lv[LV_WAHWAH][i]);
            WahWah_Bypass = WahWah_B;
            break;

        case EFX_ALIENWAH:
            Alienwah_Bypass = 0;
            efx_Alienwah->cleanup();
            for (i = 0; i < C_ALIENWAH_PARAMETERS; i++)
                efx_Alienwah->changepar(i, lv[LV_ALIENWAH][i]);
            Alienwah_Bypass = Alienwah_B;
            break;

        case EFX_CABINET:
            Cabinet_Bypass = 0;
            efx_Cabinet->cleanup();
            efx_Cabinet->setpreset(lv[LV_CABINET][0]);
            efx_Cabinet->changepar(Cabinet_Gain, lv[LV_CABINET][1]);
            Cabinet_Bypass = Cabinet_B;
            break;

        case EFX_PAN:
            Pan_Bypass = 0;
            efx_Pan->cleanup();
            for (i = 0; i < C_PAN_PARAMETERS; i++)
                efx_Pan->changepar(i, lv[LV_PAN][i]);
            Pan_Bypass = Pan_B;
            break;

        case EFX_HARMONIZER:
            Harmonizer_Bypass = 0;
            efx_Har->cleanup();
            for (i = 0; i < C_HARM_PARAMETERS; i++)
                efx_Har->changepar(i, lv[LV_HARMONIZER][i]);
            Harmonizer_Bypass = Harmonizer_B;
            break;

        case EFX_MUSICAL_DELAY:
            MusDelay_Bypass = 0;
            efx_MusDelay->cleanup();
            for (i = 0; i < C_MUSIC_PARAMETERS; i++)
                efx_MusDelay->changepar(i, lv[LV_MUSICAL_DELAY][i]);
            MusDelay_Bypass = MusDelay_B;
            break;

        case EFX_NOISEGATE:
            Gate_Bypass = 0;
            efx_Gate->cleanup();
            for (i = 0; i < C_GATE_PARAMETERS; i++)
                efx_Gate->changepar(i, lv[LV_NOISEGATE][i]);
            Gate_Bypass = Gate_B;
            break;

        case EFX_DERELICT:
            Derelict_Bypass = 0;
            efx_Derelict->cleanup();
            for (i = 0; i < C_DERE_PARAMETERS; i++)
                efx_Derelict->changepar(i, lv[LV_DERELICT][i]);
            Derelict_Bypass = Derelict_B;
            break;

        case EFX_ANALOG_PHASER:
            APhaser_Bypass = 0;
            efx_APhaser->cleanup();
            for (i = 0; i < C_APHASER_PARAMETERS; i++)
                efx_APhaser->changepar(i, lv[LV_ANALOG_PHASER][i]);
            APhaser_Bypass = APhaser_B;
            break;

        case EFX_VALVE:
            Valve_Bypass = 0;
            efx_Valve->cleanup();
            for (i = 0; i < C_VALVE_PARAMETERS; i++)
                efx_Valve->changepar(i, lv[LV_VALVE][i]);
            Valve_Bypass = Valve_B;
            break;

        case EFX_DUAL_FLANGE:
            DFlange_Bypass = 0;
            efx_DFlange->cleanup();
            for (i = 0; i < C_DFLANGE_PARAMETERS; i++)
                efx_DFlange->changepar(i, lv[LV_DUAL_FLANGE][i]);
            DFlange_Bypass = DFlange_B;
            break;

        case EFX_RING:
            Ring_Bypass = 0;
            efx_Ring->cleanup();
            for (i = 0; i < C_RING_PARAMETERS; i++)
                efx_Ring->changepar(i, lv[LV_RING][i]);
            Ring_Bypass = Ring_B;
            break;

        case EFX_EXCITER:
            Exciter_Bypass = 0;
            efx_Exciter->cleanup();
            for (i = 0; i < C_EXCITER_PARAMETERS; i++)
                efx_Exciter->changepar(i, lv[LV_EXCITER][i]);
            Exciter_Bypass = Exciter_B;
            break;

        case EFX_DISTBAND:
            DistBand_Bypass = 0;
            efx_DistBand->cleanup();
            for (i = 0; i < C_DBAND_PARAMETERS; i++)
                efx_DistBand->changepar(i, lv[LV_DISTBAND][i]);
            DistBand_Bypass = DistBand_B;
            break;

        case EFX_ARPIE:
            Arpie_Bypass = 0;
            efx_Arpie->cleanup();
            for (i = 0; i < C_ARPIE_PARAMETERS; i++)
                efx_Arpie->changepar(i, lv[LV_ARPIE][i]);
            Arpie_Bypass = Arpie_B;
            break;

        case EFX_EXPANDER:
            Expander_Bypass = 0;
            efx_Expander->cleanup();
            for (i = 0; i < C_EXPANDER_PARAMETERS; i++)
                efx_Expander->changepar(i, lv[LV_EXPANDER][i]);
            Expander_Bypass = Expander_B;
            break;

        case EFX_SHUFFLE:
            Shuffle_Bypass = 0;
            efx_Shuffle->cleanup();
            for (i = 0; i < C_SHUFFLE_PARAMETERS; i++)
                efx_Shuffle->changepar(i, lv[LV_SHUFFLE][i]);
            Shuffle_Bypass = Shuffle_B;
            break;

        case EFX_SYNTHFILTER:
            Synthfilter_Bypass = 0;
            efx_Synthfilter->cleanup();
            for (i = 0; i < C_SYNTHFILTER_PARAMETERS; i++)
                efx_Synthfilter->changepar(i, lv[LV_SYNTHFILTER][i]);
            Synthfilter_Bypass = Synthfilter_B;
            break;

        case EFX_VARYBAND:
            VaryBand_Bypass = 0;
            efx_VaryBand->cleanup();
            for (i = 0; i < C_VARYBAND_PARAMETERS; i++)
                efx_VaryBand->changepar(i, lv[LV_VARYBAND][i]);
            VaryBand_Bypass = VaryBand_B;
            break;

        case EFX_CONVOLOTRON:
            Convol_Bypass = 0;
            efx_Convol->cleanup();
            for (i = 0; i < C_CONVO_PARAMETERS; i++)
                efx_Convol->changepar(i, lv[LV_CONVOLOTRON][i]);
            Convol_Bypass = Convol_B;
            break;

        case EFX_LOOPER:
            Looper_Bypass = 0;
            // efx_Looper->cleanup();
            for (i = 0; i < C_LOOPER_PARAMETERS; i++)
                efx_Looper->changepar(i, lv[LV_LOOPER][i]);
            Looper_Bypass = Looper_B;
            break;

        case EFX_MUTROMOJO:
            MuTroMojo_Bypass = 0;
            efx_MuTroMojo->cleanup();
            for (i = 0; i < C_MUTRO_PARAMETERS; i++)
                efx_MuTroMojo->changepar(i, lv[LV_MUTROMOJO][i]);
            MuTroMojo_Bypass = MuTroMojo_B;
            break;

        case EFX_ECHOVERSE:
            Echoverse_Bypass = 0;
            efx_Echoverse->cleanup();
            for (i = 0; i < C_ECHOVERSE_PARAMETERS; i++)
                efx_Echoverse->changepar(i, lv[LV_ECHOVERSE][i]);
            Echoverse_Bypass = Echoverse_B;
            break;

        case EFX_COILCRAFTER:
            CoilCrafter_Bypass = 0;
            efx_CoilCrafter->cleanup();
            for (i = 0; i < C_COIL_PARAMETERS; i++)
                efx_CoilCrafter->changepar(i, lv[LV_COILCRAFTER][i]);
            CoilCrafter_Bypass = CoilCrafter_B;
            break;

        case EFX_SHELFBOOST:
            ShelfBoost_Bypass = 0;
            efx_ShelfBoost->cleanup();
            for (i = 0; i < C_SHELF_PARAMETERS; i++)
                efx_ShelfBoost->changepar(i, lv[LV_SHELFBOOST][i]);
            ShelfBoost_Bypass = ShelfBoost_B;
            break;

        case EFX_VOCODER:
            Vocoder_Bypass = 0;
            efx_Vocoder->cleanup();
            for (i = 0; i < C_VOCODER_PARAMETERS; i++)
                efx_Vocoder->changepar(i, lv[LV_VOCODER][i]);
            Vocoder_Bypass = Vocoder_B;
            break;

        case EFX_SUSTAINER:
            Sustainer_Bypass = 0;
            efx_Sustainer->cleanup();
            for (i = 0; i < C_SUSTAIN_PARAMETERS; i++)
                efx_Sustainer->changepar(i, lv[LV_SUSTAINER][i]);
            Sustainer_Bypass = Sustainer_B;
            break;

        case EFX_SEQUENCE:
            Sequence_Bypass = 0;
            efx_Sequence->cleanup();
            for (i = 0; i < C_SEQUENCE_PARAMETERS; i++)
                efx_Sequence->changepar(i, lv[LV_SEQUENCE][i]);
            Sequence_Bypass = Sequence_B;
            break;

        case EFX_SHIFTER:
            Shifter_Bypass = 0;
            efx_Shifter->cleanup();
            for (i = 0; i < C_SHIFTER_PARAMETERS; i++)
                efx_Shifter->changepar(i, lv[LV_SHIFTER][i]);
            Shifter_Bypass = Shifter_B;
            break;

        case EFX_STOMPBOX:
            StompBox_Bypass = 0;
            efx_StompBox->cleanup();
            for (i = 0; i < C_STOMP_PARAMETERS; i++)
                efx_StompBox->changepar(i, lv[LV_STOMPBOX][i]);
            StompBox_Bypass = StompBox_B;
            break;

        case EFX_REVERBTRON:
            Reverbtron_Bypass = 0;
            efx_Reverbtron->cleanup();
            for (i = 0; i < C_REVTRON_PARAMETERS; i++)
                efx_Reverbtron->changepar(i, lv[LV_REVERBTRON][i]);
            Reverbtron_Bypass = Reverbtron_B;
            break;

        case EFX_ECHOTRON:
            Echotron_Bypass = 0;
            efx_Echotron->cleanup();
            for (i = 0; i < C_ECHOTRON_PARAMETERS; i++)
                efx_Echotron->changepar(i, lv[LV_ECHOTRON][i]);
            Echotron_Bypass = Echotron_B;
            break;

        case EFX_STEREOHARM:
            StereoHarm_Bypass = 0;
            efx_StereoHarm->cleanup();
            for (i = 0; i < C_SHARM_PARAMETERS; i++)
                efx_StereoHarm->changepar(i, lv[LV_STEREOHARM][i]);
            if (lv[LV_STEREOHARM][10]) RC_Stereo_Harm->cleanup();
            StereoHarm_Bypass = StereoHarm_B;
            break;

        case EFX_COMPBAND:
            CompBand_Bypass = 0;
            efx_CompBand->cleanup();
            for (i = 0; i < C_COMPBAND_PARAMETERS; i++)
                efx_CompBand->changepar(i, lv[LV_COMPBAND][i]);
            CompBand_Bypass = CompBand_B;
            break;

        case EFX_OPTICALTREM:
            Opticaltrem_Bypass = 0;
            efx_Opticaltrem->cleanup();
            for (i = 0; i < C_OPTICAL_PARAMETERS; i++)
                efx_Opticaltrem->changepar(i, lv[LV_OPTICALTREM][i]);
            Opticaltrem_Bypass = Opticaltrem_B;
            break;

        case EFX_VIBE:
            Vibe_Bypass = 0;
            efx_Vibe->cleanup();
            for (i = 0; i < C_VIBE_PARAMETERS; i++)
                efx_Vibe->changepar(i, lv[LV_VIBE][i]);
            Vibe_Bypass = Vibe_B;
            break;

        case EFX_INFINITY:
            Infinity_Bypass = 0;
            efx_Infinity->cleanup();
            for (i = 0; i < C_INFINITY_PARAMETERS; i++)
                efx_Infinity->changepar(i, lv[LV_INFINITY][i]);
            Infinity_Bypass = Infinity_B;
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
    memset(efx_Convol->Filename, 0, sizeof (efx_Convol->Filename));
    memset(efx_Reverbtron->Filename, 0, sizeof (efx_Reverbtron->Filename));
    memset(efx_Echotron->Filename, 0, sizeof (efx_Echotron->Filename));
    memset(Author, 0, sizeof (char) * 64);
    strcpy(Author, UserRealName);
    Input_Gain = .5f;
    Master_Volume = .5f;
    Fraction_Bypass = 1.0f;
    Bypass = 0;
    memset(lv, 0, sizeof (lv));

    for (int j = 0; j < C_NUM_EFFECTS_PLUS_ORDER; j++)
    {
        for (int k = 0; k < C_MAX_PARAMETER_SIZE; k++)
        {
            lv[j][k] = presets_default[j][k];
        }

        lv[j][19] = 0;
    }

    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        efx_order[k] = presets_default[10][k]; // magic number 10 is order
    }

    Reverb_B = 0;
    Echo_B = 0;
    Chorus_B = 0;
    Flanger_B = 0;
    Phaser_B = 0;
    Overdrive_B = 0;
    Distorsion_B = 0;
    EQ1_B = 0;
    EQ2_B = 0;
    Compressor_B = 0;
    WahWah_B = 0;
    Alienwah_B = 0;
    Cabinet_B = 0;
    Pan_B = 0;
    Harmonizer_B = 0;
    MusDelay_B = 0;
    Gate_B = 0;
    Derelict_B = 0;
    APhaser_B = 0;
    Valve_B = 0;
    DFlange_B = 0;
    Ring_B = 0;
    Exciter_B = 0;
    DistBand_B = 0;
    Arpie_B = 0;
    Expander_B = 0;
    Shuffle_B = 0;
    Synthfilter_B = 0;
    VaryBand_B = 0;
    Convol_B = 0;
    Looper_B = 0;
    MuTroMojo_B = 0;
    Echoverse_B = 0;
    CoilCrafter_B = 0;
    ShelfBoost_B = 0;
    Vocoder_B = 0;
    Sustainer_B = 0;
    Sequence_B = 0;
    Shifter_B = 0;
    StompBox_B = 0;
    Reverbtron_B = 0;
    Echotron_B = 0;
    StereoHarm_B = 0;
    CompBand_B = 0;
    Opticaltrem_B = 0;
    Vibe_B = 0;
    Infinity_B = 0;

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

        for (int j = 0; j < C_NUM_EFFECTS_PLUS_ORDER; j++)
        {
            for (int k = 0; k < C_MAX_PARAMETER_SIZE; k++)
            {
                Bank[i].lv[j][k] = presets_default[j][k];
            }
            Bank[i].lv[j][19] = 0;
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
    memset(efx_Convol->Filename, 0, sizeof (efx_Convol->Filename));
    strcpy(efx_Convol->Filename, Bank[i].ConvoFiname);
    memset(efx_Reverbtron->Filename, 0, sizeof (efx_Reverbtron->Filename));
    strcpy(efx_Reverbtron->Filename, Bank[i].RevFiname);
    memset(efx_Echotron->Filename, 0, sizeof (efx_Echotron->Filename));
    strcpy(efx_Echotron->Filename, Bank[i].EchoFiname);

    for (int j = 0; j < C_NUM_EFFECTS_PLUS_ORDER; j++)
    {
        for (int k = 0; k < 20; k++)
        {
            lv[j][k] = Bank[i].lv[j][k];
        }
    }

    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        efx_order[k] = Bank[i].lv[LV_ORDER][k];
    }

    Reverb_B = Bank[i].lv[LV_REVERB][19];
    Echo_B = Bank[i].lv[LV_ECHO][19];
    Chorus_B = Bank[i].lv[LV_CHORUS][19];
    Flanger_B = Bank[i].lv[LV_FLANGER][19];
    Phaser_B = Bank[i].lv[LV_PHASER][19];
    Overdrive_B = Bank[i].lv[LV_OVERDRIVE][19];
    Distorsion_B = Bank[i].lv[LV_DISTORTION][19];
    EQ1_B = Bank[i].lv[LV_EQ][19];
    EQ2_B = Bank[i].lv[LV_PARAMETRIC][19];
    Compressor_B = Bank[i].lv[LV_COMPRESSOR][19];
    WahWah_B = Bank[i].lv[LV_WAHWAH][19];
    Alienwah_B = Bank[i].lv[LV_ALIENWAH][19];
    Cabinet_B = Bank[i].lv[LV_CABINET][19];
    Pan_B = Bank[i].lv[LV_PAN][19];
    Harmonizer_B = Bank[i].lv[LV_HARMONIZER][19];
    MusDelay_B = Bank[i].lv[LV_MUSICAL_DELAY][19];
    Gate_B = Bank[i].lv[LV_NOISEGATE][19];
    Derelict_B = Bank[i].lv[LV_DERELICT][19];
    APhaser_B = Bank[i].lv[LV_ANALOG_PHASER][19];
    Valve_B = Bank[i].lv[LV_VALVE][19];
    DFlange_B = Bank[i].lv[LV_DUAL_FLANGE][19];
    Ring_B = Bank[i].lv[LV_RING][19];
    Exciter_B = Bank[i].lv[LV_EXCITER][19];
    DistBand_B = Bank[i].lv[LV_DISTBAND][19];
    Arpie_B = Bank[i].lv[LV_ARPIE][19];
    Expander_B = Bank[i].lv[LV_EXPANDER][19];
    Shuffle_B = Bank[i].lv[LV_SHUFFLE][19];
    Synthfilter_B = Bank[i].lv[LV_SYNTHFILTER][19];
    VaryBand_B = Bank[i].lv[LV_VARYBAND][19];
    Convol_B = Bank[i].lv[LV_CONVOLOTRON][19];
    Looper_B = Bank[i].lv[LV_LOOPER][19];
    MuTroMojo_B = Bank[i].lv[LV_MUTROMOJO][19];
    Echoverse_B = Bank[i].lv[LV_ECHOVERSE][19];
    CoilCrafter_B = Bank[i].lv[LV_COILCRAFTER][19];
    ShelfBoost_B = Bank[i].lv[LV_SHELFBOOST][19];
    Vocoder_B = Bank[i].lv[LV_VOCODER][19];
    Sustainer_B = Bank[i].lv[LV_SUSTAINER][19];
    Sequence_B = Bank[i].lv[LV_SEQUENCE][19];
    Shifter_B = Bank[i].lv[LV_SHIFTER][19];
    StompBox_B = Bank[i].lv[LV_STOMPBOX][19];
    Reverbtron_B = Bank[i].lv[LV_REVERBTRON][19];
    Echotron_B = Bank[i].lv[LV_ECHOTRON][19];
    StereoHarm_B = Bank[i].lv[LV_STEREOHARM][19];
    CompBand_B = Bank[i].lv[LV_COMPBAND][19];
    Opticaltrem_B = Bank[i].lv[LV_OPTICALTREM][19];
    Vibe_B = Bank[i].lv[LV_VIBE][19];
    Infinity_B = Bank[i].lv[LV_INFINITY][19];

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
    memset(Bank[i].Preset_Name, 0, sizeof (Bank[i].Preset_Name));
    strcpy(Bank[i].Preset_Name, Preset_Name);
    memset(Bank[i].Author, 0, sizeof (Bank[i].Author));
    strcpy(Bank[i].Author, Author);
    memset(Bank[i].ConvoFiname, 0, sizeof (Bank[i].ConvoFiname));
    strcpy(Bank[i].ConvoFiname, efx_Convol->Filename);
    memset(Bank[i].RevFiname, 0, sizeof (Bank[i].RevFiname));
    strcpy(Bank[i].RevFiname, efx_Reverbtron->Filename);
    memset(Bank[i].EchoFiname, 0, sizeof (Bank[i].EchoFiname));
    strcpy(Bank[i].EchoFiname, efx_Echotron->Filename);


    Bank[i].Input_Gain = Input_Gain;
    Bank[i].Master_Volume = Master_Volume;
    Bank[i].Balance = Fraction_Bypass;

    int j = 0;

    for (j = 0; j < C_REVERB_PARAMETERS; j++)
        lv[LV_REVERB][j] = efx_Rev->getpar(j);
    for (j = 0; j < C_ECHO_PARAMETERS; j++)
        lv[LV_ECHO][j] = efx_Echo->getpar(j);
    for (j = 0; j < C_CHORUS_PARAMETERS; j++)
        lv[LV_CHORUS][j] = efx_Chorus->getpar(j);
    for (j = 0; j < C_FLANGER_PARAMETERS; j++)
        lv[LV_FLANGER][j] = efx_Flanger->getpar(j);
    for (j = 0; j < C_PHASER_PARAMETERS; j++)
        lv[LV_PHASER][j] = efx_Phaser->getpar(j);
    for (j = 0; j < C_OVERDRIVE_PARAMETERS; j++)
        lv[LV_OVERDRIVE][j] = efx_Overdrive->getpar(j);
    for (j = 0; j < C_DIST_PARAMETERS; j++)
        lv[LV_DISTORTION][j] = efx_Distorsion->getpar(j);
    for (j = 0; j < C_COMPRESS_PARAMETERS; j++)
        lv[LV_COMPRESSOR][j] = efx_Compressor->getpar(j);
    for (j = 0; j < C_WAHWAH_PARAMETERS; j++)
        lv[LV_WAHWAH][j] = efx_WahWah->getpar(j);
    for (j = 0; j < C_ALIENWAH_PARAMETERS; j++)
        lv[LV_ALIENWAH][j] = efx_Alienwah->getpar(j);
    for (j = 0; j < C_PAN_PARAMETERS; j++)
        lv[LV_PAN][j] = efx_Pan->getpar(j);
    for (j = 0; j < C_HARM_PARAMETERS; j++)
        lv[LV_HARMONIZER][j] = efx_Har->getpar(j);
    for (j = 0; j < C_MUSIC_PARAMETERS; j++)
        lv[LV_MUSICAL_DELAY][j] = efx_MusDelay->getpar(j);
    for (j = 0; j < C_GATE_PARAMETERS; j++)
        lv[LV_NOISEGATE][j] = efx_Gate->getpar(j);
    for (j = 0; j < C_DERE_PARAMETERS; j++)
        lv[LV_DERELICT][j] = efx_Derelict->getpar(j);
    for (j = 0; j < C_APHASER_PARAMETERS; j++)
        lv[LV_ANALOG_PHASER][j] = efx_APhaser->getpar(j);
    for (j = 0; j < C_VALVE_PARAMETERS; j++)
        lv[LV_VALVE][j] = efx_Valve->getpar(j);
    for (j = 0; j < C_DFLANGE_PARAMETERS; j++)
        lv[LV_DUAL_FLANGE][j] = efx_DFlange->getpar(j);
    for (j = 0; j < C_RING_PARAMETERS; j++)
        lv[LV_RING][j] = efx_Ring->getpar(j);
    for (j = 0; j < C_EXCITER_PARAMETERS; j++)
        lv[LV_EXCITER][j] = efx_Exciter->getpar(j);
    for (j = 0; j < C_DBAND_PARAMETERS; j++)
        lv[LV_DISTBAND][j] = efx_DistBand->getpar(j);
    for (j = 0; j < C_ARPIE_PARAMETERS; j++)
        lv[LV_ARPIE][j] = efx_Arpie->getpar(j);
    for (j = 0; j < C_EXPANDER_PARAMETERS; j++)
        lv[LV_EXPANDER][j] = efx_Expander->getpar(j);
    for (j = 0; j < C_SHUFFLE_PARAMETERS; j++)
        lv[LV_SHUFFLE][j] = efx_Shuffle->getpar(j);
    for (j = 0; j < C_SYNTHFILTER_PARAMETERS; j++)
        lv[LV_SYNTHFILTER][j] = efx_Synthfilter->getpar(j);
    for (j = 0; j < C_VARYBAND_PARAMETERS; j++)
        lv[LV_VARYBAND][j] = efx_VaryBand->getpar(j);
    for (j = 0; j < C_CONVO_PARAMETERS; j++)
        lv[LV_CONVOLOTRON][j] = efx_Convol->getpar(j);
    for (j = 0; j < C_LOOPER_PARAMETERS; j++)
        lv[LV_LOOPER][j] = efx_Looper->getpar(j);
    for (j = 0; j < C_MUTRO_PARAMETERS; j++)
        lv[LV_MUTROMOJO][j] = efx_MuTroMojo->getpar(j);
    for (j = 0; j < C_ECHOVERSE_PARAMETERS; j++)
        lv[LV_ECHOVERSE][j] = efx_Echoverse->getpar(j);
    for (j = 0; j < C_COIL_PARAMETERS; j++)
        lv[LV_COILCRAFTER][j] = efx_CoilCrafter->getpar(j);
    for (j = 0; j < C_SHELF_PARAMETERS; j++)
        lv[LV_SHELFBOOST][j] = efx_ShelfBoost->getpar(j);
    for (j = 0; j < C_VOCODER_PARAMETERS; j++)
        lv[LV_VOCODER][j] = efx_Vocoder->getpar(j);
    for (j = 0; j < C_SUSTAIN_PARAMETERS; j++)
        lv[LV_SUSTAINER][j] = efx_Sustainer->getpar(j);
    for (j = 0; j < C_SEQUENCE_PARAMETERS; j++)
        lv[LV_SEQUENCE][j] = efx_Sequence->getpar(j);
    for (j = 0; j < C_SHIFTER_PARAMETERS; j++)
        lv[LV_SHIFTER][j] = efx_Shifter->getpar(j);
    for (j = 0; j < C_STOMP_PARAMETERS; j++)
        lv[LV_STOMPBOX][j] = efx_StompBox->getpar(j);
    for (j = 0; j < C_REVTRON_PARAMETERS; j++)
        lv[LV_REVERBTRON][j] = efx_Reverbtron->getpar(j);
    for (j = 0; j < C_ECHOTRON_PARAMETERS; j++)
        lv[LV_ECHOTRON][j] = efx_Echotron->getpar(j);
    for (j = 0; j < C_SHARM_PARAMETERS; j++)
        lv[LV_STEREOHARM][j] = efx_StereoHarm->getpar(j);
    for (j = 0; j < C_COMPBAND_PARAMETERS; j++)
        lv[LV_COMPBAND][j] = efx_CompBand->getpar(j);
    for (j = 0; j < C_OPTICAL_PARAMETERS; j++)
        lv[LV_OPTICALTREM][j] = efx_Opticaltrem->getpar(j);
    for (j = 0; j < C_VIBE_PARAMETERS; j++)
        lv[LV_VIBE][j] = efx_Vibe->getpar(j);
    for (j = 0; j < C_INFINITY_PARAMETERS; j++)
        lv[LV_INFINITY][j] = efx_Infinity->getpar(j);


    for (j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
        lv[LV_ORDER][j] = efx_order[j];

    for (j = 0; j < 10; j++)
        lv[LV_EQ][j] = efx_EQ1->getpar(j * 5 + 12);

    lv[LV_EQ][10] = efx_EQ1->getpar(EQ_Gain);
    lv[LV_EQ][11] = efx_EQ1->getpar(EQ_Q);

    for (j = 0; j < 3; j++)
    {
        lv[LV_PARAMETRIC][0 + j * 3] = efx_EQ2->getpar(j * 5 + 11);
        lv[LV_PARAMETRIC][1 + j * 3] = efx_EQ2->getpar(j * 5 + 12);
        lv[LV_PARAMETRIC][2 + j * 3] = efx_EQ2->getpar(j * 5 + 13);
    }

    lv[LV_PARAMETRIC][9] = efx_EQ2->getpar(Parametric_Gain);

    lv[LV_CABINET][0] = efx_Cabinet->Cabinet_Preset;
    lv[LV_CABINET][1] = efx_Cabinet->getpar(Cabinet_Gain);


    for (j = 0; j < C_NUM_EFFECTS_PLUS_ORDER; j++)
    {
        for (int k = 0; k < 19; k++)
        {
            Bank[i].lv[j][k] = lv[j][k];
        }
    }

    Bank[i].lv[LV_REVERB][19] = Reverb_Bypass;
    Bank[i].lv[LV_ECHO][19] = Echo_Bypass;
    Bank[i].lv[LV_CHORUS][19] = Chorus_Bypass;
    Bank[i].lv[LV_FLANGER][19] = Flanger_Bypass;
    Bank[i].lv[LV_PHASER][19] = Phaser_Bypass;
    Bank[i].lv[LV_OVERDRIVE][19] = Overdrive_Bypass;
    Bank[i].lv[LV_DISTORTION][19] = Distorsion_Bypass;
    Bank[i].lv[LV_EQ][19] = EQ1_Bypass;
    Bank[i].lv[LV_PARAMETRIC][19] = EQ2_Bypass;
    Bank[i].lv[LV_COMPRESSOR][19] = Compressor_Bypass;
    Bank[i].lv[LV_WAHWAH][19] = WahWah_Bypass;
    Bank[i].lv[LV_ALIENWAH][19] = Alienwah_Bypass;
    Bank[i].lv[LV_CABINET][19] = Cabinet_Bypass;
    Bank[i].lv[LV_PAN][19] = Pan_Bypass;
    Bank[i].lv[LV_HARMONIZER][19] = Harmonizer_Bypass;
    Bank[i].lv[LV_MUSICAL_DELAY][19] = MusDelay_Bypass;
    Bank[i].lv[LV_NOISEGATE][19] = Gate_Bypass;
    Bank[i].lv[LV_DERELICT][19] = Derelict_Bypass;
    Bank[i].lv[LV_ANALOG_PHASER][19] = APhaser_Bypass;
    Bank[i].lv[LV_VALVE][19] = Valve_Bypass;
    Bank[i].lv[LV_DUAL_FLANGE][19] = DFlange_Bypass;
    Bank[i].lv[LV_RING][19] = Ring_Bypass;
    Bank[i].lv[LV_EXCITER][19] = Exciter_Bypass;
    Bank[i].lv[LV_DISTBAND][19] = DistBand_Bypass;
    Bank[i].lv[LV_ARPIE][19] = Arpie_Bypass;
    Bank[i].lv[LV_EXPANDER][19] = Expander_Bypass;
    Bank[i].lv[LV_SHUFFLE][19] = Shuffle_Bypass;
    Bank[i].lv[LV_SYNTHFILTER][19] = Synthfilter_Bypass;
    Bank[i].lv[LV_VARYBAND][19] = VaryBand_Bypass;
    Bank[i].lv[LV_CONVOLOTRON][19] = Convol_Bypass;
    Bank[i].lv[LV_LOOPER][19] = Looper_Bypass;
    Bank[i].lv[LV_MUTROMOJO][19] = MuTroMojo_Bypass;
    Bank[i].lv[LV_ECHOVERSE][19] = Echoverse_Bypass;
    Bank[i].lv[LV_COILCRAFTER][19] = CoilCrafter_Bypass;
    Bank[i].lv[LV_SHELFBOOST][19] = ShelfBoost_Bypass;
    Bank[i].lv[LV_VOCODER][19] = Vocoder_Bypass;
    Bank[i].lv[LV_SUSTAINER][19] = Sustainer_Bypass;
    Bank[i].lv[LV_SEQUENCE][19] = Sequence_Bypass;
    Bank[i].lv[LV_SHIFTER][19] = Shifter_Bypass;
    Bank[i].lv[LV_STOMPBOX][19] = StompBox_Bypass;
    Bank[i].lv[LV_REVERBTRON][19] = Reverbtron_Bypass;
    Bank[i].lv[LV_ECHOTRON][19] = Echotron_Bypass;
    Bank[i].lv[LV_STEREOHARM][19] = StereoHarm_Bypass;
    Bank[i].lv[LV_COMPBAND][19] = CompBand_Bypass;
    Bank[i].lv[LV_OPTICALTREM][19] = Opticaltrem_Bypass;
    Bank[i].lv[LV_VIBE][19] = Vibe_Bypass;
    Bank[i].lv[LV_INFINITY][19] = Infinity_Bypass;


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

