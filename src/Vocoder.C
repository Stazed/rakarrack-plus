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
 
       Pmuffle = 100;
      float tmp = (float) Pmuffle/1000.0f;
      alpha = cSAMPLE_RATE/(cSAMPLE_RATE + tmp);
      beta = 1.0f - alpha; 

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
      qq = 5.0f * center/bw;
      filterbank[i].sfreq = center;
      filterbank[i].sq = qq;
      filterbank[i].l = new AnalogFilter (4, center, qq, 0);
      filterbank[i].r = new AnalogFilter (4, center, qq, 0);
      filterbank[i].aux = new AnalogFilter (4, center, qq, 0);
    };
    
    vlp = new AnalogFilter (2, 3600.0f, 0.707f, 0);
    vhp = new AnalogFilter (3, 300.0f, 0.707f, 0);
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
  float tempgain1, tempgain2;
  float maxgain=0.0f;

    memset(tmpl,0,sizeof(float)*PERIOD);
    memset(tmpr,0,sizeof(float)*PERIOD);
    
    
   vlp->filterout(auxresampled);
   vhp->filterout(auxresampled);

    for (j = 0; j < VOC_BANDS; j++)
    {

      for(i=0; i<PERIOD; i++)
      {
        vocbuf[i]=auxresampled[i]*level;
      }  

       filterbank[j].aux->filterout(vocbuf);
 
       tempgain1 = filterbank[j].gain;
       for (i = 0; i<PERIOD; i++)
       { 
       filterbank[j].gain = beta * filterbank[j].oldgain + alpha * fabs(vocbuf[i]);   
       filterbank[j].oldgain = filterbank[j].gain; 
       };
      tempgain2 = cperiod * (filterbank[j].gain - tempgain1);    
      if(tempgain1>maxgain) maxgain = tempgain1; //vu meter level.  
       vulevel = rap2dB (maxgain) ; 
       
      memcpy (tmpsmpsl , smpsl, PERIOD * sizeof(float));  
      memcpy (tmpsmpsr , smpsr, PERIOD * sizeof(float));  
 
      filterbank[j].l->filterout(tmpsmpsl);
      filterbank[j].r->filterout(tmpsmpsr);
 
      for (i = 0; i<PERIOD; i++)
       { 
       tmpl[i] += tmpsmpsl[i] * tempgain1;  //I need to add gain interpolation here between periods.
       tmpr[i] += tmpsmpsr[i] * tempgain1;
       tempgain1+=tempgain2;   
        
       };  
      
  
    }; 
    
      for (i = 0; i<PERIOD; i++)
       { 
       efxoutl[i]=tmpl[i]*lpanning;  //I need to add gain interpolation here between periods.
       efxoutr[i]=tmpr[i]*rpanning;
       };  
 

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
Vocoder::adjustq(float q)
{
 
  for (int ii = 0; ii < VOC_BANDS; ii++)
    {
      filterbank[ii].l->setq (q);
      filterbank[ii].r->setq (q);
      filterbank[ii].aux->setq (q);
    };

}

void
Vocoder::setpreset (int npreset)
{
  const int PRESET_SIZE = 5;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Vocoder 1
    {0, 64, 10, 70, 70},
    //Vocoder 2
    {0, 64, 14, 80, 70},
    //Vocoder 3
    {0, 64, 20, 90, 70},
    //Vocoder 4
    {0, 64, 30, 100, 70}
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
float tmp = 0;
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
      Pmuffle = value;
      tmp = (float) Pmuffle/10000.0f;
      alpha = cSAMPLE_RATE/(cSAMPLE_RATE + tmp);
      beta = 1.0f - alpha; 
      break;
    case 3: 
      Pqq = value;
      tmp = (float) value;
      adjustq(tmp);
      break;
    case 4:
      Plevel = value;
      level = dB2rap (80.0f * (float)Plevel / 70.0f - 40.0f);    
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
      return(Pmuffle);
      break;
    case 3:
      return(Pqq); 
      break;  
    case 4:
      return (Plevel);
      break;
  

    };
  return (0);			//in case of bogus parameter number
};
