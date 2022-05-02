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
 * File:   Flanger.C
 * Author: sspresto
 * 
 * Created on January 25, 2021, 7:16 PM
 */

#include "Flanger.h"

Flanger::Flanger(double sample_rate, uint32_t intermediate_bufsize):
    Chorus(sample_rate, intermediate_bufsize)
{
}

void
Flanger::setpreset(int npreset)
{
    const int PRESET_SIZE = C_FLANGER_PARAMETERS;
    const int NUM_PRESETS = 5;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Flange1
        {64, 64, 39, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0},
        //Flange2
        {64, 64, 9, 34, 1, 40, 35, 3, 109, 0, 0, 0, 0},
        //Flange3
        {64, 64, 31, 34, 1, 94, 35, 3, 54, 0, 0, 1, 0},
        //Flange4
        {64, 64, 14, 0, 1, 62, 12, 19, 97, 0, 0, 0, 1},
        //Flange5
        {64, 64, 34, 105, 0, 24, 39, 19, 17, 0, 0, 1, 1}
    };

    // (npreset > NUM_PRESETS - 1 means user defined (Insert) presets for Flanger
    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_FLANGER, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else    // No user defined presets so just set it
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }

    Ppreset = npreset;
}

void
Flanger::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    
    for(int i = 0; i < (C_FLANGER_PARAMETERS - 1); i++) // -1 skip Flanger_SKIP_Flange_10
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Flanger_LFO_Tempo:
            case Flanger_LFO_Random:
            case Flanger_LFO_Type:
            case Flanger_LFO_Stereo:
            case Flanger_Depth:
            case Flanger_Delay:
            case Flanger_Feedback:
            case Flanger_Subtract:
            case Flanger_Intense:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), flange_parameters[i * 3 + 1], flange_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Flanger_Intense )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;
            
            // Special cases
            // wet/dry -> dry/wet reversal
            case Flanger_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Flanger_DryWet )), flange_parameters[i * 3 + 1], flange_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Flanger_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
            
            // Offset
            case Flanger_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Flanger_Pan ) - 64, flange_parameters[i * 3 + 1], flange_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Flanger_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;
            
            // Skip after this one
            case Flanger_LR_Cross:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Flanger_LR_Cross ), flange_parameters[i * 3 + 1], flange_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Flanger_LR_Cross ) );
                    s_buf += ":";
                }

                // increment for skipped parameter
                param_case_offset++;
            }
            break;
        }
        param_case_offset++;
    }
}
