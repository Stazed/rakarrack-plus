/*
  RKR_Input.cxx

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

#include <FL/Fl.H>

#include "RKR_Input.h"


int RKR_Input::handle(int event)
{
    switch (event)
    {
    case FL_KEYBOARD:
        /* 
         * https://stackoverflow.com/questions/40284104/fltk-fl-value-input-subclass-does-not-receive-fl-keydown-events-only-fl-keyup
         * According to above, the RKR_Value_Input subclass in our case will not get FL_KEYDOWN events.
         * So this case of right and left keys is the reason for this class. To send to parent class via return 0;
         * We want the value input to use the same keys as the RKR_Slider class for keyboard input.
         */
        switch (Fl::event_key())
        {
        /* Send all keyboard events to the parent */
        default:
            return 0;
        }
    }
    
    return Fl_Input::handle(event);
}

/**
 Creates a new Fl_Input widget using the given position, size,
 and label string. The default boxtype is FL_DOWN_BOX.
 */
RKR_Input::RKR_Input(int X, int Y, int W, int H, const char *l)
: Fl_Input(X, Y, W, H, l)
{
}

