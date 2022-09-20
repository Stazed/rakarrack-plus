/*
  rakarrack - a guitar effects software

  rkrprocess_gui.cxx  -  RKRGUI class definitions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/*
  The following function definitions are declared in the rakarrack.h header file. 
  Any changes to these function prototype declarations need to be changed in the 
  header using the fluid IDE since fluid generates the header. These definitions
  were moved to this new file on 2/17/18.
 */

#include "rakarrack.h"
#include "../icono_rakarrack_128x128.xpm"
#include <algorithm>    // std::sort
static Fl_Widget *previous_widget = NULL;
static int drag = C_NO_DRAG;
static int analyzer_redraw = 0;
static int grab_focus = 0;
static Pixmap p, mask;
static XWMHints *hints = NULL;

/**
 * This local RKR pointer is used exclusively with RKRGUI class static function.
 */
RKR * process_rkr;

RKRGUI::RKRGUI(int argc, char**argv, RKR *rkr_) :
    made(0),
    focus_delay_time(25),    // Every 25 count is about 1 second
    stress_test_time(0),
    random_parameters(0),
    efx_always_active(0),
    use_current_active_efx(0),
    max_random_active(6)
{
    // Initialize Gui
    Fl::args(argc, argv);
    fl_open_display();
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), (char **) icono_rakarrack_128x128, &p, &mask, NULL);

    Fl::visual(FL_DOUBLE | FL_RGB);
    fl_register_images();
    Fl::set_fonts(0);

    m_process = process_rkr = rkr_;

    back = NULL;
    
    memset(FX_Excluded, 0, sizeof (FX_Excluded));

    make_window();

    Principal->icon((char *) p);
    BankWindow->icon((char *) p);
    Order->icon((char *) p);
    Settings->icon((char *) p);
    AboutWin->icon((char *) p);
    MIDILearn->icon((char *) p);
    Trigger->icon((char *) p);
    DelayFile->icon((char *) p);

    char tmp[256];
    Analy->set_analyzer_ON(false);
    Sco->set_scope_ON(false);

    Sco->init(m_process->anall, m_process->analr, m_process->period_master, this);
    Analy->init(m_process->anall, m_process->analr, m_process->period_master, m_process->sample_rate, this);

    memset(tmp, 0, sizeof (tmp));
    if (!m_process->File_To_Load.empty())
    {
        sprintf(tmp, "Session: %s", m_process->File_To_Load.c_str ());
    }
    else
    {
        sprintf(tmp, "%s   v%s", m_process->jackcliname, VERSION);
    }

    Principal->copy_label(tmp);
    BankWin_Label(m_process->Config.BankFilename);
    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Effects Order", m_process->jackcliname, VERSION);
    Order->copy_label(tmp);
    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Settings", m_process->jackcliname, VERSION);
    Settings->copy_label(tmp);
    sprintf(tmp, "%s   v%s - MIDI Learn", m_process->jackcliname, VERSION);
    MIDILearn->copy_label(tmp);
    sprintf(tmp, "%s   v%s - ACI", m_process->jackcliname, VERSION);
    Trigger->copy_label(tmp);
    sprintf(tmp, "%s   v%s - Random", m_process->jackcliname, VERSION);
    RandomEdit->copy_label(tmp);
    
    load_previous_state();
    
    DelayFile->make_delay_window();
    Settings->make_table_window();
    Put_Loaded();
    Principal->show(argc, argv);
    put_icon(Principal);
    read_insert_presets();
    
    if (m_process->Config.deachide)
    {
        HideUE->label("Show");
    }
    else
    {
        HideUE->label("Hide");
    }
    
    HideUE->redraw();

    Fl::add_timeout(.04, this->TimeoutStatic, this);
    Fl::add_handler(global_shortcuts);
}

/**
 Main FLTK gui timeout
 */
void RKRGUI::TimeoutStatic(void* ptr)
{
    // Main gui timeout
    ((RKRGUI*) ptr)->GuiTimeout();
}

void RKRGUI::GuiTimeout(void)
{
    // Timing issue with NSM show/hide
    if(!m_process->Gui_Shown)
        return;

    // Main gui process on timeout
    below_mouse_highlight_and_focus();
    drag_effect();
    
#ifdef STRESS_TEST_CHECK
    if(stress_test_time)
    {
        if(stress_test_time > 25)
        {
            RandomPreset();
            stress_test_time = 1;
        }

        stress_test_time++;
    }
#endif

    if (m_process->Tuner_Active)
    {

        if (m_process->efx_Tuner->note_actual != m_process->note_old)
        {
            char notas[3];
            sprintf(notas, "%-2s", m_process->efx_Tuner->notes[m_process->efx_Tuner->note_actual]);
            WNote->copy_label(notas);
            m_process->note_old = m_process->efx_Tuner->note_actual;
        }

        if (m_process->efx_Tuner->nfreq != m_process->nfreq_old)
        {
            char refreq[60];
            sprintf(refreq, "%8.3f", m_process->efx_Tuner->nfreq);
            WRfreq->copy_label(refreq);
            m_process->nfreq_old = m_process->efx_Tuner->nfreq;
        }

        //if(m_process->efx_Tuner->afreq != m_process->afreq_old)
        if (fabsf(m_process->efx_Tuner->afreq - m_process->afreq_old) > .2f)
        {
            char nofreq[60];
            sprintf(nofreq, "%8.3f", m_process->efx_Tuner->afreq);
            if (abs(m_process->efx_Tuner->cents) > 5)
                WNfreq->labelcolor(FL_RED);
            else
                WNfreq->labelcolor(global_label_color);
            WNfreq->copy_label(nofreq);
            m_process->afreq_old = m_process->efx_Tuner->afreq;
        }

        if (m_process->efx_Tuner->afreq == 0.0f)
        {
            char notas[3];
            sprintf(notas, "  ");
            WNote->copy_label(notas);
        }


        if (m_process->efx_Tuner->cents != m_process->cents_old)
        {
            tuner_bar->value(m_process->efx_Tuner->cents);
            m_process->cents_old = m_process->efx_Tuner->cents;
        }
    }

    if (m_process->Gui_Refresh == GUI_Refresh_Plus_Key)
    {
        if (m_process->Selected_Preset < 60)
        {
            Preset_Counter->value(m_process->Selected_Preset + 1);
            Preset_Counter->do_callback();
        }

        m_process->Gui_Refresh = GUI_Refresh_Off;
    }


    if (m_process->Gui_Refresh == GUI_Refresh_Minus_Key)
    {
        if (m_process->Selected_Preset > 1)
        {
            Preset_Counter->value(m_process->Selected_Preset - 1);
            Preset_Counter->do_callback();
        }

        m_process->Gui_Refresh = GUI_Refresh_Off;
    }

    if (m_process->Gui_Refresh == GUI_Refresh_F2_Key)
    {
        if (Nivel_Salida->value()>-50)
        {
            Nivel_Salida->value(Nivel_Salida->value() - 1);
            Nivel_Salida->do_callback();
            Nivel_Salida->redraw();
        }

        m_process->Gui_Refresh = GUI_Refresh_Off;
    }


    if (m_process->Gui_Refresh == GUI_Refresh_F3_Key)
    {
        if (Nivel_Salida->value() < 50)
        {
            Nivel_Salida->value(Nivel_Salida->value() + 1);
            Nivel_Salida->do_callback();
            Nivel_Salida->redraw();
        }

        m_process->Gui_Refresh = GUI_Refresh_Off;
    }


    if (m_process->Gui_Refresh == GUI_Refresh_Looper)
    {
        update_looper();
        m_process->Gui_Refresh = GUI_Refresh_Off;
    }

    if (m_process->Gui_Refresh == GUI_Refresh_CTRL_S)
    {
        if( (int) m_process->Bank_Vector.size() > m_process->active_bank )
        {
            std::string filename = m_process->Bank_Vector[m_process->active_bank].Bank_File_Name;
            int ok = m_process->save_bank(filename, m_process->Bank);

            if (ok)
            {
                // Reload the bank vector for the new file or update the existing
                Scan_Bank_Dir(1);

                // Set the bank window alert message
                filename = filename.insert(0, "File Saved: ", 12);
                BankWindow->Alert_Message->copy_label (filename.c_str());
                BankWindow->Alert_Message->redraw ();

                // Start the counter for timed message clearing
                m_process->Alert_Count = 1;
            }
        }

        m_process->Gui_Refresh = GUI_Refresh_Off;
    }

    // Updates the Gui for MIDI Bank select
    if (m_process->Change_Bank != C_BANK_CHANGE_OFF)
    {
        BankWin_Label(m_process->Bank_Vector[m_process->Change_Bank].Bank_File_Name);
        Put_Loaded_Bank();
        m_process->Change_Bank = C_BANK_CHANGE_OFF;
    }
    
    if (m_process->Change_MIDI_Table != C_BANK_CHANGE_OFF)
    {
        // Put it in the scroll table
        Settings->Put_MidiTable();
        m_process->Change_MIDI_Table = C_BANK_CHANGE_OFF;
    }

    // Update the Gui for MIDI program changes
    if (m_process->Change_Preset != C_CHANGE_PRESET_OFF)
    {
        Preset_Counter->value(m_process->Change_Preset);
        Preset_Counter->do_callback();

        m_process->Change_Preset = C_CHANGE_PRESET_OFF;
    }


    MIDI_control_gui_refresh();
    if (m_process->ACI_Active)
        ActACI();

    m_process->cpufp++;
    if (m_process->cpufp == 40)
    {
        char tmp[8];
        memset(tmp, 0, sizeof (tmp));
        sprintf(tmp, "%5.2f%%", m_process->cpuload);
        CPULOAD->copy_label(tmp);
        m_process->cpufp = 0;
    }


    // Jack Port connections
    if (m_process->Jack_Port_Connnection_Changed)
    {
        if (m_process->Jack_IN_Port_Connnection_Status)
            PORT_IN_STATE->show();
        else
            PORT_IN_STATE->hide();
        if (m_process->Jack_OUT_Port_Connnection_Status)
            PORT_OUT_STATE->show();
        else
            PORT_OUT_STATE->hide();
        if (m_process->Jack_AUX_Port_Connnection_Status)
            PORT_AUX_STATE->show();
        else
            PORT_AUX_STATE->hide();
        if (m_process->Jack_MIDI_IN_Port_Connnection_Status)
            PORT_MIDI_IN_STATE->show();
        else
            PORT_MIDI_IN_STATE->hide();
        if (m_process->Jack_MIDI_OUT_Port_Connnection_Status)
            PORT_MIDI_OUT_STATE->show();
        else
            PORT_MIDI_OUT_STATE->hide();
        m_process->Jack_Port_Connnection_Changed = 0;
    }

    if (global_error_number > 0)
        m_process->Handle_Message(global_error_number);

    if (m_process->Tap_Active)
    {
        if ((m_process->Tap_Display == 1) && (m_process->Tap_Selection > 0))
        {
            T_DIS->value(m_process->Tap_TempoSet);
            update_tap_tempo_GUI();
            m_process->Tap_Display = 0;
        }

        if (m_process->Tap_Display == 2)
        {
            m_process->Tap_Display = 0;
            Tap_activar->value(0);
            Tap_activar->do_callback();
        }
    }


    if (m_process->Active_Preset.FX_Master_Active)
    {
        if (m_process->val_il_sum != m_process->old_il_sum)
        {
            input_vul->value(m_process->val_il_sum);
            m_process->old_il_sum = m_process->val_il_sum;
        }

        if (m_process->val_ir_sum != m_process->old_ir_sum)
        {
            input_vur->value(m_process->val_ir_sum);
            m_process->old_ir_sum = m_process->val_ir_sum;
        }

        if (m_process->val_vl_sum != m_process->old_vl_sum)
        {
            output_vul->value(m_process->val_vl_sum);
            m_process->old_vl_sum = m_process->val_vl_sum;
        }

        if (m_process->val_vr_sum != m_process->old_vr_sum)
        {
            output_vur->value(m_process->val_vr_sum);
            m_process->old_vr_sum = m_process->val_vr_sum;
        }


        if (m_process->efx_FLimiter->clipping)
        {
            CLIP_LED->color(FL_RED);
            CLIP_LED->redraw();
        }
        else
        {
            CLIP_LED->color(fl_darker(FL_BLUE));
            CLIP_LED->redraw();
        }

        if (m_process->efx_FLimiter->limit)
        {
            LMT_LED->color(global_leds_color);
            LMT_LED->redraw();
        }
        else
        {
            LMT_LED->color(fl_darker(global_leds_color));
            LMT_LED->redraw();
        }

        m_process->efx_FLimiter->clipping = 0;
        m_process->efx_FLimiter->limit = 0;


        if (m_process->checkforaux())
        {
            Vocoder *Efx_Vocoder = static_cast <Vocoder*>(m_process->Rack_Effects[EFX_VOCODER]);
            
            if (VOCODER->vu_vu->value() != Efx_Vocoder->vulevel)
            {
                VOCODER->vu_vu->value(Efx_Vocoder->vulevel);
            }
        }

        if (Sco->get_scope_ON())
        {
            Sco->redraw();
        }

        if (Analy->get_analyzer_ON())
        {
            analyzer_redraw++;
            if (analyzer_redraw == 3)
            {
                Analy->redraw();
                analyzer_redraw = 0;
            }
        }
        else
        {
            analyzer_redraw = 0;
        }

        if (m_process->EFX_Active[EFX_LOOPER])
        {
            Looper *Efx_Looper = static_cast <Looper*> (m_process->Rack_Effects[EFX_LOOPER]);
            if ((Efx_Looper->Pplay) && (!Efx_Looper->Pstop))
            {
                if (m_process->looper_lqua != Efx_Looper->looper_qua)
                {
                    m_process->looper_lqua = Efx_Looper->looper_qua;
                    char tmp[16];
                    memset(tmp, 0, sizeof (tmp));
                    sprintf(tmp, "%d/%d", Efx_Looper->looper_bar, Efx_Looper->looper_qua);
                    LOOPER->L_TimePos->copy_label(tmp);
                }
            }
        }
    }


    if ((m_process->Metro_Active) && (MetroSound->value() == 0))
    {
        if ((!m_process->M_Metronome->markctr) && ((int) Metro_Led->color() != (int) fl_lighter(FL_RED)))
        {
            Metro_Led->color(fl_lighter(FL_RED));
            Metro_Led->redraw();
        }

        if ((m_process->M_Metronome->markctr) && ((int) Metro_Led->color() != (int) fl_darker(FL_RED)))
        {
            Metro_Led->color(fl_darker(FL_RED));
            Metro_Led->redraw();
        }
    }


    if (m_process->MIDIConverter_Active)
    {
        if ((m_process->efx_MIDIConverter->hay) && ((int) MIDI->Mled->color() != (int) fl_lighter(FL_RED)))
        {
            MIDI->Mled->color(fl_lighter(FL_RED));
            MIDI->Mled->redraw();
        }

        if ((!m_process->efx_MIDIConverter->hay) && ((int) MIDI->Mled->color() != (int) fl_darker(FL_RED)))
        {
            MIDI->Mled->color(fl_darker(FL_RED));
            MIDI->Mled->redraw();
        }
    }


    if (m_process->EFX_Active[EFX_HARMONIZER])
    {
        Harmonizer *Efx_Harmonizer = static_cast <Harmonizer*> (m_process->Rack_Effects[EFX_HARMONIZER]);
        if ((Efx_Harmonizer->PSELECT) || (Efx_Harmonizer->PMIDI))
        {
            if (m_process->RC_Harm->cc)
            {
                HAR->har_chordname->copy_label(m_process->RC_Harm->NombreAcorde);
                m_process->RC_Harm->cc = 0;
                HAR->har_chordname->redraw();
                m_process->RC_Harm->Vamos(0, Efx_Harmonizer->Pinterval - 12, m_process->HarmRecNote->reconota);
                Efx_Harmonizer->r_ratio = m_process->RC_Harm->r__ratio[0]; //pass the found ratio
            }
        }
    }


    if (m_process->EFX_Active[EFX_STEREOHARM])
    {
        StereoHarm *Efx_StereoHarm = static_cast<StereoHarm*>(m_process->Rack_Effects[EFX_STEREOHARM]);

        if ((Efx_StereoHarm->PSELECT) || (Efx_StereoHarm->PMIDI))
        {
            if (m_process->RC_Stereo_Harm->cc)
            {
                SHAR->shar_chordname->copy_label(m_process->RC_Stereo_Harm->NombreAcorde);
                m_process->RC_Stereo_Harm->cc = 0;
                SHAR->shar_chordname->redraw();
                m_process->RC_Stereo_Harm->Vamos(1, Efx_StereoHarm->Pintervall - 12, m_process->StHarmRecNote->reconota);
                m_process->RC_Stereo_Harm->Vamos(2, Efx_StereoHarm->Pintervalr - 12, m_process->StHarmRecNote->reconota);
                Efx_StereoHarm->r_ratiol = m_process->RC_Stereo_Harm->r__ratio[1];
                Efx_StereoHarm->r_ratior = m_process->RC_Stereo_Harm->r__ratio[2];
            }
        }
    }


    if (m_process->RControl)
    {
        m_process->CountWait++;
        if (m_process->CountWait == 25)
        {
            MIDILearn->GMM->color(global_leds_color);
            MIDILearn->GMM->redraw();
        }

        if (m_process->CountWait == 50)
        {
            MIDILearn->GMM->color(global_fore_color);
            MIDILearn->GMM->redraw();
            m_process->CountWait = 0;
        }

        if (m_process->ControlGet != 0)
        {
            m_process->RControl = 0;
            MIDILearn->Disp_Control->value(m_process->ControlGet);
            MIDILearn->Disp_Control->redraw();
            MIDILearn->GMM->color(global_fore_color);
            MIDILearn->GMM->redraw();
            if ((m_process->comemouse) && (m_process->Config.autoassign))
            {
                m_process->comemouse = 0;
                MIDILearn->Assign->do_callback();
            }
        }
    };
    
    // The counter for bank window alert message to be cleared.
    if(m_process->Alert_Count)
    {
        m_process->Alert_Count++;

        if(m_process->Alert_Count > 75)     // about 3s
        {
            m_process->Alert_Count = 0;
            BankWindow->Alert_Message->copy_label ("");
            BankWindow->Alert_Message->redraw ();
        }
    }

    // Group widget deletion, addition, insert, move up/down
    if(DelayFile->m_need_update)
    {
        DelayFile->m_need_update = false;
        DelayFile->process_scroll_update();
    }

    if(m_process->change_scale)
    {
        m_process->change_scale = 0;  // false
        if (m_process->Config.scalable )
        {
            Principal->size_range(384, 216, 3840, 2160, 0, 0, 1);
        }
        else
        {
            Principal->size_range(320, 240, 3200, 2400);
        }
    }

    Fl::repeat_timeout(.04, this->TimeoutStatic, this);
}

