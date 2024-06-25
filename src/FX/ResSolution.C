/*
  rakarrack - Audio effects software

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
 * File:   ResSolution.C
 * Author: sspresto
 * 
 * Created on June 24, 2024, 4:26 PM
 */

#include "ResSolution.h"

ResSolution::ResSolution(double sample_rate, uint32_t intermediate_bufsize):
    Analog_Phaser(sample_rate, intermediate_bufsize)
{
    // TODO set static
}


void
ResSolution::setpreset(int npreset)
{
    const int PRESET_SIZE = C_RESSOL_PARAMETERS;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        // static 1 (beefy marshall)
        {64, 0, 268, 0, 12, 64, 76, 64, 4, 0, 0, 127, 0},
        // static 2 (euro prog/power metal)
        {35, 0, 335, 0, 12, 64, 72, 94, 8, 0, 0, 118, 0},
        // static 3 (presence boost)
        {90, 127, 48, 0, 12, 80, 27, 79, 6, 0, 1, 87, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {

        Fpre->ReadPreset(EFX_ANALOG_PHASER, npreset - NUM_PRESETS + 1, pdata);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {

        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = npreset;
}

void
ResSolution::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;

    for(int i = 0; i < C_RESSOL_PARAMETERS - 2; i++)    // -2 for LFO_Random and LFO_Type
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Ressol_Distortion:
            case Ressol_LFO_Stereo:
            case Ressol_Width:
            case Ressol_Stages:
            case Ressol_Mismatch:
            case Ressol_Subtract:
            case Ressol_Depth:
            case Ressol_Hyper:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), ressol_parameters[i * 3 + 1], ressol_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( i !=  Ressol_Hyper)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // Skip 2 for LFO_Random and LFO_Type after this
            case Ressol_LFO_Tempo:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), ressol_parameters[i * 3 + 1], ressol_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));

                    if ( i !=  Ressol_Hyper)   // last one no need for delimiter
                        s_buf += ":";
                }
                
                // increment for skipped parameters, LFO_Random and LFO_Type
                param_case_offset += 2;
            }
            break;
            // wet/dry -> dry/wet reversal
            case Ressol_DryWet: 
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( param_case_offset )), ressol_parameters[i * 3 + 1], ressol_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( param_case_offset )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Ressol_Feedback:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ) - 64, ressol_parameters[i * 3 + 1], ressol_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) - 64);
                    s_buf += ":";
                }
            }
            break;
        }
        
        param_case_offset++;
    }
}
