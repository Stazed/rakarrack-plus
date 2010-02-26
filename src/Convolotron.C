/*
  ZynAddSubFX - a software synthesizer
 
  Convolotron.C - Convolotron effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Echo effect by Transmogrifox
  
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
  Phidamp = 60;
  Filenum = 0;
  maxx_size = SAMPLE_RATE * MAX_C_SIZE;
  buf = (float *) malloc (sizeof (float) * maxx_size);

    
  
  setpreset (Ppreset);
  cleanup ();
};

Convolotron::~Convolotron ()
{
  delete[]buf;
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
  int i;
  float l,r,lout,rout;



  REALTYPE level = dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);



  for (i = 0; i < PERIOD; i++)
    {

      lout = smpsl[i];
      rout = smpsr[i];
   
      l = lout * (1.0f - lrcross) + rout * lrcross;
      r = rout * (1.0f - lrcross) + lout * lrcross;
  

      lout=buf[offset] + l;
      rout=buf[offset] + r;
      
      efxoutl[i] = lout * 2.0f * level * panning;
      efxoutr[i] = rout * 2.0f * level * (1.0f -panning);  

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

memset(buf,0,sizeof(float) * maxx_size);
Filenum = value;
bzero(Filename,sizeof(Filename));
sprintf(Filename, "%s/%d.wav",DATADIR,Filenum+1);
if(!(infile = sf_open(Filename, SFM_READ, &sfinfo))) return(0);
if (sfinfo.frames > maxx_size) howmany = maxx_size; else howmany=sfinfo.frames;
readcount = sf_seek (infile,0, SEEK_SET);
readcount = sf_readf_float(infile,buf,howmany);
return(1);
};

void
Convolotron::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 127.0f * 1.0f;
};

void
Convolotron::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.0f;
};

void
Convolotron::setpreset (int npreset)
{
  const int PRESET_SIZE = 6;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Convolotron 1
    {67, 64, 0, 64, 30, 20},
    //Convolotron 2
    {67, 64, 1, 64, 30, 20},
    //Convolotron 3
    {67, 75, 0, 64, 30, 20},
    //Convolotron 4
    {67, 60, 1, 64, 30, 20}
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
      if(!setfile(value))
      {
       printf("shit\n"); // if dont find the file .... do something :-)
      
      
      }
      break;
    case 3:
      setlrcross (value);
      break;
    case 4:
      sethidamp (value);
      break;
    case 5:
      Plevel = value;
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
      return (Filenum);
      break;
    case 3:
      return (Plrcross);
      break;
    case 4:
      return (Phidamp);
      break;
    case 5:
      return(Plevel);

    };
  return (0);			//in case of bogus parameter number
};
