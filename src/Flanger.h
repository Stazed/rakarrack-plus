/*
  ZynAddSubFX - a software synthesizer

  Chorus.C - Chorus and Flange effects
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu

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

/* 
 * File:   Flanger.h
 * Author: sspresto
 *
 * Created on January 25, 2021, 7:16 PM
 */

#ifndef FLANGER_H
#define FLANGER_H

#include "Chorus.h"

const int C_FLANGER_PARAMETERS = 13;

enum Flanger_Index
{
    Flanger_DryWet = 0,
    Flanger_Pan,
    Flanger_LFO_Tempo,
    Flanger_LFO_Random,
    Flanger_LFO_Type,
    Flanger_LFO_Stereo,
    Flanger_Depth,
    Flanger_Delay,
    Flanger_Feedback,
    Flanger_LR_Cross,
    Flanger_SKIP_Flange_10,
    Flanger_Subtract,
    Flanger_Intense
};

class Flanger :public Chorus
{
public:
    Flanger(double sample_rate, uint32_t intermediate_bufsize);
    void setpreset (int npreset);

private:

};

#endif /* FLANGER_H */

