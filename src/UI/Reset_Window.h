/*
  Rakarrack-plus - a guitar effects software

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

/* 
 * File:   Reset_Window.h
 * Author: sspresto
 *
 * Created on June 18, 2025, 6:46 PM
 */

#ifndef RESET_WINDOW_H
#define RESET_WINDOW_H

#include <X11/X.h>

#include "../process.h"

class Reset_Window {
public:
    Reset_Window(RKR * _rkr, Pixmap pixmap);
    Reset_Window(const Reset_Window&) = delete;
    Reset_Window & operator=(const Reset_Window&) = delete;
    virtual ~Reset_Window();

    bool show_reset_window ();
    void close_reset_window( );
private:

    RKR* m_process; 
    Fl_Box* m_box;
    Pixmap m_pixmap;
    bool run_reset();
};

#endif /* RESET_WINDOW_H */

