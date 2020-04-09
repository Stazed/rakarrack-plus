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

RKR_Box::RKR_Box(int X, int Y, int W, int H, const char *label) : Fl_Box(X, Y, W, H, label)
{
    m_need_font_update = true;
    m_start_height = H;
    m_start_font_offset = 0;
    this->user_data((void*)(BOX_USER_DATA));
}

void RKR_Box::draw()
{
    if(m_need_font_update)
    {
        m_need_font_update = false;
        font_resize(x(), y(), w(), h());
    }

    draw_box();
    draw_label();
}

void RKR_Box::font_resize(int X, int Y, int W, int H)
{
    float H_ratio = (float) H / m_start_height;
    int font_size = g_value_font_size + m_start_font_offset;
    int H_size = (float) (font_size * H_ratio);
    
    labelsize(H_size);
}

void RKR_Box::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(X, Y, W, H);

    Fl_Box::resize(X, Y, W, H);
}
