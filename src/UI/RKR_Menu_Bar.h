/*
  RKR_Menu_Bar.h

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
 * File:   RKR_Menu_Bar.h
 * Author: sspresto
 *
 * Created on April 11, 2020, 8:25 AM
 */

#ifndef RKR_MENU_BAR_H
#define RKR_MENU_BAR_H

#include <vector>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_draw.H>


class RKR_Menu_Bar : public Fl_Menu_Bar
{
public:
    RKR_Menu_Bar(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    void set_submenu_paths(const std::vector<std::string> &submenu){m_submenu_paths = submenu;};
    void set_label_offset(int offset){m_label_offset = offset;};

private:

    int m_label_offset;
    int m_start_width;
    int m_start_height;
    int m_look_changed;
    std::vector<std::string> m_submenu_paths;
    void font_resize(int,int);

};

#endif /* RKR_MENU_H */

