/*
  RKR_Scroll.h

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
 * File:   RKR_Scroll.h
 * Author: sspresto
 *
 * Created on April 15, 2020, 8:16 AM
 */

#ifndef RKR_SCROLL_H
#define RKR_SCROLL_H

#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>

class RKR_Scroll : public Fl_Scroll
{
public:
    RKR_Scroll(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    void set_start_height(int H){m_start_height = H;};
    void set_start_width(int W){m_start_width = W;};
    int get_start_x(){return m_start_x;};
    int get_start_y(){return m_start_y;};
    int get_start_width(){return m_start_width;};
    int get_start_height(){return m_start_height;};
    void set_delay_scroll(){m_delay_scroll = true;};
    
private:

    int m_start_x;
    int m_start_y;
    int m_start_width;
    int m_start_height;
    int m_look_changed;
    bool m_delay_scroll;
};

#endif /* RKR_SCROLL_H */