void RKRGUI::load_default_bank(unsigned bank_index)
{
    // Check if the bank was modified - request save
    is_bank_modified();

    // Copy the bank to the process active Bank if valid
    if(!m_process->Bank_Vector.empty() && (m_process->Bank_Vector.size() > bank_index))
    {
        m_process->copy_bank(m_process->Bank, m_process->Bank_Vector[bank_index].Bank);

        m_process->active_bank = bank_index;
        BankWin_Label(m_process->Bank_Vector[bank_index].Bank_File_Name);
        Put_Loaded_Bank();
        BankWindow->unlight_preset(m_process->Selected_Preset);
    }
    else
    {
        m_process->Handle_Message(47, DATADIR);
    }
}

void RKRGUI::back_color_change(Fl_Color back_color)
{
    global_back_color = back_color;
    
    // Increment look changed flag
    global_look_changed++;
    
    if (!m_process->Config.EnableBackgroundImage)
    {
        back->color_average(global_back_color, 0.0);
    }
    
    ChangeActives();
    Fl::redraw();
}

void RKRGUI::label_color_change(Fl_Color label_color)
{
    global_label_color = label_color;

    // Increment look changed flag
    global_look_changed++;

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::buttons_color_change(Fl_Color buttons_color)
{
    global_fore_color = buttons_color;
    
    // Increment look changed flag
    global_look_changed++;

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::leds_color_change(Fl_Color leds_color)
{
    global_leds_color = leds_color;

    // Increment look changed flag
    global_look_changed++;

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::font_size_change(int font_size)
{
    global_font_size = font_size;

    // Increment look changed flag
    global_look_changed++;

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::font_type_change (int font_type)
{
    global_font_type = font_type;

    // Increment look changed flag
    global_look_changed++;

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::put_icon(Fl_Window* window)
{
    // put icon
    if (hints == NULL)
    {
        hints = XGetWMHints(fl_display, fl_xid(window));
    }

    hints->icon_pixmap = p;
    hints->icon_mask = mask;
    hints->flags = IconPixmapHint | IconMaskHint;
    XSetWMHints(fl_display, fl_xid(window), hints);
}

/**
 *  Loads the previous state when shutdown, window sizes, images, etc.
 *  From ~user/.fltk/github.com.Stazed.rakarrack.plus/rakarrack-plus.prefs
 */
void RKRGUI::load_previous_state()
{
    Principal->resize(m_process->Config.Principal_X, m_process->Config.Principal_Y,
                      m_process->Config.Principal_W, m_process->Config.Principal_H);

    BankWindow->resize(m_process->Config.BankWindow_X, m_process->Config.BankWindow_Y,
                       m_process->Config.BankWindow_W, m_process->Config.BankWindow_H);

    Order->resize(m_process->Config.Order_X, m_process->Config.Order_Y,
                  m_process->Config.Order_W, m_process->Config.Order_H);

    MIDILearn->resize(m_process->Config.MIDI_Learn_X, m_process->Config.MIDI_Learn_Y,
                      m_process->Config.MIDI_Learn_W, m_process->Config.MIDI_Learn_H);

    Trigger->resize(m_process->Config.Trigger_X, m_process->Config.Trigger_Y,
                    m_process->Config.Trigger_W, m_process->Config.Trigger_H);

    Settings->resize(m_process->Config.Settings_X, m_process->Config.Settings_Y,
                     m_process->Config.Settings_W, m_process->Config.Settings_H);
    
    RandomEdit->resize(m_process->Config.Random_X, m_process->Config.Random_Y,
                     m_process->Config.Random_W, m_process->Config.Random_H);

    DelayFile->resize(m_process->Config.Delay_X, m_process->Config.Delay_Y,
                     m_process->Config.Delay_W, m_process->Config.Delay_H);

    PutBackground();

    Settings->scheme_ch->value(m_process->Config.Schema);
    Settings->scheme_ch->do_callback();

    back_color_change((Fl_Color)m_process->Config.back_color);
    buttons_color_change((Fl_Color)m_process->Config.fore_color);
    leds_color_change((Fl_Color)m_process->Config.leds_color);
    label_color_change((Fl_Color)m_process->Config.label_color);
    font_size_change(m_process->Config.font_size);
    font_type_change(m_process->Config.font_type);

    m_process->active_bank = m_process->Config.active_bank;

    if (!m_process->Bank_Load_Command_Line)
    {
        // Copy the bank to the process active Bank
        if(m_process->active_bank < (int)m_process->Bank_Vector.size ())
        {
            m_process->copy_bank(m_process->Bank, m_process->Bank_Vector[m_process->active_bank].Bank);

            BankWin_Label(m_process->Bank_Vector[m_process->active_bank].Bank_File_Name);
            Put_Loaded_Bank();
            BankWindow->unlight_preset(m_process->Selected_Preset);
        }
        else    // something went wrong with an invalid vector size (perhaps user deleted the bank)
        {
            // Load the default
            L_B1->do_callback();
        }
    }
    else
    {
        BankWin_Label(m_process->Command_Line_Bank);
    }

    if (!m_process->Command_Line_File)
    {
        m_process->Selected_Preset = m_process->Config.Preset_Number;
        Preset_Counter->value(m_process->Config.Preset_Number);
        Preset_Counter->do_callback();
    }

    // MIDI Learn
    if (!m_process->Config.MIDIway)
        ML_Menu->deactivate();

    // Custom MIDI table
    if (!m_process->Config.custom_midi_table)
        Settings->scroll->deactivate();

    Looper *Efx_Looper = static_cast <Looper*> (m_process->Rack_Effects[EFX_LOOPER]);
    Efx_Looper->setmvol(m_process->Config.Metro_Vol);

    m_process->flpos = m_process->Config.flpos;
    m_process->db6booster = m_process->Config.db6booster;

    m_process->booster = m_process->Config.booster;
    if (m_process->booster == 1.0)
    {
        BostBut->value(0);
    }
    else
    {
        BostBut->value(1);
    }

    if (m_process->Config.init_state)
    {
        m_process->Active_Preset.FX_Master_Active = 1;
        m_process->calculavol(1);
        m_process->calculavol(2);
    }

    Settings->RC_Harm_Opti->value(m_process->Config.RCOpti_Harm);
    Settings->RC_Harm_Opti->do_callback();

    Settings->RC_St_Harm_Opti->value(m_process->Config.RCOpti_Stereo);
    Settings->RC_St_Harm_Opti->do_callback();

    Settings->RC_Ring_Opti->value(m_process->Config.RCOpti_Ring);
    Settings->RC_Ring_Opti->do_callback();

    //MIDIConverter
    MIDI->Midi_out_Counter->value(m_process->Config.Midi_Out_Channel);
    MIDI->Midi_out_Counter->do_callback();
    m_process->efx_MIDIConverter->setmidichannel(m_process->Config.Midi_Out_Channel);

    MIDI->Trig_Adj->value(m_process->Config.Trigger_Adjust);
    m_process->efx_MIDIConverter->setTriggerAdjust(m_process->Config.Trigger_Adjust);

    MIDI->Vel_Adj->value(m_process->Config.Velocity_Adjust);
    m_process->efx_MIDIConverter->setVelAdjust(m_process->Config.Velocity_Adjust);

    MIDI->MIDIOctave->value(m_process->Config.Converter_Octave);
    MIDI->MIDIOctave->do_callback();

    MIDI->midi_activar->value(m_process->Config.MIDI_Converter_On_Off);
    MIDI->midi_activar->do_callback();

    //Metronome
    MetroBar->value(m_process->Config.Metronome_Time);
    MetroBar->do_callback();

    MetroSound->value(m_process->Config.Metronome_Sound);
    MetroSound->do_callback();

    Metro_Volume->value(m_process->Config.Metronome_Volume);
    Metro_Volume->do_callback();

    Metro_Tempo->value(m_process->Config.Metronome_Tempo);
    Metro_Tempo->do_callback();

    metro_activar->value(m_process->Config.Metronome_On_Off);
    metro_activar->do_callback();

    if (m_process->Config.sw_stat == 1)
    {
        MIDI->hide();
        Metro->show();
    }
    else
    {
        Metro->hide();
        MIDI->show();
    }

    // Tuner
    tuner_activar->value(m_process->Config.Tuner_On_Off);
    tuner_activar->do_callback();

    // Tap Tempo
    m_process->Tap_Selection = m_process->Config.Tap_Selection;
    T_SEL->value(m_process->Tap_Selection);

    m_process->t_timeout = m_process->Config.t_timeout;
    Settings->T_TIMEOUT->value(m_process->t_timeout);

    m_process->Tap_SetValue = m_process->Config.Tap_SetValue;
    T_SET->value(m_process->Tap_SetValue);

    Tap_activar->value(m_process->Config.TapTempo_On_Off);
    Tap_activar->do_callback();
    // End Tap

    Fl_Tooltip::enable(m_process->Config.ena_tool);

    Settings->Focus_Slider->value (m_process->Config.Focus_Delay);
    Settings->Focus_Slider->do_callback ();

    // Trigger (ACI)
    m_process->Aux_Source = m_process->Config.Aux_Source;
    Trigger->aux_source->value(m_process->Aux_Source);

    m_process->Aux_Gain = m_process->Config.Aux_Gain;
    Trigger->aux_gain->value(m_process->Aux_Gain);

    m_process->Aux_Threshold = m_process->Config.Aux_Threshold;
    Trigger->aux_thres->value(m_process->Aux_Threshold);

    m_process->Aux_MIDI = m_process->Config.Aux_MIDI;
    Trigger->aux_midi->value(m_process->Aux_MIDI);

    m_process->Aux_Minimum = m_process->Config.Aux_Minimum;
    Trigger->aux_min->value(m_process->Aux_Minimum);

    m_process->Aux_Maximum = m_process->Config.Aux_Maximum;
    Trigger->aux_max->value(m_process->Aux_Maximum);

    // Random window
    random_parameters = m_process->Config.Rand_Parameters;
    RandomEdit->random_params->value(random_parameters);

    efx_always_active = m_process->Config.Rand_Active;
    RandomEdit->random_all_active->value(efx_always_active);

    use_current_active_efx = m_process->Config.Rand_Current;
    RandomEdit->random_current->value(use_current_active_efx);

    max_random_active = m_process->Config.Rand_Max;
    RandomEdit->random_max->value(max_random_active);

    // Set any excluded effects
    for(int i = 0; i < C_NUMBER_EFFECTS; ++i)
    {
        if( m_process->Config.Rand_Exclude[i] == ASCII_One)
            FX_Excluded[i] = (char) 1;
    }

    // Set the scroll buttons
    for (int i = 0; i < RandomEdit->RandomScroll->children(); i++)
    {
        Fl_Widget *w = RandomEdit->RandomScroll->child(i);

        long long ud = (long long) w->user_data();
        ud -= UD_random_edit;

        if(ud >= 0 && ud <= 46)
        {
            RKR_Check_Button *b = static_cast<RKR_Check_Button *>(w);

            if(FX_Excluded[ud])
            {
                b->value(1);
            }
        }
    }
}

void RKRGUI::save_preferences (Fl_Preferences &rakarrack, int whati)
{
    char temp1[128];

    if (whati == 0)
    {
        rakarrack.set(m_process->Config.PrefNom("Principal X"), Principal->x());
        rakarrack.set(m_process->Config.PrefNom("Principal Y"), Principal->y());
        rakarrack.set(m_process->Config.PrefNom("Principal W"), Principal->w());
        rakarrack.set(m_process->Config.PrefNom("Principal H"), Principal->h());
        rakarrack.set(m_process->Config.PrefNom("FontSize"), global_font_size);
        rakarrack.set(m_process->Config.PrefNom("Font"), global_font_type);

        rakarrack.set(m_process->Config.PrefNom("Background Color"), (int) global_back_color);
        rakarrack.set(m_process->Config.PrefNom("Foreground Color"), (int) global_fore_color);
        rakarrack.set(m_process->Config.PrefNom("Leds Color"), (int) global_leds_color);
        rakarrack.set(m_process->Config.PrefNom("Labels Color"), (int) global_label_color);
        rakarrack.set(m_process->Config.PrefNom("Schema"), (int) Settings->scheme_ch->value());
        rakarrack.set(m_process->Config.PrefNom("Hide Effects"), (int) m_process->Config.deachide);
        rakarrack.set(m_process->Config.PrefNom("Scale Window"), (int) m_process->Config.scalable);

        rakarrack.set(m_process->Config.PrefNom("Bank Selected"), m_process->active_bank);

        if ((Preset_Counter->value() > 0) && (Preset_Counter->value() < 61))
            rakarrack.set(m_process->Config.PrefNom("Preset Num"), (int) Preset_Counter->value());

        if (m_process->help_displayed)
        {
            rakarrack.set(m_process->Config.PrefNom("Help X"), visor->x());
            rakarrack.set(m_process->Config.PrefNom("Help Y"), visor->y());
            rakarrack.set(m_process->Config.PrefNom("Help W"), visor->w());
            rakarrack.set(m_process->Config.PrefNom("Help H"), visor->h());
            rakarrack.set(m_process->Config.PrefNom("Help TextSize"), visor->textsize());
        }

        //Tuner
        rakarrack.set(m_process->Config.PrefNom("Tuner On/Off"), (int) m_process->Tuner_Active);

        //MIDIConverter
        rakarrack.set(m_process->Config.PrefNom("MIDI Converter On/Off"), (int) m_process->MIDIConverter_Active);
        rakarrack.set(m_process->Config.PrefNom("Midi Out Channel"), (int) MIDI->Midi_out_Counter->value());
        rakarrack.set(m_process->Config.PrefNom("Trigger Adjust"), (int) MIDI->Trig_Adj->value());
        rakarrack.set(m_process->Config.PrefNom("Velocity Adjust"), (int) MIDI->Vel_Adj->value());
        rakarrack.set(m_process->Config.PrefNom("Converter Octave"), (int) MIDI->MIDIOctave->value());

        //Metronome
        rakarrack.set(m_process->Config.PrefNom("Internal Metronome On/Off"), (int) m_process->Metro_Active);
        rakarrack.set(m_process->Config.PrefNom("Internal Metronome Time"), (int) MetroBar->value());
        rakarrack.set(m_process->Config.PrefNom("Internal Metronome Volume"), (int) Metro_Volume->value());
        rakarrack.set(m_process->Config.PrefNom("Internal Metronome Tempo"), (int) Metro_Tempo->value());
        rakarrack.set(m_process->Config.PrefNom("Internal Metronome Show"), (int) m_process->Config.sw_stat);
        rakarrack.set(m_process->Config.PrefNom("Internal Metronome Sound"), (int) MetroSound->value());

        //Booster
        rakarrack.set(m_process->Config.PrefNom("Booster"), m_process->booster);

        //Tap Tempo
        rakarrack.set(m_process->Config.PrefNom("TapTempo On/Off"), (int) m_process->Tap_Active);
        rakarrack.set(m_process->Config.PrefNom("TapTempo Input"), (int) m_process->Tap_Selection);
        rakarrack.set(m_process->Config.PrefNom("TapTempo Set"), (int) m_process->Tap_SetValue);
    }

    if (whati == 1)
    {
        rakarrack.set(m_process->Config.PrefNom("BankWindow X"), BankWindow->x());
        rakarrack.set(m_process->Config.PrefNom("BankWindow Y"), BankWindow->y());
        rakarrack.set(m_process->Config.PrefNom("BankWindow W"), BankWindow->w());
        rakarrack.set(m_process->Config.PrefNom("BankWindow H"), BankWindow->h());
    }

    if (whati == 2)
    {
        rakarrack.set(m_process->Config.PrefNom("Order X"), Order->x());
        rakarrack.set(m_process->Config.PrefNom("Order Y"), Order->y());
        rakarrack.set(m_process->Config.PrefNom("Order W"), Order->w());
        rakarrack.set(m_process->Config.PrefNom("Order H"), Order->h());
    }

    if (whati == 3)
    {
        rakarrack.set(m_process->Config.PrefNom("Settings X"), Settings->x());
        rakarrack.set(m_process->Config.PrefNom("Settings Y"), Settings->y());
        rakarrack.set(m_process->Config.PrefNom("Settings W"), Settings->w());
        rakarrack.set(m_process->Config.PrefNom("Settings H"), Settings->h());
    }

    if (whati == 4)
    {
        rakarrack.set(m_process->Config.PrefNom("Random X"), RandomEdit->x());
        rakarrack.set(m_process->Config.PrefNom("Random Y"), RandomEdit->y());
        rakarrack.set(m_process->Config.PrefNom("Random W"), RandomEdit->w());
        rakarrack.set(m_process->Config.PrefNom("Random H"), RandomEdit->h());

        rakarrack.set(m_process->Config.PrefNom("Rand Parameters"), random_parameters);
        rakarrack.set(m_process->Config.PrefNom("Rand Active"), efx_always_active);
        rakarrack.set(m_process->Config.PrefNom("Rand Current"), use_current_active_efx);
        rakarrack.set(m_process->Config.PrefNom("Rand Max"), max_random_active);

        // convert the asci char to string for the set 
        std::string s;
        for(int i = 0; i < C_NUMBER_EFFECTS; ++i)
        {
            s +=  NTS((int) FX_Excluded[i]);
        }

        rakarrack.set(m_process->Config.PrefNom("Rand Exclude"), s.c_str());
    }

    if (whati == 5)
    {
        rakarrack.set(m_process->Config.PrefNom("MIDI Learn X"), MIDILearn->x());
        rakarrack.set(m_process->Config.PrefNom("MIDI Learn Y"), MIDILearn->y());
        rakarrack.set(m_process->Config.PrefNom("MIDI Learn W"), MIDILearn->w());
        rakarrack.set(m_process->Config.PrefNom("MIDI Learn H"), MIDILearn->h());
    }

    if (whati == 6)
    {
        rakarrack.set(m_process->Config.PrefNom("Trigger X"), Trigger->x());
        rakarrack.set(m_process->Config.PrefNom("Trigger Y"), Trigger->y());
        rakarrack.set(m_process->Config.PrefNom("Trigger W"), Trigger->w());
        rakarrack.set(m_process->Config.PrefNom("Trigger H"), Trigger->h());

        rakarrack.set(m_process->Config.PrefNom("Aux Source"), m_process->Aux_Source);
        rakarrack.set(m_process->Config.PrefNom("Aux Gain"), m_process->Aux_Gain);
        rakarrack.set(m_process->Config.PrefNom("Aux Threshold"), m_process->Aux_Threshold);
        rakarrack.set(m_process->Config.PrefNom("Aux MIDI"), m_process->Aux_MIDI);
        rakarrack.set(m_process->Config.PrefNom("Aux Minimum"), m_process->Aux_Minimum);
        rakarrack.set(m_process->Config.PrefNom("Aux Maximum"), m_process->Aux_Maximum);
    }

    if (whati == 7)
    {
        rakarrack.set(m_process->Config.PrefNom("Delay X"), DelayFile->x());
        rakarrack.set(m_process->Config.PrefNom("Delay Y"), DelayFile->y());
        rakarrack.set(m_process->Config.PrefNom("Delay W"), DelayFile->w());
        rakarrack.set(m_process->Config.PrefNom("Delay H"), DelayFile->h());
    }

    if ((whati == 3) || (whati == 0))
    {
        rakarrack.set(m_process->Config.PrefNom("UserName"), m_process->Config.UserRealName);
        rakarrack.set(m_process->Config.PrefNom("Preserve Gain/Master"), m_process->Config.preserve_master);
        rakarrack.set(m_process->Config.PrefNom("Metronome Volume"), m_process->Config.Metro_Vol);

        rakarrack.set(m_process->Config.PrefNom("Filter DC Offset"), m_process->Config.DC_Offset);

        rakarrack.set(m_process->Config.PrefNom("Update Tap"), m_process->Config.Tap_Updated);
        rakarrack.set(m_process->Config.PrefNom("Limiter Position"), m_process->flpos);
        rakarrack.set(m_process->Config.PrefNom("Limiter +6dB"), m_process->db6booster);

        rakarrack.set(m_process->Config.PrefNom("Recognize Optimization Harmonizer"), m_process->Config.RCOpti_Harm);
        rakarrack.set(m_process->Config.PrefNom("Recognize Optimization Stereo Harm"), m_process->Config.RCOpti_Stereo);
        rakarrack.set(m_process->Config.PrefNom("Recognize Optimization Ring"), m_process->Config.RCOpti_Ring);

        rakarrack.set(m_process->Config.PrefNom("Harmonizer Downsample"), m_process->Config.Har_Down);
        rakarrack.set(m_process->Config.PrefNom("Harmonizer Up Quality"), m_process->Config.Har_U_Q);
        rakarrack.set(m_process->Config.PrefNom("Harmonizer Down Quality"), m_process->Config.Har_D_Q);
        rakarrack.set(m_process->Config.PrefNom("Reverbtron Downsample"), m_process->Config.Rev_Down);
        rakarrack.set(m_process->Config.PrefNom("Reverbtron Up Quality"), m_process->Config.Rev_U_Q);
        rakarrack.set(m_process->Config.PrefNom("Reverbtron Down Quality"), m_process->Config.Rev_D_Q);
        rakarrack.set(m_process->Config.PrefNom("Convolotron Downsample"), m_process->Config.Con_Down);
        rakarrack.set(m_process->Config.PrefNom("Convolotron Up Quality"), m_process->Config.Con_U_Q);
        rakarrack.set(m_process->Config.PrefNom("Convolotron Down Quality"), m_process->Config.Con_D_Q);
        rakarrack.set(m_process->Config.PrefNom("Sequence Downsample"), m_process->Config.Seq_Down);
        rakarrack.set(m_process->Config.PrefNom("Sequence Up Quality"), m_process->Config.Seq_U_Q);
        rakarrack.set(m_process->Config.PrefNom("Sequence Down Quality"), m_process->Config.Seq_D_Q);
        rakarrack.set(m_process->Config.PrefNom("Shifter Downsample"), m_process->Config.Shi_Down);
        rakarrack.set(m_process->Config.PrefNom("Shifter Up Quality"), m_process->Config.Shi_U_Q);
        rakarrack.set(m_process->Config.PrefNom("Shifter Down Quality"), m_process->Config.Shi_D_Q);
        rakarrack.set(m_process->Config.PrefNom("Vocoder Downsample"), m_process->Config.Voc_Down);
        rakarrack.set(m_process->Config.PrefNom("Vocoder Up Quality"), m_process->Config.Voc_U_Q);
        rakarrack.set(m_process->Config.PrefNom("Vocoder Down Quality"), m_process->Config.Voc_D_Q);
        rakarrack.set(m_process->Config.PrefNom("StereoHarm Downsample"), m_process->Config.Ste_Down);
        rakarrack.set(m_process->Config.PrefNom("StereoHarm Up Quality"), m_process->Config.Ste_U_Q);
        rakarrack.set(m_process->Config.PrefNom("StereoHarm Down Quality"), m_process->Config.Ste_D_Q);

        rakarrack.set(m_process->Config.PrefNom("Distortion Resampling"), m_process->Config.Dist_res_amount);
        rakarrack.set(m_process->Config.PrefNom("Distortion Up Quality"), m_process->Config.Dist_up_q);
        rakarrack.set(m_process->Config.PrefNom("Distortion Down Quality"), m_process->Config.Dist_down_q);

        rakarrack.set(m_process->Config.PrefNom("Overdrive Resampling"), m_process->Config.Ovrd_res_amount);
        rakarrack.set(m_process->Config.PrefNom("Overdrive Up Quality"), m_process->Config.Ovrd_up_q);
        rakarrack.set(m_process->Config.PrefNom("Overdrive Down Quality"), m_process->Config.Ovrd_down_q);

        rakarrack.set(m_process->Config.PrefNom("Derelict Resampling"), m_process->Config.Dere_res_amount);
        rakarrack.set(m_process->Config.PrefNom("Derelict Up Quality"), m_process->Config.Dere_up_q);
        rakarrack.set(m_process->Config.PrefNom("Derelict Down Quality"), m_process->Config.Dere_down_q);
        
        rakarrack.set(m_process->Config.PrefNom("DistBand Resampling"), m_process->Config.DBand_res_amount);
        rakarrack.set(m_process->Config.PrefNom("DistBand Up Quality"), m_process->Config.DBand_up_q);
        rakarrack.set(m_process->Config.PrefNom("DistBand Down Quality"), m_process->Config.DBand_down_q);

        rakarrack.set(m_process->Config.PrefNom("StompBox Resampling"), m_process->Config.Stomp_res_amount);
        rakarrack.set(m_process->Config.PrefNom("StompBox Up Quality"), m_process->Config.Stomp_up_q);
        rakarrack.set(m_process->Config.PrefNom("StompBox Down Quality"), m_process->Config.Stomp_down_q);

        rakarrack.set(m_process->Config.PrefNom("Calibration"), m_process->Config.aFreq);
        rakarrack.set(m_process->Config.PrefNom("Recognize Trigger"), m_process->Config.rtrig);

        rakarrack.set(m_process->Config.PrefNom("Vocoder Bands"), m_process->Config.VocBands);

        rakarrack.set(m_process->Config.PrefNom("FX_init_state"), m_process->Config.init_state);
        rakarrack.set(m_process->Config.PrefNom("Auto Assign"), m_process->Config.autoassign);

        rakarrack.set(m_process->Config.PrefNom("UpSampling"), m_process->Config.upsample);
        rakarrack.set(m_process->Config.PrefNom("UpQuality"), m_process->Config.UpQual);
        rakarrack.set(m_process->Config.PrefNom("DownQuality"), m_process->Config.DownQual);

        rakarrack.set(m_process->Config.PrefNom("UpAmount"), m_process->Config.UpAmo);
        rakarrack.set(m_process->Config.PrefNom("Looper Size"), m_process->Config.looper_size);

        rakarrack.set(m_process->Config.PrefNom("Bank Filename"), m_process->Config.BankFilename);
        rakarrack.set(m_process->Config.PrefNom("User Directory"), m_process->Config.UDirFilename);

        rakarrack.set(m_process->Config.PrefNom("Enable Background Image"), m_process->Config.EnableBackgroundImage);
        rakarrack.set(m_process->Config.PrefNom("Background Image"), m_process->Config.BackgroundImage);
        rakarrack.set(m_process->Config.PrefNom("Auto Connect MIDI IN"), m_process->Config.aconnect_MI);
        rakarrack.set(m_process->Config.PrefNom("Auto Connect Jack"), m_process->Config.aconnect_JA);
        rakarrack.set(m_process->Config.PrefNom("Auto Connect Jack In"), m_process->Config.aconnect_JIA);

        rakarrack.set(m_process->Config.PrefNom("MIDI Implementation"), m_process->Config.MIDIway);
        rakarrack.set(m_process->Config.PrefNom("MIDI Table"), m_process->Config.custom_midi_table);


        int i = Settings->BMidiIn->value();
        char *temp;
        temp = (char*) Settings->BMidiIn->text(i);
        
        if (temp != NULL)
        {
            rakarrack.set(m_process->Config.PrefNom("MIDI IN Device"), temp);
        }
        
        rakarrack.set(m_process->Config.PrefNom("MIDI IN Channel"), m_process->Config.MIDI_In_Channel + 1);
        rakarrack.set(m_process->Config.PrefNom("MIDI IN Harmonizer"), m_process->Config.Harmonizer_MIDI_Channel + 1);
        rakarrack.set(m_process->Config.PrefNom("MIDI IN Stereo Harmonizer"), m_process->Config.StereoHarm_MIDI_Channel + 1);
        rakarrack.set(m_process->Config.PrefNom("Harmonizer Quality"), m_process->Config.HarQual);
        rakarrack.set(m_process->Config.PrefNom("StereoHarm Quality"), m_process->Config.SteQual);
        rakarrack.set(m_process->Config.PrefNom("Sequence Quality"), m_process->Config.SeqQual);
        rakarrack.set(m_process->Config.PrefNom("Shifter Quality"), m_process->Config.ShiQual);
        rakarrack.set(m_process->Config.PrefNom("Tap Tempo Timeout"), m_process->t_timeout);
        rakarrack.set(m_process->Config.PrefNom("Disable Warnings"), m_process->Config.Disable_Warnings);
        rakarrack.set(m_process->Config.PrefNom("Enable Tooltips"), m_process->Config.ena_tool);
        rakarrack.set(m_process->Config.PrefNom("Focus Delay"), m_process->Config.Focus_Delay);
        rakarrack.set(m_process->Config.PrefNom("MIDI Table File"), m_process->Config.custom_midi_table_file);
#ifdef NSM_SUPPORT
        if(!nsm_preferences_file.empty())
        {
            rakarrack.set(m_process->Config.PrefNom("NSM Single State"), m_process->Config.NSM_single_state);
            rakarrack.set(m_process->Config.PrefNom("NSM Gui Status"), m_process->Gui_Shown);
        }
#endif
    }

    if (whati == 3)
    {
        int k = 1;
        for (int i = 1; i <= Settings->JackCo->size(); i++)
        {
            if (Settings->JackCo->selected(i))
            {
                memset(temp1, 0, sizeof (temp1));
                sprintf(temp1, "Jack Port %d", k);
                rakarrack.set(m_process->Config.PrefNom(temp1), Settings->JackCo->text(i));
                k++;
            }
        }

        rakarrack.set(m_process->Config.PrefNom("Auto Connect Num"), k - 1);

        k = 1;
        for (int i = 1; i <= Settings->JackIn->size(); i++)
        {
            if (Settings->JackIn->selected(i))
            {
                memset(temp1, 0, sizeof (temp1));
                sprintf(temp1, "Jack Port In %d", k);
                rakarrack.set(m_process->Config.PrefNom(temp1), Settings->JackIn->text(i));
                k++;
            }
        }

        rakarrack.set(m_process->Config.PrefNom("Auto Connect In Num"), k - 1);
    }
}

/**
 *  Save the current user state, windows, images, bank selected, preset, etc.
 *  In ~/user/.fltk/github.com.Stazed.rakarrack.plus/rakarrack-plus.prefs
 * 
 * @param whati
 *      The window settings that should be saved:
 *      0 = Main window & User settings
 *      1 = Bank window
 *      2 = Order window
 *      3 = Settings window & User settings
 *      4 = Random edit
 *      5 = MIDI Learn
 *      6 = Trigger window (ACI)
 *      7 = Echotron Delay file window
 */
void RKRGUI::save_current_state(int whati)
{
    if(nsm_preferences_file.empty())
    {
        Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);
        save_preferences(rakarrack, whati);
    }
    else    // Using NSM
    {
        Fl_Preferences rakarrack(nsm_preferences_file.c_str(), jack_client_name, NULL);
        save_preferences(rakarrack, whati);
    }
}

/**
 * Copy the loaded active preset to the main window gui.
 */
void RKRGUI::Put_Loaded()
{
    WPreset_Name->value(m_process->Active_Preset.Preset_Name);
    DAuthor->copy_label(m_process->Active_Preset.Author);

    Nivel_Entrada->value((int) (float) (m_process->Active_Preset.Input_Gain * 100.0) - 50);
    m_process->calculavol(1);
    Nivel_Salida->value((int) (float) (m_process->Active_Preset.Master_Volume * 100.0) - 50);
    m_process->calculavol(2);
    Balance->value((int) (float) (m_process->Active_Preset.Fraction_Bypass * 100.0));


    ActivarGeneral->value(m_process->Active_Preset.FX_Master_Active);

    if (m_process->Active_Preset.FX_Master_Active == 0)
    {
        m_process->val_il_sum = -50;
        m_process->val_ir_sum = -50;

        m_process->val_vl_sum = -50;
        m_process->val_vr_sum = -50;

        input_vul->value(-50);
        input_vur->value(-50);

        output_vul->value(-50);
        output_vur->value(-50);
    }


    /*  Original set and cleared for only the ten current from efx_order[] which did not
        clear other efx from leftover settings. The new behavior will reset all efx to default
        if not used. The reset will occur when the New button is pressed or a new bank is selected.
        If an existing bank re-positions an effect, by replacement and then moving it to a new location
        then the settings are kept from previous.
  
        Legacy:
        for(i=0;i<10;i++) {
        switch(m_process->efx_order[i]){
     */

    for (int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        // Cabinet is special
        if( i == EFX_CABINET)
        {
            CABINET->Cabinet_output->value(m_process->Active_Preset.Effect_Params[EFX_CABINET][Cabinet_Gain] - 64);
            CABINET->Cabinet_preset->value(m_process->Active_Preset.Effect_Params[EFX_CABINET][Cabinet_Preset_Idx]);
            CABINET->Cabinet_activar->value(m_process->EFX_Active[EFX_CABINET]);
        }
        else
        {
            // Calls the preset callback and sets the effect to the first preset
            Efx_Gui_Base[i]->activate_effect->value (m_process->EFX_Active[i]);
            Efx_Gui_Base[i]->preset_choice->do_callback (Efx_Gui_Base[i]->preset_choice, 1);
        }
    }

    reordena();
}

void RKRGUI::preset_click(Fl_Button* o, void* v)
{
    ((RKRGUI*) (o->parent()->parent()->user_data()))->preset_click_i(o, v);
}

inline void RKRGUI::preset_click_i(Fl_Button* o, void*)
{
    // used when selecting/moving/right click on preset from bank window

    int keyboard_key = Fl::event_key();
    long long bank_preset_button = (long long) o->user_data();
    int button_number = (int) bank_preset_button;

    if (drag != C_NO_DRAG)
    {
        int drag_destination = search_bank_preset_button(Fl::event_x(), Fl::event_y());
        if (drag_destination != C_NO_DRAG)
        {
            m_process->Bank[0] = m_process->Bank[drag_destination];
            m_process->Bank[drag_destination] = m_process->Bank[button_number];
            m_process->Bank[button_number] = m_process->Bank[0];
            Put_Loaded_Bank();
            m_process->bank_modified = 1;
            button_number = drag_destination;
            drag = C_NO_DRAG;
        }
    }

    if ((Fl::event_is_click() == 0)&&(Fl::event() == FL_DRAG))
    {
        drag = button_number;
        return;
    }
    else
    {
        drag = C_NO_DRAG;
    }

    // The user wants to save the main window preset to the bank selected preset button
    if ((Fl::event_button() == FL_RIGHT_MOUSE) && (Fl::event() == FL_RELEASE))
    {
        Fl_Widget *w = BankWindow->ob->child(button_number - 1);

        // Check if user really wants to set the preset to the bank
        Fl_Widget *m = fl_message_icon();
        m->parent()->copy_label(m_process->jackcliname);
        
        // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
        m_process->Shut_Off_Below_Mouse = 1;
        int ok = fl_choice("Overwrite \"%s\"?", "No", "Yes", NULL, w->label());
        m_process->Shut_Off_Below_Mouse = 0;
        
        if (!ok)
        {
            o->value(0);
            o->redraw();
            return;
        }

        o->value(0);
        o->redraw();
        m_process->main_window_preset_to_active_bank(button_number);
        
        std::string add_pg_change = "[";
        add_pg_change += NTS(button_number);
        add_pg_change += "] ";
        add_pg_change += m_process->Active_Preset.Preset_Name;

        w->copy_label(add_pg_change.c_str());
        m_process->bank_modified = 1;
    }

    // The user selected a preset from the bank window, so highlight and set it
    if ((Fl::event_button() == FL_LEFT_MOUSE) || keyboard_key == ASCII_Space)
    {
        if ((button_number != m_process->Selected_Preset) || (m_process->new_bank_loaded))
        {
            Fl_Widget *w = BankWindow->ob->child(button_number - 1);
            BankWindow->unlight_preset(m_process->Selected_Preset);
            m_process->Selected_Preset = button_number;
            w->color(fl_darker(global_leds_color));
            Preset_Counter->value(button_number);
            Preset_Counter->do_callback();
        }
    }
}

void RKRGUI::reordena()
{
    // Reorder efx
    unsigned int x[10], y[10];
    ulong shortcut[10];

    shortcut[0] = ASCII_One;
    shortcut[1] = ASCII_Two;
    shortcut[2] = ASCII_Three;
    shortcut[3] = ASCII_Four;
    shortcut[4] = ASCII_Five;
    shortcut[5] = ASCII_Six;
    shortcut[6] = ASCII_Seven;
    shortcut[7] = ASCII_Eight;
    shortcut[8] = ASCII_Nine;
    shortcut[9] = ASCII_Zero;


    x[0] = E1->x();
    x[1] = E2->x();
    x[2] = E3->x();
    x[3] = E4->x();
    x[4] = E5->x();
    x[5] = E6->x();
    x[6] = E7->x();
    x[7] = E8->x();
    x[8] = E9->x();
    x[9] = E10->x();

    y[0] = E1->y();
    y[1] = E2->y();
    y[2] = E3->y();
    y[3] = E4->y();
    y[4] = E5->y();
    y[5] = E6->y();
    y[6] = E7->y();
    y[7] = E8->y();
    y[8] = E9->y();
    y[9] = E10->y();

    // Hide All Effects
    for (int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        Efx_Gui_Base[i]->hide ();
    }

    // Show effects with new order
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        for (int j = 0; j < C_NUMBER_EFFECTS; j++)
        {
            // Search the effects for the requested in main window order 
            if (j == m_process->efx_order[i])
            {
                // Sets the position in the main window
                Efx_Gui_Base[j]->position(x[i], y[i]);
                Efx_Gui_Base[j]->activate_effect->shortcut (shortcut[i]);
                
                // This is the Hide/Show inactive effect button choice
                // If it is not hide, then show all effects
                if (!m_process->Config.deachide)
                {
                    Efx_Gui_Base[j]->show();
                }

                // Set the active status, for label highlighting
                // If the Hide/Show button is set for hide, then show only active
                if (m_process->EFX_Active[j])
                {
                    m_process->active[i] = 1;

                    // Show the active
                    if (m_process->Config.deachide)
                        Efx_Gui_Base[j]->show();
                }
                else
                {
                    m_process->active[i] = 0;
                }
            }
        }

        // Sets the effect label
        switch (i)
        {
            case 0:
                L1->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L1->hide();
                else
                    L1->show();
                break;
            case 1:
                L2->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L2->hide();
                else
                    L2->show();
                break;
            case 2:
                L3->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L3->hide();
                else
                    L3->show();
                break;
            case 3:
                L4->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L4->hide();
                else
                    L4->show();
                break;
            case 4:
                L5->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L5->hide();
                else
                    L5->show();
                break;
            case 5:
                L6->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L6->hide();
                else
                    L6->show();
                break;
            case 6:
                L7->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L7->hide();
                else
                    L7->show();
                break;
            case 7:
                L8->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L8->hide();
                else
                    L8->show();
                break;
            case 8:
                L9->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L9->hide();
                else
                    L9->show();
                break;
            case 9:
                L10->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->Config.deachide) && (!m_process->active[i]))
                    L10->hide();
                else
                    L10->show();
                break;
        }
    }

    Principal->init_sizes();

    ChangeActives();

    Fl::redraw();
}

void RKRGUI::show_help()
{
    // show the help window
    char temp[128];

    // Is this the first time displayed ? If true then we have to create it.
    if (m_process->help_displayed == 0)
    {
        visor = new Fl_Help_Dialog;

        visor->resize(m_process->Config.Help_X, m_process->Config.Help_Y,
                      m_process->Config.Help_W, m_process->Config.Help_H);

        visor->textsize((unsigned char) m_process->Config.Help_TextSize);
    }


    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/html/help.html", HELPDIR);


    if (FILE * file = fopen(temp, "r"))
    {
        fclose(file);
        visor->load(temp);
        m_process->help_displayed = 1;
        visor->show();
    }
}

void RKRGUI::show_lic()
{
    // Show licenses
    char temp[128];

    // Is this the first time displayed ? If true then we have to create it.
    if (m_process->help_displayed == 0)
    {
        visor = new Fl_Help_Dialog;

        visor->resize(m_process->Config.Help_X, m_process->Config.Help_Y,
                      m_process->Config.Help_W, m_process->Config.Help_H);

        visor->textsize((unsigned char) m_process->Config.Help_TextSize);
    }

    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/html/license.html", HELPDIR);
    visor->load(temp);

    m_process->help_displayed = 1;

    visor->show();
}

void RKRGUI::MiraClientes()
{
    // Find Audio and midi ports
    FILE *fp;

    Settings->BMidiIn->clear();


    if ((fp = fopen("/proc/asound/seq/clients", "r")) != NULL)
    {
        char temp[128];
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "Port") != NULL)
            {
                char temp1[128];
                char *masque;
                strcpy(temp1, temp);
                strtok(temp1, "\"");
                char *name = strtok(NULL, "\"");
                masque = strtok(NULL, ")");
                
                std::string s_name = m_process->jackcliname;
                s_name + " MC OUT";

                if ((masque[2] == 'R') && (strstr(name, s_name.c_str()) == 0))
                {
                    Settings->BMidiIn->add(name);
                }
            }
        }

        fclose(fp);
    }

    Settings->JackCo->clear();

    const char **ports;


    if ((ports = jack_get_ports(m_process->jackclient, NULL, JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput)) == NULL)
    {
        fprintf(stderr, "Cannot find any Input port\n");
    }
    else
    {
        int i = 0;

        while (ports[i] != NULL)
        {
            if ((strstr(ports[i], "rakarrack-plus:in_1") == 0) && (strstr(ports[i], "rakarrack-plus:in_2") == 0))
            {
                Settings->JackCo->add(ports[i]);
            }

            i++;
        }
    }

    free(ports);

    Settings->JackIn->clear();

    const char **iports;

    if ((iports = jack_get_ports(m_process->jackclient, NULL, JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput)) == NULL)
    {
        fprintf(stderr, "Cannot find any Output port\n");
    }
    else
    {
        int i = 0;

        while (iports[i] != NULL)
        {
            if ((strstr(iports[i], "rakarrack-plus:out_1") == 0) && (strstr(iports[i], "rakarrack-plus:out_2") == 0))
            {
                Settings->JackIn->add(iports[i]);
            }

            i++;
        }
    }

    free(iports);
}

