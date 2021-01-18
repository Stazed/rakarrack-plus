/*
  Rakarrack

  Cabinet.C - eq curves to approximate a guitar cabinet
  Copyright (C) 2015 Spencer Jackson
  Author: Spencer Jackson

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

#ifndef CABINET_H
#define CABINET_H

#include "EQ.h"

/* Used by LV2 for Gain and Preset setting */
const int C_CABINET_PARAMETERS = 2;

/**
 * This singular index is here for clarity as it is the only parameter for the 
 * Cabinet class. The effect uses setpreset() to change the individual filters.
 * The Cabinet_Gain is the only parameter used by changepar() and getpar().
 */
enum Cabinet_Index
{
    Cabinet_Gain = 0,
    Cabinet_Preset_Idx   // LV2 only
};

class Cabinet :public EQ
{
public:
    Cabinet (double sample_frequency, uint32_t intermediate_bufsize);
    void setpreset(int npreset);
    
    void initialize();
    int Cabinet_Preset;

#ifdef LV2_SUPPORT
    void lv2_update_params(uint32_t period);
#endif // LV2

};

#endif
