/*
  RKR_Light_Button.cxx

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
 * File:   RKR_Light_Button.cxx
 * Author: sspresto
 * 
 * Created on April 9, 2020, 8:31 AM
 */

#include "RKR_Light_Button.h"
#include "../global.h"

RKR_Light_Button::RKR_Light_Button(int X, int Y, int W, int H, const char *label) :
    Fl_Light_Button(X, Y, W, H, label),
    m_label_offset(1),      // C_DEFAULT_FONT_SIZE + 1 = 11pt
    m_start_width(W),
    m_start_height(H),
    m_look_changed(0)
{
    this->user_data((void*)(UD_RKR_Highlight));
}

void RKR_Light_Button::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;

        labelcolor(global_label_color);
        color(global_fore_color);
        labelfont(global_font_type);
        selection_color(global_leds_color);
        font_resize(w(), h());
    }

    if(value())
    {
        if(strcmp(label(), "Off") == 0)
        {
            label("On");
        }
        else if(strcmp(label(), "FX Off") == 0)
        {
            label("FX On");
        }   
    }
    else
    {
        if(strcmp(label(), "On") == 0)
        {
            label("Off");
        }
        else if(strcmp(label(), "FX On") == 0)
        {
            label("FX Off");
        }
    }

    Fl_Light_Button::draw();
}

void RKR_Light_Button::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    labelsize(adjusted_label_size);
}

void RKR_Light_Button::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Light_Button::resize(X, Y, W, H);
}
