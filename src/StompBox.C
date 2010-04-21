/*
  Rakarrack   Audio FX software 
  Stompbox.C - stompbox modeler
  Modified for rakarrack by Ryan Billing
  
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
#include "StompBox.h"



StompBox::StompBox (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;


  //default values
  Ppreset = 0;
  Pvolume = 50;
  
  //left channel filters
  linput = new AnalogFilter (1, 80.0f, 1.0f, 0);  //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
  lpre1 = new AnalogFilter (1, 630.0f, 1.0f, 0);   // LPF = 0, HPF = 1
  lpre2 = new AnalogFilter (1, 220.0f, 1.0f, 0);
  lpost = new AnalogFilter (0, 720.0f, 1.0f, 0);
  ltonehg = new AnalogFilter (1, 1500.0f, 1.0f, 0);  
  ltonemd = new AnalogFilter (4, 1000.0f, 1.0f, 0);  
  ltonelw = new AnalogFilter (0, 500.0f, 1.0, 0); 
  
  //Right channel filters
  rinput = new AnalogFilter (1, 80.0f, 1.0f, 0);  //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
  rpre1 = new AnalogFilter (1, 630.0f, 1.0f, 0);   // LPF = 0, HPF = 1
  rpre2 = new AnalogFilter (1, 220.0f, 1.0f, 0);
  rpost = new AnalogFilter (0, 720.0f, 1.0f, 0);
  rtonehg = new AnalogFilter (1, 1500.0f, 1.0f, 0);  
  rtonemd = new AnalogFilter (4, 1000.0f, 1.0f, 0);  
  rtonelw = new AnalogFilter (0, 500.0f, 1.0f, 0); 

  //Anti-aliasing for between stages
  ranti = new AnalogFilter (0, 8000.0f, 0.707f, 2); 
  lanti = new AnalogFilter (0, 8000.0f, 0.707f, 2);  
  
  rwshape = new Waveshaper();
  lwshape = new Waveshaper();
    
  cleanup ();

  setpreset (Ppreset);
};

StompBox::~StompBox ()
{
};

/*
 * Cleanup the effect
 */
void
StompBox::cleanup ()
{

 
};


/*
 * Effect output
 */
void
StompBox::out (float * smpsl, float * smpsr)
{
int i;

    float hfilter;  //temporary variables
    float mfilter;
    float lfilter;
    float tempr;
    float templ;
    
   switch (Pmode)
   {
    case 0:          //Odie
         
    lpre1->filterout(smpsl);
    rpre1->filterout(smpsr);
    rwshape->waveshapesmps (PERIOD, smpsl, 19, Pgain, 1);  //compression
    lwshape->waveshapesmps (PERIOD, smpsr, 19, Pgain, 1);   
    lpost->filterout(smpsl);
    rpost->filterout(smpsr);   
    
    for (i = 0; i<PERIOD; i++)
    {
    //left channel
    lfilter =  ltonelw->filterout_s(smpsl[i]);
    mfilter =  ltonemd->filterout_s(smpsl[i]); 
    hfilter =  ltonehg->filterout_s(smpsl[i]);  
    
    efxoutl[i] = 0.5f * volume * (smpsl[i] + lowb*lfilter + midb*mfilter + highb*hfilter);
    
    //Right channel
    lfilter =  rtonelw->filterout_s(smpsr[i]);
    mfilter =  rtonemd->filterout_s(smpsr[i]); 
    hfilter =  rtonehg->filterout_s(smpsr[i]);  
    
    efxoutr[i] = 0.5f * volume * (smpsr[i] + lowb*lfilter + midb*mfilter + highb*hfilter);      
       
    }
    
    break; 

    case 1:  //Grunge
    
    linput->filterout(smpsl);
    rinput->filterout(smpsr);    
  
    for (i = 0; i<PERIOD; i++)
    {   
    templ = smpsl[i] * gain * 37.0f;
    tempr = smpsr[i] * gain * 37.0f;  
    smpsl[i] += lpre1->filterout_s(templ);
    smpsr[i] += rpre1->filterout_s(tempr);       
    }     
    rwshape->waveshapesmps (PERIOD, smpsl, 15, 1, 1);  // Op amp limiting
    lwshape->waveshapesmps (PERIOD, smpsr, 15, 1, 1); 
    
    ranti->filterout(smpsr);
    lanti->filterout(smpsl);  
    
     for (i = 0; i<PERIOD; i++)
    {   
    smpsl[i]*=1.5f;
    smpsr[i]*=1.5f;     
    }  
       
    rwshape->waveshapesmps (PERIOD, smpsl, 23, Pgain, 1);  // hard crunch
    lwshape->waveshapesmps (PERIOD, smpsr, 23, Pgain, 1);   
       
    
    for (i = 0; i<PERIOD; i++)
    {
    smpsl[i] = smpsl[i] + 10.0f * lpre2->filterout_s(smpsl[i]);
    smpsr[i] = smpsr[i] + 10.0f * rpre2->filterout_s(smpsr[i]);    
    smpsl[i] = smpsl[i] + 3.0f * lpost->filterout_s(smpsl[i]);
    smpsr[i] = smpsr[i] + 3.0f * rpost->filterout_s(smpsr[i]); 
    
    //left channel
    lfilter =  ltonelw->filterout_s(smpsl[i]);
    mfilter =  ltonemd->filterout_s(smpsl[i]); 
    hfilter =  ltonehg->filterout_s(smpsl[i]);  
    
    efxoutl[i] = 0.05f * volume * (smpsl[i] + lowb*lfilter + midb*mfilter + highb*hfilter);
    
    //Right channel
    lfilter =  rtonelw->filterout_s(smpsr[i]);
    mfilter =  rtonemd->filterout_s(smpsr[i]); 
    hfilter =  rtonehg->filterout_s(smpsr[i]);  
    
    efxoutr[i] = 0.05f * volume * (smpsr[i] + lowb*lfilter + midb*mfilter + highb*hfilter);      
       
    } 
    
    
    
    break;


   } 



};


