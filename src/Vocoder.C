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
  tmpsmpsl = (float *) malloc (sizeof (float) * PERIOD);
  tmpsmpsr = (float *) malloc (sizeof (float) * PERIOD);
  tmpl = (float *) malloc (sizeof (float) * PERIOD);
  tmpr = (float *) malloc (sizeof (float) * PERIOD);

  vocbuf = (float *) malloc (sizeof (float) * PERIOD);
  
  cperiod = 1.0f/fPERIOD; 
  float center;
  float bw;
  float qq;
  float subdiv = 28.0f;  //sets range & spacing of bands
  for (int i = 0; i < VOC_BANDS; i++)
    {
      bw = powf(10.0f, (2.5f + ((float) i + 1)/subdiv)) - powf(10.0f, (2.5f + ((float) i)/subdiv));
      center = powf(10.0f, (2.5f + ((float) i)/subdiv)) + bw/2.0f;
      qq = center/bw;
      filterbank[i].sfreq = center;
      filterbank[i].sq = qq;
      filterbank[i].l = new AnalogFilter (4, center, qq, 0);
      filterbank[i].r = new AnalogFilter (4, center, qq, 0);
      filterbank[i].aux = new AnalogFilter (4, center, qq, 0);
//      printf("%f %f\n",center,qq);

    };

  setpreset (Ppreset);
  

  cleanup ();
};

Vocoder::~Vocoder ()
{
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
  int i, j;

    memset(tmpl,0,sizeof(float)*PERIOD);
    memset(tmpr,0,sizeof(float)*PERIOD);
  

    for (j = 0; j < VOC_BANDS; j++)
    {
      filterbank[j].gain = 0;

      for(i=0; i<PERIOD; i++)
      {
        vocbuf[i]=auxresampled[i]*level;
      }  

       filterbank[j].aux->filterout(vocbuf);
 
       for (i = 0; i<PERIOD; i++)
       { 
       filterbank[j].gain += fabs(vocbuf[i]);   
       };
       
       filterbank[j].gain *=cperiod;
       
      memcpy (tmpsmpsl , smpsl, PERIOD * sizeof(float));  
      memcpy (tmpsmpsr , smpsr, PERIOD * sizeof(float));  
 
      filterbank[j].l->filterout(tmpsmpsl);
      filterbank[j].r->filterout(tmpsmpsr);
 
      for (i = 0; i<PERIOD; i++)
       { 
       tmpl[i] += tmpsmpsl[i]*filterbank[j].gain;  //I need to add gain interpolation here between periods.
       tmpr[i] += tmpsmpsr[i]*filterbank[j].gain;        
       };  
      
       filterbank[j].oldgain = filterbank[j].gain;   
    }; 
    

      memcpy (efxoutl, tmpl, PERIOD * sizeof(float));  
      memcpy (efxoutr, tmpr, PERIOD * sizeof(float));  
 

};


/*
 * Parameter control
 */
void
Vocoder::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
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
float tmp;
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
    Pband = value;
      break;
    case 3: 
      tmp = powf (30.0f, ((float)value - 64.0f) / 64.0f);
      filterbank[Pband].l->setq (tmp);
      filterbank[Pband].r->setq (tmp);
      filterbank[Pband].aux->setq (tmp); 
      break;
    case 4:
      tmp = (float) value;
      filterbank[Pband].l->setfreq (tmp);
      filterbank[Pband].r->setfreq (tmp);
      filterbank[Pband].aux->setfreq (tmp);     
      break;     
      
    case 8:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      Plevel = value;
      level = dB2rap (80.0f * (float)Plevel / 127.0f - 40.0f);
      break;

    case 9:
      break;
    case 10:
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
      return(0);
      break;
    case 3:
      return(0); 
      break;  
    case 8:
      return (0);
      break;
    case 5:
      return (0);
      break;
    case 6:
      return (0);
      break;
    case 7:
      return(Plevel);
      break;
    case 4:
      return(0);
      break;
    case 9:
      return(0);
      break;   
    case 10:
      return(0);
      break; 

    };
  return (0);			//in case of bogus parameter number
};
