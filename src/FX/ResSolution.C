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
    // TODO
}

void
ResSolution::LV2_parameters(std::string &s_buf, int type)
{
    // TODO
}
