/*
  rakarrack - a guitar effects software

 Compressor.h  -  Compressor Effect definitions
 Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
 Stefan Westerfeld <stefan@space.twc.de>

  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

/* 
 * File:   Limiter.C
 * Author: sspresto
 * 
 * Created on January 29, 2021, 10:02 PM
 */

#include "Limiter.h"

Limiter::Limiter(double sample_rate, uint32_t intermediate_bufsize) :
    Compressor(sample_rate, intermediate_bufsize)
{
}

void
Limiter::setpreset(int npreset)
{
    const int PRESET_SIZE = C_COMPRESS_PARAMETERS;
    const int NUM_PRESETS = 4;
    
    // Sanity check
    if (npreset > (NUM_PRESETS - 1))
        return;

    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Final Limiter
        {-1, 15, 0, 5, 250, 0, 0, 1, 1},
        //HarmonicEnhancer
        {-20, 15, -3, 5, 50, 0, 0, 1, 1},
        //Band CompBand
        {-3, 2, 0, 5, 50, 1, 0, 1, 0},
        //End CompBand
        {-60, 2, 0, 10, 500, 1, 0, 1, 1},
    };

    // CompBand, HarmonicEnhancer, efx_Limiter
    for (int n = 0; n < PRESET_SIZE; n++)
        changepar(n, presets[npreset][n]);
}
