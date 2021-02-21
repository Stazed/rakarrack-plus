/*
  scope.h

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

#ifndef scope_h
#define scope_h

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Tooltip.H>
#include "../global.h"

class Scope : public Fl_Box {
public:
    Scope(int x,int y, int w, int h, const char *label=0);
    void init(float *smpsl, float *smpsr, int PERIOD, RKRGUI *_rgui);
    void draw();
    int handle(int event);
    void set_scope_ON(bool a_set){Scope_ON = a_set;}
    bool get_scope_ON(){return Scope_ON;}
private:
    RKRGUI *m_parent;
    float *spl; 
    float *spr; 
    int ns; 
    bool Scope_ON;
};
#endif
