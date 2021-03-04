/*
  RKR_Fl_Slider.h

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
 * File:   RKR_Fl_Slider.h
 * Author: sspresto
 *
 * Created on March 3, 2021, 7:54 PM
 */

#ifndef RKR_FL_SLIDER_H
#define RKR_FL_SLIDER_H

#include <FL/Fl.H>
#include <FL/Fl_Slider.H>
#include <FL/fl_draw.H>
#include "../global.h"

class RKR_Fl_Slider : public Fl_Slider
{
public:
    RKR_Fl_Slider(int X, int Y, int W, int H, const char *label = 0);
    void draw();
private:

    int m_look_changed;
};

#endif /* RKR_FL_SLIDER_H */

