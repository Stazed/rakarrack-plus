/*
  ZynAddSubFX - a software synthesizer
 
  Distorsion.C - Distorsion effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

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
#include "Distorsion.h"


/*
 * Waveshape (this is called by OscilGen::waveshape and Distorsion::process)
 */

void
waveshapesmps (int n, REALTYPE * smps, int type,
	       int drive, int eff)
{
  int i;
  REALTYPE ws = (float)drive / 127.0f + .00001f;
  ws = 1.0f - expf (-ws * 4.0f);
  REALTYPE tmpv;

  switch (type + 1 )
    {
    case 1:
      ws = powf (10.0f, ws * ws * 3.0f) - 1.0f + 0.001f;	//Arctangent
      for (i = 0; i < n; i++)
	smps[i] = atanf (smps[i] * ws) / atanf (ws);
      break;
    case 2:
      ws = ws * ws * 32.0f + 0.0001f;	//Asymmetric
      if (ws < 1.0)
	tmpv = sinf (ws) + 0.1f;
      else
	tmpv = 1.1f;
      for (i = 0; i < n; i++)
	{
	  smps[i] = sinf (smps[i] * (0.1f + ws - ws * smps[i])) / tmpv;
	};
      break;
    case 3:
      ws = ws * ws * ws * 20.0f + 0.0001f;	//Pow
      for (i = 0; i < n; i++)
	{
	  smps[i] *= ws;
	  if (fabsf (smps[i]) < 1.0)
	    {
	      smps[i] = (smps[i] - powf (smps[i], 3.0f)) * 3.0f;
	      if (ws < 1.0)
		smps[i] /= ws;
	    }
	  else
	    smps[i] = 0.0;
	};
      break;
    case 4:
      ws = ws * ws * ws * 32.0f + 0.0001f;	//Sine
      if (ws < 1.57f)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	smps[i] = sinf (smps[i] * ws) / tmpv;
      break;
    case 5:
      ws = ws * ws + 0.000001f;	//Quantisize
      for (i = 0; i < n; i++)
	smps[i] = floorf (smps[i] / ws + 0.15f) * ws;
      break;
    case 6:
      ws = ws * ws * ws * 32.0f + 0.0001f;	//Zigzag
      if (ws < 1.0)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	smps[i] = asinf (sinf (smps[i] * ws)) / tmpv;
      break;
    case 7:
      ws = powf (2.0f, -ws * ws * 8.0f);	//Limiter
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i];
	  if (fabsf (tmp) > ws)
	    {
	      if (tmp >= 0.0)
		smps[i] = 1.0f;
	      else
		smps[i] = -1.0f;
	    }
	  else
	    smps[i] /= ws;
	};
      break;
    case 8:
      ws = powf (2.0f, -ws * ws * 8.0f);	//Upper Limiter
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i];
	  if (tmp > ws)
	    smps[i] = ws;
	  smps[i] *= 2.0f;
	};
      break;
    case 9:
      ws = powf (2.0f, -ws * ws * 8.0f);	//Lower Limiter
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i];
	  if (tmp < -ws)
	    smps[i] = -ws;
	  smps[i] *= 2.0f;
	};
      break;
    case 10:
      ws = (powf (2.0f, ws * 6.0f) - 1.0f) / powf (2.0f, 6.0f);	//Inverse Limiter
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i];
	  if (fabsf (tmp) > ws)
	    {
	      if (tmp >= 0.0)
		smps[i] = tmp - ws;
	      else
		smps[i] = tmp + ws;
	    }
	  else
	    smps[i] = 0;
	};
      break;
    case 11:
      ws = powf (5.0f, ws * ws * 1.0f) - 1.0f;	//Clip
      for (i = 0; i < n; i++)
	smps[i] =
	  smps[i] * (ws + 0.5f) * 0.9999f - floorf (0.5f +
						  smps[i] * (ws +
							     0.5f) * 0.9999f);
      break;
    case 12:
      ws = ws * ws * ws * 30.0f + 0.001f;	//Asym2
      if (ws < 0.3)
	tmpv = ws;
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i] * ws;
	  if ((tmp > -2.0) && (tmp < 1.0))
	    smps[i] = tmp * (1.0f - tmp) * (tmp + 2.0f) / tmpv;
	  else
	    smps[i] = 0.0f;
	};
      break;
    case 13:
      ws = ws * ws * ws * 32.0f + 0.0001f;	//Pow2
      if (ws < 1.0)
	tmpv = ws * (1.0f + ws) / 2.0f;
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i] * ws;
	  if ((tmp > -1.0f) && (tmp < 1.618034f))
	    smps[i] = tmp * (1.0f - tmp) / tmpv;
	  else if (tmp > 0.0)
	    smps[i] = -1.0f;
	  else
	    smps[i] = -2.0f;
	};
      break;
    case 14:
      ws = powf (ws, 5.0f) * 80.0f + 0.0001f;	//sigmoid
      if (ws > 10.0)
	tmpv = 0.5f;
      else
	tmpv = 0.5f - 1.0f / (expf (ws) + 1.0f);
      for (i = 0; i < n; i++)
	{
	  REALTYPE tmp = smps[i] * ws;
	  if (tmp < -10.0)
	    tmp = -10.0f;
	  else if (tmp > 10.0)
	    tmp = 10.0f;
	  tmp = 0.5f - 1.0f / (expf (tmp) + 1.0f);
	  smps[i] = tmp / tmpv;
	};
      break;
      case 15:		//Sqrt "Crunch" -- Asymmetric square root distortion.
      ws = ws*ws*CRUNCH_GAIN + 1.0f;

      for (i = 0; i < n; i++)
	{
	REALTYPE tmp = smps[i] * ws;	
	if (tmp < Tlo) {  
	smps[i] = Tlc - sqrtf(-tmp*DIV_TLC_CONST);

	}
	else if (tmp > Thi) {
	smps[i] = Thc + sqrtf(tmp*DIV_THC_CONST);	
	}
	else { 
 	smps[i] = tmp;
	};
	  
	  if (smps[i] < -1.0)
	    smps[i] = -1.0f;
	  else if (smps[i] > 1.0)
	    smps[i] = 1.0f;

	};
      break;
      case 16:		//Sqrt "Crunch2" -- Asymmetric square root distortion.
      ws = ws*ws*CRUNCH_GAIN + 1.0f;

      for (i = 0; i < n; i++)
	{
	REALTYPE tmp = smps[i] * ws;	
	if (tmp < Tlo) {  
	smps[i] = Tlc;

	}
	else if (tmp > Thi) {
	smps[i] = Thc + sqrtf(tmp*DIV_THC_CONST);	
	}
	else { 
 	smps[i] = tmp;
	};
	  
	  if (smps[i] < -1.0)
	    smps[i] = -1.0f;
	  else if (smps[i] > 1.0)
	    smps[i] = 1.0f;
	};
      break;
      
      case 17:		//Octave Up
      ws = ws*ws*30.0f + 1.0f;

      for (i = 0; i < n; i++)
	{
	REALTYPE tmp = fabs(smps[i])* ws;	
	if (tmp > 1.0f) {  
	tmp = 1.0f;
	}

	smps[i] = tmp;		//a little bit of DC correction

	};
      break;
       case 18:
      ws = ws*D_PI+.00001;
      if (ws < 1.57f)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
        smps[i]=sinf(ws * smps[i] + sinf( ws * 2.0f*smps[i]))/ tmpv;  

      break;                                                               

       case 19:
      ws =  ws * D_PI + 0.0001f;
       if (ws < 1.57f)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
       for (i = 0; i < n; i++)
       smps[i]=sinf(ws * smps[i] + sinf(ws * smps[i])/tmpv);  
      break;                                                               
        
        

 
      //update to Distorsion::changepar (Ptype max) if there is added more waveshapings functions
    };

};


