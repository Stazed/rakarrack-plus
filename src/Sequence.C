/*
  Rakarrack Guitar FX
 
  Sequence.C - Simple compressor/Sequence effect with easy interface, minimal controls
  Copyright (C) 2010 Ryan Billing
  Author: Ryan Billing
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License 
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
#include "Sequence.h"

Sequence::Sequence (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  filterl = NULL;
  filterr = NULL;
  
  maxfreq = 5000.0f;
  minfreq = 40.0f;
  frequency = 40.0f;
  fq = 75.0f;
  Ppreset = 0;
  scount = 0;
  tcount = 0;
  filterl = new RBFilter (0, 80.0f, 40.0f, 2);
  filterr = new RBFilter (0, 80.0f, 40.0f, 2);
  setpreset (Ppreset);
  
  filterl->setmix(1, 0.1f, -1.0f, 0.25f);
  filterr->setmix(1, 0.1f, -1.0f, 0.25f);  

  cleanup ();
};

Sequence::~Sequence ()
{
 
};

/*
 * Cleanup the effect
 */
void
Sequence::cleanup ()
{

};




/*
 * Effect output
 */
void
Sequence::out (float * smpsl, float * smpsr)
{
  int i;
  float ldiff, rdiff, lfol, lfor, ftcount;
  float lmod = 0.0f;
  float rmod = 0.0f;
  float ldbl, ldbr;
  int nextcount,dnextcount;

  switch(Pmode)
  {
  case 0:
  
  nextcount = scount + 1;
  if (nextcount > 7 ) nextcount = 0; 
  ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);  
  lfol = fsequence[scount];

  dscount = (scount + Pstdiff) % 8;
  dnextcount = dscount + 1;
  if (dnextcount > 7 ) dnextcount = 0; 
  rdiff = ifperiod * (fsequence[dnextcount] - fsequence[dscount]);  
  lfor = fsequence[dscount];

  for ( i = 0; i < PERIOD; i++)  //Maintain sequenced modulator
  {

  if (++tcount >= intperiod)
  {
  tcount = 0;
  scount++;
  if(scount > 7) scount = 0;  //reset to beginning of sequence buffer

  nextcount = scount + 1;
  if (nextcount > 7 ) nextcount = 0; 
  ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);  
  lfol = fsequence[scount];

  dscount = (scount + Pstdiff) % 8;
  dnextcount = dscount + 1;
  if (dnextcount > 7 ) dnextcount = 0; 
  rdiff = ifperiod * (fsequence[dnextcount] - fsequence[dscount]);  
  lfor = fsequence[dscount];
  }
  
  ftcount = (float) tcount;


  lmod = lfol + ldiff * ftcount;
  rmod = lfor + rdiff * ftcount;  

  if (Pamplitude)
  {
  ldbl = lmod * (1.0f - cosf(D_PI*ifperiod*ftcount)); 
  ldbr = rmod * (1.0f - cosf(D_PI*ifperiod*ftcount)); 

  efxoutl[i] = ldbl * smpsl[i];
  efxoutr[i] = ldbr * smpsr[i];
  }
  
  float frl = MINFREQ + MAXFREQ*lmod;
  float frr = MINFREQ + MAXFREQ*rmod;

  if ( i % 8 == 0)
  {
  filterl->setfreq_and_q (frl, fq);
  filterr->setfreq_and_q (frr, fq);
  }
  
  efxoutl[i] = filterl->filterout_s(efxoutl[i]);
  efxoutr[i] = filterr->filterout_s (efxoutr[i]);  

  }  
   break;
  
  case 1:
   
    
  for ( i = 0; i < PERIOD; i++)  //Maintain sequenced modulator
  {

  if (++tcount >= intperiod)
  {
  tcount = 0;
  scount++;
  if(scount > 7) scount = 0;  //reset to beginning of sequence buffer
  dscount = (scount + Pstdiff) % 8;
  }
  
  ftcount = M_PI * ifperiod * (float)(tcount);

  lmod = sinf(ftcount)*fsequence[scount];
  rmod = sinf(ftcount)*fsequence[dscount];
   
  if (Pamplitude)
  {
  ldbl = lmod * (1.0f - cosf(2.0f*ftcount)); 
  ldbr = rmod * (1.0f - cosf(2.0f*ftcount)); 

  efxoutl[i] = ldbl * smpsl[i];
  efxoutr[i] = ldbr * smpsr[i];
  }
  
  float frl = MINFREQ + MAXFREQ*lmod;
  float frr = MINFREQ + MAXFREQ*rmod;


  if ( i % 8 == 0)
  {
  filterl->setfreq_and_q (frl, fq);
  filterr->setfreq_and_q (frr, fq);
  }
  
  efxoutl[i] = filterl->filterout_s(efxoutl[i]);
  efxoutr[i] = filterr->filterout_s (efxoutr[i]);  
   
 } 
 
  break;
   
   }  

};


/*
 * Parameter control
 */


void
Sequence::setpreset (int npreset)
{
  const int PRESET_SIZE = 14;
  const int NUM_PRESETS = 5;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Jumpy
    {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0},
    //Stair Step
    {10, 20, 30, 50, 75, 90, 100, 127, 0, 90, 40, 0, 0, 0},
    //Mild
    {20, 30, 10, 40, 25, 60, 100, 50, 0, 90, 40, 0, 0, 0},
    //WahWah
    {0, 127, 0, 127, 0, 127, 0, 127, 0, 220, 96, 0, 3, 0},
    //Filter Pan
    {28, 59, 94, 127, 120, 80, 50, 24, 64, 180, 107, 0, 3, 0}
 
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Sequence::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      Psequence[npar] = value;
      fsequence[npar] = (float) value/127.0f;
      break;
    case 8:
      Pvolume = value;
      outvolume = (float)Pvolume / 127.0f;
      volume = outvolume;
      break;
    case 9:
      Ptempo = value;
      fperiod = fSAMPLE_RATE * 60.0f/((float) Ptempo);  //number of samples before next value
      ifperiod = 1.0f/fperiod;
      intperiod = (int) fperiod;
      break;
    case 10:
      Pq = value;
      fq = powf (60.0f, ((float)value - 64.0f) / 64.0f);
      break;
    case 11:
      Pamplitude = value;
      break;
    case 12:  
      Pstdiff = value;
      break;    
    case 13:
      Pmode = value;
      break;

    };
};

int
Sequence::getpar (int npar)
{
  switch (npar)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      return (Psequence[npar]);
      break;
    case 8:
      return (Pvolume); 
      break;
    case 9:
      return (Ptempo);
      break;
    case 10:
      return (Pq);
      break;
    case 11:
      return (Pamplitude);
      break;
    case 12:
      return (Pstdiff);
      break;
    case 13:
      return (Pmode); 
    };
  return (0);			//in case of bogus parameter number
};
