/*
  RKR_Choice.h

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
 * File:   RKR_Choice.h
 * Author: sspresto
 *
 * Created on April 9, 2020, 7:17 PM
 */

#ifndef RKR_CHOICE_H
#define RKR_CHOICE_H

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>

class RKR_Choice : public Fl_Choice
{
public:
    RKR_Choice(int X, int Y, int W, int H, const char *label=0);
    void draw();
    int handle(int);
    void resize(int,int,int,int);
    void font_resize(int,int);
    int get_width(){return w();};
    int get_height(){return h();};
    int get_start_height(){return m_start_height;};
    int get_start_width(){return m_start_width;};
    int get_start_x(){return m_start_x;};
    int get_start_y(){return m_start_y;};
    void set_label_offset(int offset){m_label_offset = offset;};
    void set_text_offset(int offset){m_text_offset = offset;};

private:

    int m_label_offset;
    int m_text_offset;
    int m_start_x;
    int m_start_y;
    int m_start_width;
    int m_start_height;
    int m_look_changed;
};

#endif /* RKR_CHOICE_H */

