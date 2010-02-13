/*
  rakarrack - a guitar effects software

 process.C  -  mainloop functions
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <Fl/Fl_Preferences.H>
#include "global.h"

int Pexitprogram, preset;
int commandline;
int exitwithhelp, gui, nojack;
int PERIOD;
unsigned int SAMPLE_RATE;
float val_sum;
int note_active[POLY];
int rnote[POLY];
int gate[POLY];
int reconota;
float r__ratio;
Fl_Preferences rakarrack (Fl_Preferences::USER, WEBSITE, PACKAGE);
Pixmap p, mask;
XWMHints *hints;

RKR::RKR ()
{

  char temp[128];

  nojack = 0;
  memset (Mcontrol, 0, sizeof (Mcontrol));
  Mvalue = 0;
  actuvol= 0;
  sprintf (temp, "rakarrack");
  jackclient = jack_client_open (temp, options, &status, NULL);

  if (jackclient == NULL)
    {
      fprintf (stderr, "Cannot make a jack client, is jackd running?\n");
      nojack = 1;
      exitwithhelp = 1;
      return;

    }

  strcpy (jackcliname, jack_get_client_name (jackclient));
  SAMPLE_RATE = jack_get_sample_rate (jackclient);
  PERIOD = jack_get_buffer_size (jackclient);


  Fraction_Bypass = 1.0f;
  Master_Volume = 0.50f;
  Input_Gain = 0.50f;
  Cabinet_Preset = 0;

  rakarrack.get (PrefNom ("Harmonizer Quality"), HarQual, 4);
  rakarrack.get (PrefNom ("Auto Connect Jack"), aconnect_JA, 1);
  rakarrack.get (PrefNom ("Auto Connect Num"), cuan_jack, 2);

  int i;
  bzero (temp, sizeof (temp));
  char j_names[128];

  static const char *jack_names[] =
    { "system:playback_1", "system:playback_2" };

  for (i = 0; i < cuan_jack; i++)
    {
      bzero (temp, sizeof (temp));
      sprintf (temp, "Jack Port %d", i + 1);
      if (i < 2)
 	strcpy (j_names, jack_names[i]);
      else
	strcpy (j_names, "");
      rakarrack.get (PrefNom (temp), jack_po[i].name, j_names, 128);

    }



  efxoutl = (float *) malloc (sizeof (float) * PERIOD);
  efxoutr = (float *) malloc (sizeof (float) * PERIOD);

  smpl = (float *) malloc (sizeof (float) * PERIOD);
  smpr = (float *) malloc (sizeof (float) * PERIOD);

  anall = (float *) malloc (sizeof (float) * PERIOD);
  analr = (float *) malloc (sizeof (float) * PERIOD);


  denormal = (float *) malloc (sizeof (float) * PERIOD);

  for (i = 0; i < PERIOD; i++)

    denormal[i] = (float)(RND-0.5) * 1e-16f;


  efx_Chorus = new Chorus (efxoutl, efxoutr);
  efx_Flanger = new Chorus (efxoutl, efxoutr);
  efx_Rev = new Reverb (efxoutl, efxoutr);
  efx_Echo = new Echo (efxoutl, efxoutr);
  efx_Phaser = new Phaser (efxoutl, efxoutr);
  efx_APhaser = new Analog_Phaser(efxoutl, efxoutr);
  efx_Distorsion = new Distorsion (efxoutl, efxoutr);
  efx_Overdrive = new Distorsion (efxoutl, efxoutr);
  efx_EQ2 = new EQ (efxoutl, efxoutr);
  efx_EQ1 = new EQ (efxoutl, efxoutr);
  efx_Compressor = new Compressor (efxoutl, efxoutr);
  efx_WhaWha = new DynamicFilter (efxoutl, efxoutr);
  efx_Alienwah = new Alienwah (efxoutl, efxoutr);
  efx_Cabinet = new EQ (efxoutl, efxoutr);
  efx_Pan = new Pan (efxoutl, efxoutr);
  efx_Har = new Harmonizer (efxoutl, efxoutr, (long) HarQual);
  efx_MusDelay = new MusicDelay (efxoutl, efxoutr);
  efx_Gate = new Gate (efxoutl, efxoutr);
  efx_NewDist = new NewDist(efxoutl, efxoutr);
  efx_FLimiter = new Compressor (efxoutl, efxoutr);
  efx_Valve = new Valve(efxoutl, efxoutr);   
  efx_DFlange = new Dflange(efxoutl,efxoutr);
  efx_Ring = new Ring(efxoutl,efxoutr);
  efx_Exciter = new Exciter(efxoutl,efxoutr);
  efx_MBDist = new MBDist(efxoutl,efxoutr);
  efx_Arpie = new Arpie(efxoutl,efxoutr);
  efx_Tuner = new Tuner ();
  efx_MIDIConverter = new MIDIConverter();
  RecNote = new Recognize (efxoutl, efxoutr);
  RC = new RecChord ();


  Data_Version = (char *) malloc (sizeof (char) * 16);
  memset (Data_Version, 0, sizeof (char) * 16);
  Preset_Name = (char *) malloc (sizeof (char) * 64);
  memset (Preset_Name, 0, sizeof (char) * 64);
  Author = (char *) malloc (sizeof (char) * 64);
  memset (Author, 0, sizeof (char) * 64);
  Bank_Saved = (char *) malloc (sizeof (char) * 128);
  memset (Bank_Saved, 0, sizeof (char) * 128);
  UserRealName = (char *) malloc (sizeof (char) * 128);
  memset (UserRealName, 0, sizeof (char) * 128);

// Names

  NumEffects = 25;

  {
    static const char *los_names[] =
      { "EQ", "Compressor", "Distortion", "Overdrive", "Echo", "Chorus",
      "Phaser", "Flanger", "Reverb",
      "Parametric EQ", "WahWah", "AlienWah", "Cabinet", "Pan", "Harmonizer",
      "MusicalDelay", "NoiseGate", "Derelict", "Analog Phaser", "Valve", "Dual Flange", "Ring", "Exciter",
      "DistBand", "Arpie" 
    };
    for (i = 0; i < NumEffects; i++)
      strcpy (efx_names[i].Nom, los_names[i]);
  }


  NumParams= 186;

{
   static const char *los_params[] = 
     {
      "WahWah Depth","Derelict Dist Drive","Derelict Dist Level","Derelict Dist LPF",
      "Derelict Dist HPF","Derelict Dist Color","Derelict Sub Octave","Distortion Sub Octave",
      "Alienwah Depth","Phaser Depth","Flanger Depth","Chorus Depth","Musical Delay Gain 1",
      "Musical Delay Gain 2","Harmonizer Freq","Harmonizer Interval","Wah Wah Wet/Dry",
      "Overdrive Wet/Dry","Distortion Wet/Dry","Harmonizer Wet/Dry","Echo Pan","Overdrive Pan",
      "Distortion Pan","Harmonizer Pan","Chorus Pan","Flanger Pan","Chorus Wet/Dry","Flanger Wet/Dry",
      "Phaser Wet/Dry","Alienwah Wet/Dry","Musical Delay Wet/Dry","Reverb Wet/Dry","AutoPan/Extra Stereo Wet/Dry",
      "Echo Wet/Dry","Phaser Pan","Alienwah Pan","Musical Delay Pan 1","Reverb Pan","Musical Delay Pan 2",
      "WahWah Pan","AutoPan/Extra Stereo Pan","Overdrive Drive","Distortion Drive","Overdrive Level","Distortion Level",
      "Chorus Freq.","Flanger Freq.","Phaser Freq.","WahWah Freq.","Alienwah Freq.","AutoPan/Extra Stereo Freq.",
      "Echo Fb","Chorus Fb","Flanger Fb","Phaser Fb","Alienwah Fb","Musical Delay Fb 1","Musical Delay Fb 2",
      "Overdrive LPF","Distortion LPF","Reverb LPF","Overdrive LPF","Distortion HPF","Reverb HPF","Chorus LR_Cr",
      "Flanger LR_Cr","Phaser LR_Cr","Overdrive LR_Cr","Distortion LR_Cr","Alienwah LR_Cr","Echo LR_Cr",
      "Musical Delay LR_Cr","Chorus St_df","Flanger St_df","Phaser St_df","WahWah St_df.","Alienwah St_df.",
      "AutoPan/Extra Stereo St_df.","Chorus Rnd","Flanger Rnd","Phaser Rnd","WahWah Rnd.","Alienwah Rnd.",
      "AutoPan/Extra Stereo Rnd","WahWah Amp S.","WahWah Amp S.I.","WahWah Smooth","Phaser Phase","Alienwah Phase",
      "Analog Phaser Wet-Dry","Analog Phaser Distortion","Analog Phaser Freq","Analog Phaser Depth","Analog Phaser Width",
      "Analog Phaser Feedback","Analog Phaser Mismatch","Analog Phaser St.df","Derelict Wet-Dry","Derelict Dist Pan",
      "Derelict Dist LR Cross","Volume","Balance","Input", "Multi On/Off", "EQ Gain", "EQ Q", "EQ 31 Hz", "EQ 63 Hz",
      "EQ 125 Hz", "EQ 250 Hz", "EQ 500 Hz", "EQ 1 Khz", "EQ 2 Khz", "EQ 4 Khz", "EQ 8 Khz", "EQ 16 Khz", "Compressor A.Time",
      "Compressor R.Time", "Compressor Ratio", "Compressor Knee", "Compressor Threshold", "Compressor Output", "P.EQ Gain",
      "P.EQ Low Freq","P.EQ Low Gain","P.EQ Low Q","P.EQ Mid Freq","P.EQ Mid Gain","P.EQ Mid Q", "P.EQ High Freq",
      "P.EQ High Gain","P.EQ High Q", "Dual Flange WD", "Dual Flange Pan", "Dual Flange LR_Cr","Dual Flange Dpeth",
      "Dual Flange Width", "Dual Flange Offset", "Dual Flange FB", "Dual Flange LPF", "Dual Flange Tenpo", "Dual Flange St_df.",
      "Dual Flange Rnd","Valve WD","Valve LR_Cr","Valve Pan","Valve Level","Valve Drive", "Valve Dist","Valve Presence",
      "Valve LPF", "Valve HPF", "Ring WD", "Ring LR_Cr", "Ring Input", "Ring Level", "Ring Pan", "Ring Depth", "Ring Freq",
      "Ring Sin", "Ring Tri", "Ring Saw", "Ring Squ", "Exciter Gain", "Exciter LPF", "Exciter HPF", "Exciter Har 1",
      "Exciter Har 2", "Exciter Har 3","Exciter Har 4","Exciter Har 5","Exciter Har 6","Exciter Har 7","Exciter Har 8",
      "Exciter Har 9", "Exciter Har 10", "DistBand WD", "DistBand LR_Cr", "DistBand Drive", "DistBand Level", 
      "DistBand L.Gain", "DistBand M.Gain", "DistBand H.Gain", "DistBand Cross1", "DistBand Cross2", "DistBand Pan"   

      };
         for(i=0; i<NumParams; i++)
          strcpy (efx_params[i].Nom, los_params[i]); 
 }        

{
unsigned char Atos[] = { 1,2,3,4,5,6,8,9,20,21,22,23,24,25,26,27,28,29,30,31,46,47,48,49,50,51,52,53,54,55,56,
         57,58,59,60,61,62,63,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,
         92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,117,118,119,120,
         121,122,123,124,125,126,127,7,12,14,116,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,
         146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,
         172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
         198,199,200,201,202,203,204,205,206,207,208,209,210,211,212};
          
         for(i=0;i<NumParams; i++) efx_params[i].Ato=(int)Atos[i];

}


// Init Preset

  New ();

// Init Bank

  New_Bank ();

  init_rkr ();


}



RKR::~RKR ()
{

  if (exitwithhelp == 0)
    snd_seq_close (midi_in);

};




void
RKR::init_rkr ()
{

  Tuner_Bypass = 0;
  MIDIConverter_Bypass = 0;

  for (int i = 0; i <= 45; i += 5)
    {
      efx_EQ1->changepar (i + 10, 7);
      efx_EQ1->changepar (i + 14, 0);
    }

  efx_EQ1->changepar (0 + 11, 8);
  efx_EQ1->changepar (5 + 11, 22);
  efx_EQ1->changepar (10 + 11, 35);
  efx_EQ1->changepar (15 + 11, 48);
  efx_EQ1->changepar (20 + 11, 61);
  efx_EQ1->changepar (25 + 11, 74);
  efx_EQ1->changepar (30 + 11, 87);
  efx_EQ1->changepar (35 + 11, 100);
  efx_EQ1->changepar (40 + 11, 113);
  efx_EQ1->changepar (45 + 11, 126);

  for (int i = 0; i <= 10; i += 5)
    {
      efx_EQ2->changepar (i + 10, 7);
      efx_EQ2->changepar (i + 13, 64);
      efx_EQ2->changepar (i + 14, 0);

    }


efx_FLimiter->Compressor_Change(1,-3);
efx_FLimiter->Compressor_Change(2,15);
efx_FLimiter->Compressor_Change(3,-4);
efx_FLimiter->Compressor_Change(4,10);
efx_FLimiter->Compressor_Change(5,500);
efx_FLimiter->Compressor_Change(6,1);
efx_FLimiter->Compressor_Change(7,0);
efx_FLimiter->Compressor_Change(8,1);



  old_i_sum = -0.0f;
  old_v_sum = -0.0f;
  val_i_sum = -0.0f;
  val_v_sum = -0.0f;

  note_old = 0;
  nfreq_old = 0;
  afreq_old = 0;
  cents_old = 0;

  preset = 1000;
  MidiCh = 0;
  RControl = 0;
  ControlGet = 0;

  help_displayed = 0;
  modified = 0;
  


// Load Preset Bank File

  char temp[128];
  bzero (temp, sizeof (temp));
  sprintf (temp, "%s/Default.rkrb", DATADIR);
  rakarrack.get (PrefNom ("Bank Filename"), BankFilename, temp, 127);

  if (commandline == 0)
    loadbank (BankFilename);


  RC->cleanup ();
  reconota = -1;

}


void
RKR::ConnectMIDI ()
{


// Fl_Preferences rakarrack (Fl_Preferences::USER, WEBSITE, PACKAGE);

// Get config settings and init settings
// Get MIDI IN Setting

  rakarrack.get (PrefNom ("Auto Connect MIDI IN"), aconnect_MI, 0);
  rakarrack.get (PrefNom ("MIDI IN Device"), MID, "", 40);
  if (aconnect_MI)
    Conecta ();


}

int
RKR::Cabinet_setpreset (unsigned char npreset)
{

  const int PRESET_SIZE = 81;
  const int NUM_PRESETS = 11;
  unsigned char presets[NUM_PRESETS][PRESET_SIZE] = {

    //Marshall-4-12
    {2, 76, 0, 0, 0, 8, 33, 87, 71, 0, 7, 35, 77, 53, 0, 7, 70, 84, 64, 0, 
     7, 74, 49, 19, 0, 7, 92, 75, 95, 0, 7, 95, 72, 86, 0, 7, 118, 31, 64, 0,
     7, 127, 28, 68, 0, 3, 127, 0, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 
     67},


    //Celestion G12M
    {2, 67, 64, 64, 0, 7, 22, 90, 111, 0, 7, 38, 69, 59, 0, 7, 74, 56, 25, 0,
     7, 82, 56, 62, 0, 7, 90, 71, 77, 0, 7, 107, 53, 69, 0, 7, 110, 59, 83, 0,
     7, 112, 52, 102, 0, 7, 115, 43, 64, 0, 7, 125, 32, 59, 0, 3, 127, 1, 58, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     

    //Jensen Alnico P12-N
    {2, 49, 0, 0, 0, 7, 57, 46, 56, 0, 7, 83, 46, 84, 0, 7, 111, 53, 69, 0,
     7, 117, 43, 109, 0, 7, 121, 41, 86, 0, 7, 124, 41, 90, 0, 7, 127, 35, 27, 0,
     3, 127, 64, 61, 0, 7, 100, 71, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
    
    //Jensen Alnico P15-N
    {0, 64, 64, 64, 0, 2, 48, 64, 64, 0, 7, 57, 49, 43, 0, 7, 85, 68, 58, 0,
     7, 100, 46, 92, 0, 7, 113, 52, 75, 0, 7, 120, 43, 83, 0, 3, 125, 64, 41, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Eminence Delta Demon
    {2, 23, 64, 64, 0, 7, 64, 55, 43, 0, 7, 74, 72, 72, 0, 7, 82, 68, 92, 0,
     7, 91, 66, 80, 0, 7, 108, 52, 66, 0, 7, 116, 43, 81, 0, 3, 118, 64, 66, 0,
     7, 121, 58, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Celestion EVH12
    {2, 38, 64, 64, 0, 7, 81, 56, 64, 0, 7, 91, 66, 87, 0, 7, 95, 66, 92, 0,
     7, 111, 53, 87, 0, 7, 114, 52, 87, 0, 7, 119, 46, 59, 0, 3, 127, 61, 30, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Eminence Copperhead
    
    {2, 51, 64, 64, 0, 7, 51, 58, 77, 0, 7, 75, 68, 74, 0, 7, 91, 71, 77, 0,
     7, 109, 55, 87, 0, 7, 111, 58, 37, 0, 7, 125, 13, 75, 0, 3, 127, 50, 43, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Mesa Boogie
    {2, 64, 64, 64, 0, 7, 35, 93, 52, 0, 7, 65, 58, 80, 0, 7, 75, 52, 89, 0,
     7, 83, 66, 89, 0, 7, 87, 75, 86, 0, 7, 97, 75, 90, 0, 7, 108, 62, 52, 0,
     7, 119, 58, 71, 0, 3, 80, 64, 47, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Jazz-Chorus
    {4, 24, 0, 40, 0, 7, 24, 50, 43, 0, 7, 66, 38, 89, 0, 7, 97, 77, 112, 0,
     7, 111, 46, 93, 0, 7, 113, 43, 105, 0, 7, 127, 15, 40, 0, 3, 127, 58, 55, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
    
    //Vox-Bright  
    {2, 74, 64, 64, 0, 7, 47, 68, 72, 0, 7, 92, 75, 41, 0, 7, 97, 69, 96, 0,
     7, 103, 74, 74, 0, 7, 108, 44, 81, 0, 7, 114, 52, 100, 0, 7, 122, 46, 72, 0,
     7, 125, 52, 62, 0, 3, 123, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
           
    //Marshall-I
    {2, 101, 53, 64, 0, 7, 34, 80, 25, 0, 7, 65, 69, 86, 0, 7, 74, 78, 59, 0,
     7, 83, 75, 64, 0, 7, 90, 86, 78, 0, 7, 96, 95, 61, 0, 7, 110, 61, 59, 0,
     7, 114, 56, 84, 0, 7, 121, 22, 18, 0, 3, 117, 64, 61, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67}
    

  };


  if (npreset > (NUM_PRESETS -1))
    npreset = 0;
  for (int n = 0; n < 16; n++)
    {
      efx_Cabinet->changepar (n * 5 + 10, presets[npreset][n * 5]);
      efx_Cabinet->changepar (n * 5 + 11, presets[npreset][n * 5 + 1]);
      efx_Cabinet->changepar (n * 5 + 12, presets[npreset][n * 5 + 2]);
      efx_Cabinet->changepar (n * 5 + 13, presets[npreset][n * 5 + 3]);
      efx_Cabinet->changepar (n * 5 + 14, presets[npreset][n * 5 + 4]);

    }

  Cabinet_Preset = npreset;

  return (0);


};





int
RKR::EQ1_setpreset (int func, int num, unsigned char npreset)
{

  const int PRESET_SIZE = 12;
  const int NUM_PRESETS = 3;
  unsigned char presets[NUM_PRESETS][PRESET_SIZE] = {
    //Plain
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
    //Pop
    {78, 71, 64, 64, 64, 64, 64, 64, 71, 80, 64, 42},
    //Jazz
    {71, 68, 64, 64, 64, 64, 64, 64, 66, 69, 64, 40}
  };

  if (func)
    return ((int) presets[npreset][num]);

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < 10; n++)
    efx_EQ1->changepar (n * 5 + 12, presets[npreset][n]);
  efx_EQ1->changepar (0, presets[npreset][10]);
  for (int n = 0; n < 10; n++)
    efx_EQ1->changepar (n * 5 + 13, presets[npreset][11]);
  return (0);

};



int
RKR::EQ2_setpreset (int func, int num, unsigned char npreset)
{


  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 3;
  unsigned char presets[NUM_PRESETS][PRESET_SIZE] = {
    //Plain
    {24, 64, 64, 75, 64, 64, 113, 64, 64, 64},
    //Pop
    {24, 73, 45, 75, 64, 64, 113, 69, 38, 64},
    //Jazz
    {24, 71, 38, 75, 64, 64, 118, 69, 38, 64}
  };


  if (func)
    return ((int) presets[npreset][num]);

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < 3; n++)
    {
      efx_EQ2->changepar (n * 5 + 11, presets[npreset][n * 3]);
      efx_EQ2->changepar (n * 5 + 12, presets[npreset][n * 3 + 1]);
      efx_EQ2->changepar (n * 5 + 13, presets[npreset][n * 3 + 2]);
    }
  efx_EQ2->changepar (0, presets[npreset][9]);
  return (0);
};


void
RKR::Vol2_Efx ()
{
  memcpy(smpl,efxoutl, PERIOD * sizeof(float));
  memcpy(smpr,efxoutr, PERIOD * sizeof(float));
}


void
RKR::Vol3_Efx ()
{
  int i;
  float att=2.0f;

  for (i = 0; i < PERIOD; i++)
    {
      efxoutl[i] *= att;
      smpl[i] = efxoutl[i];
      efxoutr[i] *= att;
      smpr[i] = efxoutr[i];
    }

}


void
RKR::Vol_Efx (int NumEffect, float volume)
{
  int i;
  float v1, v2;
 

  if (volume < 0.5f)
    {
      v1 = 1.0f;
      v2 = volume * 2.0f;
    }
  else
    {
      v1 = (1.0f - volume) * 2.0f;
      v2 = 1.0f;
    };


  if ((NumEffect == 7) || (NumEffect == 12))
    v2 *= v2;

  for (i = 0; i < PERIOD; i++)
    {
      efxoutl[i] = smpl[i] * v2 + efxoutl[i] * v1;
      smpl[i] = efxoutl[i];
      efxoutr[i] = smpr[i] * v2 + efxoutr[i] * v1;
      smpr[i] = efxoutr[i];
    };


}


void
RKR::calculavol (int i)
{

  if (i == 1)
    Log_I_Gain = powf (Input_Gain * 2.0f, 4);
  if (i == 2)
    Log_M_Volume = powf (Master_Volume * 2.0f, 4);

}

void
RKR::Control_Gain ()
{

  int i;
  float i_sum = 1e-12f;
  float temp_sum;
  float tmp;
  
  for (i = 0; i <= PERIOD; i++)
    {
      efxoutl[i] *= Log_I_Gain;
      smpl[i] = efxoutl[i];
      efxoutr[i] *= Log_I_Gain;
      smpr[i] = efxoutr[i];
      tmp = fabsf(efxoutr[i] + efxoutl[i]);
      if (tmp > i_sum) i_sum = tmp;
    }
  temp_sum = (float)CLAMP (rap2dB (i_sum), -48.0, 15.0);
  val_i_sum = .6f * old_i_sum + .4f * temp_sum;
  val_sum = val_i_sum;

  
}


void
RKR::Control_Volume (float *origl,float *origr)
{
  int i;
  float i_sum = 1e-12f;
  float temp_sum;
  float tmp;

  if (have_signal) efx_FLimiter->out(efxoutl, efxoutr); 


   memcpy(anall, efxoutl, sizeof(float)* PERIOD);
   memcpy(analr, efxoutr, sizeof(float)* PERIOD);


  for (i = 0; i <= PERIOD; i++)
    {
      
      efxoutl[i] *= Log_M_Volume;
      efxoutr[i] *= Log_M_Volume;
      
      if (Fraction_Bypass < 1.0f)
      {
      efxoutl[i]= (origl[i] * (1.0f - Fraction_Bypass) + efxoutl[i] * Fraction_Bypass);
      efxoutr[i]= (origr[i] * (1.0f - Fraction_Bypass) + efxoutr[i] * Fraction_Bypass);
      }
      
      tmp = fabsf (efxoutr[i] + efxoutl[i]);
      if (tmp > i_sum) i_sum = tmp;

    }

     
  temp_sum = (float) CLAMP(rap2dB (i_sum), -48, 15);
  if (i_sum > 0.0004999f)  have_signal = 1;
  else  have_signal = 0;
  val_v_sum = .6f * old_v_sum + .4f * temp_sum;
  

}


void
RKR::cleanup_efx ()
{
  efx_EQ1->cleanup ();
  efx_Rev->cleanup ();
  efx_Distorsion->cleanup ();
  efx_Overdrive->cleanup ();
  efx_Compressor->cleanup ();
  efx_Echo->cleanup ();
  efx_Chorus->cleanup ();
  efx_Flanger->cleanup ();
  efx_Phaser->cleanup ();
  efx_EQ2->cleanup ();
  efx_WhaWha->cleanup ();
  efx_Alienwah->cleanup ();
  efx_Cabinet->cleanup ();
  efx_Pan->cleanup ();
  efx_Har->cleanup ();
  efx_MusDelay->cleanup ();
  efx_Gate->cleanup ();
  efx_NewDist->cleanup();
  efx_APhaser->cleanup();
  efx_Valve->cleanup();
  efx_DFlange->cleanup();
  efx_Ring->cleanup();  
  efx_Exciter->cleanup();
  efx_MBDist->cleanup();
  efx_Arpie->cleanup(); 
  RC->cleanup ();
};


void
RKR::Alg (float *inl1, float *inr1, float *origl, float *origr, void *)
{

  int i;
  efxoutl = inl1;
  efxoutr = inr1;

  if (Bypass)
    {

       Control_Gain ();
      
      if (Tuner_Bypass)
	efx_Tuner->schmittFloat (PERIOD, efxoutl, efxoutr);
      if (MIDIConverter_Bypass)
	efx_MIDIConverter->schmittFloat (PERIOD, efxoutl, efxoutr);

      if ((Harmonizer_Bypass) && (have_signal))
	{
	  if (efx_Har->mira)
	    {
	      if ((efx_Har->PMIDI) || (efx_Har->PSELECT))
		{
		  if (val_sum > -40.0f)
		    RecNote->schmittFloat (efxoutl, efxoutr);
		  if ((reconota != -1) && (reconota != last))
		    {
		      RC->Vamos (efx_Har->Pinterval - 12);
		      last = reconota;
		    }
		}

	    }
	}

      if((Ring_Bypass) && (efx_Ring->Pafreq))
        {
       	    RecNote->schmittFloat (efxoutl, efxoutr);
            if ((reconota != -1) && (reconota != last))
            {
             if(RecNote->afreq > 0.0) 
             {
             efx_Ring->Pfreq=lrintf(RecNote->lafreq);
             last = reconota; 
             // printf("%d\n"efx_Ring->Pfreq);
             }
            }
        }    
            
      for (i = 0; i < 10; i++)
	{
	    switch (efx_order[i])
	    {
	    case 0:
	      if (EQ1_Bypass)
		{
		  efx_EQ1->out (efxoutl, efxoutr);
		  Vol2_Efx ();
		}
	      break;

	    case 1:
	      if (Compressor_Bypass)
		{
		  efx_Compressor->out (efxoutl, efxoutr);
		  Vol2_Efx ();
		}
	      break;

	    case 5:
	      if (Chorus_Bypass)
		{
		  efx_Chorus->out (efxoutl, efxoutr);
		  Vol_Efx (1, efx_Chorus->outvolume);
		}
	      break;

	    case 7:
	      if (Flanger_Bypass)
		{
		  efx_Flanger->out (efxoutl, efxoutr);
		  Vol_Efx (2, efx_Flanger->outvolume);
		}
	      break;

	    case 6:
	      if (Phaser_Bypass)
	      	{
		  efx_Phaser->out (efxoutl, efxoutr);
		  Vol_Efx (3, efx_Phaser->outvolume);
		}
	      break;

	    case 2:
	      if (Distorsion_Bypass)
		{
		  efx_Distorsion->out (efxoutl, efxoutr);
		  Vol_Efx (4, efx_Distorsion->outvolume);
		}
	      break;

	    case 3:
	      if (Overdrive_Bypass)
		{
		  efx_Overdrive->out (efxoutl, efxoutr);
		  Vol_Efx (5, efx_Overdrive->outvolume);
		}
	      break;

	    case 4:
	      if (Echo_Bypass)
		{
		  efx_Echo->out (efxoutl, efxoutr);
		  Vol_Efx (6, efx_Echo->outvolume);
		}
	      break;
	    case 8:
	      if (Reverb_Bypass)
		{
		  efx_Rev->out (efxoutl, efxoutr);
		  Vol_Efx (7, efx_Rev->outvolume);
		}
	      break;

	    case 9:
	      if (EQ2_Bypass)
		{
		  efx_EQ2->out (efxoutl, efxoutr);
		  Vol2_Efx ();
		}
	      break;

	    case 10:
	      if (WhaWha_Bypass)
		{
		  efx_WhaWha->out (efxoutl, efxoutr);
		  Vol_Efx (8, efx_WhaWha->outvolume);
		}
	      break;

	    case 11:
	      if (Alienwah_Bypass)
	      	{
		  efx_Alienwah->out (efxoutl, efxoutr);
		  Vol_Efx (9, efx_Alienwah->outvolume);
		}
	      break;

	    case 12:
	      if (Cabinet_Bypass)
		{
		  efx_Cabinet->out (efxoutl, efxoutr);
		  Vol3_Efx ();
		}

	      break;

	    case 13:
	      if (Pan_Bypass)
		{
		  efx_Pan->out (efxoutl, efxoutr);
		  Vol_Efx (10, efx_Pan->outvolume);
		}
	      break;

	    case 14:
	      if (Harmonizer_Bypass)
		{
		  efx_Har->out (efxoutl, efxoutr);
		  Vol_Efx (11, efx_Har->outvolume);
		}
	      break;

	    case 15:
	      if (MusDelay_Bypass)
		{
		  efx_MusDelay->out (efxoutl, efxoutr);
		  Vol_Efx (12, efx_MusDelay->outvolume);
		}
	      break;

	    case 16:
	      if (Gate_Bypass)
		{
		  efx_Gate->out (efxoutl, efxoutr);
		  Vol2_Efx ();
		}
	      break;
            
            case 17:
              if(NewDist_Bypass)
                {
                  efx_NewDist->out (efxoutl, efxoutr);
                  Vol_Efx(13,efx_NewDist->outvolume);
                }
              break;    
	    
	    case 18:
	      if (APhaser_Bypass)
		{
		  efx_APhaser->out (efxoutl, efxoutr);
		  Vol_Efx (3, efx_APhaser->outvolume);
		}
	      break;

            case 19:
              if (Valve_Bypass)
                {
                  efx_Valve->out(efxoutl, efxoutr);
                  Vol_Efx (19, efx_Valve->outvolume);
                }
              break; 
	        
            case 20:
              if (DFlange_Bypass)
                {
                  efx_DFlange->out(efxoutl, efxoutr);
		  Vol2_Efx ();
                }
              break; 	           
	    
	    case 21:
              if (Ring_Bypass)
                {
                  efx_Ring->out(efxoutl, efxoutr);
		  Vol_Efx (21,efx_Ring->outvolume);
                }
              break; 	           

	    case 22:
              if (Exciter_Bypass)
                {
                  efx_Exciter->out(efxoutl, efxoutr);
		  Vol2_Efx();
                }
              break; 	           
	    
	    case 23:
              if (MBDist_Bypass)
                {
                  efx_MBDist->out(efxoutl, efxoutr);
		  Vol_Efx(22,efx_MBDist->outvolume);
                }
              break; 	           
	    
	     case 24:
              if (Arpie_Bypass)
                {
                  efx_Arpie->out(efxoutl, efxoutr);
		  Vol_Efx(6,efx_Arpie->outvolume);
                }
              break;  
	    }

	}


       Control_Volume (origl,origr);

    }

}

