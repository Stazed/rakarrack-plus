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
#include <stdlib.h>
#include "f_sin.h"

delayline::delayline (float maxdelay, int maxtaps_)
{
maxtaps = maxtaps_;
maxtime = fSAMPLE_RATE*maxdelay;
maxdelaysmps = SAMPLE_RATE*lrintf( ceilf(maxdelay));
ringbuffer = (float *) malloc (sizeof (float) * maxdelaysmps);
oldtime =  (float *) malloc (sizeof (float) * maxtaps);
avgtime =  (float *) malloc (sizeof (float) * maxtaps);
olddelta =  (float *) malloc (sizeof (float) * maxtaps);
stctr =  (int *) malloc (sizeof (int) * maxtaps);

pstruct = (phasevars *) malloc(sizeof (struct phasevars)*maxtaps);

zero_index = 0;
tconst = 1.0f;
itap = 1.0f/((float) maxtaps);
tap = 0;
rvptr=0;
distance = 0;


float dt = 1.0f/fSAMPLE_RATE;
alpha = dt/(0.15f + dt);
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
int i,k;
  for (i = 0; i < maxdelaysmps; i++)
    ringbuffer[i] = 0.0;
  for (i = 0; i < maxtaps; i++){
    oldtime[i] = 0.0;    
    avgtime[i] = 0.0;  
    olddelta[tap] = 0.0; 
     
    for(k = 0; k<4;k++) {
    pstruct[i].yn1[k] = 0.0f;
    pstruct[i].xn1[k] = 0.0f;    
    pstruct[i].gain[k] = 0.0f;     
    }   
    }
    
for (i=0;i<maxtaps;i++) {
avgtime[i] = 0.5f*fSAMPLE_RATE;
oldtime[i] = avgtime[i];
stctr[i] = 0;
}

};


float
delayline::delay_simple(float smps, float time_, int tap_, int touch, int reverse)
{
int dlytime = 0;
int bufptr = 0;

if(tap_ >= maxtaps) tap = 0;
else tap = tap_;

float time = fSAMPLE_RATE*time_;    //convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
if(time>maxtime) time = maxtime;

//now put in the sample
if(touch) {  //make touch zero if you only want to pull samples off the delay line
ringbuffer[zero_index] = smps;

if(--zero_index<0) zero_index = maxdelaysmps;


}

oldtime[tap] = time;   

dlytime = lrintf(oldtime[tap]);

//if we want reverse delay
//you need to call this every time to keep the buffers up to date, and it's on a different tap
if(reverse) {

bufptr = (dlytime + zero_index);  //this points to the sample we want to get 
if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;
if(++rvptr>maxdelaysmps) rvptr = 0;

if(bufptr>zero_index) {
  if(rvptr>bufptr) {
  rvptr = zero_index;
  distance = 0;
  }  
  else distance = rvptr - zero_index;  
}
else if ((bufptr<zero_index) && (rvptr<zero_index))
{
  if(rvptr>bufptr){
  rvptr = zero_index;
  distance = 0;
  }
  else distance = rvptr + maxdelaysmps - zero_index;
}
else distance = rvptr - zero_index;


bufptr = rvptr;// + zero_index;  //this points to the sample we want to get 
//if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;

}
else {
bufptr = (dlytime + zero_index);  //this points to the sample we want to get 
if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;
}

return ( ringbuffer[bufptr] );

};

float
delayline::delay(float smps, float time_, int tap_, int touch, int reverse)
{
int dlytime = 0;
int bufptr = 0;

if(tap_ >= maxtaps) tap = 0;
else tap = tap_;

avgtime[tap] = alpha*time_ + beta*avgtime[tap];  //smoothing the rate of time change
float time = fSAMPLE_RATE*avgtime[tap];    //convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
if(time>maxtime) time = maxtime;
if(time<0.0f) time = 0.0f;

//now put in the sample
if(touch) {  //make touch zero if you only want to pull samples off the delay line
ringbuffer[zero_index] = smps;

if(--zero_index<0) zero_index = maxdelaysmps;

}

//slew rate limiting to prevent delay from changing more than one sample
float tfloor = floorf(time);
float fracdly = time - tfloor;
	if(oldtime[tap]<tfloor) oldtime[tap] += tconst;
	else if (oldtime[tap]>tfloor) oldtime[tap] -= tconst;
        else oldtime[tap] = tfloor;   

dlytime = lrintf(oldtime[tap]);

//if we want reverse delay
//you need to call this every time to keep the buffers up to date, and it's on a different tap
if(reverse) {

bufptr = (dlytime + zero_index);  //this points to the sample we want to get 
if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;
if(++rvptr>maxdelaysmps) rvptr = 0;

if(bufptr>zero_index) {
  if(rvptr>bufptr) {
  rvptr = zero_index;
  distance = 0;
  }  
  else distance = rvptr - zero_index;  
}
else if ((bufptr<zero_index) && (rvptr<zero_index))
{
  if(rvptr>bufptr){
  rvptr = zero_index;
  distance = 0;
  }
  else distance = rvptr + maxdelaysmps - zero_index;
}
else distance = rvptr - zero_index;


bufptr = rvptr;// + zero_index;  //this points to the sample we want to get 
//if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;

}
else {
bufptr = (dlytime + zero_index);  //this points to the sample we want to get 
if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;
}


float delta = (1.0f - fracdly)/(1.0f + fracdly);

float increment = 0.0f;
float dfract = itap*(delta - olddelta[tap]);
for(int k = 0; k<4; k++) //continuous elevation of phase stages
{
//pstruct[tap].gain[k] = 0.33333f;
pstruct[tap].gain[k] =  olddelta[tap] + dfract*increment;
increment+=1.0f;
}
olddelta[tap] = delta;

return ( phaser(ringbuffer[bufptr]) );
//return ( ringbuffer[bufptr] );

};


float
delayline::get_phaser(float smps, float lfo, float tap_)
{
float delta = lfo;
if(delta > 1.0f) delta = 1.0f;
if(delta < 0.0f) delta = 0.0f;
tap = tap_;

pstruct[tap].gain[stctr[tap]] =  (1.0f - delta)/(1.0f + delta);

return ( phaser(smps) );
};


inline float
delayline::phaser(float fxn)  //4-stage All-pass phaser interpolation
{

float xn = fxn;
         for (int st = 0; st<4; st++) {
	  pstruct[tap].yn1[st] = pstruct[tap].xn1[st] - pstruct[tap].gain[st] * (xn + pstruct[tap].yn1[st]);
	  pstruct[tap].xn1[st] = xn;
	  xn = pstruct[tap].yn1[st];  //feed to the next stage
	  }
	  
return fxn;

};

void
delayline::set_averaging(float tc)
{
float dt = 1.0f/fSAMPLE_RATE;
alpha = dt/(tc + dt);
beta = 1.0f - alpha;   //time change smoothing parameters
};

float
delayline::envelope()
{
float fdist = ((float) distance)/oldtime[tap];
if(fdist>0.5f) {
 if(fdist<=1.0f) fdist = 1.0f - fdist;
 else fdist = 0.0f;
}

if(fdist<=0.125f) {
fdist = 1.0f - f_sin(M_PI*fdist*4.0f + 1.5707963267949f);
}
else fdist = 1.0f;
return fdist;

};

