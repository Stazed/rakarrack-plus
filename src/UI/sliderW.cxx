/*
  sliderW.cxx

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

  This class is the custom slider used by all the plugins. It creates the handle
  with LED that changes color when moved, etc.. 
 
*/

#include "sliderW.h"


SliderW::SliderW(int x, int y, int w, int h, const char *label) : Fl_Value_Slider(x, y, w, h, label)
{
    m_start_width = w;
    m_start_height = h;
    m_start_label_offset = 0;
    m_start_text_offset = 0;
    m_previous_font_size = g_default_font_size;
    this->user_data((void*)(SLIDERW_USER_DATA));
}

int SliderW::handle(int event)
{
    if (event == FL_PUSH && Fl::visible_focus())
    {
        Fl::focus(this);
        redraw();
    }

    int sxx = x(), syy = y(), sww = w(), shh = h();
    int bww = w(), bhh = h();
    
    if (horizontal())
    {
        bww = w() * .30;        // value box width (magic number % of total width)
        sxx += bww;             // adjust slider x location based on above box width
        sww -= bww;             // reduce slider width by box width
    }
    else
    {
        bhh = h() * .18;        // value box height (magic number % of total height)
        syy += bhh;             // adjust slider y location based on above box height
        shh -= bhh;             // reduce slider height by box height
    }

    return handle2(event,
                   sxx + Fl::box_dx(box()),
                   syy + Fl::box_dy(box()),
                   sww - Fl::box_dw(box()),
                   shh - Fl::box_dh(box()));
}

int SliderW::handle2(int event, int X, int Y, int W, int H)
{
    int mul = 1;
    switch (event)
    {
    case FL_PUSH:
    case FL_DRAG:
    {
        if (event == FL_PUSH)
        {
            if (!Fl::event_inside(X, Y, W, H)) return 0;
            handle_push();
        }
        
        if (Fl::event_button() == 3) return 1;
        double val;
        if (minimum() == maximum())
            val = 0.5;
        else
        {
            val = (value() - minimum()) / (maximum() - minimum());
            if (val > 1.0) val = 1.0;
            else if (val < 0.0) val = 0.0;
        }

        int ww = (horizontal() ? W : H);
        int mx = (horizontal() ? Fl::event_x() - X : Fl::event_y() - Y);
        int S = 0;
        static int offcenter;


        S = int(0 * ww + .5);
        if (S >= ww) return 0;
        int T = (horizontal() ? H : W) / 2 + 1;
        T += 4;
        if (S < T) S = T;
        if (event == FL_PUSH)
        {
            int xx = int(val * (ww - S) + .5);

            offcenter = mx - xx;
            if (offcenter < 0) offcenter = 0;
            else if (offcenter > S) offcenter = S;
            else return 1;
        }


        int xx = mx - offcenter;
        double v;
        char tryAgain = 1;
        while (tryAgain)
        {
            tryAgain = 0;
            if (xx < 0)
            {
                xx = 0;
                offcenter = mx;
                if (offcenter < 0) offcenter = 0;
            }
            else if (xx > (ww - S))
            {
                xx = ww - S;
                offcenter = mx - xx;
                if (offcenter > S) offcenter = S;
            }
            v = round(xx * (maximum() - minimum()) / (ww - S) + minimum());


            // make sure a click outside the sliderbar moves it:
            if (event == FL_PUSH && v == value())
            {
                offcenter = S / 2;
                event = FL_DRAG;
                tryAgain = 1;
            }
        }
        handle_drag(clamp(v));
    }
    return 1;

    case FL_MOUSEWHEEL:

        if (Fl::e_dy == 0) return 0;
        handle_push();
        handle_drag(clamp(increment(value(), Fl::e_dy)));
        handle_release();
        return 1;

    case FL_RELEASE:

        handle_release();
        return 1;
        
    case FL_KEYBOARD:
        if (Fl::event_state(FL_SHIFT)) mul = 10;
        else
            if (Fl::event_state(FL_CTRL)) mul = 100;
        else
            mul = 1;

        switch (Fl::event_key())
        {
        case FL_Up:
            if (horizontal()) return 0;
            handle_push();
            handle_drag(clamp(increment(value(), -1 * mul)));
            handle_release();
            return 1;
        case FL_Down:
            if (horizontal()) return 0;
            handle_push();
            handle_drag(clamp(increment(value(), 1 * mul)));
            handle_release();
            return 1;
        case FL_Left:
            if (!horizontal()) return 0;
            handle_push();
            handle_drag(clamp(increment(value(), -1 * mul)));
            handle_release();
            return 1;
        case FL_Right:
            if (!horizontal()) return 0;
            handle_push();
            handle_drag(clamp(increment(value(), 1 * mul)));
            handle_release();
            return 1;
        default:
            return 0;
        }
        // break not required because of switch...
    case FL_FOCUS:
    case FL_UNFOCUS:
        if (Fl::visible_focus())
        {
            redraw();
            return 1;
        }
        else return 0;
    case FL_ENTER:
    case FL_LEAVE:
        return 1;
    default:
        return 0;
    }
}

