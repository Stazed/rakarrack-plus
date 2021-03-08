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
static volatile int got_sigint = 0;
static volatile int got_sigusr1 = 0;

/**
 * This local RKR pointer is used exclusively with RKRGUI class static function.
 */
RKR * process_rkr;

RKRGUI::RKRGUI(int argc, char**argv, RKR *rkr_) :
    made(0),
    focus_delay_time(25)    // Every 25 count is about 1 second
{
    // Initialize Gui
    Fl::args(argc, argv);
    fl_open_display();
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), (char **) icono_rakarrack_128x128, &p, &mask, NULL);

    Fl::visual(FL_DOUBLE | FL_RGB);
    fl_register_images();
    Fl::set_fonts(0);

    m_process = process_rkr = rkr_;

    mBankNameList = NULL;
    mBankNameListTail = NULL;
    back = NULL;

    make_window();

    Principal->icon((char *) p);
    BankWindow->icon((char *) p);
    Order->icon((char *) p);
    Settings->icon((char *) p);
    AboutWin->icon((char *) p);
    MIDILearn->icon((char *) p);
    Trigger->icon((char *) p);

    char tmp[256];
    Analy->set_analyzer_ON(false);
    Sco->set_scope_ON(false);

    Sco->init(m_process->anall, m_process->analr, m_process->period, this);
    Analy->init(m_process->anall, m_process->analr, m_process->period, m_process->sample_rate, this);

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
    BankWin_Label(m_process->BankFilename);
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
    
    load_previous_state();
    Settings->make_table_window();
    Put_Loaded();
    Principal->show(argc, argv);
    put_icon(Principal);
    read_insert_presets();
    
    if (m_process->deachide)
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
    install_signal_handlers();
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
    // Main gui process on timeout
    below_mouse_highlight_and_focus();
    drag_effect();
    check_signals(this);

    if (m_process->Tuner_Bypass)
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

    if (m_process->Change_Bank != C_BANK_CHANGE_OFF)
    {
        BankWin_Label(m_process->Bank_Vector[m_process->Change_Bank].Bank_File_Name.c_str ());

        if (m_process->a_bank != m_process->Change_Bank)
        {
            m_process->a_bank = m_process->Change_Bank;

            Put_Loaded_Bank();
        }
        
        m_process->Change_Bank = C_BANK_CHANGE_OFF;
    }

    if (m_process->Change_Preset != C_CHANGE_PRESET_OFF)
    {
        if (!m_process->midi_table)
        {
            Preset_Counter->value(m_process->Change_Preset);
            Preset_Counter->do_callback();
        }
        else
        {
            if (m_process->a_bank != m_process->M_table[m_process->Change_Preset].bank)
            {
                switch (m_process->M_table[m_process->Change_Preset].bank)
                {
                    case 0:
                        L_B1->do_callback();
                        break;

                    case 1:
                        L_B2->do_callback();
                        break;

                    case 2:
                        L_B3->do_callback();
                        break;

                    case 3:
                        L_B4->do_callback();
                        break;
                }
            }
            Preset_Counter->value(m_process->M_table[m_process->Change_Preset].preset + 1);
            Preset_Counter->do_callback();
        }

        m_process->Change_Preset = C_CHANGE_PRESET_OFF;
    }


    MIDI_control_gui_refresh();
    if (m_process->ACI_Bypass)
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
        m_process->Error_Handle(global_error_number);

    if (m_process->Tap_Bypass)
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


    if (m_process->Bypass)
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

        if (m_process->EFX_Bypass[EFX_LOOPER])
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


    if ((m_process->Metro_Bypass) && (MetroSound->value() == 0))
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


    if (m_process->MIDIConverter_Bypass)
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


    if (m_process->EFX_Bypass[EFX_HARMONIZER])
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


    if (m_process->EFX_Bypass[EFX_STEREOHARM])
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
            if ((m_process->comemouse) && (m_process->autoassign))
            {
                m_process->comemouse = 0;
                MIDILearn->Assign->do_callback();
            }
        }
    };
    
    if(m_process->change_scale)
    {
        m_process->change_scale = 0;  // false
        if (m_process->scalable )
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

int RKRGUI::increment_look_changed ()
{
    return global_look_changed > 10 ? 0 : (global_look_changed + 1);
}

void RKRGUI::back_color_change(Fl_Color back_color)
{
    global_back_color = back_color;
    
    // Increment look changed flag
    global_look_changed = increment_look_changed ();
    
    if (!m_process->EnableBackgroundImage)
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
    global_look_changed = increment_look_changed ();

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::buttons_color_change(Fl_Color buttons_color)
{
    global_fore_color = buttons_color;
    
    // Increment look changed flag
    global_look_changed = increment_look_changed ();

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::leds_color_change(Fl_Color leds_color)
{
    global_leds_color = leds_color;

    // Increment look changed flag
    global_look_changed = increment_look_changed ();

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::font_size_change(int font_size)
{
    global_font_size = font_size;

    // Increment look changed flag
    global_look_changed = increment_look_changed ();

    ChangeActives();
    Fl::redraw();
}

void RKRGUI::font_type_change (int font_type)
{
    global_font_type = font_type;

    // Increment look changed flag
    global_look_changed = increment_look_changed ();

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
    int x, y, w, h, f, k, back_color, fore_color, leds_color, label_color, font_size, font_type;
    x = y = w = h = f = k = back_color = fore_color = leds_color = label_color = font_size = font_type = 0;

    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    rakarrack.get(m_process->PrefNom("Principal X"), x, 1);
    rakarrack.get(m_process->PrefNom("Principal Y"), y, 1);
    rakarrack.get(m_process->PrefNom("Principal W"), w, 960);
    rakarrack.get(m_process->PrefNom("Principal H"), h, 540);

    char tmp[256];
    sprintf(tmp, "%s/blackbg.png", DATADIR);

    rakarrack.get(m_process->PrefNom("Enable Background Image"), m_process->EnableBackgroundImage, 0);
    rakarrack.get(m_process->PrefNom("Background Image"), m_process->BackgroundImage, tmp, 256);
    PutBackground();
    rakarrack.get(m_process->PrefNom("FontSize"), font_size, C_DEFAULT_FONT_SIZE);
    
    // Fonts that look good - Cantarell Bold, Computer Modern Bright Bold, DejaVu Sans Condensed
    // Free Helvetian, FreeSans, Garuda, Ubuntu, Verana Sans
    rakarrack.get(m_process->PrefNom("Font"), font_type, 0);

    rakarrack.get(m_process->PrefNom("Background Color"), back_color, 56);              // FL_BLACK
    rakarrack.get(m_process->PrefNom("Foreground Color"), fore_color, 1397969664);      // FL_DARK3
    rakarrack.get(m_process->PrefNom("Leds Color"), leds_color, 2140209152);            // FL_DARK_YELLOW
    rakarrack.get(m_process->PrefNom("Labels Color"), label_color, 255);                // FL_WHITE
    rakarrack.get(m_process->PrefNom("Schema"), k, 2);
    Settings->scheme_ch->value(k);
    Settings->scheme_ch->do_callback();
    rakarrack.get(m_process->PrefNom("Hide Effects"), m_process->deachide, 0);
    rakarrack.get(m_process->PrefNom("Scale Window"), m_process->scalable, 0);

    Principal->resize(x, y, w, h);

    rakarrack.get(m_process->PrefNom("BankWindow X"), x, 1);
    rakarrack.get(m_process->PrefNom("BankWindow Y"), y, 1);
    rakarrack.get(m_process->PrefNom("BankWindow W"), w, 680);
    rakarrack.get(m_process->PrefNom("BankWindow H"), h, 425);

    BankWindow->resize(x, y, w, h);

    rakarrack.get(m_process->PrefNom("Order X"), x, 1);
    rakarrack.get(m_process->PrefNom("Order Y"), y, 1);
    rakarrack.get(m_process->PrefNom("Order W"), w, 600);
    rakarrack.get(m_process->PrefNom("Order H"), h, 330);

    Order->resize(x, y, w, h);

    rakarrack.get(m_process->PrefNom("MIDI Learn X"), x, 1);
    rakarrack.get(m_process->PrefNom("MIDI Learn Y"), y, 1);
    rakarrack.get(m_process->PrefNom("MIDI Learn W"), w, 600);
    rakarrack.get(m_process->PrefNom("MIDI Learn H"), h, 360);

    MIDILearn->resize(x, y, w, h);

    rakarrack.get(m_process->PrefNom("Trigger X"), x, 1);
    rakarrack.get(m_process->PrefNom("Trigger Y"), y, 1);
    rakarrack.get(m_process->PrefNom("Trigger W"), w, 280);
    rakarrack.get(m_process->PrefNom("Trigger H"), h, 165);

    Trigger->resize(x, y, w, h);

    rakarrack.get(m_process->PrefNom("Settings X"), x, 1);
    rakarrack.get(m_process->PrefNom("Settings Y"), y, 1);
    rakarrack.get(m_process->PrefNom("Settings W"), w, 440);
    rakarrack.get(m_process->PrefNom("Settings H"), h, 410);

    Settings->resize(x, y, w, h);

    back_color_change((Fl_Color)back_color);
    buttons_color_change((Fl_Color)fore_color);
    leds_color_change((Fl_Color)leds_color);
    label_color_change((Fl_Color)label_color);
    font_size_change(font_size);
    font_type_change(font_type);

    rakarrack.get(m_process->PrefNom("Bank Selected"), m_process->a_bank, 3);

    if (!m_process->Bank_Load_Command_Line)
    {
        switch (m_process->a_bank)
        {
            case 0:
                L_B1->do_callback();
                break;

            case 1:
                L_B2->do_callback();
                break;

            case 2:
                L_B3->do_callback();
                break;
        }
    }
    else
    {
        BankWin_Label(m_process->Command_Line_Bank.c_str());
    }

    if (!m_process->Command_Line_File)
    {
        rakarrack.get(m_process->PrefNom("Preset Num"), k, 1);
        m_process->Selected_Preset = k;
        Preset_Counter->value(k);
        Preset_Counter->do_callback();
    }

    // MIDI Learn
    if (!m_process->MIDIway)
        ML_Menu->deactivate();

    // Custom MIDI table
    rakarrack.get(m_process->PrefNom("MIDI Table"), m_process->midi_table, 0);
    if (!m_process->midi_table)
        Settings->scroll->deactivate();


    rakarrack.get(m_process->PrefNom("UserName"), m_process->UserRealName, "", 127);
    rakarrack.get(m_process->PrefNom("User Directory"), m_process->UDirFilename, DATADIR, 127);
    rakarrack.get(m_process->PrefNom("Preserve Gain/Master"), m_process->actuvol, 0);
    rakarrack.get(m_process->PrefNom("Metronome Volume"), m_process->Metro_Vol, 50);
    
    Looper *Efx_Looper = static_cast <Looper*> (m_process->Rack_Effects[EFX_LOOPER]);
    Efx_Looper->setmvol(m_process->Metro_Vol);

    rakarrack.get(m_process->PrefNom("Update Tap"), m_process->Tap_Updated, 0);
    rakarrack.get(m_process->PrefNom("MIDI IN Channel"), m_process->MidiCh, 1);
    m_process->MidiCh--;

    rakarrack.get(m_process->PrefNom("MIDI IN Harmonizer"), m_process->HarCh, 1);
    m_process->HarCh--;
    rakarrack.get(m_process->PrefNom("MIDI IN Stereo Harmonizer"), m_process->StereoHarCh, 1);
    m_process->StereoHarCh--;

    rakarrack.get(m_process->PrefNom("Limiter Position"), m_process->flpos, 0);
    rakarrack.get(m_process->PrefNom("Limiter +6dB"), m_process->db6booster, 0);
    rakarrack.get(m_process->PrefNom("Booster"), m_process->booster, 1.0f);
    if (m_process->booster == 1.0)
    {
        BostBut->value(0);
    }
    else
    {
        BostBut->value(1);
    }

    rakarrack.get(m_process->PrefNom("FX_init_state"), m_process->init_state, 0);
    rakarrack.get(m_process->PrefNom("Auto Assign"), m_process->autoassign, 0);


    if (m_process->init_state)
    {
        m_process->Bypass = 1;
        m_process->calculavol(1);
        m_process->calculavol(2);
    }


    rakarrack.get(m_process->PrefNom("Recognize Optimization Harmonizer"), m_process->RCOpti_Harm, 0);
    Settings->RC_Harm_Opti->value(m_process->RCOpti_Harm);
    Settings->RC_Harm_Opti->do_callback();

    rakarrack.get(m_process->PrefNom("Recognize Optimization Stereo Harm"), m_process->RCOpti_Stereo, 0);
    Settings->RC_St_Harm_Opti->value(m_process->RCOpti_Stereo);
    Settings->RC_St_Harm_Opti->do_callback();

    rakarrack.get(m_process->PrefNom("Recognize Optimization Ring"), m_process->RCOpti_Ring, 0);
    Settings->RC_Ring_Opti->value(m_process->RCOpti_Ring);
    Settings->RC_Ring_Opti->do_callback();

    //MIDIConverter
    rakarrack.get(m_process->PrefNom("Midi Out Channel"), k, 1);
    MIDI->Midi_out_Counter->value(k);
    MIDI->Midi_out_Counter->do_callback();

    m_process->efx_MIDIConverter->setmidichannel(k);

    rakarrack.get(m_process->PrefNom("Trigger Adjust"), k, 4);
    MIDI->Trig_Adj->value(k);
    m_process->efx_MIDIConverter->setTriggerAdjust(k);


    rakarrack.get(m_process->PrefNom("Velocity Adjust"), k, 50);
    MIDI->Vel_Adj->value(k);
    m_process->efx_MIDIConverter->setVelAdjust(k);

    rakarrack.get(m_process->PrefNom("Converter Octave"), k, 2);
    MIDI->MIDIOctave->value(k);
    MIDI->MIDIOctave->do_callback();

    rakarrack.get(m_process->PrefNom("MIDI Converter On/Off"), k, 0);
    MIDI->midi_activar->value(k);
    MIDI->midi_activar->do_callback();

    //Metronome
    rakarrack.get(m_process->PrefNom("Internal Metronome Time"), k, 2);
    MetroBar->value(k);
    MetroBar->do_callback();

    rakarrack.get(m_process->PrefNom("Internal Metronome Sound"), k, 0);
    MetroSound->value(k);
    MetroSound->do_callback();

    rakarrack.get(m_process->PrefNom("Internal Metronome Volume"), k, 50);
    Metro_Volume->value(k);
    Metro_Volume->do_callback();

    rakarrack.get(m_process->PrefNom("Internal Metronome Tempo"), k, 100);
    Metro_Tempo->value(k);
    Metro_Tempo->do_callback();

    rakarrack.get(m_process->PrefNom("Internal Metronome Show"), m_process->sw_stat, 0);

    rakarrack.get(m_process->PrefNom("Internal Metronome On/Off"), k, 0);
    metro_activar->value(k);
    metro_activar->do_callback();

    if (m_process->sw_stat == 1)
    {
        MIDI->hide();
        Metro->show();
    }
    else
    {
        Metro->hide();
        MIDI->show();
    }

    //Tuner
    rakarrack.get(m_process->PrefNom("Tuner On/Off"), k, 0);
    tuner_activar->value(k);
    tuner_activar->do_callback();


    //Tap Tempo
    rakarrack.get(m_process->PrefNom("TapTempo Input"), m_process->Tap_Selection, 0);
    T_SEL->value(m_process->Tap_Selection);
    rakarrack.get(m_process->PrefNom("Tap Tempo Timeout"), m_process->t_timeout, 0);
    Settings->T_TIMEOUT->value(m_process->t_timeout);
    rakarrack.get(m_process->PrefNom("TapTempo Set"), m_process->Tap_SetValue, 0);
    T_SET->value(m_process->Tap_SetValue);

    rakarrack.get(m_process->PrefNom("TapTempo On/Off"), k, 0);
    Tap_activar->value(k);
    Tap_activar->do_callback();


    rakarrack.get(m_process->PrefNom("Disable Warnings"), m_process->Disable_Warnings, 0);
    rakarrack.get(m_process->PrefNom("Enable Tooltips"), m_process->ena_tool, 1);
    Fl_Tooltip::enable(m_process->ena_tool);
    
    rakarrack.get(m_process->PrefNom("Focus Delay"), m_process->Focus_Delay, 25);
    Settings->Focus_Slider->value (m_process->Focus_Delay);
    Settings->Focus_Slider->do_callback ();

    //Trigger
    rakarrack.get(m_process->PrefNom("Aux Source"), m_process->Aux_Source, 0);
    Trigger->aux_source->value(m_process->Aux_Source);
    rakarrack.get(m_process->PrefNom("Aux Gain"), m_process->Aux_Gain, 0);
    Trigger->aux_gain->value(m_process->Aux_Gain);
    rakarrack.get(m_process->PrefNom("Aux Threshold"), m_process->Aux_Threshold, 0);
    Trigger->aux_thres->value(m_process->Aux_Threshold);
    rakarrack.get(m_process->PrefNom("Aux MIDI"), m_process->Aux_MIDI, 1);
    Trigger->aux_midi->value(m_process->Aux_MIDI);
    rakarrack.get(m_process->PrefNom("Aux Minimum"), m_process->Aux_Minimum, 0);
    Trigger->aux_min->value(m_process->Aux_Minimum);
    rakarrack.get(m_process->PrefNom("Aux Maximum"), m_process->Aux_Maximum, 127);
    Trigger->aux_max->value(m_process->Aux_Maximum);

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
 *      4 = Not used
 *      5 = MIDI Learn
 *      6 = Trigger window (ACI)
 */
void RKRGUI::save_current_state(int whati)
{
    char temp1[128];

    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    if (whati == 0)
    {
        rakarrack.set(m_process->PrefNom("Principal X"), Principal->x());
        rakarrack.set(m_process->PrefNom("Principal Y"), Principal->y());
        rakarrack.set(m_process->PrefNom("Principal W"), Principal->w());
        rakarrack.set(m_process->PrefNom("Principal H"), Principal->h());
        rakarrack.set(m_process->PrefNom("FontSize"), global_font_size);
        rakarrack.set(m_process->PrefNom("Font"), global_font_type);

        rakarrack.set(m_process->PrefNom("Background Color"), (int) global_back_color);
        rakarrack.set(m_process->PrefNom("Foreground Color"), (int) global_fore_color);
        rakarrack.set(m_process->PrefNom("Leds Color"), (int) global_leds_color);
        rakarrack.set(m_process->PrefNom("Labels Color"), (int) global_label_color);
        rakarrack.set(m_process->PrefNom("Schema"), (int) Settings->scheme_ch->value());
        rakarrack.set(m_process->PrefNom("Hide Effects"), (int) m_process->deachide);
        rakarrack.set(m_process->PrefNom("Scale Window"), (int) m_process->scalable);

        rakarrack.set(m_process->PrefNom("Bank Selected"), m_process->a_bank);

        if ((Preset_Counter->value() > 0) && (Preset_Counter->value() < 61))
            rakarrack.set(m_process->PrefNom("Preset Num"), (int) Preset_Counter->value());

        if (m_process->help_displayed)
        {
            rakarrack.set(m_process->PrefNom("Help X"), visor->x());
            rakarrack.set(m_process->PrefNom("Help Y"), visor->y());
            rakarrack.set(m_process->PrefNom("Help W"), visor->w());
            rakarrack.set(m_process->PrefNom("Help H"), visor->h());
            rakarrack.set(m_process->PrefNom("Help TextSize"), visor->textsize());
        }

        //Tuner
        rakarrack.set(m_process->PrefNom("Tuner On/Off"), (int) m_process->Tuner_Bypass);


        //MIDIConverter
        rakarrack.set(m_process->PrefNom("MIDI Converter On/Off"), (int) m_process->MIDIConverter_Bypass);
        rakarrack.set(m_process->PrefNom("Midi Out Channel"), (int) MIDI->Midi_out_Counter->value());
        rakarrack.set(m_process->PrefNom("Trigger Adjust"), (int) MIDI->Trig_Adj->value());
        rakarrack.set(m_process->PrefNom("Velocity Adjust"), (int) MIDI->Vel_Adj->value());
        rakarrack.set(m_process->PrefNom("Converter Octave"), (int) MIDI->MIDIOctave->value());

        //Metronome
        rakarrack.set(m_process->PrefNom("Internal Metronome On/Off"), (int) m_process->Metro_Bypass);
        rakarrack.set(m_process->PrefNom("Internal Metronome Time"), (int) MetroBar->value());
        rakarrack.set(m_process->PrefNom("Internal Metronome Volume"), (int) Metro_Volume->value());
        rakarrack.set(m_process->PrefNom("Internal Metronome Tempo"), (int) Metro_Tempo->value());
        rakarrack.set(m_process->PrefNom("Internal Metronome Show"), (int) m_process->sw_stat);
        rakarrack.set(m_process->PrefNom("Internal Metronome Sound"), (int) MetroSound->value());

        //Booster
        rakarrack.set(m_process->PrefNom("Booster"), m_process->booster);


        //Tap Tempo
        rakarrack.set(m_process->PrefNom("TapTempo On/Off"), (int) m_process->Tap_Bypass);
        rakarrack.set(m_process->PrefNom("TapTempo Input"), (int) m_process->Tap_Selection);
        rakarrack.set(m_process->PrefNom("TapTempo Set"), (int) m_process->Tap_SetValue);
    }


    if (whati == 1)
    {
        rakarrack.set(m_process->PrefNom("BankWindow X"), BankWindow->x());
        rakarrack.set(m_process->PrefNom("BankWindow Y"), BankWindow->y());
        rakarrack.set(m_process->PrefNom("BankWindow W"), BankWindow->w());
        rakarrack.set(m_process->PrefNom("BankWindow H"), BankWindow->h());
    }

    if (whati == 5)
    {
        rakarrack.set(m_process->PrefNom("MIDI Learn X"), MIDILearn->x());
        rakarrack.set(m_process->PrefNom("MIDI Learn Y"), MIDILearn->y());
        rakarrack.set(m_process->PrefNom("MIDI Learn W"), MIDILearn->w());
        rakarrack.set(m_process->PrefNom("MIDI Learn H"), MIDILearn->h());
    }


    if (whati == 6)
    {
        rakarrack.set(m_process->PrefNom("Trigger X"), Trigger->x());
        rakarrack.set(m_process->PrefNom("Trigger Y"), Trigger->y());
        rakarrack.set(m_process->PrefNom("Trigger W"), Trigger->w());
        rakarrack.set(m_process->PrefNom("Trigger H"), Trigger->h());

        rakarrack.set(m_process->PrefNom("Aux Source"), m_process->Aux_Source);
        rakarrack.set(m_process->PrefNom("Aux Gain"), m_process->Aux_Gain);
        rakarrack.set(m_process->PrefNom("Aux Threshold"), m_process->Aux_Threshold);
        rakarrack.set(m_process->PrefNom("Aux MIDI"), m_process->Aux_MIDI);
        rakarrack.set(m_process->PrefNom("Aux Minimum"), m_process->Aux_Minimum);
        rakarrack.set(m_process->PrefNom("Aux Maximum"), m_process->Aux_Maximum);
    }




    if (whati == 2)
    {
        rakarrack.set(m_process->PrefNom("Order X"), Order->x());
        rakarrack.set(m_process->PrefNom("Order Y"), Order->y());
        rakarrack.set(m_process->PrefNom("Order W"), Order->w());
        rakarrack.set(m_process->PrefNom("Order H"), Order->h());
    }

    if (whati == 3)
    {
        rakarrack.set(m_process->PrefNom("Settings X"), Settings->x());
        rakarrack.set(m_process->PrefNom("Settings Y"), Settings->y());
        rakarrack.set(m_process->PrefNom("Settings W"), Settings->w());
        rakarrack.set(m_process->PrefNom("Settings H"), Settings->h());
    }

    if ((whati == 3) || (whati == 0))
    {
        rakarrack.set(m_process->PrefNom("UserName"), m_process->UserRealName);
        rakarrack.set(m_process->PrefNom("Preserve Gain/Master"), m_process->actuvol);
        rakarrack.set(m_process->PrefNom("Metronome Volume"), m_process->Metro_Vol);

        rakarrack.set(m_process->PrefNom("Filter DC Offset"), m_process->DC_Offset);

        rakarrack.set(m_process->PrefNom("Update Tap"), m_process->Tap_Updated);
        rakarrack.set(m_process->PrefNom("Limiter Position"), m_process->flpos);
        rakarrack.set(m_process->PrefNom("Limiter +6dB"), m_process->db6booster);

        rakarrack.set(m_process->PrefNom("Recognize Optimization Harmonizer"), m_process->RCOpti_Harm);
        rakarrack.set(m_process->PrefNom("Recognize Optimization Stereo Harm"), m_process->RCOpti_Stereo);
        rakarrack.set(m_process->PrefNom("Recognize Optimization Ring"), m_process->RCOpti_Ring);

        rakarrack.set(m_process->PrefNom("Harmonizer Downsample"), m_process->Har_Down);
        rakarrack.set(m_process->PrefNom("Harmonizer Up Quality"), m_process->Har_U_Q);
        rakarrack.set(m_process->PrefNom("Harmonizer Down Quality"), m_process->Har_D_Q);
        rakarrack.set(m_process->PrefNom("Reverbtron Downsample"), m_process->Rev_Down);
        rakarrack.set(m_process->PrefNom("Reverbtron Up Quality"), m_process->Rev_U_Q);
        rakarrack.set(m_process->PrefNom("Reverbtron Down Quality"), m_process->Rev_D_Q);
        rakarrack.set(m_process->PrefNom("Convolotron Downsample"), m_process->Con_Down);
        rakarrack.set(m_process->PrefNom("Convolotron Up Quality"), m_process->Con_U_Q);
        rakarrack.set(m_process->PrefNom("Convolotron Down Quality"), m_process->Con_D_Q);
        rakarrack.set(m_process->PrefNom("Sequence Downsample"), m_process->Seq_Down);
        rakarrack.set(m_process->PrefNom("Sequence Up Quality"), m_process->Seq_U_Q);
        rakarrack.set(m_process->PrefNom("Sequence Down Quality"), m_process->Seq_D_Q);
        rakarrack.set(m_process->PrefNom("Shifter Downsample"), m_process->Shi_Down);
        rakarrack.set(m_process->PrefNom("Shifter Up Quality"), m_process->Shi_U_Q);
        rakarrack.set(m_process->PrefNom("Shifter Down Quality"), m_process->Shi_D_Q);
        rakarrack.set(m_process->PrefNom("Vocoder Downsample"), m_process->Voc_Down);
        rakarrack.set(m_process->PrefNom("Vocoder Up Quality"), m_process->Voc_U_Q);
        rakarrack.set(m_process->PrefNom("Vocoder Down Quality"), m_process->Voc_D_Q);
        rakarrack.set(m_process->PrefNom("StereoHarm Downsample"), m_process->Ste_Down);
        rakarrack.set(m_process->PrefNom("StereoHarm Up Quality"), m_process->Ste_U_Q);
        rakarrack.set(m_process->PrefNom("StereoHarm Down Quality"), m_process->Ste_D_Q);

        rakarrack.set(m_process->PrefNom("Distortion Resampling"), m_process->Dist_res_amount);
        rakarrack.set(m_process->PrefNom("Distortion Up Quality"), m_process->Dist_up_q);
        rakarrack.set(m_process->PrefNom("Distortion Down Quality"), m_process->Dist_down_q);

        rakarrack.set(m_process->PrefNom("Overdrive Resampling"), m_process->Ovrd_res_amount);
        rakarrack.set(m_process->PrefNom("Overdrive Up Quality"), m_process->Ovrd_up_q);
        rakarrack.set(m_process->PrefNom("Overdrive Down Quality"), m_process->Ovrd_down_q);

        rakarrack.set(m_process->PrefNom("Derelict Resampling"), m_process->Dere_res_amount);
        rakarrack.set(m_process->PrefNom("Derelict Up Quality"), m_process->Dere_up_q);
        rakarrack.set(m_process->PrefNom("Derelict Down Quality"), m_process->Dere_down_q);
        
        rakarrack.set(m_process->PrefNom("DistBand Resampling"), m_process->DBand_res_amount);
        rakarrack.set(m_process->PrefNom("DistBand Up Quality"), m_process->DBand_up_q);
        rakarrack.set(m_process->PrefNom("DistBand Down Quality"), m_process->DBand_down_q);

        rakarrack.set(m_process->PrefNom("StompBox Resampling"), m_process->Stomp_res_amount);
        rakarrack.set(m_process->PrefNom("StompBox Up Quality"), m_process->Stomp_up_q);
        rakarrack.set(m_process->PrefNom("StompBox Down Quality"), m_process->Stomp_down_q);

        rakarrack.set(m_process->PrefNom("Calibration"), m_process->aFreq);
        rakarrack.set(m_process->PrefNom("Recognize Trigger"), m_process->rtrig);

        rakarrack.set(m_process->PrefNom("Vocoder Bands"), m_process->VocBands);

        rakarrack.set(m_process->PrefNom("FX_init_state"), m_process->init_state);
        rakarrack.set(m_process->PrefNom("Auto Assign"), m_process->autoassign);

        rakarrack.set(m_process->PrefNom("UpSampling"), m_process->upsample);
        rakarrack.set(m_process->PrefNom("UpQuality"), m_process->UpQual);
        rakarrack.set(m_process->PrefNom("DownQuality"), m_process->DownQual);

        rakarrack.set(m_process->PrefNom("UpAmount"), m_process->UpAmo);
        rakarrack.set(m_process->PrefNom("Looper Size"), m_process->looper_size);

        rakarrack.set(m_process->PrefNom("Bank Filename"), m_process->BankFilename);
        rakarrack.set(m_process->PrefNom("User Directory"), m_process->UDirFilename);

        rakarrack.set(m_process->PrefNom("Enable Background Image"), m_process->EnableBackgroundImage);
        rakarrack.set(m_process->PrefNom("Background Image"), m_process->BackgroundImage);
        rakarrack.set(m_process->PrefNom("Auto Connect MIDI IN"), m_process->aconnect_MI);
        rakarrack.set(m_process->PrefNom("Auto Connect Jack"), m_process->aconnect_JA);
        rakarrack.set(m_process->PrefNom("Auto Connect Jack In"), m_process->aconnect_JIA);

        rakarrack.set(m_process->PrefNom("MIDI Implementation"), m_process->MIDIway);
        rakarrack.set(m_process->PrefNom("MIDI Table"), m_process->midi_table);


        int i = Settings->BMidiIn->value();
        char *temp;
        temp = (char*) Settings->BMidiIn->text(i);
        
        if (temp != NULL)
        {
            rakarrack.set(m_process->PrefNom("MIDI IN Device"), temp);
        }
        
        rakarrack.set(m_process->PrefNom("MIDI IN Channel"), m_process->MidiCh + 1);
        rakarrack.set(m_process->PrefNom("MIDI IN Harmonizer"), m_process->HarCh + 1);
        rakarrack.set(m_process->PrefNom("MIDI IN Stereo Harmonizer"), m_process->StereoHarCh + 1);
        rakarrack.set(m_process->PrefNom("Harmonizer Quality"), m_process->HarQual);
        rakarrack.set(m_process->PrefNom("StereoHarm Quality"), m_process->SteQual);
        rakarrack.set(m_process->PrefNom("Sequence Quality"), m_process->SeqQual);
        rakarrack.set(m_process->PrefNom("Shifter Quality"), m_process->ShiQual);
        rakarrack.set(m_process->PrefNom("Tap Tempo Timeout"), m_process->t_timeout);
        rakarrack.set(m_process->PrefNom("Disable Warnings"), m_process->Disable_Warnings);
        rakarrack.set(m_process->PrefNom("Enable Tooltips"), m_process->ena_tool);
        rakarrack.set(m_process->PrefNom("Focus Delay"), m_process->Focus_Delay);

        for (int i = 0; i < 128; i++)
        {
            memset(temp1, 0, sizeof (temp1));
            sprintf(temp1, "Midi Table Program %d", i);
            rakarrack.set(m_process->PrefNom(temp1), m_process->M_table[i].bank * 1000 + m_process->M_table[i].preset);
        }
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
                rakarrack.set(m_process->PrefNom(temp1), Settings->JackCo->text(i));
                k++;
            }
        }

        rakarrack.set(m_process->PrefNom("Auto Connect Num"), k - 1);

        k = 1;
        for (int i = 1; i <= Settings->JackIn->size(); i++)
        {
            if (Settings->JackIn->selected(i))
            {
                memset(temp1, 0, sizeof (temp1));
                sprintf(temp1, "Jack Port In %d", k);
                rakarrack.set(m_process->PrefNom(temp1), Settings->JackIn->text(i));
                k++;
            }
        }

        rakarrack.set(m_process->PrefNom("Auto Connect In Num"), k - 1);
    }
}

void RKRGUI::Put_Loaded()
{
    //General loading of efx default settings

    WPreset_Name->value(m_process->Preset_Name);
    DAuthor->copy_label(m_process->Author);

    Nivel_Entrada->value((int) (m_process->Input_Gain * 100.0) - 50);
    m_process->calculavol(1);
    Nivel_Salida->value((int) (m_process->Master_Volume * 100.0) - 50);
    m_process->calculavol(2);
    Balance->value((int) (m_process->Fraction_Bypass * 100.0));


    ActivarGeneral->value(m_process->Bypass);

    if (m_process->Bypass == 0)
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
            CABINET->Cabinet_output->value(m_process->lv[EFX_CABINET][Cabinet_Gain] - 64);
            CABINET->Cabinet_preset->value(m_process->lv[EFX_CABINET][Cabinet_Preset_Idx]);
            CABINET->Cabinet_activar->value(m_process->EFX_Bypass[EFX_CABINET]);
        }
        else
        {
            // Calls the preset callback and sets the effect to the first preset
            Efx_Gui_Base[i]->activate_effect->value (m_process->EFX_Bypass[i]);
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
            m_process->modified = 1;
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

    // The use wants to save the main window preset to the bank selected preset button
    if ((Fl::event_button() == FL_RIGHT_MOUSE) && (Fl::event() == FL_RELEASE))
    {
        Fl_Widget *w = BankWindow->ob->child(button_number - 1);

        // Check if user really wants to set the preset to the bank
        Fl_Widget *m = fl_message_icon();
        m->parent()->copy_label(m_process->jackcliname);
        
        // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
        Shut_Off_Below_Mouse = 1;
        int ok = fl_choice("Overwrite \"%s\"?", "No", "Yes", NULL, w->label());
        Shut_Off_Below_Mouse = 0;
        
        if (!ok)
        {
            o->value(0);
            o->redraw();
            return;
        }

        o->value(0);
        o->redraw();
        m_process->preset_to_bank(button_number);
        w->copy_label(m_process->Preset_Name);
        m_process->modified = 1;
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
                if (!m_process->deachide)
                {
                    Efx_Gui_Base[j]->show();
                }

                // Set the active status, for label highlighting
                // If the Hide/Show button is set for hide, then show only active
                if (m_process->EFX_Bypass[j])
                {
                    m_process->active[i] = 1;

                    // Show the active
                    if (m_process->deachide)
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
                if ((m_process->deachide) && (!m_process->active[i]))
                    L1->hide();
                else
                    L1->show();
                break;
            case 1:
                L2->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L2->hide();
                else
                    L2->show();
                break;
            case 2:
                L3->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L3->hide();
                else
                    L3->show();
                break;
            case 3:
                L4->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L4->hide();
                else
                    L4->show();
                break;
            case 4:
                L5->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L5->hide();
                else
                    L5->show();
                break;
            case 5:
                L6->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L6->hide();
                else
                    L6->show();
                break;
            case 6:
                L7->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L7->hide();
                else
                    L7->show();
                break;
            case 7:
                L8->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L8->hide();
                else
                    L8->show();
                break;
            case 8:
                L9->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
                    L9->hide();
                else
                    L9->show();
                break;
            case 9:
                L10->copy_label(m_process->efx_names[Busca_Eff(m_process->efx_order[i])].Nom);
                if ((m_process->deachide) && (!m_process->active[i]))
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

    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    if (m_process->help_displayed == 0)
    {
        visor = new Fl_Help_Dialog;

        int x, y, w, h, k;
        rakarrack.get(m_process->PrefNom("Help X"), x, 1);
        rakarrack.get(m_process->PrefNom("Help Y"), y, 1);
        rakarrack.get(m_process->PrefNom("Help W"), w, 640);
        rakarrack.get(m_process->PrefNom("Help H"), h, 480);
        rakarrack.get(m_process->PrefNom("Help TextSize"), k, 11);
        visor->resize(x, y, w, h);
        visor->textsize((unsigned char) k);
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

    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    if (m_process->help_displayed == 0)
    {
        visor = new Fl_Help_Dialog;

        int x, y, w, h, k;
        rakarrack.get(m_process->PrefNom("Help X"), x, 1);
        rakarrack.get(m_process->PrefNom("Help Y"), y, 1);
        rakarrack.get(m_process->PrefNom("Help W"), w, 640);
        rakarrack.get(m_process->PrefNom("Help H"), h, 480);
        rakarrack.get(m_process->PrefNom("Help TextSize"), k, 11);
        visor->resize(x, y, w, h);
        visor->textsize((unsigned char) k);
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
    char temp[128];
    char temp1[128];
    char *masque;
    char *name;
    FILE *fp;

    Settings->BMidiIn->clear();


    if ((fp = fopen("/proc/asound/seq/clients", "r")) != NULL)
    {
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "Port") != NULL)
            {
                strcpy(temp1, temp);
                strtok(temp1, "\"");
                name = strtok(NULL, "\"");
                masque = strtok(NULL, ")");

                if ((masque[2] == 'R') && (strstr(name, "rakarrack-plus MC OUT") == 0))
                {
                    Settings->BMidiIn->add(name);
                }
            }
        }
    }

    fclose(fp);

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
            if (strcmp(Settings->BMidiIn->text(i), m_process->MID) == 0)
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
            for (int k = 0; k < m_process->cuan_jack; k++)
            {
                if (strcmp(Settings->JackCo->text(i), m_process->jack_po[k].name) == 0)
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
            for (int k = 0; k < m_process->cuan_ijack; k++)
            {
                if (strcmp(Settings->JackIn->text(i), m_process->jack_poi[k].name) == 0)
                {
                    Settings->JackIn->select(i, 1);
                }
            }

            i++;
        }
    }

    if (m_process->MIDIway)
    {
        Settings->Mw1->setonly();
    }
    else
    {
        Settings->Mw0->setonly();
    }

    m_process->m_displayed = 0;
    Settings->Enable_Back->value(m_process->EnableBackgroundImage);
    Settings->Enable_DeacHide->value(m_process->deachide);
    Settings->Enable_Scale->value(m_process->scalable);

    Settings->BFiname->value(m_process->BankFilename);
    Settings->BackFiname->value(m_process->BackgroundImage);
    Settings->Udir->value(m_process->UDirFilename);
    Settings->Username->value(m_process->UserRealName);
    Settings->Pre_Serve->value(m_process->actuvol);
    Settings->LM_Volume->value(m_process->Metro_Vol);
    Settings->Filter_DC->value(m_process->DC_Offset);
    Settings->FLPosition->value(m_process->flpos);
    Settings->DB6B->value(m_process->db6booster);
    Settings->Har_Downsample->value(m_process->Har_Down);
    Settings->Har_Down_Qua->value(m_process->Har_D_Q);
    Settings->Har_Up_Qua->value(m_process->Har_U_Q);
    Settings->Rev_Downsample->value(m_process->Rev_Down);
    Settings->Rev_Down_Qua->value(m_process->Rev_D_Q);
    Settings->Rev_Up_Qua->value(m_process->Rev_U_Q);
    Settings->Con_Downsample->value(m_process->Con_Down);
    Settings->Con_Down_Qua->value(m_process->Con_D_Q);
    Settings->Con_Up_Qua->value(m_process->Con_U_Q);
    Settings->Seq_Downsample->value(m_process->Seq_Down);
    Settings->Seq_Down_Qua->value(m_process->Seq_D_Q);
    Settings->Seq_Up_Qua->value(m_process->Seq_U_Q);
    Settings->Shi_Downsample->value(m_process->Shi_Down);
    Settings->Shi_Down_Qua->value(m_process->Shi_D_Q);
    Settings->Shi_Up_Qua->value(m_process->Shi_U_Q);
    Settings->Voc_Downsample->value(m_process->Voc_Down);
    Settings->Voc_Down_Qua->value(m_process->Voc_D_Q);
    Settings->Voc_Up_Qua->value(m_process->Voc_U_Q);
    Settings->Ste_Downsample->value(m_process->Ste_Down);
    Settings->Ste_Down_Qua->value(m_process->Ste_D_Q);
    Settings->Ste_Up_Qua->value(m_process->Ste_U_Q);
    
    Settings->Dist_Amo->value(m_process->Dist_res_amount);
    Settings->Dist_Up_Qua->value(m_process->Dist_up_q);
    Settings->Dist_Down_Qua->value(m_process->Dist_down_q);
    
    Settings->Ovrd_Amo->value(m_process->Ovrd_res_amount);
    Settings->Ovrd_Up_Qua->value(m_process->Ovrd_up_q);
    Settings->Ovrd_Down_Qua->value(m_process->Ovrd_down_q);
    
    Settings->Dere_Amo->value(m_process->Dere_res_amount);
    Settings->Dere_Up_Qua->value(m_process->Dere_up_q);
    Settings->Dere_Down_Qua->value(m_process->Dere_down_q);
    
    Settings->DBand_Amo->value(m_process->DBand_res_amount);
    Settings->DBand_Up_Qua->value(m_process->DBand_up_q);
    Settings->DBand_Down_Qua->value(m_process->DBand_down_q);
    
    Settings->Stomp_Amo->value(m_process->Stomp_res_amount);
    Settings->Stomp_Up_Qua->value(m_process->Stomp_up_q);
    Settings->Stomp_Down_Qua->value(m_process->Stomp_down_q);

    Settings->Calibration->value(m_process->aFreq);
    Settings->RTrigger->value(m_process->HarmRecNote->trigfact);
    Settings->RC_Harm_Opti->value(m_process->RCOpti_Harm);
    Settings->RC_St_Harm_Opti->value(m_process->RCOpti_Stereo);
    Settings->RC_Ring_Opti->value(m_process->RCOpti_Ring);


    switch (m_process->VocBands)
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


    Settings->MTable->value(m_process->midi_table);
    Settings->MTable->do_callback ();

    Settings->AAssign->value(m_process->autoassign);
    Settings->Update_TAP->value(m_process->Tap_Updated);
    Settings->INSTATE->value(m_process->init_state);
    Settings->UPSAMPLE_C->value(m_process->upsample);
    Settings->Upr_Qual->value(m_process->UpQual);
    Settings->Downr_Qual->value(m_process->DownQual);
    Settings->MESSAGE_DISABLE->value(m_process->Disable_Warnings);
    Settings->ENA_TOOL->value(m_process->ena_tool);
    Settings->Focus_Slider->value(m_process->Focus_Delay);
    Settings->T_TIMEOUT->value(m_process->t_timeout);

    Settings->Upr_Amo->value(m_process->UpAmo);
    Settings->L_SIZE->value(m_process->looper_size);
    Settings->D_A_Connect->value(m_process->aconnect_MI);
    Settings->D_J_Connect->value(m_process->aconnect_JA);
    Settings->D_IJ_Connect->value(m_process->aconnect_JIA);

    Settings->Midi_In_Counter->value(m_process->MidiCh + 1);
    Settings->Har_In_Counter->value(m_process->HarCh + 1);
    Settings->Stereo_Har_In_Counter->value(m_process->StereoHarCh + 1);

    switch (m_process->HarQual)
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

    switch (m_process->SteQual)
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
    
    switch (m_process->SeqQual)
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

    switch (m_process->ShiQual)
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


    if (m_process->aconnect_MI)
    {
        Settings->BMidiIn->activate();
    }
    else
    {
        Settings->BMidiIn->deactivate();
    }

    if (m_process->aconnect_JA)
    {
        Settings->JackCo->activate();
    }
    else
    {
        Settings->JackCo->deactivate();
    }

    if (m_process->aconnect_JIA)
    {
        Settings->JackIn->activate();
    }
    else
    {
        Settings->JackIn->deactivate();
    }

    Fl_Menu_Item *p;
    Fl_Menu_Item *Har = Settings->get_menu_Har_Downsample();
    unsigned int SR_value = m_process->sample_rate;

    for (int j = 0; j < Har->size(); j++)
    {
        p = Har->next(j);

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
            p->deactivate();
        }
        else
        {
            p->activate();
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
            char *p = buffer;
            if (t & FL_BOLD)
            {
                *p++ = '@';
                *p++ = 'b';
            }
            if (t & FL_ITALIC)
            {
                *p++ = '@';
                *p++ = 'i';
            }
            strcpy(p, name);
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

void RKRGUI::BankWin_Label(const char *filename)
{
    // add the bankwindow label
    char tmp[256];

    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Bank Manager - %s", m_process->jackcliname, VERSION, fl_filename_name(filename));
    BankWindow->copy_label(tmp);
}

void RKRGUI::is_modified()
{
    // popup dialog for modified bank saving

    if (m_process->modified)
    {
        Fl_Widget *w = fl_message_icon();
        w->parent()->copy_label(m_process->jackcliname);

        // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
        Shut_Off_Below_Mouse = 1;
        int ok = fl_choice("Bank was modified, but not saved", "Discard", "Save", NULL);
        Shut_Off_Below_Mouse = 0;
        
        switch (ok)
        {
            case 0:
                m_process->modified = 0;
                break;

            case 1:
                set_save_file();
                break;
        }
    }
}

void RKRGUI::Put_Loaded_Bank()
{
    int k = 1;

    // Program change number before the preset name
    std::string add_pg_change = "";

    for (int t = 0; t < BankWindow->ob->children(); t++)
    {
        Fl_Widget *w = BankWindow->ob->child(t);
        long long temp = (long long) w->user_data();
        if (temp > 0)
        {
            add_pg_change = "(";
            add_pg_change += NTS(k);
            add_pg_change += ") ";
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
                Nivel_Salida->value((int) (m_process->Master_Volume * 100.0) - 50);
                Nivel_Salida->redraw();
                break;

            case MC_Balance_FX:
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Balance->value(m_process->Fraction_Bypass * 100.0);
                Balance->redraw();
                break;

            case MC_Input_Volume:
                m_process->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Nivel_Entrada->value((int) (m_process->Input_Gain * 100.0) - 50);
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
    int miralo = 0;

    while (m_process->OnOffC > 0)
    {
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
            Efx_Gui_Base[miralo]->activate_effect->value (m_process->EFX_Bypass[miralo]);
            Efx_Gui_Base[miralo]->activate_effect->do_callback ();
            m_process->OnOffC--;
            continue;
        }
        else
        {
            switch (miralo)
            {
                case EFX_TAP_TEMPO_ON_OFF:
                    Tap_activar->value(m_process->Tap_Bypass);
                    Tap_activar->do_callback();
                    break;
                case EFX_MIDI_CONVERTER_ON_OFF:
                    MIDI->midi_activar->value(m_process->MIDIConverter_Bypass);
                    MIDI->midi_activar->do_callback();
                    break;
                case EFX_TUNER_ON_OFF:
                    tuner_activar->value(m_process->Tuner_Bypass);
                    tuner_activar->do_callback();
                    break;
                case EFX_MASTER_ON_OFF:
                    ActivarGeneral->value(m_process->Bypass);
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
    back = new Fl_Tiled_Image(new Fl_PNG_Image(m_process->BackgroundImage), 3200, 2400);
    
    if (!m_process->EnableBackgroundImage)
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



    if (m_process->MIDIConverter_Bypass)
        MIDI->MIDI_LABEL->labelcolor(on);
    else
        MIDI->MIDI_LABEL->labelcolor(off);
    if (m_process->Metro_Bypass)
        Metro_Label->labelcolor(on);
    else
        Metro_Label->labelcolor(off);
    if (m_process->Tap_Bypass)
        TAP_LABEL->labelcolor(on);
    else
        TAP_LABEL->labelcolor(off);
    if (m_process->Tuner_Bypass)
        TUNER_LABEL->labelcolor(on);
    else
        TUNER_LABEL->labelcolor(off);
    if (m_process->Bypass)
        LABEL_IO->labelcolor(on);
    else
        LABEL_IO->labelcolor(off);

    if ((m_process->upsample) && (m_process->Bypass))
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
            if (m_process->XUserMIDI[i][j] == the_one)
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
    if (m_process->Disable_Warnings)
        return;

    Fl_Widget *w = fl_message_icon();
    w->parent()->copy_label(m_process->jackcliname);
    
    // Need to shut off below mouse or it tries to modify the fl_message widget and crash.
    Shut_Off_Below_Mouse = 1;
    fl_message("This setting will be changed the next time you run rakarrack-plus");
    Shut_Off_Below_Mouse = 0;
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
        if (m_process->EFX_Bypass[efx_index])
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
        if (m_process->MIDIway)
        {
            for (int i = 0; i < 20; i++)
            {
                if (m_process->XUserMIDI[m_process->Aux_MIDI][i])
                {
                    m_process->process_midi_controller_events(m_process->XUserMIDI[m_process->Aux_MIDI][i], value);
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

int RKRGUI::Busca_Eff(int num)
{
    // Busca_Eff
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
    focus_delay_time = m_process->Focus_Delay = time;
}

void RKRGUI::below_mouse_highlight_and_focus()
{
    if(Shut_Off_Below_Mouse)
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
    // This will free all memory allocated for names
    // FIXME the list created is not used - remove when done
    ClearBankNames();
    
    // Clear the bank window menus
    BankWindow->CH_UB->clear();
    BankWindow->clear_menu();
    
    if(reload)
    {
        m_process->load_bank_CC_array ();
    }
    
    for(unsigned i = 0; i < m_process->Bank_Vector.size (); i++)
    {
        std::string full_path = m_process->Bank_Vector[i].Bank_File_Name;
        std::string menu_name = m_process->Bank_Vector[i].Bank_Menu_Name;
        BankWindow->set_bank_CH_UB(&menu_name[0], (char*) full_path.c_str());
    }

    BankWindow->CH_UB->value(0);
}

/**
 *  This is no longer used - FIXME remove when done
 *  Scan a directory for Bank files and if valid, send it to the BankWindow class.
 * 
 * @param directory
 *      The directory to be scanned.
 */
void RKRGUI::Set_Bank(std::string directory)
{
    std::vector<std::string>file_name;
    DIR *dir;
    struct dirent *fs;
    
    dir = opendir(directory.c_str());
    if (dir == NULL)
    {
        return;
    }

    // Get the bank files in the directory
    while ((fs = readdir(dir)))
    {
        if (strstr(fs->d_name, ".rkrb") != NULL)
        {
            file_name.push_back (fs->d_name);
        }
    }

    closedir(dir);
    
    // Sort alpha numeric
    std::sort( file_name.begin(), file_name.end() );
    
    for(unsigned i = 0; i < file_name.size (); i++)
    {
        std::string full_path = directory;
        full_path += "/";
        full_path += file_name[i];
        
        if (m_process->CheckOldBank(full_path.c_str ()) == 0)
        {
            AddBankName(full_path.c_str ());
            
            // Construct the name to be listed in the drop down "User Banks" menu
            // Add the CC value for bank select
            std::string menu_name = "(";
            menu_name += NTS((i + 3));      // CC value starts at 4, first 4 are preset (0 index)
            menu_name += ") ";
            
            // Add the file name
            menu_name += file_name[i];
            menu_name = menu_name.substr(0, menu_name.size() - c_rkrb_ext_size);   // remove extension

            // Add the name and full path to the menu
            if(!menu_name.empty())
            {
                BankWindow->set_bank_CH_UB(&menu_name[0], (char*) full_path.c_str());
            } 
        }
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

bool RKRGUI::install_signal_handlers()
{
    /*install signal handlers*/
    struct sigaction action;
    memset(&action, 0, sizeof (action));
    action.sa_handler = sigterm_handler;

    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        printf("sigaction() failed: \n");
        return false;
    }

    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        printf("sigaction() failed: \n");
        return false;
    }

    return true;
}

void RKRGUI::sigterm_handler(int sig)
{
    // handle signal type
    if (sig == SIGUSR1)
    {
        got_sigusr1 = sig;
    }

    if (sig == SIGINT)
    {
        got_sigint = sig;
    }
}

void RKRGUI::check_signals(void *usrPtr)
{
    // process signals
    RKRGUI *gui = NULL;
    gui = (RKRGUI*) usrPtr;

    if (!gui)
        return;

    if (got_sigusr1 == SIGUSR1)
    {
        if (!gui->m_process->File_To_Load.empty()) // individual preset
        {
            printf("Saving file: %s\n", gui->m_process->File_To_Load.c_str());
            got_sigusr1 = 0;
            gui->m_process->save_preset(gui->m_process->File_To_Load.c_str());
        }
        return;
    }

    if (got_sigint == SIGINT)
    {
        printf("Got SIGTERM, quitting...\n");
        got_sigint = 0;
        gui->m_process->Exit_Program = 1;
    }
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
 *  if (!m_process->MIDIway)
 *      return;
 * 
 * @param num
 *  The efx parameter number that is to be used by midi learn.
 */
void RKRGUI::getMIDIControl(int num)
{
    // Don't pop up the midi learn window unless the user checked the box.
    if (!m_process->MIDIway)
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
        if (m_process->autoassign == 0)
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
    Shut_Off_Below_Mouse = 1;
    const char *name = fl_input("Preset Name?", "");
    Shut_Off_Below_Mouse = 0;
    
    if (name == NULL)
        return;

    char NewName[64];
    memset(NewName, 0, sizeof (NewName));
    sprintf(NewName, "*%s", name);
    add_insert_preset_name(w, NewName);
    m_process->save_insert_preset(effect, NewName);
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
void RKRGUI::add_insert_preset_name(Fl_Widget *w, char *name)
{
    RKR_Choice *s = static_cast<RKR_Choice *> (w);
    
    s->add(name);

    Fl_Menu_Item *m = const_cast<Fl_Menu_Item*>  (s->menu ());
    Fl_Menu_Item *p;
    
    int font_size = C_DEFAULT_FONT_SIZE;

    for (int i = 0; i < m->size(); i++)
    {
        p = m->next(i);
        
        if (i == 0)
        {
            font_size = p->labelsize();
        }
        
        p->labelsize(font_size);
        p->labelfont (global_font_type);
    }
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
 * Reads the insert presets file : .rkrintpreset in the user home directory
 * and adds them to the effect preset list.
 */
void RKRGUI::read_insert_presets()
{
    // Read in user presets
    FILE *fn;
    char tempfile[256];
    char buf[256];
    char *name;
    char *sbuf;
    int effect = 0;
    memset(tempfile, 0, sizeof (tempfile));

    sprintf(tempfile, "%s%s", getenv("HOME"), "/.rkrintpreset");

    if ((fn = fopen(tempfile, "r")) != NULL)
    {
        while (fgets(buf, sizeof buf, fn) != NULL)
        {
            sbuf = buf;
            sscanf(buf, "%d", &effect);
            name = strsep(&sbuf, ",");
            name = strsep(&sbuf, ",");
            add_insert_preset_name(find_effect_preset_widget(effect), name);
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

    char Rname[128];
    RKR_Choice *preset = static_cast<RKR_Choice*> (w);
    
    if (strncmp(preset->text(), "*", 1) != 0)
    {
        // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
        Shut_Off_Below_Mouse = 1;
        fl_message("Internal Presets can not be deleted ");
        Shut_Off_Below_Mouse = 0;
        return;
    }
    
    // Need to shut off below mouse or it tries to modify the fl_choice widget and crash.
    Shut_Off_Below_Mouse = 1;
    int ok = fl_choice("Delete \"%s\"?", "No", "Yes", NULL, preset->text());
    Shut_Off_Below_Mouse = 0;

    if (!ok)
    {
        return;
    }

    memset(Rname, 0, sizeof (Rname));
    sprintf(Rname, "%s", preset->text());
    preset->remove(preset->value());
    preset->value(0);
    preset->redraw();

    m_process->delete_insert_preset(effect, Rname);
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

    m_process->M_table[num - UD_Bank_Used_Start].bank = (int) o->value();
    Settings->fill_mptable(num + UD_Bank_Used_Start, m_process->M_table[num - UD_Bank_Used_Start].bank);
}

void RKRGUI::p_click(Fl_Choice* o, void* v)
{
    ((RKRGUI*) (o->parent()->parent()->parent()->parent()->user_data()))->p_click_i(o, v);
}

inline void RKRGUI::p_click_i(Fl_Choice* o, void*)
{
    long long kk = (long long) o->user_data();
    int num = (int) kk;

    m_process->M_table[num - UD_Preset_Used_Start].preset = o->value();
}

void RKRGUI::RandomPreset()
{
    // Reset all effects to defaults
    S_new->do_callback();

    // Array to hold selected effect index
    int Effect_Index[10];
    
    // Get the first one to compare for duplicates
    Effect_Index[0] = (int) (RND * C_NUMBER_EFFECTS);

    // Get random selection of effect index
    for (int i = 1; i < 10; i++)
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
                    l = 1;  // got a new selection, break from while()
            }
        }
    }

    // Set the main window order based on random selections
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        m_process->efx_order[i] = Effect_Index[i];
    }
    
    // Get random number of active effects, max of six
    int number_active_effects = (int) (RND * 6) + 1;

    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        int rack_effect = m_process->efx_order[i];

        // Set the main window effects active or inactive
        if (i < number_active_effects)
        {
            m_process->EFX_Bypass[rack_effect] = 1;
        }
        else
        {
            m_process->EFX_Bypass[rack_effect] = 0;
        }
        
        Efx_Gui_Base[rack_effect]->activate_effect->value (m_process->EFX_Bypass[rack_effect]);

        // Get the effect preset size and select a random effect preset
        Fl_Widget *w = find_effect_preset_widget(Effect_Index[i]);
        RKR_Choice *preset_widget = static_cast<RKR_Choice *> (w);

        long long widget_user_data = (long long) preset_widget->user_data();
        int preset_selection = (int) (RND * preset_widget->size());
        preset_widget->value(preset_selection);
        preset_widget->do_callback(w, widget_user_data);
    }

    FillML();
    Prepare_Order();
    Put_Loaded();

    ActivarGeneral->value(1);
    ActivarGeneral->do_callback();
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

/**
 * Adds a bank file name with full path to a double stacked list.
 * Currently the list mBankNameList is never used - FIXME
 * 
 * @param name
 *      The bank file with full path.
 * 
 * @return 
 *      1 if successful, 0 if not.
 */
int RKRGUI::AddBankName(const char *name)
{
    //AddBankName
    struct list_element *elem;

    elem = (struct list_element *) calloc(1, sizeof (struct list_element));

    if (elem == NULL)
    {
        return 0;
    }

    elem->name = (char *) calloc(1, 256);

    if (elem->name == NULL)
    {
        free(elem);
        return 0;
    }

    strncpy(elem->name, name, 255);

    if (mBankNameList == NULL)
    {
        mBankNameList = elem;
        insque(elem, NULL);
    }
    else
    {
        insque(elem, mBankNameListTail);
    }

    mBankNameListTail = elem;
    return 1;
}

/**
 * Clears the double stacked list of bank names.
 */
void RKRGUI::ClearBankNames()
{
    // clearbanknames
    struct list_element *elem;
    while (mBankNameList != NULL)
    {
        elem = mBankNameList;
        mBankNameList = elem->forward;
        free(elem->name);
        remque(elem);
        free(elem);
    }
}

char* RKRGUI::get_bank_file()
{
    // get bank file
    char *filename;
    is_modified();
    filename = fl_file_chooser("Load Bank File:", "(*.rkrb)", NULL, 0);

    if (filename == NULL)
        return 0;

    filename = fl_filename_setext(filename, ".rkrb");
    int ok = m_process->load_bank(filename);

    if (ok)
    {
        BankWin_Label(filename);
        Put_Loaded_Bank();
        AddBankName(filename);
    }

    return filename;
}

void RKRGUI::set_save_file()
{
    // set bank file save name
    char *filename;

#define EXT ".rkrb"
    filename = fl_file_chooser("Save Bank File:", "(*" EXT")", m_process->Bank_Saved, 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, EXT);
#undef EXT
    int ok = m_process->save_bank(filename);
    
    if (ok)
    {
        // Check if the saved file is one of the Bank_Vector items
        // If it is, then we need to update the Bank_Vector
        int file_match = -1;
        for(unsigned i = 0; i < m_process->Bank_Vector.size (); i++)
        {
            if(strcmp(filename, m_process->Bank_Vector[i].Bank_File_Name.c_str()) == 0)
            {
                file_match = i;
                break;
            }
        }
        
        if(file_match >= 0)
        {
            m_process->copy_bank (m_process->Bank_Vector[file_match].Bank, m_process->Bank);
        }
        
        strcpy(m_process->Bank_Saved, filename);
        BankWin_Label(filename);
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
    // load table
    char *filename;
    filename = fl_file_chooser("Load MIDI Table:", "(*.rmt)", NULL, 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, ".rmt");
    m_process->load_MIDI_table(filename);
    Settings->Put_MidiTable();
}

void RKRGUI::Save_Midi_Program_Change_Table()
{
    // save program change table
    char *filename;
#define EXT ".rmt"
    filename = fl_file_chooser("Save MIDI Table:", "(*" EXT")", "", 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, EXT);
#undef EXT
    m_process->save_MIDI_table(filename);
}
