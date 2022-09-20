/*
  RKR_Menu_Bar.cxx

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
 * File:   RKR_Menu_Bar.cxx
 * Author: sspresto
 * 
 * Created on April 11, 2020, 8:25 AM
 */

#include "RKR_Menu_Bar.h"
#include "../global.h"

RKR_Menu_Bar::RKR_Menu_Bar(int X, int Y, int W, int H, const char *label) :
    Fl_Menu_Bar(X, Y, W, H, label),
    m_label_offset(0),      // C_DEFAULT_FONT_SIZE
    m_start_width(W),
    m_start_height(H),
    m_look_changed(0)
{
}

void RKR_Menu_Bar::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;

        color(global_back_color);
        font_resize(w(), h());
    }

    Fl_Menu_Bar::draw();
}

void RKR_Menu_Bar::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    /* The submenu items */
    for(unsigned i = 0; i < m_submenu_paths.size(); i++)
    {
        Fl_Menu_Item *sm = (Fl_Menu_Item*) find_item(m_submenu_paths[i].c_str());
    
        if(sm)
        {
            sm->labelsize(adjusted_label_size);
            sm->labelfont (global_font_type);
            sm->labelcolor (global_label_color);
        }
    }
    
    /* The Menu Bar items */
    Fl_Menu_Item *m = (Fl_Menu_Item*) menu();
    
    if(!m)
    {
        return;
    }

    for (int s = 0; s < m->size(); s++)
    {
        Fl_Menu_Item *p = m->next(s);
        if(p->label())
        {
            p->labelsize(adjusted_label_size);   /* Menu bar items */
            p->labelfont (global_font_type);
            p->labelcolor (global_label_color);
        }
    }
}

void RKR_Menu_Bar::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Menu_Bar::resize(X, Y, W, H);
}
