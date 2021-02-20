/*
  RKR_GUI_Effect.h

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
 * File:   RKR_GUI_Effect.h
 * Author: sspresto
 *
 * Created on February 12, 2021, 11:21 AM
 */

#ifndef RKR_GUI_EFFECT_H
#define RKR_GUI_EFFECT_H

#include "RKR_Light_Button.h"
#include "RKR_Choice.h"

#include "../global.h"

class RKR_Gui_Effect : public Fl_Group
{
public:
    RKR_Gui_Effect(int X, int Y, int W, int H, const char *L);
    
    void initialize(RKR *_rkr, RKRGUI *_rgui);
    /**
     * Refresh gui parameter changes from MIDI control and preset changes.
     * 
     * @param 
     *      The individual parameter to change
     */
    virtual void parameter_refresh(int) = 0;
    
    /**
     * All effects that apply tap tempo should override this function.
     */
    virtual void tap_tempo_update() {};

    RKR* m_process; 
    RKRGUI* m_parent; 
    RKR_Light_Button *activate_effect;
    RKR_Choice *preset_choice;

};

#endif /* RKR_GUI_EFFECT_H */

