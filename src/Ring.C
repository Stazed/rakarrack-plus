/*
  Rakarrack Audio FX
  Ring DSP Code based on "(author)" LADSPA plugin(swh-plugins).
  ZynAddSubFX effect structure - Copyright (C) 2002-2005 Nasca Octavian Paul
  Modified and adapted for rakarrack by Josep Andreu

  Ring.C - Ring Modulator effect
  
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
#include "Ring.h"




Ring::Ring (REALTYPE * efxoutl_, REALTYPE * efxoutr_)
{

  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  sin_tbl = (float *) malloc(sizeof(float) * SAMPLE_RATE);
  tri_tbl = (float *) malloc(sizeof(float) * SAMPLE_RATE);
  squ_tbl = (float *) malloc(sizeof(float) * SAMPLE_RATE);
  saw_tbl = (float *) malloc(sizeof(float) * SAMPLE_RATE);

  Create_Tables();

  offset = 0;


  //default values
  Ppreset = 0;
  Pvolume = 50;
  Plrcross = 40;
  Plevel = 64;
  Pstereo = 0;

  scale = 1.0f;
  sin = 0.0f;
  tri = 0.0f;
  saw = 0.0f;
  squ = 0.0f;

  setpreset (Ppreset);
  cleanup ();
};

Ring::~Ring ()
{
 free(sin_tbl);
 free(tri_tbl);
 free(squ_tbl);
 free(saw_tbl);

};

/*
* Create Tables
*/
void
Ring::Create_Tables()
{
unsigned int i;
float SR = (float) SAMPLE_RATE;  
  

  for (i=0;i<SAMPLE_RATE;i++) sin_tbl[i]=sinf((float)i*D_PI/SR);
  for (i=0;i<SAMPLE_RATE;i++) tri_tbl[i]=acosf(cosf((float)i*D_PI/SR))/D_PI-1.0f;
  for (i=0;i<SAMPLE_RATE;i++) squ_tbl[i]=(i < SAMPLE_RATE/2) ? 1.0f : -1.0f;
  for (i=0;i<SAMPLE_RATE;i++) saw_tbl[i]=((2.0f*i)-SR)/SR;

}



/*
 * Cleanup the effect
 */
void
Ring::cleanup ()
{


};


/*
 * Apply the filters
 */


/*
 * Effect output
 */
void
Ring::out (REALTYPE * smpsl, REALTYPE * smpsr)
{
  int i;
  REALTYPE l, r, lout, rout, tmpfactor;

  REALTYPE inputvol = powf (5.0f, ((float)Pinput - 32.0f) / 127.0f);

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
	  efxoutl[i] =   (smpsl[i]  +  smpsr[i] ) * inputvol;
	};
    };



  for (i=0;i < PERIOD; i++)
    {
     tmpfactor = ( depth * scale * (( sin + idepth) * sin_tbl[offset] + (tri + idepth) * tri_tbl[offset] + (saw  + idepth)* saw_tbl[offset] + (squ + idepth) * squ_tbl[offset]) );
     efxoutl[i] *= tmpfactor;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
  if (Pstereo != 0)
     {
      efxoutr[i] *= tmpfactor;
     }

      offset += Pfreq;
      if (offset > SAMPLE_RATE) offset -=SAMPLE_RATE;
  }


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
       
      efxoutl[i] = dry * smpsl[i] + wet * lout * 2.0f * level * panning;
      efxoutr[i] = dry * smpsr[i] + wet * rout * 2.0f * level * (1.0f -panning);  

    };
    
};


/*
 * Parameter control
 */


void
Ring::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning) / 127.0f  + 0.5f;
};


void
Ring::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 64.0f;
};


void
Ring::setscale()
{
  scale = sin + tri + saw + squ;
  if (scale==0.0) scale = 1.0;
  scale = 1.0 / scale;
}

void
Ring::setpreset (int npreset)
{
  const int PRESET_SIZE = 12;
  const int NUM_PRESETS = 6;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //A 440
    {0, 0, 0, 64, 100, 440, 0, 20, 0, 100, 0, 64},
    //E string
    {0, 0, 0, 64, 100, 82, 0, 100, 0, 0, 0, 64},
    //A string
    {0, 0, 0, 64, 100, 110, 0, 0, 100, 50, 0, 64},
    //dissonance
    {0, 0, 0, 64, 100, 817, 0, 20, 0, 100, 0, 64},
    //Fast Beat
    {0, 0, 0, 64, 100, 15, 0, 20, 0, 100, 0, 64},
    //Ring Amp
    {0, 0, 0, 64, 100, 1, 0, 20, 0, 100, 0, 64},
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();
};


void
Ring::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
    outvolume = 0.5f + (float)value/64.0f;
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
      setlrcross (value);
      break;
    case 3:
      Plevel = value;
      break;
    case 4:
      Pdepthp = value;
      depth = (float) Pdepthp / 100.0;
      idepth = 1.0f - depth;
      break;
    case 5:
      if(value > 20000)		//Make sure bad inputs can't cause buffer overflow
      {
      Pfreq = 20000;
      }
      else if (value < 1)
      {
      Pfreq = 1;
      }
      else
      {
      Pfreq = value;
      }
      break;
    case 6:
      if (value > 1)
	value = 1;
      Pstereo = value;
      break;
    case 7:
      Psin = value;
      sin = (float) Psin / 100.0;
      setscale();
      break;
    case 8:
      Ptri = value;
      tri = (float) Ptri / 100.0;
      setscale();
      break;       
    case 9:
      Psaw = value;
      saw = (float) Psaw / 100.0;
      setscale();
      break;
    case 10:
      Psqu = value;
      squ = (float) Psqu / 100.0;
      setscale();
      break;       
    case 11:
      Pinput = value;
      break;
      
    };
};

int
Ring::getpar (int npar)
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
      return (Plevel);
      break;
    case 4:
      return (Pdepthp);
      break;
    case 5:
      return (Pfreq);
      break;
    case 6:
      return (Pstereo);
      break;
    case 7:
      return (Psin);
      break;
    case 8:
      return (Ptri);
      break;  
    case 9:
      return (Psaw);
      break;
    case 10:
      return (Psqu);
      break;  
    case 11:
      return (Pinput);
      break;


    };
  return (0);			//in case of bogus parameter number
};

