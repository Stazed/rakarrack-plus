/*
  inputW.cxx

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

  This class is the custom Fl_Value_Input used by some plugins (Infinity).
  Allows for right click MIDI learn pop up.
 
*/

/* 
 * File:   inputW.cxx
 * Author: sspresto
 * 
 * Created on March 30, 2020, 7:39 PM
 */

#include "inputW.h"

inputW::inputW(int x, int y, int w, int h, const char *label) : Fl_Value_Input(x, y, w, h, label)
{
}

int
inputW::handle(int event)
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
        
        /* Ignore all other button 3 events */
        return 1;
    }

#if 0
/*
    The FL_KEYUP approach will not work either, since key up is never set as handled
    when the key down is used. So, any FL_Up or FL_Down from any other widget, slider, etc
    that is used for navigation, the FL_KEYUP eventually will get to this widget and adjust the value.
    Bummer....
 */

    /* Need to handle focus to get keyboard events */
    if (event == FL_FOCUS || event == FL_UNFOCUS)
    {
        return 1;   // says we handed it
    }
    
/* 
 * https://stackoverflow.com/questions/40284104/fltk-fl-value-input-subclass-does-not-receive-fl-keydown-events-only-fl-keyup
 * According to above, the Fl_Value_Input subclass in our case will not get FL_KEYDOWN events.
 * So just gonna use FL_KEYUP, which is crappy and does not allow key repeat.
 */
    
    if(event == FL_KEYUP)
    {
        int mul = 1;
        if (Fl::event_state(FL_SHIFT))
        {
            mul = 10;
        }
        else if (Fl::event_state(FL_CTRL))
        {
            mul = 100;
        }
        
        switch (Fl::event_key())
        {
            case FL_Up:
                handle_drag(clamp(increment(value(), 1 * mul)));
                return 1;
            case FL_Down:
                handle_drag(clamp(increment(value(), -1 * mul)));
                return 1;
            default:
            return 0;
        }     
    }
#endif // 0
    
    /* Normal default event handling */
    return Fl_Value_Input::handle(event);
}

