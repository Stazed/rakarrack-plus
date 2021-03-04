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
    m_look_changed(0),
    m_key_search_used(1),
    m_key_found(0)
{
    this->user_data((void*)(UD_RKR_Highlight));
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

int RKR_Browser::handle(int event)
{
    // Search the browser for alpha match, cycle through all items
    if (event == FL_KEYBOARD && (Fl::focus() == this))
    {
        int keyboard_key = Fl::event_key();
        int lower_case = 0;

        for (int i = m_key_search_used; i <= size(); i++)
        {
            // The font browser text contains formatting that is added from
            // MiraConfig(). The format specifier @ + type, currently has
            // a max of two added. We check for the specifier here and 
            // match the character after the format specifiers.
            if(text(i)[0] == '@')
            {
                if(text(i)[2] == '@')
                {
                    lower_case = tolower(text(i)[4]);   // two format specifiers
                }
                else
                {
                    lower_case = tolower(text(i)[2]);   // one format specifier
                }
            }
            else
            {
                lower_case = tolower(text(i)[0]);       // no formatting
            }

            if (lower_case == keyboard_key)
            {
                select(i, 1);
                m_key_search_used = i + 1;
                m_key_found = 1;            // true
                
                // If we are on the last item, then reset to top for next search
                // or we get stuck at the bottom since the counter(i) never reaches the end
                if(m_key_search_used == (size() + 1))
                {
                    m_key_search_used = 1;
                }
                
                do_callback();
                return 1;                   // event used
            }

            // Go back to the beginning if we found a key to cycle around on each key press
            if(m_key_search_used != 1)
            {
                if((i >= size ()) && m_key_found)   // check key found or endless loop
                {
                    i = 0;
                    m_key_search_used = 1;  // restart from beginning
                    m_key_found = 0;        // false
                    continue;
                }
            }
        }
    }

    return Fl_Browser::handle (event);
}