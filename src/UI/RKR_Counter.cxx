/*
  RKR_Counter.cxx

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
 * File:   RKR_Counter.cxx
 * Author: sspresto
 * 
 * Created on April 9, 2020, 4:17 PM
 */

#include "RKR_Counter.h"
#include "../global.h"

RKR_Counter::RKR_Counter(int X, int Y, int W, int H, const char *label) : Fl_Counter(X, Y, W, H, label)
{
    m_need_font_update = true;
    m_start_height = H;
    m_start_label_offset = 0;
    m_start_text_offset = 0;
//    this->user_data((void*)(BUTTON_USER_DATA));
}

void RKR_Counter::draw()
{
    if(m_need_font_update)
    {
        m_need_font_update = false;
        font_resize(x(), y(), w(), h());
    }

    Fl_Counter::draw();
}

void RKR_Counter::font_resize(int X, int Y, int W, int H)
{
    float H_ratio = (float) H / m_start_height;
    int label_font_size = g_value_font_size + m_start_label_offset;
    int H_label_size = (float) (label_font_size * H_ratio);
    
    labelsize(H_label_size);
    
    int text_font_size = g_value_font_size + m_start_text_offset;
    int H_text_size = (float) (text_font_size * H_ratio);
    
    textsize(H_text_size);
}

void RKR_Counter::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(X, Y, W, H);

    Fl_Counter::resize(X, Y, W, H);
}
