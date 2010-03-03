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
statel = src_new(type , 1 , &errorl);
stater = src_new(type , 1 , &errorr);

srcinfol.input_frames = 0 ;
srcinfol.output_frames_gen = 0 ;
srcinfor.input_frames = 0 ;
srcinfor.output_frames_gen = 0 ;
                                         

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
Resample::out(float *inl, float *inr, float *outl, float *outr, int frames, double ratio)
{

// ratio = output_sample_rate / input_sample_rate. 

int i_frames = frames;
int o_frames = (int) ((float)frames*ratio);

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


errorl = src_process(statel, &srcinfol);
errorr = src_process(stater, &srcinfor);


}

void
Resample::mono_out(float *inl, float *outl, int frames, double ratio)
{

int i_frames = frames;
int o_frames = (int) ((float)frames*ratio);

srcinfol.data_in = inl;
srcinfol.input_frames = i_frames;
srcinfol.data_out = outl;
srcinfol.output_frames = o_frames;
srcinfol.src_ratio = ratio;
srcinfol.end_of_input = 0;
srcinfol.src_ratio = ratio;


errorl = src_process(statel, &srcinfol);

}

