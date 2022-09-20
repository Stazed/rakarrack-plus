/*
  RKR_Choice.cxx

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
 * File:   RKR_Choice.cxx
 * Author: sspresto
 * 
 * Created on April 9, 2020, 7:17 PM
 */

#include "RKR_Choice.h"
#include "../global.h"

RKR_Choice::RKR_Choice(int X, int Y, int W, int H, const char *label) :
    Fl_Choice(X, Y, W, H, label),
    m_label_offset(0),      //  C_DEFAULT_FONT_SIZE
    m_text_offset(0),       //  C_DEFAULT_FONT_SIZE
    m_start_x(X),
    m_start_y(Y),
    m_start_width(W),
    m_start_height(H),
    m_look_changed(0)
{
    this->user_data((void*)(UD_RKR_Highlight));
}

void RKR_Choice::draw()
{
    if(m_look_changed != global_look_changed)
    {
        m_look_changed = global_look_changed;

        color(global_fore_color);
        labelcolor(global_label_color);
        textcolor(global_label_color);
        labelfont(global_font_type);
        font_resize(w(), h());
    }

    Fl_Boxtype btype = Fl::scheme() ? FL_UP_BOX // non-default uses up box
            : FL_DOWN_BOX; // default scheme uses down box
    int dx = Fl::box_dx(btype);
    int dy = Fl::box_dy(btype);

    // Arrow area
    int H = h() - 2 * dy;

#if 1
    int W = H;  // Always shrink
#else
    int W = Fl::is_scheme("gtk+") ? 20 : // gtk+  -- fixed size
            Fl::is_scheme("gleam") ? 20 : // gleam -- fixed size
            Fl::is_scheme("plastic") ? ((H > 20) ? 20 : H) // plastic: shrink if H<20
            : ((H > 20) ? 20 : H); // default: shrink if H<20
#endif // 1
    int X = x() + w() - W - dx;
    int Y = y() + dy;

    // Arrow object
    int w1 = (W - 4) / 3;
    if (w1 < 1) w1 = 1;
    int x1 = X + (W - 2 * w1 - 1) / 2;
    int y1 = Y + (H - w1 - 1) / 2;

    if (Fl::scheme())
    {
        // NON-DEFAULT SCHEME

        // Draw widget box
        draw_box(btype, color());

        // Draw arrow area
        fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
        if (Fl::is_scheme("plastic"))
        {
            // Show larger up/down arrows...
            fl_polygon(x1, y1 + 3, x1 + w1, y1 + w1 + 3, x1 + 2 * w1, y1 + 3); // Up arrow
            fl_polygon(x1, y1 + 1, x1 + w1, y1 - w1 + 1, x1 + 2 * w1, y1 + 1); // Down arrow
        }
        else    // Gtk and Gleam 
        {
#if 1
            /* Draw Box and single down arrow, looks better to me */
            draw_box(FL_UP_BOX, X, Y, W, H, color());
            fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
            fl_polygon(x1, y1, x1 + w1, y1 + w1, x1 + 2 * w1, y1); // Down arrow
#else
            // Show smaller up/down arrows with a divider...
            x1 = x() + w() - 13 - dx;
            y1 = y() + h() / 2;
            fl_polygon(x1, y1 - 2, x1 + 3, y1 - 5, x1 + 6, y1 - 2); // Up arrow
            fl_polygon(x1, y1 + 2, x1 + 3, y1 + 5, x1 + 6, y1 + 2); // Down arrow

            /* Divider - two lines with different shading */
            fl_color(fl_darker(color()));
            fl_yxline(x1 - 7, y1 - 8, y1 + 8);

            fl_color(fl_lighter(color()));
            fl_yxline(x1 - 6, y1 - 8, y1 + 8);
#endif // 0
        }
    }
    else
    {
        // DEFAULT SCHEME

        // Draw widget box
        if (fl_contrast(textcolor(), FL_BACKGROUND2_COLOR) == textcolor())
        {
            draw_box(btype, FL_BACKGROUND2_COLOR);
        }
        else
        {
            draw_box(btype, fl_lighter(color()));
        }

        // Draw arrow area
        draw_box(FL_UP_BOX, X, Y, W, H, color());
        fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
        fl_polygon(x1, y1, x1 + w1, y1 + w1, x1 + 2 * w1, y1); // Down arrow
    }

    W += 2 * dx;

    // Draw menu item's label
    if (mvalue())
    {
        Fl_Menu_Item m = *mvalue();
        if (active_r()) m.activate();
        else m.deactivate();

        // Clip
        int xx = x() + dx, yy = y() + dy + 1, ww = w() - W, hh = H - 2;
        fl_push_clip(xx, yy, ww, hh);

        if (Fl::scheme())
        {
            Fl_Label l;
            l.value = m.text;
            l.image = 0;
            l.deimage = 0;
            l.type = m.labeltype_;
            l.font = m.labelsize_ || m.labelfont_ ? m.labelfont_ : textfont();
            l.size = m.labelsize_ ? m.labelsize_ : textsize();
            l.color = m.labelcolor_ ? m.labelcolor_ : textcolor();
            if (!m.active()) l.color = fl_inactive((Fl_Color) l.color);
            fl_draw_shortcut = 2; // hack value to make '&' disappear
            l.draw(xx + 3, yy, ww > 6 ? ww - 6 : 0, hh, FL_ALIGN_LEFT);
            fl_draw_shortcut = 0;
            if (Fl::focus() == this) draw_focus(box(), xx, yy, ww, hh);
        }
        else
        {
            fl_draw_shortcut = 2; // hack value to make '&' disappear
            m.draw(xx, yy, ww, hh, this, Fl::focus() == this);
            fl_draw_shortcut = 0;
        }

        fl_pop_clip();
    }

    // Widget's label
    draw_label();
}

int RKR_Choice::handle(int event)
{
    /* Right mouse button - pop up MIDI learn */
    if(Fl::event_button()== FL_RIGHT_MOUSE)
    {
        if (Fl::event_inside(x(), y(), w(), h()))
        {
            if (event == FL_RELEASE /* || event == FL_PUSH || event == FL_DRAG*/)
            {
                /* The callback will trigger MIDI learn based on FL_RIGHT_MOUSE */
                do_callback();
            }
        }
        
        /* Ignore all other right mouse events */
        return 1;
    }
    
    /* Normal processing */
    return Fl_Choice::handle(event);
}

void RKR_Choice::font_resize(int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;

    int label_font_size = global_font_size + m_label_offset;
    int adjusted_label_size = (float) (label_font_size * resize_ratio);

    labelsize(adjusted_label_size);

    int text_font_size = global_font_size + m_text_offset;
    int adjusted_text_size = (float) (text_font_size * resize_ratio);

    Fl_Menu_Item *m = (Fl_Menu_Item*) menu();

    if (!m)
    {
        return;
    }

    for (int s = 0; s < m->size(); s++)
    {
        Fl_Menu_Item *p = m->next(s);
        p->labelsize(adjusted_text_size); /* Drop down menus - menu list items */
        p->labelfont (global_font_type);
    }

    textsize(adjusted_text_size);
}

void RKR_Choice::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(W, H);

    Fl_Choice::resize(X, Y, W, H);
}
