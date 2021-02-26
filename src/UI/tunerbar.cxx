/*
  tunerbar.cxx

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
 
  This class draws the tuning bar used by the tuner.

*/

#include "tunerbar.h"

TunerBar::TunerBar(int x, int y, int w, int h, const char *label) : Fl_Slider(x, y, w, h, label)
{
}

void TunerBar::draw_bg(int X, int Y, int W, int H)
{
    fl_push_clip(X, Y, W, H);
    // draw_box(FL_THIN_DOWN_BOX,X,Y,W,H,global_back_color);

    back->draw(X, Y);
    fl_pop_clip();

    //Fl_Color black = active_r() ? FL_FOREGROUND_COLOR : FL_INACTIVE_COLOR;
}

void TunerBar::draw_rest(int X, int Y, int W, int H)
{
    int x = X;
    int y = Y;
    int h = H;
    int w = W;
    double val;

    val = (float) (value() - minimum()) / (maximum() - minimum());
    if (val > 1.0) val = 1.0;
    else if (val < 0.0) val = 0.0;

    int S = (int) ((double) w * val);

    draw_bg(X, Y, W, H);

    if ((val > .1) && (val < .9))
    {
        fl_color(fl_lighter(FL_RED));
        fl_rectf(x + S - 3, y + 2, 7, h - 4);
    }
}

void TunerBar::draw()
{
    if (damage() & FL_DAMAGE_ALL) draw_box();

    draw_rest(x() + Fl::box_dx(box()),
              y() + Fl::box_dy(box()),
              w() - Fl::box_dw(box()),
              h() - Fl::box_dh(box()));
}

int TunerBar::handle(int event)
{
    switch (event)
    {
    case FL_SHOW:


    case FL_HIDE:


    case FL_RELEASE:

    {
        return 0;
    }


    case FL_DRAG:
    {
        return 0;
    }

    case FL_SHORTCUT:
        return 0;

    default:
        return 0;
    }

    return 1;
}
