/*
  rakarrack - a guitar effects software

 process.C  -  mainloop functions
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
float fPERIOD;
unsigned int SAMPLE_RATE;
float fSAMPLE_RATE;
float cSAMPLE_RATE;
float val_sum;
int note_active[POLY];
int rnote[POLY];
int gate[POLY];
int reconota;
float r__ratio;
int maxx_len;
Fl_Preferences rakarrack (Fl_Preferences::USER, WEBSITE, PACKAGE);
Pixmap p, mask;
XWMHints *hints;

RKR::RKR ()
{

  char temp[128];
  cpufp = 0;
  nojack = 0;
  memset (Mcontrol, 0, sizeof (Mcontrol));
  Mvalue = 0;
  actuvol= 0;
  OnCounter=0;
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
  J_SAMPLE_RATE = jack_get_sample_rate (jackclient);
  J_PERIOD = jack_get_buffer_size (jackclient);

  rakarrack.get (PrefNom ("UpSampling"), upsample, 0); 
  rakarrack.get (PrefNom ("UpQuality"), UpQual, 4); 
  rakarrack.get (PrefNom ("DownQuality"), DownQual, 4); 
  rakarrack.get (PrefNom ("UpAmount"), UpAmo, 0); 

  Adjust_Upsample();

  rakarrack.get (PrefNom ("Looper Size"), looper_size, 1);

  
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


  bogomips = 0.0f;
  i = Get_Bogomips();


  
  efxoutl = (float *) malloc (sizeof (float) * PERIOD);
  efxoutr = (float *) malloc (sizeof (float) * PERIOD);

  smpl = (float *) malloc (sizeof (float) * PERIOD);
  smpr = (float *) malloc (sizeof (float) * PERIOD);

  anall = (float *) malloc (sizeof (float) * PERIOD);
  analr = (float *) malloc (sizeof (float) * PERIOD);

  auxdata = (float *) malloc (sizeof (float) * PERIOD);
  auxresampled = (float *) malloc (sizeof (float) * PERIOD);


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
  efx_Expander = new Expander(efxoutl,efxoutr);
  efx_Shuffle = new Shuffle(efxoutl,efxoutr);
  efx_Synthfilter = new Synthfilter(efxoutl,efxoutr);
  efx_MBVvol = new MBVvol(efxoutl,efxoutr);
  efx_Convol = new Convolotron(efxoutl,efxoutr);
  efx_Looper = new Looper(efxoutl,efxoutr,looper_size);
  efx_RyanWah = new RyanWah(efxoutl,efxoutr);
  efx_RBEcho = new RBEcho(efxoutl,efxoutr);
  efx_CoilCrafter = new CoilCrafter(efxoutl,efxoutr);
  efx_ShelfBoost = new ShelfBoost(efxoutl,efxoutr);
  efx_Vocoder = new Vocoder(efxoutl,efxoutr,auxresampled);
  efx_Sustainer = new Sustainer(efxoutl,efxoutr);
  efx_Sequence = new Sequence(efxoutl,efxoutr, (long) HarQual);
  efx_Shifter =  new Shifter(efxoutl,efxoutr, (long) HarQual);
  efx_StompBox = new StompBox(efxoutl,efxoutr);
  U_Resample = new Resample(UpQual);
  D_Resample = new Resample(DownQual);
  A_Resample = new Resample(3);


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

  NumEffects = 40;

  {
    static const char *los_names[] =
      { "EQ", "Compressor", "Distortion", "Overdrive", "Echo", "Chorus",
      "Phaser", "Flanger", "Reverb", "Parametric EQ", "WahWah", "AlienWah", 
      "Cabinet", "Pan", "Harmonizer", "MusicalDelay", "NoiseGate", "Derelict",
      "Analog Phaser", "Valve", "Dual Flange", "Ring", "Exciter",  "DistBand", 
      "Arpie", "Expander", "Shuffle", "Synthfilter", "VaryBand", "Convolotron", 
      "Looper", "MuTroMojo", "Echoverse", "Coil Crafter", "ShelfBoost", "Vocoder",
      "Sustainer", "Sequence", "Shifter", "StompBox"
    };
    for (i = 0; i < NumEffects; i++)
      strcpy (efx_names[i].Nom, los_names[i]);
  }


  NumParams= 308;

{
   static const char *los_params[] = 
     {

"Alienwah Depth","20",
"Alienwah Fb","82",
"Alienwah Freq.","76",
"Alienwah LR_Cr","96",
"Alienwah Pan","61",
"Alienwah Phase","115",
"Alienwah Rnd.","109",
"Alienwah St_df.","103",
"Alienwah Wet/Dry","55",
"Analog Phaser Depth","120",
"Analog Phaser Distortion","118",
"Analog Phaser Feedback","122",
"Analog Phaser Freq","119",
"Analog Phaser Mismatch","123",
"Analog Phaser St.df","124",
"Analog Phaser Wet-Dry","117",
"Analog Phaser Width","121",
"Arpie Arpe's","213",
"Arpie Damp","219",
"Arpie Fb","218",
"Arpie LR_Cr","217",
"Arpie LRdl","216",
"Arpie Pan","214",
"Arpie Tempo","215",
"Arpie WD","212",
"AutoPan/Extra Stereo Freq.","77",
"AutoPan/Extra Stereo Pan","67",
"AutoPan/Extra Stereo Rnd","110",
"AutoPan/Extra Stereo St_df.","104",
"AutoPan/Extra Stereo Wet/Dry","58",
"Balance","12",
"Chorus Depth","23",
"Chorus Fb","79",
"Chorus Freq.","72",
"Chorus LR_Cr","91",
"Chorus Pan","50",
"Chorus Rnd","105",
"Chorus St_df","99",
"Chorus Wet/Dry","52",
"CoilCrafter Freq1","288",
"CoilCrafter Freq2","290",
"CoilCrafter Gain","286",
"CoilCrafter Q1","289",
"CoilCrafter Q2","291",
"CoilCrafter Tone","287",
"Compressor A.Time","142",
"Compressor Knee","145",
"Compressor Output","147",
"Compressor Ratio","144",
"Compressor R.Time","143",
"Compressor Threshold","146",
"Convolotron Damp","283",
"Convolotron Fb","284",
"Convolotron Length","285",
"Convolotron Level","282",
"Convolotron Pan","281",
"Convolotron WD","280",
"Derelict Dist Color","6",
"Derelict Dist Drive","2",
"Derelict Dist HPF","5",
"Derelict Dist Level","3",
"Derelict Dist LPF","4",
"Derelict Dist LR Cross","127",
"Derelict Dist Pan","126",
"Derelict Sub Octave","8",
"Derelict Wet-Dry","125",
"DistBand Cross1","209",
"DistBand Cross2","210",
"DistBand Drive","204",
"DistBand H.Gain","208",
"DistBand Level","205",
"DistBand L.Gain","206",
"DistBand LR_Cr","203",
"DistBand M.Gain","207",
"DistBand Pan","211",
"DistBand WD","202",
"Distortion Drive","69",
"Distortion HPF","89",
"Distortion Level","71",
"Distortion LPF","86",
"Distortion LR_Cr","95",
"Distortion Pan","48",
"Distortion Sub Octave","9",
"Distortion Wet/Dry","30",
"Dual Flange Dpeth","161",
"Dual Flange FB","164",
"Dual Flange LPF","165",
"Dual Flange LR_Cr","160",
"Dual Flange Offset","163",
"Dual Flange Pan","159",
"Dual Flange Rnd","168",
"Dual Flange St_df.","167",
"Dual Flange Tenpo","166",
"Dual Flange WD","158",
"Dual Flange Width","162",
"Echo Fb","78",
"Echo LR_Cr","97",
"Echo Pan","46",
"Echo Wet/Dry","59",
"Echoverse Angle","311",
"Echoverse Damp","309",
"Echoverse E.S.","310",
"Echoverse Fb","308",
"Echoverse LRdl","307",
"Echoverse Pan","305",
"Echoverse Reverse","304",
"Echoverse Tempo","306",
"Echoverse WD","303",
"EQ 125 Hz","134",
"EQ 16 Khz","141",
"EQ 1 Khz","137",
"EQ 250 Hz","135",
"EQ 2 Khz","138",
"EQ 31 Hz","132",
"EQ 4 Khz","139",
"EQ 500 Hz","136",
"EQ 63 Hz","133",
"EQ 8 Khz","140",
"EQ Gain","130",
"EQ Q","131",
"Exciter Gain","189",
"Exciter Har 10","201",
"Exciter Har 1","192",
"Exciter Har 2","193",
"Exciter Har 3","194",
"Exciter Har 4","195",
"Exciter Har 5","196",
"Exciter Har 6","197",
"Exciter Har 7","198",
"Exciter Har 8","199",
"Exciter Har 9","200",
"Exciter HPF","191",
"Exciter LPF","190",
"Expander A.Time","220",
"Expander HPF","226",
"Expander Level","224",
"Expander LPF","225",
"Expander R.Time","221",
"Expander Shape","222",
"Expander Threshold","223",
"Flanger Depth","22",
"Flanger Fb","80",
"Flanger Freq.","73",
"Flanger LR_Cr","92",
"Flanger Pan","51",
"Flanger Rnd","106",
"Flanger St_df","100",
"Flanger Wet/Dry","53",
"Harmonizer Freq","26",
"Harmonizer Interval","27",
"Harmonizer Pan","49",
"Harmonizer Wet/Dry","31",
"Input","14",
"Looper Auto Play"," 271",
"Looper Clear","279",
"Looper Level 1","268",
"Looper Level 2","269",
"Looper Play","272",
"Looper R1","275",
"Looper R2","276",
"Looper Record","274",
"Looper Reverse","270",
"Looper Stop","273",
"Looper Track 1","277",
"Looper Track 2","278",
"Looper WD","267",
"Multi On/Off","116",
"Musical Delay Fb 1","83",
"Musical Delay Fb 2","84",
"Musical Delay Gain 1","24",
"Musical Delay Gain 2","25",
"Musical Delay LR_Cr","98",
"Musical Delay Pan 1","62",
"Musical Delay Pan 2","65",
"Musical Delay Wet/Dry","56",
"MuTroMojo Amp.S","265",
"MuTroMojo BP","258",
"MuTroMojo HP","259",
"MuTroMojo LP","257",
"MuTroMojo Range","263",
"MuTroMojo Res","262",
"MuTroMojo Smooth","266",
"MuTroMojo Tempo","261",
"MuTroMojo Wah","264",
"MuTroMojo WD","256",
"MuTroMojo Width","260",
"Overdrive Drive","68",
"Overdrive Level","70",
"Overdrive LPF","85",
"Overdrive LPF","88",
"Overdrive LR_Cr","94",
"Overdrive Pan","47",
"Overdrive Wet/Dry","29",
"P.EQ Gain","148",
"P.EQ High Freq","155",
"P.EQ High Gain","156",
"P.EQ High Q","157",
"P.EQ Low Freq","149",
"P.EQ Low Gain","150",
"P.EQ Low Q","151",
"P.EQ Mid Freq","152",
"P.EQ Mid Gain","153",
"P.EQ Mid Q","154",
"Phaser Depth","21",
"Phaser Fb","81",
"Phaser Freq.","74",
"Phaser LR_Cr","93",
"Phaser Pan","60",
"Phaser Phase","114",
"Phaser Rnd","107",
"Phaser St_df","101",
"Phaser Wet/Dry","54",
"Reverb HPF","90",
"Reverb LPF","87",
"Reverb Pan","63",
"Reverb Wet/Dry","57",
"Ring Depth","183",
"Ring Freq","184",
"Ring Input","180",
"Ring Level","181",
"Ring LR_Cr","179",
"Ring Pan","182",
"Ring Saw","187",
"Ring Sin","185",
"Ring Squ","188",
"Ring Tri","186",
"Ring WD","178",
"Sequence WD","314",
"Sequence 1","315",
"Sequence 2","316",
"Sequence 3","317",
"Sequence 4","318",
"Sequence 5","319",
"Sequence 6","320",
"Sequence 7","321",
"Sequence 8","322",
"Sequence Tempo","323",
"Sequence Q","324",
"Sequence St.df","325",
"ShelfBoost Gain","292",
"ShelfBoost Level","293",
"ShelfBoost Pres","295",
"ShelfBoost Tone","294",
"Shifter WD","326",
"Shifter Int","327",
"Shifter Gain","328",
"Shifter Pan","329",
"Shifter Attack","330",
"Shifter Decay","331",
"Shifter Thrshold","332",
"Shifter Whamy","333",
"Shuffle High Freq","234",
"Shuffle High Gain","235",
"Shuffle Low Freq","228",
"Shuffle Low Gain","229",
"Shuffle M.H. Freq","232",
"Shuffle M.H. Gain","233",
"Shuffle M.L. Freq","230",
"Shuffle M.L. Gain","231",
"Shuffle Q","236",
"Shuffle WD","227",
"Sustainer Gain","312",
"Sustainer Sustain","313",
"Synthfilter A.Time","245",
"Synthfilter Depth","243",
"Synthfilter Distort","238",
"Synthfilter E.Sens","244",
"Synthfilter Fb","242",
"Synthfilter Offset","247",
"Synthfilter R.Time","246",
"Synthfilter St.df","240",
"Synthfilter Tempo","239",
"Synthfilter WD","237",
"Synthfilter Width","241",
"Valve Dist","174",
"Valve Drive","173",
"Valve HPF","177",
"Valve Level","172",
"Valve LPF","176",
"Valve LR_Cr","170",
"Valve Pan","171",
"Valve Presence","175",
"Valve WD","169",
"VaryBand Cross1","253",
"VaryBand Cross2","254",
"VaryBand Cross3","255",
"VaryBand St.df 1","250",
"VaryBand St.df 2","252",
"VaryBand Tempo 1","249",
"VaryBand Tempo 2","251",
"VaryBand WD","248",
"Vocoder Input","298",
"Vocoder Level","302",
"Vocoder Muf.","299",
"Vocoder Pan","297",
"Vocoder Q","300",
"Vocoder Ring","301",
"Vocoder WD","296",
"Volume","7",
"WahWah Amp S.","111",
"WahWah Amp S.I.","112",
"WahWah Depth","1",
"WahWah Freq.","75",
"WahWah Pan","66",
"WahWah Rnd.","108",
"WahWah Smooth","113",
"WahWah St_df.","102",
"Wah Wah Wet/Dry","28"
};
       for(i=0; i<NumParams*2; i+=2)
       {
          strcpy (efx_params[i/2].Nom, los_params[i]); 
          sscanf(los_params[i+1],"%d",&efx_params[i/2].Ato);
       }
 }

// Init Preset

  New ();

// Init Bank

  New_Bank ();
  init_rkr ();

}



RKR::~RKR ()
{
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

  efx_EQ1->changepar (0 + 11, 31);
  efx_EQ1->changepar (5 + 11, 63);
  efx_EQ1->changepar (10 + 11, 125);
  efx_EQ1->changepar (15 + 11, 250);
  efx_EQ1->changepar (20 + 11, 500);
  efx_EQ1->changepar (25 + 11, 1000);
  efx_EQ1->changepar (30 + 11, 2000);
  efx_EQ1->changepar (35 + 11, 4000);
  efx_EQ1->changepar (40 + 11, 8000);
  efx_EQ1->changepar (45 + 11, 16000);

  for (int i = 0; i <= 10; i += 5)
    {
      efx_EQ2->changepar (i + 10, 7);
      efx_EQ2->changepar (i + 13, 64);
      efx_EQ2->changepar (i + 14, 0);

    }


efx_FLimiter->Compressor_Change(1,-1);
efx_FLimiter->Compressor_Change(2,15);
efx_FLimiter->Compressor_Change(3,0);
efx_FLimiter->Compressor_Change(4,10);
efx_FLimiter->Compressor_Change(5,50);
efx_FLimiter->Compressor_Change(6,0);
efx_FLimiter->Compressor_Change(7,0);
efx_FLimiter->Compressor_Change(8,1);
efx_FLimiter->Compressor_Change(9,1);


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


  tempocnt=0;
  for(int i=0;i<6;i++)tempobuf[i]=0;  
  Tap_timeB = 0;
  Tap_Display = 0;  
  Tap_Selection = 0;
  Tap_TempoSet = 90;

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
RKR::Adjust_Upsample()
{

 if(upsample)
  {
   SAMPLE_RATE = J_SAMPLE_RATE*(UpAmo+2);
   PERIOD = J_PERIOD*(UpAmo+2);
   u_up = (double)UpAmo+2.0;
   u_down = 1.0 / u_up;
   

  }
   else
  {  
   SAMPLE_RATE = J_SAMPLE_RATE;
   PERIOD = J_PERIOD;
  }
 
   fSAMPLE_RATE = (float) SAMPLE_RATE;
   cSAMPLE_RATE = 1.0f / (float)SAMPLE_RATE;
   fPERIOD= float(PERIOD);
   t_periods = J_SAMPLE_RATE / 12 / J_PERIOD;

}





void
RKR::ConnectMIDI ()
{

// Get config settings and init settings
// Get MIDI IN Setting

  rakarrack.get (PrefNom ("Auto Connect MIDI IN"), aconnect_MI, 0);
  rakarrack.get (PrefNom ("MIDI IN Device"), MID, "", 40);
  if (aconnect_MI)
    Conecta ();


}

int
RKR::Cabinet_setpreset (int npreset)
{

  const int PRESET_SIZE = 81;
  const int NUM_PRESETS = 11;
  int presets[NUM_PRESETS][PRESET_SIZE] = {

    //Marshall-4-12
    {2, 1135, 0, 0, 0, 8, 116, 87, 71, 0, 7, 128, 77, 53, 0, 7, 825, 84, 64, 0, 
     7, 1021, 49, 19, 0, 7, 2657, 75, 95, 0, 7, 3116, 72, 86, 0, 7, 10580, 31, 64, 0,
     7, 17068, 28, 68, 0, 3, 17068, 0, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 
     67},


    //Celestion G12M
    {2, 704, 64, 64, 0, 7, 64, 90, 111, 0, 7, 151, 69, 59, 0, 7, 1021, 56, 25, 0,
     7, 1562, 56, 62, 0, 7, 2389, 71, 77, 0, 7, 5896, 53, 69, 0, 7, 6916, 59, 83, 0,
     7, 7691, 52, 102, 0, 7, 9021, 43, 64, 0, 7, 15347, 32, 59, 0, 3, 17068, 1, 58, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     

    //Jensen Alnico P12-N
    {2, 270, 0, 0, 0, 7, 414, 46, 56, 0, 7, 1647, 46, 84, 0, 7, 7293, 53, 69, 0,
     7, 10032, 43, 109, 0, 7, 12408, 41, 86, 0, 7, 14553, 41, 90, 0, 7, 17068, 35, 27, 0,
     3, 17068, 64, 61, 0, 7, 4065, 71, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
    
    //Jensen Alnico P15-N
    {0, 600, 64, 64, 0, 2, 256, 64, 64, 0, 7, 414, 49, 43, 0, 7, 1832, 68, 58, 0,
     7, 4065, 46, 92, 0, 7, 8111, 52, 75, 0, 7, 11766, 43, 83, 0, 3, 15347, 64, 41, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Eminence Delta Demon
    {2, 68, 64, 64, 0, 7, 600, 55, 43, 0, 7, 1021, 72, 72, 0, 7, 1562, 68, 92, 0,
     7, 2519, 66, 80, 0, 7, 6218, 52, 66, 0, 7, 9513, 43, 81, 0, 3, 10580, 64, 66, 0,
     7, 12408, 58, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Celestion EVH12
    {2, 151, 64, 64, 0, 7, 1481, 56, 64, 0, 7, 2519, 66, 87, 0, 7, 3116, 66, 92, 0,
     7, 7293, 53, 87, 0, 7, 8554, 52, 87, 0, 7, 11157, 46, 59, 0, 3, 17068, 61, 30, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Eminence Copperhead
    
    {2, 301, 64, 64, 0, 7, 301, 58, 77, 0, 7, 1077, 68, 74, 0, 7, 2519, 71, 77, 0,
     7, 6558, 55, 87, 0, 7, 7293, 58, 37, 0, 7, 15347, 13, 75, 0, 3, 17068, 50, 43, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Mesa Boogie
    {2, 600, 64, 64, 0, 7, 128, 93, 52, 0, 7, 633, 58, 80, 0, 7, 1077, 52, 89, 0,
     7, 1647, 66, 89, 0, 7, 2037, 75, 86, 0, 7, 3466, 75, 90, 0, 7, 6218, 62, 52, 0,
     7, 11157, 58, 71, 0, 3, 1404, 64, 47, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
     
    //Jazz-Chorus
    {4, 72, 0, 40, 0, 7, 72, 50, 43, 0, 7, 667, 38, 89, 0, 7, 3466, 77, 112, 0,
     7, 7293, 46, 93, 0, 7, 8111, 43, 105, 0, 7, 17068, 15, 40, 0, 3, 17068, 58, 55, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
    
    //Vox-Bright  
    {2, 1021, 64, 64, 0, 7, 243, 68, 72, 0, 7, 2657, 75, 41, 0, 7, 3466, 69, 96, 0,
     7, 4767, 74, 74, 0, 7, 6218, 44, 81, 0, 7, 8554, 52, 100, 0, 7, 13086, 46, 72, 0,
     7, 15347, 52, 62, 0, 3, 13800, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0,
     0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 0, 64, 64, 64, 0, 67},
           
    //Marshall-I
    {2, 4287, 53, 64, 0, 7, 122, 80, 25, 0, 7, 633, 69, 86, 0, 7, 1021, 78, 59, 0,
     7, 1647, 75, 64, 0, 7, 2389, 86, 78, 0, 7, 3286, 95, 61, 0, 7, 6916, 61, 59, 0,
     7, 8554, 56, 84, 0, 7, 12408, 22, 18, 0, 3, 10032, 64, 61, 0, 0, 64, 64, 64, 0,
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





void
RKR::EQ1_setpreset (int npreset)
{

  const int PRESET_SIZE = 12;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Plain
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
    //Pop
    {78, 71, 64, 64, 64, 64, 64, 64, 71, 80, 64, 42},
    //Jazz
    {71, 68, 64, 64, 64, 64, 64, 64, 66, 69, 64, 40}
  };

  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < 10; n++)
    efx_EQ1->changepar (n * 5 + 12, presets[npreset][n]);
  efx_EQ1->changepar (0, presets[npreset][10]);
  for (int n = 0; n < 10; n++)
    efx_EQ1->changepar (n * 5 + 13, presets[npreset][11]);

};



void
RKR::EQ2_setpreset (int npreset)
{


  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Plain
    {72, 64, 64, 1077, 64, 64, 8111, 64, 64, 64},
    //Pop
    {72, 73, 45, 1077, 64, 64, 8111, 69, 38, 64},
    //Jazz
    {72, 71, 38, 1077, 64, 64, 10580, 69, 38, 64}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < 3; n++)
    {
      efx_EQ2->changepar (n * 5 + 11, presets[npreset][n * 3]);
      efx_EQ2->changepar (n * 5 + 12, presets[npreset][n * 3 + 1]);
      efx_EQ2->changepar (n * 5 + 13, presets[npreset][n * 3 + 2]);
    }
  efx_EQ2->changepar (0, presets[npreset][9]);
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
      efxoutr[i] *= att;
    }

Vol2_Efx();

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


  if ((NumEffect == 8) || (NumEffect == 15))
    v2 *= v2;

  for (i = 0; i < PERIOD; i++)
    {
      efxoutl[i] = smpl[i] * v2 + efxoutl[i] * v1;
      efxoutr[i] = smpr[i] * v2 + efxoutr[i] * v1;
    };

  Vol2_Efx();

}


void
RKR::calculavol (int i)
{

  if (i == 1)
    Log_I_Gain = powf (Input_Gain * 2.0f, 4);
  if (i == 2)
    Log_M_Volume = powf (Master_Volume * 2.0f, 4);

}

int
RKR::checkforaux()
{
int i;

for(i=0;i<10;i++)  
if(efx_order[i]==35)
{
if (Vocoder_Bypass) return(1);
}

return(0);
    
}
void
RKR::Control_Gain (float *origl, float *origr)
{

  int i;
  float i_sum = 1e-12f;
  float temp_sum;
  float tmp;
  
  if(upsample)
  {
  U_Resample->out(origl,origr,efxoutl,efxoutr,J_PERIOD,u_up);
  if(checkforaux()) A_Resample->mono_out(auxdata,auxresampled,J_PERIOD,u_up);
  }
  else
  if(checkforaux()) memcpy(auxresampled,auxdata,sizeof(float)*J_PERIOD); 

  for (i = 0; i <= PERIOD; i++)
    {
      efxoutl[i] *= Log_I_Gain;
      efxoutr[i] *= Log_I_Gain;
      tmp = fabsf(efxoutr[i] + efxoutl[i]);
      if (tmp > i_sum) i_sum = tmp;
    }
      memcpy(smpl,efxoutl,sizeof(float)*PERIOD);
      memcpy(smpr,efxoutr,sizeof(float)*PERIOD);

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
  float Temp_M_Volume = 0.0f;
  
  if (have_signal) efx_FLimiter->out(efxoutl, efxoutr); 


   memcpy(anall, efxoutl, sizeof(float)* PERIOD);
   memcpy(analr, efxoutr, sizeof(float)* PERIOD);

   

   if(upsample)
    D_Resample->out(anall,analr,efxoutl,efxoutr,PERIOD,u_down);
      

   if (OnCounter < t_periods)
     {
       Temp_M_Volume = Log_M_Volume / (float) (t_periods - OnCounter); 
       OnCounter++;
     }  

     else Temp_M_Volume = Log_M_Volume;

  for (i = 0; i <= PERIOD; i++)
    {
      
      efxoutl[i] *= Temp_M_Volume;
      efxoutr[i] *= Temp_M_Volume;
      
      
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
  efx_Expander->cleanup();
  efx_Shuffle->cleanup();
  efx_Synthfilter->cleanup();
  efx_MBVvol->cleanup();
  efx_Convol->cleanup();
  efx_Looper->cleanup();
  efx_RyanWah->cleanup();
  efx_RBEcho->cleanup();
  efx_CoilCrafter->cleanup();
  efx_ShelfBoost->cleanup();
  efx_Vocoder->cleanup();
  efx_Sustainer->cleanup();
  efx_Sequence->cleanup();
  efx_Shifter->cleanup();
  efx_StompBox->cleanup();
  RC->cleanup();
  efx_FLimiter->cleanup();

};


void
RKR::Alg (float *inl1, float *inr1, float *origl, float *origr, void *)
{

  int i;
  efxoutl = inl1;
  efxoutr = inr1;

  if((t_timeout) && (Tap_Bypass)) TapTempo_Timeout(1); 

  if (Bypass)
    {

       Control_Gain (origl, origr);

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
		  Vol_Efx (5, efx_Chorus->outvolume);
		}
	      break;

	    case 7:
	      if (Flanger_Bypass)
		{
		  efx_Flanger->out (efxoutl, efxoutr);
		  Vol_Efx (7, efx_Flanger->outvolume);
		}
	      break;

	    case 6:
	      if (Phaser_Bypass)
	      	{
		  efx_Phaser->out (efxoutl, efxoutr);
		  Vol_Efx (6, efx_Phaser->outvolume);
		}
	      break;

	    case 2:
	      if (Distorsion_Bypass)
		{
		  efx_Distorsion->out (efxoutl, efxoutr);
		  Vol_Efx (2, efx_Distorsion->outvolume);
		}
	      break;

	    case 3:
	      if (Overdrive_Bypass)
		{
		  efx_Overdrive->out (efxoutl, efxoutr);
		  Vol_Efx (3, efx_Overdrive->outvolume);
		}
	      break;

	    case 4:
	      if (Echo_Bypass)
		{
		  efx_Echo->out (efxoutl, efxoutr);
		  Vol_Efx (4, efx_Echo->outvolume);
		}
	      break;
	    case 8:
	      if (Reverb_Bypass)
		{
		  efx_Rev->out (efxoutl, efxoutr);
		  Vol_Efx (8, efx_Rev->outvolume);
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
		  Vol_Efx (10, efx_WhaWha->outvolume);
		}
	      break;

	    case 11:
	      if (Alienwah_Bypass)
	      	{
		  efx_Alienwah->out (efxoutl, efxoutr);
		  Vol_Efx (11, efx_Alienwah->outvolume);
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
		  Vol_Efx (13, efx_Pan->outvolume);
		}
	      break;

	    case 14:
	      if (Harmonizer_Bypass)
		{
		  efx_Har->out (efxoutl, efxoutr);
		  Vol_Efx (14, efx_Har->outvolume);
		}
	      break;

	    case 15:
	      if (MusDelay_Bypass)
		{
		  efx_MusDelay->out (efxoutl, efxoutr);
		  Vol_Efx (15, efx_MusDelay->outvolume);
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
                  Vol_Efx(17,efx_NewDist->outvolume);
                }
              break;    
	    
	    case 18:
	      if (APhaser_Bypass)
		{
		  efx_APhaser->out (efxoutl, efxoutr);
		  Vol_Efx (18, efx_APhaser->outvolume);
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
		  Vol_Efx(23,efx_MBDist->outvolume);
                }
              break; 	           
	    
	     case 24:
              if (Arpie_Bypass)
                {
                  efx_Arpie->out(efxoutl, efxoutr);
		  Vol_Efx(24,efx_Arpie->outvolume);
                }
              break;  
	    	    
	     case 25:
              if (Expander_Bypass)
                {
                  efx_Expander->out(efxoutl, efxoutr);
		  Vol2_Efx();
                }
              break;  

	     case 26:
              if (Shuffle_Bypass)
                {
                  efx_Shuffle->out(efxoutl, efxoutr);
		  Vol_Efx(26,efx_Shuffle->outvolume);
                }
              break;  

	     case 27:
              if (Synthfilter_Bypass)
                {
                  efx_Synthfilter->out(efxoutl, efxoutr);
		  Vol_Efx(27,efx_Synthfilter->outvolume);
                }
              break;  

	     case 28:
              if (MBVvol_Bypass)
                {
                  efx_MBVvol->out(efxoutl, efxoutr);
		  Vol_Efx(28,efx_MBVvol->outvolume);
                }
              break;  

	     case 29:
              if (Convol_Bypass)
                {
                  efx_Convol->out(efxoutl, efxoutr);
		  Vol_Efx(29,efx_Convol->outvolume);
                }
              break;  

	     case 30:
              if (Looper_Bypass)
                {
                  efx_Looper->out(efxoutl, efxoutr);
		  Vol_Efx(30,efx_Looper->outvolume);
                }
              break;  

	     case 31:
              if (RyanWah_Bypass)
                {
                  efx_RyanWah->out(efxoutl, efxoutr);
		  Vol_Efx(31,efx_RyanWah->outvolume);
                }
              break; 
	     case 32:
              if (RBEcho_Bypass)
                {
                  efx_RBEcho->out(efxoutl, efxoutr);
		  Vol_Efx(32,efx_RBEcho->outvolume);
                }
              break; 
	     case 33:
              if (CoilCrafter_Bypass)
                {
                  efx_CoilCrafter->out(efxoutl, efxoutr);
		  Vol2_Efx();
                } 
              break;
	     case 34:
              if (ShelfBoost_Bypass)
                {
                  efx_ShelfBoost->out(efxoutl, efxoutr);
		  Vol2_Efx();
                }
              break;  

	     case 35:
              if (Vocoder_Bypass)
                {
                  efx_Vocoder->out(efxoutl, efxoutr);
		  Vol_Efx(35,efx_Vocoder->outvolume);
                }
              break;  

	     case 36:
              if (Sustainer_Bypass)
                {
                  efx_Sustainer->out(efxoutl, efxoutr);
		  Vol2_Efx();
                }
              break;  

	     case 37:
              if (Sequence_Bypass)
                {
                  efx_Sequence->out(efxoutl, efxoutr);
		  Vol_Efx(37,efx_Sequence->outvolume);
                }
              break;  

	     case 38:
              if (Shifter_Bypass)
                {
                  efx_Shifter->out(efxoutl, efxoutr);
		  Vol_Efx(38,efx_Shifter->outvolume);
                }
              break;  

	     case 39:
              if (StompBox_Bypass)
                {
                  efx_StompBox->out(efxoutl, efxoutr);
		  Vol2_Efx();
                }
              break;  

	    }

	}


       Control_Volume (origl,origr);

    }

}

