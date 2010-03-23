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
  lpffreq = 26000;
  hpffreq = 2500;
  
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
 
if(Pmode)
{
RB1l->filterout(smpsl);
RB1r->filterout(smpsr);

for (i=0; i<PERIOD; i++)
{
  smpsl[i]*=att;
  smpsr[i]*=att;
}

}

RB2l->filterout(smpsl);
RB2r->filterout(smpsr);




harm->harm_out(smpsl,smpsr);



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
  outvolume = (float)Pvolume / 127.0f;

};

void
CoilCrafter::setlpf (int value)
{
  lpffreq=value;
  harm->set_freql (0, (float)value);
};

void
CoilCrafter::sethpf (int value)
{
  hpffreq=value;
  harm->set_freqh (0,(float)value);
};


void
CoilCrafter::setpreset (int npreset)
{
  const int PRESET_SIZE = 8;
  const int NUM_PRESETS = 8;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Fender Strat(old)
    {64, 3300, 16,  4400, 42, 26000, 20, 1},
    //Fender Strat(new)
    {64, 3300, 17, 4640, 23, 26000, 20, 1},
    //Squire Strat
    {64, 3300, 15,  4640, 19, 26000, 20, 1},
    //Fender Hambucker
    {64, 3000, 71,  3000, 22, 26000, 20, 0},
    //GibsonP90
    {64, 2700, 71, 2700, 21, 26000, 20, 0},
    //Gibson Standard
    {64, 3300, 71,  3300, 16, 26000, 20, 0},
    //Gibson Mini
    {64, 3300, 71, 3300, 18, 26000, 20, 0},
    //Gibson Super L6S
    {64, 2800, 71, 2800, 18, 26000, 20, 0}
    

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
      Pfreq1 = value;
      freq1 = (float) value;
      RB1l->setfreq(freq1);
      RB1l->reversecoeffs();
      RB1r->setfreq(freq1);
      RB1r->reversecoeffs();
      break;
     case 2:
      Pq1 = value;
      q1 = 10.0f/(float)value;
      RB1l->setq(q1);
      RB1l->reversecoeffs();
      RB1r->setq(q1);
      RB1r->reversecoeffs();
      break;
     case 3: 
      Pfreq2 = value;
      freq2 = (float) value;
      RB2l->setfreq(freq2);
      RB2r->setfreq(freq2);
      break;
     case 4:
      Pq2 = value;
      q2 = (float)value/10.0f;
      RB2l->setq(q2);
      RB2r->setq(q2);
      break;
    case 5:
      setlpf(value);
      break;
    case 6:
      sethpf(value);
      break;
    case 7:
      Pmode = value;
      if(Pmode) att=.02f; else att=2.0f;
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
      return (Pfreq1);
      break;
    case 2:
      return (Pq1);
      break;
    case 3:
      return (Pfreq2);
      break;
    case 4:
      return (Pq2);
      break;
    case 5:
      return (lpffreq);
      break;  
    case 6:
      return (hpffreq);
      break;
    case 7:
      return (Pmode);
      break;
    };
  return (0);			//in case of bogus parameter number
};

