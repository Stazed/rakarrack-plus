/*
  RKR_Entry_Input.cxx

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
 * File:   RKR_Entry_Input.cxx
 * Author: sspresto
 * 
 * Created on April 9, 2020, 12:33 PM
 */

#include "RKR_Entry_Input.h"
#include "../global.h"

RKR_Entry_Input::RKR_Entry_Input(int X, int Y, int W, int H, const char *label) : Fl_Input(X, Y, W, H, label)
{
    m_previous_font_size = g_default_font_size;
    m_start_width = W;
    m_start_height = H;
    m_start_font_offset = 0;
}

void RKR_Entry_Input::draw()
{
    /* To update the font size if user changes the value in settings */
    if(g_default_font_size != m_previous_font_size)
    {
        m_previous_font_size = g_default_font_size;
        font_resize(x(), y(), w(), h());
    }

    Fl_Input::draw();
}

void RKR_Entry_Input::font_resize(int X, int Y, int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = g_default_font_size + m_start_font_offset;
    int adjusted_text_size = (float) (font_size * resize_ratio);
    
    textsize(adjusted_text_size);
}

void RKR_Entry_Input::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(X, Y, W, H);

    Fl_Input::resize(X, Y, W, H);
}


