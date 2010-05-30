#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include <math.h>
#define SwapFourBytes(data) ( (((data) >> 24) & 0x000000ff) | (((data) >> 8) & 0x0000ff00) | (((data) << 8) & 0x00ff0000) | (((data) << 24) & 0xff000000) )



  struct Preset_Bank_Struct
  {
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char ConvoFiname[128];
    char cInput_Gain[64];
    char cMaster_Volume[64];
    char cBalance[64];
    float Input_Gain;
    float Master_Volume;
    float Balance;
    int Bypass;
    char RevFiname[128];
    char ReservedFiname[128];
    int lv[50][20];
    int XUserMIDI[128][20];
    int XMIDIrangeMin[128];
    int XMIDIrangeMax[128];
  } Bank[62];

  struct New_Bank_Struct
  {
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char ConvoFiname[128];
    char cInput_Gain[64];
    char cMaster_Volume[64];
    char cBalance[64];
    float Input_Gain;
    float Master_Volume;
    float Balance;
    int Bypass;
    char RevFiname[128];
    char ReservedFiname[128];
    int lv[70][20];
    int XUserMIDI[128][20];
    int XMIDIrangeMin[128];
    int XMIDIrangeMax[128];
  } NewBank[62];



void
New_Bank ()
{

  int i, j, k;

  int presets[42][16] = {
//Reverb
    {80, 64, 63, 24, 0, 0, 0, 85, 5, 83, 1, 64, 0, 0, 0, 0},
//Echo
    {67, 64, 35, 64, 30, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Chorus
    {64, 64, 50, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0},
//Flanger
    {64, 64, 57, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0},
//Phaser
    {64, 64, 36, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0},
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
    {64, 64, 80, 0, 0, 64, 20, 90, 0, 60, 0, 0, 0, 0, 0, 0},
//AlienWah1
    {64, 64, 70, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0},
//Cabinet
    {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Pan
    {64, 64, 50, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
//Harmonizer
    {64, 64, 64, 12, 64, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0},
//MusicDelay
    {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0},
//NoiseGate
    {0, 0, 1, 2, 96, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//NewDist
    {0, 64, 64, 83, 65, 15, 0, 75, 31, 68, 0, 0, 0, 0, 0, 0},
//APhaser
    {64, 20, 40, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0},
//Valve
     {0, 64, 64, 127, 64, 0, 5841, 61, 1, 0, 69, 1, 80 ,0 ,0 ,0},
//Dual Flange
    {-32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0, 0},
//Ring
    {-64, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0 ,0},
//Exciter
    {127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20, 0, 0, 0 },
//MBDist
    {0, 64, 64, 56, 40, 0, 0, 0, 29, 35, 100, 0, 450, 1500, 1, 0},
//Arpie
    {67, 64, 35, 64, 30, 59, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0},
//Expander
    {-50, 20, 50, 50, 3134, 76, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
//Shuffle 1
    {64, 10, 0, 0, 0, 600, 1200, 2000, 6000,-14, 1, 0, 0 ,0 ,0, 0},
//Synthfilter
    {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0},
//MBVvol    
    {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0, 0, 0, 0, 0, 0},
//Convolotron 1
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0},
//Looper
    {64, 0, 1, 0, 1, 0, 64, 1, 0, 0, 64, 0, 0, 0, 0, 0},
//RyanWah
    {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450},       
//Echoverse
    {64, 64, 90, 64, 64, 64, 64, 0, 1, 64, 0, 0, 0, 0, 0, 0},
//CoilCrafter
    {32, 6, 1, 3300, 16, 4400, 42, 20, 0, 0, 0, 0, 0, 0, 0, 0},
//ShelfBoost
    {127, 64, 16000, 1, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Vocoder 
    {0, 64, 10, 70, 70, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Systainer
    {67, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//Sequence
    {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3, 0},
//Shifter
    {0, 64, 64, 200, 200, -20, 2, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0},
//StompBox
    {48, 32, 0, 32, 65, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
//Reverbtron
    {64, 0, 1, 1500, 0, 0, 60, 18, 4, 0, 0, 64, 0 ,0 ,0 ,0}
    

  };




  for (i = 0; i < 62; i++)
    {
      memset (NewBank[i].Preset_Name, 0, sizeof (NewBank[i].Preset_Name));
      memset (NewBank[i].Author, 0, sizeof (NewBank[i].Author));

      NewBank[i].Input_Gain = .5f;
      NewBank[i].Master_Volume = .5f;
      NewBank[i].Balance = 1.0f;
      NewBank[i].Bypass = 0;

      memset(NewBank[i].lv ,0, sizeof(NewBank[i].lv));

      for (j = 0; j < 42; j++)
	{
	  for (k = 0; k < 16; k++)
	    {
	      NewBank[i].lv[j][k] = presets[j][k];
	    }
	}
    
      memset(NewBank[i].XUserMIDI, 0, sizeof(NewBank[i].XUserMIDI));     
      memset(NewBank[i].XMIDIrangeMin, 0, sizeof(NewBank[i].XMIDIrangeMin));     
      memset(NewBank[i].XMIDIrangeMax, 0, sizeof(NewBank[i].XMIDIrangeMax));     
    
    }




};




int
loadbank (char *filename)
{

  int i,k;
  k=0;
  FILE *fn;
  if ((fn = fopen (filename, "rb")) != NULL)
    {
      while (!feof (fn))
	{
	  i = fread (&Bank, sizeof (Bank), 1, fn);
	}
      fclose (fn);
      return (1);
    }
  return (0);
};





int
savebank (char *filename)
{

 FILE *fn;
  
  if ((fn = fopen (filename, "wb")) != NULL)
 {
      fwrite (&NewBank, sizeof (NewBank), 1, fn);
      fclose (fn);
      return(1);     
  }
 return (0);
};



void
show_help ()
{
  fprintf (stderr, "Usage: rakgit2new -c Bankfile\n\n" );
  fprintf (stderr,
	   "  -h ,     --help \t\t\t display command-line help and exit\n");
  fprintf (stderr,
	   "  -c ,     --convert \t\t\t convert Bankfile\n");
	   
  fprintf (stderr, "\n");

}

int
main (int argc, char *argv[])
{

 int i,j,k,y;
 int option_index = 0, opt;
 int exitwithhelp = 0;
 char *OldBankFile = NULL;
 char NewFile1[256];

// Read command Line

  fprintf (stderr,
   "\nrakgit2new convert old git data bank files to the new file format and store the new ones in the home user directory.\nrackconvert - Copyright (c) Josep Andreu - Ryan Billing - Douglas McClendon\n\n");


  struct option opts[] = {
    {"convert", 1, NULL, 'c'},
    {"help", 0, NULL, 'h'},
    {0, 0, 0, 0}
  };

  while (1)
    {
      opt = getopt_long (argc, argv, "c:h", opts, &option_index);
      char *optarguments = optarg;

      if (opt == -1)
	break;

      switch (opt)
	{
	case 'h':
	  exitwithhelp = 1;
	  break;
       	case 'c':
        if (optarguments != NULL)
	    {
	      OldBankFile=strdup(optarguments);
	      y=loadbank(optarguments);
	      if(!y) return(0);
	      break;
            }
         }

    }

  if (exitwithhelp != 0)
    {
      show_help ();
      return (0);
    };


  if (argc < 3)
{
    fprintf (stderr, "Try 'rakgit2new --help' for usage options.\n");
    return(0);
}    


printf("converting: %s\n\n",OldBankFile);

memset(NewFile1,0,sizeof(NewFile1));

strncpy(NewFile1, OldBankFile, strlen(OldBankFile)-5);
sprintf(NewFile1, "%s_new.rkrb",NewFile1);

printf("generating %s\n",NewFile1);

New_Bank();

for (i=1; i<61; i++)
   {
     NewBank[i].Input_Gain = Bank[i].Input_Gain;
     NewBank[i].Master_Volume = Bank[i].Master_Volume;
     NewBank[i].Balance = Bank[i].Balance;
     NewBank[i].Bypass = Bank[i].Bypass;
     
     sprintf(NewBank[i].Preset_Name, "%s",Bank[i].Preset_Name);
     sprintf(NewBank[i].Author, "%s",Bank[i].Author);
     sprintf(NewBank[i].ConvoFiname, "%s",Bank[i].ConvoFiname);
     sprintf(NewBank[i].RevFiname, "%s",Bank[i].RevFiname);
   
     sprintf(NewBank[i].cInput_Gain, "%s",Bank[i].cInput_Gain);
     sprintf(NewBank[i].cMaster_Volume, "%s",Bank[i].cMaster_Volume);
     sprintf(NewBank[i].cBalance, "%s",Bank[i].cBalance);
     
      
  for(j=0;j<50;j++)
     {
       for(k=0;k<20;k++) 
         {
           NewBank[i].lv[j][k] = Bank[i].lv[j][k];
         } 
     }

   
   memcpy(NewBank[i].XUserMIDI, Bank[i].XUserMIDI, sizeof(NewBank[i].XUserMIDI));
   memcpy(NewBank[i].XMIDIrangeMin, Bank[i].XMIDIrangeMin, sizeof(NewBank[i].XMIDIrangeMin));
   memcpy(NewBank[i].XMIDIrangeMax, Bank[i].XMIDIrangeMax, sizeof(NewBank[i].XMIDIrangeMax));



}

printf("saving %s\n\n",NewFile1);
savebank(NewFile1);

return(0);

};

