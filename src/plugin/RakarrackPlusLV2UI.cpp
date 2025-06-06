#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/x.H>

//#include "sustain_ui.h"
#include "../UI/rakarrack.h"
#include "RakarrackPlusLV2.h"
#include "RakarrackPlusLV2UI.h"


RKR * g_rkrplus;    // external


RakarrackPlusLV2UI::RakarrackPlusLV2UI(const char*, LV2UI_Write_Function, LV2UI_Controller controller,
        LV2UI_Widget* widget, LV2_Feature const *const * features) :
    plugin_human_id{"Rakarrack-plus lv2 plugin"},
    notify_on_GUI_close{},
    r_gui(NULL),
    is_shown(false)
{
    // Configure callbacks for running the UI
    LV2_External_UI_Widget::run  = RakarrackPlusLV2UI::callback_Run;
    LV2_External_UI_Widget::show = RakarrackPlusLV2UI::callback_Show;
    LV2_External_UI_Widget::hide = RakarrackPlusLV2UI::callback_Hide;

    while (*features)
    {
        LV2_Feature const* f = *features;
        if (strcmp(f->URI, LV2_EXTERNAL_UI__Host) == 0)
        {
            LV2_External_UI_Host& hostSpec = * static_cast<LV2_External_UI_Host *>(f->data);
            plugin_human_id = hostSpec.plugin_human_id;
            auto callback = hostSpec.ui_closed;
            notify_on_GUI_close = [callback,controller]{ callback(controller); };
        }
        ++features;
    }

    // this object also serves as »widget« for the event callbacks
    *widget = static_cast<LV2UI_Widget>(this);
}

RakarrackPlusLV2UI::~RakarrackPlusLV2UI()
{
    // nothing - cleanup does it
}

bool RakarrackPlusLV2UI::init()
{
    if(g_rkrplus)
        r_gui = new RKRGUI(0, NULL, g_rkrplus);
    else
        return false;

    if(!r_gui)
        return false;
#if 0   // TODO
    r_gui->installGuiClosedCallback([this]
                                        {// invoked when FLTK GUI is closed explicitly...
                                            RakarrackPlusLV2UI::nothing();    // could do something if we wanted...
                                            if (notify_on_GUI_close)
                                                notify_on_GUI_close();
                                        }); 
#endif

    return true;
}

void RakarrackPlusLV2UI::run()
{
    if (is_shown)
    {
       /* if(r_gui->m_process->m_sus->m_midi_control)
        {
            r_gui->get_parameters();
            r_gui->m_process->m_sus->m_midi_control = false;
        }
        */
        r_gui->run();
        Fl::check();
        Fl::flush();
    }
    else if (notify_on_GUI_close)
        notify_on_GUI_close();
}

void RakarrackPlusLV2UI::show()
{
    printf("SHOW CALLED\n");

    if(!is_shown)
    {
        r_gui->Principal->show();
        is_shown = true;

        // TODO
        // Update paramters
       // r_gui->get_parameters();
    }
}

void RakarrackPlusLV2UI::hide()
{
    printf("HIDING CALLED\n");

    is_shown = false;
//    r_gui->save_current_state(0);   // TODO
//    r_gui->is_bank_modified();
//    r_gui->is_PG_table_modified();  
    r_gui->Principal->hide();
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
        return static_cast<LV2UI_Handle>(uiinst);
    }
    else
        delete uiinst;

    return NULL;
}

void RakarrackPlusLV2UI::cleanup(LV2UI_Handle ui)
{
    // TODO
    printf("CLEANUP CALLED\n");
    RakarrackPlusLV2UI *self = (RakarrackPlusLV2UI*)ui;
//    delete self->r_gui->ui;
    delete self->r_gui->Principal;
    delete self->r_gui;
    delete self;
}

void RakarrackPlusLV2UI::port_event(LV2UI_Handle ui,
    uint32_t port_index,
    uint32_t /*buffer_size*/,
    uint32_t format,
    const void * buffer)
{
   // printf("GOT PORT EVENT\n");
}

#ifdef LV2_X11_EMBEDED
int RakarrackPlusLV2UI::idle(LV2UI_Handle handle)
{
    SustainUI* self = (SustainUI*)handle;
    self->idle();

    return 0;
}
#endif

const void* RakarrackPlusLV2UI::extension_data(const char* uri)
{
#ifdef LV2_X11_EMBEDED
    if (!strcmp(uri, LV2_UI__idleInterface))
    {
        return &idle_iface;
    }
    if (!strcmp(uri, LV2_UI__resize))
    {
        return &resize_ui;
    }
#endif
    return NULL;
}

static const LV2UI_Descriptor rakarrack_plus_descriptor = {
    RAKARRACK_PLUS_LV2_UI_URI,
    RakarrackPlusLV2UI::instantiate,
    RakarrackPlusLV2UI::cleanup,
    RakarrackPlusLV2UI::port_event,
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

