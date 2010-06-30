/*
  rakarrack - a guitar effects software

 Opticaltrem.C  -  Optical tremolo effect

  Copyright (C) 2008-2010 Ryan Billing
  Author: Josep Andreu


 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <math.h>
#include "Opticaltrem.h"

Opticaltrem::Opticaltrem (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

R1 = 2700.0f;	   //tremolo circuit series resistance 
Ra = 1000000.0f;  //Cds cell dark resistance.
Ra = logf(Ra);		//this is done for clarity 
Rb = 300;         //Cds cell full illumination
b = exp(logf(Ra)/logf(Rb)) - CNST_E;
dTC = 0.06f;
dRCl = dTC;
dRCr = dTC;   //Right & left channel dynamic time contsants
minTC = logf(0.005f/dTC);
alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);
alphar = alphal;
lstep = 0.0f;
rstep = 0.0f;
fdepth = 1.0f;  
oldgl = 0.0f;
oldgr = 0.0f;
gl = 0.0f;
gr = 0.0f;
cperiod = 1.0f/fPERIOD;

}

Opticaltrem::~Opticaltrem ()
{
}


void
Opticaltrem::cleanup ()
{


};

void
Opticaltrem::out (float *smpsl, float *smpsr)
{

  int i;
  float lfol, lfor, xl, xr, fxl, fxr;
  float rdiff, ldiff;
  lfo.effectlfoout (&lfol, &lfor);

  lfol = 1.0f - lfol*fdepth;
  lfor = 1.0f - lfor*fdepth;   
  
   if (lfol > 1.0f)
    lfol = 1.0f;
  else if (lfol < 0.0f)
    lfol = 0.0f;
  if (lfor > 1.0f)
    lfor = 1.0f;
  else if (lfor < 0.0f)
    lfor = 0.0f;  
    
    lfor = powf(lfor, 1.9f);
    lfol = powf(lfol, 1.9f);  //emulate lamp turn on/off characteristic
    
    //lfo interpolation
    rdiff = (lfor - oldgr)*cperiod;
    ldiff = (lfol - oldgl)*cperiod; 
    gr = lfor;
    gl = lfol;
    oldgr = lfor;
    oldgl = lfol;
    
  for (i = 0; i < PERIOD; i++)
    {
    //Left Cds   
    stepl = gl*(1.0f - alphal) + alphal*oldstepl;
    oldstepl = stepl;
    dRCl = dTC*expf(stepl*minTC);
    alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);  
    xl = CNST_E + stepl*b;
    fxl = expf(Ra/logf(xl));     
    fxl = R1/(fxl + R1);
    
    //Right Cds   
    stepr = gr*(1.0f - alphar) + alphar*oldstepr;
    oldstepr = stepr;
    dRCr = dTC*expf(stepr*minTC);
    alphar = 1.0f - cSAMPLE_RATE/(dRCr + cSAMPLE_RATE);        
    xr = CNST_E + stepr*b;
    fxr = expf(Ra/logf(xr));
    fxr = R1/(fxr + R1);  
    
    //Modulate input signal
    efxoutl[i] = 1.2f*fxl*smpsl[i];
    efxoutr[i] = 1.2f*fxr*smpsr[i];     
    
    gl += ldiff;
    gr += rdiff;  //linear interpolation of LFO 
    
    };

};



void
Opticaltrem::setpreset (int npreset)
{
  const int PRESET_SIZE = 5;
  const int NUM_PRESETS = 2;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //trem1
    {127, 60, 14, 0, 64},
    //trem2
    {64, 60, 14, 0, 64}
  };
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);

};

void
Opticaltrem::changepar (int npar, int value)
{

  switch (npar)
    {

    case 0:
      depth = value;
      fdepth = ((float) depth)/127.0f;    
      break;
    case 1:
      lfo.Pfreq = value;
      lfo.updateparams ();
      break;
    case 2:
      lfo.Prandomness = value;
      lfo.updateparams ();
      break;
    case 3:
      lfo.PLFOtype = value;
      lfo.updateparams ();
      break;
    case 4:
      lfo.Pstereo = value;
      lfo.updateparams ();
      break;
    case 5: // pan
      break;  
    }
   
};

int
Opticaltrem::getpar (int npar)
{

  switch (npar)

    {
    case 0:
      return (depth);
      break;
    case 1:
      return (lfo.Pfreq);
      break;
    case 2:
      return (lfo.Prandomness);
      break;
    case 3:
      return (lfo.PLFOtype);
      break;
    case 4:
      return (lfo.Pstereo);
      break;
    case 5:
      return (0); //pan

    }

  return (0);

};


