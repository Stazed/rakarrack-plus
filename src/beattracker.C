#include <math.h>
#include "beattracker.h"

 beattracker:: beattracker ()
{
  
  rmsfilter = new RBFilter (0, 15.0f, 0.15f, 1);  
  peaklpfilter = new RBFilter (0, 25.0f, 0.5f, 0); 
  peaklpfilter2 = new RBFilter (0, 25.0f, 0.5f, 0);  
  peakhpfilter = new RBFilter (1, 45.0f, 0.5f, 0); 
  
  index = (int *) malloc (sizeof (int) * PERIOD);
     
 //Trigger Filter Settings 
  peakpulse = peak = envrms = 0.0f;
  peakdecay = 10.0f/fSAMPLE_RATE;
  targatk = 12.0f/fSAMPLE_RATE;   ///for smoothing filter transition
  atk = 200.0f/fSAMPLE_RATE;
  trigtime = SAMPLE_RATE/12; //time to take next peak
  onset = 0;
  trigthresh = 0.15f;
  
  tscntr = 0;
  tsidx = 0;
  
  oldbpm = 0.0f;
  oldmost = 0.0f;
    
  cleanup ();

};

beattracker::~beattracker ()
{

};

void 
beattracker::cleanup ()
{

  peakpulse = peak = envrms = 0.0f;
  peakdecay = 10.0f/fSAMPLE_RATE;
  targatk = 12.0f/fSAMPLE_RATE;   ///for smoothing filter transition
  atk = 200.0f/fSAMPLE_RATE;
  trigtime = SAMPLE_RATE/20; //time to take next peak
  onset = 0;
  trigthresh = 0.15f;
  oldbpm = 0.0f;
  oldmost = 0.0f; 
  tscntr = 0;
  tsidx = 0;
};
 
void 
beattracker::detect (float * smpsl, float * smpsr)
{
   float tmp;
   int idx = 0;
   int i = 0;
   
  for ( i = 0; i < PERIOD; i++)  //Detect dynamics onset
  {
  index[i] = 0; //initializes all elements to zero
   
  tmp = 15.0f*fabs(smpsl[i] + smpsr[i]);
  envrms = rmsfilter->filterout_s(tmp);  
  if ( tmp > peak) peak =  atk + tmp;
  if ( envrms < peak) peak -= peakdecay;
  if(peak<0.0f) peak = 0.0f;

  peakpulse = peaklpfilter2->filterout_s(fabs(peakhpfilter->filterout_s(peak)));
 
  
  if( peakpulse > trigthresh ) {
     if (trigtimeout==0) { 
     index[idx] = i;  
     timeseries[tsidx] = tscntr;
     tsidx++;
     tsidx%=20;
     tscntr = 0;
     idx++;
     
     trigtimeout = trigtime;
     
     //test
     //get_tempo();
     }

  }
  else {
    if (--trigtimeout<0) {
    trigtimeout = 0;
    }

  }
    tscntr++;
}


};

float 
beattracker::get_tempo()  //returns tempo in float beats per minute
{
if((oldbpm>600.0f) || (oldbpm<0.0f)) oldbpm = 0.0f;

float time = 0;
if(tsidx>0) time = ((float) timeseries[tsidx-1])/fSAMPLE_RATE;
else  time = ((float) timeseries[19])/fSAMPLE_RATE;

//below really needs to be more clever to pull out 1/8, 1/4, 1/2, triplets or other time signatures.
//for now it's good enough to work with relatively steady patterns in 4/4
float divisor = 0;
int intdiv = 0;
if(time<1.0f) {
intdiv = lrintf(1.0f/time);
divisor = (float) intdiv;
time*=divisor;
}
else {
intdiv = lrintf(time);
divisor = 1.0f/((float) intdiv);
time*=divisor;
}

float bpm = 120.0f/time;

//below is some code to look at the outputs, a crude statistical analysis

//oldbpm = 0.95f*oldbpm + 0.05f*bpm;

/*
//oldbpm = avg;
//avg = oldmost;

float low = avg*0.95f;
float high = avg*1.05f;
if ( (bpm>low) && (bpm<high) ) {
oldmost = 0.1f*oldmost + 0.9f*bpm;
}
*/
//uncomment below to see what is happening
//printf("time: %f bpm: %f avg: %f most: %f time: 1/%f \n", time, bpm, oldbpm, oldmost,divisor);

return(bpm);

};