Distorsion::Distorsion (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  octoutl = (float *) malloc (sizeof (float) * PERIOD);
  octoutr = (float *) malloc (sizeof (float) * PERIOD);

  lpfl = new AnalogFilter (2, 22000, 1, 0);
  lpfr = new AnalogFilter (2, 22000, 1, 0);
  hpfl = new AnalogFilter (3, 20, 1, 0);
  hpfr = new AnalogFilter (3, 20, 1, 0);
  blockDCl = new AnalogFilter (2, 440.0f, 1, 0);
  blockDCr = new AnalogFilter (2, 440.0f, 1, 0);
  blockDCl->setfreq (75.0f);
  blockDCr->setfreq (75.0f);
  DCl = new AnalogFilter (3, 30, 1, 0);
  DCr = new AnalogFilter (3, 30, 1, 0);
  DCl->setfreq (30.0f);
  DCr->setfreq (30.0f);


  //default values
  Ppreset = 0;
  Pvolume = 50;
  Plrcross = 40;
  Pdrive = 90;
  Plevel = 64;
  Ptype = 0;
  Pnegate = 0;
  Plpf = 127;
  Phpf = 0;
  Pstereo = 0;
  Pprefiltering = 0;
  Poctave = 0;
  togglel = 1.0;
  octave_memoryl = -1.0;
  toggler = 1.0;
  octave_memoryr = -1.0;
  octmix = 0.0;

  setpreset (Ppreset);
  cleanup ();
};

Distorsion::~Distorsion ()
{
  delete (lpfl);
  delete (lpfr);
  delete (hpfl);
  delete (hpfr);
  delete (blockDCl);
  delete (blockDCr);
  delete (DCl);
  delete (DCr);
};

/*
 * Cleanup the effect
 */
void
Distorsion::cleanup ()
{
  lpfl->cleanup ();
  hpfl->cleanup ();
  lpfr->cleanup ();
  hpfr->cleanup ();
  blockDCr->cleanup ();
  blockDCl->cleanup ();
  DCl->cleanup();
  DCr->cleanup();

};


/*
 * Apply the filters
 */

