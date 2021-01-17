/*
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
 * File:   ParametricEQ.C
 * Author: sspresto
 * 
 * Created on January 11, 2021, 7:27 PM
 */

#include "ParametricEQ.h"

ParametricEQ::ParametricEQ(double sample_frequency, uint32_t intermediate_bufsize):
    EQ(sample_frequency, intermediate_bufsize)
{
}

void
ParametricEQ::initialize()
{
    init_filters();

    for (int i = 0; i <= 10; i += 5)
    {
        changepar(i + 10, 7);
        changepar(i + 13, 64);
        changepar(i + 14, 0);
    }
}

void
ParametricEQ::setpreset(int npreset)
{
    const int PRESET_SIZE = 10;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {72, 64, 64, 1077, 64, 64, 8111, 64, 64, 64},
        //Pop
        {72, 73, 45, 1077, 64, 64, 8111, 69, 38, 64},
        //Jazz
        {72, 71, 38, 1077, 64, 64, 10580, 69, 38, 64}
    };


    if (npreset >= NUM_PRESETS)
    {
        Fpre->ReadPreset(9, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < 3; n++)
        {
            changepar(n * 5 + 11, pdata[n * 3]);
            changepar(n * 5 + 12, pdata[n * 3 + 1]);
            changepar(n * 5 + 13, pdata[n * 3 + 2]);
        }
        
        changepar(Parametric_Gain, pdata[9]);
    }
    else
    {
        for (int n = 0; n < 3; n++)
        {
            changepar(n * 5 + 11, presets[npreset][n * 3]);
            changepar(n * 5 + 12, presets[npreset][n * 3 + 1]);
            changepar(n * 5 + 13, presets[npreset][n * 3 + 2]);
        }
        
        changepar(Parametric_Gain, presets[npreset][9]);
    }
}