void RKRGUI::MiraConfig()
{
    // Loads the settings into the settings class
    {
        int i = 1;
        while (Settings->BMidiIn->text(i) != NULL)
        {
            if (strcmp(Settings->BMidiIn->text(i), m_process->Config.MID) == 0)
            {
                Settings->BMidiIn->select(i, 1);
            }
            i++;
        }
    }
    
    {
        int i = 1;
        while (Settings->JackCo->text(i) != NULL)
        {
            for (int k = 0; k < m_process->Config.cuan_jack; k++)
            {
                if (strcmp(Settings->JackCo->text(i), m_process->Config.jack_po[k].name) == 0)
                {
                    Settings->JackCo->select(i, 1);
                }
            }

            i++;
        }
    }

    {
        int i = 1;
        while (Settings->JackIn->text(i) != NULL)
        {
            for (int k = 0; k < m_process->Config.cuan_ijack; k++)
            {
                if (strcmp(Settings->JackIn->text(i), m_process->Config.jack_poi[k].name) == 0)
                {
                    Settings->JackIn->select(i, 1);
                }
            }

            i++;
        }
    }

    if (m_process->Config.MIDIway)
    {
        Settings->Mw1->setonly();
    }
    else
    {
        Settings->Mw0->setonly();
    }

    m_process->m_displayed = 0;
    Settings->Enable_Back->value(m_process->Config.EnableBackgroundImage);
    Settings->Enable_DeacHide->value(m_process->Config.deachide);
    Settings->Enable_Scale->value(m_process->Config.scalable);

    Settings->BFiname->value(m_process->Config.BankFilename);
    Settings->BackFiname->value(m_process->Config.BackgroundImage);
    Settings->Udir->value(m_process->Config.UDirFilename);
    Settings->Username->value(m_process->Config.UserRealName);
    Settings->Pre_Serve->value(m_process->Config.preserve_master);
    Settings->LM_Volume->value(m_process->Config.Metro_Vol);
    Settings->Filter_DC->value(m_process->Config.DC_Offset);
    Settings->FLPosition->value(m_process->flpos);
    Settings->DB6B->value(m_process->db6booster);
    Settings->Har_Downsample->value(m_process->Config.Har_Down);
    Settings->Har_Down_Qua->value(m_process->Config.Har_D_Q);
    Settings->Har_Up_Qua->value(m_process->Config.Har_U_Q);
    Settings->Rev_Downsample->value(m_process->Config.Rev_Down);
    Settings->Rev_Down_Qua->value(m_process->Config.Rev_D_Q);
    Settings->Rev_Up_Qua->value(m_process->Config.Rev_U_Q);
    Settings->Con_Downsample->value(m_process->Config.Con_Down);
    Settings->Con_Down_Qua->value(m_process->Config.Con_D_Q);
    Settings->Con_Up_Qua->value(m_process->Config.Con_U_Q);
    Settings->Seq_Downsample->value(m_process->Config.Seq_Down);
    Settings->Seq_Down_Qua->value(m_process->Config.Seq_D_Q);
    Settings->Seq_Up_Qua->value(m_process->Config.Seq_U_Q);
    Settings->Shi_Downsample->value(m_process->Config.Shi_Down);
    Settings->Shi_Down_Qua->value(m_process->Config.Shi_D_Q);
    Settings->Shi_Up_Qua->value(m_process->Config.Shi_U_Q);
    Settings->Voc_Downsample->value(m_process->Config.Voc_Down);
    Settings->Voc_Down_Qua->value(m_process->Config.Voc_D_Q);
    Settings->Voc_Up_Qua->value(m_process->Config.Voc_U_Q);
    Settings->Ste_Downsample->value(m_process->Config.Ste_Down);
    Settings->Ste_Down_Qua->value(m_process->Config.Ste_D_Q);
    Settings->Ste_Up_Qua->value(m_process->Config.Ste_U_Q);
    
    Settings->Dist_Amo->value(m_process->Config.Dist_res_amount);
    Settings->Dist_Up_Qua->value(m_process->Config.Dist_up_q);
    Settings->Dist_Down_Qua->value(m_process->Config.Dist_down_q);
    
    Settings->Ovrd_Amo->value(m_process->Config.Ovrd_res_amount);
    Settings->Ovrd_Up_Qua->value(m_process->Config.Ovrd_up_q);
    Settings->Ovrd_Down_Qua->value(m_process->Config.Ovrd_down_q);
    
    Settings->Dere_Amo->value(m_process->Config.Dere_res_amount);
    Settings->Dere_Up_Qua->value(m_process->Config.Dere_up_q);
    Settings->Dere_Down_Qua->value(m_process->Config.Dere_down_q);
    
    Settings->DBand_Amo->value(m_process->Config.DBand_res_amount);
    Settings->DBand_Up_Qua->value(m_process->Config.DBand_up_q);
    Settings->DBand_Down_Qua->value(m_process->Config.DBand_down_q);
    
    Settings->Stomp_Amo->value(m_process->Config.Stomp_res_amount);
    Settings->Stomp_Up_Qua->value(m_process->Config.Stomp_up_q);
    Settings->Stomp_Down_Qua->value(m_process->Config.Stomp_down_q);

    Settings->Calibration->value(m_process->Config.aFreq);
    Settings->RTrigger->value(m_process->HarmRecNote->trigfact);
    Settings->RC_Harm_Opti->value(m_process->Config.RCOpti_Harm);
    Settings->RC_St_Harm_Opti->value(m_process->Config.RCOpti_Stereo);
    Settings->RC_Ring_Opti->value(m_process->Config.RCOpti_Ring);


    switch (m_process->Config.VocBands)
    {
        case 16:
            Settings->Voc_Bands->value(0);
            break;
        case 32:
            Settings->Voc_Bands->value(1);
            break;
        case 64:
            Settings->Voc_Bands->value(2);
            break;
        case 128:
            Settings->Voc_Bands->value(3);
            break;
        case 256:
            Settings->Voc_Bands->value(4);
            break;

    }


    Settings->MTable->value(m_process->Config.custom_midi_table);
    Settings->MTable->do_callback ();

    Settings->AAssign->value(m_process->Config.autoassign);
    Settings->Update_TAP->value(m_process->Config.Tap_Updated);
    Settings->INSTATE->value(m_process->Config.init_state);
    Settings->UPSAMPLE_C->value(m_process->Config.upsample);
    Settings->Upr_Qual->value(m_process->Config.UpQual);
    Settings->Downr_Qual->value(m_process->Config.DownQual);
    Settings->MESSAGE_DISABLE->value(m_process->Config.Disable_Warnings);
    Settings->ENA_TOOL->value(m_process->Config.ena_tool);
    Settings->Focus_Slider->value(m_process->Config.Focus_Delay);
    Settings->T_TIMEOUT->value(m_process->t_timeout);
#ifdef NSM_SUPPORT
    Settings->NSM_SINGLE_STATE->value((m_process->Config.NSM_single_state));
#endif
    Settings->Upr_Amo->value(m_process->Config.UpAmo);
    Settings->L_SIZE->value(m_process->Config.looper_size);
    Settings->D_A_Connect->value(m_process->Config.aconnect_MI);
    Settings->D_J_Connect->value(m_process->Config.aconnect_JA);
    Settings->D_IJ_Connect->value(m_process->Config.aconnect_JIA);
    
    // For NSM the default setting is Off for auto connect audio and we do not let the user change it.
    // All jack connections should be handled by NSM.
    if(!nsm_preferences_file.empty())    // NSM
    {
        Settings->D_J_Connect->deactivate ();
        Settings->D_IJ_Connect->deactivate ();
    }
    else    // Not is NSM so make this inactive
    {
        Settings->NSM_SINGLE_STATE->deactivate();
    }

    Settings->Midi_In_Counter->value(m_process->Config.MIDI_In_Channel + 1);
    Settings->Har_In_Counter->value(m_process->Config.Harmonizer_MIDI_Channel + 1);
    Settings->Stereo_Har_In_Counter->value(m_process->Config.StereoHarm_MIDI_Channel + 1);

    switch (m_process->Config.HarQual)
    {
        case 4:
            Settings->Har_Qual->value(0);
            break;

        case 8:
            Settings->Har_Qual->value(1);
            break;

        case 16:
            Settings->Har_Qual->value(2);
            break;

        case 32:
            Settings->Har_Qual->value(3);
            break;
    }

    switch (m_process->Config.SteQual)
    {
        case 4:
            Settings->Ste_Qual->value(0);
            break;

        case 8:
            Settings->Ste_Qual->value(1);
            break;

        case 16:
            Settings->Ste_Qual->value(2);
            break;

        case 32:
            Settings->Ste_Qual->value(3);
            break;
    }
    
    switch (m_process->Config.SeqQual)
    {
        case 4:
            Settings->Seq_Qual->value(0);
            break;

        case 8:
            Settings->Seq_Qual->value(1);
            break;

        case 16:
            Settings->Seq_Qual->value(2);
            break;

        case 32:
            Settings->Seq_Qual->value(3);
            break;
    }

    switch (m_process->Config.ShiQual)
    {
        case 4:
            Settings->Shi_Qual->value(0);
            break;

        case 8:
            Settings->Shi_Qual->value(1);
            break;

        case 16:
            Settings->Shi_Qual->value(2);
            break;

        case 32:
            Settings->Shi_Qual->value(3);
            break;
    }


    if (m_process->Config.aconnect_MI)
    {
        Settings->BMidiIn->activate();
    }
    else
    {
        Settings->BMidiIn->deactivate();
    }

    if (m_process->Config.aconnect_JA)
    {
        Settings->JackCo->activate();
    }
    else
    {
        Settings->JackCo->deactivate();
    }

    if (m_process->Config.aconnect_JIA)
    {
        Settings->JackIn->activate();
    }
    else
    {
        Settings->JackIn->deactivate();
    }

    Fl_Menu_Item *item;
    Fl_Menu_Item *Har = Settings->get_menu_Har_Downsample();
    unsigned int SR_value = m_process->sample_rate;

    for (int j = 0; j < Har->size(); j++)
    {
        item = Har->next(j);

        switch (j)
        {
            case 0:
                SR_value = m_process->sample_rate;
                break;
            case 1:
                SR_value = 96000;
                break;
            case 2:
                SR_value = 48000;
                break;
            case 3:
                SR_value = 44100;
                break;
            case 4:
                SR_value = 32000;
                break;
            case 5:
                SR_value = 22050;
                break;
            case 6:
                SR_value = 16000;
                break;
            case 7:
                SR_value = 12000;
                break;
            case 8:
                SR_value = 8000;
                break;
            case 9:
                SR_value = 4000;
                break;
        }

        if ((j > 0) && ((unsigned int) m_process->sample_rate <= SR_value))
        {
            item->deactivate();
        }
        else
        {
            item->activate();
        }
    }


    Settings->Font_Bro->clear();
    Settings->Font_Bro->textcolor(global_label_color);

    int k = Fl::set_fonts(0);
    for (int i = 0; i < k; i++)
    {
        int t;
        const char *name = Fl::get_font_name((Fl_Font) i, &t);
        char buffer[128];
#if 1
        // This sets format specifiers. If more are added, then the
        // alpha search in RKR_Browser::handle() may need to be adjusted.
        if (t)
        {
            char *ptr = buffer;
            if (t & FL_BOLD)
            {
                *ptr++ = '@';
                *ptr++ = 'b';
            }
            if (t & FL_ITALIC)
            {
                *ptr++ = '@';
                *ptr++ = 'i';
            }
            strcpy(ptr, name);
            name = buffer;
        }
#else // this is neat, but really slow on some X servers:
        sprintf(buffer, "@F%d@.%s", i, name);
        name = buffer;
#endif
        Settings->Font_Bro->add(name);
    }

    Settings->Font_Bro->value(global_font_type + 1);
}

