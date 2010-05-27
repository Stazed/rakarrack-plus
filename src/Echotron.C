/*
 
  Echotron.C - Echotron effect
  Author: Ryan Billing & Josep Andreu
  
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

#include "Echotron.h"

Echotron::Echotron (float * efxoutl_, float * efxoutr_)
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
  Plength = 10;
  Puser = 0;
  fb = 0.0f;
  lfeedback = 0.0f;
  rfeedback = 0.0f;

  maxx_size = (SAMPLE_RATE * 6);   //6 Seconds delay time

  lxn = (float *) malloc (sizeof (float) * (1 + maxx_size)); 
  rxn = (float *) malloc (sizeof (float) * (1 + maxx_size));    

  offset = 0;

  lpfl =  new AnalogFilter (0, 800, 1, 0);;
  lpfr =  new AnalogFilter (0, 800, 1, 0);;

   float center, qq;
  for (int i = 0; i < 8; i++)
    {
      center = 500;
      qq = 1.0f;
      filterbank[i].sfreq = center;
      filterbank[i].sq = qq;
      filterbank[i].sLP = 0.25f;  
      filterbank[i].sBP = -1.0f;
      filterbank[i].sHP = 0.5f;   
      filterbank[i].sStg = 1.0f;               
      filterbank[i].l = new RBFilter (0, center, qq, 0);
      filterbank[i].r = new RBFilter (0, center, qq, 0);
      
      filterbank[i].l->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);
      filterbank[i].r->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);      
    };
    
   setpreset (Ppreset);
  cleanup ();
};

Echotron::~Echotron ()
{
};

/*
 * Cleanup the effect
 */
void
Echotron::cleanup ()
{
memset(lxn,0,sizeof(float)*maxx_size);
memset(rxn,0,sizeof(float)*maxx_size);

lpfl->cleanup ();
lpfr->cleanup ();

};

/*
 * Effect output
 */
void
Echotron::out (float * smpsl, float * smpsr)
{
  int i, j, k, rxindex, lxindex;
  float l,r,lyn, ryn;
  int length = Plength;

if((Pmoddly)||(Pmodfilts)) modulate_delay();  

  float tmpmodl = oldldmod;
  float tmpmodr = oldrdmod;
  int intmodl, intmodr;
  
  for (i = 0; i < PERIOD; i++)
    {
      tmpmodl+=interpl;
      tmpmodr+=interpr;
      intmodl = lrintf(tmpmodl);
      intmodr = lrintf(tmpmodr);

      l = lpfl->filterout_s(smpsl[i] + lfeedback);  //High Freq damping 
      r = lpfr->filterout_s(smpsr[i] + rfeedback);

      lxn[offset] = l;
      rxn[offset] = r;
      
      //Convolve 
      lyn = 0.0f;
      ryn = 0.0f;

      if(Pfilters)
      {
      j=0;
      for (k=0; k<length; k++)
      {    
  
      lxindex = offset + ltime[k] + intmodl;
      rxindex = offset + rtime[k] + intmodr;
      if(lxindex>=maxx_size) lxindex -= maxx_size;
      if(rxindex>=maxx_size) rxindex -= maxx_size;
      lyn += filterbank[j].l->filterout_s(lxn[lxindex]) * ldata[k];		//filter each tap
      ryn += filterbank[j].r->filterout_s(rxn[rxindex]) * rdata[k];
      if (++j>ECHOTRON_MAXFILTERS) j=0;
      }

      }
      else
      {      
      for (k=0; k<length; k++)
      {   
  
      lxindex = offset + ltime[k] + intmodl;
      rxindex = offset + rtime[k] + intmodr;
      if(lxindex>=maxx_size) lxindex -= maxx_size;
      if(rxindex>=maxx_size) rxindex -= maxx_size;
      lyn += lxn[lxindex] * ldata[k];		
      ryn += rxn[rxindex] * rdata[k];
      }      

      }

      lfeedback =  (lrcross*ryn + ilrcross*lyn) * lpanning;
      rfeedback = (lrcross*lyn + ilrcross*ryn) * rpanning;
      efxoutl[i] = lfeedback;
      efxoutr[i] = rfeedback;       
      lfeedback *= fb;
      rfeedback *= fb;      

      offset--;
      if (offset<0) offset = maxx_size;   
     
    };



};


/*
 * Parameter control
 */
void
Echotron::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Echotron::setpanning (int value)
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
};

int
Echotron::setfile(int value)
{

FILE *fs;

char wbuf[128];

if(!Puser)
{
Filenum = value;

memset(Filename,0,sizeof(Filename));
sprintf(Filename, "%s/%d.dly",DATADIR,Filenum+1);
}

if ((fs = fopen (Filename, "r")) == NULL) return(0);

memset(wbuf,0,sizeof(wbuf));
fgets(wbuf,sizeof wbuf,fs); //Read Header
memset(wbuf,0,sizeof(wbuf));

int count = 0;

    while ((fgets(wbuf,sizeof wbuf,fs) != NULL) && (count<ECHOTRON_F_SIZE))
    {  
     sscanf(wbuf,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",&fPan[count], &fTime[count], &fLevel[count],
      &fLP[count],  &fBP[count],  &fHP[count],  &fFreq[count],  &fQ[count],  &iStages[count]);
     memset(wbuf,0,sizeof(wbuf));
    count++;
    }
    fclose(fs);  

Plength=count+1;
cleanup();
init_params();
return(1);
};

