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
  convlength = 10.0f;  //max reverb time
  fb = 0.0f;
  feedback = 0.0f;
  maxtime = 0.0f;
  maxx_size = (int) (fSAMPLE_RATE * convlength);  //just to get the max memory allocated
  time = (int *) malloc (sizeof (int) * 2000);
  rndtime = (int *) malloc (sizeof (int) * 2000);
  ftime = (float *) malloc (sizeof (float) * 2000);
  data = (float *) malloc (sizeof (float) * 2000);
  rnddata = (float *) malloc (sizeof (float) * 2000);
  tdata = (float *) malloc (sizeof (float) * 2000);
  lxn = (float *) malloc (sizeof (float) * (1 + maxx_size));  
  imax = SAMPLE_RATE/2;  // 1/2 second available
  imdelay = (float *) malloc (sizeof (float) * imax);
  offset = 0;
  data_length=0;
  fstretch = 1.0f;
  idelay = 0.0f;
  decay = expf(-1.0f/(0.2f*fSAMPLE_RATE));  //0.2 seconds

  lpfl =  new AnalogFilter (0, 800, 1, 0);;
  lpfr =  new AnalogFilter (0, 800, 1, 0);;

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
lpfl->cleanup ();
lpfr->cleanup ();

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
  float ldiff,rdiff;
  int length = Plength;
  int doffset;
  float randlevel=0.05f;
  
  if(length<400)randlevel = randlevel+.06f*(400.0f -(float)length)/380.0f;
  
  for(i=0;i<length;i++) 
  {
  if(fabs(rnddata[i])<randlevel)
 {  

  rnddata[i]=data[i]+data[i]*((-.2f+(float)(RND*.4f))*diffusion);
  if((data[i]>0.0) && (rnddata[i]<0.0)) rnddata[i]=data[i];
  if((data[i]<0.0) && (rnddata[i]>0.0)) rnddata[i]=data[i];
  if (rnddata[i]>1.0f) rnddata[i]=data[i];
  if (rnddata[i]<0.0f) rnddata[i]=data[i];
  
  
  rndtime[i]=time[i]+lrintf((float)time[i]*((-.0005f+(float)(RND*.001f))*diffusion));
  if(rndtime[i]<0)rndtime[i]=time[i];
  if(rndtime[i]>=maxx_size)rndtime[i]=maxx_size-1;
 }
  else rndtime[i]=time[i]; 

  }
  
  for (i = 0; i < PERIOD; i++)
    {

      l = 0.5f*(smpsr[i] + smpsl[i]); 
      oldl = l * hidamp + oldl * (alpha_hidamp);  //apply damping while I'm in the loop      
      if(Prv)
      {
       oldl = 0.5f*oldl - smpsl[i];     
      }

      lxn[offset] = oldl;
      
      //Convolve 
      lyn = 0.0f;
      xindex = offset;

      for (j =0; j<length; j++)
      {
      xindex = offset + rndtime[j];
      if(xindex>maxx_size) xindex -= maxx_size;
      lyn += lxn[xindex] * rnddata[j];		//this is all of the magic
      }

    
       if(Pes) // just so I have the code to get started
       {

 	  ldiff = lyn;
 	  rdiff = imdelay[imctr];
	  
          ldiff = lpfl->filterout_s(ldiff);
          rdiff = lpfr->filterout_s(rdiff);
	  
      imdelay[imctr] = decay*ldiff;	
      imctr--;
      if (imctr<0) imctr = roomsize;
      
      efxoutl[i] = (lyn + ldiff )* levpanl;
      efxoutr[i] = (lyn + rdiff ) * levpanr;  
       
      feedback = fb*rdiff*decay;          

       }
       else
       {
      feedback = fb * lyn;
      efxoutl[i] = lyn * levpanl;
      efxoutr[i] = lyn * levpanr;         
       
       }        

      offset--;
      if (offset<0) offset = maxx_size - 1;   
      
      doffset = (offset + roomsize);  
      if (doffset>maxx_size) doffset -= maxx_size;

      lxn[doffset] += feedback; 
      
      xindex = offset + roomsize;      
      
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
Reverbtron::setpanning (int value)
{
  Ppanning = value;
  rpanning = ((float)Ppanning) / 64.0f;
  lpanning = 2.0f - rpanning;
  lpanning = 10.0f * powf(lpanning, 4);
  rpanning = 10.0f * powf(rpanning, 4);
  lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
  rpanning = 1.0f - 1.0f/(rpanning + 1.0f); 
  lpanning *= 1.1f;
  rpanning *= 1.1f; 
  levpanl=level*lpanning;
  levpanr=level*rpanning;
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

cleanup();
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

maxtime = 0.0f;
maxdata = 0.0f;
float averaget = 0.0f;
float tempor = 0.0f;
for(i=0;i<data_length;i++)
{
  if(ftime[i] > maxtime) maxtime = ftime[i];
  if(tdata[i] > maxdata) maxdata = tdata[i];  //used to normalize so feedback is more predictable
  if(i>0){
   tempor = ftime[i] - ftime[i-1];
   if(tempor>averaget) averaget = tempor;
   }
}

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
float tmpstretch = 1.0f;
float normal = 0.9999f/maxdata;

memset(data, 0, sizeof(float)*2000);
memset(time, 0, sizeof(int)*2000);

if(Plength>=data_length) Plength = data_length;
if(Plength==0) Plength=400;
incr = ((float) Plength)/((float) data_length);


if(fstretch>0.0) 
{
tmpstretch = 1.0f + fstretch * (convlength/maxtime);
}
else
{
tmpstretch = 1.0f + 0.95f*fstretch;
}


skip = 0.0f;
index = 0;
chunk = 10;

if(data_length>Plength)
{
for(i=0;i<data_length;i++)
{ 
  skip += incr;
  findex = (float)index;
  if( findex<skip)
  {
    if(index<Plength)   
      { 
       if( (tmpstretch*(idelay + ftime[i] )) > 9.9f ) 
       {
       ftime[i] = 0.0f;
       data[i] = 0.0f;
       }   
       time[index]=lrintf(tmpstretch*(idelay + ftime[i])*fSAMPLE_RATE);  //Add initial delay to all the samples
       data[index]=normal*tdata[i];  
       index++;
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
    time[i]=lrintf(tmpstretch*(idelay + ftime[i])*fSAMPLE_RATE);  //Add initial delay to all the samples
    data[i]=normal*tdata[i];  
 
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
  
//guess at room size
roomsize = time[0] + (time[1] - time[0])/2;  //to help stagger left/right reflection times
if(roomsize>imax) roomsize = imax;

};


void
Reverbtron::setlpf (int value)
{
  Plpf = value;
  float fr = (float)Plpf;
  lpfl->setfreq (fr);
  lpfr->setfreq (fr);
    
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
  const int PRESET_SIZE = 16;
  const int NUM_PRESETS = 9;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Spring
    {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64, 0, 0, 20000, 0},
    //Concrete Stair
    {64, 0, 1, 500, 0, 0, 0, 40, 1, 0, 0, 64, 0, 0, 20000, 0},
    //Nice Hall
    {64, 0, 1, 500, 0, 0, 60, 15, 2, 0, 0, 64, 0, 0, 20000, 0},
    //Hall
    {64, 16, 1, 500, 0, 0, 0, 22, 3, -17, 0, 64, 0, 0, 20000, 0},
    //Room
    {64, 0, 1, 1500, 0, 0, 48, 20, 4, 0, 0, 64, 0, 0, 20000, 0},
    //Hall
    {88, 0, 1, 1500, 0, 0, 88, 14, 5, 0, 0, 64, 0, 0, 20000, 0},
    //Guitar
    {64, 0, 1, 1500, 0, 0, 30, 34, 6, 0, 0, 64, 0, 0, 20000, 0},
    //Studio
    {64, 0, 1, 1500, 0, 0, 30, 20, 7, 0, 0, 64, 0, 0, 20000, 0},
    //Cathedral
    {64, 0, 1, 1500, 0, 30, 0, 40, 9, 0, 0, 64, 0, 0, 20000, 0}

  };

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
      levpanl=level*lpanning;
      levpanr=level*rpanning;
      break;
    case 8:
      if(!setfile(value)) error_num=2;
      break;
    case 9:
      Pstretch = value;
      fstretch = ((float) value)/64.0f;   
      convert_time();
      break;
    case 10:
      Pfb = value;
      if(Pfb<=0)
      {
      fb = (float) value/64.0f * 0.3;  
      }
      else
      {
      fb = (float) value/64.0f * 0.15; 
      }  
      break;
    case 11:
      setpanning (value);
      break;
    case 12:
      Pes = value;
      break;
    case 13:
      Prv = value;
      break;     
    case 14:
      setlpf (value);
      break;
    case 15:
      Pdiff=value;
      diffusion = ((float) value)/127.0f;
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
      break;
    case 12:
      return(Pes);
      break;
    case 13:
      return(Prv);
      break;  
    case 14:
      return(Plpf);
      break;  
    case 15:
      return(Pdiff);
      break;  


    };
  return (0);			//in case of bogus parameter number
};
