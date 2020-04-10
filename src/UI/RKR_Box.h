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

#define BOX_USER_DATA   99997
#define BOX_LED_DATA    5

class RKR_Box : public Fl_Box
{
public:
    RKR_Box(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    void set_font_adjustment(){m_need_font_update = true;}
    int m_start_font_offset;
private:
    
    int m_start_height;
    int m_start_width;
    bool m_need_font_update;
    void font_resize(int,int,int,int);

};

#endif /* RKR_BOX_H */

