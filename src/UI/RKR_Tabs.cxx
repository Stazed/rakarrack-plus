/*
  RKR_Tabs.cxx

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
 * File:   RKR_Tabs.cxx
 * Author: sspresto
 * 
 * Created on April 15, 2020, 5:05 AM
 */

#include <FL/Fl_Widget.H>

#include "RKR_Tabs.h"
#include "../global.h"

RKR_Tabs::RKR_Tabs(int X, int Y, int W, int H, const char *label) :
    Fl_Tabs(X, Y, W, H, label),
    m_label_offset(4),      // C_DEFAULT_FONT_SIZE + 4 = 14pt
    m_start_width(W),
    m_start_height(H),
    m_look_changed(0)
{
}

void RKR_Tabs::draw()
{

    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;

        font_resize(w(), h());
    }

    Fl_Tabs::draw();
}

void RKR_Tabs::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    for (int i = 0; i < children(); ++i)
    {
        Fl_Widget *c = child(i);
        c->labelsize(adjusted_label_size);
        c->labelfont (global_font_type);
        c->labelcolor (global_label_color);
        c->color (global_fore_color);
        c->selection_color (global_back_color);
    }
}

void RKR_Tabs::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Tabs::resize(X, Y, W, H);
}


