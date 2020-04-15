/*
  RKR_Scroll.cxx

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
 * File:   RKR_Scroll.cxx
 * Author: sspresto
 * 
 * Created on April 15, 2020, 8:16 AM
 */

#include "RKR_Scroll.h"
#include "../global.h"
#include "RKR_Choice.h"
#include "RKR_Box.h"

RKR_Scroll::RKR_Scroll(int X, int Y, int W, int H, const char *label) : Fl_Scroll(X, Y, W, H, label)
{
//    printf("Start W = %d: Start H = %d\n", W, H);
    m_previous_font_size = g_default_font_size;
    m_start_width = W;
    m_start_height = H;
    m_start_label_offset = 0;
}

void RKR_Scroll::draw()
{
    /* To update the font size if user changes the value in settings */
    if(g_default_font_size != m_previous_font_size)
    {
        m_previous_font_size = g_default_font_size;
//        font_resize(w(), h());
    }

    Fl_Scroll::draw();
}

void RKR_Scroll::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = g_default_font_size + m_start_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    for (int i = 0; i < children(); ++i)
    {
      Fl_Widget *c = child(i);
      c->labelsize(adjusted_label_size);
    }
}

void RKR_Scroll::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
//    font_resize(W, H);
    
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    
//    printf("W_ratio = %f: H_ratio = %f: X = %d: Y = %d: W = %d: H = %d\n", W_ratio, H_ratio, X, Y, W, H);
    
    
    for (int i = 0; i < children(); ++i)
    {
        
        Fl_Widget *c = child(i);
        
        long long ud = (long long) c->user_data();
        
        if(ud >= c_bank_used && ud < c_bank_number)
        {
            RKR_Choice *c_choice = (RKR_Choice *) c;
            c_choice->resize
            (
                (c_choice->get_start_x() + 32 )* W_ratio,
                (c_choice->get_start_y() + 380) * H_ratio,
                c_choice->get_start_width() * W_ratio,
                c_choice->get_start_height() * H_ratio
            );
        }
        else if(ud >= c_bank_number)
        {
            RKR_Box *c_box = (RKR_Box *) c;
            c_box->resize
            (
                (c_box->get_start_x() + 32 )* W_ratio,
                (c_box->get_start_y() + 380) * H_ratio,
                c_box->get_start_width() * W_ratio,
                c_box->get_start_height() * H_ratio
            );
        }
    }

    Fl_Scroll::resize(X, Y, W, H);
}


