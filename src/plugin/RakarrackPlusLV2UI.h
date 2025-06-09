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

/* 
 * File:   RakarrackPlusUI.h
 * Author: sspresto
 *
 * Created on May 31, 2025, 4:22 AM
 */

#ifndef RAKARRACKPLUSLV2_UI_H
#define RAKARRACKPLUSLV2_UI_H

#include <string>
#include <cassert>
#include <functional>
#include <memory>

#include "external-ui.h"
#include "../UI/rakarrack.h"

extern RKR *g_rkrplus;

class RakarrackPlusLV2UI : public LV2_External_UI_Widget
{
    std::string plugin_human_id;
    std::function<void()> notify_on_GUI_close;
    std::unique_ptr<RKRGUI> r_gui;
    bool is_shown;

    static RakarrackPlusLV2UI& self(void* handle) { assert(handle); return * static_cast<RakarrackPlusLV2UI *>(handle); }

public:
    static LV2UI_Handle	instantiate(const LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget, const LV2_Feature *const *features);
    static void cleanup(LV2UI_Handle ui);
    static void port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t /*buffer_size*/, uint32_t format, const void * buffer);
    static const void *extension_data(const char *uri);

    RakarrackPlusLV2UI(const char*, LV2UI_Write_Function, LV2UI_Controller, LV2UI_Widget*, LV2_Feature const *const *);
    ~RakarrackPlusLV2UI();
    void shutDownGUI();
    bool init();
    void run();
    void show();
    void hide();
    static void callback_Run (LV2_External_UI_Widget* ui){ self(ui).run();  }
    static void callback_Show(LV2_External_UI_Widget* ui){ self(ui).show(); }
    static void callback_Hide(LV2_External_UI_Widget* ui){ self(ui).hide(); }
    static int callback_IdleInterface(LV2_Handle ui){ self(ui).run(); return 0; }
    static int callback_ShowInterface(LV2_Handle ui){ self(ui).show(); return 0; }
    static int callback_HideInterface(LV2_Handle ui){ self(ui).hide(); return 0; }

public:
    using CallbackGuiClosed = std::function<void()>;
    void installGuiClosedCallback(CallbackGuiClosed callback)
    {
        callbackGuiClosed = std::move(callback);
    }
private:
    CallbackGuiClosed callbackGuiClosed;

};


#endif /* RAKARRACKPLUSLV2_UI_H */