void
Distorsion::applyfilters (REALTYPE * efxoutl, REALTYPE * efxoutr)
{
  lpfl->filterout (efxoutl);
  hpfl->filterout (efxoutl);

  if (Pstereo != 0)
    {				//stereo
      lpfr->filterout (efxoutr);
      hpfr->filterout (efxoutr);
    };

};


/*
 * Effect output
 */
void
Distorsion::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;
  REALTYPE l, r, lout, rout;

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

  //no optimised, yet (no look table)


  waveshapesmps (PERIOD, efxoutl, Ptype, Pdrive, 1);
  if (Pstereo != 0)
  waveshapesmps (PERIOD, efxoutr, Ptype, Pdrive, 1);

  if (Pprefiltering == 0)
    applyfilters (efxoutl, efxoutr);

  if (Pstereo == 0) memcpy (efxoutr , efxoutl, PERIOD * sizeof(float));

  if (octmix > 0.01f)
  {
   for (i = 0; i < PERIOD; i++)
    {
      lout = efxoutl[i];
      rout = efxoutr[i];
      
	
	if ( (octave_memoryl < 0.0f) && (lout > 0.0f) ) togglel *= -1.0f;
	
	octave_memoryl = lout;    

	if ( (octave_memoryr < 0.0f) && (rout > 0.0f) ) toggler *= -1.0f;
	
	octave_memoryr = rout;    

        octoutl[i] = lout *  togglel;
        octoutr[i] = rout *  toggler;
      }
      
       blockDCr->filterout (octoutr);
       blockDCl->filterout (octoutl);
   }



  REALTYPE level = dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);

  for (i = 0; i < PERIOD; i++)
    {
      lout = efxoutl[i];
      rout = efxoutr[i];
        
      
      l = lout * (1.0f - lrcross) + rout * lrcross;
      r = rout * (1.0f - lrcross) + lout * lrcross;

        if (octmix > 0.01f)
       {
        lout = l * (1.0f - octmix) + octoutl[i] * octmix;
        rout = r * (1.0f - octmix) + octoutr[i] * octmix;
       }
        else
       {
        lout = l;
        rout = r;  
       }
       
      efxoutl[i] = lout * 2.0f * level * panning;
      efxoutr[i] = rout * 2.0f * level * (1.0f -panning);  

    };
    
    DCr->filterout (efxoutr);
    DCl->filterout (efxoutl);
};


/*
 * Parameter control
 */
void
Distorsion::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;

  outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Distorsion::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};


void
Distorsion::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 127.0f * 1.0f;
};

void
Distorsion::setlpf (int value)
{
  Plpf = value;
  REALTYPE fr = (float)Plpf;
  lpfl->setfreq (fr);
  lpfr->setfreq (fr);
};

void
Distorsion::sethpf (int value)
{
  Phpf = value;
  REALTYPE fr = (float)Phpf;

  hpfl->setfreq (fr);
  hpfr->setfreq (fr);
  //Prefiltering of 51 is approx 630 Hz. 50 - 60 generally good for OD pedal.
};

void
Distorsion::setoctave (int Poctave)
{
  this->Poctave = Poctave;
  octmix = (REALTYPE) (Poctave) / 127.0f;
};

void
Distorsion::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 6;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Overdrive 1
    {84, 64, 35, 56, 40, 0, 0, 6703, 21, 0, 0},
    //Overdrive 2
    {85, 64, 35, 29, 45, 1, 0, 25040, 21, 0, 0},
    //Distorsion 1
    {0, 64, 0, 87, 14, 6, 0, 3134, 157, 0, 1},
    //Distorsion 2
    {0, 64, 127, 87, 14, 0, 1, 3134, 102, 0, 0},
    //Distorsion 3
    {0, 64, 127, 127, 12, 13, 0, 5078, 56, 0, 1},
    //Guitar Amp
    {84, 64, 35, 63, 50, 2, 0, 824, 21, 0, 0}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();
};


void
Distorsion::changepar (int npar, int value)
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
      break;
    case 4:
      Plevel = value;
      break;
    case 5:
      if (value > 18)
	value = 18;		//this must be increased if more distorsion types are added
      Ptype = value;
      break;
    case 6:
      if (value > 1)
	value = 1;
      Pnegate = value;
      break;
    case 7:
      setlpf (value);
      break;
    case 8:
      sethpf (value);
      break;
    case 9:
      if (value > 1)
	value = 1;
      Pstereo = value;
      break;
    case 10:
      Pprefiltering = value;
      break;
    case 11:
      break;  
    case 12:
      setoctave (value);
      break;
    };
};

int
Distorsion::getpar (int npar)
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
      return (Ptype);
      break;
    case 6:
      return (Pnegate);
      break;
    case 7:
      return (Plpf);
      break;
    case 8:
      return (Phpf);
      break;
    case 9:
      return (Pstereo);
      break;
    case 10:
      return (Pprefiltering);
      break;
    case 11:
      return (0);
      break;  
    case 12:
      return (Poctave);
      break;
    };
  return (0);			//in case of bogus parameter number
};