void RKRGUI::BankWin_Label(const std::string &filename)
{
    char tmp[256];

    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Bank Manager - %s", m_process->jackcliname, VERSION, fl_filename_name(filename.c_str()));
    BankWindow->copy_label(tmp);
}

void RKRGUI::is_bank_modified()
{
    // popup dialog for modified bank saving

    if (m_process->bank_modified)
    {
        Fl_Widget *w = fl_message_icon();
        w->parent()->copy_label(m_process->jackcliname);

        // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
        m_process->Shut_Off_Below_Mouse = 1;
        int ok = fl_choice("Bank was modified, but not saved", "Discard", "Save", NULL);
        m_process->Shut_Off_Below_Mouse = 0;
        
        switch (ok)
        {
            case 0:
                m_process->bank_modified = 0;
                break;

            case 1:
                set_save_file();
                break;
        }
    }
}

void
RKRGUI::is_PG_table_modified()
{
    if (m_process->PG_table_modified)
    {
        Fl_Widget *w = fl_message_icon();
        w->parent()->copy_label(m_process->jackcliname);

        // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
        m_process->Shut_Off_Below_Mouse = 1;
        int ok = fl_choice("Program change table was modified, but not saved", "Discard", "Save", NULL);
        m_process->Shut_Off_Below_Mouse = 0;
        
        switch (ok)
        {
            case 0:
                m_process->PG_table_modified = 0;
                break;

            case 1:
                Save_Midi_Program_Change_Table();
                break;
        }
    }
}

