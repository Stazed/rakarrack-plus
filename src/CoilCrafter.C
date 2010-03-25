/*
  Rakarrack   Audio FX software 
  CoilCrafter.C - Harmonic Enhancer
  Based in Steve Harris LADSPA Plugin harmonic_gen
  Modified for rakarrack by Josep Andreu
  
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
#include "CoilCrafter.h"



CoilCrafter::CoilCrafter (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;


  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ptone = 20;
  att = 30;

  tfreqs[0]=4400.0f;
  tfreqs[1]=4200.0f;
  tfreqs[2]=2900.0f;
  tfreqs[3]=3000.0f;
  tfreqs[4]=2700.0f;
  tfreqs[5]=3300.0f;
  tfreqs[6]=3300.0f;
  tfreqs[7]=2800.0f;
  
  tqs[0]=4.2f;
  tqs[1]=2.3f;
  tqs[2]=1.8f;
  tqs[3]=2.2f;
  tqs[4]=2.1f;
  tqs[5]=1.7f;
  tqs[6]=1.7f;
  tqs[7]=1.8f;
   
  
  
  for(int i=0; i<10; i++)
  {
    rm[i]=0.0f;
  }
  rm[0]=1.0; rm[2]= -1.0; rm[4]=1.0; rm[6]=-1.0; rm[8]=1.0;


  harm = new HarmEnhancer (rm, 2500.0f,26000.0f,1.0f);

  RB1l =  new AnalogFilter(2,2000.0f,1.0f,0);
  RB1r =  new AnalogFilter(2,2000.0f,1.0f,0);
  RB2l =  new AnalogFilter(2,2000.0f,1.0f,0);
  RB2r =  new AnalogFilter(2,2000.0f,1.0f,0);
   

  cleanup ();

  setpreset (Ppreset);
};

CoilCrafter::~CoilCrafter ()
{
  delete (harm);
};

/*
 * Cleanup the effect
 */
void
CoilCrafter::cleanup ()
{
 harm->cleanup ();
 harm->calcula_mag(rm);
 harm->set_vol(1,1.0f);

 RB1l->cleanup();
 RB1r->cleanup();
 RB2l->cleanup();
 RB2r->cleanup();
 
};


/*
 * Effect output
 */
void
CoilCrafter::out (float * smpsl, float * smpsr)
{
 int i;



RB1l->filterout(smpsl);
RB1r->filterout(smpsr);

for (i=0; i<PERIOD; i++)
{
  smpsl[i]*=att;
  smpsr[i]*=att;
}


RB2l->filterout(smpsl);
RB2r->filterout(smpsr);

if(Pmode) harm->harm_out(smpsl,smpsr);


for (i=0; i<PERIOD; i++)
{
  smpsl[i]*=outvolume;
  smpsr[i]*=outvolume;
  
}


};


/*
 * Parameter control
 */
void
CoilCrafter::setvolume (int value)
{
  Pvolume = value;
  outvolume = (1.0f + (float)Pvolume) / 127.0f;

};

void
CoilCrafter::setfreq1()
{
      RB1l->setfreq(freq1);
      RB1l->reversecoeffs();
      RB1r->setfreq(freq1);
      RB1r->reversecoeffs();
}

void
CoilCrafter::setq1()
{
      RB1l->setq(q1);
      RB1l->reversecoeffs();
      RB1r->setq(q1);
      RB1r->reversecoeffs();
}

void
CoilCrafter::setfreq2()
{

      RB2l->setfreq(freq2);
      RB2r->setfreq(freq2);
}

void
CoilCrafter::setq2()
{

     RB2l->setq(q2);
     RB2r->setq(q2);
 


}

void
CoilCrafter::sethpf (int value)
{
  harm->set_freqh (0,(float)Ptone);
};


void
CoilCrafter::setpreset (int npreset)
{
  const int PRESET_SIZE = 9;
  const int NUM_PRESETS = 2;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //H to S
    {64, 5, 0, 3300, 16,  4400, 42, 20, 0},
    //S to H
    {64, 0, 5, 4400, 42, 3300, 16, 20, 0},
   

  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();

};


void
CoilCrafter::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      Ppo = value; 
      freq1 = tfreqs[value];
      Pfreq1 = (int)freq1;
      setfreq1();
      q1 = tqs[value];
      Pq1 = (int)(q1*10.0f);
      setq1();
      break;
    case 2:
      Ppd = value; 
      freq2 = tfreqs[value];
      Pfreq2 = (int)freq2;
      setfreq2();
      q2 = tqs[value];
      Pq2 =(int)(q2*10.0f);
      setq2();
      break;
    case 3: 
      Pfreq1 = value;
      freq1 = (float) value;
      setfreq1();
      break;
    case 4:
      Pq1 = value;
      q1 = (float)value/10.0f;
      setq1();
      break;
    case 5: 
      Pfreq2 = value;
      freq2 = (float) value;
      setfreq2();
      break; 
     case 6:
      Pq2 = value;
      q2 = (float)value/10.0f;
      setq2();
      break;
     case 7:
      Ptone = value;
      sethpf(value);
      break;
     case 8:
      Pmode = value;

      break;
 
    };
};

int
CoilCrafter::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppo);
      break;
    case 2:
      return (Ppd);
      break;
    case 3:
      return (Pfreq1);
      break;
    case 4:
      return (Pq1);
      break;
    case 5:
      return (Pfreq2);
      break;  
    case 6:
      return (Pq2);
      break;
    case 7:
      return (Ptone);
      break;
    case 8:
      return (Pmode);
      break;

    };
  return (0);			//in case of bogus parameter number
};

