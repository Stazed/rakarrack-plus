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
#include <time.h>

Sequence::Sequence (float * efxoutl_, float * efxoutr_, long int Quality)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;
  hq = Quality;
  outi = (float *) malloc (sizeof (float) * PERIOD);
  outo = (float *) malloc (sizeof (float) * PERIOD);

  filterl = NULL;
  filterr = NULL;
  
  MAXFREQ = 10000.0f;
  MINFREQ = 100.0f;
  fq = 75.0f;
  Ppreset = 0;
  scount = 0;
  tcount = 0;
  rndflag = 0;
  filterl = new RBFilter (0, 80.0f, 40.0f, 2);
  filterr = new RBFilter (0, 80.0f, 40.0f, 2);
  modfilterl = new RBFilter (0, 25.0f, 0.15f, 2);
  modfilterr = new RBFilter (0, 25.0f, 0.15f, 2);  
  setpreset (Ppreset);
  
  filterl->setmix(1, 0.33f, -1.0f, 0.25f);
  filterr->setmix(1, 0.33f, -1.0f, 0.25f);  

  PS = new PitchShifter (2048, hq, fSAMPLE_RATE);
  PS->ratio = 1.0f;


  cleanup ();
};

Sequence::~Sequence ()
{
  delete[]outi;
  delete[]outo;
  delete PS;
 
};

/*
 * Cleanup the effect
 */
void
Sequence::cleanup ()
{

  memset(outi, 0, sizeof(float)*PERIOD);
  memset(outo, 0, sizeof(float)*PERIOD);

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
  
  if ((rndflag) && (tcount < PERIOD + 1))//This is an Easter Egg
  {
   srand(time(NULL));
   for (i = 0; i<8; i++)  
   {
   fsequence[i] = RND1;
   }
  }  

  switch(Pmode)
  {
  case 0:	//Lineal
  
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
  
  case 1:		//Up Down
   
    
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
  
  efxoutl[i] = filterl->filterout_s (efxoutl[i]);
  efxoutr[i] = filterr->filterout_s (efxoutr[i]);  
   
 } 
 
  break;

 case 2:  //Stepper
      
  for ( i = 0; i < PERIOD; i++)  //Maintain sequenced modulator
  {

  if (++tcount >= intperiod)
  {
  tcount = 0;
  scount++;
  if(scount > 7) scount = 0;  //reset to beginning of sequence buffer
  dscount = (scount + Pstdiff) % 8;
  }
 
  lmod = fsequence[scount];
  rmod = fsequence[dscount];
  
  lmod = modfilterl->filterout_s(lmod);
  rmod = modfilterr->filterout_s(rmod);   
  
  if (Pamplitude)
  {
  ldbl = seqpower * lmod; 
  ldbr = seqpower * rmod; 

  efxoutl[i] = ldbl * smpsl[i];
  efxoutr[i] = ldbr * smpsr[i];
  }
  
  float frl = MINFREQ + lmod * MAXFREQ;
  float frr = MINFREQ + rmod * MAXFREQ;


  if ( i % 8 == 0)
  {
  filterl->setfreq_and_q (frl, fq);
  filterr->setfreq_and_q (frr, fq);
  }
  
  efxoutl[i] = filterl->filterout_s (efxoutl[i]);
  efxoutr[i] = filterr->filterout_s (efxoutr[i]);  
   
 } 
 
  break;   
  
  case 3:  //Shifter
  
  nextcount = scount + 1;
  if (nextcount > 7 ) nextcount = 0; 
  ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);  
  lfol = fsequence[scount];

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
  }
  
  ftcount = (float) tcount;

  lmod = 1.0f + lfol + ldiff * ftcount;

  if (Pamplitude) lmod = 1.0f - (lfol + ldiff * ftcount) * .5f;
  
  outi[i] = (smpsl[i] + smpsr[i])*.5;
     if (outi[i] > 1.0)
	outi[i] = 1.0f;
      if (outi[i] < -1.0)
	outi[i] = -1.0f;
  }


   PS->ratio = lmod;
   PS->smbPitchShift (PS->ratio, PERIOD, 2048, hq, fSAMPLE_RATE, outi, outo);


   memcpy(efxoutl, outo, sizeof(float)*PERIOD);
   memcpy(efxoutr, outo, sizeof(float)*PERIOD);

 
   break;
  
   case 4:      //Tremor
  
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
//Process Amplitude modulation
   if (Pamplitude)
  {
  ftcount = (float) tcount;
  lmod = lfol + ldiff * ftcount;
  rmod = lfor + rdiff * ftcount;  

  ldbl = seqpower * lmod * (1.0f - cosf(D_PI*ifperiod*ftcount)); 
  ldbr = seqpower * rmod * (1.0f - cosf(D_PI*ifperiod*ftcount)); 

  efxoutl[i] = ldbl * smpsl[i];
  efxoutr[i] = ldbr * smpsr[i];
  }
  else
  {
  lmod = seqpower * fsequence[scount];
  rmod = seqpower * fsequence[dscount];
  lmod = modfilterl->filterout_s(lmod);
  rmod = modfilterr->filterout_s(rmod);   

  efxoutl[i] = lmod * smpsl[i];
  efxoutr[i] = rmod * smpsr[i];
  }
 
 
  }; 
   break;

  case 5:  //Arpegiator
  
  lfol = floorf(fsequence[scount]*12.0f);

  for ( i = 0; i < PERIOD; i++)  //Maintain sequenced modulator
  {

  if (++tcount >= intperiod)
  {
  tcount = 0;
  scount++;
  if(scount > 7) scount = 0;  //reset to beginning of sequence buffer
  lfol = floorf(fsequence[scount]*12.0f);
  }
  
  lmod = powf (2.0f, lfol / 12.0f);

  if (Pamplitude) lmod = powf (2.0f, -lfol / 12.0f);
  
  outi[i] = (smpsl[i] + smpsr[i])*.5;
     if (outi[i] > 1.0)
	outi[i] = 1.0f;
      if (outi[i] < -1.0)
	outi[i] = -1.0f;
  }


   PS->ratio = lmod;
   PS->smbPitchShift (PS->ratio, PERIOD, 2048, hq, fSAMPLE_RATE, outi, outo);


   memcpy(efxoutl, outo, sizeof(float)*PERIOD);
   memcpy(efxoutr, outo, sizeof(float)*PERIOD);

 
   break;

  case 6:  //Chorus
  
  nextcount = scount + 1;
  if (nextcount > 7 ) nextcount = 0; 
  ldiff = ifperiod * (fsequence[nextcount] - fsequence[scount]);  
  lfol = fsequence[scount];

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
  }
  
  ftcount = (float) tcount;
 
  lmod = 1.0f + (lfol + ldiff * ftcount)*.03f;
  if (Pamplitude) lmod = 1.0f - (lfol + ldiff * ftcount)*.03f;
 
  outi[i] = (smpsl[i] + smpsr[i])*.5;
     if (outi[i] > 1.0)
	outi[i] = 1.0f;
      if (outi[i] < -1.0)
	outi[i] = -1.0f;
  }


   PS->ratio = lmod;
   PS->smbPitchShift (PS->ratio, PERIOD, 2048, hq, fSAMPLE_RATE, outi, outo);


   memcpy(efxoutl, outo, sizeof(float)*PERIOD);
   memcpy(efxoutr, outo, sizeof(float)*PERIOD);

 
   break;
 
  // here case 6:
  //
  // break;




   }  

};


