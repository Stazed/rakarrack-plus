

#include <FL/Fl.H>

#include "RKR_Input.h"


int RKR_Input::handle(int event)
{
    switch (event)
    {

    case FL_FOCUS:
        switch (Fl::event_key())
        {
        case FL_Right:
            position(0);
            break;
        case FL_Left:
            position(size());
            break;
        case FL_Down:
        case FL_Up:
        case FL_Tab:
            position(size(), 0);
            break;
        default:
            position(position(), mark()); // turns off the saved up/down arrow position
            break;
        }
        break;

    case FL_KEYBOARD:
        /* 
         * https://stackoverflow.com/questions/40284104/fltk-fl-value-input-subclass-does-not-receive-fl-keydown-events-only-fl-keyup
         * According to above, the RKR_Value_Input subclass in our case will not get FL_KEYDOWN events.
         * So this case of right and left keys is the reason for this class. To send to parent class via return 0;
         * We want the value input to use the same keys as the sliderW class for keyboard input.
         */
        switch (Fl::event_key())
        {
        case FL_Right:
        case FL_Left:
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

