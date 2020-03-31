/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
    if(Fl::event_button()== 3)
    {
        if (Fl::event_inside(x(), y(), w(), h()))
        {
            if (event == FL_RELEASE  || event == FL_PUSH || event == FL_DRAG)
            {
                /* The callback will trigger MIDI learn based on 3 */
                do_callback();
            }
        }
        
        /* Ignore all other button 3 events */
        return 1;
    }
    
#if 0   // This is not working - need focus
    if(event == FL_KEYBOARD)
    {
        //printf("FL_KEYBOARD\n");
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
                //printf("FL_up\n");
                handle_drag(clamp(increment(value(), -1 * mul)));
                handle_release();
                return 1;
            case FL_Down:
                handle_drag(clamp(increment(value(), 1 * mul)));
                handle_release();
                return 1;
            default:
            return 0;
        }     
    }
#endif // 0
    
    //printf("Get here event = %d\n", event);
    
    /* Normal default event handling */
    return Fl_Value_Input::handle(event);
}