void Echotron::init_params()
{

float hSR = fSAMPLE_RATE*0.5f;
float tmp_time;
float tpanl, tpanr;
float len = 1.0f / (float)Plength;

for(int i=0; i<Plength; i++)
{
tmp_time=lrintf(fTime[i]*tempo_coeff*fSAMPLE_RATE);
if(tmp_time<maxx_size) rtime[i]=tmp_time; else rtime[i]=maxx_size;

ltime[i] = rtime[i];  
 
tpanl = 1.0f + fPan[i];
tpanr = 2.0f-tpanl;
tpanl = 10*powf(tpanl,4.0f);
tpanr = 10.0f*powf(tpanr,4.0f);
tpanr = 1.0f-1.0f/(tpanr + 1.0f);
tpanl = 1.0f -1.0f/(tpanl + 1.0f);
tpanr = 1.1f*tpanr;
tpanl = 1.1f*tpanl;

ldata[i]=fLevel[i]*tpanl;
rdata[i]=fLevel[i]*tpanr;

if(i<ECHOTRON_MAXFILTERS)
{

 int Freq=fFreq[i]*powf(2.0f,depth*4.5f);
 if (Freq<20.0) Freq=20.0f;
 if (Freq>hSR) Freq=hSR;
 filterbank[i].l->setfreq_and_q(Freq,fQ[i]);
 filterbank[i].r->setfreq_and_q(Freq,fQ[i]);
 filterbank[i].l->setstages(iStages[i]);
 filterbank[i].r->setstages(iStages[i]);
 filterbank[i].l->setmix (1, fLP[i] , fBP[i], fHP[i]);
 filterbank[i].r->setmix (1, fLP[i] , fBP[i], fHP[i]);      
}
}

for(int i=0; i<Plength; i++)
{
ldata[i] *=len;
rdata[i] *=len;
}


};

void Echotron::modulate_delay()
{

float lfmod, rfmod, lfol, lfor;
float fperiod = 1.0f/((float) PERIOD);

  lfo.effectlfoout (&lfol, &lfor);

if(Pmodfilts)
{  
  lfmod = powf(2.0f,(lfol*width + 0.25f + depth)*4.5f);
  rfmod = powf(2.0f,(lfor*width + 0.25f + depth)*4.5f); 
for(int i=0; i<ECHOTRON_MAXFILTERS; i++)
{
 
 filterbank[i].l->setfreq(lfmod*fFreq[i]);
 filterbank[i].r->setfreq(rfmod*fFreq[i]);
     
}

}

if(Pmoddly)
{
oldldmod = ldmod;
oldrdmod = rdmod;
ldmod = width*lfol;
rdmod = width*lfor;

ldmod=lrintf(0.05f*tempo_coeff*fSAMPLE_RATE*ldmod);
rdmod=lrintf(0.05f*tempo_coeff*fSAMPLE_RATE*rdmod);

interpl = (ldmod - oldldmod)*fperiod;
interpr = (rdmod - oldrdmod)*fperiod;
}

};


void
Echotron::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.1f;
  float fr = 20.0f*powf(2.0, hidamp*10.0f);
  lpfl->setfreq (fr);
  lpfr->setfreq (fr);
};

void
Echotron::setfb(int value)
{

   fb = (float)value/64.0f;
}


void
Echotron::setpreset (int npreset)
{
  const int PRESET_SIZE = 16;
  const int NUM_PRESETS = 1;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Test
    {64, 0, 1, 8, 0, 60, 0, 64, 0, 0, 0, 64, 0, 0, 0, 0}

  };

  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Echotron::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      Pdepth=value;
      depth = ((float) (value - 64))/64.0f;
      init_params();
      break;
    case 2:
      Pwidth=value;
      width = ((float) value)/127.0f;
      init_params();
      break;
    case 3:
     Plength = value;
     if(Plength>127) Plength = 127;
     init_params(); 
     break;
    case 4:
      Puser = value;
      break;
    case 5:
      Ptempo = value;
      tempo_coeff = 60.0f / ((float)Ptempo);
      lfo.Pfreq = Ptempo;
      lfo.updateparams ();      
      init_params();
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      Plrcross = value;
      lrcross = ((float)(Plrcross)-64)/64.0;
      ilrcross = 1.0f - abs(lrcross);      
      break;
    case 8:
      if(!setfile(value)) error_num=4;
      break;
    case 9:
      lfo.Pstereo = value;
      lfo.updateparams ();
      break;
    case 10:
      Pfb = value;
      setfb(value);
      break;
    case 11:
      setpanning (value);
      break;
    case 12:
      Pmoddly = value;//delay modulation on/off
      break;
    case 13:
      Pmodfilts = value;//filter modulation on/off
      break;     
    case 14:
      //LFO Type      
      lfo.PLFOtype = value;
      lfo.updateparams ();
      break;
    case 15:
      Pfilters = value;//Pfilters
      break;
   };
};

int
Echotron::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Pdepth);
      break;
    case 2:
      return(Pwidth);
      break;
    case 3:
      return(Plength); 
      break;  
    case 8:
      return (Filenum);
      break;
    case 5:
      return (Ptempo);
      break;
    case 6:
      return (Phidamp);
      break;
    case 7:
      return(Plrcross);
      break;
    case 4:
      return(Puser);
      break;
    case 9:
      return(lfo.Pstereo);
      break;   
    case 10:
      return(Pfb);
      break; 
    case 11:
      return(Ppanning);
      break;
    case 12:
      return(Pmoddly);  //modulate delays
      break;
    case 13:
      return(Pmodfilts); //modulate filters
      break;  
    case 14:
      return(lfo.PLFOtype);
      break;  
    case 15:
      return(Pfilters);  //Filter delay line on/off
      break;  


    };
  return (0);			//in case of bogus parameter number
};
