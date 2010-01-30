/*
  Rakarrack Audio FX
  Valve DSP Code based Steve Harris valve LADSPA plugin(swh-plugins).
  ZynAddSubFX effect structure - Copyright (C) 2002-2005 Nasca Octavian Paul
  Modified and adapted for rakarrack by Josep Andreu

  Valve.C - Distorsion effect
  
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
#include "Valve.h"




Valve::Valve (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  lpfl = new AnalogFilter (2, 22000, 1, 0);
  lpfr = new AnalogFilter (2, 22000, 1, 0);
  hpfl = new AnalogFilter (3, 20, 1, 0);
  hpfr = new AnalogFilter (3, 20, 1, 0);
  harm = new HarmEnhancer (rm, 2200.0f,4.0f);


  //default values
  Ppreset = 0;
  Pvolume = 50;
  Plrcross = 40;
  Pdrive = 90;
  Plevel = 64;
  Pnegate = 0;
  Plpf = 127;
  Phpf = 0;
  Q_q = 64;
  Pstereo = 0;
  Pprefiltering = 0;
  q = 0.0f;
  dist = 0.0f;
  setlpf(127);
  sethpf(1);

  for(int i=0;i<10;i++) rm[i]=-1.0;
  rm[1]=1.0; rm[6]=1.0; rm[9]=1.0;

  setpreset (Ppreset);
  cleanup ();
};

Valve::~Valve ()
{
  delete (lpfl);
  delete (lpfr);
  delete (hpfl);
  delete (hpfr);
};

/*
 * Cleanup the effect
 */
void
Valve::cleanup ()
{
  lpfl->cleanup ();
  hpfl->cleanup ();
  lpfr->cleanup ();
  hpfr->cleanup ();
  otml = 0.0f;
  itml=0.0f;
  otmr=0.0f;
  itmr=0.0f;




};


/*
 * Apply the filters
 */

void
Valve::applyfilters (REALTYPE * efxoutl, REALTYPE * efxoutr)
{
  lpfl->filterout (efxoutl);
  hpfl->filterout (efxoutl);

  if (Pstereo != 0)
    {				//stereo
      lpfr->filterout (efxoutr);
      hpfr->filterout (efxoutr);
    };

};


float
Valve::Wshape(float x)
{
float fq=fabsf(q);
if(x<fq) return(x);
if(x>fq) return(fq+(x-fq)/powf(1.0+((x-fq)/(1.0-fq)),2.0));
if(x>1.0) return((fq+1.0)*.5);
return(0.0);
}



/*
 * Effect output
 */
