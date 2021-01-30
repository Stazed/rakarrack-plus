/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
    if (npreset > 3) return;

    const int PRESET_SIZE = C_COMPRESS_PARAMETERS;
    const int NUM_PRESETS = 4;

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
