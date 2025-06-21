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
 * File:   Reset_Window.cpp
 * Author: sspresto
 * 
 * Created on June 18, 2025, 6:46 PM
 */

#include <unistd.h> // usleep()
#include <FL/Fl_Pixmap.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

#include "Reset_Window.h"

static Fl_Window * g_reset_window = 0;

static void
window_cb( Fl_Widget *, void * )
{
    // don't allow user to close the window with X box because scan will
    // continue and we lose the modal. Make them use the cancel button.
    return;
}

static void
reset_timeout( void* )
{
    g_reset_window->redraw ( );
    g_reset_window->show ( );

    Fl::repeat_timeout ( 0.03f, &reset_timeout );
}

Reset_Window::Reset_Window(RKR * _rkr, Pixmap pixmap):
    m_process( _rkr ),
    m_box( nullptr ),
    m_pixmap( pixmap )
{
    g_reset_window = new Fl_Window ( 600, 60, "Resetting" );
    g_reset_window->icon((char *) m_pixmap);
    m_box = new Fl_Box ( 20, 10, 560, 40, "Re-initializing effects please wait..." );
    m_box->box ( FL_UP_BOX );
    m_box->labelsize ( 12 );
    m_box->labelfont ( FL_BOLD );
    m_box->show ( );

    g_reset_window->callback ( (Fl_Callback*) window_cb );
    g_reset_window->end ( );
    g_reset_window->set_modal ( );
}

Reset_Window::~Reset_Window()
{
}

void
Reset_Window::close_reset_window( )
{
    Fl::remove_timeout ( &reset_timeout );
    g_reset_window->hide ( );
    delete g_reset_window;
    g_reset_window = 0;
}

bool
Reset_Window::show_reset_window ()
{
    Fl::add_timeout ( 0.03f, &reset_timeout );

    if(!run_reset())
    {
        close_reset_window();
        return false;
    }

    close_reset_window();

    return true;
}

bool
Reset_Window::run_reset()
{
    m_process->Re_init_in_progress = 1;
    m_process->reset_everything();
    while(m_process->Re_init_in_progress)
    {
        Fl::check ( );
        usleep(2000);
    }

    m_process->reset_join_thread();

    return true;
}