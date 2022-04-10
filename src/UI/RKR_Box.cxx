/*
  RKR_Box.cxx

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
 * File:   RKR_Box.cxx
 * Author: sspresto
 * 
 * Created on April 8, 2020, 12:29 PM
 */

#include "RKR_Box.h"
#include "../global.h"

RKR_Box::RKR_Box(int X, int Y, int W, int H, const char *label) :
    Fl_Box(X, Y, W, H, label),
    m_label_offset(4),     // default 14pt - (C_DEFAULT_FONT_SIZE + 4)
    m_start_x(X),
    m_start_y(Y),
    m_start_width(W),
    m_start_height(H),
    m_box_type(BOX_DEFAULT),
    m_look_changed(0)
{
}

void RKR_Box::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;

        switch(m_box_type)
        {
            case BOX_DEFAULT:
            {
                labelcolor(global_label_color);
                color(global_fore_color);
                break;
            }

            case BOX_LEDS:
            {
                labelcolor(global_leds_color);
                color(global_back_color);
                break;
            }

            case BOX_LIGHT:
            {
                break;
            }
        }

        labelfont(global_font_type);
        font_resize(w(), h());
    }

    draw_box();
    draw_label();
}

void RKR_Box::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    labelsize(adjusted_label_size);
}

void RKR_Box::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Box::resize(X, Y, W, H);
}

int RKR_Box::handle(int event)
{
    if(event == FL_PUSH)
    {
        if (Fl::event_button() == 3)
        {
            do_callback();
        }
    }
    return Fl_Box::handle(event);
}
