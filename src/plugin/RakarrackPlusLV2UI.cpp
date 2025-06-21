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
    m_RKR(0),
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
    r_gui.reset();
}

bool RakarrackPlusLV2UI::init()
{
    if(m_RKR)
        r_gui.reset(new RKRGUI(0, NULL, m_RKR));
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

void RakarrackPlusLV2UI::run()
{
    Fl::check();
    Fl::flush();
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
        void* parentXwindow = 0;
        for (int i = 0; features[i]; ++i)
        {
            if (!strcmp(features[i]->URI, LV2_UI__parent)) 
            {
               parentXwindow = features[i]->data;
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
        fl_embed( uiinst->r_gui->Principal ,(Window)parentXwindow);
        *widget = (LV2UI_Widget)fl_xid_(uiinst->r_gui->Principal);

        return static_cast<LV2UI_Handle>(uiinst);
    }
    else
        delete uiinst;

    return NULL;
}

void RakarrackPlusLV2UI::cleanup(LV2UI_Handle ui)
{
    printf("CLEANUP CALLED\n");
    RakarrackPlusLV2UI *self = static_cast<RakarrackPlusLV2UI *>(ui);
    self->r_gui->LV2_gui_hide();
    delete self->r_gui->Principal;
    self->r_gui.reset();
    delete self;
}

LV2UI_Idle_Interface rakarrack_plus_idle_interface_desc =
{
    RakarrackPlusLV2UI::callback_IdleInterface,
};

const void *RakarrackPlusLV2UI::extension_data(const char *uri)
{
    if (strcmp(uri, LV2_UI__idleInterface) == 0) {
        return &rakarrack_plus_idle_interface_desc;
    }
    return nullptr;
}

#if 0
void RakarrackPlusLV2UI::port_event(LV2UI_Handle ui,
    uint32_t port_index,
    uint32_t /*buffer_size*/,
    uint32_t format,
    const void * buffer)
{
   // printf("GOT PORT EVENT\n");
}
#endif

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
