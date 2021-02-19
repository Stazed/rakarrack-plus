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
static Fl_Widget *old;
static int last_tecla;
static int drag;
static int at;
static Pixmap p, mask;
static XWMHints *hints = NULL;
static volatile int got_sigint = 0;
static volatile int got_sigusr1 = 0;

RKRGUI::RKRGUI(int argc, char**argv, RKR *rkr_) :
    made()
{
    // Initialize Gui
    Fl::args(argc, argv);
    fl_open_display();
    XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), (char **) icono_rakarrack_128x128, &p, &mask, NULL);

    Fl::visual(FL_DOUBLE | FL_RGB);
    fl_register_images();
    Fl::set_fonts(0);
    rkr = rkr_;

    mBankNameList = NULL;
    mBankNameListTail = NULL;

    back = NULL;
    old = NULL;
    make_window();

    Principal->icon((char *) p);
    BankWindow->icon((char *) p);
    Order->icon((char *) p);
    Settings->icon((char *) p);
    AboutWin->icon((char *) p);
    MIDILearn->icon((char *) p);
    Trigger->icon((char *) p);

    last_tecla = 0;
    made = 0;
    char tmp[256];
    drag = 1000;
    at = 0;
    Analy->set_analyzer_ON(false);
    Sco->set_scope_ON(false);

    Sco->init(rkr->anall, rkr->analr, rkr->period);
    Analy->init(rkr->anall, rkr->analr, rkr->period, rkr->sample_rate);

    memset(tmp, 0, sizeof (tmp));
    if (!rkr->File_To_Load.empty())
    {
        sprintf(tmp, "Session: %s", rkr->File_To_Load.c_str ());
    }
    else
    {
        sprintf(tmp, "%s   v%s", rkr->jackcliname, VERSION);
    }

    Principal->copy_label(tmp);
    BankWin_Label(rkr->BankFilename);
    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Effects Order", rkr->jackcliname, VERSION);
    Order->copy_label(tmp);
    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Settings", rkr->jackcliname, VERSION);
    Settings->copy_label(tmp);
    sprintf(tmp, "%s   v%s - MIDI Learn", rkr->jackcliname, VERSION);
    MIDILearn->copy_label(tmp);
    sprintf(tmp, "%s   v%s - ACI", rkr->jackcliname, VERSION);
    Trigger->copy_label(tmp);
    
    load_previous_state();
    Settings->make_table_window();
    Put_Loaded();
    Principal->show(argc, argv);
    put_icon(Principal);
    ReadIntPresets();
    
    if (rkr->deachide)
    {
        HideUE->label("Show");
    }
    else
    {
        HideUE->label("Hide");
    }
    
    HideUE->redraw();

    Fl::add_timeout(.04, this->TimeoutStatic, this);
    Fl::add_handler(prevnext);
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
    highlight();
    drag_effect();
    check_signals(this);

    if (rkr->Tuner_Bypass)
    {

        if (rkr->efx_Tuner->note_actual != rkr->note_old)
        {
            char notas[3];
            sprintf(notas, "%-2s", rkr->efx_Tuner->notes[rkr->efx_Tuner->note_actual]);
            WNote->copy_label(notas);
            rkr->note_old = rkr->efx_Tuner->note_actual;
        }

        if (rkr->efx_Tuner->nfreq != rkr->nfreq_old)
        {
            char refreq[60];
            sprintf(refreq, "%8.3f", rkr->efx_Tuner->nfreq);
            WRfreq->copy_label(refreq);
            rkr->nfreq_old = rkr->efx_Tuner->nfreq;
        }

        //if(rkr->efx_Tuner->afreq != rkr->afreq_old)
        if (fabsf(rkr->efx_Tuner->afreq - rkr->afreq_old) > .2f)
        {
            char nofreq[60];
            sprintf(nofreq, "%8.3f", rkr->efx_Tuner->afreq);
            if (abs(rkr->efx_Tuner->cents) > 5)
                WNfreq->labelcolor(FL_RED);
            else
                WNfreq->labelcolor(label_color);
            WNfreq->copy_label(nofreq);
            rkr->afreq_old = rkr->efx_Tuner->afreq;
        }

        if (rkr->efx_Tuner->afreq == 0.0f)
        {
            char notas[3];
            sprintf(notas, "  ");
            WNote->copy_label(notas);
        }


        if (rkr->efx_Tuner->cents != rkr->cents_old)
        {
            tuner_bar->value(rkr->efx_Tuner->cents);
            rkr->cents_old = rkr->efx_Tuner->cents;
        }
    }

    if (rkr->Gui_Refresh == GUI_Refresh_Plus_Key)
    {
        if (rkr->Selected_Preset < 60)
        {
            Preset_Counter->value(rkr->Selected_Preset + 1);
            Preset_Counter->do_callback();
        }

        rkr->Gui_Refresh = GUI_Refresh_Off;
    }


    if (rkr->Gui_Refresh == GUI_Refresh_Minus_Key)
    {
        if (rkr->Selected_Preset > 1)
        {
            Preset_Counter->value(rkr->Selected_Preset - 1);
            Preset_Counter->do_callback();
        }

        rkr->Gui_Refresh = GUI_Refresh_Off;
    }

    if (rkr->Gui_Refresh == GUI_Refresh_F2_Key)
    {
        if (Nivel_Salida->value()>-50)
        {
            Nivel_Salida->value(Nivel_Salida->value() - 1);
            Nivel_Salida->do_callback();
            Nivel_Salida->redraw();
        }

        rkr->Gui_Refresh = GUI_Refresh_Off;
    }


    if (rkr->Gui_Refresh == GUI_Refresh_F3_Key)
    {
        if (Nivel_Salida->value() < 50)
        {
            Nivel_Salida->value(Nivel_Salida->value() + 1);
            Nivel_Salida->do_callback();
            Nivel_Salida->redraw();
        }

        rkr->Gui_Refresh = GUI_Refresh_Off;
    }


    if (rkr->Gui_Refresh == GUI_Refresh_Looper)
    {
        update_looper();
        rkr->Gui_Refresh = GUI_Refresh_Off;
    }


    if (rkr->Change_Preset != C_CHANGE_PRESET_OFF)
    {
        if (!rkr->midi_table)
        {
            Preset_Counter->value(rkr->Change_Preset);
            Preset_Counter->do_callback();
        }
        else
        {
            if (rkr->a_bank != rkr->M_table[rkr->Change_Preset].bank)
            {
                switch (rkr->M_table[rkr->Change_Preset].bank)
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
            Preset_Counter->value(rkr->M_table[rkr->Change_Preset].preset + 1);
            Preset_Counter->do_callback();
        }

        rkr->Change_Preset = C_CHANGE_PRESET_OFF;
    }


    MIDI_control_gui_refresh();
    if (rkr->ACI_Bypass)
        ActACI();

    rkr->cpufp++;
    if (rkr->cpufp == 40)
    {
        char tmp[8];
        memset(tmp, 0, sizeof (tmp));
        sprintf(tmp, "%5.2f%%", rkr->cpuload);
        CPULOAD->copy_label(tmp);
        rkr->cpufp = 0;
    }


    // Jack Port connections
    if (rkr->Jack_Port_Connnection_Changed)
    {
        if (rkr->Jack_IN_Port_Connnection_Status)
            PORT_IN_STATE->show();
        else
            PORT_IN_STATE->hide();
        if (rkr->Jack_OUT_Port_Connnection_Status)
            PORT_OUT_STATE->show();
        else
            PORT_OUT_STATE->hide();
        if (rkr->Jack_AUX_Port_Connnection_Status)
            PORT_AUX_STATE->show();
        else
            PORT_AUX_STATE->hide();
        if (rkr->Jack_MIDI_IN_Port_Connnection_Status)
            PORT_MIDI_IN_STATE->show();
        else
            PORT_MIDI_IN_STATE->hide();
        if (rkr->Jack_MIDI_OUT_Port_Connnection_Status)
            PORT_MIDI_OUT_STATE->show();
        else
            PORT_MIDI_OUT_STATE->hide();
        rkr->Jack_Port_Connnection_Changed = 0;
    }

    if (global_error_number > 0)
        rkr->Error_Handle(global_error_number);

    if (rkr->Tap_Bypass)
    {
        if ((rkr->Tap_Display == 1) && (rkr->Tap_Selection > 0))
        {
            T_DIS->value(rkr->Tap_TempoSet);
            UpdateTGUI();
            rkr->Tap_Display = 0;
        }

        if (rkr->Tap_Display == 2)
        {
            rkr->Tap_Display = 0;
            Tap_activar->value(0);
            Tap_activar->do_callback();
        }
    }


    if (rkr->Bypass)
    {
        if (rkr->val_il_sum != rkr->old_il_sum)
        {
            input_vul->value(rkr->val_il_sum);
            rkr->old_il_sum = rkr->val_il_sum;
        }

        if (rkr->val_ir_sum != rkr->old_ir_sum)
        {
            input_vur->value(rkr->val_ir_sum);
            rkr->old_ir_sum = rkr->val_ir_sum;
        }

        if (rkr->val_vl_sum != rkr->old_vl_sum)
        {
            output_vul->value(rkr->val_vl_sum);
            rkr->old_vl_sum = rkr->val_vl_sum;
        }

        if (rkr->val_vr_sum != rkr->old_vr_sum)
        {
            output_vur->value(rkr->val_vr_sum);
            rkr->old_vr_sum = rkr->val_vr_sum;
        }


        if (rkr->efx_FLimiter->clipping)
        {
            CLIP_LED->color(FL_RED);
            CLIP_LED->redraw();
        }
        else
        {
            CLIP_LED->color(fl_darker(FL_BLUE));
            CLIP_LED->redraw();
        }

        if (rkr->efx_FLimiter->limit)
        {
            LMT_LED->color(leds_color);
            LMT_LED->redraw();
        }
        else
        {
            LMT_LED->color(fl_darker(leds_color));
            LMT_LED->redraw();
        }

        rkr->efx_FLimiter->clipping = 0;
        rkr->efx_FLimiter->limit = 0;


        if (rkr->checkforaux())
        {
            Vocoder *Efx_Vocoder = static_cast <Vocoder*>(rkr->Rack_Effects[EFX_VOCODER]);
            
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
            at++;
            if (at == 3)
            {
                Analy->redraw();
                at = 0;
            }
        }
        else
        {
            at = 0;
        }

        if (rkr->EFX_Bypass[EFX_LOOPER])
        {
            Looper *Efx_Looper = static_cast <Looper*> (rkr->Rack_Effects[EFX_LOOPER]);
            if ((Efx_Looper->Pplay) && (!Efx_Looper->Pstop))
            {
                if (rkr->looper_lqua != Efx_Looper->looper_qua)
                {
                    rkr->looper_lqua = Efx_Looper->looper_qua;
                    char tmp[16];
                    memset(tmp, 0, sizeof (tmp));
                    sprintf(tmp, "%d/%d", Efx_Looper->looper_bar, Efx_Looper->looper_qua);
                    LOOPER->L_TimePos->copy_label(tmp);
                }
            }
        }
    }


    if ((rkr->Metro_Bypass) && (MetroSound->value() == 0))
    {
        if ((!rkr->M_Metronome->markctr) && ((int) Metro_Led->color() != (int) fl_lighter(FL_RED)))
        {
            Metro_Led->color(fl_lighter(FL_RED));
            Metro_Led->redraw();
        }

        if ((rkr->M_Metronome->markctr) && ((int) Metro_Led->color() != (int) fl_darker(FL_RED)))
        {
            Metro_Led->color(fl_darker(FL_RED));
            Metro_Led->redraw();
        }
    }


    if (rkr->MIDIConverter_Bypass)
    {
        if ((rkr->efx_MIDIConverter->hay) && ((int) MIDI->Mled->color() != (int) fl_lighter(FL_RED)))
        {
            MIDI->Mled->color(fl_lighter(FL_RED));
            MIDI->Mled->redraw();
        }

        if ((!rkr->efx_MIDIConverter->hay) && ((int) MIDI->Mled->color() != (int) fl_darker(FL_RED)))
        {
            MIDI->Mled->color(fl_darker(FL_RED));
            MIDI->Mled->redraw();
        }
    }


    if (rkr->EFX_Bypass[EFX_HARMONIZER])
    {
        Harmonizer *Efx_Harmonizer = static_cast <Harmonizer*> (rkr->Rack_Effects[EFX_HARMONIZER]);
        if ((Efx_Harmonizer->PSELECT) || (Efx_Harmonizer->PMIDI))
        {
            if (rkr->RC_Harm->cc)
            {
                HAR->har_chordname->copy_label(rkr->RC_Harm->NombreAcorde);
                rkr->RC_Harm->cc = 0;
                HAR->har_chordname->redraw();
                rkr->RC_Harm->Vamos(0, Efx_Harmonizer->Pinterval - 12, rkr->HarmRecNote->reconota);
                Efx_Harmonizer->r_ratio = rkr->RC_Harm->r__ratio[0]; //pass the found ratio
            }
        }
    }


    if (rkr->EFX_Bypass[EFX_STEREOHARM])
    {
        StereoHarm *Efx_StereoHarm = static_cast<StereoHarm*>(rkr->Rack_Effects[EFX_STEREOHARM]);

        if ((Efx_StereoHarm->PSELECT) || (Efx_StereoHarm->PMIDI))
        {
            if (rkr->RC_Stereo_Harm->cc)
            {
                SHAR->shar_chordname->copy_label(rkr->RC_Stereo_Harm->NombreAcorde);
                rkr->RC_Stereo_Harm->cc = 0;
                SHAR->shar_chordname->redraw();
                rkr->RC_Stereo_Harm->Vamos(1, Efx_StereoHarm->Pintervall - 12, rkr->StHarmRecNote->reconota);
                rkr->RC_Stereo_Harm->Vamos(2, Efx_StereoHarm->Pintervalr - 12, rkr->StHarmRecNote->reconota);
                Efx_StereoHarm->r_ratiol = rkr->RC_Stereo_Harm->r__ratio[1];
                Efx_StereoHarm->r_ratior = rkr->RC_Stereo_Harm->r__ratio[2];
            }
        }
    }


    if (rkr->RControl)
    {
        rkr->CountWait++;
        if (rkr->CountWait == 25)
        {
            MIDILearn->GMM->color(leds_color);
            MIDILearn->GMM->redraw();
        }

        if (rkr->CountWait == 50)
        {
            MIDILearn->GMM->color(fore_color);
            MIDILearn->GMM->redraw();
            rkr->CountWait = 0;
        }

        if (rkr->ControlGet != 0)
        {
            rkr->RControl = 0;
            MIDILearn->Disp_Control->value(rkr->ControlGet);
            MIDILearn->Disp_Control->redraw();
            MIDILearn->GMM->color(fore_color);
            MIDILearn->GMM->redraw();
            if ((rkr->comemouse) && (rkr->autoassign))
            {
                rkr->comemouse = 0;
                MIDILearn->Assign->do_callback();
            }
        }
    };
    
    if(rkr->change_scale)
    {
        rkr->change_scale = 0;  // false
        if (rkr->scalable )
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

void RKRGUI::Background_Color_Change(Fl_Color bcolor)
{
    // change the gui background color if png is not used
    MenuP->color(bcolor);
    BankWindow->MenuB->color(bcolor);

    back_color = bcolor;

    Label_Color_Change(label_color);
    
    if (!rkr->EnableBackgroundImage)
    {
        back->color_average(back_color, 0.0);
    }
}

void RKRGUI::Label_Color_Change(Fl_Color bcolor)
{
    // change the label colors
    label_color = bcolor;
    chfsize(C_DONT_CHANGE_FONT_SIZE);


    for (int t = 0; t < Settings->STabs->children(); t++)
    {
        Fl_Widget *tabg = Settings->STabs->child(t);
        tabg->labelcolor(label_color);
        tabg->selection_color(back_color);
        tabg->color(fore_color);

        long long ud = (long long) tabg->user_data();

        if (ud == 1)
        {
            Fl_Group *g = (Fl_Group *) tabg;

            for (int i = 0; i < g->children(); i++)
            {
                Fl_Widget *w = g->child(i);

                w->labelcolor(label_color);
                w->selection_color(back_color);
                w->color(fore_color);
                
                long long ugd = (long long) w->user_data();
                
                if(ugd == GROUP_USER_DATA)
                {
                    Fl_Group *gg = (Fl_Group *) w;
                    for (int i = 0; i < gg->children(); i++)
                    {
                        Fl_Widget *ww = gg->child(i);

                        ww->labelcolor(label_color);
                        ww->selection_color(back_color);
                        ww->color(fore_color);
                    }
                }
                
            }
        }
    }

    for (int t = 0; t < BankWindow->children(); t++)
    {
        Fl_Widget *w = BankWindow->child(t);

        w->labelcolor(label_color);
    }


    for (int t = 0; t < BankWindow->ob->children(); t++)
    {
        Fl_Widget *w = BankWindow->ob->child(t);

        w->labelcolor(label_color);
    }


    for (int t = 0; t < Order->children(); t++)
    {
        Fl_Widget *w = Order->child(t);

        w->labelcolor(label_color);
        w->selection_color(back_color);
        w->color(fore_color);
    }

    for (int t = 0; t < Order->Filters->children(); t++)
    {
        Fl_Widget *w = Order->Filters->child(t);

        w->labelcolor(label_color);
        w->selection_color(back_color);
        w->color(fore_color);
    }


    for (int t = 0; t < MIDILearn->children(); t++)
    {
        Fl_Widget *w = MIDILearn->child(t);

        w->labelcolor(label_color);
        w->selection_color(back_color);
        w->color(fore_color);
    }

    Settings->Font_Bro->textcolor(label_color);
    Trigger->ACI_LABEL->labelcolor(fl_lighter(label_color));
    Trigger->aux_source->labelcolor(label_color);
    Trigger->aux_midi->labelcolor(label_color);

    Fl::redraw();
}

void RKRGUI::Buttons_Color_Change(Fl_Color bcolor)
{
    // change the button colors
    if (made)
    {
        for (int t = 0; t < BankWindow->ob->children(); t++)
        {
            Fl_Widget *w = BankWindow->ob->child(t);
            long long temp = (long long) w->user_data();
            
            if (temp > 0)
            {
                w->color(bcolor);
            }
        }

        BankWindow->B_B1->color(bcolor);
        BankWindow->B_B2->color(bcolor);
        BankWindow->B_B3->color(bcolor);
        BankWindow->B_B4->color(bcolor);
        BankWindow->CH_UB->color(bcolor);
    }


    fore_color = bcolor;
    Trigger->aux_source->color(bcolor);
    Label_Color_Change(label_color);
}

void RKRGUI::Leds_Color_Change(Fl_Color bcolor)
{
    // change the leds color
    leds_color = bcolor;
    chfsize(C_DONT_CHANGE_FONT_SIZE);
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
    int x, y, w, h, k, b, f, l, a;
    x = y = w = h = k = b = f = l = a = 0;

    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    rakarrack.get(rkr->PrefNom("Principal X"), x, 1);
    rakarrack.get(rkr->PrefNom("Principal Y"), y, 1);
    rakarrack.get(rkr->PrefNom("Principal W"), w, 960);
    rakarrack.get(rkr->PrefNom("Principal H"), h, 540);

    char tmp[256];
    sprintf(tmp, "%s/blackbg.png", DATADIR);

    rakarrack.get(rkr->PrefNom("Enable Background Image"), rkr->EnableBackgroundImage, 1);
    rakarrack.get(rkr->PrefNom("Background Image"), rkr->BackgroundImage, tmp, 256);
    PutBackground();
    rakarrack.get(rkr->PrefNom("FontSize"), rkr->fontsize, C_DEFAULT_FONT_SIZE);
    if (rkr->fontsize != C_DEFAULT_FONT_SIZE)
        chfsize(rkr->fontsize);
    rakarrack.get(rkr->PrefNom("Font"), rkr->font, 0);

    rakarrack.get(rkr->PrefNom("Background Color"), b, 56);
    rakarrack.get(rkr->PrefNom("Foreground Color"), f, 34499072);
    rakarrack.get(rkr->PrefNom("Leds Color"), l, -13485824);
    rakarrack.get(rkr->PrefNom("Labels Color"), a, -1075849984);
    rakarrack.get(rkr->PrefNom("Schema"), k, 4);
    Settings->scheme_ch->value(k);
    Settings->scheme_ch->do_callback();
    rakarrack.get(rkr->PrefNom("Hide Effects"), rkr->deachide, 0);
    rakarrack.get(rkr->PrefNom("Scale Window"), rkr->scalable, 0);

    Principal->resize(x, y, w, h);

    rakarrack.get(rkr->PrefNom("BankWindow X"), x, 1);
    rakarrack.get(rkr->PrefNom("BankWindow Y"), y, 1);
    rakarrack.get(rkr->PrefNom("BankWindow W"), w, 680);
    rakarrack.get(rkr->PrefNom("BankWindow H"), h, 425);

    BankWindow->resize(x, y, w, h);

    rakarrack.get(rkr->PrefNom("Order X"), x, 1);
    rakarrack.get(rkr->PrefNom("Order Y"), y, 1);
    rakarrack.get(rkr->PrefNom("Order W"), w, 600);
    rakarrack.get(rkr->PrefNom("Order H"), h, 330);

    Order->resize(x, y, w, h);

    rakarrack.get(rkr->PrefNom("MIDI Learn X"), x, 1);
    rakarrack.get(rkr->PrefNom("MIDI Learn Y"), y, 1);
    rakarrack.get(rkr->PrefNom("MIDI Learn W"), w, 600);
    rakarrack.get(rkr->PrefNom("MIDI Learn H"), h, 360);

    MIDILearn->resize(x, y, w, h);

    rakarrack.get(rkr->PrefNom("Trigger X"), x, 1);
    rakarrack.get(rkr->PrefNom("Trigger Y"), y, 1);
    rakarrack.get(rkr->PrefNom("Trigger W"), w, 280);
    rakarrack.get(rkr->PrefNom("Trigger H"), h, 165);

    Trigger->resize(x, y, w, h);

    rakarrack.get(rkr->PrefNom("Settings X"), x, 1);
    rakarrack.get(rkr->PrefNom("Settings Y"), y, 1);
    rakarrack.get(rkr->PrefNom("Settings W"), w, 440);
    rakarrack.get(rkr->PrefNom("Settings H"), h, 410);

    Settings->resize(x, y, w, h);


    back_color = (Fl_Color) b;
    fore_color = (Fl_Color) f;
    leds_color = (Fl_Color) l;
    label_color = (Fl_Color) a;

    Background_Color_Change(back_color);
    Buttons_Color_Change(fore_color);
    Leds_Color_Change(leds_color);
    Label_Color_Change(label_color);

    rakarrack.get(rkr->PrefNom("Bank Selected"), rkr->a_bank, 3);

    if (!rkr->Bank_Load_Command_Line)
    {
        switch (rkr->a_bank)
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
        BankWin_Label(rkr->Command_Line_Bank.c_str());
    }

    if (!rkr->Command_Line_File)
    {
        rakarrack.get(rkr->PrefNom("Preset Num"), k, 1);
        rkr->Selected_Preset = k;
        Preset_Counter->value(k);
        Preset_Counter->do_callback();
    }

    rakarrack.get(rkr->PrefNom("MIDI Implementation"), rkr->MIDIway, 0);
    if (!rkr->MIDIway)
        ML_Menu->deactivate();

    rakarrack.get(rkr->PrefNom("MIDI Table"), rkr->midi_table, 0);
    if (!rkr->midi_table)
        Settings->scroll->deactivate();


    rakarrack.get(rkr->PrefNom("UserName"), rkr->UserRealName, "", 127);
    rakarrack.get(rkr->PrefNom("User Directory"), rkr->UDirFilename, DATADIR, 127);
    rakarrack.get(rkr->PrefNom("Preserve Gain/Master"), rkr->actuvol, 0);
    rakarrack.get(rkr->PrefNom("Metronome Volume"), rkr->Metro_Vol, 50);
    
    Looper *Efx_Looper = static_cast <Looper*> (rkr->Rack_Effects[EFX_LOOPER]);
    Efx_Looper->setmvol(rkr->Metro_Vol);

    rakarrack.get(rkr->PrefNom("Update Tap"), rkr->Tap_Updated, 0);
    rakarrack.get(rkr->PrefNom("MIDI IN Channel"), rkr->MidiCh, 1);
    rkr->MidiCh--;

    rakarrack.get(rkr->PrefNom("MIDI IN Harmonizer"), rkr->HarCh, 1);
    rkr->HarCh--;
    rakarrack.get(rkr->PrefNom("MIDI IN Stereo Harmonizer"), rkr->StereoHarCh, 1);
    rkr->StereoHarCh--;

    rakarrack.get(rkr->PrefNom("Limiter Position"), rkr->flpos, 0);
    rakarrack.get(rkr->PrefNom("Limiter +6dB"), rkr->db6booster, 0);
    rakarrack.get(rkr->PrefNom("Booster"), rkr->booster, 1.0f);
    if (rkr->booster == 1.0)
    {
        BostBut->value(0);
    }
    else
    {
        BostBut->value(1);
    }

    rakarrack.get(rkr->PrefNom("FX_init_state"), rkr->init_state, 0);
    rakarrack.get(rkr->PrefNom("Auto Assign"), rkr->autoassign, 0);


    if (rkr->init_state)
    {
        rkr->Bypass = 1;
        rkr->calculavol(1);
        rkr->calculavol(2);
    }


    rakarrack.get(rkr->PrefNom("Recognize Optimization Harmonizer"), rkr->RCOpti_Harm, 0);
    Settings->RC_Harm_Opti->value(rkr->RCOpti_Harm);
    Settings->RC_Harm_Opti->do_callback();

    rakarrack.get(rkr->PrefNom("Recognize Optimization Stereo Harm"), rkr->RCOpti_Stereo, 0);
    Settings->RC_St_Harm_Opti->value(rkr->RCOpti_Stereo);
    Settings->RC_St_Harm_Opti->do_callback();

    rakarrack.get(rkr->PrefNom("Recognize Optimization Ring"), rkr->RCOpti_Ring, 0);
    Settings->RC_Ring_Opti->value(rkr->RCOpti_Ring);
    Settings->RC_Ring_Opti->do_callback();

    //MIDIConverter
    rakarrack.get(rkr->PrefNom("Midi Out Channel"), k, 1);
    MIDI->Midi_out_Counter->value(k);
    MIDI->Midi_out_Counter->do_callback();

    rkr->efx_MIDIConverter->setmidichannel(k);

    rakarrack.get(rkr->PrefNom("Trigger Adjust"), k, 4);
    MIDI->Trig_Adj->value(k);
    rkr->efx_MIDIConverter->setTriggerAdjust(k);


    rakarrack.get(rkr->PrefNom("Velocity Adjust"), k, 50);
    MIDI->Vel_Adj->value(k);
    rkr->efx_MIDIConverter->setVelAdjust(k);

    rakarrack.get(rkr->PrefNom("Converter Octave"), k, 2);
    MIDI->MIDIOctave->value(k);
    MIDI->MIDIOctave->do_callback();

    rakarrack.get(rkr->PrefNom("MIDI Converter On/Off"), k, 0);
    MIDI->midi_activar->value(k);
    MIDI->midi_activar->do_callback();

    //Metronome
    rakarrack.get(rkr->PrefNom("Internal Metronome Time"), k, 2);
    MetroBar->value(k);
    MetroBar->do_callback();

    rakarrack.get(rkr->PrefNom("Internal Metronome Sound"), k, 0);
    MetroSound->value(k);
    MetroSound->do_callback();

    rakarrack.get(rkr->PrefNom("Internal Metronome Volume"), k, 50);
    Metro_Volume->value(k);
    Metro_Volume->do_callback();

    rakarrack.get(rkr->PrefNom("Internal Metronome Tempo"), k, 100);
    Metro_Tempo->value(k);
    Metro_Tempo->do_callback();

    rakarrack.get(rkr->PrefNom("Internal Metronome Show"), rkr->sw_stat, 0);

    rakarrack.get(rkr->PrefNom("Internal Metronome On/Off"), k, 0);
    metro_activar->value(k);
    metro_activar->do_callback();

    if (rkr->sw_stat == 1)
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
    rakarrack.get(rkr->PrefNom("Tuner On/Off"), k, 0);
    tuner_activar->value(k);
    tuner_activar->do_callback();


    //Tap Tempo
    rakarrack.get(rkr->PrefNom("TapTempo Input"), rkr->Tap_Selection, 0);
    T_SEL->value(rkr->Tap_Selection);
    rakarrack.get(rkr->PrefNom("Tap Tempo Timeout"), rkr->t_timeout, 0);
    Settings->T_TIMEOUT->value(rkr->t_timeout);
    rakarrack.get(rkr->PrefNom("TapTempo Set"), rkr->Tap_SetValue, 0);
    T_SET->value(rkr->Tap_SetValue);

    rakarrack.get(rkr->PrefNom("TapTempo On/Off"), k, 0);
    Tap_activar->value(k);
    Tap_activar->do_callback();


    rakarrack.get(rkr->PrefNom("Disable Warnings"), rkr->Disable_Warnings, 0);
    rakarrack.get(rkr->PrefNom("Enable Tooltips"), rkr->ena_tool, 1);
    Fl_Tooltip::enable(rkr->ena_tool);

    //Trigger
    rakarrack.get(rkr->PrefNom("Aux Source"), rkr->Aux_Source, 0);
    Trigger->aux_source->value(rkr->Aux_Source);
    rakarrack.get(rkr->PrefNom("Aux Gain"), rkr->Aux_Gain, 0);
    Trigger->aux_gain->value(rkr->Aux_Gain);
    rakarrack.get(rkr->PrefNom("Aux Threshold"), rkr->Aux_Threshold, 0);
    Trigger->aux_thres->value(rkr->Aux_Threshold);
    rakarrack.get(rkr->PrefNom("Aux MIDI"), rkr->Aux_MIDI, 1);
    Trigger->aux_midi->value(rkr->Aux_MIDI);
    rakarrack.get(rkr->PrefNom("Aux Minimum"), rkr->Aux_Minimum, 0);
    Trigger->aux_min->value(rkr->Aux_Minimum);
    rakarrack.get(rkr->PrefNom("Aux Maximum"), rkr->Aux_Maximum, 127);
    Trigger->aux_max->value(rkr->Aux_Maximum);


    char temp[64];


    for (int i = 0; i < 128; i++)
    {
        if (i < 60)
            k = i;
        
        if ((i > 59)&&(i < 120))
            k = 1000 + i - 60;
        
        if (i > 119)
            k = 0;
        
        memset(temp, 0, sizeof (temp));
        sprintf(temp, "Midi Table Program %d", i);
        rakarrack.get(rkr->PrefNom(temp), f, k);


        if (f < 1000)
        {
            rkr->M_table[i].bank = 0;
            rkr->M_table[i].preset = f;
        }

        if ((f > 999) && (f < 2000))
        {
            rkr->M_table[i].bank = 1;
            rkr->M_table[i].preset = f - 1000;
        }

        if ((f > 1999) && (f < 3000))
        {
            rkr->M_table[i].bank = 2;
            rkr->M_table[i].preset = f - 2000;
        }


        if ((f > 2999) && (f < 4000))
        {
            rkr->M_table[i].bank = 3;
            rkr->M_table[i].preset = f - 3000;
        }
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
        rakarrack.set(rkr->PrefNom("Principal X"), Principal->x());
        rakarrack.set(rkr->PrefNom("Principal Y"), Principal->y());
        rakarrack.set(rkr->PrefNom("Principal W"), Principal->w());
        rakarrack.set(rkr->PrefNom("Principal H"), Principal->h());
        rakarrack.set(rkr->PrefNom("FontSize"), rkr->fontsize);
        rakarrack.set(rkr->PrefNom("Font"), rkr->font);

        rakarrack.set(rkr->PrefNom("Background Color"), (int) back_color);
        rakarrack.set(rkr->PrefNom("Foreground Color"), (int) fore_color);
        rakarrack.set(rkr->PrefNom("Leds Color"), (int) leds_color);
        rakarrack.set(rkr->PrefNom("Labels Color"), (int) label_color);
        rakarrack.set(rkr->PrefNom("Schema"), (int) Settings->scheme_ch->value());
        rakarrack.set(rkr->PrefNom("Hide Effects"), (int) rkr->deachide);
        rakarrack.set(rkr->PrefNom("Scale Window"), (int) rkr->scalable);

        rakarrack.set(rkr->PrefNom("Bank Selected"), rkr->a_bank);

        if ((Preset_Counter->value() > 0) && (Preset_Counter->value() < 61))
            rakarrack.set(rkr->PrefNom("Preset Num"), (int) Preset_Counter->value());

        if (rkr->help_displayed)
        {
            rakarrack.set(rkr->PrefNom("Help X"), visor->x());
            rakarrack.set(rkr->PrefNom("Help Y"), visor->y());
            rakarrack.set(rkr->PrefNom("Help W"), visor->w());
            rakarrack.set(rkr->PrefNom("Help H"), visor->h());
            rakarrack.set(rkr->PrefNom("Help TextSize"), visor->textsize());
        }

        //Tuner
        rakarrack.set(rkr->PrefNom("Tuner On/Off"), (int) rkr->Tuner_Bypass);


        //MIDIConverter
        rakarrack.set(rkr->PrefNom("MIDI Converter On/Off"), (int) rkr->MIDIConverter_Bypass);
        rakarrack.set(rkr->PrefNom("Midi Out Channel"), (int) MIDI->Midi_out_Counter->value());
        rakarrack.set(rkr->PrefNom("Trigger Adjust"), (int) MIDI->Trig_Adj->value());
        rakarrack.set(rkr->PrefNom("Velocity Adjust"), (int) MIDI->Vel_Adj->value());
        rakarrack.set(rkr->PrefNom("Converter Octave"), (int) MIDI->MIDIOctave->value());

        //Metronome
        rakarrack.set(rkr->PrefNom("Internal Metronome On/Off"), (int) rkr->Metro_Bypass);
        rakarrack.set(rkr->PrefNom("Internal Metronome Time"), (int) MetroBar->value());
        rakarrack.set(rkr->PrefNom("Internal Metronome Volume"), (int) Metro_Volume->value());
        rakarrack.set(rkr->PrefNom("Internal Metronome Tempo"), (int) Metro_Tempo->value());
        rakarrack.set(rkr->PrefNom("Internal Metronome Show"), (int) rkr->sw_stat);
        rakarrack.set(rkr->PrefNom("Internal Metronome Sound"), (int) MetroSound->value());

        //Booster
        rakarrack.set(rkr->PrefNom("Booster"), rkr->booster);


        //Tap Tempo
        rakarrack.set(rkr->PrefNom("TapTempo On/Off"), (int) rkr->Tap_Bypass);
        rakarrack.set(rkr->PrefNom("TapTempo Input"), (int) rkr->Tap_Selection);
        rakarrack.set(rkr->PrefNom("TapTempo Set"), (int) rkr->Tap_SetValue);
    }


    if (whati == 1)
    {
        rakarrack.set(rkr->PrefNom("BankWindow X"), BankWindow->x());
        rakarrack.set(rkr->PrefNom("BankWindow Y"), BankWindow->y());
        rakarrack.set(rkr->PrefNom("BankWindow W"), BankWindow->w());
        rakarrack.set(rkr->PrefNom("BankWindow H"), BankWindow->h());
    }

    if (whati == 5)
    {
        rakarrack.set(rkr->PrefNom("MIDI Learn X"), MIDILearn->x());
        rakarrack.set(rkr->PrefNom("MIDI Learn Y"), MIDILearn->y());
        rakarrack.set(rkr->PrefNom("MIDI Learn W"), MIDILearn->w());
        rakarrack.set(rkr->PrefNom("MIDI Learn H"), MIDILearn->h());
    }


    if (whati == 6)
    {
        rakarrack.set(rkr->PrefNom("Trigger X"), Trigger->x());
        rakarrack.set(rkr->PrefNom("Trigger Y"), Trigger->y());
        rakarrack.set(rkr->PrefNom("Trigger W"), Trigger->w());
        rakarrack.set(rkr->PrefNom("Trigger H"), Trigger->h());

        rakarrack.set(rkr->PrefNom("Aux Source"), rkr->Aux_Source);
        rakarrack.set(rkr->PrefNom("Aux Gain"), rkr->Aux_Gain);
        rakarrack.set(rkr->PrefNom("Aux Threshold"), rkr->Aux_Threshold);
        rakarrack.set(rkr->PrefNom("Aux MIDI"), rkr->Aux_MIDI);
        rakarrack.set(rkr->PrefNom("Aux Minimum"), rkr->Aux_Minimum);
        rakarrack.set(rkr->PrefNom("Aux Maximum"), rkr->Aux_Maximum);
    }




    if (whati == 2)
    {
        rakarrack.set(rkr->PrefNom("Order X"), Order->x());
        rakarrack.set(rkr->PrefNom("Order Y"), Order->y());
        rakarrack.set(rkr->PrefNom("Order W"), Order->w());
        rakarrack.set(rkr->PrefNom("Order H"), Order->h());
    }

    if (whati == 3)
    {
        rakarrack.set(rkr->PrefNom("Settings X"), Settings->x());
        rakarrack.set(rkr->PrefNom("Settings Y"), Settings->y());
        rakarrack.set(rkr->PrefNom("Settings W"), Settings->w());
        rakarrack.set(rkr->PrefNom("Settings H"), Settings->h());
    }

    if ((whati == 3) || (whati == 0))
    {
        rakarrack.set(rkr->PrefNom("UserName"), rkr->UserRealName);
        rakarrack.set(rkr->PrefNom("Preserve Gain/Master"), rkr->actuvol);
        rakarrack.set(rkr->PrefNom("Metronome Volume"), rkr->Metro_Vol);

        rakarrack.set(rkr->PrefNom("Filter DC Offset"), rkr->DC_Offset);

        rakarrack.set(rkr->PrefNom("Update Tap"), rkr->Tap_Updated);
        rakarrack.set(rkr->PrefNom("Limiter Position"), rkr->flpos);
        rakarrack.set(rkr->PrefNom("Limiter +6dB"), rkr->db6booster);

        rakarrack.set(rkr->PrefNom("Recognize Optimization Harmonizer"), rkr->RCOpti_Harm);
        rakarrack.set(rkr->PrefNom("Recognize Optimization Stereo Harm"), rkr->RCOpti_Stereo);
        rakarrack.set(rkr->PrefNom("Recognize Optimization Ring"), rkr->RCOpti_Ring);

        rakarrack.set(rkr->PrefNom("Harmonizer Downsample"), rkr->Har_Down);
        rakarrack.set(rkr->PrefNom("Harmonizer Up Quality"), rkr->Har_U_Q);
        rakarrack.set(rkr->PrefNom("Harmonizer Down Quality"), rkr->Har_D_Q);
        rakarrack.set(rkr->PrefNom("Reverbtron Downsample"), rkr->Rev_Down);
        rakarrack.set(rkr->PrefNom("Reverbtron Up Quality"), rkr->Rev_U_Q);
        rakarrack.set(rkr->PrefNom("Reverbtron Down Quality"), rkr->Rev_D_Q);
        rakarrack.set(rkr->PrefNom("Convolotron Downsample"), rkr->Con_Down);
        rakarrack.set(rkr->PrefNom("Convolotron Up Quality"), rkr->Con_U_Q);
        rakarrack.set(rkr->PrefNom("Convolotron Down Quality"), rkr->Con_D_Q);
        rakarrack.set(rkr->PrefNom("Sequence Downsample"), rkr->Seq_Down);
        rakarrack.set(rkr->PrefNom("Sequence Up Quality"), rkr->Seq_U_Q);
        rakarrack.set(rkr->PrefNom("Sequence Down Quality"), rkr->Seq_D_Q);
        rakarrack.set(rkr->PrefNom("Shifter Downsample"), rkr->Shi_Down);
        rakarrack.set(rkr->PrefNom("Shifter Up Quality"), rkr->Shi_U_Q);
        rakarrack.set(rkr->PrefNom("Shifter Down Quality"), rkr->Shi_D_Q);
        rakarrack.set(rkr->PrefNom("Vocoder Downsample"), rkr->Voc_Down);
        rakarrack.set(rkr->PrefNom("Vocoder Up Quality"), rkr->Voc_U_Q);
        rakarrack.set(rkr->PrefNom("Vocoder Down Quality"), rkr->Voc_D_Q);
        rakarrack.set(rkr->PrefNom("StereoHarm Downsample"), rkr->Ste_Down);
        rakarrack.set(rkr->PrefNom("StereoHarm Up Quality"), rkr->Ste_U_Q);
        rakarrack.set(rkr->PrefNom("StereoHarm Down Quality"), rkr->Ste_D_Q);

        rakarrack.set(rkr->PrefNom("Distortion Resampling"), rkr->Dist_res_amount);
        rakarrack.set(rkr->PrefNom("Distortion Up Quality"), rkr->Dist_up_q);
        rakarrack.set(rkr->PrefNom("Distortion Down Quality"), rkr->Dist_down_q);

        rakarrack.set(rkr->PrefNom("Overdrive Resampling"), rkr->Ovrd_res_amount);
        rakarrack.set(rkr->PrefNom("Overdrive Up Quality"), rkr->Ovrd_up_q);
        rakarrack.set(rkr->PrefNom("Overdrive Down Quality"), rkr->Ovrd_down_q);

        rakarrack.set(rkr->PrefNom("Derelict Resampling"), rkr->Dere_res_amount);
        rakarrack.set(rkr->PrefNom("Derelict Up Quality"), rkr->Dere_up_q);
        rakarrack.set(rkr->PrefNom("Derelict Down Quality"), rkr->Dere_down_q);
        
        rakarrack.set(rkr->PrefNom("DistBand Resampling"), rkr->DBand_res_amount);
        rakarrack.set(rkr->PrefNom("DistBand Up Quality"), rkr->DBand_up_q);
        rakarrack.set(rkr->PrefNom("DistBand Down Quality"), rkr->DBand_down_q);

        rakarrack.set(rkr->PrefNom("StompBox Resampling"), rkr->Stomp_res_amount);
        rakarrack.set(rkr->PrefNom("StompBox Up Quality"), rkr->Stomp_up_q);
        rakarrack.set(rkr->PrefNom("StompBox Down Quality"), rkr->Stomp_down_q);

        rakarrack.set(rkr->PrefNom("Calibration"), rkr->aFreq);
        rakarrack.set(rkr->PrefNom("Recognize Trigger"), rkr->rtrig);

        rakarrack.set(rkr->PrefNom("Vocoder Bands"), rkr->VocBands);

        rakarrack.set(rkr->PrefNom("FX_init_state"), rkr->init_state);
        rakarrack.set(rkr->PrefNom("Auto Assign"), rkr->autoassign);

        rakarrack.set(rkr->PrefNom("UpSampling"), rkr->upsample);
        rakarrack.set(rkr->PrefNom("UpQuality"), rkr->UpQual);
        rakarrack.set(rkr->PrefNom("DownQuality"), rkr->DownQual);

        rakarrack.set(rkr->PrefNom("UpAmount"), rkr->UpAmo);
        rakarrack.set(rkr->PrefNom("Looper Size"), rkr->looper_size);

        rakarrack.set(rkr->PrefNom("Bank Filename"), rkr->BankFilename);
        rakarrack.set(rkr->PrefNom("User Directory"), rkr->UDirFilename);

        rakarrack.set(rkr->PrefNom("Enable Background Image"), rkr->EnableBackgroundImage);
        rakarrack.set(rkr->PrefNom("Background Image"), rkr->BackgroundImage);
        rakarrack.set(rkr->PrefNom("Auto Connect MIDI IN"), rkr->aconnect_MI);
        rakarrack.set(rkr->PrefNom("Auto Connect Jack"), rkr->aconnect_JA);
        rakarrack.set(rkr->PrefNom("Auto Connect Jack In"), rkr->aconnect_JIA);

        rakarrack.set(rkr->PrefNom("MIDI Implementation"), rkr->MIDIway);
        rakarrack.set(rkr->PrefNom("MIDI Table"), rkr->midi_table);


        int i = Settings->BMidiIn->value();
        char *temp;
        temp = (char*) Settings->BMidiIn->text(i);
        
        if (temp != NULL)
        {
            rakarrack.set(rkr->PrefNom("MIDI IN Device"), temp);
        }
        
        rakarrack.set(rkr->PrefNom("MIDI IN Channel"), rkr->MidiCh + 1);
        rakarrack.set(rkr->PrefNom("MIDI IN Harmonizer"), rkr->HarCh + 1);
        rakarrack.set(rkr->PrefNom("MIDI IN Stereo Harmonizer"), rkr->StereoHarCh + 1);
        rakarrack.set(rkr->PrefNom("Harmonizer Quality"), rkr->HarQual);
        rakarrack.set(rkr->PrefNom("StereoHarm Quality"), rkr->SteQual);
        rakarrack.set(rkr->PrefNom("Sequence Quality"), rkr->SeqQual);
        rakarrack.set(rkr->PrefNom("Shifter Quality"), rkr->ShiQual);
        rakarrack.set(rkr->PrefNom("Tap Tempo Timeout"), rkr->t_timeout);
        rakarrack.set(rkr->PrefNom("Disable Warnings"), rkr->Disable_Warnings);
        rakarrack.set(rkr->PrefNom("Enable Tooltips"), rkr->ena_tool);

        for (int i = 0; i < 128; i++)
        {
            memset(temp1, 0, sizeof (temp1));
            sprintf(temp1, "Midi Table Program %d", i);
            rakarrack.set(rkr->PrefNom(temp1), rkr->M_table[i].bank * 1000 + rkr->M_table[i].preset);
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
                rakarrack.set(rkr->PrefNom(temp1), Settings->JackCo->text(i));
                k++;
            }
        }

        rakarrack.set(rkr->PrefNom("Auto Connect Num"), k - 1);

        k = 1;
        for (int i = 1; i <= Settings->JackIn->size(); i++)
        {
            if (Settings->JackIn->selected(i))
            {
                memset(temp1, 0, sizeof (temp1));
                sprintf(temp1, "Jack Port In %d", k);
                rakarrack.set(rkr->PrefNom(temp1), Settings->JackIn->text(i));
                k++;
            }
        }

        rakarrack.set(rkr->PrefNom("Auto Connect In Num"), k - 1);
    }
}

void RKRGUI::Put_Loaded()
{
    //General loading of efx default settings

    WPreset_Name->value(rkr->Preset_Name);
    DAuthor->copy_label(rkr->Author);

    Nivel_Entrada->value((int) (rkr->Input_Gain * 100.0) - 50);
    rkr->calculavol(1);
    Nivel_Salida->value((int) (rkr->Master_Volume * 100.0) - 50);
    rkr->calculavol(2);
    Balance->value((int) (rkr->Fraction_Bypass * 100.0));


    ActivarGeneral->value(rkr->Bypass);

    if (rkr->Bypass == 0)
    {
        rkr->val_il_sum = -50;
        rkr->val_ir_sum = -50;

        rkr->val_vl_sum = -50;
        rkr->val_vr_sum = -50;

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
        switch(rkr->efx_order[i]){
     */

    for (int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        // Cabinet is special
        if( i == EFX_CABINET)
        {
            CABINET->Cabinet_output->value(rkr->lv[EFX_CABINET][Cabinet_Gain] - 64);
            CABINET->Cabinet_preset->value(rkr->lv[EFX_CABINET][Cabinet_Preset_Idx]);
            CABINET->Cabinet_activar->value(rkr->EFX_Bypass[EFX_CABINET]);
        }
        else
        {
            // Calls the preset callback and sets the effect to the first preset
            Efx_Gui_Base[i]->activate_effect->value (rkr->EFX_Bypass[i]);
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

    int tecla = Fl::event_key();
    long long kk = (long long) o->user_data();
    int num = (int) kk;

    if (drag != 1000)
    {
        int en = search_but(Fl::event_x(), Fl::event_y());
        if (en != 1000)
        {
            rkr->Bank[0] = rkr->Bank[en];
            rkr->Bank[en] = rkr->Bank[num];
            rkr->Bank[num] = rkr->Bank[0];
            Put_Loaded_Bank();
            rkr->modified = 1;
            num = en;
            drag = 1000;
        }
    }

    if ((Fl::event_is_click() == 0)&&(Fl::event() == FL_DRAG))
    {
        drag = num;
        return;
    }
    else
    {
        drag = 1000;
    }

    if ((Fl::event_button() == FL_RIGHT_MOUSE) && (Fl::event() == FL_RELEASE))
    {
        Fl_Widget *w = BankWindow->ob->child(num - 1);

        // Check if user really wants to set the preset to the bank
        Fl_Widget *m = fl_message_icon();
        m->parent()->copy_label(rkr->jackcliname);
        int ok = fl_choice("Overwrite \"%s\"?", "No", "Yes", NULL, w->label());
        if (!ok)
        {
            o->value(0);
            o->redraw();
            return;
        }

        o->value(0);
        o->redraw();
        rkr->preset_to_bank(num);
        w->copy_label(rkr->Preset_Name);
        rkr->modified = 1;
    }


    if ((Fl::event_button() == FL_LEFT_MOUSE) || tecla == SPACE_BAR)
    {
        if ((num != rkr->Selected_Preset) || (rkr->new_bank_loaded))
        {
            Fl_Widget *w = BankWindow->ob->child(num - 1);
            BankWindow->unlight_preset(rkr->Selected_Preset);
            rkr->Selected_Preset = num;
            w->color(fl_darker(leds_color));
            Preset_Counter->value(num);
            Preset_Counter->do_callback();
        }
    }
}

void RKRGUI::reordena()
{
    // Reorder efx
    unsigned int x[10], y[10];
    ulong s[10];

    s[0] = 49;
    s[1] = 50;
    s[2] = 51;
    s[3] = 52;
    s[4] = 53;
    s[5] = 54;
    s[6] = 55;
    s[7] = 56;
    s[8] = 57;
    s[9] = 48;


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
            if (j == rkr->efx_order[i])
            {
                // Sets the position in the main window
                Efx_Gui_Base[j]->position(x[i], y[i]);
                Efx_Gui_Base[j]->activate_effect->shortcut (s[i]);
                
                // This is the Hide/Show inactive effect button choice
                // If it is not hide, then show all effects
                if (!rkr->deachide)
                {
                    Efx_Gui_Base[j]->show();
                }

                // Set the active status, for label highlighting
                // If the Hide/Show button is set for hide, then show only active
                if (rkr->EFX_Bypass[j])
                {
                    rkr->active[i] = 1;

                    // Show the active
                    if (rkr->deachide)
                        Efx_Gui_Base[j]->show();
                }
                else
                {
                    rkr->active[i] = 0;
                }
            }
        }

        // Sets the effect label
        switch (i)
        {
            case 0:
                L1->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L1->hide();
                else
                    L1->show();
                break;
            case 1:
                L2->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L2->hide();
                else
                    L2->show();
                break;
            case 2:
                L3->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L3->hide();
                else
                    L3->show();
                break;
            case 3:
                L4->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L4->hide();
                else
                    L4->show();
                break;
            case 4:
                L5->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L5->hide();
                else
                    L5->show();
                break;
            case 5:
                L6->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L6->hide();
                else
                    L6->show();
                break;
            case 6:
                L7->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L7->hide();
                else
                    L7->show();
                break;
            case 7:
                L8->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L8->hide();
                else
                    L8->show();
                break;
            case 8:
                L9->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
                    L9->hide();
                else
                    L9->show();
                break;
            case 9:
                L10->copy_label(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
                if ((rkr->deachide) && (!rkr->active[i]))
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

    if (rkr->help_displayed == 0)
    {
        visor = new Fl_Help_Dialog;

        int x, y, w, h, k;
        rakarrack.get(rkr->PrefNom("Help X"), x, 1);
        rakarrack.get(rkr->PrefNom("Help Y"), y, 1);
        rakarrack.get(rkr->PrefNom("Help W"), w, 640);
        rakarrack.get(rkr->PrefNom("Help H"), h, 480);
        rakarrack.get(rkr->PrefNom("Help TextSize"), k, 11);
        visor->resize(x, y, w, h);
        visor->textsize((unsigned char) k);
    }


    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/html/help.html", HELPDIR);


    if (FILE * file = fopen(temp, "r"))
    {
        fclose(file);
        visor->load(temp);
        rkr->help_displayed = 1;
        visor->show();
    }
}

void RKRGUI::show_lic()
{
    // Show licenses
    char temp[128];

    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

    if (rkr->help_displayed == 0)
    {
        visor = new Fl_Help_Dialog;

        int x, y, w, h, k;
        rakarrack.get(rkr->PrefNom("Help X"), x, 1);
        rakarrack.get(rkr->PrefNom("Help Y"), y, 1);
        rakarrack.get(rkr->PrefNom("Help W"), w, 640);
        rakarrack.get(rkr->PrefNom("Help H"), h, 480);
        rakarrack.get(rkr->PrefNom("Help TextSize"), k, 11);
        visor->resize(x, y, w, h);
        visor->textsize((unsigned char) k);
    }

    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/html/license.html", HELPDIR);
    visor->load(temp);

    rkr->help_displayed = 1;

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


    if ((ports = jack_get_ports(rkr->jackclient, NULL, JACK_DEFAULT_AUDIO_TYPE,
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

    if ((iports = jack_get_ports(rkr->jackclient, NULL, JACK_DEFAULT_AUDIO_TYPE,
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
            if (strcmp(Settings->BMidiIn->text(i), rkr->MID) == 0)
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
            for (int k = 0; k < rkr->cuan_jack; k++)
            {
                if (strcmp(Settings->JackCo->text(i), rkr->jack_po[k].name) == 0)
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
            for (int k = 0; k < rkr->cuan_ijack; k++)
            {
                if (strcmp(Settings->JackIn->text(i), rkr->jack_poi[k].name) == 0)
                {
                    Settings->JackIn->select(i, 1);
                }
            }

            i++;
        }
    }

    if (rkr->MIDIway)
    {
        Settings->Mw1->setonly();
    }
    else
    {
        Settings->Mw0->setonly();
    }

    rkr->m_displayed = 0;
    Settings->Enable_Back->value(rkr->EnableBackgroundImage);
    Settings->Enable_DeacHide->value(rkr->deachide);
    Settings->Enable_Scale->value(rkr->scalable);

    Settings->BFiname->value(rkr->BankFilename);
    Settings->BackFiname->value(rkr->BackgroundImage);
    Settings->Udir->value(rkr->UDirFilename);
    Settings->Username->value(rkr->UserRealName);
    Settings->Pre_Serve->value(rkr->actuvol);
    Settings->LM_Volume->value(rkr->Metro_Vol);
    Settings->Filter_DC->value(rkr->DC_Offset);
    Settings->FLPosition->value(rkr->flpos);
    Settings->DB6B->value(rkr->db6booster);
    Settings->Har_Downsample->value(rkr->Har_Down);
    Settings->Har_Down_Qua->value(rkr->Har_D_Q);
    Settings->Har_Up_Qua->value(rkr->Har_U_Q);
    Settings->Rev_Downsample->value(rkr->Rev_Down);
    Settings->Rev_Down_Qua->value(rkr->Rev_D_Q);
    Settings->Rev_Up_Qua->value(rkr->Rev_U_Q);
    Settings->Con_Downsample->value(rkr->Con_Down);
    Settings->Con_Down_Qua->value(rkr->Con_D_Q);
    Settings->Con_Up_Qua->value(rkr->Con_U_Q);
    Settings->Seq_Downsample->value(rkr->Seq_Down);
    Settings->Seq_Down_Qua->value(rkr->Seq_D_Q);
    Settings->Seq_Up_Qua->value(rkr->Seq_U_Q);
    Settings->Shi_Downsample->value(rkr->Shi_Down);
    Settings->Shi_Down_Qua->value(rkr->Shi_D_Q);
    Settings->Shi_Up_Qua->value(rkr->Shi_U_Q);
    Settings->Voc_Downsample->value(rkr->Voc_Down);
    Settings->Voc_Down_Qua->value(rkr->Voc_D_Q);
    Settings->Voc_Up_Qua->value(rkr->Voc_U_Q);
    Settings->Ste_Downsample->value(rkr->Ste_Down);
    Settings->Ste_Down_Qua->value(rkr->Ste_D_Q);
    Settings->Ste_Up_Qua->value(rkr->Ste_U_Q);
    
    Settings->Dist_Amo->value(rkr->Dist_res_amount);
    Settings->Dist_Up_Qua->value(rkr->Dist_up_q);
    Settings->Dist_Down_Qua->value(rkr->Dist_down_q);
    
    Settings->Ovrd_Amo->value(rkr->Ovrd_res_amount);
    Settings->Ovrd_Up_Qua->value(rkr->Ovrd_up_q);
    Settings->Ovrd_Down_Qua->value(rkr->Ovrd_down_q);
    
    Settings->Dere_Amo->value(rkr->Dere_res_amount);
    Settings->Dere_Up_Qua->value(rkr->Dere_up_q);
    Settings->Dere_Down_Qua->value(rkr->Dere_down_q);
    
    Settings->DBand_Amo->value(rkr->DBand_res_amount);
    Settings->DBand_Up_Qua->value(rkr->DBand_up_q);
    Settings->DBand_Down_Qua->value(rkr->DBand_down_q);
    
    Settings->Stomp_Amo->value(rkr->Stomp_res_amount);
    Settings->Stomp_Up_Qua->value(rkr->Stomp_up_q);
    Settings->Stomp_Down_Qua->value(rkr->Stomp_down_q);

    Settings->Calibration->value(rkr->aFreq);
    Settings->RTrigger->value(rkr->HarmRecNote->trigfact);
    Settings->RC_Harm_Opti->value(rkr->RCOpti_Harm);
    Settings->RC_St_Harm_Opti->value(rkr->RCOpti_Stereo);
    Settings->RC_Ring_Opti->value(rkr->RCOpti_Ring);


    switch (rkr->VocBands)
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


    Settings->MTable->value(rkr->midi_table);

    Settings->AAssign->value(rkr->autoassign);
    Settings->Update_TAP->value(rkr->Tap_Updated);
    Settings->INSTATE->value(rkr->init_state);
    Settings->UPSAMPLE_C->value(rkr->upsample);
    Settings->Upr_Qual->value(rkr->UpQual);
    Settings->Downr_Qual->value(rkr->DownQual);
    Settings->MESSAGE_DISABLE->value(rkr->Disable_Warnings);
    Settings->ENA_TOOL->value(rkr->ena_tool);
    Settings->T_TIMEOUT->value(rkr->t_timeout);

    Settings->Upr_Amo->value(rkr->UpAmo);
    Settings->L_SIZE->value(rkr->looper_size);
    Settings->D_A_Connect->value(rkr->aconnect_MI);
    Settings->D_J_Connect->value(rkr->aconnect_JA);
    Settings->D_IJ_Connect->value(rkr->aconnect_JIA);

    Settings->Midi_In_Counter->value(rkr->MidiCh + 1);
    Settings->Har_In_Counter->value(rkr->HarCh + 1);
    Settings->Stereo_Har_In_Counter->value(rkr->StereoHarCh + 1);

    switch (rkr->HarQual)
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

    switch (rkr->SteQual)
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
    
    switch (rkr->SeqQual)
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

    switch (rkr->ShiQual)
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


    if (rkr->aconnect_MI)
    {
        Settings->BMidiIn->activate();
    }
    else
    {
        Settings->BMidiIn->deactivate();
    }

    if (rkr->aconnect_JA)
    {
        Settings->JackCo->activate();
    }
    else
    {
        Settings->JackCo->deactivate();
    }

    if (rkr->aconnect_JIA)
    {
        Settings->JackIn->activate();
    }
    else
    {
        Settings->JackIn->deactivate();
    }

    Fl_Menu_Item *p;
    Fl_Menu_Item *Har = Settings->get_menu_Har_Downsample();
    unsigned int SR_value = rkr->sample_rate;

    for (int j = 0; j < Har->size(); j++)
    {
        p = Har->next(j);

        switch (j)
        {
            case 0:
                SR_value = rkr->sample_rate;
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

        if ((j > 0) && ((unsigned int) rkr->sample_rate <= SR_value))
        {
            p->deactivate();
        }
        else
        {
            p->activate();
        }
    }


    Settings->Font_Bro->clear();
    Settings->Font_Bro->textcolor(label_color);

    int k = Fl::set_fonts(0);
    for (int i = 0; i < k; i++)
    {
        int t;
        const char *name = Fl::get_font_name((Fl_Font) i, &t);
        char buffer[128];
#if 1
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

    Settings->Font_Bro->value(rkr->font + 1);
}

void RKRGUI::BankWin_Label(const char *filename)
{
    // add the bankwindow label
    char tmp[256];

    memset(tmp, 0, sizeof (tmp));
    sprintf(tmp, "%s   v%s - Bank Manager - %s", rkr->jackcliname, VERSION, fl_filename_name(filename));
    BankWindow->copy_label(tmp);
}

void RKRGUI::is_modified()
{
    // popup dialog for modified bank saving

    if (rkr->modified)
    {
        Fl_Widget *w = fl_message_icon();
        w->parent()->copy_label(rkr->jackcliname);

        int ok = fl_choice("Bank was modified, but not saved", "Discard", "Save", NULL);

        switch (ok)
        {
            case 0:
                break;

            case 1:
                set_save_file();
                break;
        }
    }
}

void RKRGUI::Put_Loaded_Bank()
{
    // Put loaded bank
    int k = 1;

    for (int t = 0; t < BankWindow->ob->children(); t++)
    {
        Fl_Widget *w = BankWindow->ob->child(t);
        long long temp = (long long) w->user_data();
        if (temp > 0)
        {
            w->copy_label(rkr->Bank[k].Preset_Name);
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
            tipo = rkr->Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Chord);
            undi = rkr->Rack_Effects[EFX_HARMONIZER]->getpar(Harm_Note);
            break;
        case 1:
            tipo = rkr->Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Chord);
            undi = rkr->Rack_Effects[EFX_STEREOHARM]->getpar(Sharm_Note);
            break;
    }


    if (eff == 0)
    {
        sprintf(rkr->RC_Harm->NombreAcorde, "%s%s", rkr->RC_Harm->NCE[undi].Nom, rkr->RC_Harm->ChN[tipo].Nom);

        rkr->RC_Harm->ctipo = tipo;
        rkr->RC_Harm->fundi = undi;
        rkr->RC_Harm->cc = 1;
    }
    else
    {
        sprintf(rkr->RC_Stereo_Harm->NombreAcorde, "%s%s", rkr->RC_Stereo_Harm->NCE[undi].Nom, rkr->RC_Stereo_Harm->ChN[tipo].Nom);

        rkr->RC_Stereo_Harm->ctipo = tipo;
        rkr->RC_Stereo_Harm->fundi = undi;
        rkr->RC_Stereo_Harm->cc = 1;
    }
}

/**
 *  Adjust Gui parameters from MIDI control action.
 */
void RKRGUI::MIDI_control_gui_refresh()
{
    // Mvalue is flag to indicate at least one parameter needs update
    if (!rkr->Mvalue)
        return;

    // Index starts at 1, since case 0 is unused
    for (int mc_index = 1; mc_index < C_MC_RANGE; mc_index++)
    {
        // Mcontrol is flag for each individual parameter
        // 1 = the parameter needs update, 0 = no update needed
        if (!rkr->Mcontrol[mc_index])
        {
            continue;   // No gui update needed
        }
        
        // Special cases Master controls
        switch(mc_index)
        {
            case MC_Output_Volume:
                rkr->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Nivel_Salida->value((int) (rkr->Master_Volume * 100.0) - 50);
                Nivel_Salida->redraw();
                break;

            case MC_Balance_FX:
                rkr->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Balance->value(rkr->Fraction_Bypass * 100.0);
                Balance->redraw();
                break;

            case MC_Input_Volume:
                rkr->Mcontrol[mc_index] = 0;   // clear the parameter flag
                Nivel_Entrada->value((int) (rkr->Input_Gain * 100.0) - 50);
                Nivel_Entrada->redraw();
                break;

            case MC_Multi_On_Off:
                rkr->Mcontrol[mc_index] = 0;   // clear the parameter flag
                ActOnOff();
                break;
        }
        
        if (!rkr->Mcontrol[mc_index])
        {
            continue;   // We found it in above special cases
        }
        
        // Normal MIDI processing of rack effects
        for (int param_index = 0; param_index < C_MC_PARAMETER_SIZE; param_index++)
        {
            // Find the effect parameter that needs update
            if(rkr->mc_efx_params[param_index].MC_params_index == mc_index)
            {
                rkr->Mcontrol[mc_index] = 0;   // clear the parameter flag
                
                // Get the effect and parameter
                int effect_index = rkr->mc_efx_params[param_index].Effect_index;
                int efx_param_index = rkr->mc_efx_params[param_index].Efx_param_index;

                // Tell the gui effect to update the parameter
                Efx_Gui_Base[effect_index]->parameter_refresh (efx_param_index);
                break;
            }
        }
    }

    // Clear the flag
    rkr->Mvalue = 0;
}

/**
 * Turn effect on or off from MIDI control (default CC #116)
 */
void RKRGUI::ActOnOff()
{
    int miralo = 0;

    while (rkr->OnOffC > 0)
    {
        if (rkr->Mnumeff[rkr->OnOffC] > 2000)
        {
            miralo = rkr->Mnumeff[rkr->OnOffC] - 2000;
        }
        else if (rkr->Mnumeff[rkr->OnOffC] >= 1000)
        {
            miralo = rkr->Mnumeff[rkr->OnOffC] - 1000;
        }
        else    // efx_order[] is limited by C_NUMBER_ORDERED_EFFECTS
        {
            miralo = rkr->efx_order[rkr->Mnumeff[rkr->OnOffC]];
        }
        
        // Check for rack effects
        if (miralo < C_NUMBER_EFFECTS)
        {
            Efx_Gui_Base[miralo]->activate_effect->value (rkr->EFX_Bypass[miralo]);
            Efx_Gui_Base[miralo]->activate_effect->do_callback ();
            rkr->OnOffC--;
            continue;
        }
        else
        {
            switch (miralo)
            {
                case EFX_TAP_TEMPO_ON_OFF:
                    Tap_activar->value(rkr->Tap_Bypass);
                    Tap_activar->do_callback();
                    break;
                case EFX_MIDI_CONVERTER_ON_OFF:
                    MIDI->midi_activar->value(rkr->MIDIConverter_Bypass);
                    MIDI->midi_activar->do_callback();
                    break;
                case EFX_TUNER_ON_OFF:
                    tuner_activar->value(rkr->Tuner_Bypass);
                    tuner_activar->do_callback();
                    break;
                case EFX_MASTER_ON_OFF:
                    ActivarGeneral->value(rkr->Bypass);
                    ActivarGeneral->do_callback();
                    break;
            }
        }

        rkr->OnOffC--;
    }
}

void RKRGUI::PutBackground()
{
    // Put selected .png background image on everything
    delete back;
    back = new Fl_Tiled_Image(new Fl_PNG_Image(rkr->BackgroundImage), 3200, 2400);
    
    if (!rkr->EnableBackgroundImage)
    {
        back->color_average(back_color, 0.0);
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

/**
 *  Changes the font size, colors and font type.
 * 
 * @param font_size
 *      The amount to change the font size. When != 0, it comes from the
 *      user adjusting the font up or down from the Settings/Preferences/Look
 *      + or - buttons for font size.
 */
void RKRGUI::chfsize(int font_size)
{
    /* global_font_size is used by all RKR widget overrides to adjust font size in draw().
     * This variable should only be adjusted here. */
    if(font_size)
    {
        global_font_size = rkr->fontsize = font_size;
    }

    /* Sort through widgets and adjust font colors and type */
    for (int t = 0; t < Principal->children(); t++)
    {
        Fl_Widget *w = Principal->child(t);
        long long ud = (long long) w->user_data();

        if (ud != BOX_LED_DATA)
        {
            w->labelcolor(label_color);     /* Lmt, Clip, Resample */
        }
        else
        {
            w->labelcolor(leds_color);      /* Aux, In, Out, 0.0% */
        }
        
        if (ud != 2)
        {
            w->selection_color(back_color); /* ????? */
        }
        else
        {
            w->selection_color(leds_color); /* ???? */
        }


        /* EFX, tuner, tap, volume control, etc */
        if (ud == 1)
        {
            Fl_Group *g = (Fl_Group *) w;

            for (int i = 0; i < g->children(); i++)
            {
                Fl_Widget *c = g->child(i);
                long long uh = (long long) c->user_data();

                if (uh != BOX_LED_DATA)
                {
                    c->labelcolor(label_color);     /* Labels on (almost) everything except sliders */
                }
                else
                {
                    c->labelcolor(leds_color);      /* ????? */
                }

                if (uh != 7)
                {
                    c->selection_color(back_color); /* ????? */
                }

                /* All efx On buttons, +10db, selections boxes, and all efx other buttons */
                if ((uh == 2) || (uh == 7) || (uh == 77) || (uh == 78))
                {
                    c->selection_color(leds_color);
                }

                c->color(fore_color);
                c->labelfont(rkr->font);
            }
        }
    }

    CLIP_LED->selection_color(FL_RED);
    Etit->labelcolor(leds_color);
    Trigger->aux_midi->color(fore_color);
    Trigger->aux_midi->textcolor(FL_BACKGROUND2_COLOR);

    ChangeActives();

    Fl::redraw();
}

void RKRGUI::ChangeActives()
{
    //Change label colors based on active or inactive
    Fl_Color on = fl_lighter(fl_lighter(label_color));
    Fl_Color off = fl_darker(label_color);


    if (rkr->active[0])
        L1->labelcolor(on);
    else
        L1->labelcolor(off);
    if (rkr->active[1])
        L2->labelcolor(on);
    else
        L2->labelcolor(off);
    if (rkr->active[2])
        L3->labelcolor(on);
    else
        L3->labelcolor(off);
    if (rkr->active[3])
        L4->labelcolor(on);
    else
        L4->labelcolor(off);
    if (rkr->active[4])
        L5->labelcolor(on);
    else
        L5->labelcolor(off);
    if (rkr->active[5])
        L6->labelcolor(on);
    else
        L6->labelcolor(off);
    if (rkr->active[6])
        L7->labelcolor(on);
    else
        L7->labelcolor(off);
    if (rkr->active[7])
        L8->labelcolor(on);
    else
        L8->labelcolor(off);
    if (rkr->active[8])
        L9->labelcolor(on);
    else
        L9->labelcolor(off);
    if (rkr->active[9])
        L10->labelcolor(on);
    else
        L10->labelcolor(off);



    if (rkr->MIDIConverter_Bypass)
        MIDI->MIDI_LABEL->labelcolor(on);
    else
        MIDI->MIDI_LABEL->labelcolor(off);
    if (rkr->Metro_Bypass)
        Metro_Label->labelcolor(on);
    else
        Metro_Label->labelcolor(off);
    if (rkr->Tap_Bypass)
        TAP_LABEL->labelcolor(on);
    else
        TAP_LABEL->labelcolor(off);
    if (rkr->Tuner_Bypass)
        TUNER_LABEL->labelcolor(on);
    else
        TUNER_LABEL->labelcolor(off);
    if (rkr->Bypass)
        LABEL_IO->labelcolor(on);
    else
        LABEL_IO->labelcolor(off);

    if ((rkr->upsample) && (rkr->Bypass))
    {
        UPS_LED->color(leds_color);
        UPS_LED->redraw();
    }
    else
    {
        UPS_LED->color(fl_darker(leds_color));
        UPS_LED->redraw();
    }

    CLIP_LED->color(fl_darker(FL_RED));
    LMT_LED->color(fl_darker(leds_color));
}

void RKRGUI::findpos(int num, int value, Fl_Widget*)
{
    // adjust efx title label colors based on active of inactive
    int i = 0;
    Fl_Color on = fl_lighter(fl_lighter(label_color));
    Fl_Color off = fl_darker(label_color);

    for (i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        if (rkr->efx_order[i] == num)
        {
            rkr->active[i] = value;
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
    // adjust based on theme selected in settings-look
    Principal->resize(Principal->x(), Principal->y(), rkr->swidth, rkr->sheight);

    back_color = (Fl_Color) rkr->sback_color;
    fore_color = (Fl_Color) rkr->sfore_color;
    leds_color = (Fl_Color) rkr->sleds_color;
    label_color = (Fl_Color) rkr->slabel_color;

    Settings->scheme_ch->value(rkr->sschema);
    Settings->scheme_ch->do_callback();
    PutBackground();
    chfsize(rkr->fontsize);
    Leds_Color_Change(leds_color);
    Buttons_Color_Change(fore_color);


    save_current_state(3);
    Fl::redraw();
}

void RKRGUI::FillML(/*int type*/)
{
    // Midi learn
    char tmp[256];
    memset(tmp, 0, sizeof (tmp));


    sprintf(tmp, "%s   v%s - MIDI Learn - Preset : %s", rkr->jackcliname, VERSION, rkr->Bank[rkr->Selected_Preset].Preset_Name);
    MIDILearn->copy_label(tmp);

    memset(rkr->ML_clist, 0, sizeof (rkr->ML_clist));
    MIDILearn->Epar->clear();
    
    int k = 0;

    switch (rkr->ML_filter)
    {
        case 0:
            for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
                MIDILearn->Epar->add(rkr->mc_efx_params[i].Description);
            break;

        case 1:
            for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
            {
                if (rkr->mc_efx_params[i].Effect_index == EFX_MASTER)
                {
                    MIDILearn->Epar->add(rkr->mc_efx_params[i].Description);
                    rkr->ML_clist[k] = rkr->mc_efx_params[i].MC_params_index;
                    k++;
                }
            }

            for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
            {
                for (int i = 0; i < C_MC_PARAMETER_SIZE; i++)
                {
                    if (rkr->mc_efx_params[i].Effect_index == rkr->efx_order[j])
                    {
                        MIDILearn->Epar->add(rkr->mc_efx_params[i].Description);
                        rkr->ML_clist[k] = rkr->mc_efx_params[i].MC_params_index;
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
        MIDILearn->TPresets->add(rkr->Bank[i].Preset_Name);
    }

    MIDILearn->TPresets->select(rkr->Selected_Preset, 1);
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
        if (rkr->ML_filter == 0)
        {
            the_one = rkr->mc_efx_params[(int) MIDILearn->Epar->value() - 1].MC_params_index;
        }
        else
        {
            the_one = rkr->ML_clist[(int) MIDILearn->Epar->value() - 1];
        }
    }

    int k = 0;

    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (rkr->XUserMIDI[i][j] == the_one)
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
        rkr->new_order[i] = rkr->efx_order[i];
        rkr->saved_order[i] = rkr->efx_order[i];
        Order->Order_Bro->add(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
    }

    Fill_Avail(rkr->Effect_Type_Filter);

    Order->Order_Bro->select(1);
}

void RKRGUI::Show_Next_Time()
{
    // popup for settings changes that will not take effect until restart
    if (rkr->Disable_Warnings)
        return;

    Fl_Widget *w = fl_message_icon();
    w->parent()->copy_label(rkr->jackcliname);
    fl_message("This setting will be changed the next time you run rakarrack-plus");
}

void RKRGUI::update_looper()
{
    // update looper
    Looper *Efx_Looper = static_cast <Looper*> (rkr->Rack_Effects[EFX_LOOPER]);
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
 */
void RKRGUI::UpdateTGUI()
{
    if (rkr->EFX_Bypass[EFX_LOOPER])
    {
        LOOPER->looper_Tempo->value(rkr->Rack_Effects[EFX_LOOPER]->getpar(Looper_Tempo));
        LOOPER->looper_Tempo->redraw();
    }

    if (rkr->EFX_Bypass[EFX_CHORUS])
    {
        CHORUS->chorus_freq->value(rkr->Rack_Effects[EFX_CHORUS]->getpar(Chorus_LFO_Tempo));
        CHORUS->chorus_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_FLANGER])
    {
        FLANGER->flanger_freq->value(rkr->Rack_Effects[EFX_FLANGER]->getpar(Flanger_LFO_Tempo));
        FLANGER->flanger_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_PHASER])
    {
        PHASER->phaser_freq->value(rkr->Rack_Effects[EFX_PHASER]->getpar(Phaser_LFO_Tempo));
        PHASER->phaser_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_PAN])
    {
        PAN->pan_freq->value(rkr->Rack_Effects[EFX_PAN]->getpar(Pan_LFO_Tempo));
        PAN->pan_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_WAHWAH])
    {
        WAHWAH->WahWah_freq->value(rkr->Rack_Effects[EFX_WAHWAH]->getpar(WahWah_LFO_Tempo));
        WAHWAH->WahWah_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_ALIENWAH])
    {
        ALIENWAH->Alienwah_freq->value(rkr->Rack_Effects[EFX_ALIENWAH]->getpar(Alien_LFO_Tempo));
        ALIENWAH->Alienwah_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_MUSICAL_DELAY])
    {
        MUSDELAY->musdelay_tempo->value(rkr->Rack_Effects[EFX_MUSICAL_DELAY]->getpar(Music_Tempo));
        MUSDELAY->musdelay_tempo->redraw();
    }

    if (rkr->EFX_Bypass[EFX_ANALOG_PHASER])
    {
        APHASER->aphaser_freq->value(rkr->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Tempo));
        APHASER->aphaser_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_DUAL_FLANGE])
    {
        DFLANGE->dflange_freq->value(rkr->Rack_Effects[EFX_DUAL_FLANGE]->getpar(DFlange_LFO_Tempo));
        DFLANGE->dflange_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_SYNTHFILTER])
    {
        SYNTHFILTER->synthfilter_freq->value(rkr->Rack_Effects[EFX_SYNTHFILTER]->getpar(Synthfilter_LFO_Tempo));
        SYNTHFILTER->synthfilter_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_MUTROMOJO])
    {
        MUTROMOJO->mutromojo_freq->value(rkr->Rack_Effects[EFX_MUTROMOJO]->getpar(MuTro_LFO_Tempo));
        MUTROMOJO->mutromojo_freq->redraw();
    }

    if (rkr->EFX_Bypass[EFX_VARYBAND])
    {
        VARYBAND->varyband_freq1->value(rkr->Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Tempo_1));
        VARYBAND->varyband_freq1->redraw();
    }

    if (rkr->EFX_Bypass[EFX_VARYBAND])
    {
        VARYBAND->varyband_freq2->value(rkr->Rack_Effects[EFX_VARYBAND]->getpar(VaryBand_LFO_Tempo_2));
        VARYBAND->varyband_freq2->redraw();
    }

    if (rkr->EFX_Bypass[EFX_ARPIE])
    {
        ARPIE->arpie_delay->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Tempo));
        ARPIE->arpie_delay->redraw();
    }

    if (rkr->EFX_Bypass[EFX_ECHOVERSE])
    {
        ECHOVERSE->echoverse_delay->value(rkr->Rack_Effects[EFX_ECHOVERSE]->getpar(Echoverse_Tempo));
        ECHOVERSE->echoverse_delay->redraw();
    }
    if (rkr->EFX_Bypass[EFX_SEQUENCE])
    {
        SEQUENCE->seq_tempo->value(rkr->Rack_Effects[EFX_SEQUENCE]->getpar(Sequence_Tempo));
        SEQUENCE->seq_tempo->redraw();
    }
    if (rkr->EFX_Bypass[EFX_ECHO])
    {
        ECHO->echo_delay->value(rkr->Rack_Effects[EFX_ECHO]->getpar(Echo_Delay));
        ECHO->echo_delay->redraw();
    }
    if (rkr->EFX_Bypass[EFX_ECHOTRON])
    {
        ECHOTRON->echotron_tempo->value(rkr->Rack_Effects[EFX_ECHOTRON]->getpar(Echotron_Tempo));
        ECHOTRON->echotron_tempo->redraw();
    }
    if (rkr->EFX_Bypass[EFX_OPTICALTREM])
    {
        OTREM->otrem_freq->value(rkr->Rack_Effects[EFX_OPTICALTREM]->getpar(Optical_LFO_Tempo));
        OTREM->otrem_freq->redraw();
    }
    if (rkr->EFX_Bypass[EFX_VIBE])
    {
        VIBE->vibe_freq->value(rkr->Rack_Effects[EFX_VIBE]->getpar(Vibe_LFO_Tempo));
        VIBE->vibe_freq->redraw();
    }
    if (rkr->EFX_Bypass[EFX_INFINITY])
    {
        INFINIT->infinity_rate->value(rkr->Rack_Effects[EFX_INFINITY]->getpar(Infinity_Tempo));
        INFINIT->infinity_rate->redraw();
    }
}

/**
 * Analog Control effect.
 */
void RKRGUI::ActACI()
{
    float gain = 0.0;
    float tmp = 0.0;

    switch (rkr->Aux_Source)
    {
        case 0:
            gain = dB2rap(75.0f * (float) rkr->Aux_Gain / 127.0f);
            tmp = rkr->val_a_sum * gain;
            break;
        case 1:
            gain = (float) rkr->Aux_Gain / 127.0f;
            tmp = dB2rap(rkr->val_il_sum) * 12.0f * gain;
            break;
        case 2:
            gain = (float) rkr->Aux_Gain / 127.0f;
            tmp = dB2rap(rkr->val_ir_sum) * 12.0f * gain;
            break;
    }

    float aux_vulevel = (float) CLAMP(rap2dB(tmp), -48.0, 15.0);

    float threshold = (float) rkr->Aux_Threshold;
    Trigger->aux_vu->value(aux_vulevel);

    if (aux_vulevel < threshold)
        tmp = 0.0;

    if (tmp > 1.0)
        tmp = 1.0f;

    int amax = rkr->Aux_Maximum;
    int amin = rkr->Aux_Minimum;

    int value = amin + lrintf((float) (amax - amin) * tmp);

    if (value != rkr->last_auxvalue)
    {
        if (rkr->MIDIway)
        {
            for (int i = 0; i < 20; i++)
            {
                if (rkr->XUserMIDI[rkr->Aux_MIDI][i])
                {
                    rkr->process_midi_controller_events(rkr->XUserMIDI[rkr->Aux_MIDI][i], value);
                }
                else
                    break;
            }
        }
        else
        {
            rkr->process_midi_controller_events(rkr->Aux_MIDI, value);
        }

        rkr->last_auxvalue = value;
    }
}

int RKRGUI::Busca_Eff(int num)
{
    // Busca_Eff
    int i = 0;

    for (i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        if (rkr->efx_names[i].Pos == num)
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
            if (rkr->new_order[j] == rkr->efx_names[i].Pos)
            {
                k = 1;
            }
        }

        if (!k)
        {
            if ((rkr->efx_names[i].Type & rkr->Effect_Type_Filter) == filter)
            {
                Order->Avail_Bro->add(rkr->efx_names[i].Nom);
                rkr->availables[t] = rkr->efx_names[i].Pos;
                t++;
            }
        }
    }

    Order->Avail_Bro->select(1);
}

void RKRGUI::highlight()
{
    // highlight
    int tipo = 0;
    long long k = 0;

    Fl_Widget *w;

    if (Fl::focus() == TITTLE_L)
        Fl::focus(Open_Order);

    if (Fl::focus() == Etit)
        Fl::focus(Open_Order);

    w = Fl::belowmouse();

    if (w != NULL)
    {
        tipo = (int) w->type();
        k = (long long) w->user_data();
    }
    else
    {
        if (old != NULL)
        {
            old->color(fore_color);
            old->redraw();
        }
    }


    if ((tipo == 1) || (k == 77))
    {
        if (old != NULL)
        {
            old->color(fore_color);
            old->redraw();
        }
        w->color(fl_color_average(fore_color, fl_lighter(fore_color), .8));
        w->redraw();
        old = w;
    }

    else
    {
        if (old != NULL)
        {
            old->color(fore_color);
            old->redraw();
            old->redraw_label();
        }
    }

    if (k == 99)
    {
        Fl_Browser *b;
        b = (Fl_Browser *) w;
        int tecla = Fl::event_key();
        if (tecla != last_tecla)
        {
            last_tecla = tecla;
            for (int i = 1; i <= b->size(); i++)
            {
                if (b->text(i)[0] >= tecla - 32)
                {
                    b->select(i, 1);
                    break;
                }
            }
        }
    }
}

int RKRGUI::search_but(int x, int y)
{
    // search_but
    for (int t = 0; t < BankWindow->ob->children(); t++)
    {
        Fl_Widget *w = BankWindow->ob->child(t);

        if ((x >= w->x()) && (x <= (w->x() + w->w())) && (y >= w->y()) && (y <= (w->y() + w->h())))
        {
            long long kn = (long long) w->user_data();
            return ((int) kn);
        }
    }

    return (1000);
}

/** 
 Scan the default (DATADIR) and user directories for bank files (.rkrb).
 The user directory is set in Settings/Preferences/Bank/User Directory.
 Any directory files found are passed to the BankWindow class and listed on the
 Bank Manager "User Banks" drop down menu.
 */
void RKRGUI::Scan_Bank_Dir()
{
    ClearBankNames(); /* This will free all memory allocated for names */
    BankWindow->CH_UB->clear();
    BankWindow->clear_menu();
    
    /* Scan Default Directory for Bank files */
    Set_Bank(DATADIR);
    
    /* Scan User Directory for Bank files */
    Set_Bank(rkr->UDirFilename);
    
    BankWindow->CH_UB->value(0);
}

/**
 *  Scan a directory for Bank files and if valid, send it to the BankWindow class.
 * 
 * @param directory
 *      The directory to be scanned.
 */
void RKRGUI::Set_Bank(std::string directory)
{
    char nombank[FILENAME_MAX];
    DIR *dir;
    struct dirent *fs;
    
    dir = opendir(directory.c_str());
    if (dir == NULL)
    {
        return;
    }

    while ((fs = readdir(dir)))
    {
        if (strstr(fs->d_name, ".rkrb") != NULL)
        {
            /* Construct full bank file name with path */
            sprintf(nombank, "%s/%s", directory.c_str(), fs->d_name);
            
            AddBankName(nombank);
            
            if (rkr->CheckOldBank(nombank) == 0)
            {
                /* Get the name to be listed in the drop down "User Banks" menu */
                std::string s_nombre = fs->d_name;
                s_nombre = s_nombre.substr(0, s_nombre.size() - c_rkrb_ext_size);   // remove extension
                
                if(!s_nombre.empty())
                {
                    BankWindow->set_bank_CH_UB(&s_nombre[0], nombank);
                } 
            }
        }
    }

    closedir(dir);
}

int RKRGUI::prevnext(int e)
{
    // find various keybinding keys for processing
    if (e == 5)
    {
        Fl_Widget *w = Fl::belowmouse();
        long long k = (long long) w->user_data();
        if ((k < 770) || (k > 779))
            return 0;
        drag = k - 770;
        return 1;
    }


    if (e != 12)
    {
        return 0;
    }
    else
    {
        if ((Fl::event_key(43)) || (Fl::event_key(FL_KP + 43))) // +(plus) key
        {
            rkr->Gui_Refresh = GUI_Refresh_Plus_Key;
            return 1;
        }

        if ((Fl::event_key(45)) || (Fl::event_key(FL_KP + 45))) // -(minus) key
        {
            rkr->Gui_Refresh = GUI_Refresh_Minus_Key;
            return 1;
        }

        if (Fl::event_key(FL_F + 2)) // F2 key - decrease output volume
        {
            rkr->Gui_Refresh = GUI_Refresh_F2_Key;
            return 1;
        }

        if (Fl::event_key(FL_F + 3)) // F3 key - increase output volume
        {
            rkr->Gui_Refresh = GUI_Refresh_F3_Key;
            return 1;
        }

        if (Fl::event_key(FL_Insert)) // Insert key - add user preset
        {
            Fl_Widget *w = Fl::belowmouse();
            
            if (w == NULL)
            {
                return 0;
            }
            
            long long k = (long long) w->user_data();
            
            if ((k > 11999) && (k < 12100))
            {
                ((RKRGUI*) (w->parent()->parent()->user_data()))->addpreset(w, k - 12000);
            }
            
            return 1;
        }

        if (Fl::event_key(FL_Delete)) // Delete key - delete user preset
        {
            Fl_Widget *w = Fl::belowmouse();
            
            if (w == NULL)
            {
                return 0;
            }
            long long k = (long long) w->user_data();
            
            if ((k > 11999) && (k < 12100))
            {
                ((RKRGUI*) (w->parent()->parent()->user_data()))->delpreset(w, k - 12000);
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
        if (!rkr->File_To_Load.empty()) // individual preset
        {
            printf("Saving file: %s\n", rkr->File_To_Load.c_str());
            got_sigusr1 = 0;
            rkr->save_preset(rkr->File_To_Load.c_str());
        }
        return;
    }

    if (got_sigint == SIGINT)
    {
        printf("Got SIGTERM, quitting...\n");
        got_sigint = 0;
        rkr->Exit_Program = 1;
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
 *  if (!rkr->MIDIway)
 *      return;
 * 
 * @param num
 *  The efx parameter number that is to be used by midi learn.
 */
void RKRGUI::getMIDIControl(int num)
{
    // Don't pop up the midi learn window unless the user checked the box.
    if (!rkr->MIDIway)
        return;

    int i = 0;
    rkr->comemouse = 1;
    PrepareML();
    while (i < MIDILearn->Epar->size())
    {
        if ((rkr->ML_filter == 0) && (rkr->mc_efx_params[i].MC_params_index == num))
        {
            MIDILearn->Epar->select(i + 1);
            break;
        }

        if ((rkr->ML_filter == 1) && (rkr->ML_clist[i] == num))
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
    if (rkr->comemouse == 0)
    {
        MIDILearn->show();
        put_icon(MIDILearn);
        return;
    }

    if (rkr->comemouse)
    {
        if (rkr->autoassign == 0)
        {
            MIDILearn->show();
            put_icon(MIDILearn);
            return;
        }
    }
}

inline void RKRGUI::addpreset(Fl_Widget *w, int num)
{
    // Add preset
    if (num == 12)
        return;

    const char *name = fl_input("Preset Name?", "");
    if (name == NULL)
        return;

    char NewName[64];
    memset(NewName, 0, sizeof (NewName));
    sprintf(NewName, "*%s", name);
    add_name(w, NewName);
    rkr->save_insert_preset(num, NewName);
}

void RKRGUI::add_name(Fl_Widget *w, char *name)
{
    // add name
    Fl_Choice *s = (Fl_Choice *) w;
    s->add(name);

    Fl_Menu_*n = (Fl_Menu_*) s->menu();
    Fl_Menu_Item *m = (Fl_Menu_Item*) n;
    Fl_Menu_Item *p;
    
    int k = 10; // default label size

    for (int i = 0; i < m->size(); i++)
    {
        p = m->next(i);
        
        if (i == 0)
        {
            k = p->labelsize();
        }
        
        p->labelsize(k);
    }
}

Fl_Widget * RKRGUI::FindWidget(int num)
{
    // FindWidget
    for (int t = 0; t < Principal->children(); t++)
    {
        Fl_Widget *w = Principal->child(t);
        long long ud = (long long) w->user_data();

        if (ud == 1)
        {
            Fl_Group *g = (Fl_Group *) w;

            for (int i = 0; i < g->children(); i++)
            {
                Fl_Widget *c = g->child(i);
                long long uh = (long long) c->user_data();
                if (uh == (num + 12000))
                    return c;
            }
        }
    }

    return NULL;
}

void RKRGUI::ReadIntPresets()
{
    // Read in user presets
    FILE *fn;
    char tempfile[256];
    char buf[256];
    char *name;
    char *sbuf;
    int num = 0;
    memset(tempfile, 0, sizeof (tempfile));

    sprintf(tempfile, "%s%s", getenv("HOME"), "/.rkrintpreset");

    if ((fn = fopen(tempfile, "r")) != NULL)
    {
        while (fgets(buf, sizeof buf, fn) != NULL)
        {
            sbuf = buf;
            sscanf(buf, "%d", &num);
            name = strsep(&sbuf, ",");
            name = strsep(&sbuf, ",");
            add_name(FindWidget(num), name);
        }

        fclose(fn);
    }
}

inline void RKRGUI::delpreset(Fl_Widget *w, int num)
{
    // delete user preset
    if (num == 12)
        return;

    char Rname[128];
    Fl_Choice *s = (Fl_Choice *) w;

    if (strncmp(s->text(), "*", 1) != 0)
    {
        fl_message("Internal Presets can not be deleted ");
        return;
    }

    int ok = fl_choice("Delete \"%s\"?", "No", "Yes", NULL, s->text());

    if (!ok)
    {
        return;
    }

    memset(Rname, 0, sizeof (Rname));
    sprintf(Rname, "%s", s->text());
    s->remove(s->value());
    s->value(0);
    s->redraw();
    rkr->delete_insert_preset(num, Rname);
}

void RKRGUI::Prep_Reorden(int source, int dest)
{
    // Prepare Reorden
    int x = rkr->efx_order[source];
    int y = rkr->efx_order[dest];

    rkr->efx_order[source] = y;
    rkr->efx_order[dest] = x;

    reordena();

    if (Order->visible())
        Prepare_Order();
}

void RKRGUI::bank_click(Fl_Choice* o, void* v)
{
    // Bank click
    ((RKRGUI*) (o->parent()->parent()->parent()->parent()->user_data()))->bank_click_i(o, v);
}

inline void RKRGUI::bank_click_i(Fl_Choice* o, void*)
{
    // bank_click_i
    long long kk = (long long) o->user_data();
    int num = (int) kk;

    rkr->M_table[num - c_bank_used].bank = (int) o->value();
    Settings->fill_mptable(num + c_bank_used, rkr->M_table[num - c_bank_used].bank);
}

void RKRGUI::p_click(Fl_Choice* o, void* v)
{
    // p_click
    ((RKRGUI*) (o->parent()->parent()->parent()->parent()->user_data()))->p_click_i(o, v);
}

inline void RKRGUI::p_click_i(Fl_Choice* o, void*)
{
    // p_click_i
    long long kk = (long long) o->user_data();
    int num = (int) kk;

    rkr->M_table[num - c_preset_used].preset = o->value();
}

void RKRGUI::RandomPreset()
{
    // Random select button
    int SelEff[10];
    int numEff = (int) (RND * 6) + 1;
 
    S_new->do_callback();

    SelEff[0] = (int) (RND * C_NUMBER_EFFECTS);

    for (int i = 1; i < 10; i++)
    {
        int l = 0;
        while (l == 0)
        {
            SelEff[i] = (int) (RND * C_NUMBER_EFFECTS);
            for (int j = 0; j < i; j++)
            {
                if (SelEff[j] == SelEff[i])
                {
                    l = 0;
                    break;
                }
                else
                    l = 1;
            }
        }
    }

    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        rkr->efx_order[i] = SelEff[i];
    }

    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        int rack_effect = rkr->efx_order[i];

        if (i < numEff)
        {
            rkr->EFX_Bypass[rack_effect] = 1;
        }
        else
        {
            rkr->EFX_Bypass[rack_effect] = 0;
        }
        
        Efx_Gui_Base[rack_effect]->activate_effect->value (rkr->EFX_Bypass[rack_effect]);

        Fl_Widget *w = FindWidget(SelEff[i]);
        Fl_Choice *s = (Fl_Choice *) w;
        long long k = (long long) s->user_data();
        int Esel = (int) (RND * s->size());
        s->value(Esel);
        s->do_callback(w, k);
    }

    FillML();
    Prepare_Order();
    Put_Loaded();

    ActivarGeneral->value(1);
    ActivarGeneral->do_callback();
}

void RKRGUI::drag_effect()
{
    // drag effect
    if ((drag != 1000) && (Fl::event_button1() == 0))
    {
        Fl_Widget *w = Fl::belowmouse();
        if (w != NULL)
        {
            long long k = (long long) w->user_data();

            if ((k < 770) || (k > 779))
            {
                return;
            }

            // w->user_data() is Label in range 770 to 779
            // which is checked above, so this is valid range (10) for
            // rkr->efx_order[C_NUMBER_ORDERED_EFFECTS] used by Prep_Reorden(source, dest)
            if (drag != (int) k - 770)
            {
                Prep_Reorden(drag, (int) k - 770);
            }
        }
        drag = 1000;
    }
}

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
    int ok = rkr->load_bank(filename);

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
    filename = fl_file_chooser("Save Bank File:", "(*" EXT")", rkr->Bank_Saved, 0);

    if (filename == NULL)
        return;

    filename = fl_filename_setext(filename, EXT);
#undef EXT
    int ok = rkr->save_bank(filename);
    if (ok)
    {
        strcpy(rkr->Bank_Saved, filename);
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
    rkr->load_MIDI_table(filename);
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
    rkr->save_MIDI_table(filename);
}
