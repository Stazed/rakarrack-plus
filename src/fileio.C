/*
  rakarrack - a organ software

  fileio.C  -  File Input/Output functions
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

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

#include "global.h"


void
RKR::savefile (char *filename)
{

  int i, j;
  FILE *fn;
  char buf[256];
  fn = fopen (filename, "w");

  bzero (buf, sizeof (buf));
  sprintf (buf, "%s\n", VERSION);
  fputs (buf, fn);


  //Autor

  bzero (buf, sizeof (buf));
  if (strlen (Author) != 0)
    sprintf (buf, "%s\n", Author);
  else
    {
      if (UserRealName != NULL)
	sprintf (buf, "%s\n", UserRealName);
      else
	sprintf (buf, "%s\n", getenv ("USER"));
    }
  fputs (buf, fn);

  //Preset Name

  bzero (buf, sizeof (buf));
  fputs (Preset_Name, fn);
  fputs ("\n", fn);


  //General
  bzero (buf, sizeof (buf));
  sprintf (buf, "%f,%f,%d\n", Input_Gain, Master_Volume, Bypass_B);
  fputs (buf, fn);


  for (i = 0; i < 10; i++)
    {
      j = efx_order[i];
      bzero (buf, sizeof (buf));
      switch (j)
	{
	case 8:
	  //Reverb
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Rev->getpar (0), efx_Rev->getpar (1),
		   efx_Rev->getpar (2), efx_Rev->getpar (3),
		   efx_Rev->getpar (4), efx_Rev->getpar (5),
		   efx_Rev->getpar (6), efx_Rev->getpar (7),
		   efx_Rev->getpar (8), efx_Rev->getpar (9),
		   efx_Rev->getpar (10), efx_Rev->getpar (11), Reverb_Bypass);
	  break;

	case 4:
	  //Echo
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Echo->getpar (0), efx_Echo->getpar (1),
		   efx_Echo->getpar (2), efx_Echo->getpar (3),
		   efx_Echo->getpar (4), efx_Echo->getpar (5),
		   efx_Echo->getpar (6), Echo_Bypass, efx_Echo->getpar(7));
	  break;

	case 5:
	  //Chorus
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Chorus->getpar (0), efx_Chorus->getpar (1),
		   efx_Chorus->getpar (2), efx_Chorus->getpar (3),
		   efx_Chorus->getpar (4), efx_Chorus->getpar (5),
		   efx_Chorus->getpar (6), efx_Chorus->getpar (7),
		   efx_Chorus->getpar (8), efx_Chorus->getpar (9),
		   efx_Chorus->getpar (10), efx_Chorus->getpar (11),
		   Chorus_Bypass);
	  break;

	case 7:
	  //Flanger
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Flanger->getpar (0), efx_Flanger->getpar (1),
		   efx_Flanger->getpar (2), efx_Flanger->getpar (3),
		   efx_Flanger->getpar (4), efx_Flanger->getpar (5),
		   efx_Flanger->getpar (6), efx_Flanger->getpar (7),
		   efx_Flanger->getpar (8), efx_Flanger->getpar (9),
		   efx_Flanger->getpar (10), efx_Flanger->getpar (11),
		   Flanger_Bypass);
	  break;

	case 6:
	  //Phaser
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Phaser->getpar (0), efx_Phaser->getpar (1),
		   efx_Phaser->getpar (2), efx_Phaser->getpar (3),
		   efx_Phaser->getpar (4), efx_Phaser->getpar (5),
		   efx_Phaser->getpar (6), efx_Phaser->getpar (7),
		   efx_Phaser->getpar (8), efx_Phaser->getpar (9),
		   efx_Phaser->getpar (10), efx_Phaser->getpar (11),
		   Phaser_Bypass);
	  break;

	case 3:
	  //Overdrive
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Overdrive->getpar (0), efx_Overdrive->getpar (1),
		   efx_Overdrive->getpar (2), efx_Overdrive->getpar (3),
		   efx_Overdrive->getpar (4), efx_Overdrive->getpar (5),
		   efx_Overdrive->getpar (6), efx_Overdrive->getpar (7),
		   efx_Overdrive->getpar (8), efx_Overdrive->getpar (9),
		   efx_Overdrive->getpar (10), efx_Overdrive->getpar (11),
		   Overdrive_Bypass);
	  break;

	case 2:
	  //Distorsion
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Distorsion->getpar (0), efx_Distorsion->getpar (1),
		   efx_Distorsion->getpar (2), efx_Distorsion->getpar (3),
		   efx_Distorsion->getpar (4), efx_Distorsion->getpar (5),
		   efx_Distorsion->getpar (6), efx_Distorsion->getpar (7),
		   efx_Distorsion->getpar (8), efx_Distorsion->getpar (9),
		   efx_Distorsion->getpar (10), efx_Distorsion->getpar (11),
		   Distorsion_Bypass,efx_Distorsion->getpar(12));
	  break;

	case 0:
	  //EQ1
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_EQ1->getpar (12), efx_EQ1->getpar (5 + 12),
		   efx_EQ1->getpar (10 + 12), efx_EQ1->getpar (15 + 12),
		   efx_EQ1->getpar (20 + 12), efx_EQ1->getpar (25 + 12),
		   efx_EQ1->getpar (30 + 12), efx_EQ1->getpar (35 + 12),
		   efx_EQ1->getpar (40 + 12), efx_EQ1->getpar (45 + 12),
		   efx_EQ1->getpar (0), efx_EQ1->getpar (13), EQ1_Bypass);
	  break;

	case 9:
	  //EQ2
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_EQ2->getpar (11), efx_EQ2->getpar (12),
		   efx_EQ2->getpar (13), efx_EQ2->getpar (5 + 11),
		   efx_EQ2->getpar (5 + 12), efx_EQ2->getpar (5 + 13),
		   efx_EQ2->getpar (10 + 11), efx_EQ2->getpar (10 + 12),
		   efx_EQ2->getpar (10 + 13), efx_EQ2->getpar (0),
		   EQ2_Bypass);
	  break;

	case 1:
	  // Compressor
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Compressor->getpar (1), efx_Compressor->getpar (2),
		   efx_Compressor->getpar (3), efx_Compressor->getpar (4),
		   efx_Compressor->getpar (5), efx_Compressor->getpar (6),
		   efx_Compressor->getpar (7), efx_Compressor->getpar (8),
		   Compressor_Bypass);
	  break;


	case 10:
	  //WhaWha
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_WhaWha->getpar (0), efx_WhaWha->getpar (1),
		   efx_WhaWha->getpar (2), efx_WhaWha->getpar (3),
		   efx_WhaWha->getpar (4), efx_WhaWha->getpar (5),
		   efx_WhaWha->getpar (6), efx_WhaWha->getpar (7),
		   efx_WhaWha->getpar (8), efx_WhaWha->getpar (9),
		   efx_WhaWha->Ppreset, WhaWha_Bypass);
	  break;

	case 11:
	  //Alienwah
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Alienwah->getpar (0), efx_Alienwah->getpar (1),
		   efx_Alienwah->getpar (2), efx_Alienwah->getpar (3),
		   efx_Alienwah->getpar (4), efx_Alienwah->getpar (5),
		   efx_Alienwah->getpar (6), efx_Alienwah->getpar (7),
		   efx_Alienwah->getpar (8), efx_Alienwah->getpar (9),
		   efx_Alienwah->getpar (10), Alienwah_Bypass);
	  break;

	case 12:
	  //Cabinet
	  sprintf (buf, "%d,%d,%d\n",
		   Cabinet_Preset, efx_Cabinet->getpar (0), Cabinet_Bypass);
	  break;

	case 13:
	  //Pan
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Pan->getpar (0), efx_Pan->getpar (1),
		   efx_Pan->getpar (2), efx_Pan->getpar (3),
		   efx_Pan->getpar (4), efx_Pan->getpar (5),
		   efx_Pan->getpar (6), efx_Pan->getpar (7), 
		   efx_Pan->getpar (8), Pan_Bypass);
	  break;

	case 14:
	  //Harmonizer
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Har->getpar (0), efx_Har->getpar (1),
		   efx_Har->getpar (2), efx_Har->getpar (3),
		   efx_Har->getpar (4), efx_Har->getpar (5),
		   efx_Har->getpar (6), efx_Har->getpar (7),
		   efx_Har->getpar (8), efx_Har->getpar (9),
		   efx_Har->getpar (10), Harmonizer_Bypass);
	  break;

	case 15:
	  //MusicalDelay
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_MusDelay->getpar (0), efx_MusDelay->getpar (1),
		   efx_MusDelay->getpar (2), efx_MusDelay->getpar (3),
		   efx_MusDelay->getpar (4), efx_MusDelay->getpar (5),
		   efx_MusDelay->getpar (6), efx_MusDelay->getpar (7),
		   efx_MusDelay->getpar (8), efx_MusDelay->getpar (9),
		   efx_MusDelay->getpar (10), efx_MusDelay->getpar (11),
		   efx_MusDelay->getpar (12), MusDelay_Bypass);
	  break;

	case 16:
	  //NoiseGate
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Gate->getpar (1), efx_Gate->getpar (2),
		   efx_Gate->getpar (3), efx_Gate->getpar (4),
		   efx_Gate->getpar (5), efx_Gate->getpar (6),
		   efx_Gate->getpar (7), Gate_Bypass);
	  break;

	case 17:    
	  //NewDist
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_NewDist->getpar (0), efx_NewDist->getpar (1),
		   efx_NewDist->getpar (2), efx_NewDist->getpar (3),
		   efx_NewDist->getpar (4), efx_NewDist->getpar (5),
		   efx_NewDist->getpar (6), efx_NewDist->getpar (7),
		   efx_NewDist->getpar (8), efx_NewDist->getpar (9),
		   efx_NewDist->getpar (10), efx_NewDist->getpar (11),
		   NewDist_Bypass,efx_NewDist->getpar(11));
	  break;
         
	case 18:
	  //Analog Phaser
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_APhaser->getpar (0), efx_APhaser->getpar (1),
		   efx_APhaser->getpar (2), efx_APhaser->getpar (3),
		   efx_APhaser->getpar (4), efx_APhaser->getpar (5),
		   efx_APhaser->getpar (6), efx_APhaser->getpar (7),
		   efx_APhaser->getpar (8), efx_APhaser->getpar (9),
		   efx_APhaser->getpar (10), efx_APhaser->getpar (11),
		   APhaser_Bypass);
	  break;

	case 19:
	  //Valve
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Valve->getpar (0), efx_Valve->getpar (1),
		   efx_Valve->getpar (2), efx_Valve->getpar (3),
		   efx_Valve->getpar (4), efx_Valve->getpar (5),
		   efx_Valve->getpar (6), efx_Valve->getpar (7),
		   efx_Valve->getpar (8), efx_Valve->getpar (9),
		   efx_Valve->getpar (10), efx_Valve->getpar (11), 
		   efx_Valve->getpar (12), Valve_Bypass);
	  break;

	case 20:
	  //Dual_Flange
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_DFlange->getpar (0), efx_DFlange->getpar (1),
		   efx_DFlange->getpar (2), efx_DFlange->getpar (3),
		   efx_DFlange->getpar (4), efx_DFlange->getpar (5),
		   efx_DFlange->getpar (6), efx_DFlange->getpar (7),
		   efx_DFlange->getpar (8), efx_DFlange->getpar (9),
		   efx_DFlange->getpar (10), efx_DFlange->getpar (11),
		   efx_DFlange->getpar (12), efx_DFlange->getpar (13),
		   DFlange_Bypass);
	  break;

	case 21:
	  //Ring
	  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		   efx_Ring->getpar (0), efx_Ring->getpar (1),
		   efx_Ring->getpar (2), efx_Ring->getpar (3),
		   efx_Ring->getpar (4), efx_Ring->getpar (5),
		   efx_Ring->getpar (6), efx_Ring->getpar (7),
		   efx_Ring->getpar (8), efx_Ring->getpar (9),
		   efx_Ring->getpar (10), efx_Ring->getpar (11),
		   efx_Ring->getpar (12),  Ring_Bypass);
	  break;




	}
      fputs (buf, fn);

    }



  // Order
  bzero (buf, sizeof (buf));
  sprintf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	   efx_order[0], efx_order[1], efx_order[2], efx_order[3],
	   efx_order[4], efx_order[5], efx_order[6], efx_order[7],
	   efx_order[8], efx_order[9]);

  fputs (buf, fn);


for(i=0;i<128;i++)
       {
         bzero(buf,sizeof(buf));
         sprintf(buf,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           XUserMIDI[i][0], XUserMIDI[i][1], XUserMIDI[i][2], XUserMIDI[i][3], XUserMIDI[i][4],
           XUserMIDI[i][5], XUserMIDI[i][6], XUserMIDI[i][7], XUserMIDI[i][8], XUserMIDI[i][9],
           XUserMIDI[i][10], XUserMIDI[i][10], XUserMIDI[i][12], XUserMIDI[i][13], XUserMIDI[i][14],
           XUserMIDI[i][15], XUserMIDI[i][16], XUserMIDI[i][17], XUserMIDI[i][18], XUserMIDI[i][19]);
       
         fputs (buf, fn);
                 
        }

  



  fclose (fn);

}




void
RKR::loadfile (char *filename)
{

  int i, j;
  int Num_Version=0;
  float in_vol, out_vol;
  FILE *fn;
  char buf[256];
  int l[10];

  if ((fn = fopen (filename, "r")) == NULL)
    return;
  New();

  for (i = 0; i < 14; i++)
    {
      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
    }

  //Order
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	  &l[0], &l[1], &l[2], &l[3], &l[4], &l[5], &l[6], &l[7], &l[8],
	  &l[9]);


  fclose (fn);


  if ((fn = fopen (filename, "r")) == NULL)
    return;

  //Version

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%s\n", Data_Version);
  
  Num_Version=Conv_Data_Version(Data_Version);


  if (strcmp (Data_Version, "0.1.2") == 0)
    {
      l[0] = 8;
      l[1] = 4;
      l[2] = 5;
      l[3] = 7;
      l[4] = 6;
      l[5] = 3;
      l[6] = 2;
      l[7] = 0;
      l[8] = 9;
      l[9] = 1;
    }


  //Author

  bzero (Author, 64);
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);

  for (i = 0; i < 64; i++)
    if (buf[i] > 20)
      Author[i] = buf[i];


  // Preset Name

  bzero (Preset_Name, 64);
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);

  for (i = 0; i < 64; i++)
    if (buf[i] > 20)
      Preset_Name[i] = buf[i];

  //General

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f,%f,%d\n", &in_vol, &out_vol, &Bypass_B);

  if (actuvol == 0)
    {
      Input_Gain = in_vol;
      Master_Volume = out_vol;
    }


  for (i = 0; i < 10; i++)
    {
      j = l[i];

      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);



      switch (j)
	{

	case 8:
	  //Reverb
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[0][0], &lv[0][1], &lv[0][2], &lv[0][3], &lv[0][4],
		  &lv[0][5], &lv[0][6], &lv[0][7], &lv[0][8], &lv[0][9],
		  &lv[0][10], &lv[0][11], &Reverb_B);
	  break;

	case 4:
	  //Echo  
          if( Num_Version > 30)
          {
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[1][0], &lv[1][1], &lv[1][2], &lv[1][3], &lv[1][4],
		  &lv[1][5], &lv[1][6], &Echo_B, &lv[1][8]);
	  }
	  else
	 { 
	   sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[1][0], &lv[1][1], &lv[1][2], &lv[1][3], &lv[1][4],
		  &lv[1][5], &lv[1][6], &Echo_B);
	 }
	  break;

	case 5:
	  //Chorus
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[2][0], &lv[2][1], &lv[2][2], &lv[2][3], &lv[2][4],
		  &lv[2][5], &lv[2][6], &lv[2][7], &lv[2][8], &lv[2][9],
		  &lv[2][10], &lv[2][11], &Chorus_B);
	  break;

	case 7:
	  //Flanger
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[3][0], &lv[3][1], &lv[3][2], &lv[3][3], &lv[3][4],
		  &lv[3][5], &lv[3][6], &lv[3][7], &lv[3][8], &lv[3][9],
		  &lv[3][10], &lv[3][11], &Flanger_B);
	  break;

	case 6:
	  //Phaser
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[4][0], &lv[4][1], &lv[4][2], &lv[4][3], &lv[4][4],
		  &lv[4][5], &lv[4][6], &lv[4][7], &lv[4][8], &lv[4][9],
		  &lv[4][10], &lv[4][11], &Phaser_B);
	  break;

	case 3:
	  //Overdrive
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[5][0], &lv[5][1], &lv[5][2], &lv[5][3], &lv[5][4],
		  &lv[5][5], &lv[5][6], &lv[5][7], &lv[5][8], &lv[5][9],
		  &lv[5][10], &lv[5][11], &Overdrive_B);
	  break;

	case 2:
	  //Distorsion
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[6][0], &lv[6][1], &lv[6][2], &lv[6][3], &lv[6][4],
		  &lv[6][5], &lv[6][6], &lv[6][7], &lv[6][8], &lv[6][9],
		  &lv[6][10], &lv[6][11], &Distorsion_B, &lv[6][12]);
	  break;

	case 0:
	  //EQ1  
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[7][0], &lv[7][1], &lv[7][2], &lv[7][3], &lv[7][4],
		  &lv[7][5], &lv[7][6], &lv[7][7], &lv[7][8], &lv[7][9],
		  &lv[7][10], &lv[7][11], &EQ1_B);
	  break;

	case 9:
	  //EQ2  
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[8][0], &lv[8][1], &lv[8][2], &lv[8][3], &lv[8][4],
		  &lv[8][5], &lv[8][6], &lv[8][7], &lv[8][8], &lv[8][9],
		  &EQ2_B);
	  break;

	case 1:
	  //Compressor
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[9][0], &lv[9][1], &lv[9][2], &lv[9][3], &lv[9][4],
		  &lv[9][5], &lv[9][6], &lv[9][7], &Compressor_B);
	  break;

	case 10:
	  //WhaWha
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[11][0], &lv[11][1], &lv[11][2], &lv[11][3], &lv[11][4],
		  &lv[11][5], &lv[11][6], &lv[11][7], &lv[11][8], &lv[11][9],
		  &lv[11][10], &WhaWha_B);
	  break;

	case 11:
	  //Alienwah
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[12][0], &lv[12][1], &lv[12][2], &lv[12][3], &lv[12][4],
		  &lv[12][5], &lv[12][6], &lv[12][7], &lv[12][8], &lv[12][9],
		  &lv[12][10], &Alienwah_B);
	  break;

	case 12:
	  //Cabinet
	  sscanf (buf, "%d,%d,%d\n", &lv[13][0], &lv[13][1], &Cabinet_B);
	  break;

	case 13:
	  //Pan
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[14][0], &lv[14][1], &lv[14][2], &lv[14][3], &lv[14][4],
		  &lv[14][5], &lv[14][6], &lv[14][7], &lv[14][8],&Pan_B);
	  break;

	case 14:
	  //Harmonizer
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[15][0], &lv[15][1], &lv[15][2], &lv[15][3], &lv[15][4],
		  &lv[15][5], &lv[15][6], &lv[15][7], &lv[15][8], &lv[15][9],
		  &lv[15][10], &Harmonizer_B);
	  break;

	case 15:
	  //Musical Delay
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[16][0], &lv[16][1], &lv[16][2], &lv[16][3], &lv[16][4],
		  &lv[16][5], &lv[16][6], &lv[16][7], &lv[16][8], &lv[16][9],
		  &lv[16][10], &lv[16][11], &lv[16][12], &MusDelay_B);
	  break;

	case 16:
	  //NoiseGate
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[17][0], &lv[17][1], &lv[17][2], &lv[17][3], &lv[17][4],
		  &lv[17][5], &lv[17][6], &Gate_B);

	  break;

	case 17:
	  //NewDist
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[18][0], &lv[18][1], &lv[18][2], &lv[18][3], &lv[18][4],
		  &lv[18][5], &lv[18][6], &lv[18][7], &lv[18][8], &lv[18][9],
		  &lv[18][10], &lv[18][11], &Distorsion_B);
	  break;

	case 18:
	  //Analog Phaser
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[19][0], &lv[19][1], &lv[19][2], &lv[19][3], &lv[19][4],
		  &lv[19][5], &lv[19][6], &lv[19][7], &lv[19][8], &lv[19][9],
		  &lv[19][10], &lv[19][11], &APhaser_B);
	  break;

	case 19:
	  //Valve
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[20][0], &lv[20][1], &lv[20][2], &lv[20][3], &lv[20][4],
		  &lv[20][5], &lv[20][6], &lv[20][7], &lv[20][8], &lv[20][9],
		  &lv[20][10],&lv[20][11],&lv[20][12], &Valve_B);
	  break;

	case 20:
	  //Dual Flnage
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[21][0], &lv[21][1], &lv[21][2], &lv[21][3], &lv[21][4],
		  &lv[21][5], &lv[21][6], &lv[21][7], &lv[21][8], &lv[21][9],
		  &lv[21][10], &lv[21][11], &lv[21][12], &lv[21][13],&DFlange_B);
	  break;

	case 21:
	  //Ring
	  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		  &lv[22][0], &lv[22][1], &lv[22][2], &lv[22][3], &lv[22][4],
		  &lv[22][5], &lv[22][6], &lv[22][7], &lv[22][8], &lv[22][9],
		  &lv[22][10], &lv[22][11], &lv[22][12],&Ring_B);
	  break;


	}
    }

  //Order
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	  &lv[10][0], &lv[10][1], &lv[10][2], &lv[10][3], &lv[10][4],
	  &lv[10][5], &lv[10][6], &lv[10][7], &lv[10][8], &lv[10][9]);



for(i=0;i<128;i++)
       {
         bzero(buf,sizeof(buf));
         fgets (buf, sizeof buf, fn);

         sscanf(buf,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           &XUserMIDI[i][0], &XUserMIDI[i][1], &XUserMIDI[i][2], &XUserMIDI[i][3], &XUserMIDI[i][4],
           &XUserMIDI[i][5], &XUserMIDI[i][6], &XUserMIDI[i][7], &XUserMIDI[i][8], &XUserMIDI[i][9],
           &XUserMIDI[i][10], &XUserMIDI[i][10], &XUserMIDI[i][12], &XUserMIDI[i][13], &XUserMIDI[i][14],
           &XUserMIDI[i][15], &XUserMIDI[i][16], &XUserMIDI[i][17], &XUserMIDI[i][18], &XUserMIDI[i][19]);
                 
        }



  fclose (fn);

  Actualizar_Audio ();

}


void
RKR::Actualizar_Audio ()
{
  int i;


  Bypass = 0;

  Reverb_Bypass = 0;
  Echo_Bypass = 0;
  Chorus_Bypass = 0;
  Flanger_Bypass = 0;
  Phaser_Bypass = 0;
  Overdrive_Bypass = 0;
  Distorsion_Bypass = 0;
  EQ1_Bypass = 0;
  EQ2_Bypass = 0;
  Compressor_Bypass = 0;
  Alienwah_Bypass = 0;
  WhaWha_Bypass = 0;
  Cabinet_Bypass = 0;
  Pan_Bypass = 0;
  Harmonizer_Bypass = 0;
  MusDelay_Bypass = 0;
  Gate_Bypass = 0;
  NewDist_Bypass = 0;
  APhaser_Bypass = 0;
  DFlange_Bypass = 0;
  Valve_Bypass = 0;
  Ring_Bypass = 0;
  
  cleanup_efx ();

  for (i = 0; i <= 11; i++)
    efx_Rev->changepar (i, (unsigned char)lv[0][i]);
  for (i = 0; i <= 6; i++)
    efx_Echo->changepar (i, (unsigned char)lv[1][i]);
    efx_Echo->changepar (7, (unsigned char)lv[1][8]);
  for (i = 0; i <= 11; i++)
    efx_Chorus->changepar (i, lv[2][i]);
  for (i = 0; i <= 11; i++)
    efx_Flanger->changepar (i, lv[3][i]);
  for (i = 0; i <= 11; i++)
    efx_Phaser->changepar (i,lv[4][i]);
  for (i = 0; i <= 10; i++)
    efx_Overdrive->changepar (i, (unsigned char)lv[5][i]);
  for (i = 0; i <= 12; i++)
    efx_Distorsion->changepar (i, (unsigned char)lv[6][i]);
  for (i = 0; i <= 7; i++)
    efx_Compressor->Compressor_Change (i + 1, lv[9][i]);
  efx_WhaWha->setpreset (lv[11][10]);
  for (i = 0; i <= 9; i++)
    efx_WhaWha->changepar (i, lv[11][i]);
  for (i = 0; i <= 10; i++)
    efx_Alienwah->changepar (i, lv[12][i]);
  for (i = 0; i <= 8; i++)
    efx_Pan->changepar (i, lv[14][i]);
  for (i = 0; i <= 10; i++)
    efx_Har->changepar (i, (unsigned char)lv[15][i]);
  for (i = 0; i <= 12; i++)
    efx_MusDelay->changepar (i, (unsigned char)lv[16][i]);
  for (i = 0; i <= 6; i++)
    efx_Gate->Gate_Change (i + 1,lv[17][i]);
  for (i = 0; i <= 11; i++)
    efx_NewDist->changepar (i, (unsigned char)lv[18][i]);
  for (i = 0; i <= 12; i++)
    efx_APhaser->changepar (i, lv[19][i]);
  for (i = 0; i <= 12; i++)
    efx_Valve->changepar (i, lv[20][i]);
 for (i = 0; i <= 13; i++)
    efx_DFlange->changepar (i, lv[21][i]);
 for (i = 0; i <= 12; i++)
    efx_Ring->changepar (i, lv[22][i]);


  for (i = 0; i < 19; i++)
    efx_order[i] = lv[10][i];


  if (lv[15][10])
    RC->cleanup ();

  for (i = 0; i < 10; i++)
    {
      efx_EQ1->changepar (i * 5 + 12, (unsigned char)lv[7][i]);
      efx_EQ1->changepar (i * 5 + 13, (unsigned char)lv[7][11]);
    }
  efx_EQ1->changepar (0, (unsigned char)lv[7][10]);
  for (i = 0; i < 3; i++)
    {
      efx_EQ2->changepar (i * 5 + 11, (unsigned char)lv[8][0 + i * 3]);
      efx_EQ2->changepar (i * 5 + 12, (unsigned char)lv[8][1 + i * 3]);
      efx_EQ2->changepar (i * 5 + 13, (unsigned char)lv[8][2 + i * 3]);
    }
  efx_EQ2->changepar (0, (unsigned char)lv[8][9]);

  Cabinet_setpreset ((unsigned char)lv[13][0]);
  efx_Cabinet->changepar (0, (unsigned char)lv[13][1]);




  Reverb_Bypass = Reverb_B;
  Echo_Bypass = Echo_B;
  Chorus_Bypass = Chorus_B;
  Flanger_Bypass = Flanger_B;
  Phaser_Bypass = Phaser_B;
  Overdrive_Bypass = Overdrive_B;
  Distorsion_Bypass = Distorsion_B;
  EQ1_Bypass = EQ1_B;
  EQ2_Bypass = EQ2_B;
  Compressor_Bypass = Compressor_B;
  Alienwah_Bypass = Alienwah_B;
  WhaWha_Bypass = WhaWha_B;
  Cabinet_Bypass = Cabinet_B;
  Pan_Bypass = Pan_B;
  Harmonizer_Bypass = Harmonizer_B;
  MusDelay_Bypass = MusDelay_B;
  Gate_Bypass = Gate_B;
  NewDist_Bypass = NewDist_B;
  APhaser_Bypass = APhaser_B;
  Valve_Bypass = Valve_B;
  DFlange_Bypass = DFlange_B;
  Ring_Bypass = Ring_B;

  Bypass = Bypass_B;

}



int
RKR::loadbank (char *filename)
{

  int i;
  char meslabel[64];
  FILE *fn;

  bzero(meslabel,sizeof(meslabel));
  sprintf(meslabel, "%s %s",jackcliname,VERSION);

  if(CheckOldBank(filename)) 
  {
  Message(meslabel, "Can not load this file because is from a old rakarrack version,\n please use 'Convert Old Bank' menu entry in the Bank window.");
  
  return(0);
  }


  if ((fn = fopen (filename, "rb")) != NULL)
    {
      New_Bank();
      while (!feof (fn))
	{
	  i = fread (&Bank, sizeof (Bank), 1, fn);
	}
      fclose (fn);
      if(BigEndian()) fix_endianess();
      convert_IO();
      return (1);
    }
  return (0);
};


int
RKR::savebank (char *filename)
{

  FILE *fn;
  
  if ((fn = fopen (filename, "wb")) != NULL)
 {
      copy_IO();
      if(BigEndian()) fix_endianess();
      fwrite (&Bank, sizeof (Bank), 1, fn);
      if(BigEndian()) fix_endianess();
      fclose (fn);
      return(1);     
  }
 return (0);
};

void
RKR::New ()
{

  int j, k;

  int presets[23][16] = {
//Reverb
    {80, 64, 63, 24, 0, 0, 0, 85, 5, 83, 1, 64, 0, 0, 0, 0},
//Echo
    {67, 64, 35, 64, 30, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Chorus
    {64, 64, 110, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0},
//Flanger
    {64, 64, 93, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0},
//Phaser
    {64, 64, 326, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0},
//Overdrive
    {84, 64, 35, 56, 40, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0},
//Distorsion
    {0, 64, 0, 87, 14, 6, 0, 80, 30, 0, 1, 0, 0, 0, 0, 0},
//EQ1
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0},
//EQ2
    {24, 64, 64, 75, 64, 64, 113, 64, 64, 64, 0, 0, 0, 0, 0, 0},
//Compressor
    {-13, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Order
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
//WahWah
    {64, 64, 26, 0, 0, 64, 20, 90, 0, 60, 0, 0, 0, 0, 0, 0},
//AlienWah1
    {64, 64, 45, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0},
//Cabinet
    {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Pan
    {64, 64, 140, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
//Harmonizer
    {64, 64, 64, 12, 64, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0},
//MusicDelay
    {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0},
//NoiseGate
    {0, 0, 1, 2, 96, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//NewDist
    {0, 64, 64, 83, 15, 15, 0, 75, 31, 68, 0, 0, 0, 0, 0, 0},
//APhaser
    {64, 20, 254, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0},
// Valve
    {0, 64, 64, 127, 64, 0, 93, 17, 1, 0, 69, 1, 80 ,0 ,0 ,0},
// Dual Flange
    {-32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 150, 64, 1, 10, 0, 0},
//Ring
    {-64, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0 ,0}



  };


  for (j=0;j<10;j++) active[j]=0;

  bzero (Preset_Name, sizeof (Preset_Name));
  bzero (Author, sizeof (Author));

  Input_Gain = .5f;
  Master_Volume = .5f;
  Fraction_Bypass = 1.0f;
  Bypass = 0;
  memset(lv, 0 , sizeof(lv));

  for (j = 0; j < NumEffects; j++)
    {
      for (k = 0; k < 20; k++)
	{
	  lv[j][k] = presets[j][k];
	}
    }


  for (k = 0; k < 16; k++)
    efx_order[k] = presets[10][k];



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
  NewDist_B = 0;
  APhaser_B = 0;
  Valve_B = 0;
  DFlange_B = 0;
  Ring_B = 0;
  Bypass_B = 0;

  
 memset(XUserMIDI,0,sizeof(XUserMIDI));





  Actualizar_Audio ();


};



void
RKR::New_Bank ()
{

  int i, j, k;

  int presets[23][16] = {
//Reverb
    {80, 64, 63, 24, 0, 0, 0, 85, 5, 83, 1, 64, 0, 0, 0, 0},
//Echo
    {67, 64, 35, 64, 30, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Chorus
    {64, 64, 110, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0},
//Flanger
    {64, 64, 93, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0},
//Phaser
    {64, 64, 326, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0},
//Overdrive
    {84, 64, 35, 56, 40, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0},
//Distorsion
    {0, 64, 0, 87, 14, 6, 0, 80, 30, 0, 1, 0, 0, 0, 0, 0},
//EQ1
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0},
//EQ2
    {24, 64, 64, 75, 64, 64, 113, 64, 64, 64, 0, 0, 0, 0, 0, 0},
//Compressor
    {-30, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Order
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
//WahWah
    {64, 64, 26, 0, 0, 64, 20, 90, 0, 60, 0, 0, 0, 0, 0, 0},
//AlienWah1
    {64, 64, 45, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0},
//Cabinet
    {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Pan
    {64, 64, 140, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
//Harmonizer
    {64, 64, 64, 12, 64, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0},
//MusicDelay
    {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0},
//NoiseGate
    {0, 0, 1, 2, 96, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//NewDist
    {0, 64, 64, 83, 65, 15, 0, 75, 31, 68, 0, 0, 0, 0, 0, 0},
//APhaser
    {64, 20, 254, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0},
//Valve
     {0, 64, 64, 127, 64, 0, 93, 17, 1, 0, 69, 1, 80 ,0 ,0 ,0},
// Dual Flange
    {-32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 150, 64, 1, 10, 0, 0},
//Ring
    {-64, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0 ,0}
    
     
         
  };




  for (i = 0; i < 62; i++)
    {
      bzero (Bank[i].Preset_Name, sizeof (Bank[i].Preset_Name));
      bzero (Bank[i].Author, sizeof (Bank[i].Author));

      Bank[i].Input_Gain = .5f;
      Bank[i].Master_Volume = .5f;
      Bank[i].Balance = 1.0f;
      Bank[i].Bypass = 0;
      memset(Bank[i].lv , 0 , sizeof(Bank[i].lv));

      for (j = 0; j < NumEffects; j++)
	{
	  for (k = 0; k < 20; k++)
	    {
	      Bank[i].lv[j][k] = presets[j][k];
	    }
	}
    
      memset(Bank[i].XUserMIDI, 0, sizeof(Bank[i].XUserMIDI));     
    
    }




};


void
RKR::Bank_to_Preset (int i)
{

  int j, k;


  bzero (Preset_Name, sizeof (Preset_Name));
  strcpy (Preset_Name, Bank[i].Preset_Name);
  bzero (Author, sizeof (Author));
  strcpy (Author, Bank[i].Author);

  for (j = 0; j < 50; j++)
    {
      for (k = 0; k < 20; k++)
	{
	  lv[j][k] = Bank[i].lv[j][k];
	}
    }


  for (k = 0; k < 16; k++)
  efx_order[k] = Bank[i].lv[10][k];


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
  NewDist_B = Bank[i].lv[18][19];
  APhaser_B = Bank[i].lv[19][19];
  Valve_B = Bank[i].lv[20][19];
  DFlange_B = Bank[i].lv[21][19];
  Ring_B = Bank[i].lv[22][19];
  
  Bypass_B = Bypass;


  memcpy(XUserMIDI, Bank[i].XUserMIDI, sizeof(XUserMIDI));



  Actualizar_Audio ();

  if (actuvol == 0)
    {
      Input_Gain = Bank[i].Input_Gain;
      Master_Volume = Bank[i].Master_Volume;
      Fraction_Bypass = Bank[i].Balance;
    }


};


void
RKR::Preset_to_Bank (int i)
{


  int j, k;
  bzero (Bank[i].Preset_Name, sizeof (Bank[i].Preset_Name));
  strcpy (Bank[i].Preset_Name, Preset_Name);
  bzero (Bank[i].Author, sizeof (Bank[i].Author));
  strcpy (Bank[i].Author, Author);
  Bank[i].Input_Gain = Input_Gain;
  Bank[i].Master_Volume = Master_Volume;
  Bank[i].Balance = Fraction_Bypass;


  for (j = 0; j <= 11; j++)
    lv[0][j] = efx_Rev->getpar (j);
  for (j = 0; j <= 6; j++)
    lv[1][j] = efx_Echo->getpar (j);
    lv[1][8] = efx_Echo->getpar(7);
  for (j = 0; j <= 11; j++)
    lv[2][j] = efx_Chorus->getpar (j);
  for (j = 0; j <= 11; j++)
    lv[3][j] = efx_Flanger->getpar (j);
  for (j = 0; j <= 11; j++)
    lv[4][j] = efx_Phaser->getpar (j);
  for (j = 0; j <= 10; j++)
    lv[5][j] = efx_Overdrive->getpar (j);
  for (j = 0; j <= 12; j++)
    lv[6][j] = efx_Distorsion->getpar (j);
  for (j = 0; j <= 7; j++)
    lv[9][j] = efx_Compressor->getpar (j + 1);
  for (j = 0; j <= 9; j++)
    lv[11][j] = efx_WhaWha->getpar (j);
  for (j = 0; j <= 10; j++)
    lv[12][j] = efx_Alienwah->getpar (j);
  for (j = 0; j <= 8; j++)
    lv[14][j] = efx_Pan->getpar (j);
  for (j = 0; j <= 10; j++)
    lv[15][j] = efx_Har->getpar (j);
  for (j = 0; j <= 12; j++)
    lv[16][j] = efx_MusDelay->getpar (j);
  for (j = 0; j <= 6; j++)
    lv[17][j] = efx_Gate->getpar (j + 1);
  for (j = 0; j <= 11; j++)
    lv[18][j] = efx_NewDist->getpar (j);
  for (j = 0; j <= 12; j++)
    lv[19][j] = efx_APhaser->getpar(j);
  for (j = 0; j <= 12; j++)
    lv[20][j] = efx_Valve->getpar(j);
  for (j = 0; j <= 13; j++)
    lv[21][j] = efx_DFlange->getpar(j);
  for (j = 0; j <= 12; j++)
    lv[22][j] = efx_Ring->getpar(j);


  for (j = 0; j <= 16; j++)
    lv[10][j] = efx_order[j];

  for (j = 0; j < 10; j++)
    lv[7][j] = efx_EQ1->getpar (j * 5 + 12);
  lv[7][10] = efx_EQ1->getpar (0);
  lv[7][11] = efx_EQ1->getpar (13);

  for (j = 0; j < 3; j++)
    {
      lv[8][0 + j * 3] = efx_EQ2->getpar (j * 5 + 11);
      lv[8][1 + j * 3] = efx_EQ2->getpar (j * 5 + 12);
      lv[8][2 + j * 3] = efx_EQ2->getpar (j * 5 + 13);
    }
  lv[8][9] = efx_EQ2->getpar (0);

  lv[13][0] = Cabinet_Preset;
  lv[13][1] = efx_Cabinet->getpar (0);


  for (j = 0; j < 50; j++)
    {
      for (k = 0; k < 19; k++)
	{
	  Bank[i].lv[j][k] = lv[j][k];
	}
    }



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
  Bank[i].lv[11][19] = efx_WhaWha->Ppreset;
  Bank[i].lv[11][19] = WhaWha_Bypass;
  Bank[i].lv[12][19] = Alienwah_Bypass;
  Bank[i].lv[13][19] = Cabinet_Bypass;
  Bank[i].lv[14][19] = Pan_Bypass;
  Bank[i].lv[15][19] = Harmonizer_Bypass;
  Bank[i].lv[16][19] = MusDelay_Bypass;
  Bank[i].lv[17][19] = Gate_Bypass;
  Bank[i].lv[18][19] = NewDist_Bypass;
  Bank[i].lv[19][19] = APhaser_Bypass;
  Bank[i].lv[20][19] = Valve_Bypass;
  Bank[i].lv[21][19] = DFlange_Bypass;
  Bank[i].lv[22][19] = Ring_Bypass;
  
  memcpy(Bank[i].XUserMIDI,XUserMIDI,sizeof(XUserMIDI));
  
  
};




int
RKR::Conv_Data_Version(char* D_Version)

{

char tmp[8];
int NumVersion=0;


tmp[0]=D_Version[0];
tmp[1]=D_Version[2];
tmp[2]=D_Version[4];

sscanf(tmp , "%d", &NumVersion);


return(NumVersion);
}


int
RKR::BigEndian()
{
 long one= 1;
 return !(*((char *)(&one)));
}       

void
RKR::copy_IO()
{

int i;

for(i=0; i<62; i++)
{
  bzero(Bank[i].cInput_Gain, sizeof(Bank[i].cInput_Gain));
  sprintf(Bank[i].cInput_Gain, "%f", Bank[i].Input_Gain);
  bzero(Bank[i].cMaster_Volume, sizeof(Bank[i].cMaster_Volume));
  sprintf(Bank[i].cMaster_Volume, "%f", Bank[i].Master_Volume);
  bzero(Bank[i].cBalance, sizeof(Bank[i].cBalance));
  sprintf(Bank[i].cBalance, "%f", Bank[i].Balance);



}  



}

void
RKR::convert_IO()
{

int i;

for(i=0; i<62; i++)
{
   sscanf(Bank[i].cInput_Gain, "%f", &Bank[i].Input_Gain);
   if(Bank[i].Input_Gain == 0.0) Bank[i].Input_Gain=0.5f;
   
   sscanf(Bank[i].cMaster_Volume, "%f", &Bank[i].Master_Volume);
   if(Bank[i].Master_Volume == 0.0) Bank[i].Master_Volume=0.5f;

   sscanf(Bank[i].cBalance, "%f", &Bank[i].Balance);
   if(Bank[i].Balance == 0.0) Bank[i].Balance=1.0f;



}




}

void
RKR::fix_endianess()
{
  
int i,j,k;
unsigned int data;
  
for(i=0; i<62; i++)
{

   data = Bank[i].Bypass;
   data = SwapFourBytes(data);
   Bank[i].Bypass=data;
   
   for(j=0; j<50; j++)
     {
       for(k=0;k<20;k++)
           {
             data = Bank[i].lv[j][k];
             data = SwapFourBytes(data);
             Bank[i].lv[j][k]=data;
           }  
 
     } 

   for(j=0; j<128; j++)
     {
       for(k=0;k<20;k++)
           {
             data = Bank[i].XUserMIDI[j][k];
             data = SwapFourBytes(data);
             Bank[i].XUserMIDI[j][k]=data;
           }  
 
     } 






}



}


void
RKR::saveskin (char *filename)
{


  FILE *fn;
  char buf[256];
       fn = fopen (filename, "w");
        
       bzero (buf, sizeof (buf));
       sprintf (buf, "%d,%d\n", resolution,sh);
       fputs (buf, fn);
              
       bzero (buf, sizeof (buf));
       sprintf (buf, "%d,%d,%d,%d\n", sback_color,sfore_color,slabel_color,sleds_color);
       fputs (buf, fn);
              
       
       bzero (buf, sizeof (buf));
       sprintf (buf, "%s", BackgroundImage);
       fputs (buf, fn);
       fputs ("\n",fn);

       bzero (buf, sizeof (buf));
       sprintf (buf, "%d\n", relfontsize);
       fputs (buf, fn);

       bzero (buf, sizeof (buf));
       sprintf (buf, "%d\n", sschema);
       fputs (buf, fn);

 fclose (fn);      

}



void
RKR::loadskin (char *filename)
{
  unsigned int i;
  FILE *fn;
  char buf[256];

  if ((fn = fopen (filename, "r")) == NULL)
    return;

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d,%d\n", &resolution, &sh);

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d,%d,%d,%d\n", &sback_color,&sfore_color,&slabel_color,&sleds_color);

  bzero (BackgroundImage, sizeof(BackgroundImage));
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);

  for(i=0;i<256;i++) if(buf[i]>20) BackgroundImage[i]=buf[i]; 

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d\n", &relfontsize);

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d\n", &sschema);

  
fclose(fn);

}

void
RKR::dump_preset_names (void)
{
   int i;
   
   for (i = 0; i < 62; i++) {
     fprintf(stderr,
           "RKR_BANK_NAME:%d:%s\n",
           i,
           Bank[i].Preset_Name);
   }
 
}


int
RKR::CheckOldBank(char *filename)
{

long Pos,Length;
FILE *fs;

if ((fs = fopen (filename, "rb")) != NULL)

Pos = ftell(fs);

fseek(fs, 0L, SEEK_END);

Length = ftell(fs);

if (Length != 993488) return (1); else return(0);

}


void
RKR::ConvertOldFile(char * filename)
{

char buff[255];
bzero(buff,sizeof(buff));
sprintf(buff,"rakconvert -c %s",filename);
system(buff);

}



void 
RKR::update_bank()
{

int i,j;

//NewDist   

int preset[16]= {0, 64, 64, 83, 65, 15, 0, 75, 31, 68, 0, 0, 0, 0, 0, 0};
       


for (i=0; i<62; i++)
  {
 
 for (j=0; j<16; j++)   
    {
      Bank[i].lv[18][j]=preset[j];
    }   
  }
  

}





