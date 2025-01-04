/*
Cubic Sine Approximation based upon a modified Taylor Series Expansion
Author: Ryan Billing (C) 2010

This is unlicensed.  Do whatever you want with but use at your own disgression.
The author makes no guarantee of its suitability for any purpose.
*/


#ifndef FSIN_H
#define FSIN_H

#include <math.h>
#include "../global.h"

//globals
static const float pie2 = M_PI/2.0f;
static const float fact3 = 0.148148148148148f; //can multiply by 1/fact3

static inline float
f_sin(float x)
{
    if ((x>D_PI) || (x<-D_PI))
    {
        x = fmod(x,D_PI);
    }

    if (x < 0.0f)
    {
        x+=D_PI;
    }
    
    bool sign = 0;
    
    if(x>M_PI)
    {
        x = D_PI - x;
        sign = 1;
    }

    float y = 0;  //function output
    if (x <= pie2)
    {
        y = x - x*x*x*fact3;
    }
    else
    {
        float tmp = x - M_PI;
        y = -tmp + tmp*tmp*tmp*fact3;
    }

    if (sign)
    {
        y = -y;
    }

    return y;
}

static inline float
f_cos(float x_)
{
    return f_sin(pie2 + x_);
}

#endif
