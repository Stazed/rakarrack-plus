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
  trigtime = SAMPLE_RATE/12; //time to take next peak
  onset = 0;
  trigthresh = 0.15f;
  
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
   
  tmp = 10.0f*fabs(smpsl[i] + smpsr[i]);
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
     tscntr = 0;
     idx++;
     
     trigtimeout = trigtime;
     }

  }
  else {
    if (--trigtimeout<0) {
    trigtimeout = 0;
    }
    
    tscntr++;
  }

}


};

float 
beattracker::get_tempo()  //returns tempo in float beats per minute
{

return(0);

};

