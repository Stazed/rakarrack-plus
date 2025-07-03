/*
  rakarrack-plus - a guitar effects software

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

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/x.H>
#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>
#include "../UI/rakarrack.h"
#include "RakarrackPlusLV2.h"
#include "RakarrackPlusLV2UI.h"



RakarrackPlusLV2UI::RakarrackPlusLV2UI(const char*, LV2_Feature const *const * features) :
    m_RKR(NULL),
    r_gui(NULL),
    parentXWindow(NULL),
    is_shown(false),
    is_active(false)
{
    while (*features)
    {
        LV2_Feature const* f = *features;
        if (strcmp(f->URI, LV2_INSTANCE_ACCESS_URI) == 0)
        {
            _RKRPLUSLV2 * plug = (static_cast<_RKRPLUSLV2 *>(f->data));
            m_RKR = plug->rkrplus;
           // printf("GOT M_RKR = %p\n", m_RKR);
        }
        ++features;
    }
}

RakarrackPlusLV2UI::~RakarrackPlusLV2UI()
{
    // cleanup does it
}

bool RakarrackPlusLV2UI::init()
{
    if(m_RKR)
        r_gui = new RKRGUI(0, NULL, m_RKR);
    else
    {
       // printf("Global g_rkrplus pointer is NULL %p\n", m_RKR);
        return false;
    }

    if(!r_gui)
    {
        printf("R_GUI is NULL\n");
        return false;
    }

    return true;
}

int RakarrackPlusLV2UI::idle(LV2UI_Handle /*handle*/)
{
    Fl::check();
    Fl::flush();
    return 0;
}

int RakarrackPlusLV2UI::resize_func(LV2UI_Feature_Handle handle, int w, int h)
{
    RakarrackPlusLV2UI *self = static_cast<RakarrackPlusLV2UI *>(handle);
    if(self && w>0 && h>0)
    {
        self->r_gui->Principal->size(w,h);
    }

    return 0;
}

LV2UI_Handle RakarrackPlusLV2UI::instantiate(const struct LV2UI_Descriptor * /*descriptor*/,
		const char * plugin_uri,
		const char * bundle_path,
		LV2UI_Write_Function /*write_function*/,
		LV2UI_Controller /*controller*/,
		LV2UI_Widget * widget,
		const LV2_Feature * const * features) 
{
    if (strcmp(plugin_uri, RAKARRACK_PLUS_LV2_URI) != 0)
    {
        fprintf(stderr, "RAKARRACK_PLUS_LV2_URI error: this GUI does not support plugin with URI %s\n", plugin_uri);
        return NULL;
    }

    RakarrackPlusLV2UI* uiinst = new RakarrackPlusLV2UI(bundle_path, features);
    if (uiinst->init())
    {
        LV2UI_Resize* resize = NULL;
        for (int i = 0; features[i]; ++i)
        {
            if (!strcmp(features[i]->URI, LV2_UI__parent)) 
            {
               uiinst->parentXWindow = features[i]->data;
            }
            else if (!strcmp(features[i]->URI, LV2_UI__resize)) 
            {
               resize = (LV2UI_Resize*)features[i]->data;
            }
        }
        fl_open_display();
        // set host to change size of the window
        if (resize)
        {
            resize->ui_resize(resize->handle, uiinst->r_gui->Principal->w(), uiinst->r_gui->Principal->h());
        }
        fl_embed( uiinst->r_gui->Principal ,(Window)uiinst->parentXWindow);
        *widget = (LV2UI_Widget)fl_xid_(uiinst->r_gui->Principal);

        uiinst->m_RKR->Gui_Shown = 1;
        uiinst->thread_check_xwindow();

        return static_cast<LV2UI_Handle>(uiinst);
    }
    else
        delete uiinst;

    return NULL;
}

void RakarrackPlusLV2UI::cleanup(LV2UI_Handle handle)
{
    RakarrackPlusLV2UI *self = static_cast<RakarrackPlusLV2UI *>(handle);
    self->join_thread_xwindow();
    self->r_gui->LV2_gui_hide();    // This does not hide the Principal (embedded) window.
    self->r_gui->Principal->hide(); // Muse seems to need this...
    Fl::check();
    Fl::flush();
    delete self->r_gui->Principal;
    delete self->r_gui;
    delete self;
}

LV2UI_Idle_Interface rakarrack_plus_idle_interface_desc =
{
    RakarrackPlusLV2UI::idle_iface,
};

LV2UI_Resize rakarrack_plus_resize_ui_desc =
{
    RakarrackPlusLV2UI::resize_ui,
};

const void *RakarrackPlusLV2UI::extension_data(const char *uri)
{
    if (strcmp(uri, LV2_UI__idleInterface) == 0)
    {
        return &rakarrack_plus_idle_interface_desc;
    }

    if (strcmp(uri, LV2_UI__resize) == 0)
    {
        return &rakarrack_plus_resize_ui_desc;
    }

    return NULL;
}

static const LV2UI_Descriptor rakarrack_plus_descriptor = {
    RAKARRACK_PLUS_LV2_UI_URI,
    RakarrackPlusLV2UI::instantiate,
    RakarrackPlusLV2UI::cleanup,
    NULL,
    RakarrackPlusLV2UI::extension_data
};


LV2_SYMBOL_EXPORT 
const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) 
{
    switch (index)
    {
        case 0:
            return &rakarrack_plus_descriptor;
        default:
            return NULL;
    }
}

static void* check_xwindow_status(void * _RGUI)
{
    RakarrackPlusLV2UI * self = (RakarrackPlusLV2UI *) _RGUI;

    while (self->is_active)
    {
        Display* display = XOpenDisplay(nullptr);

        if (!display)
        {
            printf("Could not open display\n");
            return 0;
        }

        Window root_window = (Window) self->parentXWindow;

        XWindowAttributes attrs;
        if (XGetWindowAttributes(display, root_window, &attrs) == 0)
        {
            printf("Could not get XWindow attributes\n");
            XCloseDisplay(display);
            return 0;
        }

        if (attrs.map_state == IsViewable)
        {
            // The window is mapped (shown)
            // Note: IsViewable means it's mapped and all its ancestors are mapped.
            if(!self->is_shown)
            {
//                printf("Showing GUI\n");
                self->is_shown = true;
                self->r_gui->LV2_gui_show();
            }
        } else if (attrs.map_state == IsUnmapped)
        {
            // The window is unmapped (hidden)
            // Is triggered by user closing with the X box or host
            if(self->is_shown)
            {
//                printf("GUI is hidden\n");
                self->is_shown = false;
                self->r_gui->LV2_gui_hide();
                Fl::check();
                Fl::flush();
            }

        } else if (attrs.map_state == IsUnviewable)
        {
            // The window is mapped but an ancestor is unmapped, so it's not viewable.
            printf("Window is UN-viewable\n");
        }

        XCloseDisplay(display);
        usleep(1500);
    }

    return 0;
}

void
RakarrackPlusLV2UI::thread_check_xwindow()
{
    is_active = true;
    int result = pthread_create(&t_Xwin, nullptr, check_xwindow_status, this);
    if(result != 0)
    {
        m_RKR->Handle_Message (52, "pthread_create - at thread_check_xwindow().");
    }
}

void
RakarrackPlusLV2UI::join_thread_xwindow()
{
    if(t_Xwin)
    {
        is_active = false;
        int result = pthread_join(t_Xwin, nullptr);
        if(result != 0)
        {
            m_RKR->Handle_Message (52, "pthread_join - at join_thread_xwindow().");
        }
    }
}