void RKRGUI::Put_Loaded_Bank()
{
    int k = 1;

    // Program change number before the preset name
    for (int t = 0; t < BankWindow->ob->children(); t++)
    {
        Fl_Widget *w = BankWindow->ob->child(t);
        long long temp = (long long) w->user_data();
        if (temp > 0)
        {
            std::string add_pg_change = "[";
            add_pg_change += NTS(k);
            add_pg_change += "] ";
            add_pg_change += m_process->Bank[k].Preset_Name;
            w->copy_label(add_pg_change.c_str ());
            k++;
        }
    }

    FillML();
}

void RKRGUI::Chord(int eff)
{
    // display selected chord in harmonizer and stereo harmonizer
    int tipo, undi;
    tipo = 0;
    undi = 0;

    switch (eff)
    {
        case 0:
            tipo = m_process->Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Chord);
            undi = m_process->Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Note);
            break;
        case 1:
            tipo = m_process->Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Chord);
            undi = m_process->Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Note);
            break;
    }


    if (eff == 0)
    {
        sprintf(m_process->RC_Harm->NombreAcorde, "%s%s", m_process->RC_Harm->NCE[undi].Nom, m_process->RC_Harm->ChN[tipo].Nom);

        m_process->RC_Harm->ctipo = tipo;
        m_process->RC_Harm->fundi = undi;
        m_process->RC_Harm->cc = 1;
    }
    else
    {
        sprintf(m_process->RC_Stereo_Harm->NombreAcorde, "%s%s", m_process->RC_Stereo_Harm->NCE[undi].Nom, m_process->RC_Stereo_Harm->ChN[tipo].Nom);

        m_process->RC_Stereo_Harm->ctipo = tipo;
        m_process->RC_Stereo_Harm->fundi = undi;
        m_process->RC_Stereo_Harm->cc = 1;
    }
}

/**
 *  Adjust Gui parameters from MIDI control action.
 */
void RKRGUI::MIDI_control_gui_refresh()
{
    // Mvalue is flag to indicate at least one parameter needs update
    if (!m_process->Mvalue)
        return;

    // Index starts at 1, since case 0 is unused
    for (int mc_index = 1; mc_index < C_MC_RANGE; mc_index++)
    {
        // Mcontrol is flag for each individual parameter
        // 1 = the parameter needs update, 0 = no update needed
        if (!m_process->Mcontrol[mc_index])
        {
            continue;   // No gui update needed
        }
        
        // Special cases Master controls
        switch(mc_index)
        {
            case MC_Output_Volume:
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Nivel_Salida->value((int) (m_process->Active_Preset.Master_Volume * 100.0) - 50);
                Nivel_Salida->redraw();
                break;

            case MC_Balance_FX:
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Balance->value(m_process->Active_Preset.Fraction_Bypass * 100.0);
                Balance->redraw();
                break;

            case MC_Input_Volume:
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Nivel_Entrada->value((int) (m_process->Active_Preset.Input_Gain * 100.0) - 50);
                Nivel_Entrada->redraw();
                break;

            case MC_Multi_On_Off:
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                ActOnOff();
                break;
        }
        
        if (!m_process->Mcontrol[mc_index])
        {
            continue;   // We found it in above special cases
        }
        
        // Normal MIDI processing of rack effects
        for (int param_index = 0; param_index < C_MC_PARAMETER_SIZE; param_index++)
        {
            // Find the effect parameter that needs update
            if(m_process->mc_efx_params[param_index].MC_params_index == mc_index)
            {
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                
                // Get the effect and parameter
                int effect_index = m_process->mc_efx_params[param_index].Effect_index;
                int efx_param_index = m_process->mc_efx_params[param_index].Efx_param_index;

                // Tell the gui effect to update the parameter
                Efx_Gui_Base[effect_index]->parameter_refresh (efx_param_index);
                break;
            }
        }
    }

    // Clear the flag
    m_process->Mvalue = 0;
}

/**
 * Turn effect on or off from MIDI control (default CC #116)
 */
void RKRGUI::ActOnOff()
{
    while (m_process->OnOffC > 0)
    {
        int miralo;

        if (m_process->Mnumeff[m_process->OnOffC] > 2000)
        {
            miralo = m_process->Mnumeff[m_process->OnOffC] - 2000;
        }
        else if (m_process->Mnumeff[m_process->OnOffC] >= 1000)
        {
            miralo = m_process->Mnumeff[m_process->OnOffC] - 1000;
        }
        else    // efx_order[] is limited by C_NUMBER_ORDERED_EFFECTS
        {
            miralo = m_process->efx_order[m_process->Mnumeff[m_process->OnOffC]];
        }
        
        // Check for rack effects
        if (miralo < C_NUMBER_EFFECTS)
        {
            Efx_Gui_Base[miralo]->activate_effect->value (m_process->EFX_Active[miralo]);
            Efx_Gui_Base[miralo]->activate_effect->do_callback ();
            m_process->OnOffC--;
            continue;
        }
        else
        {
            switch (miralo)
            {
                case EFX_TAP_TEMPO_ON_OFF:
                    Tap_activar->value(m_process->Tap_Active);
                    Tap_activar->do_callback();
                    break;
                case EFX_MIDI_CONVERTER_ON_OFF:
                    MIDI->midi_activar->value(m_process->MIDIConverter_Active);
                    MIDI->midi_activar->do_callback();
                    break;
                case EFX_TUNER_ON_OFF:
                    tuner_activar->value(m_process->Tuner_Active);
                    tuner_activar->do_callback();
                    break;
                case EFX_MASTER_ON_OFF:
                    ActivarGeneral->value(m_process->Active_Preset.FX_Master_Active);
                    ActivarGeneral->do_callback();
                    break;
            }
        }

        m_process->OnOffC--;
    }
}

void RKRGUI::PutBackground()
{
    // Put selected .png background image on everything
    delete back;
    back = new Fl_Tiled_Image(new Fl_PNG_Image(m_process->Config.BackgroundImage), 3200, 2400);
    
    if (!m_process->Config.EnableBackgroundImage)
    {
        back->color_average(global_back_color, 0.0);
    }

    InOut->image(back);

    // Rack effects
    for (int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        Efx_Gui_Base[i]->image (InOut->image());
    }

    Tap->image(InOut->image());
    Presets->image(InOut->image());
    Tuner->image(InOut->image());
    MIDI->image(InOut->image());
    Metro->image(InOut->image());
    fondo->image(InOut->image());
    TITTLE_L->image(InOut->image());
    Order->Fondo1->image(InOut->image());
    Settings->Fondo2->image(InOut->image());
    BankWindow->Fondo3->image(InOut->image());
    MIDILearn->Fondo4->image(InOut->image());
    Trigger->Fondo5->image(InOut->image());
    Settings->Fondo6->image(InOut->image());
    Settings->Fondo7->image(InOut->image());
    Settings->Fondo8->image(InOut->image());
    Settings->Fondo9->image(InOut->image());
    Settings->Fondo10->image(InOut->image());
    Settings->Fondo11->image(InOut->image());
    Settings->Fondo12->image(InOut->image());

    Etit->image(InOut->image());
    MIDILearn->Ares->image(InOut->image());

    MenuP->image(InOut->image());
    BankWindow->MenuB->image(InOut->image());
    
    DelayFile->Fondo13->image(InOut->image());
    RandomEdit->Fondo14->image(InOut->image());

    Fl::redraw();
}

