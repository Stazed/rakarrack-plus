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

#include <thread>
#include <unistd.h>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>

#include "Reset_Window.h"

static Fl_Window * g_reset_window = 0;
static bool _reset_complete = false;

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

Reset_Window::Reset_Window():
    _box( nullptr )
{
    g_reset_window = new Fl_Window ( 720, 60, "Resetting" );
    _box = new Fl_Box ( 20, 10, 560, 40, "Re-initializing effects please wait..." );
    _box->box ( FL_UP_BOX );
    _box->labelsize ( 12 );
    _box->labelfont ( FL_BOLD );
    _box->show ( );

    g_reset_window->callback ( (Fl_Callback*) window_cb );
    g_reset_window->end ( );
    g_reset_window->set_modal ( );
}

void Reset_Window::initialize(RKR * _rkr) {
  m_process = _rkr;
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