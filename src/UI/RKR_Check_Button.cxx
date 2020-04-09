/*
  RKR_Check_Button.cxx

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
 * File:   RKR_Check_Button.cxx
 * Author: sspresto
 * 
 * Created on April 9, 2020, 1:06 PM
 */

#include "RKR_Check_Button.h"

RKR_Check_Button::RKR_Check_Button(int X, int Y, int W, int H, const char *label) : RKR_Light_Button(X, Y, W, H, label)
{
    box(FL_NO_BOX);
    down_box(FL_DOWN_BOX);
    selection_color(FL_FOREGROUND_COLOR);
}