void RKRGUI::ChangeActives()
{
    //Change label colors based on active or inactive
    Fl_Color on = fl_lighter(fl_lighter(global_label_color));
    Fl_Color off = fl_darker(global_label_color);


    if (m_process->active[0])
        L1->labelcolor(on);
    else
        L1->labelcolor(off);
    if (m_process->active[1])
        L2->labelcolor(on);
    else
        L2->labelcolor(off);
    if (m_process->active[2])
        L3->labelcolor(on);
    else
        L3->labelcolor(off);
    if (m_process->active[3])
        L4->labelcolor(on);
    else
        L4->labelcolor(off);
    if (m_process->active[4])
        L5->labelcolor(on);
    else
        L5->labelcolor(off);
    if (m_process->active[5])
        L6->labelcolor(on);
    else
        L6->labelcolor(off);
    if (m_process->active[6])
        L7->labelcolor(on);
    else
        L7->labelcolor(off);
    if (m_process->active[7])
        L8->labelcolor(on);
    else
        L8->labelcolor(off);
    if (m_process->active[8])
        L9->labelcolor(on);
    else
        L9->labelcolor(off);
    if (m_process->active[9])
        L10->labelcolor(on);
    else
        L10->labelcolor(off);



    if (m_process->MIDIConverter_Active)
        MIDI->MIDI_LABEL->labelcolor(on);
    else
        MIDI->MIDI_LABEL->labelcolor(off);
    if (m_process->Metro_Active)
        Metro_Label->labelcolor(on);
    else
        Metro_Label->labelcolor(off);
    if (m_process->Tap_Active)
        TAP_LABEL->labelcolor(on);
    else
        TAP_LABEL->labelcolor(off);
    if (m_process->Tuner_Active)
        TUNER_LABEL->labelcolor(on);
    else
        TUNER_LABEL->labelcolor(off);
    if (m_process->Active_Preset.FX_Master_Active)
        LABEL_IO->labelcolor(on);
    else
        LABEL_IO->labelcolor(off);

    if ((m_process->Config.upsample) && (m_process->Active_Preset.FX_Master_Active))
    {
        UPS_LED->color(global_leds_color);
        UPS_LED->redraw();
    }
    else
    {
        UPS_LED->color(fl_darker(global_leds_color));
        UPS_LED->redraw();
    }

    CLIP_LED->color(fl_darker(FL_RED));
    LMT_LED->color(fl_darker(global_leds_color));
}

void RKRGUI::findpos(int num, int value, Fl_Widget*)
{
    // adjust efx title label colors based on active of inactive
    int i = 0;
    Fl_Color on = fl_lighter(fl_lighter(global_label_color));
    Fl_Color off = fl_darker(global_label_color);

    for (i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        if (m_process->efx_order[i] == num)
        {
            m_process->active[i] = value;
            break;
        }
    }


    switch (i)
    {

        case 0:
            if (value)
                L1->labelcolor(on);
            else
                L1->labelcolor(off);
            L1->redraw_label();
            break;

        case 1:
            if (value)
                L2->labelcolor(on);
            else
                L2->labelcolor(off);
            L2->redraw_label();
            break;

        case 2:
            if (value)
                L3->labelcolor(on);
            else
                L3->labelcolor(off);
            L3->redraw_label();
            break;

        case 3:
            if (value)
                L4->labelcolor(on);
            else
                L4->labelcolor(off);
            L4->redraw_label();
            break;

        case 4:
            if (value)
                L5->labelcolor(on);
            else
                L5->labelcolor(off);
            L5->redraw_label();
            break;

        case 5:
            if (value)
                L6->labelcolor(on);
            else
                L6->labelcolor(off);
            L6->redraw_label();
            break;

        case 6:
            if (value)
                L7->labelcolor(on);
            else
                L7->labelcolor(off);
            L7->redraw_label();
            break;

        case 7:
            if (value)
                L8->labelcolor(on);
            else
                L8->labelcolor(off);
            L8->redraw_label();
            break;

        case 8:
            if (value)
                L9->labelcolor(on);
            else
                L9->labelcolor(off);
            L9->redraw_label();
            break;

        case 9:
            if (value)
                L10->labelcolor(on);
            else
                L10->labelcolor(off);
            L10->redraw_label();
            break;
    }
}

void RKRGUI::Put_Skin()
{
    Principal->resize(Principal->x(), Principal->y(), m_process->swidth, m_process->sheight);

    Settings->scheme_ch->value(m_process->sschema);
    Settings->scheme_ch->do_callback();
    PutBackground();
    
    back_color_change((Fl_Color) m_process->sback_color);
    leds_color_change((Fl_Color) m_process->sleds_color);
    buttons_color_change((Fl_Color) m_process->sfore_color);
    label_color_change((Fl_Color) m_process->slabel_color);
    font_size_change(m_process->sfont_size);
    font_type_change(m_process->sfont_type);

    save_current_state(3);
    Fl::redraw();
}

void RKRGUI::FillML(/*int type*/)
{
    // Midi learn
    char tmp[256];
    memset(tmp, 0, sizeof (tmp));


    sprintf(tmp, "%s   v%s - MIDI Learn - Preset : %s", m_process->jackcliname, VERSION, m_process->Bank[m_process->Selected_Preset].Preset_Name);
    MIDILearn->copy_label(tmp);

    memset(m_process->ML_clist, 0, sizeof (m_process->ML_clist));
    MIDILearn->Epar->clear();
    
    int k = 0;

    switch (m_process->ML_filter)
    {
        case 0:
            for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
                MIDILearn->Epar->add(m_process->mc_efx_params[i].Description);
            break;

        case 1:
            for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
            {
                if (m_process->mc_efx_params[i].Effect_index == EFX_MASTER)
                {
                    MIDILearn->Epar->add(m_process->mc_efx_params[i].Description);
                    m_process->ML_clist[k] = m_process->mc_efx_params[i].MC_params_index;
                    k++;
                }
            }

            for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
            {
                for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
                {
                    if (m_process->mc_efx_params[i].Effect_index == m_process->efx_order[j])
                    {
                        MIDILearn->Epar->add(m_process->mc_efx_params[i].Description);
                        m_process->ML_clist[k] = m_process->mc_efx_params[i].MC_params_index;
                        k++;
                    }
                }
            }
            break;
    }

    MIDILearn->Epar->redraw();


    MIDILearn->TPresets->clear();
    for (int i = 1; i <= 60; i++)
    {
        MIDILearn->TPresets->add(m_process->Bank[i].Preset_Name);
    }

    MIDILearn->TPresets->select(m_process->Selected_Preset, 1);
    MIDILearn->TPresets->redraw();

    DisAssigns();
}

/**
 * Clear MIDI learn table.
 */
void RKRGUI::DisAssigns()
{
    char tmp[8];

    int the_one = 0;

    if ((int) MIDILearn->Epar->value())
    {
        if (m_process->ML_filter == 0)
        {
            the_one = m_process->mc_efx_params[(int) MIDILearn->Epar->value() - 1].MC_params_index;
        }
        else
        {
            the_one = m_process->ML_clist[(int) MIDILearn->Epar->value() - 1];
        }
    }

    int k = 0;

    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (m_process->Active_Preset.XUserMIDI[i][j] == the_one)
            {
                k++;
                memset(tmp, 0, sizeof (tmp));
                sprintf(tmp, "%d", i);

                switch (k)
                {
                    case 1:
                        MIDILearn->Ar1->copy_label(tmp);
                        MIDILearn->Ar1->redraw();
                        break;

                    case 2:
                        MIDILearn->Ar2->copy_label(tmp);
                        MIDILearn->Ar2->redraw();
                        break;

                    case 3:
                        MIDILearn->Ar3->copy_label(tmp);
                        MIDILearn->Ar3->redraw();
                        break;

                    case 4:
                        MIDILearn->Ar4->copy_label(tmp);
                        MIDILearn->Ar4->redraw();
                        break;

                    case 5:
                        MIDILearn->Ar5->copy_label(tmp);
                        MIDILearn->Ar5->redraw();
                        break;

                    case 6:
                        MIDILearn->Ar6->copy_label(tmp);
                        MIDILearn->Ar6->redraw();
                        break;

                    case 7:
                        MIDILearn->Ar7->copy_label(tmp);
                        MIDILearn->Ar7->redraw();
                        break;

                    case 8:
                        MIDILearn->Ar8->copy_label(tmp);
                        MIDILearn->Ar8->redraw();
                        break;

                    case 9:
                        MIDILearn->Ar9->copy_label(tmp);
                        MIDILearn->Ar9->redraw();
                        break;

                    case 10:
                        MIDILearn->Ar10->copy_label(tmp);
                        MIDILearn->Ar10->redraw();
                        break;

                    case 11:
                        MIDILearn->Ar11->copy_label(tmp);
                        MIDILearn->Ar11->redraw();
                        break;

                    case 12:
                        MIDILearn->Ar12->copy_label(tmp);
                        MIDILearn->Ar12->redraw();
                        break;

                    case 13:
                        MIDILearn->Ar13->copy_label(tmp);
                        MIDILearn->Ar13->redraw();
                        break;

                    case 14:
                        MIDILearn->Ar14->copy_label(tmp);
                        MIDILearn->Ar14->redraw();
                        break;

                    case 15:
                        MIDILearn->Ar15->copy_label(tmp);
                        MIDILearn->Ar15->redraw();
                        break;

                    case 16:
                        MIDILearn->Ar16->copy_label(tmp);
                        MIDILearn->Ar16->redraw();
                        break;

                    case 17:
                        MIDILearn->Ar17->copy_label(tmp);
                        MIDILearn->Ar17->redraw();
                        break;

                    case 18:
                        MIDILearn->Ar18->copy_label(tmp);
                        MIDILearn->Ar18->redraw();
                        break;

                    case 19:
                        MIDILearn->Ar19->copy_label(tmp);
                        MIDILearn->Ar19->redraw();
                        break;

                    case 20:
                        MIDILearn->Ar20->copy_label(tmp);
                        MIDILearn->Ar20->redraw();
                        break;
                }
            }
        }
    }


    k++;

    for (int i = k; i <= 20; i++)
    {
        memset(tmp, 0, sizeof (tmp));

        switch (i)
        {
            case 1:
                MIDILearn->Ar1->copy_label(tmp);
                MIDILearn->Ar1->redraw();
                break;

            case 2:
                MIDILearn->Ar2->copy_label(tmp);
                MIDILearn->Ar2->redraw();
                break;

            case 3:
                MIDILearn->Ar3->copy_label(tmp);
                MIDILearn->Ar3->redraw();
                break;

            case 4:
                MIDILearn->Ar4->copy_label(tmp);
                MIDILearn->Ar4->redraw();
                break;

            case 5:
                MIDILearn->Ar5->copy_label(tmp);
                MIDILearn->Ar5->redraw();
                break;

            case 6:
                MIDILearn->Ar6->copy_label(tmp);
                MIDILearn->Ar6->redraw();
                break;

            case 7:
                MIDILearn->Ar7->copy_label(tmp);
                MIDILearn->Ar7->redraw();
                break;

            case 8:
                MIDILearn->Ar8->copy_label(tmp);
                MIDILearn->Ar8->redraw();
                break;

            case 9:
                MIDILearn->Ar9->copy_label(tmp);
                MIDILearn->Ar9->redraw();
                break;

            case 10:
                MIDILearn->Ar10->copy_label(tmp);
                MIDILearn->Ar10->redraw();
                break;

            case 11:
                MIDILearn->Ar11->copy_label(tmp);
                MIDILearn->Ar11->redraw();
                break;

            case 12:
                MIDILearn->Ar12->copy_label(tmp);
                MIDILearn->Ar12->redraw();
                break;

            case 13:
                MIDILearn->Ar13->copy_label(tmp);
                MIDILearn->Ar13->redraw();
                break;

            case 14:
                MIDILearn->Ar14->copy_label(tmp);
                MIDILearn->Ar14->redraw();
                break;

            case 15:
                MIDILearn->Ar15->copy_label(tmp);
                MIDILearn->Ar15->redraw();
                break;

            case 16:
                MIDILearn->Ar16->copy_label(tmp);
                MIDILearn->Ar16->redraw();
                break;

            case 17:
                MIDILearn->Ar17->copy_label(tmp);
                MIDILearn->Ar17->redraw();
                break;

            case 18:
                MIDILearn->Ar18->copy_label(tmp);
                MIDILearn->Ar18->redraw();
                break;

            case 19:
                MIDILearn->Ar19->copy_label(tmp);
                MIDILearn->Ar19->redraw();
                break;

            case 20:
                MIDILearn->Ar20->copy_label(tmp);
                MIDILearn->Ar20->redraw();
                break;
        }
    }
}

void RKRGUI::Prepare_Order()
{
    // prepare order
    Order->Order_Bro->clear();

    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        m_process->new_order[i] = m_process->efx_order[i];
        m_process->saved_order[i] = m_process->efx_order[i];
        Order->Order_Bro->add(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
    }

    Fill_Avail(m_process->Effect_Type_Filter);

    Order->Order_Bro->select(1);
}

void RKRGUI::Show_Next_Time()
{
    // popup for settings changes that will not take effect until restart
    if (m_process->Config.Disable_Warnings)
        return;

    Fl_Widget *w = fl_message_icon();
    w->parent()->copy_label(m_process->jackcliname);
    
    m_process->Handle_Message (38);
}

void RKRGUI::update_looper()
{
    // update looper
    Looper *Efx_Looper = static_cast <Looper*> (m_process->Rack_Effects[EFX_LOOPER]);
    Efx_Looper->getstate();

    LOOPER->looper_play->value(Efx_Looper->progstate[0]);
    LOOPER->looper_play->redraw();
    LOOPER->looper_record->value(Efx_Looper->progstate[2]);
    LOOPER->looper_record->redraw();
    LOOPER->looper_stop->value(Efx_Looper->progstate[1]);
    LOOPER->looper_stop->redraw();

    if (Efx_Looper->progstate[2])
    {
        LOOPER->Box_P->copy_label("Stop");
        LOOPER->looper_stop->copy_label("@square");
    }
    else
    {
        LOOPER->Box_P->copy_label("Pause");
        LOOPER->looper_stop->copy_label("@||");
    }

    LOOPER->looper_t1->value(Efx_Looper->progstate[4]);
    LOOPER->looper_t1->redraw();
    LOOPER->looper_t2->value(Efx_Looper->progstate[5]);
    LOOPER->looper_t2->redraw();
}

/**
 * Updates the gui for global tempo settings from Tap Tempo.
 * This cycles through all effects, but only those that apply
 * tap tempo have an override of the base virtual function.
 * The effects that do not override use the base empty function.
 */
void RKRGUI::update_tap_tempo_GUI()
{
    for(int efx_index = 0; efx_index < C_NUMBER_EFFECTS; efx_index++)
    {
        if (m_process->EFX_Active[efx_index])
        {
            Efx_Gui_Base[efx_index]->tap_tempo_update ();
        }
    }
}

/**
 * Analog Control effect.
 */
void RKRGUI::ActACI()
{
    float gain = 0.0;
    float tmp = 0.0;

    switch (m_process->Aux_Source)
    {
        case 0:
            gain = dB2rap(75.0f * (float) m_process->Aux_Gain / 127.0f);
            tmp = m_process->val_a_sum * gain;
            break;
        case 1:
            gain = (float) m_process->Aux_Gain / 127.0f;
            tmp = dB2rap(m_process->val_il_sum) * 12.0f * gain;
            break;
        case 2:
            gain = (float) m_process->Aux_Gain / 127.0f;
            tmp = dB2rap(m_process->val_ir_sum) * 12.0f * gain;
            break;
    }

    float aux_vulevel = (float) CLAMP(rap2dB(tmp), -48.0, 15.0);

    float threshold = (float) m_process->Aux_Threshold;
    Trigger->aux_vu->value(aux_vulevel);

    if (aux_vulevel < threshold)
        tmp = 0.0;

    if (tmp > 1.0)
        tmp = 1.0f;

    int amax = m_process->Aux_Maximum;
    int amin = m_process->Aux_Minimum;

    int value = amin + lrintf((float) (amax - amin) * tmp);

    if (value != m_process->last_auxvalue)
    {
        if (m_process->Config.MIDIway)
        {
            for (int i = 0; i < 20; i++)
            {
                if (m_process->Active_Preset.XUserMIDI[m_process->Aux_MIDI][i])
                {
                    m_process->process_midi_controller_events(m_process->Active_Preset.XUserMIDI[m_process->Aux_MIDI][i], value);
                }
                else
                    break;
            }
        }
        else
        {
            m_process->process_midi_controller_events(m_process->Aux_MIDI, value);
        }

        m_process->last_auxvalue = value;
    }
}

