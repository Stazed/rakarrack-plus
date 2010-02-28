/*

  Resample.C  -  Class
  This file is based in the harmonic_gen_1220.c by Steve Harris
  Copyright (C) 2008-2009 Josep Andreu (Holborn)
  Author: Josep Andreu

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License
(version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Resample.h"


Resample::Resample(int type) 
{


//minl = (float *) malloc (sizeof(float) * PERIOD);
//minr = (float *) malloc (sizeof(float) * PERIOD);
//moutl = (float *) malloc (sizeof(float) * PERIOD);
//moutr = (float *) malloc (sizeof(float) * PERIOD);


statel = src_new(type , 1 , errorl);
stater = src_new(type , 1 , errorr);


}


Resample::~Resample()
{
statel = src_delete(statel);
stater = src_delete(stater);


};

void
Resample::cleanup()
{
src_reset(statel);
src_reset(stater);



};



void
Resample::out(float *inl, float *inr, float *outl, float *outr, int frames, double ratio, int sent)
{

int i_frames = 0;
int o_frames = 0;

switch (sent)

{
   case 0: //Up
   i_frames = frames / 2; 
   o_frames = frames; 
   break;
   case 1:
   i_frames = frames;
   o_frames = frames / 2;
   break;
}   





srcinfol.data_in = inl;
srcinfol.input_frames = i_frames;
srcinfol.data_out = outl;
srcinfol.output_frames = o_frames;
srcinfol.src_ratio = ratio;
srcinfol.end_of_input = 0;
srcinfol.src_ratio = ratio;

srcinfor.data_in = inr;
srcinfor.input_frames = i_frames;
srcinfor.data_out = outr;
srcinfor.output_frames = o_frames;
srcinfor.src_ratio = ratio;
srcinfor.end_of_input = 0;
srcinfor.src_ratio=ratio;


src_process(statel, &srcinfol);
src_process(stater, &srcinfor);




}

