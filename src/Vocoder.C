/*
 
  Vocoder.C - Vocoder effect
  Author: Ryam Billing & Josep Andreu
  
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
#include "Vocoder.h"

Vocoder::Vocoder (float * efxoutl_, float * efxoutr_, float *auxresampled_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;
  auxresampled = auxresampled_;
  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ppanning = 64;
  Plrcross = 100;
  Psafe = 1;
  Phidamp = 60;
  Filenum = 0;
  Plength = 10;
  real_len = 0;
  convlength = .5f;
  fb = 0.0f;
  feedback = 0.0f;
  maxx_size = (int) (fSAMPLE_RATE * convlength);  //just to get the max memory allocated
  window = (float *) malloc (sizeof (float) * PERIOD);
  vocbuf = (float *) malloc (sizeof (float) * PERIOD);
  lxn = (float *) malloc (sizeof (float) * PERIOD);  
  offset = voffset = 0;  
  setpreset (Ppreset);
  cperiod = 1.0f/((float) PERIOD);
 
  float center;
  float bw;
  float qq;
  for (int i = 0; i < VOC_BANDS; i++)
    {
      bw = powf(10.0f, (2.5f + ((float) i + 1)/13.0f)) - powf(10.0f, (2.5f + ((float) i)/13.0f));
      center = powf(10.0f, (2.5f + ((float) i)/13.0f)) + bw/2.0f;
      qq = center/bw;
      filterbank[i].sfreq = center;
      filterbank[i].sq = qq;
      filterbank[i].l = new AnalogFilter (4, center, qq, 0);
      filterbank[i].r = new AnalogFilter (4, center, qq, 0);
      filterbank[i].aux = new AnalogFilter (4, center, qq, 0);
    };
  
  cleanup ();
};

Vocoder::~Vocoder ()
{
  delete[]window;
  delete[]vocbuf;
  delete[]lxn;
};

/*
 * Cleanup the effect
 */
void
Vocoder::cleanup ()
{


};

/*
 * Effect output
 */
void
Vocoder::out (float * smpsl, float * smpsr)
{
  int i, j, xindex, vindex;
  float l,lyn;

    for (j = 0; j < VOC_BANDS; j++)
    {
      memcpy (vocbuf , auxresampled, PERIOD * sizeof(float));
      filterbank[j].aux->filterout(vocbuf);
       for (i = 0; i<PERIOD; i++)
       { 
       filterbank[j].sgain += cperiod * fabs(vocbuf[i]);   
       };
       
      memcpy (tmpsmpsl , smpsl, PERIOD * sizeof(float));  
      memcpy (tmpsmpsr , smpsr, PERIOD * sizeof(float));  
      filterbank[j].l->filterout(tmpsmpsl);
      filterbank[j].r->filterout(tmpsmpsr);
      for (i = 0; i<PERIOD; i++)
       { 
       smpsl[i] += tmpsmpsl*filterbank[j].gain;  //I need to add gain interpolation here between periods.
       smpsr[i] += tmpsmpsr*filterbank[j].gain;        
       };  
      
       filterbank[j].oldgain = filterbank[j].gain;   
    }; 
    
};


/*
 * Parameter control
 */
void
Vocoder::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  volume = outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Vocoder::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  lpanning = ((float)Ppanning + 0.5f) / 127.0f;
  rpanning = 1.0f - lpanning;
};


void
Vocoder::init_filters()
{
 int ii;
 float ff, qq;
 
  for (int ii = 0; ii < VOC_BANDS; ii++)
    {
      ff = filterbank[ii].sfreq;
      qq = filterbank[ii].sq;
      filterbank[ii].l->setfreq_and_q (ff, qq);
      filterbank[ii].r->setfreq_and_q (ff, qq);
      filterbank[ii].aux->setfreq_and_q (ff, qq);
    };

}

void
Vocoder::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.1f;
  alpha_hidamp = 1.0f - hidamp;
};

void
Vocoder::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Vocoder 1
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0},
    //Vocoder 2
    {67, 64, 1, 100, 0, 64, 30, 20, 1, 0, 0},
    //Vocoder 3
    {67, 75, 1, 100, 0, 64, 30, 20, 2, 0, 0},
    //Vocoder 4
    {67, 60, 1, 100, 0, 64, 30, 20, 3, 0, 0}
  };

  
  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Vocoder::changepar (int npar, int value)
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
      if (value < 50)
      Plength = value;
      else
      Plength = 50;  //temporarily force max just to test things

      convlength = ((float) Plength)/1000.0f;                   //time in seconds
      length = (int) (fSAMPLE_RATE * convlength);        //time in samples  
      process_window();     
      break;
    case 8:
      break;
    case 5:
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      Plevel = value;
      level =  dB2rap (72.0f * (float)Plevel / 127.0f - 60.0f);;
      break;
    case 4:
      Puser = value;
      break;
    case 9:
      Preverb = value;      
      break;
    case 10:
      Pfb = value;
      smear = ((float) value)/64.2f;   
      break;

   };
};

int
Vocoder::getpar (int npar)
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