/**
 * Look up effect number from efx_names structure array by EFX_Index.
 *
 * @param num
 *      The EFX_Index number for the requested name.
 *
 * @return
 *      The index position for the effect name.
 */
int RKRGUI::Busca_Eff(int num)
{
    int i = 0;

    for (i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        if (m_process->efx_names[i].Pos == num)
        {
            break;
        }
    }

    return (i);
}

void RKRGUI::Fill_Avail(int filter)
{
    //Fill_Avail

    Order->Avail_Bro->clear();

    int t = 1;

    for (int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        int k = 0;
        for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
        {
            if (m_process->new_order[j] == m_process->efx_names[i].Pos)
            {
                k = 1;
            }
        }

        if (!k)
        {
            if ((m_process->efx_names[i].Type & m_process->Effect_Type_Filter) == filter)
            {
                Order->Avail_Bro->add(m_process->efx_names[i].Nom);
                m_process->availables[t] = m_process->efx_names[i].Pos;
                t++;
            }
        }
    }

    Order->Avail_Bro->select(1);
}

void RKRGUI::set_focus_timer(int time)
{
    focus_delay_time = m_process->Config.Focus_Delay = time;
}

void RKRGUI::below_mouse_highlight_and_focus()
{
    if(m_process->Shut_Off_Below_Mouse)
        return;

    if (Fl::focus() == TITTLE_L)
        Fl::focus(Open_Order);

    if (Fl::focus() == Etit)
        Fl::focus(Open_Order);

    Fl_Widget *widget_belowmouse = Fl::belowmouse();

    long long widget_user_data = 0;
    if (widget_belowmouse != NULL)
    {
        if(widget_belowmouse->active ())
        {
            widget_user_data = (long long) widget_belowmouse->user_data();
            
            // Check range
            if ((widget_user_data <= C_UD_Highlight_Begin) || (widget_user_data >= C_UD_Highlight_End))
            {
                widget_user_data = 0;   // out of range so don't use it
            }
        }
    }

    // Un-highlight if no longer below the mouse
    if ((previous_widget != NULL) && (previous_widget != widget_belowmouse))
    {
        // Check if bank window preset button, special handling in RKR_Button::draw()
        long long previous_widget_user_data = (long long) previous_widget->user_data();
        if( (previous_widget_user_data >= UD_Bank_Preset_Start) && (previous_widget_user_data <= UD_Bank_Preset_End) )
        {
            RKR_Button *previous_button = static_cast<RKR_Button*> (previous_widget);
            previous_button->set_bank_under_mouse (0);  // False
            previous_button->redraw ();
        }
        else    // Everything else not bank window preset buttons
        {
            previous_widget->color(global_fore_color);
            previous_widget->redraw();
            previous_widget->redraw_label();
        }
        previous_widget = NULL;
    }

    // Check if new below mouse widget
    if(widget_user_data && (widget_belowmouse != previous_widget))
    {
        // Start the focus count
        grab_focus = 1;

        // Highlight the item below mouse within the user_data range - see global.h, USER_DATA_index
        // Check if bank window preset button, special handling in RKR_Button::draw()
        if( (widget_user_data >= UD_Bank_Preset_Start) && (widget_user_data <= UD_Bank_Preset_End) )
        {
            RKR_Button *bank_button = static_cast<RKR_Button*> (widget_belowmouse);
            bank_button->set_bank_under_mouse (1);  // True
            bank_button->redraw ();
        }
        else    // Everything else not bank window preset buttons
        {
            widget_belowmouse->color(fl_color_average(global_fore_color, fl_lighter(global_fore_color), .6));
            widget_belowmouse->redraw();
        }
        previous_widget = widget_belowmouse;
    }
    
    if(focus_delay_time)
    {
        if(widget_user_data && (widget_belowmouse == previous_widget))
        {
            if(grab_focus &&  (grab_focus > focus_delay_time))
            {
                Fl::focus (widget_belowmouse);

                // Shut off focus count
                grab_focus = 0;
            }
            else if(grab_focus)
            {
                grab_focus++;
            }
        }
    }
}

int RKRGUI::search_bank_preset_button(int x, int y)
{
    for (int t = 0; t < BankWindow->ob->children(); t++)
    {
        Fl_Widget *w = BankWindow->ob->child(t);

        if ((x >= w->x()) && (x <= (w->x() + w->w())) && (y >= w->y()) && (y <= (w->y() + w->h())))
        {
            long long bank_preset_number = (long long) w->user_data();
            return ((int) bank_preset_number);
        }
    }

    return (C_NO_DRAG);
}

/** 
 Scan the default (DATADIR) and user directories for bank files (.rkrb).
 The user directory is set in Settings/Preferences/Bank/User Directory.
 Any directory files found are passed to the BankWindow class and listed on the
 Bank Manager "User Banks" drop down menu.
 */
void RKRGUI::Scan_Bank_Dir(int reload)
{
    int menu_item = 0;

    if(reload)
    {
        m_process->load_bank_vector ();

        // On reload save the menu item so it can be set again
        menu_item = BankWindow->CH_UB->value();
    }

    // Clear the bank window menus
    BankWindow->CH_UB->clear();
    BankWindow->clear_menu();
    
    for(unsigned i = 0; i < m_process->Bank_Vector.size (); i++)
    {
        std::string full_path = m_process->Bank_Vector[i].Bank_File_Name;
        std::string menu_name = m_process->Bank_Vector[i].Bank_Menu_Name;
        BankWindow->set_bank_CH_UB(&menu_name[0], (char*) full_path.c_str());
    }

    BankWindow->CH_UB->value(menu_item);
    
    if(reload)
    {
        Settings->make_table_window ();
        m_process->load_default_midi_table ();
        m_process->load_custom_MIDI_table_preset_names();
    }
}

/**
 * Find various global keys, buttons for special processing.
 * 
 * @param event
 *      FLTK event to check.
 * 
 * @return 
 *      The status if used = 1, unused = 0.
 */
int RKRGUI::global_shortcuts(int event)
{
    if (event == FL_DRAG)
    {
        Fl_Widget *widget_belowmouse = Fl::belowmouse();
        long long widget_user_data = (long long) widget_belowmouse->user_data();
        if ((widget_user_data < UD_Label_1) || (widget_user_data > UD_Label_10))
            return 0;
        drag = widget_user_data - UD_Label_1;
        return 1;
    }

    if (event != FL_SHORTCUT)
    {
        return 0;
    }
    else
    {
        if ((Fl::event_key(ASCII_Plus)) || (Fl::event_key(FL_KP + ASCII_Plus))) // +(plus) key
        {
            process_rkr->Gui_Refresh = GUI_Refresh_Plus_Key;
            return 1;
        }

        if ((Fl::event_key(ASCII_Minus)) || (Fl::event_key(FL_KP + ASCII_Minus))) // -(minus) key
        {
            process_rkr->Gui_Refresh = GUI_Refresh_Minus_Key;
            return 1;
        }

        if (Fl::event_key(FL_F + 2)) // F2 key - decrease output volume
        {
            process_rkr->Gui_Refresh = GUI_Refresh_F2_Key;
            return 1;
        }

        if (Fl::event_key(FL_F + 3)) // F3 key - increase output volume
        {
            process_rkr->Gui_Refresh = GUI_Refresh_F3_Key;
            return 1;
        }

        if (Fl::event_key(FL_Insert)) // Insert key - add user preset
        {
            Fl_Widget *widget_belowmouse = Fl::belowmouse();
            
            if (widget_belowmouse == NULL)
            {
                return 0;
            }
            
            long long widget_user_data = (long long) widget_belowmouse->user_data();
            
            if ((widget_user_data >= UD_PRESET_EQ) && (widget_user_data <= UD_PRESET_MAX_EFFECTS))
            {
                ((RKRGUI*) (widget_belowmouse->parent()->parent()->user_data()))->
                        get_insert_preset_name(widget_belowmouse, widget_user_data - UD_PRESET_EQ);
            }
            
            return 1;
        }

        if (Fl::event_key(FL_Delete)) // Delete key - delete user preset
        {
            Fl_Widget *widget_belowmouse = Fl::belowmouse();
            
            if (widget_belowmouse == NULL)
            {
                return 0;
            }
            long long widget_user_data = (long long) widget_belowmouse->user_data();
            
            if ((widget_user_data >= UD_PRESET_EQ) && (widget_user_data <= UD_PRESET_MAX_EFFECTS))
            {
                ((RKRGUI*) (widget_belowmouse->parent()->parent()->user_data()))->
                        delete_insert_preset(widget_belowmouse, widget_user_data - UD_PRESET_EQ);
            }
            
            return 1;
        }
    }

    return 0;
}

/**
 *  This pops up the midi learn window and sets the item (num) to be set with 
 *  midi learn. It is triggered by right mouse click on any efx parameter (slider, etc)
 *  that has midi learn capability.
 * 
 *  Don't pop up the midi learn window unless the user checked the box
 *  in Settings/Preferences/MIDI/Midi Learn - Really confusing when
 *  it pops up but nothing happens when set. The menu item for Midi Learn
 *  is set inactive unless the box is checked as well. For consistency here,
 *  if (!m_process->Config.MIDIway)
 *      return;
 * 
 * @param num
 *  The efx parameter number that is to be used by midi learn.
 */
void RKRGUI::getMIDIControl(int num)
{
    // Don't pop up the midi learn window unless the user checked the box.
    if (!m_process->Config.MIDIway)
        return;

    int i = 0;
    m_process->comemouse = 1;
    PrepareML();
    while (i < MIDILearn->Epar->size())
    {
        if ((m_process->ML_filter == 0) && (m_process->mc_efx_params[i].MC_params_index == num))
        {
            MIDILearn->Epar->select(i + 1);
            break;
        }

        if ((m_process->ML_filter == 1) && (m_process->ML_clist[i] == num))
        {
            MIDILearn->Epar->select(i + 1);
            break;
        }

        i++;
    }

    DisAssigns();
    MIDILearn->GMM->do_callback();
}

void RKRGUI::PrepareML()
{
    // PrepareML
    FillML();
    if (m_process->comemouse == 0)
    {
        MIDILearn->show();
        put_icon(MIDILearn);
        return;
    }

    if (m_process->comemouse)
    {
        if (m_process->Config.autoassign == 0)
        {
            MIDILearn->show();
            put_icon(MIDILearn);
            return;
        }
    }
}

/**
 * Shows the input window so the user can enter a 'Insert' preset name.
 * 
 * @param w
 *      The RKR_Choice widget to the related effect.
 * 
 * @param effect
 *      The EFX_Index number.
 */
inline void RKRGUI::get_insert_preset_name(Fl_Widget *w, int effect)
{
    // Cabinet does not allow insert presets
    if (effect == EFX_CABINET)
        return;

    // Need to shut off below mouse or it tries to modify the fl_input widget and crash.
    m_process->Shut_Off_Below_Mouse = 1;
    const char *buf = fl_input("Preset Name?", "");
    m_process->Shut_Off_Below_Mouse = 0;

    if (!buf)
        return;
    
    std::string name = buf;

    // check for commas in the name, since the file is comma delimited
    std::size_t found = name.find_first_of(",");

    if(found != std::string::npos)
    {
        m_process->Handle_Message(51, name);
        return;
    }

    name.insert (0, "*" );

    if(name.length () > 64)
    {
        name.resize (64);
    }
    
    if(!check_insert_duplicate(w, name))
    {
        if(m_process->save_insert_preset(effect, name))
        {
            add_insert_preset_name(w, name);
        }
    }
    else
    {
        m_process->Handle_Message(50, name);
    }
}

/**
 * Adds user defined 'Insert' preset name to preset drop down menu for an effect.
 * 
 * @param w
 *      The RKR_Choice widget to add the name.
 * 
 * @param name
 *      The user defined preset name to add.
 */
void RKRGUI::add_insert_preset_name(Fl_Widget *w, const std::string &name)
{
    RKR_Choice *s = static_cast<RKR_Choice *> (w);

    s->add(name.c_str ());

    Fl_Menu_Item *m = const_cast<Fl_Menu_Item*>  (s->menu ());
    Fl_Menu_Item *item;
    
    int font_size = C_DEFAULT_FONT_SIZE;

    for (int i = 0; i < m->size(); i++)
    {
        item = m->next(i);

        if (i == 0)
        {
            font_size = item->labelsize();
        }
        
        item->labelsize(font_size);
        item->labelfont (global_font_type);
    }
}

bool RKRGUI::check_insert_duplicate(Fl_Widget *w, const std::string &name)
{
    RKR_Choice *s = static_cast<RKR_Choice *> (w);

    Fl_Menu_Item *m0 = const_cast<Fl_Menu_Item*>  (s->menu ());

    bool is_duplicate = false;

    for (int i = 0; i < m0->size(); i++)
    {
        Fl_Menu_Item *p0 = m0->next(i);
        
        if(p0->label() != NULL)
        {
            if(strcmp(p0->label(), name.c_str ()) == 0)
            {
                is_duplicate = true;
            }
        }
    }

    return is_duplicate;
}

/**
 * Gets the 'Preset' RKR_Choice widget for a given effect.
 * 
 * @param effect
 *      The EFX_Index effect number that is requested.
 * 
 * @return 
 *      The widget (RKR_Choice) for the requested effect.
 */
Fl_Widget * RKRGUI::find_effect_preset_widget(int effect)
{
    for (int t = 0; t < Principal->children(); t++)
    {
        Fl_Widget *w = Principal->child(t);
        long long effect_user_data = (long long) w->user_data();

        if (effect_user_data == UD_Group_Efx)
        {
            Fl_Group *g = static_cast<Fl_Group *> (w);

            for (int i = 0; i < g->children(); i++)
            {
                Fl_Widget *c = g->child(i);
                long long preset_user_data = (long long) c->user_data();
                if (preset_user_data == (effect + UD_PRESET_EQ))  // UD_PRESET_EQ is start efx user_data
                    return c;
            }
        }
    }

    return NULL;
}

/**
 * Reads the insert presets file : "InsertPresets.rkis" in user directory
 * and adds them to the effect preset list.
 * 
 * @param location
 *      The file location to read from. Only used when importing/merging from
 *      another user file.
 */
void RKRGUI::read_insert_presets(const std::string &location)
{
    std::string insert_preset_location = "";
    
    // Did the user set a User Directory
    if( (strcmp(m_process->Config.UDirFilename, DATADIR) != 0) && (strcmp(m_process->Config.UDirFilename, UD_NOT_SET) != 0) )
    {
        insert_preset_location = m_process->Config.UDirFilename;
        
        if(insert_preset_location[insert_preset_location.size() - 1] != '/')
            insert_preset_location += "/";

        insert_preset_location += "InsertPresets.rkis";
    }
    else
    {
        printf("Custom User Directory is not set: Go to Settings/Preferences/User to change.\n");
        return;
    }
    
    // This would be from import/merge file. If empty we are loading from the user directory default
    if(!location.empty())
        insert_preset_location = location;

    // Read in user presets
    FILE *fn;
    char buf[256];
    char *sbuf;
    int effect = 0;

    if ((fn = fopen(insert_preset_location.c_str(), "r")) != NULL)
    {
        while (fgets(buf, sizeof buf, fn) != NULL)
        {
            sbuf = buf;
            sscanf(buf, "%d", &effect);
            char *name = strsep(&sbuf, ",");  // return not used, incrementing the buffer
            name = strsep(&sbuf, ",");

            Fl_Widget * w = find_effect_preset_widget(effect);

            if(!check_insert_duplicate(w, name))
            {
                if(!location.empty())
                {
                    // merge insert presets must be saved
                    if(m_process->save_insert_preset(effect, name))
                        add_insert_preset_name(w, name);
                }
                else
                {
                    // normal loading, no save, already in file
                    add_insert_preset_name(w, name);
                }
            }
            else    // duplicate name, don't load or save
            {
                std::string message = m_process->efx_names[Busca_Eff(effect)].Nom;
                message += " ";
                message += name;
                
                m_process->Handle_Message(50, message);
            }
        }

        fclose(fn);
    }
}

