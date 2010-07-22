/*
  rakarrack - a guitar effects software

 Vibe.C  -  Optical tremolo effect

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
#include "Vibe.h"

Vibe::Vibe (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

//Swing was measured on operating device of: 10K to 250k.  
//400K is reported to sound better for the "low end" (high resistance) 
//Because of time response, Rb needs to be driven further.
//End resistance will max out to around 10k for most LFO freqs.
//pushing low end a little lower for kicks and giggles
Ra = 450000.0f;  //Cds cell dark resistance.
Ra = logf(Ra);		//this is done for clarity 
Rb = 1000.0f;         //Cds cell full illumination
b = exp(Ra/logf(Rb)) - CNST_E;
dTC = 0.08f;
dRCl = dTC;
dRCr = dTC;   //Right & left channel dynamic time contsants
minTC = logf(0.005f/dTC);
alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);
alphar = alphal;
dalphal = dalphar = alphal;
lstep = 0.0f;
rstep = 0.0f;
Pdepth = 127;
Ppanning = 64;
lpanning = 1.0f;
rpanning = 1.0f;
fdepth = 1.0f;  
oldgl = 0.0f;
oldgr = 0.0f;
gl = 0.0f;
gr = 0.0f;
for(int jj = 0; jj<8; jj++) oldcvolt[jj] = 0.0f;
cperiod = 1.0f/fPERIOD;

init_vibes();
cleanup();

}

Vibe::~Vibe ()
{
}


void
Vibe::cleanup ()
{


};

void
Vibe::out (float *smpsl, float *smpsr)
{

  int i,j;
  float lfol, lfor, xl, xr, fxl, fxr;
  float rdiff, ldiff;
  float cvolt, ocvolt, evolt, input;
  float emitterfb = 0.0f;
  
  input = cvolt = ocvolt = evolt = 0.0f;
  
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
    stepl = gl*(1.0f - alphal) + dalphal*oldstepl;
    oldstepl = stepl;
    dRCl = dTC*expf(stepl*minTC);
    alphal = 1.0f - cSAMPLE_RATE/(dRCl + cSAMPLE_RATE);  
    dalphal = 1.0f - cSAMPLE_RATE/(0.5f*dRCl + cSAMPLE_RATE);     //different attack & release character
    xl = CNST_E + stepl*b;
    fxl = expf(Ra/logf(xl));   
    
    //Right Cds   
    stepr = gr*(1.0f - alphar) + dalphar*oldstepr;
    oldstepr = stepr;
    dRCr = dTC*expf(stepr*minTC);
    alphar = 1.0f - cSAMPLE_RATE/(dRCr + cSAMPLE_RATE);  
    dalphar = 1.0f - cSAMPLE_RATE/(0.5f*dRCr + cSAMPLE_RATE);      //different attack & release character
    xr = CNST_E + stepr*b;
    fxr = expf(Ra/logf(xr));
    
    modulate(fxl, fxr);   
     
    //Left Channel  
    input = bjt_shape(smpsl[i]);  
    
    emitterfb = 1.0f/fxl;     
    for(j=0;j<4;j++) //4 stages phasing
    {
    cvolt = vibefilter(input,ecvc,j) + vibefilter(input + emitterfb*oldcvolt[j],vc,j);
    ocvolt = vibefilter(cvolt,vcvo,j);
    oldcvolt[j] = ocvolt;
    evolt = vibefilter(input, vevo,j);
    
    input = bjt_shape(ocvolt + evolt);

    }
    efxoutl[i] = lpanning*input;    
    
    //Right channel
    input = bjt_shape(smpsr[i]);  
    
    emitterfb = 25.0f/fxr;     
    for(j=4;j<8;j++) //4 stages phasing
    {
    cvolt = vibefilter(input,ecvc,j) + vibefilter(input + emitterfb*oldcvolt[j],vc,j);
    ocvolt = vibefilter(cvolt,vcvo,j);
    oldcvolt[j] = ocvolt;
    evolt = vibefilter(input, vevo,j);
    
    input = bjt_shape(ocvolt + evolt);

    }
    efxoutr[i] = rpanning*input;     
    
    
    gl += ldiff;
    gr += rdiff;  //linear interpolation of LFO 
    
    };

};

float 
Vibe::vibefilter(float data, fparams *ftype, int stage)
{
float y0 = 0.0f;
y0 = data*ftype[stage].n0 + ftype[stage].x1*ftype[stage].n1 - ftype[stage].y1*ftype[stage].d1;
ftype[stage].y1 = y0;
ftype[stage].x1 = data; 
return y0;
};

float 
Vibe::bjt_shape(float data)
{
float vbe, vout;
float vin = 7.5f*(1.0f + data);
if(vin<0.0f) vin = 0.0f;
if(vin>15.0f) vin = 15.0f;
vbe = 0.8f - 0.8f/(vin + 1.0f);  //really rough, simplistic bjt turn-on emulator
vout = vin - vbe;
vout = vout*0.1333333333f -0.90588f;  //some magic numbers to return gain to unity & zero the DC
return vout;

}

void
Vibe::init_vibes()
{
k = 2.0f*fSAMPLE_RATE;
float tmpgain = 1.0f;
 R1 = 4700.0f;
 Rv = 4700.0f;
 C2 = 1e-6f;
 beta = 150.0f;  //transistor forward gain.
 gain = -beta/(beta + 1.0f); 

//Univibe cap values 0.015uF, 0.22uF, 470pF, and 0.0047uF
C1[0] = 0.015e-6f;
C1[1] = 0.22e-6f;
C1[2] = 470e-12f;
C1[3] = 0.0047e-6f;
C1[4] = 0.015e-6f;
C1[5] = 0.22e-6f;
C1[6] = 470e-12f;
C1[7] = 0.0047e-6f;

for(int i =0; i<8; i++)
{
//Vo/Ve driven from emitter
en1[i] = R1*C1[i];
en0[i] = 1.0f;
ed1[i] = (R1 + Rv)*C1[i];
ed0[i] = 1.0f + C1[i]/C2;

// Vc~=Ve/(Ic*Re*alpha^2) collector voltage from current input.  
//Output here represents voltage at the collector

cn1[i] = gain*Rv*C1[i];
cn0[i] = gain*(1.0f + C1[i]/C2);
cd1[i] = (R1 + Rv)*C1[i];
cd0[i] = 1.0f + C1[i]/C2;

//Contribution from emitter load through passive filter network
ecn1[i] = gain*R1*(R1 + Rv)*C1[i]*C2/(Rv*(C2 + C1[i]));
ecn0[i] = 0.0f;
ecd1[i] = (R1 + Rv)*C1[i]*C2/(C2 + C1[i]);
ecd0[i] = 1.0f;

// %Represents Vo/Vc.  Output over collector voltage
on1[i] = Rv*C2;
on0[i] = 1.0f;
od1[i] = Rv*C2;
od0[i] = 1.0f + C2/C1[i];

//%Bilinear xform stuff
tmpgain =  1.0f/(k*cd1[i] + cd0[i]);
vc[i].n1 = tmpgain*(cn0[i] - k*cn1[i]);
vc[i].n0 = tmpgain*(k*cn1[i] + cn0[i]);
vc[i].d1 = tmpgain*(cd0[i] - k*cd1[i]);
vc[i].d0 = 1.0f;

tmpgain =  1.0f/(k*ecd1[i] + ecd0[i]);
ecvc[i].n1 = tmpgain*(ecn0[i] - k*ecn1[i]);
ecvc[i].n0 = tmpgain*(k*ecn1[i] + ecn0[i]);
ecvc[i].d1 = tmpgain*(ecd0[i] - k*ecd1[i]);
ecvc[i].d0 = 1.0f;

tmpgain =  1.0f/(k*od1[i] + od0[i]);
vcvo[i].n1 = tmpgain*(on0[i] - on1[i]*k);
vcvo[i].n0 = tmpgain*(k*on1[i] + on0[i]);
vcvo[i].d1 = tmpgain*(od0[i] - k*od1[i]);
vcvo[i].d0 = 1.0f;

tmpgain =  1.0f/(k*ed1[i] + ed0[i]);
vevo[i].n1 = tmpgain*(en0[i] - en1[i]*k);
vevo[i].n0 = tmpgain*(k*en1[i] + en0[i]);
vevo[i].d1 = tmpgain*(ed0[i] - k*ed1[i]);
vevo[i].d0 = 1.0f;

// bootstrap[i].n1
// bootstrap[i].n0
// bootstrap[i].d1
}


};

void
Vibe::modulate(float ldrl, float ldrr)
{
float tmpgain;
 Rv = 4700.0f + ldrl;

for(int i =0; i<8; i++)
{
if(i==4) Rv = 4700.0f + ldrr;

//Vo/Ve driven from emitter
ed1[i] = (R1 + Rv)*C1[i];

// Vc~=Ve/(Ic*Re*alpha^2) collector voltage from current input.  
//Output here represents voltage at the collector
cn1[i] = gain*Rv*C1[i];
cd1[i] = (R1 + Rv)*C1[i];

//Contribution from emitter load through passive filter network
ecn1[i] = gain*R1*(R1 + Rv)*C1[i]*C2/(Rv*(C2 + C1[i]));
ecd1[i] = (R1 + Rv)*C1[i]*C2/(C2 + C1[i]);

// %Represents Vo/Vc.  Output over collector voltage
on1[i] = Rv*C2;
od1[i] = on1[i];

//%Bilinear xform stuff
tmpgain =  1.0f/(k*cd1[i] + cd0[i]);
vc[i].n1 = tmpgain*(cn0[i] - k*cn1[i]);
vc[i].n0 = tmpgain*(k*cn1[i] + cn0[i]);
vc[i].d1 = tmpgain*(cd0[i] - k*cd1[i]);

tmpgain =  1.0f/(k*ecd1[i] + ecd0[i]);
ecvc[i].n1 = tmpgain*(ecn0[i] - k*ecn1[i]);
ecvc[i].n0 = tmpgain*(k*ecn1[i] + ecn0[i]);
ecvc[i].d1 = tmpgain*(ecd0[i] - k*ecd1[i]);
ecvc[i].d0 = 1.0f;

tmpgain =  1.0f/(k*od1[i] + od0[i]);
vcvo[i].n1 = tmpgain*(on0[i] - on1[i]*k);
vcvo[i].n0 = tmpgain*(k*on1[i] + on0[i]);
vcvo[i].d1 = tmpgain*(od0[i] - k*od1[i]);

tmpgain =  1.0f/(k*ed1[i] + ed0[i]);
vevo[i].n1 = tmpgain*(en0[i] - en1[i]*k);
vevo[i].n0 = tmpgain*(k*en1[i] + en0[i]);
vevo[i].d1 = tmpgain*(ed0[i] - k*ed1[i]);

}


};

void
Vibe::setpanning (int value)
{
  Ppanning = value;
  rpanning = ((float)Ppanning) / 64.0f;
  lpanning = 2.0f - rpanning;
  lpanning = 10.0f * powf(lpanning, 4);
  rpanning = 10.0f * powf(rpanning, 4);
  lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
  rpanning = 1.0f - 1.0f/(rpanning + 1.0f); 
  lpanning *= 1.3f;
  rpanning *= 1.3f; 
};


void
Vibe::setvolume (int value)
{
  Pvolume = value;
  outvolume = (float)Pvolume / 127.0f;
};



void
Vibe::setpreset (int npreset)
{
  const int PRESET_SIZE = 6;
  const int NUM_PRESETS = 6;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Fast
    {127, 260, 10, 0, 64, 64},
    //trem2
    {45, 140, 10, 0, 64, 64},
    //hard pan
    {127, 120, 10, 5, 0, 64},
    //soft pan
    {45, 240, 10, 1, 16, 64},    
    //ramp down
    {65, 200, 0, 3, 32, 64},
    //hard ramp
    {127, 480, 0, 3, 32, 64}  
    
  };
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);

};

void
Vibe::changepar (int npar, int value)
{

  switch (npar)
    {

    case 0:
      Pdepth = value;
      fdepth = 0.5f + ((float) Pdepth)/254.0f;    
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
    setpanning(value);
      break;  
    case 6:
    setvolume(value);
      break;
    

    }
   
};

int
Vibe::getpar (int npar)
{

  switch (npar)

    {
    case 0:
      return (Pdepth);
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
      return (Ppanning); //pan
    case 6:
      return(Pvolume);

    }

  return (0);

};


