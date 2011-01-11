/*
  Author: Ryan BillingV
  
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
#include "delayline.h"
#include "math.h"

delayline::delayline (float maxdelay, int maxtaps_)
{
maxtaps = maxtaps_;
maxtime = fSAMPLE_RATE*maxdelay;
maxdelaysmps = SAMPLE_RATE*lrintf( ceilf(maxdelay));
ringbuffer = (float *) malloc (sizeof (float) * maxdelaysmps);
oldtime =  (float *) malloc (sizeof (float) * maxtaps);
avgtime =  (float *) malloc (sizeof (float) * maxtaps);
stctr =  (int *) malloc (sizeof (int) * maxtaps);

// for(int j=0; j<maxtaps; j++)
// pstruct[j] = (phasevars *) malloc(sizeof(phasevars));
pstruct = (phasevars *) malloc(sizeof (struct phasevars)*maxtaps);

tconst = 1.0f;

tap = 0;


float dt = 1.0f/fSAMPLE_RATE;
alpha = dt/(0.5f + dt);
beta = 1.0f - alpha;   //time change smoothing parameters


cleanup();
};

delayline::~delayline ()
{

}

void
delayline::cleanup()
{
zero_index = 0;
int i;
  for (i = 0; i < maxdelaysmps; i++)
    ringbuffer[i] = 0.0;
  for (i = 0; i < maxtaps; i++){
    oldtime[i] = 0.0;    
    avgtime[i] = 0.0;       
    }
    
for (i=0;i<maxtaps;i++) {
avgtime[i] = 0.5f*fSAMPLE_RATE;
oldtime[i] = avgtime[i];
stctr[i] = 0;
}

};


float
delayline::delay(float smps, float time_, int tap_, int touch)
{
if(tap_ >= maxtaps) tap = 0;
else tap = tap_;

avgtime[tap] = alpha*time_ + beta*avgtime[tap];  //smoothing the rate of time change
float time = fSAMPLE_RATE*avgtime[tap];    //convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
if(time>maxtime) time = maxtime;

//now put in the sample
if(touch) {  //make touch zero if you only want to pull samples off the delay line
ringbuffer[zero_index] = smps;
if(--zero_index<0) zero_index = maxdelaysmps;
}

//slew rate limiting to prevent delay from changing more than one sample
float fracdly = 0.0f; 
float test1 = fabs(time - oldtime[tap]);
if (test1>1.0f) {
	fracdly = time - lrintf(time);
	if(oldtime[tap]<time) oldtime[tap] += tconst + fracdly;
	else if (oldtime[tap]>time) oldtime[tap] -= tconst + fracdly;
}
else oldtime[tap] = time;   

int dlytime = lrintf(oldtime[tap]);
int bufptr = (dlytime + zero_index);  //this points to the sample we want to get 
if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;

pstruct[tap].gain[stctr[tap]] =  oldtime[tap] - floorf(oldtime[tap]);
if(++stctr[tap]>4) stctr = 0;  //set a continuously morphing fractional delay time to each interpolating phase stage

return ( phaser(ringbuffer[bufptr]) );


};





inline float
delayline::phaser(float fxn)  //4-stage All-pass phaser interpolation
{

float xn = fxn + DENORMAL_GUARD;
         for (int st = 0; st<4; st++) {
	  pstruct[tap].yn1[st] = pstruct[tap].xn1[st] - pstruct[tap].gain[st] * (xn + pstruct[tap].yn1[st]);
	  pstruct[tap].xn1[st] = xn;
	  xn = pstruct[tap].yn1[st];  //feed to the next stage
	  }
	  
return fxn;

}
