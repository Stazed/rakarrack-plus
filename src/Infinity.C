/*
  ZynAddSubFX - a software synthesizer
 
  Rakarrack Guitar Effects Processor
  
  InfiniT : Infinitely rising & falling filter effect
  Author:  Ryan Billing (aka Transmogrifox)
  Copyright (C) 2010

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

#include <math.h>
#include "Infinity.h"
#include <stdio.h>

Infinity::Infinity (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  Ppreset = 0;
  int i;
  for (i = 0; i<NUM_INF_BANDS; i++) {
  filterl[i] = new RBFilter (0, 80.0f, 70.0f, 1);
  filterr[i] = new RBFilter (0, 80.0f, 70.0f, 1);
  bandstate[i].level = 1.0f;
  bandstate[i].vol = 1.0f; 
   
  Pb[i] = 1;  
  }

  //setpreset (Ppreset);
  Pvolume = 0;
  Pq = 20;
  Pstartfreq = 0;
  Pendfreq = 80;
  Prate = 20;
  
  reinitfilter();
  adjustfreqs();
};

Infinity::~Infinity ()
{
};

void inline
Infinity::oscillator()
{
for (int i=0; i<NUM_INF_BANDS; i++)  {
bandstate[i].sinp += fconst*bandstate[i].cosp;
bandstate[i].cosp -= bandstate[i].sinp*fconst;
bandstate[i].lfo = 0.5f*(1.0f + bandstate[i].sinp);  //lfo modulates filter band volume
bandstate[i].ramp += rampconst;  //ramp modulates filter band frequency cutoff
if (bandstate[i].ramp > fend) bandstate[i].ramp = fstart;  //probably faster than fmod()
if (bandstate[i].ramp < fstart) bandstate[i].ramp = fend;  //if it is going in reverse (rampconst < 0)
bandstate[i].vol = bandstate[i].level*bandstate[i].lfo;

  filterl[i]->directmod(bandstate[i].ramp);
  filterr[i]->directmod(bandstate[i].ramp);
}

}

/*
 * Apply the effect
 */
void
Infinity::out (float * smpsl, float * smpsr)
{
  int i, j;
  float tmpr, tmpl;

  
  for (i = 0; i<PERIOD; i++)  {
    //modulate
    oscillator();
   tmpr = tmpl = 0.0f;   
    //run filter
    for (j=0; j<NUM_INF_BANDS; j++)  {
    tmpl+=bandstate[j].vol*filterl[j]->filterout_s(smpsl[i]);
    tmpr+=bandstate[j].vol*filterr[j]->filterout_s(smpsr[i]);
    }
    efxoutl[i] = tmpl;
    efxoutr[i] = tmpr;
  }
 
};

/*
 * Cleanup the effect
 */
void
Infinity::cleanup ()
{
  reinitfilter ();
  for ( int i = 0; i<NUM_INF_BANDS; i++) {
  filterl[i]->cleanup();
  filterr[i]->cleanup();
  }
  
  
};


/*
 * Parameter control
 */

void
Infinity::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
};


void
Infinity::reinitfilter ()
{
float halfpi = -M_PI/2.0f;  //offset so bandstate[0].sinp = -1.0 when bandstate[0].ramp = 0;
for (int i=0; i<NUM_INF_BANDS; i++)  {  //get them started on their respective phases
bandstate[i].sinp = sinf(halfpi + D_PI*((float) i)/((float) NUM_INF_BANDS));
bandstate[i].cosp = cosf(halfpi + D_PI*((float) i)/((float) NUM_INF_BANDS));
bandstate[i].ramp = ((float) i)/((float) NUM_INF_BANDS);
bandstate[i].lfo = 0.5f*(1.0f + bandstate[i].sinp);  //lfo modulates filter band volume
  
  filterl[i]->setmix(0, NULL, NULL, NULL);
  filterr[i]->setmix(0, NULL, NULL, NULL);
  filterl[i]->setmode(1);
  filterr[i]->setmode(1);
  filterl[i]->settype(2);
  filterr[i]->settype(2);  //bandpass  
  filterl[i]->setq(qq);
  filterr[i]->setq(qq);
  filterl[i]->directmod(bandstate[i].ramp);
  filterr[i]->directmod(bandstate[i].ramp);
  
      if(Pq<0) qq = powf(2.0f,((float) Pq)/64.0f);  //q ranges down to 0.5
      else qq = powf(2.0f,((float) Pq)/8.0f);  //q can go up to 256  
}
  
};

void
Infinity::setpreset (int npreset)
{
  const int PRESET_SIZE = 13;
  const int NUM_PRESETS = 1;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Basic
    {0, 127, 127, 127, 127, 127, 127, 127, 127, 10, 20, 80, 60 }

  };

  if(npreset>NUM_PRESETS-1)  
    {   
     Fpre->ReadPreset(31,npreset-NUM_PRESETS+1);    
     for (int n = 0; n < PRESET_SIZE; n++)    
     changepar (n, pdata[n]);    
    }    
  else                                      
  {     

  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  }

  Ppreset = npreset;
  
  reinitfilter ();
};

void
Infinity::adjustfreqs()
{
  float guard;
  float scale;
  float frate;
  
      fstart = (float) Pendfreq/127.0f;
      fend = (float) Pstartfreq/127.0f;  
      frate = (float) Prate/60.0f;    //beats/second  
      guard = 20.0f/fSAMPLE_RATE;
      scale = 44100.0f/fSAMPLE_RATE;  //makes it so presets behave the same.  Filter maxes out at 
                                      //~7kHz w/ 44.1k samplerate, but higher sample rates
				      //allow it to go more.  Scale keeps the range equal for all presets
				      //regardless of sample rate.      
      fstart += guard;
      fend += guard;

      fstart *= scale;
      fend *= scale;
      
      if (fstart >= 1.0f) fstart = 0.999999f;
      if (fstart < 0.0f) fstart = 0.0f;
      if (fend >= 1.0f) fend = 0.999999f;
      if (fend < 0.0f) fend = 0.0f;  
      
      rampconst = frate*(fend - fstart)/fSAMPLE_RATE;  
      //fconst =  2.0f * sinf(PI*frate / fSAMPLE_RATE);  //this is a more precise interpretation
      fconst =  D_PI*frate / fSAMPLE_RATE;  //this seems to work well at low frequencies
}

void
Infinity::changepar (int npar, int value)
{

  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      Pb[npar - 1] = value;
      bandstate[npar - 1].level = (float) value/64.0f;
      break;
    case 9:
      Pq = value;
      if(Pq<0) qq = powf(2.0f,((float) Pq)/64.0f);  //q ranges down to 0.5
      else qq = powf(2.0f,((float) Pq)/8.0f);  //q can go up to 256
      break;
     case 10:
      Pstartfreq = value;
      adjustfreqs();
      break;     
     case 11:
      Pendfreq = value;
      adjustfreqs();
      break;     
     case 12:
      Prate = value;
      adjustfreqs();
      break;           
    };
};

int
Infinity::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      return (Pb[npar + 1]);
      break;
    case 9:
      return (Pq);
      break;
    case 10:
      return (Pstartfreq);
      break;
    case 11:
      return (Pendfreq);
      break;
    case 12:
      return (Prate);
      break;
    default:
      return (0);
    };

};
