#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#define SwapFourBytes(data) ( (((data) >> 24) & 0x000000ff) | (((data) >> 8) & 0x0000ff00) | (((data) << 8) & 0x00ff0000) | (((data) << 24) & 0xff000000) )



  struct ML
  {
    int XUserMIDI[128][20];
  } PML[82]; 


  struct Preset_Bank_Struct
  {
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char Reserva[64];
    char Reserva1[64];
    float Input_Gain;
    float Master_Volume;
    int Bypass;
    int lv[24][22];
  } Bank[82];





  struct New_Bank_Struct
  {
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char Reserva[64];
    char Reserva1[64];
    char cInput_Gain[64];
    char cMaster_Volume[64];
    char cBalance[64];
    float Input_Gain;
    float Master_Volume;
    float Balance;
    int Bypass;
    int Res[64];
    int lv[50][20];
    int XUserMIDI[128][20];
    int XMIDIrangeMin[128];
    int XMIDIrangeMax[128];
  } NewBank[62];



int
BigEndian()
{
 long one= 1;
 return !(*((char *)(&one)));
}       




void
copy_IO()
{

int i;

for(i=0; i<62; i++)
{
  bzero(NewBank[i].cInput_Gain, sizeof(NewBank[i].cInput_Gain));
  sprintf(NewBank[i].cInput_Gain, "%f", NewBank[i].Input_Gain);
  bzero(NewBank[i].cMaster_Volume, sizeof(NewBank[i].cMaster_Volume));
  sprintf(NewBank[i].cMaster_Volume, "%f", NewBank[i].Master_Volume);
  bzero(NewBank[i].cBalance, sizeof(NewBank[i].cBalance));
  sprintf(NewBank[i].cBalance, "%f", NewBank[i].Balance);
}  
}


void
convert_IO()
{

int i;

for(i=0; i<82; i++)
{
   sscanf(Bank[i].Reserva, "%f", &Bank[i].Input_Gain);
   if(Bank[i].Input_Gain == 0.0) Bank[i].Input_Gain=0.5f;
   
   sscanf(Bank[i].Reserva1, "%f", &Bank[i].Master_Volume);
   if(Bank[i].Master_Volume == 0.0) Bank[i].Master_Volume=0.5f;
}




}


void
old_fix_endianess()
{
  
int i,j,k;
unsigned int data;
  
for(i=0; i<82; i++)
{

   data = Bank[i].Bypass;
   data = SwapFourBytes(data);
   Bank[i].Bypass=data;
   
   for(j=0; j<24; j++)
     {
       for(k=0;k<22;k++)
           {
             data = Bank[i].lv[j][k];
             data = SwapFourBytes(data);
             Bank[i].lv[j][k]=data;
           }  
 
     } 

}



}




void
fix_endianess()
{
  
int i,j,k;
unsigned int data;
  
for(i=0; i<62; i++)
{

   data = NewBank[i].Bypass;
   data = SwapFourBytes(data);
   NewBank[i].Bypass=data;
   
   for(j=0; j<50; j++)
     {
       for(k=0;k<20;k++)
           {
             data = NewBank[i].lv[j][k];
             data = SwapFourBytes(data);
             NewBank[i].lv[j][k]=data;
           }  
 
     } 

   for(j=0; j<128; j++)
     {
       for(k=0;k<20;k++)
           {
             data = NewBank[i].XUserMIDI[j][k];
             data = SwapFourBytes(data);
             NewBank[i].XUserMIDI[j][k]=data;
           }  
 
     } 

}
}


void
New_Bank ()
{

  int i, j, k;

  int presets[20][16] = {
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
    {64, 20, 40, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0}
  };




  for (i = 0; i < 62; i++)
    {
      bzero (NewBank[i].Preset_Name, sizeof (NewBank[i].Preset_Name));
      bzero (NewBank[i].Author, sizeof (NewBank[i].Author));

      NewBank[i].Input_Gain = .5f;
      NewBank[i].Master_Volume = .5f;
      NewBank[i].Balance = 1.0f;
      NewBank[i].Bypass = 0;

      memset(NewBank[i].lv ,0, sizeof(NewBank[i].lv));

      for (j = 0; j < 20; j++)
	{
	  for (k = 0; k < 16; k++)
	    {
	      NewBank[i].lv[j][k] = presets[j][k];
	    }
	}
    
      memset(NewBank[i].XUserMIDI, 0, sizeof(NewBank[i].XUserMIDI));     
    
    }




};




