/*
 
  Reverbtron.C - Reverbtron effect
  Author: Ryam Billing & Jospe Andreu
  
  Adapted effect structure of ZynAddSubFX - a software synthesizer
  Author: Nasca Octavian Paul
  
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Reverbtron.h"

Reverbtron::Reverbtron (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ppanning = 64;
  Plrcross = 100;
  Psafe = 1;
  Phidamp = 60;
  Filenum = 0;
  Plength = 50;
  Puser = 0;
  real_len = 0;
  convlength = 6.0f;  //max 6s verb time
  fb = 0.0f;
  feedback = 0.0f;
  maxx_size = (int) (fSAMPLE_RATE * convlength);  //just to get the max memory allocated
  time = (float *) malloc (sizeof (float) * 2000);
  data = (float *) malloc (sizeof (float) * 2000);
  lxn = (float *) malloc (sizeof (float) * maxx_size);  
  offset = 0;  
  M_Resample = new Resample(0);
  setpreset (Ppreset);
  cleanup ();
};

Reverbtron::~Reverbtron ()
{
};

/*
 * Cleanup the effect
 */
void
Reverbtron::cleanup ()
{


};

/*
 * Effect output
 */
void
Reverbtron::out (float * smpsl, float * smpsr)
{
  int i, j, xindex, verbindex;
  int interval = length/2;
  int numtaps = 5;
  float l,lyn;

  for (i = 0; i < PERIOD; i++)
    {

      l = smpsl[i] + smpsr[i] +  feedback;
      oldl = l * hidamp + oldl * (alpha_hidamp);  //apply damping while I'm in the loop
      lxn[offset] = oldl;

      
      //Convolve left channel
      lyn = 0;
      xindex = offset;

      for (j =0; j<Plength; j++)
      {
      if((xindex = offset + time[j])>=maxx_size) xindex -= maxx_size;
      lyn += data[j] * lxn[xindex];		//this is all there is to convolution
         
      }

      feedback = 0.15f * fb * lyn;
      efxoutl[i] = lyn * 2.0f * level * lpanning;
      efxoutr[i] = lyn * 2.0f * level * rpanning;  

      offset--;
      if (offset<0) offset = maxx_size;     

      
    };



};


/*
 * Parameter control
 */
void
Reverbtron::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Reverbtron::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  lpanning = ((float)Ppanning + 0.5f) / 127.0f;
  rpanning = 1.0f - lpanning;
};

int
Reverbtron::setfile(int value)
{
int i;
int data_length=0;
int subsample = 0;
float compresion = 0.0;
float skip = 0.0;
char wbuf[256];
FILE *fs;

if(!Puser)
{
Filenum = value;
bzero(Filename,sizeof(Filename));
sprintf(Filename, "%s/%d.rvb",DATADIR,Filenum+1);
}

if ((fs = fopen (Filename, "r")) == NULL) return(0);

//Name
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);

// Subsample Compresion Skip 
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);
sscanf(wbuf,"%d,%f,%f\n",&subsample,&compresion,&skip);

//Length
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);
sscanf(wbuf, "%d\n", &data_length);


//Time Data
for(i=0;i<data_length;i++)
{
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);
sscanf(wbuf,"%f,%f\n",&time[i],&data[i]);
}

fclose(fs);
return(1);
};



void
Reverbtron::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.1f;
  alpha_hidamp = 1.0f - hidamp;
};

void
Reverbtron::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Reverbtron 1
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0},
    //Reverbtron 2
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0},
    //Reverbtron 3
    {67, 75, 1, 100, 0, 64, 30, 20, 0, 0, 0},
    //Reverbtron 4
    {67, 60, 1, 100, 0, 64, 30, 20, 0, 0, 0}
  };

  
  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Reverbtron::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
      Psafe = value;
      break;
    case 3:
      if (Psafe)
      {
      if (value < maxx_len)
      Plength = value;
      else
      Plength = maxx_len;
      }
      else Plength = value;
      length = (int) (fSAMPLE_RATE * convlength);        //time in samples       
      break;
    case 8:
      if(!setfile(value))
      {
       printf("Unable to open the IR file\n");
      }
      break;
    case 5:
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      Plevel = value;
      level =  dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);;
      break;
    case 4:
      Puser = value;
      break;
    case 9:
      Preverb = value;      
      break;
    case 10:
      Pfb = value;
      if(Pfb<0)
      {
      fb = (float) .1f*value/250.0f;  
      }
      else
      {
      fb = (float) .1f*value/500.0f; 
      }    
      break;

   };
};

int
Reverbtron::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppanning);
      break;
    case 2:
      return(Psafe);
      break;
    case 3:
      return(Plength); 
      break;  
    case 8:
      return (Filenum);
      break;
    case 5:
      return (0);
      break;
    case 6:
      return (Phidamp);
      break;
    case 7:
      return(Plevel);
      break;
    case 4:
      return(Puser);
      break;
    case 9:
      return(Preverb);
      break;   
    case 10:
      return(Pfb);
      break; 

    };
  return (0);			//in case of bogus parameter number
};
