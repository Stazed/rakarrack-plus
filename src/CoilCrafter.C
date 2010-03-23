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

  RB1l =  new AnalogFilter(7,2000.0f,0.5f,0);
  RB1r =  new AnalogFilter(7,2000.0f,0.5f,0);
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
harm->harm_out(smpsl,smpsr);

RB1l->filterout(smpsl);
RB1r->filterout(smpsr);
}

RB2l->filterout(smpsl);
RB2r->filterout(smpsr);

//  harm->harm_out(smpsl,smpsr);



float level = dB2rap (60.0f * (float)Pvolume / 127.0f - 40.0f);

  for (i = 0; i < PERIOD; i++)
    {

        smpsl[i]*=level;
        smpsr[i]*=level;
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
CoilCrafter::reinitfilter ()
{
//  RB1l->setmix(1, lpmix1, bpmix1, hpmix1);
//  RB1r->setmix(1, lpmix1, bpmix1, hpmix1);
  
};


void
CoilCrafter::setpreset (int npreset)
{
  const int PRESET_SIZE = 12;
  const int NUM_PRESETS = 8;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Fender Strat(old)
    {102, 20, 6, 75, 2436, 64,  4400, 92, 26000, 20, 1, 44},
    //Fender Strat(new)
    {112, 1, 5, 75, 3498, 85, 4640, 88, 26000, 20, 1, 40},
    //Squire Strat
    {20, 1, 3, 30, 3498, 85,  4640, 88, 26000, 20, 1, 120},
    //Fender Hambucker
    {64, 0, 0, 0, 3000, 71,  3000, 92, 26000, 20, 1, 64},
    //GibsonP90
    {64, 0, 0, 0, 2700, 71, 2700, 92, 26000, 20, 1, 64},
    //Gibson Standard
    {64, 0, 0, 0, 3300, 71,  3300, 92, 26000, 20, 1, 64},
    //Gibson Mini
    {64, 0, 0, 0, 3300, 71, 3300, 92, 26000, 20, 1, 64},
    //Gibson Super L6S
    {64, 0, 0, 0, 2800, 71, 2800, 92, 26000, 20, 1, 64}
    

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
      Plp1 = value;
      lpmix1 = ((float) Plp1)/32.0f;
      //reinitfilter ();     
      break;     
     case 2:
      Pbp1 = value;
      bpmix1 = ((float) Pbp1)/32.0f;
      //reinitfilter ();
      break;     
     case 3:
      Php1 = value;
      hpmix1 = ((float) Php1)/32.0f;
      //reinitfilter (); 
      break;
     case 4: 
      Pfreq1 = value;
      freq1 = (float) value;
      RB1l->setfreq(freq1);
      RB1r->setfreq(freq1);
      break;
     case 5:
      Pq1 = value;
      //q1 = 10.0f/(float)value;
      q1 = powf (30.0f, ((float)value - 64.0f) / 64.0f);
      printf("%f\n",q1);
      RB1l->setq(q1);
      RB1r->setq(q1);
      break;
     case 6: 
      Pfreq2 = value;
      freq2 = (float) value;
      RB2l->setfreq(freq2);
      RB2r->setfreq(freq2);
      break;
     case 7:
      Pq2 = value;
      q2 = powf (30.0f, ((float)value - 64.0f) / 64.0f);
      //      q2 = (float)value/10.0f;
      printf("%f\n",q2);
      RB2l->setq(q2);
      RB2r->setq(q2);
      break;
    case 8:
      setlpf(value);
      break;
    case 9:
      sethpf(value);
      break;
    case 10:
      Pmode = value;
      break;
    case 11:
      Plevel = value;
      float tmp = 30.0f * ((float)value - 64.0f) / 64.0f;
      RB1l->setgain(tmp);
      RB1r->setgain(tmp); 
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
      return (Plp1);
      break;
    case 2:
      return (Pbp1);
      break;
    case 3:
      return (Php1);
      break;
    case 4:
      return (Pfreq1);
      break;
    case 5:
      return (Pq1);
      break;
    case 6:
      return (Pfreq2);
      break;
    case 7:
      return (Pq2);
      break;
    case 8:
      return (lpffreq);
      break;  
    case 9:
      return (hpffreq);
      break;
    case 10:
      return (Pmode);
      break;
    case 11:
      return (Plevel);
      break; 

    };
  return (0);			//in case of bogus parameter number
};

