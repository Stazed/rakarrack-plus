/*
  tunerbar.h

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

#ifndef tunerbar_h
#define tunerbar_h

#include <FL/Fl.H>
#include <FL/Fl_Slider.H>
#include <FL/fl_draw.H>
#include "../global.h"

class TunerBar : public Fl_Slider {
public:
    TunerBar(int x, int y, int w, int h, const char *label = 0);
    void draw_bg(int X, int Y, int W, int H);
    void draw_rest(int X, int Y, int W, int H);
    void draw();
    int handle(int event);
};
#endif
