/*
  RKR_Value_Input.h

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
  This override class had been modified from the original Fl_Value_Input class to 
  allow for keyboard input handling and auto resizing of fonts.
  Copied from fltk-1.3.4-1.
 */


/* \file
   RKR_Value_Input widget . */

#ifndef RKR_Value_Input_H
#define RKR_Value_Input_H

#include <FL/Fl_Valuator.H>
#include "RKR_Key_Input.h"


/**
  The RKR_Value_Input widget displays a numeric value.
  The user can click in the text field and edit it - there is in
  fact a hidden Fl_Input widget with
  type(FL_FLOAT_INPUT) or type(FL_INT_INPUT) in
  there - and when they hit return or tab the value updates to
  what they typed and the callback is done.
  
  <P>If step() is non-zero and integral, then the range of numbers
  is limited to integers instead of floating point numbers. As
  well as displaying the value as an integer, typed input is also
  limited to integer values, even if the hidden Fl_Input widget
  is of type(FL_FLOAT_INPUT).</P>
  
  <P>If step() is non-zero, the user can also drag the
  mouse across the object and thus slide the value. The left
  button moves one step() per pixel, the middle by 10
 * step(), and the right button by 100 * step(). It
  is therefore impossible to select text by dragging across it,
  although clicking can still move the insertion cursor.</P>
  
  <P>If step() is non-zero and integral, then the range
  of numbers are limited to integers instead of floating point
  values.
  
  <P ALIGN="CENTER">\image html RKR_Value_Input.png 
  \image latex  RKR_Value_Input.png "RKR_Value_Input" width=4cm
 */
class RKR_Value_Input : public Fl_Valuator
{
public:
    /* This is the encapsulated Fl_input attribute to which 
    this class delegates the value font, color and shortcut.
    The RKR_Key_Input override class hijacks all keyboard events
    and sends them to this parent class for special key usage. */
    RKR_Key_Input input;
private:
    char soft_;
    static void input_cb(Fl_Widget*, void*);
    virtual void value_damage(); // cause damage() due to value() changing
public:
    int handle(int);
protected:
    void draw();
private:

    /* The font resize variables added to this class for rakarrack+ */
    int m_label_offset;
    int m_text_offset;
    int m_start_width;
    int m_start_height;
    int m_look_changed;
    int is_redraw;
    void font_resize(int, int);
    
public:
    void set_label_offset(int offset){m_label_offset = offset;};
    void set_text_offset(int offset){m_text_offset = offset;};
    /* End of RKR rakarrack+ modifications */

    void resize(int, int, int, int);
    RKR_Value_Input(int x, int y, int w, int h, const char *l = 0);
    ~RKR_Value_Input();

    /** See void RKR_Value_Input::soft(char s) */
    void soft(char s)
    {
        soft_ = s;
    }

    /**
      If "soft" is turned on, the user is allowed to drag
      the value outside the range. If they drag the value to one of
      the ends, let go, then grab again and continue to drag, they can
      get to any value. The default is true.
     */
    char soft() const
    {
        return soft_;
    }

    /**
     Returns the current shortcut key for the Input.
     \see RKR_Value_Input::shortcut(int) 
     */
    int shortcut() const
    {
        return input.shortcut();
    }

    /** 
     Sets the shortcut key to \p s. Setting this
     overrides the use of '&' in the label().  The value is a bitwise
     OR of a key and a set of shift flags, for example FL_ALT | 'a'
     , FL_ALT | (FL_F + 10), or just 'a'.  A value
     of 0 disables the shortcut.
   
     The key can be any value returned by 
     Fl::event_key(), but will usually be an ASCII letter.  Use
     a lower-case letter unless you require the shift key to be held down.
   
     The shift flags can be any set of values accepted by 
     Fl::event_state().  If the bit is on that shift key must
     be pushed.  Meta, Alt, Ctrl, and Shift must be off if they are not in
     the shift flags (zero for the other bits indicates a "don't care"
     setting).
     */
    void shortcut(int s)
    {
        input.shortcut(s);
    }

    /** Gets the typeface of the text in the value box.  */
    Fl_Font textfont() const
    {
        return input.textfont();
    }

    /** Sets the typeface of the text in the value box.  */
    void textfont(Fl_Font s)
    {
        input.textfont(s);
    }

    /** Gets the size of the text in the value box.  */
    Fl_Fontsize textsize() const
    {
        return input.textsize();
    }

    /** Sets the size of the text in the value box.  */
    void textsize(Fl_Fontsize s)
    {
        input.textsize(s);
    }

    /** Gets the color of the text in the value box.  */
    Fl_Color textcolor() const
    {
        return input.textcolor();
    }

    /** Sets the color of the text in the value box.*/
    void textcolor(Fl_Color n)
    {
        input.textcolor(n);
    }

    /** Gets the color of the text cursor. The text cursor is black by default. */
    Fl_Color cursor_color() const
    {
        return input.cursor_color();
    }

    /** Sets the color of the text cursor. The text cursor is black by default. */
    void cursor_color(Fl_Color n)
    {
        input.cursor_color(n);
    }

};

#endif

//
// End of "$Id: RKR_Value_Input.H 8864 2011-07-19 04:49:30Z greg.ercolano $".
//
