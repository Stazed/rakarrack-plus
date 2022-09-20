/*
  scope.cxx

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
  
  This class is the scope from the main window that is activated by clicking above
  the tuner.
*/

#include "scope.h"
#include "rakarrack.h"

Scope::Scope(int x, int y, int w, int h, const char *label) : Fl_Box(x, y, w, h, label)
{
    Scope_ON = false;
}

void Scope::init(float *smpsl, float *smpsr, int PERIOD, RKRGUI *_rgui)
{
    spl = smpsl;
    spr = smpsr;
    ns = PERIOD;
    m_parent = _rgui;
}

void Scope::draw()
{
    int ox = x(), oy = y(), lx = w(), ly = h();
    int Xl, Xr, Yl, Yr;
    int SW, SH;
    double pP = (double) ns;

    SW = lx / 2 - 5;
    SH = ly;

    Xl = ox;
    Yl = oy + ly / 2;


    Xr = ox + SW + 5;
    Yr = Yl;


    double dSW = (double) SW;
    double coeff = 1.0 / pP * dSW;


    if (Scope_ON)
    {
        //Draw Curve Reponse  
        back->draw(ox, oy);

        fl_color(global_leds_color);


        int old_px = Xl;
        int old_py = Yl;
        int oldr_px = Xr;
        int oldr_py = Yr;

        for (int i = 0; i < ns; i++)
        {
            int posx = (int) ((double) i * coeff);

            double value = spl[i];
            if (value > 1.0) value = 1.0;
            if (value<-1.0) value = -1.0;


            int px = Xl + posx;
            int py = Yl + lrint(value * .5 * SH);

            // printf("%d %d %d\n",i,px,py);


            if (i > 0) fl_line(old_px, old_py, px, py);

            old_px = px;
            old_py = py;

            value = spr[i];
            if (value > 1.0) value = 1.0;
            if (value<-1.0) value = -1.0;



            px = Xr + posx;
            py = Yr + lrint(value * .5 * SH);

            if (i > 0) fl_line(oldr_px, oldr_py, px, py);

            oldr_px = px;
            oldr_py = py;
        }
    }
    else
    {
        draw_box(box(), ox, oy, lx, ly, global_back_color);
        draw_label();
    }
}

int Scope::handle(int event)
{
    if ((event == FL_PUSH) || (event == FL_DRAG) || (event == FL_RELEASE))
    {
        switch (event)
        {
        case FL_RELEASE:

            if (Scope_ON)
            {
                Scope_ON = false;
                m_parent->Tuner->show();
                m_parent->Tuner->redraw();
            }
            return (1);
            break;
        }
    }
    if(Fl::belowmouse() == this)
        Fl_Tooltip::enter(this);
    return (0);
}
