/*
  ZynAddSubFX - a software synthesizer

  Analog Filter.h - Several analog filters (lowpass, highpass...)
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu and Ryan Billing

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

#ifndef ANALOG_FILTER_H
#define ANALOG_FILTER_H

#include "Filter_.h"

class AnalogFilter:public Filter_
{
public:
//    AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,
//                  unsigned char Fstages, double samplerate);
    AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,
                  unsigned char Fstages, double samplerate, float* interpbuf);//interpbuf MUST be array equal or larger than period
    ~AnalogFilter ();
    void filterout (float * smp, uint32_t period);
    float filterout_s (float smp);

    void setfreq (float frequency);
    void setfreq_and_q (float frequency, float q_);
    void setq (float q_);
    void setSR(unsigned int SR);

    void settype (int type_);
    void setgain (float dBgain);
    void setstages (int stages_);
    void cleanup ();
    void reversecoeffs();

    float H (float freq);	//Obtains the response for a given frequency


private:
    
    unsigned int iSAMPLE_RATE;
    float ifSAMPLE_RATE;
    float* ismp;        //buffer for interpolating filter
    
    struct fstage
    {
        float c1, c2;

        fstage():
            c1(0.0),
            c2(0.0) {}
    } x[MAX_FILTER_STAGES + 1], y[MAX_FILTER_STAGES + 1],
    oldx[MAX_FILTER_STAGES + 1], oldy[MAX_FILTER_STAGES + 1];

    void singlefilterout (float * smp, fstage & x, fstage & y, const float * c,
                          const float * d, uint32_t period);
    float singlefilterout_s (float smp, fstage & x, fstage & y, const float * c,
                             const float * d);

    void computefiltercoefs ();
    int type;			//The type of the filter (LPF1,HPF1,LPF2,HPF2...)
    unsigned int stages;			//how many times the filter is applied (0->1,1->2,etc.)
    int order;			//the order of the filter (number of poles)
    int needsinterpolation, firsttime;
    int abovenq;			//this is 1 if the frequency is above the nyquist
    int oldabovenq;		//if the last time was above nyquist (used to see if it needs interpolation)

    float m_freq;		//Frequency given in Hz
    float q;			//Q factor (resonance or Q factor)
    float gain;		//the gain of the filter (if are shelf/peak) filters


    float c[3], d[3];		//coefficients

    float oldc[3], oldd[3];	//old coefficients(used only if some filter paremeters changes very fast, and it needs interpolation)

//    float xd[3], yd[3];	//used if the filter is applied more times - not used

};


#endif
