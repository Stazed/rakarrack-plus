/*
  RKR_Browser.h

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
 * File:   RKR_Browser.h
 * Author: sspresto
 *
 * Created on April 13, 2020, 8:02 PM
 */

#ifndef RKR_BROWSER_H
#define RKR_BROWSER_H

#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_draw.H>

class RKR_Browser : public Fl_Browser
{
public:
    RKR_Browser(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    int m_start_label_offset;
    int m_start_text_offset;
private:
    
    int m_start_height;
    int m_start_width;
    int m_previous_font_size;
    void font_resize(int,int);

};

#endif /* RKR_BROWSER_H */

