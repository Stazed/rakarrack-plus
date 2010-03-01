/*
 
  Convolotron.C - Convolotron effect
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
#include "Convolotron.h"

Convolotron::Convolotron (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
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
  howmany = 0;
  convlength = MAX_C_SIZE / 1000.0;
  maxx_size = (int) ((float) SAMPLE_RATE * convlength);  //just to get 1 second memory allocated
  buf = (float *) malloc (sizeof (float) * maxx_size);
  rbuf = (float *) malloc (sizeof (float) * maxx_size);
  lxn = (float *) malloc (sizeof (float) * maxx_size);  
  offset = 0;  
  M_Resample = new Resample(0);
  setpreset (Ppreset);
  cleanup ();
};

Convolotron::~Convolotron ()
{
  delete[]buf;
  delete[]rbuf;
  delete[]lxn;
};

/*
 * Cleanup the effect
 */
void
Convolotron::cleanup ()
{



};

/*
 * Effect output
 */
void
Convolotron::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i, j, xindex;
  float l,r,lyn;

  for (i = 0; i < PERIOD; i++)
    {

      l = smpsl[i];
      r = smpsr[i];
    
      lxn[offset] = l + r;
      
      //Convolve left channel
      lyn = 0;
      xindex = offset;
      for (j =0; j<howmany; j++)
      {
      xindex--;
      if (xindex<0) xindex = howmany;		//input signal scrolls backward with each iteration

      lyn += buf[j] * lxn[xindex];		//this is all there is to convolution
      }
      
      efxoutl[i] = lyn * 2.0f * level * panning;
      efxoutr[i] = lyn * 2.0f * level * (1.0f -panning);  

      offset++;
      if (offset>howmany) offset = 0;     

      
    };



};


/*
 * Parameter control
 */
void
Convolotron::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  volume = outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Convolotron::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};

int
Convolotron::setfile(int value)
{

int readcount;
double sr_ratio;
offset = 0;
memset(buf,0,sizeof(float) * maxx_size);
memset(rbuf,0,sizeof(float) * maxx_size);

Filenum = value;
bzero(Filename,sizeof(Filename));
sprintf(Filename, "%s/%d.wav",DATADIR,Filenum+1);
if(!(infile = sf_open(Filename, SFM_READ, &sfinfo))) return(0);
if (sfinfo.frames > maxx_size) howmany = maxx_size; else howmany=sfinfo.frames;
readcount = sf_seek (infile,0, SEEK_SET);
readcount = sf_readf_float(infile,buf,howmany);

if (sfinfo.samplerate != (int)SAMPLE_RATE)
{
  sr_ratio = (double)SAMPLE_RATE/((double) sfinfo.samplerate);
  M_Resample->mono_out(buf,rbuf,maxx_size,sr_ratio);
  howmany =lrintf((float)maxx_size*(float)sr_ratio);

}

process_rbuf();

return(1);
};


void
Convolotron::process_rbuf()
{
 int ii,N,N2;
 float tailfader, alpha, a0, a1, a2, Nm1p, Nm1pp;
 memset(buf,0, sizeof(float)*maxx_size);
 
 /*Blackman Window function
 wn = a0 - a1*cos(2*pi*n/(N-1)) + a2 * cos(4*PI*n/(N-1)
 a0 = (1 - alpha)/2; a1 = 0.5; a2 = alpha/2
 */
 alpha = 0.16f;
 a0 = 0.5f*(1.0f - alpha);
 a1 = 0.5f;
 a2 = 0.5*alpha;
 N = howmany;
 N2 = (int) (howmany/2);
 Nm1p = 2.0f * PI/((float) (N - 1));
 Nm1pp = 4.0f * PI/((float) (N - 1));
 
for(ii=0;ii<howmany;ii++)
{   
  if (ii<N2)
  {
  tailfader = 1.0f;
  }
  else
  { 
  tailfader = a0 - a1*cosf(ii*Nm1p) + a2 * cosf(ii*Nm1pp);   //Calculate Blackman Window for right half of IR
  }
  
  buf[ii]= rbuf[ii] * tailfader;   //Apply window function
 }
}

void
Convolotron::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.0f;
};

void
Convolotron::setpreset (int npreset)
{
  const int PRESET_SIZE = 10;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Convolotron 1
    {67, 64, 1, 100, 0, 64, 30, 20, 1, 0},
    //Convolotron 2
    {67, 64, 1, 100, 1, 64, 30, 20, 1, 0},
    //Convolotron 3
    {67, 75, 1, 100, 3, 64, 30, 20, 1, 0},
    //Convolotron 4
    {67, 60, 1, 100, 3, 64, 30, 20, 1, 0}
  };

  howmany = 0;
  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Convolotron::changepar (int npar, int value)
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
      convlength = ((float) Plength)/1000.0f;			//time in seconds
      maxx_size = (int) ((float) SAMPLE_RATE * convlength);	//time in samples
      setfile(Filenum);
      break;
    case 4:
      if(!setfile(value))
      {
       printf("shit\n"); // if dont find the file .... do something :-) <--Yes, spew forth profanity when something is wrong :-)
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
    case 8:
      Pfb = value;
      break;
    case 9:
      Preverb = value;      
      break;

   };
};

int
Convolotron::getpar (int npar)
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
    case 4:
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
    case 8:
      return(Pfb);
      break;
    case 9:
      return(Preverb);
      break;    

    };
  return (0);			//in case of bogus parameter number
};