int
loadbank (char *filename)
{

  int i,j,k,t;
  k=0;
  char buf[256];
  char nfilename[256];
  FILE *fn;
  if ((fn = fopen (filename, "rb")) != NULL)
    {
      while (!feof (fn))
	{
	  i = fread (&Bank, sizeof (Bank), 1, fn);
	}
      fclose (fn);
      if(BigEndian()) old_fix_endianess();
      convert_IO();
 
    sprintf(nfilename, "%s.ml",filename);
    if ((fn = fopen (nfilename, "r")) == NULL)
    return(1);

for(j=0;j<80;j++)
   {
    bzero(buf,sizeof(buf));
    fgets (buf, sizeof buf, fn);
    sscanf(buf,"%d\n",&k);
      if(k)
          { 
           for(i=0;i<128;i++)
             {
              bzero(buf,sizeof(buf));
              fgets (buf, sizeof buf, fn);
              sscanf(buf,"%d\n",&t);
              if(t)
                 { 
                  bzero(buf,sizeof(buf));
                  fgets (buf, sizeof buf, fn);
                  sscanf(buf,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           &PML[j].XUserMIDI[i][0], &PML[j].XUserMIDI[i][1], &PML[j].XUserMIDI[i][2], &PML[j].XUserMIDI[i][3], &PML[j].XUserMIDI[i][4],
           &PML[j].XUserMIDI[i][5], &PML[j].XUserMIDI[i][6], &PML[j].XUserMIDI[i][7], &PML[j].XUserMIDI[i][8], &PML[j].XUserMIDI[i][9],
           &PML[j].XUserMIDI[i][10], &PML[j].XUserMIDI[i][10], &PML[j].XUserMIDI[i][12], &PML[j].XUserMIDI[i][13], &PML[j].XUserMIDI[i][14],
           &PML[j].XUserMIDI[i][15], &PML[j].XUserMIDI[i][16], &PML[j].XUserMIDI[i][17], &PML[j].XUserMIDI[i][18], &PML[j].XUserMIDI[i][19]);
                  }
     
              }                
            }

    }  
       
  fclose(fn);

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
      copy_IO();
      if(BigEndian()) fix_endianess();
      fwrite (&NewBank, sizeof (NewBank), 1, fn);
      if(BigEndian()) fix_endianess();
      fclose (fn);
      return(1);     
  }
 return (0);
};



void
show_help ()
{
  fprintf (stderr, "Usage: rakaconvert -c Bankfile\n\n" );
  fprintf (stderr,
	   "  -h ,     --help \t\t\t display command-line help and exit\n");
  fprintf (stderr,
	   "  -c ,     --convert \t\t\t convert Bankfile\n");
	   
  fprintf (stderr, "\n");

}

int
main (int argc, char *argv[])
{

 int i,j,k;
 int option_index = 0, opt;
 int exitwithhelp = 0;
 char *OldBankFile = NULL;
 char NewFile1[256];
 char NewFile2[256];

// Read command Line

  fprintf (stderr,
   "\nrackconvert convert old data bank files to the new file format and store the new ones in the home user directory.\nrackconvert - Copyright (c) Josep Andreu - Ryan Billing - Douglas McClendon - Hernan Ordiales - Jose Luis Di Biase\n\n");

  if (argc == 1)
    fprintf (stderr, "Try 'rakconvert --help' for usage options.\n");


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
	      loadbank(optarguments);
	      break;
            }
         }

    }

  if (exitwithhelp != 0)
    {
      show_help ();
      return (0);
    };


printf("converting: %s\n\n",OldBankFile);

bzero(NewFile1,sizeof(NewFile1));
bzero(NewFile2,sizeof(NewFile2));

strncpy(NewFile1, OldBankFile, strlen(OldBankFile)-5);
strncpy(NewFile2, OldBankFile, strlen(OldBankFile)-5);
sprintf(NewFile1, "%s01_050.rkrb",NewFile1);
sprintf(NewFile2, "%s02_050.rkrb",NewFile2);

printf("generating %s\n",NewFile1);

New_Bank();

