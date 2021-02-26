/*
  RKR_Tabs.h

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
 * File:   RKR_Tabs.h
 * Author: sspresto
 *
 * Created on April 15, 2020, 5:05 AM
 */

#ifndef RKR_TABS_H
#define RKR_TABS_H

#include <FL/Fl.H>
#include <FL/Fl_Tabs.H>

class RKR_Tabs : public Fl_Tabs
{
public:
    RKR_Tabs(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    void set_label_offset(int offset){m_label_offset = offset;};

private:

    int m_label_offset;
    int m_start_width;
    int m_start_height;
    int m_look_changed;
    void font_resize(int,int);
};

#endif /* RKR_TABS_H */

