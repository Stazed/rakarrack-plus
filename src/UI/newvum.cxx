/*
  newvum.h

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
  
  This class creates the volume meters used in the main window In/Out section, vocoder aux in,
  and ACI (Analog Control In - in Settings/ACI).

*/

#include "newvum.h"

NewVum::NewVum(int x, int y, int w, int h, const char *label) : Fl_Slider(x, y, w, h, label)
{
}

void NewVum::draw_bg(int X, int Y, int W, int H)
{
    fl_push_clip(X, Y, W, H);
    //  draw_box(FL_THIN_DOWN_BOX,X,Y,W,H,global_back_color);
    back->draw(X, Y);

    fl_pop_clip();

    //Fl_Color black = active_r() ? FL_FOREGROUND_COLOR : FL_INACTIVE_COLOR;
}

void NewVum::draw_rest(int X, int Y, int W, int H)
{
    int x = X;
    int y = Y;
    int h = H;
    int w = W;
    int S, S1, S2, S3;
    int ko;
    double val;


    double fred = .239f;
    double fora = .35f;
    double fyel = .508f;


    val = 1.0 - ((value() + 48.0) *.015873016);
    if (val > 1.0) val = 1.0;
    else if (val < 0.0) val = 0.0;


    S = h - lrint(val * h + .5);

    S1 = h - lrint(fred * (double) h + .5f);
    S2 = h - lrint(fora * (double) h + .5f);
    S3 = h - lrint(fyel * (double) h + .5f);


    draw_bg(X, Y, W, H);

    ko = H + Y - S;

    if (val < fred)
    {
        fl_color(fl_darker(FL_RED));
        fl_rectf(x + 1, h + y - S, 3, S - S1);
        fl_color(FL_RED);
        fl_rectf(x + 4, h + y - S, w - 7, S - S1);
        fl_color(fl_darker(FL_RED));
        fl_rectf(x + w - 3, h + y - S, 2, S - S1);

        S = S1;
    }

    if (val < fora)
    {
        fl_color(fl_darker((Fl_Color) 93));
        fl_rectf(x + 1, h + y - S, 3, S - S2);
        fl_color((Fl_Color) 93);
        fl_rectf(x + 4, h + y - S, w - 7, S - S2);
        fl_color(fl_darker((Fl_Color) 93));
        fl_rectf(x + w - 3, h + y - S, 2, S - S2);

        S = S2;
    }

    if (val < fyel)
    {
        fl_color(fl_darker(FL_YELLOW));
        fl_rectf(x + 1, h + y - S, 3, S - S3);
        fl_color(FL_YELLOW);
        fl_rectf(x + 4, h + y - S, w - 7, S - S3);
        fl_color(fl_darker(FL_YELLOW));
        fl_rectf(x + w - 3, h + y - S, 2, S - S3);


        S = S3;
    }


    fl_color(fl_darker(FL_GREEN));
    fl_rectf(x + 1, h + y - S, 3, S);
    fl_color(FL_GREEN);
    fl_rectf(x + 4, h + y - S, w - 7, S);
    fl_color(fl_darker(FL_GREEN));
    fl_rectf(x + w - 3, h + y - S, 2, S);


    if (val < 1.0)
    {
        fl_color(FL_BLACK);

        for (int i = H + Y - 4; i > ko; i = i - 4)
            fl_line(x, i, x + w - 1, i);

    }
}

void NewVum::draw()
{
    if (damage() & FL_DAMAGE_ALL) draw_box();

    draw_rest(x() + Fl::box_dx(box()),
              y() + Fl::box_dy(box()),
              w() - Fl::box_dw(box()),
              h() - Fl::box_dh(box()));
}

int NewVum::handle(int event)
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
