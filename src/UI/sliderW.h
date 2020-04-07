/*
  slider.h

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

#ifndef sliderW_h
#define sliderW_h

#include <FL/Fl.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include "../global.h"

#define SLIDERW_USER_DATA   99999

class SliderW : public Fl_Value_Slider {
public:
    SliderW(int x, int y, int w, int h, const char *label = 0);
    int handle(int event);
    int handle2(int event, int X, int Y, int W, int H);
    void draw();
    void resize(int,int,int,int);
private:
    
    int m_text_size;
    int m_label_size;
};
#endif
