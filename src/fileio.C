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


const int presets_default[48][MAX_PRESET_SIZE] = {
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
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18},
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

void RKR::putbuf(char *buf, int j)
{
    char *cfilename;
    cfilename = (char *) malloc(sizeof (char) * 128);

    switch (j)
    {

    case 8:
        //Reverb
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[0][0], &lv[0][1], &lv[0][2], &lv[0][3], &lv[0][4],
               &lv[0][5], &lv[0][6], &lv[0][7], &lv[0][8], &lv[0][9],
               &lv[0][10], &lv[0][11], &Reverb_B);
        break;

    case 4:
        //Echo
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[1][0], &lv[1][1], &lv[1][2], &lv[1][3], &lv[1][4],
               &lv[1][5], &lv[1][6], &lv[1][7], &lv[1][8], &Echo_B);
        break;

    case 5:
        //Chorus
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[2][0], &lv[2][1], &lv[2][2], &lv[2][3], &lv[2][4],
               &lv[2][5], &lv[2][6], &lv[2][7], &lv[2][8], &lv[2][9],
               &lv[2][10], &lv[2][11], &lv[2][12], &Chorus_B);
        break;

    case 7:
        //Flanger
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[3][0], &lv[3][1], &lv[3][2], &lv[3][3], &lv[3][4],
               &lv[3][5], &lv[3][6], &lv[3][7], &lv[3][8], &lv[3][9],
               &lv[3][10], &lv[3][11], &lv[3][12], &Flanger_B);
        break;

    case 6:
        //Phaser
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[4][0], &lv[4][1], &lv[4][2], &lv[4][3], &lv[4][4],
               &lv[4][5], &lv[4][6], &lv[4][7], &lv[4][8], &lv[4][9],
               &lv[4][10], &lv[4][11], &Phaser_B);
        break;

    case 3:
        //Overdrive
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[5][0], &lv[5][1], &lv[5][2], &lv[5][3], &lv[5][4],
               &lv[5][5], &lv[5][6], &lv[5][7], &lv[5][8], &lv[5][9],
               &lv[5][10], &lv[5][11], &lv[5][12], &Overdrive_B);
        break;

    case 2:
        //Distorsion
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[6][0], &lv[6][1], &lv[6][2], &lv[6][3], &lv[6][4],
               &lv[6][5], &lv[6][6], &lv[6][7], &lv[6][8], &lv[6][9],
               &lv[6][10], &lv[6][11], &lv[6][12], &Distorsion_B);
        break;

    case 0:
        //EQ1
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[7][0], &lv[7][1], &lv[7][2], &lv[7][3], &lv[7][4],
               &lv[7][5], &lv[7][6], &lv[7][7], &lv[7][8], &lv[7][9],
               &lv[7][10], &lv[7][11], &EQ1_B);
        break;

    case 9:
        //EQ2
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[8][0], &lv[8][1], &lv[8][2], &lv[8][3], &lv[8][4],
               &lv[8][5], &lv[8][6], &lv[8][7], &lv[8][8], &lv[8][9],
               &EQ2_B);
        break;

    case 1:
        //Compressor
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[9][0], &lv[9][1], &lv[9][2], &lv[9][3], &lv[9][4],
               &lv[9][5], &lv[9][6], &lv[9][7], &lv[9][8], &Compressor_B);
        break;

    case 10:
        //WhaWha
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[11][0], &lv[11][1], &lv[11][2], &lv[11][3], &lv[11][4],
               &lv[11][5], &lv[11][6], &lv[11][7], &lv[11][8], &lv[11][9],
               &lv[11][10], &WhaWha_B);
        break;

    case 11:
        //Alienwah
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[12][0], &lv[12][1], &lv[12][2], &lv[12][3], &lv[12][4],
               &lv[12][5], &lv[12][6], &lv[12][7], &lv[12][8], &lv[12][9],
               &lv[12][10], &Alienwah_B);
        break;

    case 12:
        //Cabinet
        sscanf(buf, "%d,%d,%d\n", &lv[13][0], &lv[13][1], &Cabinet_B);
        break;

    case 13:
        //Pan
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[14][0], &lv[14][1], &lv[14][2], &lv[14][3], &lv[14][4],
               &lv[14][5], &lv[14][6], &lv[14][7], &lv[14][8], &Pan_B);
        break;

    case 14:
        //Harmonizer
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[15][0], &lv[15][1], &lv[15][2], &lv[15][3], &lv[15][4],
               &lv[15][5], &lv[15][6], &lv[15][7], &lv[15][8], &lv[15][9],
               &lv[15][10], &Harmonizer_B);
        break;

    case 15:
        //Musical Delay
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[16][0], &lv[16][1], &lv[16][2], &lv[16][3], &lv[16][4],
               &lv[16][5], &lv[16][6], &lv[16][7], &lv[16][8], &lv[16][9],
               &lv[16][10], &lv[16][11], &lv[16][12], &MusDelay_B);
        break;

    case 16:
        //NoiseGate
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[17][0], &lv[17][1], &lv[17][2], &lv[17][3], &lv[17][4],
               &lv[17][5], &lv[17][6], &Gate_B);

        break;

    case 17:
        //Derelict
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[18][0], &lv[18][1], &lv[18][2], &lv[18][3], &lv[18][4],
               &lv[18][5], &lv[18][6], &lv[18][7], &lv[18][8], &lv[18][9],
               &lv[18][10], &lv[18][11], &Derelict_B);
        break;

    case 18:
        //Analog Phaser
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[19][0], &lv[19][1], &lv[19][2], &lv[19][3], &lv[19][4],
               &lv[19][5], &lv[19][6], &lv[19][7], &lv[19][8], &lv[19][9],
               &lv[19][10], &lv[19][11], &APhaser_B);
        break;

    case 19:
        //Valve
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[20][0], &lv[20][1], &lv[20][2], &lv[20][3], &lv[20][4],
               &lv[20][5], &lv[20][6], &lv[20][7], &lv[20][8], &lv[20][9],
               &lv[20][10], &lv[20][11], &lv[20][12], &Valve_B);
        break;

    case 20:
        //Dual Flnage
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[21][0], &lv[21][1], &lv[21][2], &lv[21][3], &lv[21][4],
               &lv[21][5], &lv[21][6], &lv[21][7], &lv[21][8], &lv[21][9],
               &lv[21][10], &lv[21][11], &lv[21][12], &lv[21][13], &lv[21][14],
               &DFlange_B);
        break;

    case 21:
        //Ring
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[22][0], &lv[22][1], &lv[22][2], &lv[22][3], &lv[22][4],
               &lv[22][5], &lv[22][6], &lv[22][7], &lv[22][8], &lv[22][9],
               &lv[22][10], &lv[22][11], &lv[22][12], &Ring_B);
        break;

    case 22:
        //Exciter
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[23][0], &lv[23][1], &lv[23][2], &lv[23][3], &lv[23][4],
               &lv[23][5], &lv[23][6], &lv[23][7], &lv[23][8], &lv[23][9],
               &lv[23][10], &lv[23][11], &lv[23][12], &Exciter_B);
        break;

    case 23:
        //DistBand
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[24][0], &lv[24][1], &lv[24][2], &lv[24][3], &lv[24][4],
               &lv[24][5], &lv[24][6], &lv[24][7], &lv[24][8], &lv[24][9],
               &lv[24][10], &lv[24][11], &lv[24][12], &lv[24][13], &lv[24][14],
               &DistBand_B);
        break;

    case 24:
        //Arpie
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[25][0], &lv[25][1], &lv[25][2], &lv[25][3], &lv[25][4],
               &lv[25][5], &lv[25][6], &lv[25][7], &lv[25][8], &lv[25][9],
               &lv[25][10], &Arpie_B);
        break;

    case 25:
        //Expander
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[26][0], &lv[26][1], &lv[26][2], &lv[26][3], &lv[26][4],
               &lv[26][5], &lv[26][6], &Expander_B);

        break;

    case 26:
        //Shuffle
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[27][0], &lv[27][1], &lv[27][2], &lv[27][3], &lv[27][4],
               &lv[27][5], &lv[27][6], &lv[27][7], &lv[27][8], &lv[27][9],
               &lv[27][10], &Shuffle_B);
        break;

    case 27:
        //Synthfilter
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[28][0], &lv[28][1], &lv[28][2], &lv[28][3], &lv[28][4],
               &lv[28][5], &lv[28][6], &lv[28][7], &lv[28][8], &lv[28][9],
               &lv[28][10], &lv[28][11], &lv[28][12], &lv[28][13], &lv[28][14],
               &lv[28][15], &Synthfilter_B);
        break;

    case 28:
        //VaryBand
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[29][0], &lv[29][1], &lv[29][2], &lv[29][3], &lv[29][4],
               &lv[29][5], &lv[29][6], &lv[29][7], &lv[29][8], &lv[29][9],
               &lv[29][10], &VaryBand_B);
        break;

    case 29:
        //Convolotron
        memset(efx_Convol->Filename, 0, sizeof (efx_Convol->Filename));
        memset(cfilename, 0, sizeof (*cfilename));
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[30][0], &lv[30][1], &lv[30][2], &lv[30][3], &lv[30][4],
               &lv[30][5], &lv[30][6], &lv[30][7], &lv[30][8], &lv[30][9],
               &lv[30][10], &Convol_B, cfilename);
        strcpy(efx_Convol->Filename, cfilename);
        break;

    case 30:
        //Looper
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[31][0], &lv[31][1], &lv[31][2], &lv[31][3], &lv[31][4],
               &lv[31][5], &lv[31][6], &lv[31][7], &lv[31][8], &lv[31][9],
               &lv[31][10], &lv[31][11], &lv[31][12], &lv[31][13], &Looper_B);
        break;

    case 31:
        //MuTroMojo
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[32][0], &lv[32][1], &lv[32][2], &lv[32][3], &lv[32][4],
               &lv[32][5], &lv[32][6], &lv[32][7], &lv[32][8], &lv[32][9],
               &lv[32][10], &lv[32][11], &lv[32][12], &lv[32][13], &lv[32][14],
               &lv[32][15], &lv[32][16], &lv[32][17], &MuTroMojo_B);
        break;

    case 32:
        //Echoverse
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[33][0], &lv[33][1], &lv[33][2], &lv[33][3], &lv[33][4],
               &lv[33][5], &lv[33][6], &lv[33][7], &lv[33][8], &lv[33][9], &Echoverse_B);
        break;


    case 33:
        //CoilCrafter
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[34][0], &lv[34][1], &lv[34][2], &lv[34][3], &lv[34][4],
               &lv[34][5], &lv[34][6], &lv[34][7], &lv[34][8], &CoilCrafter_B);
        break;

    case 34:
        //CoilCrafter
        sscanf(buf, "%d,%d,%d,%d,%d,%d\n",
               &lv[35][0], &lv[35][1], &lv[35][2], &lv[35][3], &lv[35][4],
               &ShelfBoost_B);
        break;

    case 35:
        //Vocoder
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[36][0], &lv[36][1], &lv[36][2], &lv[36][3], &lv[36][4],
               &lv[36][5], &lv[36][6], &Vocoder_B);
        break;

    case 36:
        //Sustainer
        sscanf(buf, "%d,%d,%d\n",
               &lv[37][0], &lv[37][1], &Sustainer_B);
        break;

    case 37:
        //Sequence
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[38][0], &lv[38][1], &lv[38][2], &lv[38][3], &lv[38][4],
               &lv[38][5], &lv[38][6], &lv[38][7], &lv[38][8], &lv[38][9],
               &lv[38][10], &lv[38][11], &lv[38][12], &lv[38][13], &lv[38][14], &Sequence_B);
        break;

    case 38:
        //Shifter
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[39][0], &lv[39][1], &lv[39][2], &lv[39][3], &lv[39][4],
               &lv[39][5], &lv[39][6], &lv[39][7], &lv[39][8], &lv[39][9], &Shifter_B);
        break;

    case 39:
        //StompBox
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
               &lv[40][0], &lv[40][1], &lv[40][2], &lv[40][3], &lv[40][4],
               &lv[40][5], &StompBox_B);
        break;

    case 40:
        //Reverbtron
        memset(efx_Reverbtron->Filename, 0, sizeof (efx_Reverbtron->Filename));
        memset(cfilename, 0, sizeof (*cfilename));
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[41][0], &lv[41][1], &lv[41][2], &lv[41][3], &lv[41][4],
               &lv[41][5], &lv[41][6], &lv[41][7], &lv[41][8], &lv[41][9],
               &lv[41][10], &lv[41][11], &lv[41][12], &lv[41][13], &lv[41][14], &lv[41][15],
               &Reverbtron_B,
               cfilename);
        strcpy(efx_Reverbtron->Filename, cfilename);
        break;

    case 41:
        //Echotron
        memset(efx_Echotron->Filename, 0, sizeof (efx_Echotron->Filename));
        memset(cfilename, 0, sizeof (*cfilename));
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
               &lv[42][0], &lv[42][1], &lv[42][2], &lv[42][3], &lv[42][4],
               &lv[42][5], &lv[42][6], &lv[42][7], &lv[42][8], &lv[42][9],
               &lv[42][10], &lv[42][11], &lv[42][12], &lv[42][13], &lv[42][14], &lv[42][15],
               &Echotron_B,
               cfilename);
        strcpy(efx_Echotron->Filename, cfilename);
        break;

    case 42:
        //StereoHarm
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[43][0], &lv[43][1], &lv[43][2], &lv[43][3], &lv[43][4],
               &lv[43][5], &lv[43][6], &lv[43][7], &lv[43][8], &lv[43][9],
               &lv[43][10], &lv[43][11], &StereoHarm_B);
        break;

    case 43:
        //CompBand
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[44][0], &lv[44][1], &lv[44][2], &lv[44][3], &lv[44][4],
               &lv[44][5], &lv[44][6], &lv[44][7], &lv[44][8], &lv[44][9],
               &lv[44][10], &lv[44][11], &lv[44][12], &CompBand_B);
        break;

    case 44:
        //Opticaltrem
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
               &lv[45][0], &lv[45][1], &lv[45][2], &lv[45][3], &lv[45][4],
               &lv[45][5], &Opticaltrem_B);
        break;

    case 45:
        //Vibe
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[46][0], &lv[46][1], &lv[46][2], &lv[46][3], &lv[46][4],
               &lv[46][5], &lv[46][6], &lv[46][7], &lv[46][8], &lv[46][9], &lv[46][10],
               &Vibe_B);
        break;

    case 46:
        //Infinity
        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &lv[47][0], &lv[47][1], &lv[47][2], &lv[47][3], &lv[47][4],
               &lv[47][5], &lv[47][6], &lv[47][7], &lv[47][8], &lv[47][9],
               &lv[47][10], &lv[47][11], &lv[47][12], &lv[47][13], &lv[47][14],
               &lv[47][15], &lv[47][16], &lv[47][17], &Infinity_B);
        break;
    }

    free(cfilename);
}

