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
        change_parameters(i + 10, 7);
        change_parameters(i + 13, 64);
        change_parameters(i + 14, 0);
    }
}

void
ParametricEQ::set_random_parameters()
{
    for(int i = 0; i < C_PARAMETRIC_EQ_PARAMETERS; i++)
    {
        switch(i)
        {
            case Parametric_Low_Gain:
            case Parametric_Low_Q:
            case Parametric_Mid_Gain:
            case Parametric_Mid_Q:
            case Parametric_High_Gain:
            case Parametric_High_Q:
            case Parametric_Gain:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Parametric_Low_Freq:
            {
                int value = (int) (RND * 980);
                changepar (i, value + 20);
            }
            break;

            case Parametric_Mid_Freq:
            {
                int value = (int) (RND * 7200);
                changepar (i, value + 800);
            }
            break;

            case Parametric_High_Freq:
            {
                int value = (int) (RND * 20000);
                changepar (i, value + 6000);
            }
            break;
        }
    }
}

void
ParametricEQ::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = Parametric_Gain;

    for(int i = 0; i < C_PARAMETRIC_EQ_PARAMETERS; i++)
    {
        switch(param_case_offset)
        {
            case Parametric_Gain:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, 1, getpar( Parametric_Gain ) - 64, eqp_parameters[0 * 3 + 1], eqp_parameters[0 * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Parametric_Gain ) - 64);
                    s_buf += ":";
                }

                // the subsequent parameters start at 0, we add one on break
                param_case_offset = -1;
            }
            break;

            case Parametric_Low_Freq:
            case Parametric_Mid_Freq:
            case Parametric_High_Freq:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), eqp_parameters[i * 3 + 1], eqp_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) );
                    s_buf += ":";
                }
            }
            break;
                
            case Parametric_Low_Gain:
            case Parametric_Low_Q:
            case Parametric_Mid_Gain:
            case Parametric_Mid_Q:
            case Parametric_High_Gain:
            case Parametric_High_Q:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ) - 64, eqp_parameters[i * 3 + 1], eqp_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) - 64);

                    if ( param_case_offset !=  Parametric_High_Q)   // last one no need for delimiter
                    s_buf += ":";
                }
            }
            break;
        }
        
        param_case_offset++;
    }
}

void
ParametricEQ::setpreset(int npreset)
{
    const int PRESET_SIZE = C_PARAMETRIC_EQ_PARAMETERS;
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
        Fpre->ReadPreset(EFX_PARAMETRIC, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < 3; n++)
        {
            change_parameters(n * 5 + 11, pdata[n * 3]);
            change_parameters(n * 5 + 12, pdata[n * 3 + 1]);
            change_parameters(n * 5 + 13, pdata[n * 3 + 2]);
        }
        
        changepar(Parametric_Gain, pdata[9]);   // Yes changepar() vs change_parameters() is correct
    }
    else
    {
        for (int n = 0; n < 3; n++)
        {
            change_parameters(n * 5 + 11, presets[npreset][n * 3]);
            change_parameters(n * 5 + 12, presets[npreset][n * 3 + 1]);
            change_parameters(n * 5 + 13, presets[npreset][n * 3 + 2]);
        }
        
        changepar(Parametric_Gain, presets[npreset][9]);    // Yes changepar() vs change_parameters() is correct
    }
}

void
ParametricEQ::changepar(int npar, int value)
{
    switch(npar)
    {
    case Parametric_Low_Freq:
    case Parametric_Low_Gain:
    case Parametric_Low_Q:
        change_parameters(npar + 11, value);
        break;

    case Parametric_Mid_Freq:
    case Parametric_Mid_Gain:
    case Parametric_Mid_Q:
        change_parameters(npar + 13, value);
        break;

    case Parametric_High_Freq:
    case Parametric_High_Gain:
    case Parametric_High_Q:
        change_parameters(npar + 15, value);
        break;

    case Parametric_Gain:
        change_parameters(0, value);
        break;

    default:
        return;
    }
}

int
ParametricEQ::getpar(int npar)
{
    switch(npar)
    {
    case Parametric_Low_Freq:
    case Parametric_Low_Gain:
    case Parametric_Low_Q:
        return get_parameters(npar + 11);


    case Parametric_Mid_Freq:
    case Parametric_Mid_Gain:
    case Parametric_Mid_Q:
        return get_parameters(npar + 13);


    case Parametric_High_Freq:
    case Parametric_High_Gain:
    case Parametric_High_Q:
        return get_parameters(npar + 15);


    case Parametric_Gain:
        return get_parameters(0);

    }
    
    return 0;
}