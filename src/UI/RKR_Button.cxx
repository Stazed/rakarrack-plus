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
    m_previous_font_size(global_font_size),
    m_bank_highlight_preset(0),     // false
    m_button_type(BUTTON_DEFAULT)
{
    this->user_data((void*)(BUTTON_USER_DATA));
}

void RKR_Button::draw()
{
    switch(m_button_type)
    {
        case BUTTON_DEFAULT:
        {
            color(global_fore_color);
            labelcolor(label_color);
            break;
        }

        case BUTTON_BANK_HIGHLIGHT:
        {
            if(m_bank_highlight_preset)
            {
                color(fl_darker(global_leds_color));
            }
            else
            {
                color(global_fore_color);
            }
            
            labelcolor(label_color);
            break;
        }

        case BUTTON_MIDI_GET:
        {
            labelcolor(label_color);
            break;
        }

        case BUTTON_RKR_LABEL:
        {
            labelcolor(fl_darker(global_leds_color));
            break;
        }
    }

    labelfont(global_font_type);
    selection_color(global_leds_color);
    
    /* To update the font size if user changes the value in settings */
    if(global_font_size != m_previous_font_size)
    {
        m_previous_font_size = global_font_size;
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
