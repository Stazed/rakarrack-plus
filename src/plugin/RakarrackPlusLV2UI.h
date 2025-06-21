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


#include <cassert>  // assert
#include <memory>   // std::unique_ptr

#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include "../UI/rakarrack.h"
#include "../process.h"


class RakarrackPlusLV2UI
{
    std::unique_ptr<RKRGUI> r_gui;
    RKR * m_RKR;
    bool is_shown;

    static RakarrackPlusLV2UI& self(void* handle) { assert(handle); return * static_cast<RakarrackPlusLV2UI *>(handle); }

public:
    RakarrackPlusLV2UI(const char*, LV2UI_Write_Function, LV2UI_Controller, LV2UI_Widget*, LV2_Feature const *const *);
    ~RakarrackPlusLV2UI();
    static LV2UI_Handle	instantiate(const LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget, const LV2_Feature *const *features);
    static void cleanup(LV2UI_Handle ui);
//    static void port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t /*buffer_size*/, uint32_t format, const void * buffer);
    static const void *extension_data(const char *uri);
    bool init();
    void run();
    static int idle(LV2UI_Handle handle);
    static int resize_func(LV2UI_Feature_Handle handle, int w, int h);
    static constexpr LV2UI_Idle_Interface idle_iface = { RakarrackPlusLV2UI::idle };
    static constexpr LV2UI_Resize resize_ui = { 0, RakarrackPlusLV2UI::resize_func };
    static int callback_IdleInterface(LV2_Handle ui){ self(ui).run(); return 0; }
private:

};


#endif /* RAKARRACKPLUSLV2_UI_H */

