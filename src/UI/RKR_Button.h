/*
  RKR_Button.h

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
 * File:   RKR_Button.h
 * Author: sspresto
 *
 * Created on April 8, 2020, 9:46 PM
 */

#ifndef RKR_BUTTON_H
#define RKR_BUTTON_H

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>


#define BUTTON_DEFAULT          0
#define BUTTON_BANK_HIGHLIGHT   1
#define BUTTON_MIDI_GET         2
#define BUTTON_RKR_LABEL        3
#define BUTTON_BANK_RESET       4

class RKR_Button : public Fl_Button
{
public:
    RKR_Button(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    void font_resize(int,int);
    int get_width(){return w();};
    int get_height(){return h();};
    void set_start_width(int W) {m_start_width = W;};
    void set_start_height(int H) {m_start_height = H;};
    void set_label_offset(int offset) {m_label_offset = offset;};
    void set_button_type(int type) {m_button_type = type;};
    void set_highlight_preset(int highlight) {m_bank_highlight_preset = highlight;};
    void set_bank_under_mouse(int mouse) {m_bank_under_mouse = mouse;};

private:

    int m_label_offset;
    int m_start_width;
    int m_start_height;
    int m_bank_highlight_preset;
    int m_button_type;
    int m_look_changed;
    int m_bank_under_mouse;
};

#endif /* RKR_BUTTON_H */

