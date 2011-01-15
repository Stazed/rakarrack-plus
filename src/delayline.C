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
avgtime =  (float *) malloc (sizeof (float) * maxtaps);
time =  (float *) malloc (sizeof (float) * maxtaps);

pstruct = (phasevars *) malloc(sizeof (struct phasevars)*maxtaps);

zero_index = 0;
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
    avgtime[i] = 0.0;  
    time[i] = 0.0;      
    for(k = 0; k<4;k++) {
    pstruct[i].yn1[k] = 0.0f;
    pstruct[i].xn1[k] = 0.0f;    
    pstruct[i].gain[k] = 0.0f;     
    }   
    }
    
for (i=0;i<maxtaps;i++) {
avgtime[i] = 0.5f*fSAMPLE_RATE;
}
for(i=0; i<4; i++) {
lvars[i] = 0.0f;
fracts[i] = 0.0f;
}


};


float
delayline::delay_simple(float smps, float time_, int tap_, int touch, int reverse)
{
int dlytime = 0;
int bufptr = 0;

if(tap_ >= maxtaps) tap = 0;
else tap = tap_;

time[tap] = fSAMPLE_RATE*time_;    //convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
if(time[tap]>maxtime) time[tap] = maxtime;
dlytime = lrintf(time[tap]);

//now put in the sample
if(touch) {  //make touch zero if you only want to pull samples off the delay line
ringbuffer[zero_index] = smps;
if(--zero_index<0) zero_index = maxdelaysmps - 1;
}

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


bufptr = rvptr;    //this points to the sample we want to get 

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

tap = fabs(tap_);
if(tap >= maxtaps) tap = 0;

avgtime[tap] = alpha*time_ + beta*avgtime[tap];  //smoothing the rate of time change
time[tap] = 1.0f + fSAMPLE_RATE*avgtime[tap];    //convert to something that can be used as a delay line index

//Do some checks to keep things in bounds
if(time[tap]>maxtime) time[tap] = maxtime;
if(time[tap]<0.0f) time[tap] = 0.0f;

float fract = time[tap] - floorf(time[tap]); //compute fractional delay
dlytime = lrintf(floorf(time[tap]));

//now put in the sample
if(touch) {  //make touch zero if you only want to pull samples off the delay line
ringbuffer[zero_index] = smps;
if(--zero_index<0) zero_index = maxdelaysmps -1;
}

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

bufptr = rvptr; //this points to the sample we want to get 

}
else {
bufptr = (dlytime + zero_index);  //this points to the sample we want to get 
if (bufptr >= maxdelaysmps) bufptr-=maxdelaysmps;
}

lvars[3] = lvars[2];
lvars[2] = lvars[1];
lvars[1] = lvars[0];
lvars[0] = ringbuffer[bufptr];
fracts[3] = fracts[2];
fracts[2] = fracts[1];
fracts[1] = fracts[0];
fracts[0] = fract;

float output = lagrange(lvars[0], lvars[1], lvars[2], lvars[3], 0.5f*(fracts[1] + fracts[2]));

return ( output );

};


float
delayline::get_phaser(float smps, float lfo, float tap_)
{
float delta = lfo;
if(delta > 1.0f) delta = 1.0f;
if(delta < 0.0f) delta = 0.0f;
tap = tap_;

pstruct[tap].gain[0] =  (1.0f - delta)/(1.0f + delta);

return ( phaser(smps) );
};


inline float
delayline::phaser(float fxn)  //All-pass interpolation
{

float xn = fxn;
         int st = 0;
	  pstruct[tap].yn1[st] = pstruct[tap].xn1[st] - pstruct[tap].gain[st] * (xn + pstruct[tap].yn1[st]);
	  pstruct[tap].xn1[st] = xn;
	  
//return xn;
return pstruct[tap].yn1[0];

};

inline float
delayline::lagrange(float p0, float p1, float p2, float p3, float x_)
{
float x = x_;
//float x1 = -1;
//float x2 = 0;
//float x3 = 1;
//float x4 = 2;
float xm2xm1 = (x - 1.0f)*(x - 2.0f);
x = -p0*x*xm2xm1*0.16666666667f + p1*(x + 1.0f)*xm2xm1*0.5f - p2*x*(x + 1.0f)*(x - 2.0f)*0.5f + p3*x*(x + 1.0f)*(x - 1.0f)*0.16666666667f;     

return x;
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
float fdist = ((float) distance)/time[tap];
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

