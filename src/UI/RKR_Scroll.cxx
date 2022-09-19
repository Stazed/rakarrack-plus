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
#include "../global.h"  /* UD_Bank_Used_Start, UD_Preset_Used_Start, UD_Bank_Number */
#include "RKR_Choice.h"
#include "RKR_Box.h"
#include "RKR_Group.h"

RKR_Scroll::RKR_Scroll(int X, int Y, int W, int H, const char *label) :
    Fl_Scroll(X, Y, W, H, label),
    m_start_x(X),
    m_start_y(Y),
    m_start_width(W),
    m_start_height(H),
    m_look_changed(0),
    m_delay_scroll(false)
{
}

void RKR_Scroll::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;
        color(fl_lighter(global_back_color));
        labelcolor(global_label_color);
    }
    
    int y_pos = yposition();

    Fl_Scroll::draw();

    // need this because draw will change position when adding or re-ordering the delay scroller
    if (m_delay_scroll)
    {
        scroll_to(xposition(), y_pos);
        Fl_Scroll::draw();
    }
}

void RKR_Scroll::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    
    if(m_delay_scroll)
    {
        for (int i = 0; i < children(); ++i)
        {
            Fl_Widget *c = child(i);

            long long ud = (long long) c->user_data();
            
            if(ud == UD_delay_group)
            {
                RKR_Group *g_group = static_cast<RKR_Group *>(c);
                
                g_group->resize
                (
                    (g_group->get_start_x() + m_start_x) * W_ratio,
                    ((g_group->get_start_y() + m_start_y) + (30 * i)),   // 30 is height of RKR_Group
                //    ((g_group->get_start_y() + m_start_y) + (30 * i)) * H_ratio,   // 30 is height of RKR_Group
                    (g_group->get_start_width()) * W_ratio ,
                    g_group->get_start_height()
                //    g_group->get_start_height() * H_ratio
                );
            }
        }
    }
    else    // settings/MIDI
    {
        for (int i = 0; i < children(); ++i)
        {
            Fl_Widget *c = child(i);

            long long ud = (long long) c->user_data();

            // This finds the bank and preset choice widgets
            // UD_Bank_Used_Start = 1000 to 1127 >> UD_Preset_Used_End = 2000 to 2127
            if((ud >= UD_Bank_Used_Start) && (ud <= UD_Preset_Used_End))
            {
                RKR_Choice *c_choice = static_cast<RKR_Choice *> (c);

                c_choice->resize
                (
                    (c_choice->get_start_x()* W_ratio),
                    (c_choice->get_start_y() + m_start_y - 20) * H_ratio,   // 20 is height of RKR_Choice
                    (c_choice->get_start_width()) * W_ratio ,
                    c_choice->get_start_height() * H_ratio
                );
            }
            else if(ud == UD_Bank_Number)
            {
                RKR_Box *c_box = static_cast<RKR_Box *> (c);
                c_box->resize
                (
                    (c_box->get_start_x() + m_start_x )* W_ratio,
                    (c_box->get_start_y() + m_start_y - 20) * H_ratio,      // 20 is height of RKR_Choice
                    (c_box->get_start_width() * W_ratio),
                    c_box->get_start_height() * H_ratio
                );
            }
        }
    }

    Fl_Scroll::resize(X, Y, W, H);
}