/*
 * Parameter control
 */
void StompBox::init_mode (int value)
{
  int tinput = 1;
  float finput = 80.0f;
  float qinput = 1.0f;
  int sinput = 0;
  
  int tpre1 = 1;
  float fpre1 = 630.0f;
  float qpre1 = 1.0f;
  int spre1 = 0;
  
  int tpre2 = 1;
  float fpre2 = 220.0f;
  float qpre2 = 1.0f;
  int spre2 = 0;
  
  int tpost = 0;
  float fpost = 720.0f;
  float qpost = 1.0f;
  int spost = 0;
  
  int ttonehg = 1;
  float ftonehg = 1500.0f;
  float qtonehg = 1.0f;
  int stonehg = 0;
  
  int ttonemd = 4;
  float ftonemd = 720.0f;
  float qtonemd = 1.0f;
  int stonemd = 0;
  
  int ttonelw = 0;
  float ftonelw = 500.0f;
  float qtonelw = 1.0f;
  int stonelw = 0;

switch (value)
  {  
  case 0:
  tinput = 1;
  finput = 80.0f;
  qinput = 1.0f;
  sinput = 0;
  
  tpre1 = 1;
  fpre1 = 630.0f;
  qpre1 = 1.0f;
  spre1 = 0;
  
  tpre2 = 1;
  fpre2 = 220.0f;
  qpre2 = 1.0f;
  spre2 = 0;
  
  tpost = 0;
  fpost = 720.0f;
  qpost = 1.0f;
  spost = 0;
  
  ttonehg = 1;
  ftonehg = 1500.0f;
  qtonehg = 1.0f;
  stonehg = 0;
  
  ttonemd = 4;
  ftonemd = 720.0f;
  qtonemd = 1.0f;
  stonemd = 0;
  
  ttonelw = 0;
  ftonelw = 500.0f;
  qtonelw = 1.0f;
  stonelw = 0;    
  break;
  
  case 1: //Grunge
// Some key filter stages based upon a schematic for a grunge pedal 
// Total gain up to 25,740/2 (91dB)
// Fc1 =  999.02  Gain = 110 = 40.8dB
// Q1 =  2.9502
// gain stage 1rst order HP @ 340 Hz, Gain = 21.3 ... 234 (26dB ... 47dB)
// Fc2 =  324.50
// Q2 =  4.5039
// Fc3 =  5994.1
// Q3 =  1.7701
// Fc4 =  127.80
// Q4 =  3.7739

  tinput = 4;         //Pre-Emphasis filter
  finput = 1000.0f;
  qinput = 2.95f;
  sinput = 0;
  
  tpre1 = 0;         //Gain stage reduce aliasing
  fpre1 = 6000.0f;
  qpre1 = 0.707f;
  spre1 = 1;
  
  tpre2 = 4;        //being used as a recovery filter, gain = 10
  fpre2 = 324.5f;
  qpre2 = 4.5f;
  spre2 = 0;
  
  tpost = 4;       //The other recovery filter, gain = 3
  fpost = 6000.0f;
  qpost = 1.77f;
  spost = 0;
  
  ttonehg = 1;       //high shelf ranging 880 to 9700 Hz, gain 10
  ftonehg = 4000.0f;
  qtonehg = 1.0f;
  stonehg = 0;
  
  ttonemd = 4;       // Pedal has no mid filter so I'll make up my own
  ftonemd = 1000.0f;
  qtonemd = 2.0f;
  stonemd = 0;
  
  ttonelw = 4;       //Low Eq band, peaking type, gain = up to 22.
  ftonelw = 128.0f;
  qtonelw = 3.8f;
  stonelw = 0;  
  break;
 
  }
   
  //left channel filters
  //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
  // LPF = 0, HPF = 1
  linput->settype(tinput);
  linput->setfreq_and_q(finput, qinput);
  linput->setstages(sinput);
  
  lpre1->settype(tpre1);
  lpre1->setfreq_and_q(fpre1, qpre1);
  lpre1->setstages(spre1);  
  
  lpre2->settype(tpre2);
  lpre2->setfreq_and_q(fpre2, qpre2);
  lpre2->setstages(spre2); 
  
  lpost->settype(tpost);
  lpost->setfreq_and_q(fpost, qpost);
  lpost->setstages(spost); 
  
  ltonehg->settype(ttonehg);
  ltonehg->setfreq_and_q(ftonehg,qtonehg);
  ltonehg->setstages(stonehg);   
  
  ltonemd->settype(ttonemd);
  ltonemd->setfreq_and_q(ftonemd,qtonemd);
  ltonemd->setstages(stonemd);  

  ltonelw->settype(ttonelw);
  ltonelw->setfreq_and_q(ftonelw, qtonelw);
  ltonelw->setstages(stonelw); 
  
  //right channel filters  
  
  rinput->settype(tinput);
  rinput->setfreq_and_q(finput, qinput);
  rinput->setstages(sinput);
  
  rpre1->settype(tpre1);
  rpre1->setfreq_and_q(fpre1, qpre1);
  rpre1->setstages(spre1);  
  
  rpre2->settype(tpre2);
  rpre2->setfreq_and_q(fpre2, qpre2);
  rpre2->setstages(spre2); 
  
  rpost->settype(tpost);
  rpost->setfreq_and_q(fpost, qpost);
  rpost->setstages(spost); 
  
  rtonehg->settype(ttonehg);
  rtonehg->setfreq_and_q(ftonehg,qtonehg);
  rtonehg->setstages(stonehg);   
  
  rtonemd->settype(ttonemd);
  rtonemd->setfreq_and_q(ftonemd,qtonemd);
  rtonemd->setstages(stonemd);  

  rtonelw->settype(ttonelw);
  rtonelw->setfreq_and_q(ftonelw, qtonelw);
  rtonelw->setstages(stonelw);  
   
};

