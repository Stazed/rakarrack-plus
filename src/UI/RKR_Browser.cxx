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

RKR_Browser::RKR_Browser(int X, int Y, int W, int H, const char *label) :
    Fl_Browser(X, Y, W, H, label),
    m_label_offset(4),      // default 14pt - (C_DEFAULT_FONT_SIZE + 4)
    m_text_offset(4),       // default 14pt - (C_DEFAULT_FONT_SIZE + 4)
    m_start_width(W),
    m_start_height(H),
    m_type_browser(DEFAULT_BROWSER),
    m_look_changed(0)
{
}

void RKR_Browser::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;
        
        labelcolor(global_label_color);
        labelfont(global_font_type);
        selection_color(global_back_color);
        textcolor(global_label_color);
        color(global_fore_color);

        if(m_type_browser != FONT_BROWSER)
            textfont(global_font_type);

        font_resize(w(), h());
    }

    Fl_Browser::draw();
}

void RKR_Browser::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int label_font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (label_font_size * resize_ratio);

    labelsize(adjusted_label_size);

    int text_font_size = global_font_size + m_text_offset;
    int adjusted_text_size = (float) (text_font_size * resize_ratio);
    
    textsize(adjusted_text_size);
}

void RKR_Browser::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Browser::resize(X, Y, W, H);
}
