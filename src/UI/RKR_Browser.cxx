/*
  RKR_Browser.cxx

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
 * File:   RKR_Browser.cxx
 * Author: sspresto
 * 
 * Created on April 13, 2020, 8:02 PM
 */

#include "RKR_Browser.h"
#include "../global.h"

RKR_Browser::RKR_Browser(int X, int Y, int W, int H, const char *label) : Fl_Browser(X, Y, W, H, label)
{
    m_previous_font_size = g_default_font_size;
    m_start_width = W;
    m_start_height = H;
    m_start_label_offset = 0;
    m_start_text_offset = 0;
//    this->user_data((void*)(BOX_USER_DATA));
}

void RKR_Browser::draw()
{
    /* To update the font size if user changes the value in settings */
    if(g_default_font_size != m_previous_font_size)
    {
        m_previous_font_size = g_default_font_size;
        font_resize(w(), h());
    }

    Fl_Browser::draw();
}

void RKR_Browser::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int label_font_size = g_default_font_size + m_start_label_offset;
    int adjusted_label_size = (float) (label_font_size * resize_ratio);

    labelsize(adjusted_label_size);

    int text_font_size = g_default_font_size + m_start_text_offset;
    int adjusted_text_size = (float) (text_font_size * resize_ratio);
    
    textsize(adjusted_text_size);
}

void RKR_Browser::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Browser::resize(X, Y, W, H);
}