void StompBox::init_tone ()
{
   float varf;
  switch (Pmode)
    {
    case 0:
    varf = 2533.0f + highb*1733.0f;  //High tone ranges from 800 to 6000Hz
    rtonehg->setfreq(varf); 
    ltonehg->setfreq(varf); 
   if (highb > 0.0f) highb = ((float) Phigh)/8.0f;   
    break;
    
    case 1:
    varf = 3333.0f + highb*2500.0f;  //High tone ranges from 833 to 8333Hz
    rtonehg->setfreq(varf);
    ltonehg->setfreq(varf); 
    
    if (highb > 0.0f) highb = ((float) Phigh)/16.0f;    
    if (lowb > 0.0f) lowb = ((float) Plow)/18.0f;          
    break;
    
    
    
    }

};


void
StompBox::setvolume (int value)
{
  Pvolume = value;
  volume = (float)Pvolume / 127.0f;
};

void
StompBox::setpreset (int npreset)
{
  const int PRESET_SIZE = 6;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Odie
    {48, 32, 0, 32, 65, 0}
    //Grunger
    {48, 10, -6, 55, 70, 1}  
    //Hard Dist.
    {48, -42, -6, 38, 5, 1}      
    
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();

};


void
StompBox::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
     case 1:
      Phigh = value;
      if( value < 0) highb = ((float) value)/64.0f;
      if( value > 0) highb = ((float) value)/32.0f;  
      break;
     case 2: 
      Pmid = value;
      if( value < 0) midb = ((float) value)/64.0f;
       if( value > 0) midb = ((float) value)/32.0f;     
      break;
    case 3:
      Plow = value;
      if( value < 0) lowb = ((float) value)/64.0f;
      if( value > 0) lowb = ((float) value)/32.0f;
      break;
    case 4:
      Pgain = value;
      gain = dB2rap(44.0f * ((float)value)/127.0f  - 44.0f);
      break;
    case 5:
      Pmode = value;
      init_mode (Pmode);
      break;
 
    };
      init_tone ();     
};

int
StompBox::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Phigh);
      break;
    case 2:
      return (Pmid);
      break;
    case 3:
      return (Plow);
      break;
    case 4:
      return (Pgain);
      break; 
    case 5:
      return (Pmode);
      break; 

    };
  return (0);			//in case of bogus parameter number
};

