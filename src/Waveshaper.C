/*
  ZynAddSubFX - a software synthesizer
 
  Waveshaper.C - Distortion effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified and evolved for rakarrack by Josep Andreu and Ryan Billing
  
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


#include <stdlib.h>
#include <math.h>

Waveshaper::Waveshaper()
{
  compg = 0.0f;  //used by compression distortion
  cratio = 0.25f;  //used by compression for hardness
  tmpgain = 1.0f;  // compression distortion temp variable
  dthresh = 0.25;
  dyno = 0.0f;
  dynodecay = cSAMPLE_RATE * 0.0167f; //about 60Hz sub modulation from this

};

Waveshaper::~Waveshaper()
{
};

void
Waveshaper::waveshapesmps (int n, float * smps, int type,
	       int drive, int eff)
{
  int i;
  float ws = (float)drive / 127.0f + .00001f;
  ws = 1.0f - expf (-ws * 4.0f);
  float tmpv;

  switch (type + 1 )
    {
    case 1:
      ws = powf (10.0f, ws * ws * 3.0f) - 1.0f + 0.001f;	//Arctangent
      for (i = 0; i < n; i++)
	smps[i] = atanf (smps[i] * ws) / atanf (ws);
      break;
    case 2:
      ws = ws * ws * 32.0f + 0.0001f;	//Asymmetric
      if (ws < 1.0)
	tmpv = sinf (ws) + 0.1f;
      else
	tmpv = 1.1f;
      for (i = 0; i < n; i++)
	{
	  smps[i] = sinf (smps[i] * (0.1f + ws - ws * smps[i])) / tmpv;
	};
      break;
    case 3:
      ws = ws * ws * ws * 20.0f + 0.0001f;	//Pow
      for (i = 0; i < n; i++)
	{
	  smps[i] *= ws;
	  if (fabsf (smps[i]) < 1.0)
	    {
	      smps[i] = (smps[i] - powf (smps[i], 3.0f)) * 3.0f;
	      if (ws < 1.0)
		smps[i] /= ws;
	    }
	  else
	    smps[i] = 0.0;
	};
      break;
    case 4:
      ws = ws * ws * ws * 32.0f + 0.0001f;	//Sine
      if (ws < 1.57f)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	smps[i] = sinf (smps[i] * ws) / tmpv;
      break;
    case 5:
      ws = ws * ws + 0.000001f;	//Quantisize
      for (i = 0; i < n; i++)
	smps[i] = floorf (smps[i] / ws + 0.15f) * ws;
      break;
    case 6:
      ws = ws * ws * ws * 32.0f + 0.0001f;	//Zigzag
      if (ws < 1.0)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	smps[i] = asinf (sinf (smps[i] * ws)) / tmpv;
      break;
    case 7:
      ws = powf (2.0f, -ws * ws * 8.0f);	//Limiter
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i];
	  if (fabsf (tmp) > ws)
	    {
	      if (tmp >= 0.0)
		smps[i] = 1.0f;
	      else
		smps[i] = -1.0f;
	    }
	  else
	    smps[i] /= ws;
	};
      break;
    case 8:
      ws = powf (2.0f, -ws * ws * 8.0f);	//Upper Limiter
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i];
	  if (tmp > ws)
	    smps[i] = ws;
	  smps[i] *= 2.0f;
	};
      break;
    case 9:
      ws = powf (2.0f, -ws * ws * 8.0f);	//Lower Limiter
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i];
	  if (tmp < -ws)
	    smps[i] = -ws;
	  smps[i] *= 2.0f;
	};
      break;
    case 10:
      ws = (powf (2.0f, ws * 6.0f) - 1.0f) / powf (2.0f, 6.0f);	//Inverse Limiter
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i];
	  if (fabsf (tmp) > ws)
	    {
	      if (tmp >= 0.0)
		smps[i] = tmp - ws;
	      else
		smps[i] = tmp + ws;
	    }
	  else
	    smps[i] = 0;
	};
      break;
    case 11:
      ws = powf (5.0f, ws * ws * 1.0f) - 1.0f;	//Clip
      for (i = 0; i < n; i++)
	smps[i] =
	  smps[i] * (ws + 0.5f) * 0.9999f - floorf (0.5f +
						  smps[i] * (ws +
							     0.5f) * 0.9999f);
      break;
    case 12:
      ws = ws * ws * ws * 30.0f + 0.001f;	//Asym2
      if (ws < 0.3)
	tmpv = ws;
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i] * ws;
	  if ((tmp > -2.0) && (tmp < 1.0))
	    smps[i] = tmp * (1.0f - tmp) * (tmp + 2.0f) / tmpv;
	  else
	    smps[i] = 0.0f;
	};
      break;
    case 13:
      ws = ws * ws * ws * 32.0f + 0.0001f;	//Pow2
      if (ws < 1.0)
	tmpv = ws * (1.0f + ws) / 2.0f;
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i] * ws;
	  if ((tmp > -1.0f) && (tmp < 1.618034f))
	    smps[i] = tmp * (1.0f - tmp) / tmpv;
	  else if (tmp > 0.0)
	    smps[i] = -1.0f;
	  else
	    smps[i] = -2.0f;
	};
      break;
    case 14:
      ws = powf (ws, 5.0f) * 80.0f + 0.0001f;	//sigmoid
      if (ws > 10.0)
	tmpv = 0.5f;
      else
	tmpv = 0.5f - 1.0f / (expf (ws) + 1.0f);
      for (i = 0; i < n; i++)
	{
	  float tmp = smps[i] * ws;
	  if (tmp < -10.0)
	    tmp = -10.0f;
	  else if (tmp > 10.0)
	    tmp = 10.0f;
	  tmp = 0.5f - 1.0f / (expf (tmp) + 1.0f);
	  smps[i] = tmp / tmpv;
	};
      break;
      case 15:		//Sqrt "Crunch" -- Asymmetric square root distortion.
      ws = ws*ws*CRUNCH_GAIN + 1.0f;

      for (i = 0; i < n; i++)
	{
	float tmp = smps[i] * ws;	
	if (tmp < Tlo) {  
	smps[i] = Tlc - sqrtf(-tmp*DIV_TLC_CONST);

	}
	else if (tmp > Thi) {
	smps[i] = Thc + sqrtf(tmp*DIV_THC_CONST);	
	}
	else { 
 	smps[i] = tmp;
	};
	  
	  if (smps[i] < -1.0)
	    smps[i] = -1.0f;
	  else if (smps[i] > 1.0)
	    smps[i] = 1.0f;

	};
      break;
      case 16:		//Sqrt "Crunch2" -- Asymmetric square root distortion.
      ws = ws*ws*CRUNCH_GAIN + 1.0f;

      for (i = 0; i < n; i++)
	{
	float tmp = smps[i] * ws;	
	if (tmp < Tlo) {  
	smps[i] = Tlc;

	}
	else if (tmp > Thi) {
	smps[i] = Thc + sqrtf(tmp*DIV_THC_CONST);	
	}
	else { 
 	smps[i] = tmp;
	};
	  
	  if (smps[i] < -1.0)
	    smps[i] = -1.0f;
	  else if (smps[i] > 1.0)
	    smps[i] = 1.0f;
	};
      break;
      
      case 17:		//Octave Up
      ws = ws*ws*30.0f + 1.0f;

      for (i = 0; i < n; i++)
	{
	float tmp = fabs(smps[i])* ws;	
	if (tmp > 1.0f) {  
	tmp = 1.0f;
	}

	smps[i] = tmp;		//a little bit of DC correction

	};
      break;
       case 18:
      ws = ws*D_PI+.00001;
      if (ws < 1.57f)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
      for (i = 0; i < n; i++)
        smps[i]=sinf(ws * smps[i] + sinf( ws * 2.0f*smps[i]))/ tmpv;  

      break;                                                               

       case 19:
      ws =  ws * D_PI + 0.0001f;
       if (ws < 1.57f)
	tmpv = sinf (ws);
      else
	tmpv = 1.0f;
       for (i = 0; i < n; i++)
       smps[i]=sinf(ws * smps[i] + sinf(ws * smps[i])/tmpv);  
      break;                                                               
        
	case 20:  //Compression
        cratio = 1.25f - ws;
	ws =  2.0f*ws*CRUNCH_GAIN + 1.0f;
	   for (i = 0; i < n; i++)    //apply compression 
	   {
	   tmpv = fabs(dynodecay + ws * smps[i]);

	   if(dyno > 0.5) dyno = 0.5 + dynodecay*(dyno - 0.5);
   
	   if(tmpv > dthresh)                                //if envelope of signal exceeds thresh, then compress
	   {
	   compg = dthresh + dthresh*(tmpv - dthresh)/tmpv; 
	   dthresh = 0.25f + cratio*(compg - dthresh);   //dthresh changes dynamically
	   
		   if (smps[i] > 0.0)
		   {
		    smps[i] = compg;
		    }
		   else 
		   {
		   smps[i] = -1.0f * compg;
		   }  
		   dyno += compg;
		   dyno *= dynodecay;
		   
	   }
	   else
	   {
	   smps[i] *= ws;
	   }
   
	   if(tmpv < dthresh) dthresh = tmpv;
	   if(dthresh < 0.25f) dthresh = 0.25f;
   
	   };	
	break;
        

 
      //update to Distorsion::changepar (Ptype max) if there is added more waveshapings functions
    };

};
