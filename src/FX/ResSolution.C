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
    ResSolution::setpreset(0); //this will get done before out is run
    Analog_Phaser::cleanup();
}

void
ResSolution::setpreset(int npreset)
{
    const int PRESET_SIZE = C_RESSOL_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        // (beefy marshall)
        {64, 0, 268, 0, 12, 64, 76, 64, 4, 0, 0, 127, 0},
        // (euro prog/power metal)
        {35, 0, 335, 0, 12, 64, 72, 94, 8, 0, 0, 118, 0},
        // (presence boost)
        {90, 127, 48, 0, 12, 80, 27, 79, 6, 0, 1, 87, 0},
        // (suppressor)
        {64, 0, 175, 0, 12, 64, 45, 64, 8, 0, 0, 40, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {

        Fpre->ReadPreset(EFX_RESSOLUTION, npreset - NUM_PRESETS + 1, pdata);
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

                    if ( param_case_offset !=  Ressol_Hyper)   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // Skip 2 for LFO_Random and LFO_Type after this
            case Ressol_LFO_Tempo:      // Labeled 'Shift'
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( i ), ressol_parameters[i * 3 + 1], ressol_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( i ));
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

/**
 * This override function is necessary to ensure that the LFO type and random parameters are
 * set to fixed values. This is necessary when using bank presets since the ResSolution is a
 * new effect that did not exist when the included banks were created. For these banks the
 * effect slot has default values all set to zero. When loading the bank those defaults would
 * then override the preset values as if loading from any bank file. So, for backwards compatibility,
 * we override the LFO_Random and LFO_Type with fixed values here. Also necessary when using
 * random parameter changes.
 * 
 * @param npar
 *      The parameter number.
 * 
 * @param value
 *      The parameter value.
 */

void
ResSolution::changepar(int npar, int value)
{
    switch (npar)
    {
    case Ressol_DryWet:
        setvolume(value);
        break;
    case Ressol_Distortion:
        setdistortion(value);
        break;
    case Ressol_LFO_Tempo:          // Label is shift for ResSolution
        lfo->Pfreq = value;
        lfo->updateparams(PERIOD);
        break;
    case Ressol_LFO_Random:
        lfo->Prandomness = 0;       // always 0, not really needed since static bypasses random
        lfo->updateparams(PERIOD);
        break;
    case Ressol_LFO_Type:
        lfo->PLFOtype = LFO_STATIC; // always static for ResSolution
        lfo->updateparams(PERIOD);
        barber = 0;
        break;
    case Ressol_LFO_Stereo:
        lfo->Pstereo = value;
        lfo->updateparams(PERIOD);
        break;
    case Ressol_Width:
        setwidth(value);
        break;
    case Ressol_Feedback:
        setfb(value);
        break;
    case Ressol_Stages:
        setstages(value);
        break;
    case Ressol_Mismatch:
        setoffset(value);
        break;
    case Ressol_Subtract:
        if (value > 1)
            value = 1;
        Poutsub = value;
        break;
    case Ressol_Depth:
        setdepth(value);
        break;
    case Ressol_Hyper:
        if (value > 1)
            value = 1;
        Phyper = value;
        break;
    default:
        return;
    }
}