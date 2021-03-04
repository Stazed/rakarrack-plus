/*
  RKR_Fl_Slider.cxx

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
 * File:   RKR_Fl_Slider.cxx
 * Author: sspresto
 * 
 * Created on March 3, 2021, 7:54 PM
 */

#include "RKR_Fl_Slider.h"

RKR_Fl_Slider::RKR_Fl_Slider(int X, int Y, int W, int H, const char *label) :
    Fl_Slider(X, Y, W, H, label),
    m_look_changed(0)
{
}

void RKR_Fl_Slider::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;

        color(global_fore_color);
        labelfont(global_font_type);
        labelcolor(global_label_color);
    }
    
    Fl_Slider::draw();
}



