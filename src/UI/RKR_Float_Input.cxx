/*
  RKR_Float_Input.cxx

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
 * File:   RKR_Float_Input.cxx
 * Author: sspresto
 * 
 * Created on May 15, 2020, 11:45 AM
 */

#include "RKR_Float_Input.h"
#include "../global.h"

RKR_Float_Input::RKR_Float_Input(int X, int Y, int W, int H, const char *label) :
    Fl_Float_Input(X, Y, W, H, label),
    m_label_offset(0),      // C_DEFAULT_FONT_SIZE
    m_text_offset(0),       // C_DEFAULT_FONT_SIZE
    m_start_width(W),
    m_start_height(H),
    m_look_changed(0),
    m_max_value(0.0),
    m_min_value(0.0)
{
}

void RKR_Float_Input::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;
        color(global_fore_color);
        labelcolor(global_label_color);
        labelfont(global_font_type);
        textfont(global_font_type);
        font_resize(w(), h());
    }

    Fl_Float_Input::draw();
}

void RKR_Float_Input::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    labelsize(adjusted_label_size);
    
    int text_font_size = global_font_size + m_text_offset;
    int adjusted_text_size = (float) (text_font_size * resize_ratio);
    
    textsize(adjusted_text_size);
}

void RKR_Float_Input::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Float_Input::resize(X, Y, W, H);
}

int RKR_Float_Input::handle(int event)
{
    int ret = Fl_Input::handle(event);
    
    if(strcmp(value(), ""))
    {
        return ret; // Not blank - we have a value
    }
    else
    {
        value("0"); // User cleared the field - this is minimum
    }
    
    return ret;
}