void SliderW::draw()
{
    /* To update the font size if user changes the value in settings */
    if(g_default_font_size != m_previous_font_size)
    {
        m_previous_font_size = g_default_font_size;
        font_resize(x(), y(), w(), h());
    }

    int X, Y, W, H;
    int sxx = x(), syy = y(), sww = w(), shh = h();
    int bxx = x(), byy = y(), bww = w(), bhh = h();
    when(FL_WHEN_RELEASE_ALWAYS | FL_WHEN_CHANGED);

    if (horizontal())
    {
        bww = w() * .30;        // value box width (magic number % of total width)
        sxx += bww;             // adjust slider x location based on above box width
        sww -= bww;             // reduce slider width by box width
    }
    else
    {
        bhh = h() * .18;        // value box height (magic number % of total height)
        syy += bhh;             // adjust slider y location based on above box height
        shh -= bhh;             // reduce slider height by box height
    }

    //  if (damage()&FL_DAMAGE_ALL) draw_box(box(),sxx,syy,sww,shh,back_color);

    X = sxx + Fl::box_dx(box());
    Y = syy + Fl::box_dy(box());
    W = sww - Fl::box_dw(box());
    H = shh - Fl::box_dh(box());


    double val;
    if (minimum() == maximum())
    {
        val = 0.5;
    }
    else
    {
        val = (value() - minimum()) / (maximum() - minimum());
        if (val > 1.0) val = 1.0;
        else if (val < 0.0) val = 0.0;
    }

    int ww = (horizontal() ? W : H);
    int xx, S;

    S = int(.25 * ww + .5) + 1;
    int T = (horizontal() ? H : W) / 2 + 1;
    T += 12;


    xx = int(val * (ww - S) + .5);

    int xsl, ysl, wsl, hsl;
    if (horizontal())
    {
        xsl = X + xx;
        wsl = S - 4;
        ysl = Y;
        hsl = H - 1;
    }
    else
    {
        ysl = Y + xx;
        hsl = S;
        xsl = X;
        wsl = W - 1;
    }


    fl_push_clip(X, Y, W, H);
    back->draw(X, Y);
    fl_pop_clip();

    Fl_Color black = active_r() ? FL_FOREGROUND_COLOR : FL_INACTIVE_COLOR;

    if (type() == FL_VERT_NICE_SLIDER)
    {
        draw_box(FL_THIN_DOWN_BOX, X + W / 2 - 2, Y, 4, H, black);
    }
    else if (type() == FL_HOR_NICE_SLIDER)
    {
        draw_box(FL_THIN_DOWN_BOX, X, Y + H / 2 - 2, W - 4, 4, black);
    }


    //Line to the knob --- I dont like 

    /*
   
     if (type() == FL_VERT_NICE_SLIDER) {
       draw_box(FL_THIN_DOWN_BOX, X+W/2-2, ysl, 3,Y+H-ysl , fl_darker(leds_color));
      }
      else
      {
        draw_box(FL_THIN_DOWN_BOX, X, Y+H/2-2,xsl-X,3, fl_darker(leds_color));
      }  
    
     */


    Fl_Color juan, pepe, luis;
    float vval = fabsf((float) val);

    juan = fl_color_average(fl_darker(fl_darker(leds_color)), fl_lighter(fl_lighter(leds_color)), vval);
    pepe = fl_color_average(fl_lighter(fl_lighter(leds_color)), fl_darker(fl_darker(leds_color)), vval);

    Fl_Boxtype box1 = slider();

    if (!box1)
    {
        box1 = (Fl_Boxtype) (box()&-2);
        if (!box1) box1 = FL_UP_BOX;
    }

    if (type() == FL_VERT_NICE_SLIDER)
    {

        draw_box(box1, xsl, ysl, wsl, hsl, fore_color);
        int d = (hsl - 6) / 2;

        draw_box(FL_THIN_DOWN_BOX, xsl + 2, ysl + d, wsl - 4, hsl - 2 * d, juan);
    }
    else if (type() == FL_HOR_NICE_SLIDER)
    {
        draw_box(box1, xsl, ysl, wsl, hsl, fore_color);
        int d = (wsl - 6) / 2;
        draw_box(FL_THIN_DOWN_BOX, xsl + d, ysl + 2, wsl - 2 * d, hsl - 4, pepe);
    }


    labelcolor(label_color);
    draw_label(xsl, ysl, wsl, hsl);

    if (Fl::focus() == this)
    {
        draw_focus(box1, xsl, ysl, wsl, hsl);
        luis = leds_color;
    }
    else luis = textcolor();

    /*  if (( Fl::scheme_) && (strcmp(Fl::scheme_, "plastic")==0)) 
      {
    
    
       if (type() == FL_HOR_NICE_SLIDER)
         back->draw(bxx+2,byy);
       
       else
         back->draw(bxx,byy); 
    
       }
       else
        back->draw(bxx,byy);
     */

    /* The slider value text amount */
    int datasize = textsize();
/*    if (textsize() < 16)
    {
        datasize = textsize();
    }
    else
    {
        datasize = 16;
    }*/

    char buf[128];
    format(buf);
    fl_font(textfont(), datasize);
    
    if ((Fl::scheme_) && (strcmp(Fl::scheme_, "plastic") == 0))
    {
        fl_color(active_r() ? leds_color : fl_inactive(textcolor()));
    }
    else
    {
        fl_color(active_r() ? luis : fl_inactive(textcolor()));
    }
    
    /* The value box and amount */
    fl_draw(buf, bxx, byy, bww, bhh, FL_ALIGN_CLIP, back);
    fl_draw(buf, bxx, byy, bww, bhh, FL_ALIGN_CLIP);
}

void SliderW::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(X, Y, W, H);
    
    Fl_Valuator::resize(X, Y, W, H);
}

void SliderW::font_resize(int X, int Y, int W, int H)
{
    float W_ratio = (float) W / m_start_width;
    float H_ratio = (float) H / m_start_height;
    float resize_ratio = (W_ratio < H_ratio) ? W_ratio : H_ratio;
    
    int font_size = g_default_font_size + m_start_label_offset;
    int adjusted_label_size = (float) (font_size * resize_ratio);
    
    labelsize(adjusted_label_size);
    
    int text_font_size = g_default_font_size + m_start_text_offset;
    int adjusted_text_size = (float) (text_font_size * resize_ratio);
    
    textsize(adjusted_text_size);
}