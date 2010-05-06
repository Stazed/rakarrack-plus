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
  Phidamp = 60;
  Filenum = 0;
  Plength = 50;
  Puser = 0;
  Psafe = 0;
  convlength = 6.0f;  //max 6s verb time
  fb = 0.0f;
  feedback = 0.0f;
  maxx_size = (int) (fSAMPLE_RATE * convlength);  //just to get the max memory allocated
  time = (int *) malloc (sizeof (int) * 2000);
  ftime = (float *) malloc (sizeof (float) * 2000);
  data = (float *) malloc (sizeof (float) * 2000);
  tdata = (float *) malloc (sizeof (float) * 2000);
  lxn = (float *) malloc (sizeof (float) * maxx_size);  
  offset = 0;
  data_length=0;
  fstretch = 1.0f;
  idelay = 0.0f;
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
memset(lxn,0,sizeof(float)*maxx_size);
feedback = 0.0f;
oldl = 0.0f;

};

/*
 * Effect output
 */
void
Reverbtron::out (float * smpsl, float * smpsr)
{
  int i, j, xindex;
//  int fbindex;
//  int interval = length/2;
//  int fbctr = 5;
  float l,lyn;
  int length = Plength;

  for (i = 0; i < PERIOD; i++)
    {

      l = smpsl[i] + smpsr[i] +  feedback;
      oldl = l * hidamp + oldl * (alpha_hidamp);  //apply damping while I'm in the loop
      lxn[offset] = oldl;
      
      //Convolve 
      lyn = 0.0f;
      xindex = offset;

      for (j =0; j<length; j++)
      {
      xindex = offset + time[j];
      if(xindex>=maxx_size) xindex -= maxx_size;
      lyn += data[j] * lxn[xindex];		//this is all of the magic
         
      }

      feedback = fb * lyn;
      efxoutl[i] = lyn * level * lpanning;
      efxoutr[i] = lyn * level * rpanning;  

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
float compresion = 0.0f;
float quality = 0.0f;
char wbuf[128];

FILE *fs;

if(!Puser)
{
Filenum = value;
bzero(Filename,sizeof(Filename));
sprintf(Filename, "%s/%d.rvb",DATADIR,Filenum+1);
}

if ((fs = fopen (Filename, "r")) == NULL) return(0);

memset(tdata, 0, sizeof(float)*2000);
memset(ftime, 0, sizeof(float)*2000);


//Name
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);

// Subsample Compresion Skip 
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);
sscanf(wbuf,"%f,%f\n",&compresion,&quality);

//Length
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);
sscanf(wbuf, "%d\n", &data_length);
if(data_length>2000) data_length = 2000;
//Time Data
for(i=0;i<data_length;i++)
{
bzero(wbuf,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs);
sscanf(wbuf,"%f,%f\n",&ftime[i],&tdata[i]);
}

fclose(fs);
cleanup();
convert_time();
return(1);
};

void Reverbtron::convert_time()
{
int i;
int index = 0;
int count;
float tmp;
int chunk;
float skip = 0.0f;
float incr = 0.0f;
float findex;

memset(data, 0, sizeof(float)*2000);
memset(time, 0, sizeof(int)*2000);

if(Plength>=data_length) Plength = data_length;
if(Plength==0) Plength=400;
incr = ((float) Plength)/((float) data_length);

skip = 0.0f;
index = 0;
chunk = 10;

if(data_length>100)
{
for(i=0;i<data_length;i++)
{ 
  skip += incr;
  findex = (float)index;
  if( findex<skip)
  {
    if(index<Plength)   
      { 
     if ((i==0) || ((data[index-1]>0) && (tdata[i]<0)) || ((data[index-1]<0) && (tdata[i]>0)))    
      {
       if( (idelay + ftime[i] ) > 5.9f ) ftime[i] = 5.9f;   
       time[index]=lrintf(fstretch*(idelay + ftime[i])*fSAMPLE_RATE);  //Add initial delay to all the samples
       data[index]=tdata[i];  
       index++;
      }
      }
  }
}; 
    Plength = index;
} //endif
else
{
for(i=0;i<data_length;i++)
{ 

    if( (idelay + ftime[i] ) > 5.9f ) ftime[i] = 5.9f;   
    time[i]=lrintf(fstretch*(idelay + ftime[i])*fSAMPLE_RATE);  //Add initial delay to all the samples
    data[i]=tdata[i];  
 
};
  Plength = i;
}


 if(Pfade > 0)
 {
 count = lrintf(ffade * ((float) index));
 tmp = 0.0f;
 for (i=0; i<count;i++) //head fader 
 {

  tmp = ((float) i)/((float) count);
  data[i] *= tmp;
  //fade the head here
  }
  }

 

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
  const int PRESET_SIZE = 12;
  const int NUM_PRESETS = 8;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Spring
    {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64},
    //Concrete Stair
    {64, 0, 1, 500, 0, 0, 0, 40, 1, 0, 0, 64},
    //Nice Hall
    {64, 0, 1, 500, 0, 0, 60, 15, 2, 0, 0, 64},
    //Hall
    {64, 16, 1, 500, 0, 0, 0, 22, 3, -17, 0, 64},
    //Room
    {64, 0, 1, 1500, 0, 0, 48, 20, 4, 0, 0, 64},
    //Hall
    {88, 0, 1, 1500, 0, 0, 88, 14, 5, 0, 0, 64},
    //Guitar
    {64, 0, 1, 1500, 0, 0, 30, 34, 6, 0, 0, 64},
    //Studio
    {64, 0, 1, 1500, 0, 0, 30, 20, 7, 0, 0, 64}

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
      Pfade=value;
      ffade = ((float) value)/127.0f;
      convert_time();
      break;
    case 2:
      Psafe=value;
      break;
    case 3:
     Plength = value;
     if((Psafe) && (Plength>400)) Plength = 400;
     convert_time(); 
     break;
    case 4:
      Puser = value;
      break;
    case 5:
      Pidelay = value;
      idelay = ((float) value)/1000.0f;
      convert_time();
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      Plevel = value;
      level =  2.0f * dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);
      break;
    case 8:
      if(!setfile(value))
      {
       printf("Unable to open the IR file\n");
      }
      break;
    case 9:
      Pstretch = value;
      if(Pstretch > 0) {
      fstretch = 1.0f + ((float) value)/20.0f;
      }
      else {
      fstretch = 1.0f + ((float) value)/69.0f;
      }      
      convert_time();
      break;
    case 10:
      Pfb = value;
      if(Pfb<0)
      {
      fb = (float) value/250.0f*.15f;  
      }
      else
      {
      fb = (float) value/128.0f*.15f; 
      }    
      break;
    case 11:
      setpanning (value);
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
      return (Pfade);
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
      return (Pidelay);
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
      return(Pstretch);
      break;   
    case 10:
      return(Pfb);
      break; 
    case 11:
      return(Ppanning);
    };
  return (0);			//in case of bogus parameter number
};