void
Valve::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;
  REALTYPE l, r, lout, rout, fx;

  REALTYPE inputvol = powf (5.0f, ((float)Pdrive - 32.0f) / 127.0f);
  if (Pnegate != 0)
    inputvol *= -1.0f;

  if (Pstereo != 0)
    {				//Stereo
      for (i = 0; i < PERIOD; i++)
	{
	  efxoutl[i] = smpsl[i] * inputvol * 2.0f;
	  efxoutr[i] = smpsr[i] * inputvol * 2.0f;
	};
    }
  else
    {
      for (i = 0; i < PERIOD; i++)
	{
	  efxoutl[i] =
	    (smpsl[i]  +  smpsr[i] ) * inputvol;
	};
    };

  if (Pprefiltering != 0)
    applyfilters (efxoutl, efxoutr);



     if (q == 0.0f) 
       {
           for (i =0; i<PERIOD; i++) 
             {
              if (efxoutl[i] == q) fx = 1.0f / dist;
              else fx = efxoutl[i] / (1.0f - powf(2,-dist * efxoutl[i] * LN2R));
              otml = 0.999f * otml + fx - itml;
              itml = fx;
              otml=Wshape(otml);
              efxoutl[i]= otml;
             }
        } 
        else 
        {
           for (i = 0; i < PERIOD; i++) 
             {
               if (efxoutl[i] == q) fx = 1.0f / dist + q / (1.0f - powf(2,dist * q * LN2R));
               else fx = (efxoutl[i] - q) / (1.0f - powf(2,-dist * (efxoutl[i] - q)* LN2R)) + q / (1.0f - powf(2,dist * q * LN2R));
               otml = 0.999f * otml + fx - itml;
               itml = fx;
               otml=Wshape(otml);
               efxoutl[i]= otml;
              }
        }

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
  if (Pstereo != 0)
 {
 
       if (q == 0.0f) 
       {
           for (i =0; i<PERIOD; i++) 
             {
              if (efxoutr[i] == q) fx = 1.0f / dist;
              else fx = efxoutr[i] / (1.0f - powf(2,-dist * efxoutr[i] * LN2R));
              otmr = 0.999f * otmr + fx - itmr;
              itmr = fx;
              otmr=Wshape(otmr);
              efxoutr[i]= otmr;
             }
        } 
        else 
        {
           for (i = 0; i < PERIOD; i++) 
             {
               if (efxoutr[i] == q) fx = 1.0f / dist + q / (1.0f - powf(2,dist * q * LN2R));
               else fx = (efxoutr[i] - q) / (1.0f - powf(2,-dist * (efxoutr[i] - q)* LN2R)) + q / (1.0f - powf(2,dist * q * LN2R));
               otmr = 0.999f * otmr + fx - itmr;
               itmr = fx;
               otmr=Wshape(otmr);
               efxoutr[i]= otmr;
              }
        }

  }

     
  harm->harm_out(efxoutl,efxoutr);
  

  if (Pprefiltering == 0)
    applyfilters (efxoutl, efxoutr);

  if (Pstereo == 0) memcpy (efxoutr , efxoutl, PERIOD * sizeof(float));


  REALTYPE level = dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);

  for (i = 0; i < PERIOD; i++)
    {
      lout = efxoutl[i];
      rout = efxoutr[i];
        
      
      l = lout * (1.0f - lrcross) + rout * lrcross;
      r = rout * (1.0f - lrcross) + lout * lrcross;

      lout = l;
      rout = r;  
       
      efxoutl[i] = lout * 2.0f * level * panning;
      efxoutr[i] = rout * 2.0f * level * (1.0f -panning);  

    };
    
};


/*
 * Parameter control
 */
void
Valve::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;

  outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Valve::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};


void
Valve::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 127.0f * 1.0f;
};

void
Valve::setlpf (int Plpf)
{
  this->Plpf = Plpf;
  REALTYPE fr = expf (powf ((float)Plpf / 127.0f, 0.5f) * logf (25000.0f)) + 40.0f;

  lpfl->setfreq (fr);
  lpfr->setfreq (fr);
};

void
Valve::sethpf (int Phpf)
{
  this->Phpf = Phpf;
  REALTYPE fr = expf (powf ((float)Phpf / 127.0f, 0.5f) * logf (25000.0f)) + 20.0f;

  hpfl->setfreq (fr);
  hpfr->setfreq (fr);
  //Prefiltering of 51 is approx 630 Hz. 50 - 60 generally good for OD pedal.
};

void
Valve::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 3;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Valve 1
    {0, 64, 64, 127, 64, 1, 93, 17, 1, 0, 69},
    //Valve 2
    {0, 64, 64, 127, 64, 0, 90, 17, 1, 0, 112},
    //Valve 3
    {0, 64, 35, 80, 64, 0, 80, 40, 1, 1, 100}

  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();
};


void
Valve::changepar (int npar, int value)
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
      setlrcross (value);
      break;
    case 3:
      Pdrive = value;
      dist = (float) Pdrive / 127.0f * 40.0f + .5f;
      break;
    case 4:
      Plevel = value;
      break;
    case 5:
      if (value > 1)
	value = 1;
      Pnegate = value;
      break;
    case 6:
      setlpf (value);
      break;
    case 7:
      sethpf (value);
      break;
    case 8:
      if (value > 1)
	value = 1;
      Pstereo = value;
      break;
    case 9:
      Pprefiltering = value;
      break;
    case 10:
      Q_q = value;
      q = (float)Q_q /127.0f - .999;
      break;       

    };
};

int
Valve::getpar (int npar)
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
      return (Plrcross);
      break;
    case 3:
      return (Pdrive);
      break;
    case 4:
      return (Plevel);
      break;
    case 5:
      return (Pnegate);
      break;
    case 6:
      return (Plpf);
      break;
    case 7:
      return (Phpf);
      break;
    case 8:
      return (Pstereo);
      break;
    case 9:
      return (Pprefiltering);
      break;
    case 10:
      return (Q_q);
      break;  
    };
  return (0);			//in case of bogus parameter number
};

