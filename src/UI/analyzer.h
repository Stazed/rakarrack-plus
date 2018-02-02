/*
  analyzer.h 

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


#ifndef analyzer_h
#define analyzer_h

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tiled_Image.H>
#include <FL/fl_draw.H>
#include "../global.h"

class Analyzer : public Fl_Box {
public:
    Analyzer(int x,int y, int w, int h, const char *label=0);
    void init(float *smpsl, float *smpsr, int PERIOD, int SAMPLERATE);
    void draw();
    int handle(int event);
    void set_analyzer_ON(bool a_set){Analyzer_ON = a_set;}
    bool get_analyzer_ON(){return Analyzer_ON;}
    void background_image_change (Fl_Tiled_Image *a_back);
    void leds_color_change (Fl_Color a_leds_color);
    void background_color_change(Fl_Color a_back_color);
    
private:
    float *spl; 
    float *spr; 
    int sr; 
    int ns; 
    bool Analyzer_ON; 
    Fl_Tiled_Image *back;
    Fl_Color leds_color;
    Fl_Color back_color; 
};
#endif
