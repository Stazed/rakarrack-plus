/*
  Renamed from sliderW.h on 4/18/2020 by stazed.

  RKR_Slider.h

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

#ifndef RKR_Slider_h
#define RKR_Slider_h

#include <FL/Fl.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include "../global.h"

class RKR_Slider : public Fl_Value_Slider {
public:
    RKR_Slider(int X, int Y, int W, int H, const char *label = 0);
    int handle(int event);
    int handle2(int event, int X, int Y, int W, int H);
    void draw();
    void resize(int,int,int,int);
    void shut_off_pixel_adjustment(){m_use_pixel_adjustment = false;};
    void set_label_offset(int offset){m_label_offset = offset;};
    void set_text_offset(int offset){m_text_offset = offset;};

private:

    int m_label_offset;
    int m_text_offset;
    int m_start_width;
    int m_start_height;
    int m_look_changed;
    bool m_use_pixel_adjustment;
    
    void font_resize(int,int);
};
#endif
