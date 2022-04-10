/*
  RKR_Box.h

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
 * File:   RKR_Box.h
 * Author: sspresto
 *
 * Created on April 8, 2020, 12:29 PM
 */

#ifndef RKR_BOX_H
#define RKR_BOX_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#define BOX_DEFAULT     0
#define BOX_LEDS        1
#define BOX_LIGHT       2

class RKR_Box : public Fl_Box
{
public:
    RKR_Box(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    int handle(int);
    int get_start_height(){return m_start_height;};
    int get_start_width(){return m_start_width;};
    int get_start_x(){return m_start_x;};
    int get_start_y(){return m_start_y;};
    void set_label_offset(int offset){m_label_offset = offset;};
    void set_box_type(int type) {m_box_type = type;};

private:

    int m_label_offset;
    int m_start_x;
    int m_start_y;
    int m_start_width;
    int m_start_height;
    int m_box_type;
    int m_look_changed;
    void font_resize(int,int);

};

#endif /* RKR_BOX_H */

