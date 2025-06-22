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



RakarrackPlusLV2UI::RakarrackPlusLV2UI(const char*, LV2UI_Write_Function, LV2UI_Controller controller,
        LV2UI_Widget* widget, LV2_Feature const *const * features) :
    r_gui(NULL),
    m_RKR(NULL),
    parentXWindow(NULL),
    is_shown(false)
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
#if 0
    // this object also serves as »widget« for the event callbacks
    *widget = static_cast<LV2UI_Widget>(this);
#endif
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

int RakarrackPlusLV2UI::idle(LV2UI_Handle handle)
{
    RakarrackPlusLV2UI *self = static_cast<RakarrackPlusLV2UI *>(handle);

 
    Display* display = XOpenDisplay(nullptr);
    
    if (!display)
    {
        // Handle error: could not open display
        return 1;
    }
    
    Window xid = (Window) self->parentXWindow;
    
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, xid, &attrs) == 0) {
        // Handle error: could not get window attributes (e.g., invalid XID)
        XCloseDisplay(display);
        return 1;
    }

    bool is_hidden = false;
    
    if (attrs.map_state == IsViewable)
    {
        // The window is mapped (shown)
        // Note: IsViewable means it's mapped and all its ancestors are mapped.
//        printf("Window is viewable\n");
        if(!self->is_shown)
        {
            self->is_shown = true;
            self->r_gui->LV2_gui_show(1);   // FIXME
            //  self->r_gui->update_gui_from_LV2_hide();
        }

    } else if (attrs.map_state == IsUnmapped)
    {
        // The window is unmapped (hidden)
        // Is triggered by user closing with the X box on the window
        printf("Window is hidden\n");
        bool is_hidden = true;
        self->is_shown = false;
        self->r_gui->LV2_gui_hide();
        
    } else if (attrs.map_state == IsUnviewable)
    {
        // The window is mapped but an ancestor is unmapped, so it's not viewable.
        printf("Window is UN-viewable\n");
    }
    
    XCloseDisplay(display);

    if(is_hidden)
        return 1;

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

    printf("RESIZE\n");
    return 0;
}

LV2UI_Handle RakarrackPlusLV2UI::instantiate(const struct LV2UI_Descriptor * /*descriptor*/,
		const char * plugin_uri,
		const char * bundle_path,
		LV2UI_Write_Function write_function,
		LV2UI_Controller controller,
		LV2UI_Widget * widget,
		const LV2_Feature * const * features) 
{
    if (strcmp(plugin_uri, RAKARRACK_PLUS_LV2_URI) != 0)
    {
        fprintf(stderr, "RAKARRACK_PLUS_LV2_URI error: this GUI does not support plugin with URI %s\n", plugin_uri);
        return NULL;
    }

    RakarrackPlusLV2UI* uiinst = new RakarrackPlusLV2UI(bundle_path, write_function, controller, widget, features);
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

        return static_cast<LV2UI_Handle>(uiinst);
    }
    else
        delete uiinst;

    return NULL;
}

void RakarrackPlusLV2UI::cleanup(LV2UI_Handle handle)
{
    printf("CLEANUP CALLED\n");
    RakarrackPlusLV2UI *self = static_cast<RakarrackPlusLV2UI *>(handle);
    self->r_gui->LV2_gui_hide();
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
    //    case 1:
    //        return &stuckstackerUI_descriptor;
        default:
            return NULL;
    }
}
