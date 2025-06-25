/*
  analyzer.cxx

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
  
  This class is the analyzer that is activated by clicking below the "Put Order in your Rack" button
  in the main window Presets section.
*/

#include "analyzer.h"
#include "rakarrack.h"

const int C_DEFAULT_SAMPLES = 1024;
const double C_DEFAULT_ADJUST = 0.125;

Analyzer::Analyzer(int x, int y, int w, int h, const char *label) :
    Fl_Box(x, y, w, h, label),
    m_parent(NULL),
    spl(NULL),
    spr(NULL),
    sr(44100), 
    ns(256),
    Analyzer_ON(false),
    sample_adjustment(C_DEFAULT_ADJUST)
{
}

void Analyzer::init(float *smpsl, float *smpsr, int PERIOD, int SAMPLERATE, RKRGUI *_rgui)
{
    spl = smpsl;
    spr = smpsr;
    ns = PERIOD;
    sr = SAMPLERATE;
    m_parent = _rgui;

    /* This adjusts the drawing based on period size. Otherwise the smaller period, lower height, etc...*/
    if(ns)
        sample_adjustment = C_DEFAULT_ADJUST * (double(C_DEFAULT_SAMPLES) / double(ns));
}

void Analyzer::draw()
{
    int ox = x(), oy = y(), lx = w(), ly = h();
    int px;
    double nsp = 1.0 / (double) ns;
    double dsr = (double) sr;
    double udsr = 1.0 / dsr;
    double acoeff = udsr / (0.005 + udsr);
    double factor = (double) ns / 64.0;
    char buf[4];
    const double xscale[] = {22.0, 31.0, 39.0, 62.0, 79.0, 125.0, 158.0, 200.0, 251.0, 317.0, 400.0,
                       503.0, 634.0, 800.0, 1000.0, 1200.0, 1500.0, 2000.0, 2500.0, 3200.0, 4000.0,
                        5000.0, 6000.0, 8000.0, 10000.0, 12000.0, 16000.0, 20000.0};

    memset(buf, 0, sizeof (buf));
    px = (lx - 22) / 29;


    double scale = (double) ly;

    if (Analyzer_ON)
    {
        fl_draw(buf, ox, oy, lx, ly, FL_ALIGN_CLIP, back);

        fl_color(global_leds_color);

        for (int i = 0; i < 28; i++)
        {
            double y = 0.0;
            double coeff = xscale[i] * udsr;
            double oldimage = 0.0;

            for (int j = 0; j < ns; j++)
            {
                double t = cos(D_PI * (double) j * coeff);
                double image = t * ((spl[j] + spr[j]) * sample_adjustment) ;
                oldimage = acoeff * image + (1.0 - acoeff) * oldimage;

                y += fabs(oldimage);
            }

            y = sqrt((double) i * y * nsp * factor);

            if (y < 0.0) y = 0.0;
            if (y > 1.0) y = 1.0;

            int py = lrint(y * scale);


            fl_color(global_leds_color);
            fl_rectf(px + ox + px * i + 4, oy + ly - py, px - 2, py);
        }
    }
    else
    {
        draw_box(box(), ox, oy, lx, ly, global_back_color);
        draw_label();
    }
}

int Analyzer::handle(int event)
{
    if ((event == FL_PUSH) || (event == FL_DRAG) || (event == FL_RELEASE))
    {
        switch (event)
        {
        case FL_RELEASE:

            if (Analyzer_ON)    // toggle
            {
                Analyzer_ON = false;
                m_parent->Etit->show();
                m_parent->Etit->redraw();
            }
            return (1);
            break;
        }
    }
    if(Fl::belowmouse() == this)
        Fl_Tooltip::enter(this);
    return (0);
}