/*
 * Parameter control
 */


void
Sequence::setranges(int value)
{



  switch(value)
  {
  
     case 1:              //typical for wahwah pedal
       MINFREQ = 450.0f;
       MAXFREQ = 2500.0f;
       break;
     case 2:
       MINFREQ = 150.0f;
       MAXFREQ = 4000.0f;
       break;     
     case 3:
       MINFREQ = 120.0f;
       MAXFREQ = 8000.0f; 
       break;
     case 4:
       MINFREQ = 100.0f;
       MAXFREQ = 12000.0f; 
       break;
     case 5:
       MINFREQ = 80.0f;
       MAXFREQ = 16000.0f; 
       break;
     case 6:
       MINFREQ = 60.0f;
       MAXFREQ = 18000.0f; 
       break;
     case 7:
       MINFREQ = 40.0f;
       MAXFREQ = 22000.0f; 
       break;
     case 8:
       MINFREQ = 20.0f;
       MAXFREQ = 26000.0f; 
       break;


   }
}   


void
Sequence::setpreset (int npreset)
{
  const int PRESET_SIZE = 15;
  const int NUM_PRESETS = 10;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Jumpy
    {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3},
    //Stair Step
    {10, 20, 30, 50, 75, 90, 100, 127, 64, 90, 96, 0, 0, 2, 5},
    //Mild
    {20, 30, 10, 40, 25, 60, 100, 50, 0, 90, 40, 0, 0, 0, 4},
    //WahWah
    {11, 55, 15, 95, 12, 76, 11, 36, 30, 80, 110, 0, 4, 1, 2},
    //Filter Pan
    {28, 59, 94, 127, 120, 80, 50, 24, 64, 180, 107, 0, 3, 0, 8},
    //Stepper
    {30, 127, 30, 50, 80, 40, 110, 80, 0, 240, 95, 1, 1, 2, 2},
    //Shifter
    {0, 0, 127, 127, 0, 0, 127, 127, 64, 114, 64, 1, 0, 3, 0}, 
    //Tremor
     {30, 127, 30, 50, 80, 40, 110, 80, 0, 240, 95, 1, 1, 4, 2},
    //Boogie 
     {0, 40, 50, 60, 70, 60, 40, 0, 0, 220, 64, 0, 0, 5, 0},
    //Chorus
     {64, 30, 45, 20, 60, 25, 42, 15, 64, 120, 64, 0, 0, 6, 0}   

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
  int testegg, i;
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

  seqpower = 0.0f;  
  for (i = 0; i<8; i++)  seqpower += fsequence[i];
  if(seqpower > 0.1f) 
  {
  seqpower = 15.0f/seqpower;
  rndflag = 0;
  } 
  
  testegg = 0;
  for (i = 0; i<8; i++)  testegg += Psequence[i];
  if(testegg < 4)
  {
  seqpower = 5.0f;  //Easter egg
  rndflag = 1;
  }
      break;
    case 8:
      Pvolume = value;
      outvolume = (float)Pvolume / 127.0f;
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
    case 14:
      Prange = value;
      setranges(Prange);
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
      break;
    case 14:
      return (Prange);  
      break;
    };
  return (0);			//in case of bogus parameter number
};
