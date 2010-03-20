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

Convolotron::Convolotron (float * efxoutl_, float * efxoutr_)
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
  real_len = 0;
  convlength = .5f;
  fb = 0.0f;
  feedback = 0.0f;
  maxx_size = (int) (fSAMPLE_RATE * convlength);  //just to get the max memory allocated
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
Convolotron::out (float * smpsl, float * smpsr)
{
  int i, j, xindex, verbindex;
  float l,lyn;
  int interval = length/2;
  int numtaps = 5;

  for (i = 0; i < PERIOD; i++)
    {

      l = smpsl[i] + smpsr[i] + (float)Preverb*fb*lxn[offset] + feedback;
      oldl = l * hidamp + oldl * (alpha_hidamp);  //apply damping while I'm in the loop
      lxn[offset] = oldl;

      if(Preverb)
      {
      //Multitap feedback for reverbs
     verbindex = offset - length;
      for (j = 0; j<numtaps; j++)
      {
      verbindex-=interval;
      if(verbindex<0) verbindex += maxx_size;
      lxn[offset] += fb * lxn[verbindex];
      }
      }
      
      //Convolve left channel
      lyn = 0;
      xindex = offset;

      for (j =0; j<length; j++)
      {
      xindex--;		//input signal scrolls backward with each iteration
      if (xindex<0) xindex = maxx_size;		//length of lxn is maxx_size.  

      lyn += buf[j] * lxn[xindex];		//this is all there is to convolution

      }


      feedback = 0.15f * fb * lyn;
      efxoutl[i] = lyn * 2.0f * level * lpanning;
      efxoutr[i] = lyn * 2.0f * level * rpanning;  

      offset++;
      if (offset>maxx_size) offset = 0;     

      
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
  lpanning = ((float)Ppanning + 0.5f) / 127.0f;
  rpanning = 1.0f - lpanning;
};

int
Convolotron::setfile(int value)
{

int readcount;
double sr_ratio;
offset = 0;
maxx_read = maxx_size / 2;
memset(buf,0,sizeof(float) * maxx_size);
memset(rbuf,0,sizeof(float) * maxx_size);
if(!Puser)
{
Filenum = value;
bzero(Filename,sizeof(Filename));
sprintf(Filename, "%s/%d.wav",DATADIR,Filenum+1);
}
sfinfo.format = 0;
if(!(infile = sf_open(Filename, SFM_READ, &sfinfo))) return(0);
if (sfinfo.frames > maxx_read) real_len = maxx_read; else real_len=sfinfo.frames;
readcount = sf_seek (infile,0, SEEK_SET);
readcount = sf_readf_float(infile,buf,real_len);
sf_close(infile);




if (sfinfo.samplerate != (int)SAMPLE_RATE)
{
  sr_ratio = (double)SAMPLE_RATE/((double) sfinfo.samplerate);
  M_Resample->mono_out(buf,rbuf,real_len,sr_ratio);
  real_len =lrintf((float)real_len*(float)sr_ratio);
}

else memcpy(rbuf,buf,real_len*sizeof(float));

process_rbuf();

return(1);
};


void
Convolotron::process_rbuf()
{
 int ii,j,N,N2;
 float tailfader, alpha, a0, a1, a2, Nm1p, Nm1pp, IRpowa, IRpowb, ngain, maxamp;
 memset(buf,0, sizeof(float)*real_len);
 
 
 /*Blackman Window function
 wn = a0 - a1*cos(2*pi*n/(N-1)) + a2 * cos(4*PI*n/(N-1)
 a0 = (1 - alpha)/2; a1 = 0.5; a2 = alpha/2
 */
 alpha = 0.16f;
 a0 = 0.5f*(1.0f - alpha);
 a1 = 0.5f;
 a2 = 0.5*alpha;
 N = length;
 N2 = length/2;
 Nm1p = D_PI/((float) (N - 1));
 Nm1pp = 4.0f * PI/((float) (N - 1));
 
	for(ii=0;ii<length;ii++)
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


 	memcpy(buf,rbuf,real_len*sizeof(float));

	IRpowa = IRpowb = maxamp = 0.0f;
	//compute IR signal power
	 for(j=0;j<maxx_read;j++)
	 {
	 IRpowa += fabsf(rbuf[j]);
            if(maxamp < fabsf(buf[j])) maxamp = fabsf(buf[j]);   //find maximum level to normalize	
	     
		 if(j < length) 
		 {
                  IRpowb += fabsf(buf[j]);
		 }
 
	 }
	 
	 //if(maxamp < 0.3f) maxamp = 0.3f;
	 ngain = IRpowa/IRpowb;
	 if (ngain > maxx_read) ngain = maxx_read;
	 for(j=0; j<length; j++) buf[j] *= ngain; 


}

void
Convolotron::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.1f;
  alpha_hidamp = 1.0f - hidamp;
};

void
Convolotron::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Convolotron 1
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0},
    //Convolotron 2
    {67, 64, 1, 100, 0, 64, 30, 20, 1, 0, 0},
    //Convolotron 3
    {67, 75, 1, 100, 0, 64, 30, 20, 2, 0, 0},
    //Convolotron 4
    {67, 60, 1, 100, 0, 64, 30, 20, 3, 0, 0}
  };

  
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
      convlength = ((float) Plength)/1000.0f;                   //time in seconds
      length = (int) (fSAMPLE_RATE * convlength);        //time in samples       
      process_rbuf();
      break;
    case 8:
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