for (i=1; i<61; i++)
   {
     NewBank[i].Input_Gain = Bank[i].Input_Gain;
     NewBank[i].Master_Volume = Bank[i].Master_Volume;
     NewBank[i].Balance = 1.0f;
     NewBank[i].Bypass = Bank[i].Bypass;
     
     sprintf(NewBank[i].Preset_Name, "%s",Bank[i].Preset_Name);
     sprintf(NewBank[i].Author, "%s",Bank[i].Author);
     sprintf(NewBank[i].cInput_Gain, "%s",Bank[i].Reserva);
     sprintf(NewBank[i].cMaster_Volume, "%s",Bank[i].Reserva1);
     sprintf(NewBank[i].cBalance, "%s","1.000000");
      
  for(j=0;j<24;j++)
     {
       for(k=0;k<20;k++) 
         {
           NewBank[i].lv[j][k] = Bank[i].lv[j][k];
         } 
     }

   memcpy(NewBank[i].XUserMIDI, PML[i].XUserMIDI, sizeof(NewBank[i].XUserMIDI));


   NewBank[i].lv[0][19] = Bank[i].lv[0][12];
   NewBank[i].lv[1][19] = Bank[i].lv[1][7];
   NewBank[i].lv[2][19] = Bank[i].lv[2][12];
   NewBank[i].lv[3][19] = Bank[i].lv[3][12];
   NewBank[i].lv[4][19] = Bank[i].lv[4][12];
   NewBank[i].lv[5][19] = Bank[i].lv[5][11];
   NewBank[i].lv[6][19] = Bank[i].lv[6][11];
   NewBank[i].lv[7][19] = Bank[i].lv[7][12];
   NewBank[i].lv[8][19] = Bank[i].lv[8][10];
   NewBank[i].lv[9][19] = Bank[i].lv[9][8];
   NewBank[i].lv[11][19] = Bank[i].lv[11][11];
   NewBank[i].lv[12][19] = Bank[i].lv[12][11];
   NewBank[i].lv[13][19] = Bank[i].lv[13][2];
   NewBank[i].lv[14][19] = Bank[i].lv[14][8];
   NewBank[i].lv[15][19] = Bank[i].lv[15][11];
   NewBank[i].lv[16][19] = Bank[i].lv[16][13];
   NewBank[i].lv[17][19] = Bank[i].lv[17][7];
   NewBank[i].lv[18][19] = Bank[i].lv[18][12];
   NewBank[i].lv[19][19] = Bank[i].lv[19][12];
  


}

printf("saving %s\n\n",NewFile1);
savebank(NewFile1);

printf("generating %s\n",NewFile2);

New_Bank();

for (i=61; i<81; i++)
   {
     NewBank[i-60].Input_Gain = Bank[i].Input_Gain;
     NewBank[i-60].Master_Volume = Bank[i].Master_Volume;
     NewBank[i-60].Balance = 1.0f;
     NewBank[i-60].Bypass = Bank[i].Bypass;
     
     sprintf(NewBank[i-60].Preset_Name, "%s",Bank[i].Preset_Name);
     sprintf(NewBank[i-60].Author, "%s",Bank[i].Author);
     sprintf(NewBank[i-60].cInput_Gain, "%s",Bank[i].Reserva);
     sprintf(NewBank[i-60].cMaster_Volume, "%s",Bank[i].Reserva1);
     sprintf(NewBank[i-60].cBalance, "%s","1.000000");
      
  for(j=0;j<24;j++)
     {
       for(k=0;k<20;k++) 
         {
           NewBank[i-60].lv[j][k] = Bank[i].lv[j][k];
         } 
     }

   memcpy(NewBank[i-60].XUserMIDI, PML[i].XUserMIDI, sizeof(NewBank[i-60].XUserMIDI));


   NewBank[i-60].lv[0][19] = Bank[i].lv[0][12];
   NewBank[i-60].lv[1][19] = Bank[i].lv[1][7];
   NewBank[i-60].lv[2][19] = Bank[i].lv[2][12];
   NewBank[i-60].lv[3][19] = Bank[i].lv[3][12];
   NewBank[i-60].lv[4][19] = Bank[i].lv[4][12];
   NewBank[i-60].lv[5][19] = Bank[i].lv[5][11];
   NewBank[i-60].lv[6][19] = Bank[i].lv[6][11];
   NewBank[i-60].lv[7][19] = Bank[i].lv[7][12];
   NewBank[i-60].lv[8][19] = Bank[i].lv[8][10];
   NewBank[i-60].lv[9][19] = Bank[i].lv[9][8];
   NewBank[i-60].lv[11][19] = Bank[i].lv[11][11];
   NewBank[i-60].lv[12][19] = Bank[i].lv[12][11];
   NewBank[i-60].lv[13][19] = Bank[i].lv[13][2];
   NewBank[i-60].lv[14][19] = Bank[i].lv[14][8];
   NewBank[i-60].lv[15][19] = Bank[i].lv[15][11];
   NewBank[i-60].lv[16][19] = Bank[i].lv[16][13];
   NewBank[i-60].lv[17][19] = Bank[i].lv[17][7];
   NewBank[i-60].lv[18][19] = Bank[i].lv[18][12];
   NewBank[i-60].lv[19][19] = Bank[i].lv[19][12];
  


}

printf("saving %s\n",NewFile2);
savebank(NewFile2);

printf("Done.\n");

return(0);

};

