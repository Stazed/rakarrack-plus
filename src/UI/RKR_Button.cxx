/*
  RKR_Buttom.cxx

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
 * File:   RKR_Button.cxx
 * Author: sspresto
 * 
 * Created on April 8, 2020, 9:46 PM
 */

#include "RKR_Button.h"
#include "../global.h"

RKR_Button::RKR_Button(int X, int Y, int W, int H, const char *label) :
    Fl_Button(X, Y, W, H, label),
    m_label_offset(0),              // default 10pt - (C_DEFAULT_FONT_SIZE)
    m_start_width(W),
    m_start_height(H),
    m_bank_highlight_preset(0),     // false
    m_button_type(BUTTON_DEFAULT),
    m_look_changed(0),
    m_bank_under_mouse(0)
{
    this->user_data((void*)(UD_RKR_Highlight));
}

void RKR_Button::draw()
{
    // This must be done for bank window outside m_look_changed for preset selection
    if(m_button_type == BUTTON_BANK_HIGHLIGHT)
    {
        if(m_bank_highlight_preset)
        {
            if(m_bank_under_mouse)
            {
                color(global_leds_color);
            }
            else
            {
                color(fl_darker(global_leds_color));
            }
        }
        else
        {
            if(m_bank_under_mouse)
            {
                color(fl_color_average(global_fore_color, fl_lighter(global_fore_color), .6));
            }
            else
            {
                color(global_fore_color);
            }
        }

        labelcolor(global_label_color);
    }
    
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;
    
        switch(m_button_type)
        {
            case BUTTON_DEFAULT:
            {
                color(global_fore_color);
                labelcolor(global_label_color);
                break;
            }

            case BUTTON_MIDI_GET:
            {
                labelcolor(global_label_color);
                break;
            }

            case BUTTON_RKR_LABEL:
            {
                labelcolor(fl_darker(global_leds_color));
                break;
            }

            case BUTTON_BANK_RESET:
            {
                color(fl_darker(FL_RED));
                labelcolor(global_label_color);
                break;
            }
        }

        labelfont(global_font_type);
        selection_color(global_leds_color);
        font_resize(w(), h());
    }

    Fl_Button::draw();
}

void RKR_Button::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    labelsize(adjusted_label_size);
}

void RKR_Button::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Button::resize(X, Y, W, H);
}
