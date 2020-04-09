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

RKR_Light_Button::RKR_Light_Button(int X, int Y, int W, int H, const char *label) : Fl_Light_Button(X, Y, W, H, label)
{
    m_need_font_update = true;
    m_start_height = H;
    m_start_font_offset = 0;
    this->user_data((void*)(LIGHT_BUTTON_USER_DATA));
}

void RKR_Light_Button::draw()
{
    if(m_need_font_update)
    {
        m_need_font_update = false;
        font_resize(x(), y(), w(), h());
    }

    Fl_Light_Button::draw();
}

void RKR_Light_Button::font_resize(int X, int Y, int W, int H)
{
    float H_ratio = (float) H / m_start_height;
    int font_size = g_value_font_size + m_start_font_offset;
    int H_size = (float) (font_size * H_ratio);
    
    labelsize(H_size);
}

void RKR_Light_Button::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(X, Y, W, H);

    Fl_Light_Button::resize(X, Y, W, H);
}