/**
 * Deletes user defined 'Insert' preset from list.
 * 
 * @param w
 *      The RKR_Choice preset widget from which the item will be deleted.
 * 
 * @param num
 *      The EFX_Index number of the related insert preset.
 */
inline void RKRGUI::delete_insert_preset(Fl_Widget *w, int effect)
{
    // Cabinet does not allow insert presets
    if (effect == EFX_CABINET)
        return;

    RKR_Choice *preset = static_cast<RKR_Choice*> (w);
    
    // Check if user defined preset
    if (strncmp(preset->text(), "*", 1) != 0)
    {
        m_process->Handle_Message(39);
        return;
    }
    
    // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
    m_process->Shut_Off_Below_Mouse = 1;
    int ok = fl_choice("Delete \"%s\"?", "No", "Yes", NULL, preset->text());
    m_process->Shut_Off_Below_Mouse = 0;

    if (!ok)
    {
        return;
    }

    std::string Sname = preset->text();
    preset->remove(preset->value());
    preset->value(0);
    preset->redraw();

    m_process->delete_insert_preset(effect, Sname);
}

void RKRGUI::Prep_Reorden(int source, int dest)
{
    // Prepare Reorden
    int x = m_process->efx_order[source];
    int y = m_process->efx_order[dest];

    m_process->efx_order[source] = y;
    m_process->efx_order[dest] = x;

    reordena();

    if (Order->visible())
        Prepare_Order();
}

void RKRGUI::bank_click(Fl_Choice* o, void* v)
{
    ((RKRGUI*) (o->parent()->parent()->parent()->parent()->user_data()))->bank_click_i(o, v);
}

inline void RKRGUI::bank_click_i(Fl_Choice* o, void*)
{
    long long kk = (long long) o->user_data();
    int num = (int) kk;

    m_process->MIDI_Table[num - UD_Bank_Used_Start].bank = (int) o->value();
    Settings->fill_mptable(num + UD_Bank_Used_Start, m_process->MIDI_Table[num - UD_Bank_Used_Start].bank);
    m_process->PG_table_modified = 1;
}

void RKRGUI::p_click(Fl_Choice* o, void* v)
{
    ((RKRGUI*) (o->parent()->parent()->parent()->parent()->user_data()))->p_click_i(o, v);
}

inline void RKRGUI::p_click_i(Fl_Choice* o, void*)
{
    long long kk = (long long) o->user_data();
    int num = (int) kk;

    m_process->MIDI_Table[num - UD_Preset_Used_Start].preset = o->value();
    m_process->PG_table_modified = 1;
}

void RKRGUI::Set_effect(RKR_Check_Button* o, void* v)
{
    ((RKRGUI*)(o->parent()->parent()->user_data()))->cb_Set_effect_i(o,v);
}

inline void RKRGUI::cb_Set_effect_i(RKR_Check_Button* o, void* v)
{
    long long ud = (long long) v;
    
    ud -= UD_random_edit;

    FX_Excluded[ud] = (char) o->value();
}

void RKRGUI::RandomPreset()
{
    if(use_current_active_efx)
    {
        for(int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
        {
            int rack_effect = m_process->efx_order[i];
            if(m_process->EFX_Active[rack_effect])
            {
                if(random_parameters)
                {
                    set_random_parameters(i);
                }
                else
                {
                    // Get the effect preset size and select a random effect preset
                    Fl_Widget *w = find_effect_preset_widget(rack_effect);
                    RKR_Choice *preset_widget = static_cast<RKR_Choice *> (w);

                    long long widget_user_data = (long long) preset_widget->user_data();
                    int preset_selection = (int) (RND * preset_widget->size());
                    preset_widget->value(preset_selection);
                    preset_widget->do_callback(w, widget_user_data);
                }
            }
        }
        return;
    }
    
    // Check if invalid number of effects are excluded
    int excluded = 0;
    for(unsigned e = 0; e < C_NUMBER_EFFECTS; ++e)
    {
        if(FX_Excluded[e])
            excluded++;
    }

    if(excluded > C_MAX_EXCLUDED)
    {
        m_process->Handle_Message(49, NTS(excluded));
        return; // don't generate until they fix it
    }
    
    // Reset all effects to defaults
    S_new->do_callback();

    // Array to hold selected effect index
    int Effect_Index[10];

    // Get the first one to compare for duplicates
    while(1)
    {
        Effect_Index[0] = (int) (RND * C_NUMBER_EFFECTS);

        // Did the user want this to be selected
        if(!FX_Excluded[Effect_Index[0]])
        {
            break;  // it is a good one
        }
    }

    // Get random selection of effect index
    for (int i = 1; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        int l = 0;
        while (l == 0)
        {
            Effect_Index[i] = (int) (RND * C_NUMBER_EFFECTS);
            for (int j = 0; j < i; j++)
            {
                // Check that there are no duplicate selections
                if (Effect_Index[j] == Effect_Index[i])
                {
                    l = 0;  // duplicate selection, continue while()
                    break;
                }
                else
                {
                    bool dont_use = false;

                    // Check if the user wants this effect chosen
                    for (int k = 0; k < C_NUMBER_EFFECTS; k++)
                    {
                        if (k == Effect_Index[i])
                        {
                            if(FX_Excluded[k])
                            {
                                l = 0;  // user does not want this one
                                dont_use = true;
                            }
                            break;
                        }
                    }

                    if(dont_use)
                    {
                        break; // from for loop
                    }
                    else
                    {
                        l = 1;  // got all valid new selections, break from while()
                    }
                }
            }
        }
    }

    // Set the main window order based on random selections
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        m_process->efx_order[i] = Effect_Index[i];
    }
    
    // Get random number of active effects, max of six
    int number_active_effects = max_random_active;
    if(!efx_always_active)
    {
        number_active_effects = (int) (RND * max_random_active) + 1;
    }

    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        int rack_effect = m_process->efx_order[i];

        // Set the main window effects active or inactive
        if (i < number_active_effects)
        {
            m_process->EFX_Active[rack_effect] = 1;
        }
        else
        {
            m_process->EFX_Active[rack_effect] = 0;
        }
        
        Efx_Gui_Base[rack_effect]->activate_effect->value (m_process->EFX_Active[rack_effect]);
        
        if(random_parameters)
        {
            set_random_parameters(i);
        }
        else
        {
            // Get the effect preset size and select a random effect preset
            Fl_Widget *w = find_effect_preset_widget(Effect_Index[i]);
            RKR_Choice *preset_widget = static_cast<RKR_Choice *> (w);

            long long widget_user_data = (long long) preset_widget->user_data();
            int preset_selection = (int) (RND * preset_widget->size());
            preset_widget->value(preset_selection);
            preset_widget->do_callback(w, widget_user_data);
        }
    }

    FillML();
    Prepare_Order();
    Put_Loaded();

    ActivarGeneral->value(1);
    ActivarGeneral->do_callback();
}

void RKRGUI::set_random_parameters(int effect)
{
    int efx = m_process->efx_order[effect];

    m_process->Rack_Effects[efx]->set_random_parameters();

    // needed for cabinet because cabinet uses Active_Preset for parameter_refresh()
    if (efx == EFX_CABINET)
    {
        m_process->Active_Preset.Effect_Params[EFX_CABINET][0] = m_process->Rack_Effects[EFX_CABINET]->getpar(0);
        m_process->Active_Preset.Effect_Params[EFX_CABINET][1] = m_process->Rack_Effects[EFX_CABINET]->getpar(1);
    }
    
    int parameters = m_process->Rack_Effects[efx]->get_number_efx_parameters();
    
    if (efx == EFX_VARYBAND)
        parameters += 4;    // For un-combined volume bands 

    for(int i = 0; i < parameters; ++i)
    {
        Efx_Gui_Base[efx]->parameter_refresh (i);
    }
}

void RKRGUI::set_parameters_random(int value)
{
    random_parameters = value;
}

void RKRGUI::set_all_random_active(int value)
{
    efx_always_active = value;
}

void RKRGUI::set_random_current(int value)
{
    use_current_active_efx = value;
}

void RKRGUI::set_max_random(int value)
{
    max_random_active = value;
}

void RKRGUI::drag_effect()
{
    if ((drag != C_NO_DRAG) && (Fl::event_button1() == 0))
    {
        Fl_Widget *widget_belowmouse = Fl::belowmouse();
        if (widget_belowmouse != NULL)
        {
            long long widget_user_data = (long long) widget_belowmouse->user_data();

            if ((widget_user_data < UD_Label_1) || (widget_user_data > UD_Label_10))
            {
                return;
            }

            // w->user_data() is Label in range 770 to 779
            // which is checked above, so this is valid range (10) for
            // m_process->efx_order[C_NUMBER_ORDERED_EFFECTS] used by Prep_Reorden(source, dest)
            if (drag != (int) widget_user_data - UD_Label_1)
            {
                Prep_Reorden(drag, (int) widget_user_data - UD_Label_1);
            }
        }
        drag = C_NO_DRAG;
    }
}

void RKRGUI::set_bank_file()
{
    // get bank file
    char *filename;
    is_bank_modified();
    
    // If nothing previously set, then default location
    std::string chooser_start_location = "";
    
    // Did the user set a User Directory
    if( (strcmp(m_process->Config.UDirFilename, DATADIR) != 0) && (strcmp(m_process->Config.UDirFilename, UD_NOT_SET) != 0) )
    {
        chooser_start_location = m_process->Config.UDirFilename;
    }

    filename = fl_file_chooser("Load Bank File:", "(*.rkrb)", chooser_start_location.c_str (), 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, ".rkrb");
    int ok = m_process->load_bank_from_vector(filename);

    if (ok)
    {
        BankWin_Label(filename);
        Put_Loaded_Bank();
    }
}

void RKRGUI::set_save_file()
{
    // If nothing previously set, then default location
    std::string chooser_start_location = m_process->Bank_Saved;
    
    // If we do not have a previous file, then try User Directory
    if(chooser_start_location.empty ())
    {
        // Did the user set a User Directory
        if( (strcmp(m_process->Config.UDirFilename, DATADIR) != 0) && (strcmp(m_process->Config.UDirFilename, UD_NOT_SET) != 0) )
        {
            chooser_start_location = m_process->Config.UDirFilename;
        }
    }

    char *filename;

#define EXT ".rkrb"
    filename = fl_file_chooser("Save Bank File:", "(*" EXT")", chooser_start_location.c_str (), 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, EXT);
#undef EXT
    int ok = m_process->save_bank(filename, m_process->Bank);
    
    if (ok)
    {
        // Reload the bank vector for the new file or update the existing
        Scan_Bank_Dir(1);
        
        // Check if the user saved outside the user directory
        int in_user_directory = -1;
        for(unsigned i = 0; i < m_process->Bank_Vector.size (); i++)
        {
            if(strcmp(filename, m_process->Bank_Vector[i].Bank_File_Name.c_str()) == 0)
            {
                in_user_directory = i;
                break;
            }
        }
        
        if(in_user_directory >= 0)
        {
            m_process->active_bank = in_user_directory;

            // Save the Bank name for next file saving
            m_process->Bank_Saved = filename;
            BankWin_Label(filename);
        }
        else
        {
            // since we did not save in the user directory, we do not update
            // the active bank, and leave the previous in place. We reset the
            // modified flag for the current bank since save_bank() cleared it.
            m_process->bank_modified = 1;

            // Message telling user to put the file in the user directory
            m_process->Handle_Message (40, filename);
        }
    }
}

void RKRGUI::set_bank_made(int a_made)
{
    // set bank made
    made = a_made;
}

int RKRGUI::get_bank_made()
{
    // get bank made
    return made;
}

void RKRGUI::Load_Midi_Program_Change_Table()
{
    // If nothing previously set, then default location
    std::string chooser_start_location = "";
    
    // If the user set a User Directory, then use it
    if( (strcmp(m_process->Config.UDirFilename, DATADIR) != 0) && (strcmp(m_process->Config.UDirFilename, UD_NOT_SET) != 0) )
    {
        chooser_start_location = m_process->Config.UDirFilename;
    }
    
    char *filename;
    filename = fl_file_chooser("Load MIDI Table:", "(*.rmt)", chooser_start_location.c_str (), 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, ".rmt");

    // Check to see if the file is in the User Directory.
    // This will set the m_process->Config.custom_midi_table_file to the file if found.
    if(!m_process->file_in_midi_table_vector(filename))
    {
        m_process->Handle_Message (42, filename);
        return;     // Do not load it!!!!
    }
    else
    {
        // Use the Midi_Table_Vector to load it
        m_process->set_midi_table (m_process->Config.custom_midi_table_file);
    }

    // Put it in the scroll table
    Settings->Put_MidiTable();
}

void RKRGUI::Save_Midi_Program_Change_Table()
{
    // If nothing previously set, then default location
    std::string chooser_start_location = "";
    
    // If the user set a User Directory, then use it
    if( (strcmp(m_process->Config.UDirFilename, DATADIR) != 0) && (strcmp(m_process->Config.UDirFilename, UD_NOT_SET) != 0) )
    {
        chooser_start_location = m_process->Config.UDirFilename;

        // If we have a previous file, then use it
        if(m_process->Config.custom_midi_table_file >= 0)
        {
             if(m_process->Config.custom_midi_table_file < (int)m_process->Midi_Table_Vector.size ())
             {
                 chooser_start_location = m_process->Midi_Table_Vector[m_process->Config.custom_midi_table_file].Table_File_Name;
             }
        }
    }

    char *filename;
#define EXT ".rmt"
    filename = fl_file_chooser("Save MIDI Table:", "(*" EXT")", chooser_start_location.c_str (), 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, EXT);
#undef EXT

    // We save the file anywhere they want, but give an error message if
    // it is not in the User Directory.
    m_process->save_MIDI_table(filename);

    // rescan the User Directory for any new file, or refresh existing one
    m_process->load_MIDI_table_vector();

    // Check to see if the file is in the User Directory.
    // This will set the m_process->Config.custom_midi_table_file to the file if found.
    if(!m_process->file_in_midi_table_vector(filename))
    {
        m_process->Handle_Message (42, filename);
    }
}

int RKRGUI::NSM_gui_show(int hold_preset)
{
#ifdef NSM_SUPPORT
    // To update the Gui for any MIDI changes

    /* For cabinet we need to update active preset on return from hide to refresh the gui */
    m_process->Active_Preset.Effect_Params[EFX_CABINET][0] = m_process->Rack_Effects[EFX_CABINET]->getpar(0);
    m_process->Active_Preset.Effect_Params[EFX_CABINET][1] = m_process->Rack_Effects[EFX_CABINET]->getpar(1);

    Put_Loaded();
    Put_Loaded_Bank();

    if(hold_preset != C_CHANGE_PRESET_OFF)
    {
        BankWindow->unlight_preset(m_process->Selected_Preset);
        BankWindow->light_preset(hold_preset);
        Preset_Counter->value(hold_preset);
        m_process->Selected_Preset = hold_preset;
        hold_preset = C_CHANGE_PRESET_OFF;
    }
    Principal->show();

    global_gui_show = CONST_GUI_OFF;
    m_process->Gui_Shown = 1;

    // Need to reset OnOffC because the value is not adjusted or
    // reset when the gui is hidden. If not reset, then it can
    // result in an out of range.. segfault. Since this is used
    // for efx_order[] array location.
    m_process->OnOffC = 0;
#endif

    return hold_preset;
}

void RKRGUI::NSM_gui_hide()
{
#ifdef NSM_SUPPORT
    m_process->Gui_Shown = 0;
    is_bank_modified();
    is_PG_table_modified();

    BankWindow->hide();
    Order->hide();
    Settings->hide();
    AboutWin->hide();
    MIDILearn->hide();
    Trigger->hide();
    Principal->hide();
    DelayFile->hide();
    RandomEdit->hide();
    Fl::flush();
    global_gui_show = CONST_GUI_OFF;
#endif
}