void RKR::getbuf(char *buf, int j)
{
    switch (j)
    {
    case 8:
        //Reverb
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Rev->getpar(Reverb_DryWet), efx_Rev->getpar(Reverb_Pan),
                efx_Rev->getpar(Reverb_Time), efx_Rev->getpar(Reverb_I_Delay),
                efx_Rev->getpar(Reverb_Delay_FB), efx_Rev->getpar(Reverb_SKIP_5),
                efx_Rev->getpar(Reverb_SKIP_6), efx_Rev->getpar(Reverb_LPF),
                efx_Rev->getpar(Reverb_HPF), efx_Rev->getpar(Reverb_Damp),
                efx_Rev->getpar(Reverb_Type), efx_Rev->getpar(Reverb_Room), Reverb_Bypass);
        break;

    case 4:
        //Echo
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Echo->getpar(Echo_DryWet), efx_Echo->getpar(Echo_Pan),
                efx_Echo->getpar(Echo_Delay), efx_Echo->getpar(Echo_LR_Delay),
                efx_Echo->getpar(Echo_LR_Cross), efx_Echo->getpar(Echo_Feedback),
                efx_Echo->getpar(Echo_Damp), efx_Echo->getpar(Echo_Reverse),
                efx_Echo->getpar(Echo_Direct), Echo_Bypass);
        break;

    case 5:
        //Chorus
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Chorus->getpar(Chorus_DryWet), efx_Chorus->getpar(Chorus_Pan),
                efx_Chorus->getpar(Chorus_LFO_Tempo), efx_Chorus->getpar(Chorus_LFO_Random),
                efx_Chorus->getpar(Chorus_LFO_Type), efx_Chorus->getpar(Chorus_LFO_Stereo),
                efx_Chorus->getpar(Chorus_Depth), efx_Chorus->getpar(Chorus_Delay),
                efx_Chorus->getpar(Chorus_Feedback), efx_Chorus->getpar(Chorus_LR_Cross),
                efx_Chorus->getpar(Chorus_SKIP_Flange_10), efx_Chorus->getpar(Chorus_Subtract),
                efx_Chorus->getpar(Chorus_Intense), Chorus_Bypass);
        break;

    case 7:
        //Flanger
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Flanger->getpar(Chorus_DryWet), efx_Flanger->getpar(Chorus_Pan),
                efx_Flanger->getpar(Chorus_LFO_Tempo), efx_Flanger->getpar(Chorus_LFO_Random),
                efx_Flanger->getpar(Chorus_LFO_Type), efx_Flanger->getpar(Chorus_LFO_Stereo),
                efx_Flanger->getpar(Chorus_Depth), efx_Flanger->getpar(Chorus_Delay),
                efx_Flanger->getpar(Chorus_Feedback), efx_Flanger->getpar(Chorus_LR_Cross),
                efx_Flanger->getpar(Chorus_SKIP_Flange_10), efx_Flanger->getpar(Chorus_Subtract),
                efx_Flanger->getpar(Chorus_Intense), Flanger_Bypass);
        break;

    case 6:
        //Phaser
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Phaser->getpar(Phaser_DryWet), efx_Phaser->getpar(Phaser_Pan),
                efx_Phaser->getpar(Phaser_LFO_Tempo), efx_Phaser->getpar(Phaser_LFO_Random),
                efx_Phaser->getpar(Phaser_LFO_Type), efx_Phaser->getpar(Phaser_LFO_Stereo),
                efx_Phaser->getpar(Phaser_Depth), efx_Phaser->getpar(Phaser_Feedback),
                efx_Phaser->getpar(Phaser_Stages), efx_Phaser->getpar(Phaser_LR_Cross),
                efx_Phaser->getpar(Phaser_Subtract), efx_Phaser->getpar(Phaser_Phase),
                Phaser_Bypass);
        break;

    case 3:
        //Overdrive
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Overdrive->getpar(Dist_DryWet), efx_Overdrive->getpar(Dist_Pan),
                efx_Overdrive->getpar(Dist_LR_Cross), efx_Overdrive->getpar(Dist_Drive),
                efx_Overdrive->getpar(Dist_Level), efx_Overdrive->getpar(Dist_Type),
                efx_Overdrive->getpar(Dist_Negate), efx_Overdrive->getpar(Dist_LPF),
                efx_Overdrive->getpar(Dist_HPF), efx_Overdrive->getpar(Dist_Stereo),
                efx_Overdrive->getpar(Dist_Prefilter), efx_Overdrive->getpar(Dist_SKIP_11),
                efx_Overdrive->getpar(Dist_Suboctave), Overdrive_Bypass);
        break;

    case 2:
        //Distorsion
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Distorsion->getpar(Dist_DryWet), efx_Distorsion->getpar(Dist_Pan),
                efx_Distorsion->getpar(Dist_LR_Cross), efx_Distorsion->getpar(Dist_Drive),
                efx_Distorsion->getpar(Dist_Level), efx_Distorsion->getpar(Dist_Type),
                efx_Distorsion->getpar(Dist_Negate), efx_Distorsion->getpar(Dist_LPF),
                efx_Distorsion->getpar(Dist_HPF), efx_Distorsion->getpar(Dist_Stereo),
                efx_Distorsion->getpar(Dist_Prefilter), efx_Distorsion->getpar(Dist_SKIP_11),
                efx_Distorsion->getpar(Dist_Suboctave), Distorsion_Bypass);
        break;

    case 0:
        //EQ1
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_EQ1->getpar(12), efx_EQ1->getpar(5 + 12),
                efx_EQ1->getpar(10 + 12), efx_EQ1->getpar(15 + 12),
                efx_EQ1->getpar(20 + 12), efx_EQ1->getpar(25 + 12),
                efx_EQ1->getpar(30 + 12), efx_EQ1->getpar(35 + 12),
                efx_EQ1->getpar(40 + 12), efx_EQ1->getpar(45 + 12),
                efx_EQ1->getpar(0), efx_EQ1->getpar(13), EQ1_Bypass);
        break;

    case 9:
        //EQ2
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_EQ2->getpar(11), efx_EQ2->getpar(12),
                efx_EQ2->getpar(13), efx_EQ2->getpar(5 + 11),
                efx_EQ2->getpar(5 + 12), efx_EQ2->getpar(5 + 13),
                efx_EQ2->getpar(10 + 11), efx_EQ2->getpar(10 + 12),
                efx_EQ2->getpar(10 + 13), efx_EQ2->getpar(0),
                EQ2_Bypass);
        break;

    case 1:
        // Compressor
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Compressor->getpar(Compress_Threshold), efx_Compressor->getpar(Compress_Ratio),
                efx_Compressor->getpar(Compress_Output), efx_Compressor->getpar(Compress_Attack),
                efx_Compressor->getpar(Compress_Release), efx_Compressor->getpar(Compress_Auto_Out),
                efx_Compressor->getpar(Compress_Knee), efx_Compressor->getpar(Compress_Stereo),
                efx_Compressor->getpar(Compress_Peak), Compressor_Bypass);
        break;


    case 10:
        //WhaWha
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_WhaWha->getpar(0), efx_WhaWha->getpar(1),
                efx_WhaWha->getpar(2), efx_WhaWha->getpar(3),
                efx_WhaWha->getpar(4), efx_WhaWha->getpar(5),
                efx_WhaWha->getpar(6), efx_WhaWha->getpar(7),
                efx_WhaWha->getpar(8), efx_WhaWha->getpar(9),
                efx_WhaWha->Ppreset, WhaWha_Bypass);
        break;

    case 11:
        //Alienwah
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Alienwah->getpar(Alien_DryWet), efx_Alienwah->getpar(Alien_Pan),
                efx_Alienwah->getpar(Alien_LFO_Tempo), efx_Alienwah->getpar(Alien_LFO_Rand),
                efx_Alienwah->getpar(Alien_LFO_Type), efx_Alienwah->getpar(Alien_LFO_Stereo),
                efx_Alienwah->getpar(Alien_Depth), efx_Alienwah->getpar(Alien_Feedback),
                efx_Alienwah->getpar(Alien_Delay), efx_Alienwah->getpar(Alien_LRCross),
                efx_Alienwah->getpar(Alien_Phase), Alienwah_Bypass);
        break;

    case 12:
        //Cabinet
        sprintf(buf, "%d,%d,%d\n",
                efx_Cabinet->Cabinet_Preset, efx_Cabinet->getpar(0), Cabinet_Bypass);
        break;

    case 13:
        //Pan
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Pan->getpar(Pan_DryWet), efx_Pan->getpar(Pan_Pan),
                efx_Pan->getpar(Pan_LFO_Tempo), efx_Pan->getpar(Pan_LFO_Random),
                efx_Pan->getpar(Pan_LFO_Type), efx_Pan->getpar(Pan_LFO_Stereo),
                efx_Pan->getpar(Pan_Ex_St_Amt), efx_Pan->getpar(Pan_AutoPan),
                efx_Pan->getpar(Pan_Enable_Extra), Pan_Bypass);
        break;

    case 14:
        //Harmonizer
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Har->getpar(Harm_DryWet), efx_Har->getpar(Harm_Pan),
                efx_Har->getpar(Harm_Gain), efx_Har->getpar(Harm_Interval),
                efx_Har->getpar(Harm_Filter_Freq), efx_Har->getpar(Harm_Select),
                efx_Har->getpar(Harm_Note), efx_Har->getpar(Harm_Chord),
                efx_Har->getpar(Harm_Filter_Gain), efx_Har->getpar(Harm_Filter_Q),
                efx_Har->getpar(Harm_MIDI), Harmonizer_Bypass);
        break;

    case 15:
        //MusicalDelay
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_MusDelay->getpar(Music_DryWet), efx_MusDelay->getpar(Music_Pan_1),
                efx_MusDelay->getpar(Music_Delay_1), efx_MusDelay->getpar(Music_Del_Offset),
                efx_MusDelay->getpar(Music_LR_Cross), efx_MusDelay->getpar(Music_Feedback_1),
                efx_MusDelay->getpar(Music_Damp), efx_MusDelay->getpar(Music_Pan_2),
                efx_MusDelay->getpar(Music_Delay_2), efx_MusDelay->getpar(Music_Feedback_2),
                efx_MusDelay->getpar(Music_Tempo), efx_MusDelay->getpar(Music_Gain_1),
                efx_MusDelay->getpar(Music_Gain_2), MusDelay_Bypass);
        break;

    case 16:
        //NoiseGate
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Gate->getpar(Gate_Threshold), efx_Gate->getpar(Gate_Range),
                efx_Gate->getpar(Gate_Attack), efx_Gate->getpar(Gate_Release),
                efx_Gate->getpar(Gate_LPF), efx_Gate->getpar(Gate_HPF),
                efx_Gate->getpar(Gate_Hold), Gate_Bypass);
        break;

    case 17:
        //Derelict
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Derelict->getpar(Dere_DryWet), efx_Derelict->getpar(Dere_Pan),
                efx_Derelict->getpar(Dere_LR_Cross), efx_Derelict->getpar(Dere_Drive),
                efx_Derelict->getpar(Dere_Level), efx_Derelict->getpar(Dere_Type),
                efx_Derelict->getpar(Dere_Negate), efx_Derelict->getpar(Dere_LPF),
                efx_Derelict->getpar(Dere_HPF), efx_Derelict->getpar(Dere_Color),
                efx_Derelict->getpar(Dere_Prefilter), efx_Derelict->getpar(Dere_Suboctave),
                efx_Derelict->getpar(12), Derelict_Bypass); // 12 is invalid...
        break;

    case 18:
        //Analog Phaser
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_APhaser->getpar(APhase_DryWet), efx_APhaser->getpar(APhase_Distortion),
                efx_APhaser->getpar(APhase_LFO_Tempo), efx_APhaser->getpar(APhase_LFO_Rand),
                efx_APhaser->getpar(APhase_LFO_Type), efx_APhaser->getpar(APhase_LFO_Stereo),
                efx_APhaser->getpar(APhase_Width), efx_APhaser->getpar(APhase_Feedback),
                efx_APhaser->getpar(APhase_Stages), efx_APhaser->getpar(APhase_Mismatch),
                efx_APhaser->getpar(APhase_Subtract), efx_APhaser->getpar(APhase_Depth),
                efx_APhaser->getpar(APhase_Hyper), APhaser_Bypass);
        break;

    case 19:
        //Valve
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Valve->getpar(0), efx_Valve->getpar(1),
                efx_Valve->getpar(2), efx_Valve->getpar(3),
                efx_Valve->getpar(4), efx_Valve->getpar(5),
                efx_Valve->getpar(6), efx_Valve->getpar(7),
                efx_Valve->getpar(8), efx_Valve->getpar(9),
                efx_Valve->getpar(10), efx_Valve->getpar(11),
                efx_Valve->getpar(12), Valve_Bypass);
        break;

    case 20:
        //Dual_Flange
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_DFlange->getpar(DFlange_DryWet), efx_DFlange->getpar(DFlange_Pan),
                efx_DFlange->getpar(DFlange_LR_Cross), efx_DFlange->getpar(DFlange_Depth),
                efx_DFlange->getpar(DFlange_Width), efx_DFlange->getpar(DFlange_Offset),
                efx_DFlange->getpar(DFlange_Feedback), efx_DFlange->getpar(DFlange_LPF),
                efx_DFlange->getpar(DFlange_Subtract), efx_DFlange->getpar(DFlange_Zero),
                efx_DFlange->getpar(DFlange_LFO_Tempo), efx_DFlange->getpar(DFlange_LFO_Stereo),
                efx_DFlange->getpar(DFlange_LFO_Type), efx_DFlange->getpar(DFlange_LFO_Rand),
                efx_DFlange->getpar(DFlange_Intense), DFlange_Bypass);
        break;

    case 21:
        //Ring
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Ring->getpar(Ring_DryWet), efx_Ring->getpar(Ring_Pan),
                efx_Ring->getpar(Ring_LR_Cross), efx_Ring->getpar(Ring_Level),
                efx_Ring->getpar(Ring_Depth), efx_Ring->getpar(Ring_Freq),
                efx_Ring->getpar(Ring_Stereo), efx_Ring->getpar(Ring_Sine),
                efx_Ring->getpar(Ring_Triangle), efx_Ring->getpar(Ring_Saw),
                efx_Ring->getpar(Ring_Square), efx_Ring->getpar(Ring_Input),
                efx_Ring->getpar(Ring_Auto_Freq), Ring_Bypass);
        break;

    case 22:
        //Exciter
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Exciter->getpar(Exciter_Gain), efx_Exciter->getpar(Exciter_Harm_1),
                efx_Exciter->getpar(Exciter_Harm_2), efx_Exciter->getpar(Exciter_Harm_3),
                efx_Exciter->getpar(Exciter_Harm_4), efx_Exciter->getpar(Exciter_Harm_5),
                efx_Exciter->getpar(Exciter_Harm_6), efx_Exciter->getpar(Exciter_Harm_7),
                efx_Exciter->getpar(Exciter_Harm_8), efx_Exciter->getpar(Exciter_Harm_9),
                efx_Exciter->getpar(Exciter_Harm_10), efx_Exciter->getpar(Exciter_LPF),
                efx_Exciter->getpar(Exciter_HPF), Exciter_Bypass);
        break;

    case 23:
        //DistBand
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

    case 24:
        //Arpie
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Arpie->getpar(Arpie_DryWet), efx_Arpie->getpar(Arpie_Pan),
                efx_Arpie->getpar(Arpie_Tempo), efx_Arpie->getpar(Arpie_LR_Delay),
                efx_Arpie->getpar(Arpie_LR_Cross), efx_Arpie->getpar(Arpie_Feedback),
                efx_Arpie->getpar(Arpie_Damp), efx_Arpie->getpar(Arpie_ArpeWD),
                efx_Arpie->getpar(Arpie_Harm), efx_Arpie->getpar(Arpie_Pattern),
                efx_Arpie->getpar(Arpie_Subdivision), Arpie_Bypass);
        break;

    case 25:
        //Expander
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Expander->getpar(Expander_Threshold), efx_Expander->getpar(Expander_Shape),
                efx_Expander->getpar(Expander_Attack), efx_Expander->getpar(Expander_Release),
                efx_Expander->getpar(Expander_LPF), efx_Expander->getpar(Expander_HPF),
                efx_Expander->getpar(Expander_Gain), Expander_Bypass);
        break;

    case 26:
        //Shuffle
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Shuffle->getpar(Shuffle_DryWet), efx_Shuffle->getpar(Shuffle_Gain_L),
                efx_Shuffle->getpar(Shuffle_Gain_ML), efx_Shuffle->getpar(Shuffle_Gain_MH),
                efx_Shuffle->getpar(Shuffle_Gain_H), efx_Shuffle->getpar(Shuffle_Freq_L),
                efx_Shuffle->getpar(Shuffle_Freq_ML), efx_Shuffle->getpar(Shuffle_Freq_MH),
                efx_Shuffle->getpar(Shuffle_Freq_H), efx_Shuffle->getpar(Shuffle_Width),
                efx_Shuffle->getpar(Shuffle_F_Band), Shuffle_Bypass);
        break;

    case 27:
        //Synthfilter
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

    case 28:
        //VaryBand
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_VaryBand->getpar(0), efx_VaryBand->getpar(1),
                efx_VaryBand->getpar(2), efx_VaryBand->getpar(3),
                efx_VaryBand->getpar(4), efx_VaryBand->getpar(5),
                efx_VaryBand->getpar(6), efx_VaryBand->getpar(7),
                efx_VaryBand->getpar(8), efx_VaryBand->getpar(9),
                efx_VaryBand->getpar(10), VaryBand_Bypass);
        break;

    case 29:
        //Convolotron
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n",
                efx_Convol->getpar(Convo_DryWet), efx_Convol->getpar(Convo_Pan),
                efx_Convol->getpar(Convo_Safe), efx_Convol->getpar(Convo_Length),
                efx_Convol->getpar(Convo_User_File), efx_Convol->getpar(Convo_SKIP_5),
                efx_Convol->getpar(Convo_Damp), efx_Convol->getpar(Convo_Level),
                efx_Convol->getpar(Convo_Set_File), efx_Convol->getpar(Convo_SKIP_9),
                efx_Convol->getpar(Convo_Feedback), Convol_Bypass, efx_Convol->Filename);
        break;

    case 30:
        //Looper
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Looper->getpar(Looper_DryWet), efx_Looper->getpar(Looper_Play),
                efx_Looper->getpar(Looper_Stop), efx_Looper->getpar(Looper_Record),
                efx_Looper->getpar(Looper_Clear), efx_Looper->getpar(Looper_Reverse),
                efx_Looper->getpar(Looper_Level_1), efx_Looper->getpar(Looper_Track_1),
                efx_Looper->getpar(Looper_Track_2), efx_Looper->getpar(Looper_AutoPlay),
                efx_Looper->getpar(Looper_Level_2), efx_Looper->getpar(Looper_Rec_1),
                efx_Looper->getpar(Looper_Rec_2), efx_Looper->getpar(Looper_Link), Looper_Bypass);
        break;

    case 31:
        //MuTroMojo
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
    case 32:
        //Echoverse
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Echoverse->getpar(Echoverse_DryWet), efx_Echoverse->getpar(Echoverse_Pan),
                efx_Echoverse->getpar(Echoverse_Tempo), efx_Echoverse->getpar(Echoverse_LR_Delay),
                efx_Echoverse->getpar(Echoverse_Angle), efx_Echoverse->getpar(Echoverse_Feedback),
                efx_Echoverse->getpar(Echoverse_Damp), efx_Echoverse->getpar(Echoverse_Reverse),
                efx_Echoverse->getpar(Echoverse_Subdivision), efx_Echoverse->getpar(Echoverse_Ext_Stereo),
                Echoverse_Bypass);
        break;



    case 33:
        //CoilCrafter
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_CoilCrafter->getpar(Coil_Gain), efx_CoilCrafter->getpar(Coil_Origin),
                efx_CoilCrafter->getpar(Coil_Destiny), efx_CoilCrafter->getpar(Coil_Freq_1),
                efx_CoilCrafter->getpar(Coil_Q_1), efx_CoilCrafter->getpar(Coil_Freq_2),
                efx_CoilCrafter->getpar(Coil_Q_2), efx_CoilCrafter->getpar(Coil_Tone),
                efx_CoilCrafter->getpar(Coil_NeckMode), CoilCrafter_Bypass);
        break;

    case 34:
        //ShelfBoost
        sprintf(buf, "%d,%d,%d,%d,%d,%d\n",
                efx_ShelfBoost->getpar(Shelf_Gain), efx_ShelfBoost->getpar(Shelf_Presence),
                efx_ShelfBoost->getpar(Shelf_Tone), efx_ShelfBoost->getpar(Shelf_Stereo),
                efx_ShelfBoost->getpar(Shelf_Level), ShelfBoost_Bypass);
        break;

    case 35:
        //Vocoder
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Vocoder->getpar(0), efx_Vocoder->getpar(1),
                efx_Vocoder->getpar(2), efx_Vocoder->getpar(3),
                efx_Vocoder->getpar(4), efx_Vocoder->getpar(5),
                efx_Vocoder->getpar(6), Vocoder_Bypass);
        break;

    case 36:
        //Sustainer
        sprintf(buf, "%d,%d,%d\n",
                efx_Sustainer->getpar(Sustain_Gain), efx_Sustainer->getpar(Sustain_Sustain),
                Sustainer_Bypass);
        break;

    case 37:
        //Sequence
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

    case 38:
        //Shifter
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Shifter->getpar(Shifter_DryWet), efx_Shifter->getpar(Shifter_Pan),
                efx_Shifter->getpar(Shifter_Gain), efx_Shifter->getpar(Shifter_Attack),
                efx_Shifter->getpar(Shifter_Decay), efx_Shifter->getpar(Shifter_Threshold),
                efx_Shifter->getpar(Shifter_Interval), efx_Shifter->getpar(Shifter_Shift),
                efx_Shifter->getpar(Shifter_Mode), efx_Shifter->getpar(Shifter_Whammy), Shifter_Bypass);
        break;


    case 39:
        //StompBox
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d\n",
                efx_StompBox->getpar(Stomp_Level), efx_StompBox->getpar(Stomp_Tone),
                efx_StompBox->getpar(Stomp_Mid), efx_StompBox->getpar(Stomp_Bias),
                efx_StompBox->getpar(Stomp_Gain), efx_StompBox->getpar(Stomp_Mode),
                StompBox_Bypass);
        break;

    case 40:
        //Reverbtron
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

    case 41:
        //Echotron
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
    case 42:
        //StereoHarm
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_StereoHarm->getpar(Sharm_DryWet), efx_StereoHarm->getpar(Sharm_L_Gain),
                efx_StereoHarm->getpar(Sharm_L_Interval), efx_StereoHarm->getpar(Sharm_L_Chroma),
                efx_StereoHarm->getpar(Sharm_R_Gain), efx_StereoHarm->getpar(Sharm_R_Interval),
                efx_StereoHarm->getpar(Sharm_R_Chroma), efx_StereoHarm->getpar(Sharm_Select),
                efx_StereoHarm->getpar(Sharm_Note), efx_StereoHarm->getpar(Sharm_Chord),
                efx_StereoHarm->getpar(Sharm_MIDI), efx_StereoHarm->getpar(Sharm_LR_Cross),
                StereoHarm_Bypass);
        break;

    case 43:
        //CompBand
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_CompBand->getpar(CompBand_DryWet), efx_CompBand->getpar(CompBand_Low_Ratio),
                efx_CompBand->getpar(CompBand_Mid_1_Ratio), efx_CompBand->getpar(CompBand_Mid_2_Ratio),
                efx_CompBand->getpar(CompBand_High_Ratio), efx_CompBand->getpar(CompBand_Low_Thresh),
                efx_CompBand->getpar(CompBand_Mid_1_Thresh), efx_CompBand->getpar(CompBand_Mid_2_Thresh),
                efx_CompBand->getpar(CompBand_High_Thresh), efx_CompBand->getpar(CompBand_Cross_1),
                efx_CompBand->getpar(CompBand_Cross_2), efx_CompBand->getpar(CompBand_Cross_3),
                efx_CompBand->getpar(CompBand_Gain), CompBand_Bypass);
        break;

    case 44:
        //Opticaltrem
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Opticaltrem->getpar(Optical_Depth), efx_Opticaltrem->getpar(Optical_LFO_Tempo),
                efx_Opticaltrem->getpar(Optical_LFO_Random), efx_Opticaltrem->getpar(Optical_LFO_Type),
                efx_Opticaltrem->getpar(Optical_LFO_Stereo), efx_Opticaltrem->getpar(Optical_Pan),
                efx_Opticaltrem->getpar(Optical_Invert), Opticaltrem_Bypass);
        break;


    case 45:
        //Vibe
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                efx_Vibe->getpar(0), efx_Vibe->getpar(1),
                efx_Vibe->getpar(2), efx_Vibe->getpar(3),
                efx_Vibe->getpar(4), efx_Vibe->getpar(5),
                efx_Vibe->getpar(6), efx_Vibe->getpar(7),
                efx_Vibe->getpar(8), efx_Vibe->getpar(9), efx_Vibe->getpar(10),
                Vibe_Bypass);
        break;

    case 46:
        //Infinity
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


    for (int i = 0; i < 10; i++)
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
           &lv[10][0], &lv[10][1], &lv[10][2], &lv[10][3], &lv[10][4],
           &lv[10][5], &lv[10][6], &lv[10][7], &lv[10][8], &lv[10][9]);

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

    for (int i = 0; i < 12; i++)
    {
        efx_order[i] = lv[10][i];
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

    for (int j = 0; j < NumEffects; j++)
    {
        switch (j)
        {

        case 0: //EQ1
            EQ1_Bypass = 0;
            efx_EQ1->cleanup();
            for (i = 0; i < 10; i++)
            {
                efx_EQ1->changepar(i * 5 + 12, lv[7][i]);
                efx_EQ1->changepar(i * 5 + 13, lv[7][11]);
            }
            efx_EQ1->changepar(0, lv[7][10]);
            EQ1_Bypass = EQ1_B;
            break;

        case 1:// Compressor
            Compressor_Bypass = 0;
            efx_Compressor->cleanup();
            for (i = 0; i <= 8; i++)
                efx_Compressor->changepar(i, lv[9][i]);
            Compressor_Bypass = Compressor_B;
            break;

        case 2://Distortion

            Distorsion_Bypass = 0;
            efx_Distorsion->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Distorsion->changepar(i, lv[6][i]);
            Distorsion_Bypass = Distorsion_B;
            break;

        case 3://Overdrive

            Overdrive_Bypass = 0;
            efx_Overdrive->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Overdrive->changepar(i, lv[5][i]);
            Overdrive_Bypass = Overdrive_B;
            break;

        case 4://Echo

            Echo_Bypass = 0;
            efx_Echo->cleanup();
            for (i = 0; i <= 8; i++)
                efx_Echo->changepar(i, lv[1][i]);
            Echo_Bypass = Echo_B;
            break;

        case 5://Chorus

            Chorus_Bypass = 0;
            efx_Chorus->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Chorus->changepar(i, lv[2][i]);
            Chorus_Bypass = Chorus_B;
            break;

        case 6://Phaser

            Phaser_Bypass = 0;
            efx_Phaser->cleanup();
            for (i = 0; i <= 11; i++)
                efx_Phaser->changepar(i, lv[4][i]);
            Phaser_Bypass = Phaser_B;
            break;

        case 7://Flanger

            Flanger_Bypass = 0;
            efx_Flanger->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Flanger->changepar(i, lv[3][i]);
            Flanger_Bypass = Flanger_B;
            break;

        case 8://Reverb

            Reverb_Bypass = 0;
            efx_Rev->cleanup();
            for (i = 0; i <= 11; i++)
                efx_Rev->changepar(i, lv[0][i]);
            Reverb_Bypass = Reverb_B;
            break;

        case 9://EQ2

            EQ2_Bypass = 0;
            efx_EQ2->cleanup();
            for (i = 0; i < 3; i++)
            {
                efx_EQ2->changepar(i * 5 + 11, lv[8][0 + i * 3]);
                efx_EQ2->changepar(i * 5 + 12, lv[8][1 + i * 3]);
                efx_EQ2->changepar(i * 5 + 13, lv[8][2 + i * 3]);
            }
            efx_EQ2->changepar(0, lv[8][9]);
            EQ2_Bypass = EQ2_B;
            break;

        case 10://WhaWha

            WhaWha_Bypass = 0;
            efx_WhaWha->cleanup();
            efx_WhaWha->setpreset(lv[11][10]);
            for (i = 0; i <= 9; i++)
                efx_WhaWha->changepar(i, lv[11][i]);
            WhaWha_Bypass = WhaWha_B;
            break;

        case 11://Alienwah

            Alienwah_Bypass = 0;
            efx_Alienwah->cleanup();
            for (i = 0; i <= 10; i++)
                efx_Alienwah->changepar(i, lv[12][i]);
            Alienwah_Bypass = Alienwah_B;
            break;

        case 12://Cabinet

            Cabinet_Bypass = 0;
            efx_Cabinet->cleanup();
            efx_Cabinet->setpreset(lv[13][0]);
            efx_Cabinet->changepar(0, lv[13][1]);
            Cabinet_Bypass = Cabinet_B;
            break;

        case 13://Pan

            Pan_Bypass = 0;
            efx_Pan->cleanup();
            for (i = 0; i <= 8; i++)
                efx_Pan->changepar(i, lv[14][i]);
            Pan_Bypass = Pan_B;
            break;

        case 14://Harmonizer

            Harmonizer_Bypass = 0;
            efx_Har->cleanup();
            for (i = 0; i <= 10; i++)
                efx_Har->changepar(i, lv[15][i]);
            Harmonizer_Bypass = Harmonizer_B;
            break;

        case 15://MusDelay

            MusDelay_Bypass = 0;
            efx_MusDelay->cleanup();
            for (i = 0; i <= 12; i++)
                efx_MusDelay->changepar(i, lv[16][i]);
            MusDelay_Bypass = MusDelay_B;
            break;

        case 16://Gate

            Gate_Bypass = 0;
            efx_Gate->cleanup();
            for (i = 0; i <= 6; i++)
                efx_Gate->changepar(i, lv[17][i]);
            Gate_Bypass = Gate_B;
            break;

        case 17://Derelict

            Derelict_Bypass = 0;
            efx_Derelict->cleanup();
            for (i = 0; i <= 11; i++)
                efx_Derelict->changepar(i, lv[18][i]);
            Derelict_Bypass = Derelict_B;
            break;

        case 18://APhaser

            APhaser_Bypass = 0;
            efx_APhaser->cleanup();
            for (i = 0; i <= 12; i++)
                efx_APhaser->changepar(i, lv[19][i]);
            APhaser_Bypass = APhaser_B;
            break;

        case 19://Valve

            Valve_Bypass = 0;
            efx_Valve->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Valve->changepar(i, lv[20][i]);
            Valve_Bypass = Valve_B;
            break;

        case 20://DFlange

            DFlange_Bypass = 0;
            efx_DFlange->cleanup();
            for (i = 0; i <= 14; i++)
                efx_DFlange->changepar(i, lv[21][i]);
            DFlange_Bypass = DFlange_B;
            break;

        case 21://Ring

            Ring_Bypass = 0;
            efx_Ring->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Ring->changepar(i, lv[22][i]);
            Ring_Bypass = Ring_B;
            break;

        case 22://Exciter

            Exciter_Bypass = 0;
            efx_Exciter->cleanup();
            for (i = 0; i <= 12; i++)
                efx_Exciter->changepar(i, lv[23][i]);
            Exciter_Bypass = Exciter_B;
            break;

        case 23://DistBand

            DistBand_Bypass = 0;
            efx_DistBand->cleanup();
            for (i = 0; i <= 14; i++)
                efx_DistBand->changepar(i, lv[24][i]);
            DistBand_Bypass = DistBand_B;
            break;

        case 24://Arpie

            Arpie_Bypass = 0;
            efx_Arpie->cleanup();
            for (i = 0; i <= 10; i++)
                efx_Arpie->changepar(i, lv[25][i]);
            Arpie_Bypass = Arpie_B;
            break;

        case 25://Expander

            Expander_Bypass = 0;
            efx_Expander->cleanup();
            for (i = 0; i <= 6; i++)
                efx_Expander->changepar(i, lv[26][i]);
            Expander_Bypass = Expander_B;
            break;

        case 26://Shuffle

            Shuffle_Bypass = 0;
            efx_Shuffle->cleanup();
            for (i = 0; i <= 10; i++)
                efx_Shuffle->changepar(i, lv[27][i]);
            Shuffle_Bypass = Shuffle_B;
            break;

        case 27://Synthfilter

            Synthfilter_Bypass = 0;
            efx_Synthfilter->cleanup();
            for (i = 0; i <= 15; i++)
                efx_Synthfilter->changepar(i, lv[28][i]);
            Synthfilter_Bypass = Synthfilter_B;
            break;

        case 28://VaryBand

            VaryBand_Bypass = 0;
            efx_VaryBand->cleanup();
            for (i = 0; i <= 10; i++)
                efx_VaryBand->changepar(i, lv[29][i]);
            VaryBand_Bypass = VaryBand_B;
            break;

        case 29://Convolotron

            Convol_Bypass = 0;
            efx_Convol->cleanup();
            for (i = 0; i <= 10; i++)
                efx_Convol->changepar(i, lv[30][i]);
            Convol_Bypass = Convol_B;
            break;

        case 30://Looper

            Looper_Bypass = 0;
            // efx_Looper->cleanup();
            for (i = 0; i <= 13; i++)
                efx_Looper->loadpreset(i, lv[31][i]);
            Looper_Bypass = Looper_B;
            break;

        case 31://MuTroMojo

            MuTroMojo_Bypass = 0;
            efx_MuTroMojo->cleanup();
            for (i = 0; i <= 18; i++)
                efx_MuTroMojo->changepar(i, lv[32][i]);
            MuTroMojo_Bypass = MuTroMojo_B;
            break;

        case 32://Echoverse

            Echoverse_Bypass = 0;
            efx_Echoverse->cleanup();
            for (i = 0; i <= 9; i++)
                efx_Echoverse->changepar(i, lv[33][i]);
            Echoverse_Bypass = Echoverse_B;
            break;

        case 33://CoilCrafter

            CoilCrafter_Bypass = 0;
            efx_CoilCrafter->cleanup();
            for (i = 0; i <= 8; i++)
                efx_CoilCrafter->changepar(i, lv[34][i]);
            CoilCrafter_Bypass = CoilCrafter_B;
            break;

        case 34://ShelfBoost

            ShelfBoost_Bypass = 0;
            efx_ShelfBoost->cleanup();
            for (i = 0; i <= 4; i++)
                efx_ShelfBoost->changepar(i, lv[35][i]);
            ShelfBoost_Bypass = ShelfBoost_B;
            break;

        case 35://Vocoder

            Vocoder_Bypass = 0;
            efx_Vocoder->cleanup();
            for (i = 0; i <= 6; i++)
                efx_Vocoder->changepar(i, lv[36][i]);
            Vocoder_Bypass = Vocoder_B;
            break;

        case 36://Sustainer

            Sustainer_Bypass = 0;
            efx_Sustainer->cleanup();
            for (i = 0; i <= 1; i++)
                efx_Sustainer->changepar(i, lv[37][i]);
            Sustainer_Bypass = Sustainer_B;
            break;

        case 37://Sequence

            Sequence_Bypass = 0;
            efx_Sequence->cleanup();
            for (i = 0; i <= 14; i++)
                efx_Sequence->changepar(i, lv[38][i]);
            Sequence_Bypass = Sequence_B;
            break;

        case 38://Shifter

            Shifter_Bypass = 0;
            efx_Shifter->cleanup();
            for (i = 0; i <= 9; i++)
                efx_Shifter->changepar(i, lv[39][i]);
            Shifter_Bypass = Shifter_B;
            break;

        case 39://StompBox

            StompBox_Bypass = 0;
            efx_StompBox->cleanup();
            for (i = 0; i <= 5; i++)
                efx_StompBox->changepar(i, lv[40][i]);
            StompBox_Bypass = StompBox_B;
            break;

        case 40://Reverbtron

            Reverbtron_Bypass = 0;
            efx_Reverbtron->cleanup();
            for (i = 0; i <= 15; i++)
                efx_Reverbtron->changepar(i, lv[41][i]);
            Reverbtron_Bypass = Reverbtron_B;
            break;

        case 41://Echotron

            Echotron_Bypass = 0;
            efx_Echotron->cleanup();
            for (i = 0; i <= 15; i++)
                efx_Echotron->changepar(i, lv[42][i]);
            Echotron_Bypass = Echotron_B;
            break;

        case 42://StereoHarm

            StereoHarm_Bypass = 0;
            efx_StereoHarm->cleanup();
            for (i = 0; i <= 11; i++)
                efx_StereoHarm->changepar(i, lv[43][i]);
            if (lv[43][10]) RC_Stereo_Harm->cleanup();
            StereoHarm_Bypass = StereoHarm_B;
            break;

        case 43://CompBand

            CompBand_Bypass = 0;
            efx_CompBand->cleanup();
            for (i = 0; i <= 12; i++)
                efx_CompBand->changepar(i, lv[44][i]);
            CompBand_Bypass = CompBand_B;
            break;

        case 44://OpticalTrem

            Opticaltrem_Bypass = 0;
            efx_Opticaltrem->cleanup();
            for (i = 0; i <= 6; i++)
                efx_Opticaltrem->changepar(i, lv[45][i]);
            Opticaltrem_Bypass = Opticaltrem_B;
            break;

        case 45://Vibe

            Vibe_Bypass = 0;
            efx_Vibe->cleanup();
            for (i = 0; i <= 10; i++)
                efx_Vibe->changepar(i, lv[46][i]);
            Vibe_Bypass = Vibe_B;
            break;

        case 46://Infinity

            Infinity_Bypass = 0;
            efx_Infinity->cleanup();
            for (i = 0; i <= 17; i++)
                efx_Infinity->changepar(i, lv[47][i]);
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
    for (int j = 0; j < 10; j++)
    {
        active[j] = 0;
    }

    memset(Preset_Name, 0, sizeof (*Preset_Name));
    memset(efx_Convol->Filename, 0, sizeof (efx_Convol->Filename));
    memset(efx_Reverbtron->Filename, 0, sizeof (efx_Reverbtron->Filename));
    memset(efx_Echotron->Filename, 0, sizeof (efx_Echotron->Filename));
    memset(Author, 0, sizeof (*Author));
    strcpy(Author, UserRealName);
    Input_Gain = .5f;
    Master_Volume = .5f;
    Fraction_Bypass = 1.0f;
    Bypass = 0;
    memset(lv, 0, sizeof (lv));

    for (int j = 0; j <= NumEffects; j++)
    { //  <= NumEffects because presets_default has one extra #10 for order
        for (int k = 0; k < MAX_PRESET_SIZE; k++)
        {
            lv[j][k] = presets_default[j][k];
        }

        lv[j][19] = 0;
    }

    for (int k = 0; k < 12; k++)
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
    WhaWha_B = 0;
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

        for (int j = 0; j <= NumEffects; j++)
        { // <= NumEffects because presets_default has one extra #10 for order
            for (int k = 0; k < MAX_PRESET_SIZE; k++)
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
    memset(Preset_Name, 0, sizeof (*Preset_Name));
    strcpy(Preset_Name, Bank[i].Preset_Name);
    memset(Author, 0, sizeof (*Author));
    strcpy(Author, Bank[i].Author);
    memset(efx_Convol->Filename, 0, sizeof (efx_Convol->Filename));
    strcpy(efx_Convol->Filename, Bank[i].ConvoFiname);
    memset(efx_Reverbtron->Filename, 0, sizeof (efx_Reverbtron->Filename));
    strcpy(efx_Reverbtron->Filename, Bank[i].RevFiname);
    memset(efx_Echotron->Filename, 0, sizeof (efx_Echotron->Filename));
    strcpy(efx_Echotron->Filename, Bank[i].EchoFiname);

    for (int j = 0; j <= NumEffects; j++)
    { // <= NumEffects because presets_default has one extra #10 for order
        for (int k = 0; k < 20; k++)
        {
            lv[j][k] = Bank[i].lv[j][k];
        }
    }

    for (int k = 0; k < 12; k++)
    {
        efx_order[k] = Bank[i].lv[10][k];
    }

    Reverb_B = Bank[i].lv[0][19];
    Echo_B = Bank[i].lv[1][19];
    Chorus_B = Bank[i].lv[2][19];
    Flanger_B = Bank[i].lv[3][19];
    Phaser_B = Bank[i].lv[4][19];
    Overdrive_B = Bank[i].lv[5][19];
    Distorsion_B = Bank[i].lv[6][19];
    EQ1_B = Bank[i].lv[7][19];
    EQ2_B = Bank[i].lv[8][19];
    Compressor_B = Bank[i].lv[9][19];
    WhaWha_B = Bank[i].lv[11][19];
    Alienwah_B = Bank[i].lv[12][19];
    Cabinet_B = Bank[i].lv[13][19];
    Pan_B = Bank[i].lv[14][19];
    Harmonizer_B = Bank[i].lv[15][19];
    MusDelay_B = Bank[i].lv[16][19];
    Gate_B = Bank[i].lv[17][19];
    Derelict_B = Bank[i].lv[18][19];
    APhaser_B = Bank[i].lv[19][19];
    Valve_B = Bank[i].lv[20][19];
    DFlange_B = Bank[i].lv[21][19];
    Ring_B = Bank[i].lv[22][19];
    Exciter_B = Bank[i].lv[23][19];
    DistBand_B = Bank[i].lv[24][19];
    Arpie_B = Bank[i].lv[25][19];
    Expander_B = Bank[i].lv[26][19];
    Shuffle_B = Bank[i].lv[27][19];
    Synthfilter_B = Bank[i].lv[28][19];
    VaryBand_B = Bank[i].lv[29][19];
    Convol_B = Bank[i].lv[30][19];
    Looper_B = Bank[i].lv[31][19];
    MuTroMojo_B = Bank[i].lv[32][19];
    Echoverse_B = Bank[i].lv[33][19];
    CoilCrafter_B = Bank[i].lv[34][19];
    ShelfBoost_B = Bank[i].lv[35][19];
    Vocoder_B = Bank[i].lv[36][19];
    Sustainer_B = Bank[i].lv[37][19];
    Sequence_B = Bank[i].lv[38][19];
    Shifter_B = Bank[i].lv[39][19];
    StompBox_B = Bank[i].lv[40][19];
    Reverbtron_B = Bank[i].lv[41][19];
    Echotron_B = Bank[i].lv[42][19];
    StereoHarm_B = Bank[i].lv[43][19];
    CompBand_B = Bank[i].lv[44][19];
    Opticaltrem_B = Bank[i].lv[45][19];
    Vibe_B = Bank[i].lv[46][19];
    Infinity_B = Bank[i].lv[47][19];

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

    for (j = 0; j <= 11; j++)
        lv[0][j] = efx_Rev->getpar(j);
    for (j = 0; j <= 8; j++)
        lv[1][j] = efx_Echo->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[2][j] = efx_Chorus->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[3][j] = efx_Flanger->getpar(j);
    for (j = 0; j <= 11; j++)
        lv[4][j] = efx_Phaser->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[5][j] = efx_Overdrive->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[6][j] = efx_Distorsion->getpar(j);
    for (j = 0; j <= 8; j++)
        lv[9][j] = efx_Compressor->getpar(j);
    for (j = 0; j <= 9; j++)
        lv[11][j] = efx_WhaWha->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[12][j] = efx_Alienwah->getpar(j);
    for (j = 0; j <= 8; j++)
        lv[14][j] = efx_Pan->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[15][j] = efx_Har->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[16][j] = efx_MusDelay->getpar(j);
    for (j = 0; j <= 6; j++)
        lv[17][j] = efx_Gate->getpar(j);
    for (j = 0; j <= 11; j++)
        lv[18][j] = efx_Derelict->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[19][j] = efx_APhaser->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[20][j] = efx_Valve->getpar(j);
    for (j = 0; j <= 14; j++)
        lv[21][j] = efx_DFlange->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[22][j] = efx_Ring->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[23][j] = efx_Exciter->getpar(j);
    for (j = 0; j <= 14; j++)
        lv[24][j] = efx_DistBand->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[25][j] = efx_Arpie->getpar(j);
    for (j = 0; j <= 6; j++)
        lv[26][j] = efx_Expander->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[27][j] = efx_Shuffle->getpar(j);
    for (j = 0; j <= 15; j++)
        lv[28][j] = efx_Synthfilter->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[29][j] = efx_VaryBand->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[30][j] = efx_Convol->getpar(j);
    for (j = 0; j <= 13; j++)
        lv[31][j] = efx_Looper->getpar(j);
    for (j = 0; j <= 18; j++)
        lv[32][j] = efx_MuTroMojo->getpar(j);
    for (j = 0; j <= 9; j++)
        lv[33][j] = efx_Echoverse->getpar(j);
    for (j = 0; j <= 8; j++)
        lv[34][j] = efx_CoilCrafter->getpar(j);
    for (j = 0; j <= 4; j++)
        lv[35][j] = efx_ShelfBoost->getpar(j);
    for (j = 0; j <= 6; j++)
        lv[36][j] = efx_Vocoder->getpar(j);
    for (j = 0; j <= 1; j++)
        lv[37][j] = efx_Sustainer->getpar(j);
    for (j = 0; j <= 14; j++)
        lv[38][j] = efx_Sequence->getpar(j);
    for (j = 0; j <= 9; j++)
        lv[39][j] = efx_Shifter->getpar(j);
    for (j = 0; j <= 5; j++)
        lv[40][j] = efx_StompBox->getpar(j);
    for (j = 0; j <= 15; j++)
        lv[41][j] = efx_Reverbtron->getpar(j);
    for (j = 0; j <= 15; j++)
        lv[42][j] = efx_Echotron->getpar(j);
    for (j = 0; j <= 11; j++)
        lv[43][j] = efx_StereoHarm->getpar(j);
    for (j = 0; j <= 12; j++)
        lv[44][j] = efx_CompBand->getpar(j);
    for (j = 0; j <= 6; j++)
        lv[45][j] = efx_Opticaltrem->getpar(j);
    for (j = 0; j <= 10; j++)
        lv[46][j] = efx_Vibe->getpar(j);
    for (j = 0; j <= 17; j++)
        lv[47][j] = efx_Infinity->getpar(j);


    for (j = 0; j <= 12; j++)
        lv[10][j] = efx_order[j];

    for (j = 0; j < 10; j++)
        lv[7][j] = efx_EQ1->getpar(j * 5 + 12);

    lv[7][10] = efx_EQ1->getpar(0);
    lv[7][11] = efx_EQ1->getpar(13);

    for (j = 0; j < 3; j++)
    {
        lv[8][0 + j * 3] = efx_EQ2->getpar(j * 5 + 11);
        lv[8][1 + j * 3] = efx_EQ2->getpar(j * 5 + 12);
        lv[8][2 + j * 3] = efx_EQ2->getpar(j * 5 + 13);
    }

    lv[8][9] = efx_EQ2->getpar(0);

    lv[13][0] = efx_Cabinet->Cabinet_Preset;
    lv[13][1] = efx_Cabinet->getpar(0);


    for (j = 0; j <= NumEffects; j++)
    {
        for (int k = 0; k < 19; k++)
        {
            Bank[i].lv[j][k] = lv[j][k];
        }
    }

    Bank[i].lv[11][10] = efx_WhaWha->Ppreset;


    Bank[i].lv[0][19] = Reverb_Bypass;
    Bank[i].lv[1][19] = Echo_Bypass;
    Bank[i].lv[2][19] = Chorus_Bypass;
    Bank[i].lv[3][19] = Flanger_Bypass;
    Bank[i].lv[4][19] = Phaser_Bypass;
    Bank[i].lv[5][19] = Overdrive_Bypass;
    Bank[i].lv[6][19] = Distorsion_Bypass;
    Bank[i].lv[7][19] = EQ1_Bypass;
    Bank[i].lv[8][19] = EQ2_Bypass;
    Bank[i].lv[9][19] = Compressor_Bypass;
    Bank[i].lv[11][19] = WhaWha_Bypass;
    Bank[i].lv[12][19] = Alienwah_Bypass;
    Bank[i].lv[13][19] = Cabinet_Bypass;
    Bank[i].lv[14][19] = Pan_Bypass;
    Bank[i].lv[15][19] = Harmonizer_Bypass;
    Bank[i].lv[16][19] = MusDelay_Bypass;
    Bank[i].lv[17][19] = Gate_Bypass;
    Bank[i].lv[18][19] = Derelict_Bypass;
    Bank[i].lv[19][19] = APhaser_Bypass;
    Bank[i].lv[20][19] = Valve_Bypass;
    Bank[i].lv[21][19] = DFlange_Bypass;
    Bank[i].lv[22][19] = Ring_Bypass;
    Bank[i].lv[23][19] = Exciter_Bypass;
    Bank[i].lv[24][19] = DistBand_Bypass;
    Bank[i].lv[25][19] = Arpie_Bypass;
    Bank[i].lv[26][19] = Expander_Bypass;
    Bank[i].lv[27][19] = Shuffle_Bypass;
    Bank[i].lv[28][19] = Synthfilter_Bypass;
    Bank[i].lv[29][19] = VaryBand_Bypass;
    Bank[i].lv[30][19] = Convol_Bypass;
    Bank[i].lv[31][19] = Looper_Bypass;
    Bank[i].lv[32][19] = MuTroMojo_Bypass;
    Bank[i].lv[33][19] = Echoverse_Bypass;
    Bank[i].lv[34][19] = CoilCrafter_Bypass;
    Bank[i].lv[35][19] = ShelfBoost_Bypass;
    Bank[i].lv[36][19] = Vocoder_Bypass;
    Bank[i].lv[37][19] = Sustainer_Bypass;
    Bank[i].lv[38][19] = Sequence_Bypass;
    Bank[i].lv[39][19] = Shifter_Bypass;
    Bank[i].lv[40][19] = StompBox_Bypass;
    Bank[i].lv[41][19] = Reverbtron_Bypass;
    Bank[i].lv[42][19] = Echotron_Bypass;
    Bank[i].lv[43][19] = StereoHarm_Bypass;
    Bank[i].lv[44][19] = CompBand_Bypass;
    Bank[i].lv[45][19] = Opticaltrem_Bypass;
    Bank[i].lv[46][19] = Vibe_Bypass;
    Bank[i].lv[47][19] = Infinity_Bypass;


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

