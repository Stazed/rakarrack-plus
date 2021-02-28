/*
  RKR_GUI_Effect.cxx

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
 * File:   RKR_GUI_Effect.cxx
 * Author: sspresto
 * 
 * Created on February 12, 2021, 11:21 AM
 */

#include "RKR_GUI_Effect.h"

RKR_Gui_Effect::RKR_Gui_Effect(int X, int Y, int W, int H, const char *L)
  : Fl_Group(X, Y, W, H, L)
{
    this->user_data((void*)(UD_Group_Efx));
}

void
RKR_Gui_Effect::initialize(RKR *_rkr, RKRGUI *_rgui )
{
    m_process = _rkr;
    m_parent = _rgui;
}
