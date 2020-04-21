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
    if (filetoload != NULL)
    {
        sprintf(tmp, "Session: %s", filetoload);
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
    
    load_stat();
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

    if (stecla == 1)
    {
        if (rkr->Selected_Preset < 60)
        {
            Preset_Counter->value(rkr->Selected_Preset + 1);
            Preset_Counter->do_callback();
        }

        stecla = 0;
    }


    if (stecla == 2)
    {
        if (rkr->Selected_Preset > 1)
        {
            Preset_Counter->value(rkr->Selected_Preset - 1);
            Preset_Counter->do_callback();
        }

        stecla = 0;
    }

    if (stecla == 3)
    {
        if (Nivel_Salida->value()>-50)
        {
            Nivel_Salida->value(Nivel_Salida->value() - 1);
            Nivel_Salida->do_callback();
            Nivel_Salida->redraw();
        }

        stecla = 0;
    }


    if (stecla == 4)
    {
        if (Nivel_Salida->value() < 50)
        {
            Nivel_Salida->value(Nivel_Salida->value() + 1);
            Nivel_Salida->do_callback();
            Nivel_Salida->redraw();
        }

        stecla = 0;
    }


    if (stecla == 5)
    {
        update_looper();
        stecla = 0;
    }


    if (preset != 1000)
    {
        if (!rkr->midi_table)
        {
            Preset_Counter->value(preset);
            Preset_Counter->do_callback();
        }
        else
        {
            if (rkr->a_bank != rkr->M_table[preset].bank)
            {
                switch (rkr->M_table[preset].bank)
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
            Preset_Counter->value(rkr->M_table[preset].preset + 1);
            Preset_Counter->do_callback();
        }

        preset = 1000;
    }


    ActMIDI();
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


    if (rkr->numpc)
    {
        if (rkr->numpi)
            P_IN_ST->show();
        else
            P_IN_ST->hide();
        if (rkr->numpo)
            P_OUT_ST->show();
        else
            P_OUT_ST->hide();
        if (rkr->numpa)
            P_AUX_ST->show();
        else
            P_AUX_ST->hide();
        if (rkr->numpmi)
            P_MIN_ST->show();
        else
            P_MIN_ST->hide();
        if (rkr->numpmo)
            P_MOUT_ST->show();
        else
            P_MOUT_ST->hide();
        rkr->numpc = 0;
    }

    if (error_num > 0)
        rkr->Error_Handle(error_num);

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
            CLIP_LED->color(fl_darker(FL_RED));
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
            if (VOCODER->vu_vu->value() != rkr->efx_Vocoder->vulevel)
                VOCODER->vu_vu->value(rkr->efx_Vocoder->vulevel);
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

        if (rkr->Looper_Bypass)
        {
            if ((rkr->efx_Looper->Pplay) && (!rkr->efx_Looper->Pstop))
            {
                if (looper_lqua != rkr->efx_Looper->looper_qua)
                {
                    looper_lqua = rkr->efx_Looper->looper_qua;
                    char tmp[16];
                    memset(tmp, 0, sizeof (tmp));
                    sprintf(tmp, "%d/%d", rkr->efx_Looper->looper_bar, rkr->efx_Looper->looper_qua);
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


    if (rkr->Harmonizer_Bypass)
    {
        if ((rkr->efx_Har->PSELECT) || (rkr->efx_Har->PMIDI))
        {
            if (rkr->RC_Harm->cc)
            {
                HAR->har_chordname->copy_label(rkr->RC_Harm->NombreAcorde);
                rkr->RC_Harm->cc = 0;
                HAR->har_chordname->redraw();
                rkr->RC_Harm->Vamos(0, rkr->efx_Har->Pinterval - 12, rkr->HarmRecNote->reconota);
                rkr->efx_Har->r_ratio = rkr->RC_Harm->r__ratio[0]; //pass the found ratio
            }
        }
    }


    if (rkr->StereoHarm_Bypass)
    {
        if ((rkr->efx_StereoHarm->PSELECT) || (rkr->efx_StereoHarm->PMIDI))
        {
            if (rkr->RC_Stereo_Harm->cc)
            {
                SHAR->shar_chordname->copy_label(rkr->RC_Stereo_Harm->NombreAcorde);
                rkr->RC_Stereo_Harm->cc = 0;
                SHAR->shar_chordname->redraw();
                rkr->RC_Stereo_Harm->Vamos(1, rkr->efx_StereoHarm->Pintervall - 12, rkr->StHarmRecNote->reconota);
                rkr->RC_Stereo_Harm->Vamos(2, rkr->efx_StereoHarm->Pintervalr - 12, rkr->StHarmRecNote->reconota);
                rkr->efx_StereoHarm->r_ratiol = rkr->RC_Stereo_Harm->r__ratio[1];
                rkr->efx_StereoHarm->r_ratior = rkr->RC_Stereo_Harm->r__ratio[2];
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

void RKRGUI::load_stat()
{
    // load user preferences and last fltk state from ~user/.fltk/github.com.Stazed.rakarrack.plus/rakarrack-plus.prefs
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

    if (!needtoloadbank)
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

    if (!commandline)
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
    rkr->efx_Looper->setmvol(rkr->Metro_Vol);

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
        if (!needtoloadstate)
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


    rakarrack.get(rkr->PrefNom("Disable Warnings"), rkr->mess_dis, 0);
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

void RKRGUI::save_stat(int whati)
{
    // save user preferences and last fltk state in ~/user/.fltk/github.com.Stazed.rakarrack.plus/rakarrack-plus.prefs

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

        rakarrack.set(rkr->PrefNom("Waveshape Resampling"), rkr->Wave_res_amount);
        rakarrack.set(rkr->PrefNom("Waveshape Up Quality"), rkr->Wave_up_q);
        rakarrack.set(rkr->PrefNom("Waveshape Down Quality"), rkr->Wave_down_q);
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
        rakarrack.set(rkr->PrefNom("Tap Tempo Timeout"), rkr->t_timeout);
        rakarrack.set(rkr->PrefNom("Disable Warnings"), rkr->mess_dis);
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

    for (int i = 0; i < rkr->NumEffects; i++)
    {
        switch (i)
        {

            case 0: //EQ1
                EQ->eq_activar->value(rkr->EQ1_Bypass);
                EQ->eq_preset->do_callback(EQ->eq_preset, 1);
                break;

            case 1://Compressor
                COMPRESS->compress_activar->value(rkr->Compressor_Bypass);
                COMPRESS->compress_preset->do_callback(COMPRESS->compress_preset, 1);
                break;

            case 2://Distorsion
                DIST->dist_activar->value(rkr->Distorsion_Bypass);
                DIST->dist_preset->do_callback(DIST->dist_preset, 1);
                break;

            case 3://Overdrive
                OVRD->ovrd_activar->value(rkr->Overdrive_Bypass);
                OVRD->ovrd_preset->do_callback(OVRD->ovrd_preset, 1);
                break;

            case 4://Echo
                ECHO->echo_activar->value(rkr->Echo_Bypass);
                ECHO->echo_preset->do_callback(ECHO->echo_preset, 1);
                break;

            case 5://Chorus
                CHORUS->chorus_activar->value(rkr->Chorus_Bypass);
                CHORUS->chorus_preset->do_callback(CHORUS->chorus_preset, 1);
                break;

            case 6://Phaser
                PHASER->phaser_activar->value(rkr->Phaser_Bypass);
                PHASER->phaser_preset->do_callback(PHASER->phaser_preset, 1);
                break;

            case 7://Flanger
                FLANGER->flanger_activar->value(rkr->Flanger_Bypass);
                FLANGER->flanger_preset->do_callback(FLANGER->flanger_preset, 1);
                break;

            case 8://Reverb
                REVERB->reverb_activar->value(rkr->Reverb_Bypass);
                REVERB->reverb_preset->do_callback(REVERB->reverb_preset, 1);
                break;

            case 9://EQ2
                PEQ->eqp_activar->value(rkr->EQ2_Bypass);
                PEQ->eqp_preset->do_callback(PEQ->eqp_preset, 1);
                break;

            case 10://WahWah
                WHAWHA->WhaWha_activar->value(rkr->WhaWha_Bypass);
                WHAWHA->WhaWha_preset->do_callback(WHAWHA->WhaWha_preset, 1);
                break;

            case 11://AlienWah
                ALIENWAH->Alienwah_activar->value(rkr->Alienwah_Bypass);
                ALIENWAH->Alienwah_preset->do_callback(ALIENWAH->Alienwah_preset, 1);
                break;

            case 12://Cabinet
                CABINET->Cabinet_output->value(rkr->lv[13][1] - 64);
                CABINET->Cabinet_preset->value(rkr->lv[13][0]);
                CABINET->Cabinet_activar->value(rkr->Cabinet_Bypass);
                break;

            case 13://Pan
                PAN->pan_activar->value(rkr->Pan_Bypass);
                PAN->pan_preset->do_callback(PAN->pan_preset, 1);
                break;

            case 14://Har
                HAR->har_activar->value(rkr->Harmonizer_Bypass);
                HAR->har_preset->do_callback(HAR->har_preset, 1);
                break;

            case 15://MsicalDelay
                MUSDELAY->musdelay_activar->value(rkr->MusDelay_Bypass);
                MUSDELAY->musdelay_preset->do_callback(MUSDELAY->musdelay_preset, 1);
                break;

            case 16://NoiseGate
                GATE->gate_activar->value(rkr->Gate_Bypass);
                GATE->gate_preset->do_callback(GATE->gate_preset, 1);
                break;

            case 17://Derelict
                DERELICT->derelict_activar->value(rkr->Derelict_Bypass);
                DERELICT->derelict_preset->do_callback(DERELICT->derelict_preset, 1);
                break;

            case 18://APhaser
                APHASER->aphaser_activar->value(rkr->APhaser_Bypass);
                APHASER->aphaser_preset->do_callback(APHASER->aphaser_preset, 1);
                break;

            case 19://Valve
                VALVE->valve_activar->value(rkr->Valve_Bypass);
                VALVE->valve_preset->do_callback(VALVE->valve_preset, 1);
                break;

            case 20://Dual_Flange
                DFLANGE->dflange_activar->value(rkr->DFlange_Bypass);
                DFLANGE->dflange_preset->do_callback(DFLANGE->dflange_preset, 1);
                break;

            case 21:// Ring
                RING->ring_activar->value(rkr->Ring_Bypass);
                RING->ring_preset->do_callback(RING->ring_preset, 1);
                break;

            case 22:// Exciter
                EXCITER->exciter_activar->value(rkr->Exciter_Bypass);
                EXCITER->exciter_preset->do_callback(EXCITER->exciter_preset, 1);
                break;

            case 23://DistBand
                DISTBAND->distband_activar->value(rkr->DistBand_Bypass);
                DISTBAND->distband_preset->do_callback(DISTBAND->distband_preset, 1);
                break;

            case 24://Arpie
                ARPIE->arpie_activar->value(rkr->Arpie_Bypass);
                ARPIE->arpie_preset->do_callback(ARPIE->arpie_preset, 1);
                break;

            case 25://Expander
                EXPANDER->expander_activar->value(rkr->Expander_Bypass);
                EXPANDER->expander_preset->do_callback(EXPANDER->expander_preset, 1);
                break;

            case 26://Shuffle
                SHUFFLE->shuffle_activar->value(rkr->Shuffle_Bypass);
                SHUFFLE->shuffle_preset->do_callback(SHUFFLE->shuffle_preset, 1);
                break;

            case 27://Synthfilter
                SYNTHFILTER->synthfilter_activar->value(rkr->Synthfilter_Bypass);
                SYNTHFILTER->synthfilter_preset->do_callback(SYNTHFILTER->synthfilter_preset, 1);
                break;

            case 28://VaryBand
                VARYBAND->varyband_activar->value(rkr->VaryBand_Bypass);
                VARYBAND->varyband_preset->do_callback(VARYBAND->varyband_preset, 1);
                break;

            case 29://Convolotron
                CONVOLOTRON->convo_activar->value(rkr->Convol_Bypass);
                CONVOLOTRON->convo_preset->do_callback(CONVOLOTRON->convo_preset, 1);
                break;

            case 30://Looper
                LOOPER->looper_activar->value(rkr->Looper_Bypass);
                LOOPER->looper_preset->do_callback(LOOPER->looper_preset, 1);
                update_looper();
                break;

            case 31://MuTroMojo
                MUTROMOJO->mutromojo_activar->value(rkr->MuTroMojo_Bypass);
                MUTROMOJO->mutromojo_preset->do_callback(MUTROMOJO->mutromojo_preset, 1);
                break;

            case 32:// Echoverse
                ECHOVERSE->echoverse_activar->value(rkr->Echoverse_Bypass);
                ECHOVERSE->echoverse_preset->do_callback(ECHOVERSE->echoverse_preset, 1);
                break;

            case 33:// CoilCrafter
                COILCRAFTER->coil_activar->value(rkr->CoilCrafter_Bypass);
                COILCRAFTER->coil_preset->do_callback(COILCRAFTER->coil_preset, 1);
                break;

            case 34://ShelfBoost
                SHELFBOOST->shelf_activar->value(rkr->ShelfBoost_Bypass);
                SHELFBOOST->shelf_preset->do_callback(SHELFBOOST->shelf_preset, 1);
                break;

            case 35://Vocoder
                VOCODER->vo_activar->value(rkr->Vocoder_Bypass);
                VOCODER->vo_preset->do_callback(VOCODER->vo_preset, 1);
                break;

            case 36://Sustainer
                SUSTAINER->sus_activar->value(rkr->Sustainer_Bypass);
                SUSTAINER->sus_preset->do_callback(SUSTAINER->sus_preset, 1);
                break;

            case 37://Sequence
                SEQUENCE->seq_activar->value(rkr->Sequence_Bypass);
                SEQUENCE->seq_preset->do_callback(SEQUENCE->seq_preset, 1);
                break;

            case 38://Shifter
                SHIFTER->shifter_activar->value(rkr->Shifter_Bypass);
                SHIFTER->shifter_preset->do_callback(SHIFTER->shifter_preset, 1);
                break;

            case 39://StompBox
                STOMPBOX->stomp_activar->value(rkr->StompBox_Bypass);
                STOMPBOX->stomp_preset->do_callback(STOMPBOX->stomp_preset, 1);
                break;

            case 40://Reverbtron
                REVERBTRON->revtron_activar->value(rkr->Reverbtron_Bypass);
                REVERBTRON->revtron_preset->do_callback(REVERBTRON->revtron_preset, 1);
                break;

            case 41://Echotron
                ECHOTRON->echotron_activar->value(rkr->Echotron_Bypass);
                ECHOTRON->echotron_preset->do_callback(ECHOTRON->echotron_preset, 1);
                break;

            case 42://StereoHarm
                SHAR->shar_activar->value(rkr->StereoHarm_Bypass);
                SHAR->shar_preset->do_callback(SHAR->shar_preset, 1);
                break;

            case 43://CompBand
                COMPBAND->cband_activar->value(rkr->CompBand_Bypass);
                COMPBAND->cband_preset->do_callback(COMPBAND->cband_preset, 1);
                break;

            case 44://Opticaltrem
                OTREM->otrem_activar->value(rkr->Opticaltrem_Bypass);
                OTREM->otrem_preset->do_callback(OTREM->otrem_preset, 1);
                break;

            case 45://Vibe
                VIBE->vibe_activar->value(rkr->Vibe_Bypass);
                VIBE->vibe_preset->do_callback(VIBE->vibe_preset, 1);
                break;

            case 46://Infinity
                INFINIT->infinity_activar->value(rkr->Infinity_Bypass);
                INFINIT->infinity_preset->do_callback(INFINIT->infinity_preset, 1);
                break;

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

        if (strlen(rkr->Bank[num].Preset_Name) > 0)
        {
            Fl_Widget *m = fl_message_icon();
            m->parent()->copy_label(rkr->jackcliname);
            int ok = fl_choice("Overwrite \"%s\"?", "No", "Yes", NULL, w->label());
            if (!ok)
            {
                o->value(0);
                o->redraw();
                return;
            }
        }

        o->value(0);
        o->redraw();
        rkr->Preset_to_Bank(num);
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


    //Hide All
    EQ->hide();
    COMPRESS->hide();
    DIST->hide();
    OVRD->hide();
    ECHO->hide();
    CHORUS->hide();
    PHASER->hide();
    FLANGER->hide();
    REVERB->hide();
    PEQ->hide();
    WHAWHA->hide();
    ALIENWAH->hide();
    CABINET->hide();
    PAN->hide();
    HAR->hide();
    MUSDELAY->hide();
    GATE->hide();
    DERELICT->hide();
    APHASER->hide();
    VALVE->hide();
    DFLANGE->hide();
    RING->hide();
    EXCITER->hide();
    DISTBAND->hide();
    ARPIE->hide();
    EXPANDER->hide();
    SHUFFLE->hide();
    SYNTHFILTER->hide();
    VARYBAND->hide();
    CONVOLOTRON->hide();
    LOOPER->hide();
    MUTROMOJO->hide();
    ECHOVERSE->hide();
    COILCRAFTER->hide();
    SHELFBOOST->hide();
    VOCODER->hide();
    SUSTAINER->hide();
    SEQUENCE->hide();
    SHIFTER->hide();
    STOMPBOX->hide();
    REVERBTRON->hide();
    ECHOTRON->hide();
    SHAR->hide();
    COMPBAND->hide();
    OTREM->hide();
    VIBE->hide();
    INFINIT->hide();


    // Show

    for (int i = 0; i < 10; i++)
    {
        switch (rkr->efx_order[i])
        {

            case 0:

                EQ->position(x[i], y[i]);
                EQ->eq_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    EQ->show();
                if (rkr->EQ1_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        EQ->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 1:

                COMPRESS->position(x[i], y[i]);
                COMPRESS->compress_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    COMPRESS->show();
                if (rkr->Compressor_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        COMPRESS->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 2:

                DIST->position(x[i], y[i]);
                DIST->dist_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    DIST->show();
                if (rkr->Distorsion_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        DIST->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 3:

                OVRD->position(x[i], y[i]);
                OVRD->ovrd_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    OVRD->show();
                if (rkr->Overdrive_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        OVRD->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 4:

                ECHO->position(x[i], y[i]);
                ECHO->echo_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    ECHO->show();
                if (rkr->Echo_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        ECHO->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 5:

                CHORUS->position(x[i], y[i]);
                CHORUS->chorus_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    CHORUS->show();
                if (rkr->Chorus_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        CHORUS->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 6:

                PHASER->position(x[i], y[i]);
                PHASER->phaser_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    PHASER->show();
                if (rkr->Phaser_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        PHASER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 7:

                FLANGER->position(x[i], y[i]);
                FLANGER->flanger_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    FLANGER->show();
                if (rkr->Flanger_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        FLANGER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 8:

                REVERB->position(x[i], y[i]);
                REVERB->reverb_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    REVERB->show();
                if (rkr->Reverb_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        REVERB->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 9:

                PEQ->position(x[i], y[i]);
                PEQ->eqp_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    PEQ->show();
                if (rkr->EQ2_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        PEQ->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 10:

                WHAWHA->position(x[i], y[i]);
                WHAWHA->WhaWha_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    WHAWHA->show();
                if (rkr->WhaWha_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        WHAWHA->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 11:

                ALIENWAH->position(x[i], y[i]);
                ALIENWAH->Alienwah_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    ALIENWAH->show();
                if (rkr->Alienwah_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        ALIENWAH->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 12:

                CABINET->position(x[i], y[i]);
                CABINET->Cabinet_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    CABINET->show();
                if (rkr->Cabinet_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        CABINET->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 13:

                PAN->position(x[i], y[i]);
                PAN->pan_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    PAN->show();
                if (rkr->Pan_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        PAN->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 14:
                HAR->position(x[i], y[i]);
                HAR->har_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    HAR->show();
                if (rkr->Harmonizer_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        HAR->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 15:
                MUSDELAY->position(x[i], y[i]);
                MUSDELAY->musdelay_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    MUSDELAY->show();
                if (rkr->MusDelay_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        MUSDELAY->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 16:
                GATE->position(x[i], y[i]);
                GATE->gate_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    GATE->show();
                if (rkr->Gate_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        GATE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 17:
                DERELICT->position(x[i], y[i]);
                DERELICT->derelict_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    DERELICT->show();
                if (rkr->Derelict_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        DERELICT->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 18:
                APHASER->position(x[i], y[i]);
                APHASER->aphaser_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    APHASER->show();
                if (rkr->APhaser_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        APHASER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 19:
                VALVE->position(x[i], y[i]);
                VALVE->valve_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    VALVE->show();
                if (rkr->Valve_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        VALVE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 20:
                DFLANGE->position(x[i], y[i]);
                DFLANGE->dflange_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    DFLANGE->show();
                if (rkr->DFlange_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        DFLANGE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 21:
                RING->position(x[i], y[i]);
                RING->ring_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    RING->show();
                if (rkr->Ring_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        RING->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 22:
                EXCITER->position(x[i], y[i]);
                EXCITER->exciter_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    EXCITER->show();
                if (rkr->Exciter_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        EXCITER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 23:
                DISTBAND->position(x[i], y[i]);
                DISTBAND->distband_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    DISTBAND->show();
                if (rkr->DistBand_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        DISTBAND->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 24:
                ARPIE->position(x[i], y[i]);
                ARPIE->arpie_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    ARPIE->show();
                if (rkr->Arpie_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        ARPIE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 25:
                EXPANDER->position(x[i], y[i]);
                EXPANDER->expander_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    EXPANDER->show();
                if (rkr->Expander_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        EXPANDER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 26:
                SHUFFLE->position(x[i], y[i]);
                SHUFFLE->shuffle_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SHUFFLE->show();
                if (rkr->Shuffle_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SHUFFLE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 27:
                SYNTHFILTER->position(x[i], y[i]);
                SYNTHFILTER->synthfilter_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SYNTHFILTER->show();
                if (rkr->Synthfilter_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SYNTHFILTER->show();
                }
                else
                    rkr->active[i] = 0;

                break;

            case 28:
                VARYBAND->position(x[i], y[i]);
                VARYBAND->varyband_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    VARYBAND->show();
                if (rkr->VaryBand_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        VARYBAND->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 29:
                CONVOLOTRON->position(x[i], y[i]);
                CONVOLOTRON->convo_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    CONVOLOTRON->show();
                if (rkr->Convol_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        CONVOLOTRON->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 30:
                LOOPER->position(x[i], y[i]);
                LOOPER->looper_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    LOOPER->show();
                if (rkr->Looper_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        LOOPER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 31:
                MUTROMOJO->position(x[i], y[i]);
                MUTROMOJO->mutromojo_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    MUTROMOJO->show();
                if (rkr->MuTroMojo_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        MUTROMOJO->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 32:
                ECHOVERSE->position(x[i], y[i]);
                ECHOVERSE->echoverse_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    ECHOVERSE->show();
                if (rkr->Echoverse_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        ECHOVERSE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 33:
                COILCRAFTER->position(x[i], y[i]);
                COILCRAFTER->coil_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    COILCRAFTER->show();
                if (rkr->CoilCrafter_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        COILCRAFTER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 34:
                SHELFBOOST->position(x[i], y[i]);
                SHELFBOOST->shelf_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SHELFBOOST->show();
                if (rkr->ShelfBoost_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SHELFBOOST->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 35:
                VOCODER->position(x[i], y[i]);
                VOCODER->vo_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    VOCODER->show();
                if (rkr->Vocoder_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        VOCODER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 36:
                SUSTAINER->position(x[i], y[i]);
                SUSTAINER->sus_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SUSTAINER->show();
                if (rkr->Sustainer_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SUSTAINER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 37:
                SEQUENCE->position(x[i], y[i]);
                SEQUENCE->seq_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SEQUENCE->show();
                if (rkr->Sequence_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SEQUENCE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 38:
                SHIFTER->position(x[i], y[i]);
                SHIFTER->shifter_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SHIFTER->show();
                if (rkr->Shifter_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SHIFTER->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 39:
                STOMPBOX->position(x[i], y[i]);
                STOMPBOX->stomp_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    STOMPBOX->show();
                if (rkr->StompBox_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        STOMPBOX->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 40:
                REVERBTRON->position(x[i], y[i]);
                REVERBTRON->revtron_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    REVERBTRON->show();
                if (rkr->Reverbtron_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        REVERBTRON->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 41:
                ECHOTRON->position(x[i], y[i]);
                ECHOTRON->echotron_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    ECHOTRON->show();
                if (rkr->Echotron_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        ECHOTRON->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 42:
                SHAR->position(x[i], y[i]);
                SHAR->shar_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    SHAR->show();
                if (rkr->StereoHarm_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        SHAR->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 43:
                COMPBAND->position(x[i], y[i]);
                COMPBAND->cband_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    COMPBAND->show();
                if (rkr->CompBand_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        COMPBAND->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 44:
                OTREM->position(x[i], y[i]);
                OTREM->otrem_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    OTREM->show();
                if (rkr->Opticaltrem_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        OTREM->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 45:
                VIBE->position(x[i], y[i]);
                VIBE->vibe_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    VIBE->show();
                if (rkr->Vibe_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        VIBE->show();
                }
                else
                    rkr->active[i] = 0;
                break;

            case 46:
                INFINIT->position(x[i], y[i]);
                INFINIT->infinity_activar->shortcut(s[i]);
                if (!rkr->deachide)
                    INFINIT->show();
                if (rkr->Infinity_Bypass)
                {
                    rkr->active[i] = 1;
                    if (rkr->deachide)
                        INFINIT->show();
                }
                else
                    rkr->active[i] = 0;
                break;

        }


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
    Settings->Wave_Amo->value(rkr->Wave_res_amount);
    Settings->Wave_Up_Qua->value(rkr->Wave_up_q);
    Settings->Wave_Down_Qua->value(rkr->Wave_down_q);
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
    Settings->MES_DIS->value(rkr->mess_dis);
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

void RKRGUI::BankWin_Label(char *filename)
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
            tipo = rkr->efx_Har->getpar(7);
            undi = rkr->efx_Har->getpar(6);
            break;
        case 1:
            tipo = rkr->efx_StereoHarm->getpar(9);
            undi = rkr->efx_StereoHarm->getpar(8);
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

void RKRGUI::ActMIDI()
{
    // adjust gui parameters from midi incoming action

    if (rkr->Mvalue == 0)
        return;

    for (int i = 1; i < (rkr->NumParams + 26); i++)
    {
        if (rkr->Mcontrol[i] == 0)
        {
            continue;
        }

        rkr->Mcontrol[i] = 0;

        switch (i)
        {
            case 12:
                Balance->value(rkr->Fraction_Bypass * 100.0);
                Balance->redraw();
                break;
            case 14:
                Nivel_Entrada->value((int) (rkr->Input_Gain * 100.0) - 50);
                Nivel_Entrada->redraw();
                break;
            case 7:
                Nivel_Salida->value((int) (rkr->Master_Volume * 100.0) - 50);
                Nivel_Salida->redraw();
                break;
            case 1:
                WHAWHA->WhaWha_dpth->value(rkr->efx_WhaWha->getpar(6));
                WHAWHA->WhaWha_dpth->redraw();
                break;
            case 20:
                ALIENWAH->Alienwah_dpth->value(rkr->efx_Alienwah->getpar(6));
                ALIENWAH->Alienwah_dpth->redraw();
                break;
            case 21:
                PHASER->phaser_dpth->value(rkr->efx_Phaser->getpar(6));
                PHASER->phaser_dpth->redraw();
                break;
            case 22:
                FLANGER->flanger_dpth->value(rkr->efx_Flanger->getpar(6));
                FLANGER->flanger_dpth->redraw();
                break;
            case 23:
                CHORUS->chorus_dpth->value(rkr->efx_Chorus->getpar(6));
                CHORUS->chorus_dpth->redraw();
                break;
            case 24:
                MUSDELAY->musdelay_gain1->value(rkr->efx_MusDelay->getpar(11));
                MUSDELAY->musdelay_gain1->redraw();
                break;
            case 25:
                MUSDELAY->musdelay_gain2->value(rkr->efx_MusDelay->getpar(12));
                MUSDELAY->musdelay_gain2->redraw();
                break;
            case 26:
                HAR->har_freq1->value(rkr->efx_Har->getpar(4));
                HAR->har_freq1->redraw();
                break;
            case 27:
                HAR->har_int->value(rkr->efx_Har->getpar(3) - 12);
                HAR->har_int->redraw();
                break;
            case 28:
                WHAWHA->WhaWha_WD->value(Dry_Wet(rkr->efx_WhaWha->getpar(0)));
                WHAWHA->WhaWha_WD->redraw();
                break;
            case 29:
                OVRD->ovrd_WD->value(Dry_Wet(rkr->efx_Overdrive->getpar(0)));
                OVRD->ovrd_WD->redraw();
                break;
            case 30:
                DIST->dist_WD->value(Dry_Wet(rkr->efx_Distorsion->getpar(0)));
                DIST->dist_WD->redraw();
                break;
            case 31:
                HAR->har_WD->value(Dry_Wet(rkr->efx_Har->getpar(0)));
                HAR->har_WD->redraw();
                break;
            case 52:
                CHORUS->chorus_WD->value(Dry_Wet(rkr->efx_Chorus->getpar(0)));
                CHORUS->chorus_WD->redraw();
                break;
            case 53:
                FLANGER->flanger_WD->value(Dry_Wet(rkr->efx_Flanger->getpar(0)));
                FLANGER->flanger_WD->redraw();
                break;
            case 54:
                PHASER->phaser_WD->value(Dry_Wet(rkr->efx_Phaser->getpar(0)));
                PHASER->phaser_WD->redraw();
                break;
            case 55:
                ALIENWAH->Alienwah_WD->value(Dry_Wet(rkr->efx_Alienwah->getpar(0)));
                ALIENWAH->Alienwah_WD->redraw();
                break;
            case 56:
                MUSDELAY->musdelay_WD->value(Dry_Wet(rkr->efx_MusDelay->getpar(0)));
                MUSDELAY->musdelay_WD->redraw();
                break;
            case 57:
                REVERB->reverb_WD->value(Dry_Wet(rkr->efx_Rev->getpar(0)));
                REVERB->reverb_WD->redraw();
                break;
            case 58:
                PAN->pan_WD->value(Dry_Wet(rkr->efx_Pan->getpar(0)));
                PAN->pan_WD->redraw();
                break;
            case 59:
                ECHO->echo_WD->value(Dry_Wet(rkr->efx_Echo->getpar(0)));
                ECHO->echo_WD->redraw();
                break;
            case 46:
                ECHO->echo_pan->value(rkr->efx_Echo->getpar(1) - 64);
                ECHO->echo_pan->redraw();
                break;
            case 47:
                OVRD->ovrd_pan->value(rkr->efx_Overdrive->getpar(1) - 64);
                OVRD->ovrd_pan->redraw();
                break;
            case 48:
                DIST->dist_pan->value(rkr->efx_Distorsion->getpar(1) - 64);
                DIST->dist_pan->redraw();
                break;
            case 49:
                HAR->har_pan->value(rkr->efx_Har->getpar(1) - 64);
                HAR->har_pan->redraw();
                break;
            case 50:
                CHORUS->chorus_pan->value(rkr->efx_Chorus->getpar(1) - 64);
                CHORUS->chorus_pan->redraw();
                break;
            case 51:
                FLANGER->flanger_pan->value(rkr->efx_Flanger->getpar(1) - 64);
                FLANGER->flanger_pan->redraw();
                break;
            case 60:
                PHASER->phaser_pan->value(rkr->efx_Phaser->getpar(1) - 64);
                PHASER->phaser_pan->redraw();
                break;
            case 61:
                ALIENWAH->Alienwah_pan->value(rkr->efx_Alienwah->getpar(1) - 64);
                ALIENWAH->Alienwah_pan->redraw();
                break;
            case 62:
                MUSDELAY->musdelay_pan1->value(rkr->efx_MusDelay->getpar(1) - 64);
                MUSDELAY->musdelay_pan1->redraw();
                break;
            case 63:
                REVERB->reverb_pan->value(rkr->efx_Rev->getpar(1) - 64);
                REVERB->reverb_pan->redraw();
                break;
            case 65:
                MUSDELAY->musdelay_pan2->value(rkr->efx_MusDelay->getpar(7) - 64);
                MUSDELAY->musdelay_pan2->redraw();
                break;
            case 66:
                WHAWHA->WhaWha_pan->value(rkr->efx_WhaWha->getpar(1) - 64);
                WHAWHA->WhaWha_pan->redraw();
                break;
            case 67:
                PAN->pan_pan->value(rkr->efx_Pan->getpar(1) - 64);
                PAN->pan_pan->redraw();
                break;
            case 68:
                OVRD->ovrd_drive->value(rkr->efx_Overdrive->getpar(3));
                OVRD->ovrd_drive->redraw();
                break;
            case 69:
                DIST->dist_drive->value(rkr->efx_Distorsion->getpar(3));
                DIST->dist_drive->redraw();
                break;
            case 70:
                OVRD->ovrd_level->value(rkr->efx_Overdrive->getpar(4));
                OVRD->ovrd_level->redraw();
                break;
            case 71:
                DIST->dist_level->value(rkr->efx_Distorsion->getpar(4));
                DIST->dist_level->redraw();
                break;
            case 72:
                CHORUS->chorus_freq->value(rkr->efx_Chorus->getpar(2));
                CHORUS->chorus_freq->redraw();
                break;
            case 73:
                FLANGER->flanger_freq->value(rkr->efx_Flanger->getpar(2));
                FLANGER->flanger_freq->redraw();
                break;
            case 74:
                PHASER->phaser_freq->value(rkr->efx_Phaser->getpar(2));
                PHASER->phaser_freq->redraw();
                break;
            case 75:
                WHAWHA->WhaWha_freq->value(rkr->efx_WhaWha->getpar(2));
                WHAWHA->WhaWha_freq->redraw();
                break;
            case 76:
                ALIENWAH->Alienwah_freq->value(rkr->efx_Alienwah->getpar(2));
                ALIENWAH->Alienwah_freq->redraw();
                break;
            case 77:
                PAN->pan_freq->value(rkr->efx_Pan->getpar(2));
                PAN->pan_freq->redraw();
                break;
            case 78:
                ECHO->echo_fb->value(rkr->efx_Echo->getpar(5));
                ECHO->echo_fb->redraw();
                break;
            case 79:
                CHORUS->chorus_fb->value(rkr->efx_Chorus->getpar(8));
                CHORUS->chorus_fb->redraw();
                break;
            case 80:
                FLANGER->flanger_fb->value(rkr->efx_Flanger->getpar(8));
                FLANGER->flanger_fb->redraw();
                break;
            case 81:
                PHASER->phaser_fb->value(rkr->efx_Phaser->getpar(7));
                PHASER->phaser_fb->redraw();
                break;
            case 82:
                ALIENWAH->Alienwah_fb->value(rkr->efx_Alienwah->getpar(7));
                ALIENWAH->Alienwah_fb->redraw();
                break;
            case 83:
                MUSDELAY->musdelay_fb1->value(rkr->efx_MusDelay->getpar(5));
                MUSDELAY->musdelay_fb1->redraw();
                break;
            case 84:
                MUSDELAY->musdelay_fb2->value(rkr->efx_MusDelay->getpar(9));
                MUSDELAY->musdelay_fb2->redraw();
                break;
            case 85:
                OVRD->ovrd_lpf->value(rkr->efx_Overdrive->getpar(7));
                OVRD->ovrd_lpf->redraw();
                break;
            case 86:
                DIST->dist_lpf->value(rkr->efx_Distorsion->getpar(7));
                DIST->dist_lpf->redraw();
                break;
            case 87:
                REVERB->reverb_LPF->value(rkr->efx_Rev->getpar(7));
                REVERB->reverb_LPF->redraw();
                break;
            case 88:
                OVRD->ovrd_hpf->value(rkr->efx_Overdrive->getpar(8));
                OVRD->ovrd_hpf->redraw();
                break;
            case 89:
                DIST->dist_hpf->value(rkr->efx_Distorsion->getpar(8));
                DIST->dist_hpf->redraw();
                break;
            case 90:
                REVERB->reverb_HPF->value(rkr->efx_Rev->getpar(8));
                REVERB->reverb_HPF->redraw();
                break;
            case 91:
                CHORUS->chorus_LR->value(rkr->efx_Chorus->getpar(9));
                CHORUS->chorus_LR->redraw();
                break;
            case 92:
                FLANGER->flanger_LR->value(rkr->efx_Flanger->getpar(9));
                FLANGER->flanger_LR->redraw();
                break;
            case 93:
                PHASER->phaser_LR->value(rkr->efx_Phaser->getpar(9) - 64);
                PHASER->phaser_LR->redraw();
                break;
            case 94:
                OVRD->ovrd_LRc->value(rkr->efx_Overdrive->getpar(2));
                OVRD->ovrd_LRc->redraw();
                break;
            case 95:
                DIST->dist_LRc->value(rkr->efx_Distorsion->getpar(2));
                DIST->dist_LRc->redraw();
                break;
            case 96:
                ALIENWAH->Alienwah_LR->value(rkr->efx_Alienwah->getpar(9));
                ALIENWAH->Alienwah_LR->redraw();
                break;
            case 97:
                ECHO->echo_LRc->value(rkr->efx_Echo->getpar(4));
                ECHO->echo_LRc->redraw();
                break;
            case 98:
                MUSDELAY->musdelay_LRc->value(rkr->efx_MusDelay->getpar(4));
                MUSDELAY->musdelay_LRc->redraw();
                break;
            case 99:
                CHORUS->chorus_stdf->value(rkr->efx_Chorus->getpar(5));
                CHORUS->chorus_stdf->redraw();
                break;
            case 100:
                FLANGER->flanger_stdf->value(rkr->efx_Flanger->getpar(5));
                FLANGER->flanger_stdf->redraw();
                break;
            case 101:
                PHASER->phaser_stdf->value(rkr->efx_Phaser->getpar(5));
                PHASER->phaser_stdf->redraw();
                break;
            case 102:
                WHAWHA->WhaWha_stdf->value(rkr->efx_WhaWha->getpar(5));
                WHAWHA->WhaWha_stdf->redraw();
                break;
            case 103:
                ALIENWAH->Alienwah_stdf->value(rkr->efx_Alienwah->getpar(5));
                ALIENWAH->Alienwah_stdf->redraw();
                break;
            case 104:
                PAN->pan_stdf->value(rkr->efx_Pan->getpar(5));
                PAN->pan_stdf->redraw();
                break;
            case 105:
                CHORUS->chorus_rnd->value(rkr->efx_Chorus->getpar(3));
                CHORUS->chorus_rnd->redraw();
                break;
            case 106:
                FLANGER->flanger_rnd->value(rkr->efx_Flanger->getpar(3));
                FLANGER->flanger_rnd->redraw();
                break;
            case 107:
                PHASER->phaser_rnd->value(rkr->efx_Phaser->getpar(3));
                PHASER->phaser_rnd->redraw();
                break;
            case 108:
                WHAWHA->WhaWha_rnd->value(rkr->efx_WhaWha->getpar(3));
                WHAWHA->WhaWha_rnd->redraw();
                break;
            case 109:
                ALIENWAH->Alienwah_rnd->value(rkr->efx_Alienwah->getpar(3));
                ALIENWAH->Alienwah_rnd->redraw();
                break;
            case 110:
                PAN->pan_rnd->value(rkr->efx_Pan->getpar(3));
                PAN->pan_rnd->redraw();
                break;
            case 111:
                WHAWHA->WhaWha_ampsns->value(rkr->efx_WhaWha->getpar(7));
                WHAWHA->WhaWha_ampsns->redraw();
                break;
            case 112:
                WHAWHA->WhaWha_ampsnsinv->value(rkr->efx_WhaWha->getpar(8));
                WHAWHA->WhaWha_ampsnsinv->redraw();
                break;
            case 113:
                WHAWHA->WhaWha_smooth->value(rkr->efx_WhaWha->getpar(9));
                WHAWHA->WhaWha_smooth->redraw();
                break;
            case 114:
                PHASER->phaser_phase->value(rkr->efx_Phaser->getpar(11));
                PHASER->phaser_phase->redraw();
                break;
            case 115:
                ALIENWAH->Alienwah_phase->value(rkr->efx_Alienwah->getpar(10));
                ALIENWAH->Alienwah_phase->redraw();
                break;
            case 116:
                ActOnOff();
                break;
            case 117:
                APHASER->aphaser_WD->value(Dry_Wet(rkr->efx_APhaser->getpar(0)));
                APHASER->aphaser_WD->redraw();
                break;
            case 118:
                APHASER->aphaser_distort->value(rkr->efx_APhaser->getpar(1));
                APHASER->aphaser_distort->redraw();
                break;
            case 119:
                APHASER->aphaser_freq->value(rkr->efx_APhaser->getpar(2));
                APHASER->aphaser_freq->redraw();
                break;
            case 120:
                APHASER->aphaser_phase->value(rkr->efx_APhaser->getpar(11));
                APHASER->aphaser_phase->redraw();
                break;
            case 121:
                APHASER->aphaser_width->value(rkr->efx_APhaser->getpar(6));
                APHASER->aphaser_width->redraw();
                break;
            case 122:
                APHASER->aphaser_fb->value(rkr->efx_APhaser->getpar(7) - 64);
                APHASER->aphaser_fb->redraw();
                break;
            case 123:
                APHASER->aphaser_mismatch->value(rkr->efx_APhaser->getpar(9));
                APHASER->aphaser_mismatch->redraw();
                break;
            case 124:
                APHASER->aphaser_stdf->value(rkr->efx_APhaser->getpar(5));
                APHASER->aphaser_stdf->redraw();
                break;
            case 125:
                DERELICT->derelict_WD->value(Dry_Wet(rkr->efx_Derelict->getpar(0)));
                DERELICT->derelict_WD->redraw();
                break;
            case 126:
                DERELICT->derelict_pan->value(rkr->efx_Derelict->getpar(1) - 64);
                DERELICT->derelict_pan->redraw();
                break;
            case 127:
                DERELICT->derelict_LRc->value(rkr->efx_Derelict->getpar(2));
                DERELICT->derelict_LRc->redraw();
                break;
            case 2:
                DERELICT->derelict_drive->value(rkr->efx_Derelict->getpar(3));
                DERELICT->derelict_drive->redraw();
                break;
            case 3:
                DERELICT->derelict_level->value(rkr->efx_Derelict->getpar(4));
                DERELICT->derelict_level->redraw();
                break;
            case 4:
                DERELICT->derelict_lpf->value(rkr->efx_Derelict->getpar(7));
                DERELICT->derelict_lpf->redraw();
                break;
            case 5:
                DERELICT->derelict_hpf->value(rkr->efx_Derelict->getpar(8));
                DERELICT->derelict_hpf->redraw();
                break;
            case 6:
                DERELICT->derelict_st->value(rkr->efx_Derelict->getpar(9));
                DERELICT->derelict_st->redraw();
                break;
            case 8:
                DERELICT->derelict_oct->value(rkr->efx_Derelict->getpar(11));
                DERELICT->derelict_oct->redraw();
                break;
            case 9:
                DIST->dist_oct->value(rkr->efx_Distorsion->getpar(12));
                DIST->dist_oct->redraw();
                break;
            case 130:
                EQ->eq_Gain->value(rkr->efx_EQ1->getpar(0) - 64);
                EQ->eq_Gain->redraw();
                break;
            case 131:
                EQ->eq_Q->value(rkr->efx_EQ1->getpar(13) - 64);
                EQ->eq_Q->redraw();
                break;
            case 132:
                EQ->eq_1->value(rkr->efx_EQ1->getpar(12) - 64);
                EQ->eq_1->redraw();
                break;
            case 133:
                EQ->eq_2->value(rkr->efx_EQ1->getpar(17) - 64);
                EQ->eq_2->redraw();
                break;
            case 134:
                EQ->eq_3->value(rkr->efx_EQ1->getpar(22) - 64);
                EQ->eq_3->redraw();
                break;
            case 135:
                EQ->eq_4->value(rkr->efx_EQ1->getpar(27) - 64);
                EQ->eq_4->redraw();
                break;
            case 136:
                EQ->eq_5->value(rkr->efx_EQ1->getpar(32) - 64);
                EQ->eq_5->redraw();
                break;
            case 137:
                EQ->eq_6->value(rkr->efx_EQ1->getpar(37) - 64);
                EQ->eq_6->redraw();
                break;
            case 138:
                EQ->eq_7->value(rkr->efx_EQ1->getpar(42) - 64);
                EQ->eq_7->redraw();
                break;
            case 139:
                EQ->eq_8->value(rkr->efx_EQ1->getpar(47) - 64);
                EQ->eq_8->redraw();
                break;
            case 140:
                EQ->eq_9->value(rkr->efx_EQ1->getpar(52) - 64);
                EQ->eq_9->redraw();
                break;
            case 141:
                EQ->eq_10->value(rkr->efx_EQ1->getpar(57) - 64);
                EQ->eq_10->redraw();
                break;
            case 142:
                COMPRESS->compress_ATime->value(rkr->efx_Compressor->getpar(4));
                COMPRESS->compress_ATime->redraw();
                break;
            case 143:
                COMPRESS->compress_RTime->value(rkr->efx_Compressor->getpar(5));
                COMPRESS->compress_RTime->redraw();
                break;
            case 144:
                COMPRESS->compress_Ratio->value(rkr->efx_Compressor->getpar(2));
                COMPRESS->compress_Ratio->redraw();
                break;
            case 145:
                COMPRESS->compress_Knee->value(rkr->efx_Compressor->getpar(7));
                COMPRESS->compress_Knee->redraw();
                break;
            case 146:
                COMPRESS->compress_threshold->value(rkr->efx_Compressor->getpar(1));
                COMPRESS->compress_threshold->redraw();
                break;
            case 147:
                COMPRESS->compress_output->value(rkr->efx_Compressor->getpar(3));
                COMPRESS->compress_output->redraw();
                break;
            case 148:
                PEQ->eqp_Gain->value(rkr->efx_EQ2->getpar(0) - 64);
                PEQ->eqp_Gain->redraw();
                break;
            case 149:
                PEQ->eqp_LF->value(rkr->efx_EQ2->getpar(11));
                PEQ->eqp_LF->redraw();
                break;
            case 150:
                PEQ->eqp_LFg->value(rkr->efx_EQ2->getpar(12) - 64);
                PEQ->eqp_LFg->redraw();
                break;
            case 151:
                PEQ->eqp_LQ->value(rkr->efx_EQ2->getpar(13) - 64);
                PEQ->eqp_LQ->redraw();
                break;
            case 152:
                PEQ->eqp_MF->value(rkr->efx_EQ2->getpar(16));
                PEQ->eqp_MF->redraw();
                break;
            case 153:
                PEQ->eqp_MFg->value(rkr->efx_EQ2->getpar(17) - 64);
                PEQ->eqp_MFg->redraw();
                break;
            case 154:
                PEQ->eqp_MQ->value(rkr->efx_EQ2->getpar(18) - 64);
                PEQ->eqp_MQ->redraw();
                break;
            case 155:
                PEQ->eqp_HF->value(rkr->efx_EQ2->getpar(21));
                PEQ->eqp_HF->redraw();
                break;
            case 156:
                PEQ->eqp_HFg->value(rkr->efx_EQ2->getpar(22) - 64);
                PEQ->eqp_HFg->redraw();
                break;
            case 157:
                PEQ->eqp_HQ->value(rkr->efx_EQ2->getpar(23) - 64);
                PEQ->eqp_HQ->redraw();
                break;
            case 158:
                DFLANGE->dflange_WD->value(Dry_Wet(rkr->efx_DFlange->getpar(0)));
                DFLANGE->dflange_WD->redraw();
                break;
            case 159:
                DFLANGE->dflange_pan->value(rkr->efx_DFlange->getpar(1));
                DFLANGE->dflange_pan->redraw();
                break;
            case 160:
                DFLANGE->dflange_LR->value(rkr->efx_DFlange->getpar(2));
                DFLANGE->dflange_LR->redraw();
                break;
            case 161:
                DFLANGE->dflange_depth->value(rkr->efx_DFlange->getpar(3));
                DFLANGE->dflange_depth->redraw();
                break;
            case 162:
                DFLANGE->dflange_width->value(rkr->efx_DFlange->getpar(4));
                DFLANGE->dflange_width->redraw();
                break;
            case 163:
                DFLANGE->dflange_offset->value(rkr->efx_DFlange->getpar(5));
                DFLANGE->dflange_offset->redraw();
                break;
            case 164:
                DFLANGE->dflange_fb->value(rkr->efx_DFlange->getpar(6));
                DFLANGE->dflange_fb->redraw();
                break;
            case 165:
                DFLANGE->dflange_lpf->value(rkr->efx_DFlange->getpar(7));
                DFLANGE->dflange_lpf->redraw();
                break;
            case 166:
                DFLANGE->dflange_freq->value(rkr->efx_DFlange->getpar(10));
                DFLANGE->dflange_freq->redraw();
                break;
            case 167:
                DFLANGE->dflange_stdf->value(rkr->efx_DFlange->getpar(11));
                DFLANGE->dflange_stdf->redraw();
                break;
            case 168:
                DFLANGE->dflange_rnd->value(rkr->efx_DFlange->getpar(13));
                DFLANGE->dflange_rnd->redraw();
                break;
            case 169:
                VALVE->valve_WD->value(Dry_Wet(rkr->efx_Valve->getpar(0)));
                VALVE->valve_WD->redraw();
                break;
            case 170:
                VALVE->valve_LRc->value(rkr->efx_Valve->getpar(2));
                VALVE->valve_LRc->redraw();
                break;
            case 171:
                VALVE->valve_pan->value(rkr->efx_Valve->getpar(1) - 64);
                VALVE->valve_pan->redraw();
                break;
            case 172:
                VALVE->valve_level->value(rkr->efx_Valve->getpar(4));
                VALVE->valve_level->redraw();
                break;
            case 173:
                VALVE->valve_drive->value(rkr->efx_Valve->getpar(3));
                VALVE->valve_drive->redraw();
                break;
            case 174:
                VALVE->valve_Q->value(rkr->efx_Valve->getpar(10));
                VALVE->valve_Q->redraw();
                break;
            case 175:
                VALVE->valve_Pre->value(rkr->efx_Valve->getpar(12));
                VALVE->valve_Pre->redraw();
                break;
            case 176:
                VALVE->valve_lpf->value(rkr->efx_Valve->getpar(6));
                VALVE->valve_lpf->redraw();
                break;
            case 177:
                VALVE->valve_hpf->value(rkr->efx_Valve->getpar(7));
                VALVE->valve_hpf->redraw();
                break;
            case 178:
                RING->ring_WD->value(Dry_Wet(rkr->efx_Ring->getpar(0)));
                RING->ring_WD->redraw();
                break;
            case 179:
                RING->ring_LRc->value(rkr->efx_Ring->getpar(2));
                RING->ring_LRc->redraw();
                break;
            case 180:
                RING->ring_input->value(rkr->efx_Ring->getpar(11));
                RING->ring_input->redraw();
                break;
            case 181:
                RING->ring_level->value(rkr->efx_Ring->getpar(3));
                RING->ring_level->redraw();
                break;
            case 182:
                RING->ring_pan->value(rkr->efx_Ring->getpar(1));
                RING->ring_pan->redraw();
                break;
            case 183:
                RING->ring_depth->value(rkr->efx_Ring->getpar(4));
                RING->ring_depth->redraw();
                break;
            case 184:
                RING->ring_freq->value(rkr->efx_Ring->getpar(5));
                RING->ring_freq->redraw();
                break;
            case 185:
                RING->ring_sin->value(rkr->efx_Ring->getpar(7));
                RING->ring_sin->redraw();
                break;
            case 186:
                RING->ring_tri->value(rkr->efx_Ring->getpar(8));
                RING->ring_tri->redraw();
                break;
            case 187:
                RING->ring_saw->value(rkr->efx_Ring->getpar(9));
                RING->ring_saw->redraw();
                break;
            case 188:
                RING->ring_squ->value(rkr->efx_Ring->getpar(10));
                RING->ring_squ->redraw();
                break;
            case 189:
                EXCITER->ex_Gain->value(rkr->efx_Exciter->getpar(0));
                EXCITER->ex_Gain->redraw();
                break;
            case 190:
                EXCITER->ex_hfreq->value(rkr->efx_Exciter->getpar(11));
                EXCITER->ex_hfreq->redraw();
                break;
            case 191:
                EXCITER->ex_lfreq->value(rkr->efx_Exciter->getpar(12));
                EXCITER->ex_lfreq->redraw();
                break;
            case 192:
                EXCITER->ex_1->value(rkr->efx_Exciter->getpar(1));
                EXCITER->ex_1->redraw();
                break;
            case 193:
                EXCITER->ex_2->value(rkr->efx_Exciter->getpar(2));
                EXCITER->ex_2->redraw();
                break;
            case 194:
                EXCITER->ex_3->value(rkr->efx_Exciter->getpar(3));
                EXCITER->ex_3->redraw();
                break;
            case 195:
                EXCITER->ex_4->value(rkr->efx_Exciter->getpar(4));
                EXCITER->ex_4->redraw();
                break;
            case 196:
                EXCITER->ex_5->value(rkr->efx_Exciter->getpar(5));
                EXCITER->ex_5->redraw();
                break;
            case 197:
                EXCITER->ex_6->value(rkr->efx_Exciter->getpar(6));
                EXCITER->ex_6->redraw();
                break;
            case 198:
                EXCITER->ex_7->value(rkr->efx_Exciter->getpar(7));
                EXCITER->ex_7->redraw();
                break;
            case 199:
                EXCITER->ex_8->value(rkr->efx_Exciter->getpar(8));
                EXCITER->ex_8->redraw();
                break;
            case 200:
                EXCITER->ex_9->value(rkr->efx_Exciter->getpar(9));
                EXCITER->ex_9->redraw();
                break;
            case 201:
                EXCITER->ex_10->value(rkr->efx_Exciter->getpar(10));
                EXCITER->ex_10->redraw();
                break;
            case 202:
                DISTBAND->distband_WD->value(Dry_Wet(rkr->efx_DistBand->getpar(0)));
                DISTBAND->distband_WD->redraw();
                break;
            case 203:
                DISTBAND->distband_LRc->value(rkr->efx_DistBand->getpar(2));
                DISTBAND->distband_LRc->redraw();
                break;
            case 204:
                DISTBAND->distband_drive->value(rkr->efx_DistBand->getpar(3));
                DISTBAND->distband_drive->redraw();
                break;
            case 205:
                DISTBAND->distband_level->value(rkr->efx_DistBand->getpar(4));
                DISTBAND->distband_level->redraw();
                break;
            case 206:
                DISTBAND->distband_volL->value(rkr->efx_DistBand->getpar(8));
                DISTBAND->distband_volL->redraw();
                break;
            case 207:
                DISTBAND->distband_volM->value(rkr->efx_DistBand->getpar(9));
                DISTBAND->distband_volM->redraw();
                break;
            case 208:
                DISTBAND->distband_volH->value(rkr->efx_DistBand->getpar(10));
                DISTBAND->distband_volH->redraw();
                break;
            case 209:
                DISTBAND->distband_cross1->value(rkr->efx_DistBand->getpar(12));
                DISTBAND->distband_cross1->redraw();
                break;
            case 210:
                DISTBAND->distband_cross2->value(rkr->efx_DistBand->getpar(13));
                DISTBAND->distband_cross2->redraw();
                break;
            case 211:
                DISTBAND->distband_pan->value(rkr->efx_DistBand->getpar(1) - 64);
                DISTBAND->distband_pan->redraw();
                break;
            case 212:
                ARPIE->arpie_WD->value(Dry_Wet(rkr->efx_Arpie->getpar(0)));
                ARPIE->arpie_WD->redraw();
                break;
            case 213:
                ARPIE->arpie_arpe->value(rkr->efx_Arpie->getpar(7));
                ARPIE->arpie_arpe->redraw();
                break;
            case 214:
                ARPIE->arpie_pan->value(rkr->efx_Arpie->getpar(1));
                ARPIE->arpie_pan->redraw();
                break;
            case 215:
                ARPIE->arpie_delay->value(rkr->efx_Arpie->getpar(2));
                ARPIE->arpie_delay->redraw();
                break;
            case 216:
                ARPIE->arpie_LRdl->value(rkr->efx_Arpie->getpar(3));
                ARPIE->arpie_LRdl->redraw();
                break;
            case 217:
                ARPIE->arpie_LRc->value(rkr->efx_Arpie->getpar(4));
                ARPIE->arpie_LRc->redraw();
                break;
            case 218:
                ARPIE->arpie_fb->value(rkr->efx_Arpie->getpar(5));
                ARPIE->arpie_fb->redraw();
                break;
            case 219:
                ARPIE->arpie_damp->value(rkr->efx_Arpie->getpar(6));
                ARPIE->arpie_damp->redraw();
                break;
            case 220:
                EXPANDER->expander_ATime->value(rkr->efx_Expander->getpar(3));
                EXPANDER->expander_ATime->redraw();
                break;
            case 221:
                EXPANDER->expander_RTime->value(rkr->efx_Expander->getpar(4));
                EXPANDER->expander_RTime->redraw();
                break;
            case 222:
                EXPANDER->expander_shape->value(rkr->efx_Expander->getpar(2));
                EXPANDER->expander_shape->redraw();
                break;
            case 223:
                EXPANDER->expander_threshold->value(rkr->efx_Expander->getpar(1));
                EXPANDER->expander_threshold->redraw();
                break;
            case 224:
                EXPANDER->expander_level->value(rkr->efx_Expander->getpar(7));
                EXPANDER->expander_level->redraw();
                break;
            case 225:
                EXPANDER->expander_LPF->value(rkr->efx_Expander->getpar(5));
                EXPANDER->expander_LPF->redraw();
                break;
            case 226:
                EXPANDER->expander_HPF->value(rkr->efx_Expander->getpar(6));
                EXPANDER->expander_HPF->redraw();
                break;
            case 227:
                SHUFFLE->shuffle_WD->value(Dry_Wet(rkr->efx_Shuffle->getpar(0)));
                SHUFFLE->shuffle_WD->redraw();
                break;
            case 228:
                SHUFFLE->shuffle_cross1->value(rkr->efx_Shuffle->getpar(5));
                SHUFFLE->shuffle_cross1->redraw();
                break;
            case 229:
                SHUFFLE->shuffle_volL->value(rkr->efx_Shuffle->getpar(1));
                SHUFFLE->shuffle_volL->redraw();
                break;
            case 230:
                SHUFFLE->shuffle_cross2->value(rkr->efx_Shuffle->getpar(6));
                SHUFFLE->shuffle_cross2->redraw();
                break;
            case 231:
                SHUFFLE->shuffle_volML->value(rkr->efx_Shuffle->getpar(2));
                SHUFFLE->shuffle_volML->redraw();
                break;
            case 232:
                SHUFFLE->shuffle_cross3->value(rkr->efx_Shuffle->getpar(7));
                SHUFFLE->shuffle_cross3->redraw();
                break;
            case 233:
                SHUFFLE->shuffle_volMH->value(rkr->efx_Shuffle->getpar(3));
                SHUFFLE->shuffle_volMH->redraw();
                break;
            case 234:
                SHUFFLE->shuffle_cross4->value(rkr->efx_Shuffle->getpar(8));
                SHUFFLE->shuffle_cross4->redraw();
                break;
            case 235:
                SHUFFLE->shuffle_volH->value(rkr->efx_Shuffle->getpar(4));
                SHUFFLE->shuffle_volH->redraw();
                break;
            case 236:
                SHUFFLE->shuffle_Q->value(rkr->efx_Shuffle->getpar(9));
                SHUFFLE->shuffle_Q->redraw();
                break;
            case 237:
                SYNTHFILTER->synthfilter_WD->value(Dry_Wet(rkr->efx_Synthfilter->getpar(0)));
                SYNTHFILTER->synthfilter_WD->redraw();
                break;
            case 238:
                SYNTHFILTER->synthfilter_Distort->value(rkr->efx_Synthfilter->getpar(1));
                SYNTHFILTER->synthfilter_Distort->redraw();
                break;
            case 239:
                SYNTHFILTER->synthfilter_freq->value(rkr->efx_Synthfilter->getpar(2));
                SYNTHFILTER->synthfilter_freq->redraw();
                break;
            case 240:
                SYNTHFILTER->synthfilter_stdf->value(rkr->efx_Synthfilter->getpar(5));
                SYNTHFILTER->synthfilter_stdf->redraw();
                break;
            case 241:
                SYNTHFILTER->synthfilter_width->value(rkr->efx_Synthfilter->getpar(6));
                SYNTHFILTER->synthfilter_width->redraw();
                break;
            case 242:
                SYNTHFILTER->synthfilter_fb->value(rkr->efx_Synthfilter->getpar(7));
                SYNTHFILTER->synthfilter_fb->redraw();
                break;
            case 243:
                SYNTHFILTER->synthfilter_dpth->value(rkr->efx_Synthfilter->getpar(11));
                SYNTHFILTER->synthfilter_dpth->redraw();
                break;
            case 244:
                SYNTHFILTER->synthfilter_EnvSens->value(rkr->efx_Synthfilter->getpar(12));
                SYNTHFILTER->synthfilter_EnvSens->redraw();
                break;
            case 245:
                SYNTHFILTER->synthfilter_ATime->value(rkr->efx_Synthfilter->getpar(13));
                SYNTHFILTER->synthfilter_ATime->redraw();
                break;
            case 246:
                SYNTHFILTER->synthfilter_RTime->value(rkr->efx_Synthfilter->getpar(14));
                SYNTHFILTER->synthfilter_RTime->redraw();
                break;
            case 247:
                SYNTHFILTER->synthfilter_Offset->value(rkr->efx_Synthfilter->getpar(15));
                SYNTHFILTER->synthfilter_Offset->redraw();
                break;
            case 248:
                VARYBAND->varyband_WD->value(Dry_Wet(rkr->efx_VaryBand->getpar(0)));
                VARYBAND->varyband_WD->redraw();
                break;
            case 249:
                VARYBAND->varyband_freq1->value(rkr->efx_VaryBand->getpar(1));
                VARYBAND->varyband_freq1->redraw();
                break;
            case 250:
                VARYBAND->varyband_stdf1->value(rkr->efx_VaryBand->getpar(3));
                VARYBAND->varyband_stdf1->redraw();
                break;
            case 251:
                VARYBAND->varyband_freq2->value(rkr->efx_VaryBand->getpar(4));
                VARYBAND->varyband_freq2->redraw();
                break;
            case 252:
                VARYBAND->varyband_stdf2->value(rkr->efx_VaryBand->getpar(6));
                VARYBAND->varyband_stdf2->redraw();
                break;
            case 253:
                VARYBAND->varyband_cross1->value(rkr->efx_VaryBand->getpar(7));
                VARYBAND->varyband_cross1->redraw();
                break;
            case 254:
                VARYBAND->varyband_cross2->value(rkr->efx_VaryBand->getpar(8));
                VARYBAND->varyband_cross2->redraw();
                break;
            case 255:
                VARYBAND->varyband_cross3->value(rkr->efx_VaryBand->getpar(9));
                VARYBAND->varyband_cross3->redraw();
                break;
            case 256:
                MUTROMOJO->mutromojo_WD->value(Dry_Wet(rkr->efx_MuTroMojo->getpar(0)));
                MUTROMOJO->mutromojo_WD->redraw();
                break;
            case 257:
                MUTROMOJO->mutromojo_lp->value(rkr->efx_MuTroMojo->getpar(10));
                MUTROMOJO->mutromojo_lp->redraw();
                break;
            case 258:
                MUTROMOJO->mutromojo_bp->value(rkr->efx_MuTroMojo->getpar(11));
                MUTROMOJO->mutromojo_bp->redraw();
                break;
            case 259:
                MUTROMOJO->mutromojo_hp->value(rkr->efx_MuTroMojo->getpar(12));
                MUTROMOJO->mutromojo_hp->redraw();
                break;
            case 260:
                MUTROMOJO->mutromojo_dpth->value(rkr->efx_MuTroMojo->getpar(6));
                MUTROMOJO->mutromojo_dpth->redraw();
                break;
            case 261:
                MUTROMOJO->mutromojo_freq->value(rkr->efx_MuTroMojo->getpar(2));
                MUTROMOJO->mutromojo_freq->redraw();
                break;
            case 262:
                MUTROMOJO->mutromojo_q->value(rkr->efx_MuTroMojo->getpar(1));
                MUTROMOJO->mutromojo_q->redraw();
                break;
            case 263:
                MUTROMOJO->mutromojo_rng->value(rkr->efx_MuTroMojo->getpar(14));
                MUTROMOJO->mutromojo_rng->redraw();
                break;
            case 264:
                MUTROMOJO->mutromojo_ampsnsinv->value(rkr->efx_MuTroMojo->getpar(8));
                MUTROMOJO->mutromojo_ampsnsinv->redraw();
                break;
            case 265:
                MUTROMOJO->mutromojo_ampsns->value(rkr->efx_MuTroMojo->getpar(7));
                MUTROMOJO->mutromojo_ampsns->redraw();
                break;
            case 266:
                MUTROMOJO->mutromojo_smooth->value(rkr->efx_MuTroMojo->getpar(9));
                MUTROMOJO->mutromojo_smooth->redraw();
                break;
            case 267:
                LOOPER->looper_WD->value(Dry_Wet(rkr->efx_Looper->getpar(0)));
                LOOPER->looper_WD->redraw();
                break;
            case 268:
                LOOPER->looper_level1->value(rkr->efx_Looper->getpar(6));
                LOOPER->looper_level1->redraw();
                break;
            case 269:
                LOOPER->looper_level2->value(rkr->efx_Looper->getpar(10));
                LOOPER->looper_level2->redraw();
                break;
            case 270:
                LOOPER->looper_rv->value(rkr->efx_Looper->getpar(5));
                LOOPER->looper_rv->redraw();
                break;
            case 271:
                LOOPER->looper_ap->value(rkr->efx_Looper->getpar(9));
                LOOPER->looper_ap->redraw();
                break;
            case 272:
                LOOPER->looper_play->value(rkr->efx_Looper->getpar(1));
                update_looper();
                break;
            case 273:
                LOOPER->looper_stop->value(rkr->efx_Looper->getpar(2));
                update_looper();
                break;
            case 274:
                LOOPER->looper_record->value(rkr->efx_Looper->getpar(3));
                update_looper();
                break;
            case 275:
                LOOPER->looper_r1->value(rkr->efx_Looper->getpar(11));
                LOOPER->looper_r1->redraw();
                break;
            case 276:
                LOOPER->looper_r2->value(rkr->efx_Looper->getpar(12));
                LOOPER->looper_r2->redraw();
                break;
            case 277:
                LOOPER->looper_t1->value(rkr->efx_Looper->getpar(7));
                update_looper();
                break;
            case 278:
                LOOPER->looper_t2->value(rkr->efx_Looper->getpar(8));
                update_looper();
                break;
            case 279:
                /* Setting the looper_clear button after MIDI control really does not work. 
                   The button for the gui is just press and release, i.e. not a toggle button.
                   Since the clear flag Pclear is set and remains set until another button such
                   as play or record gets pushed, the Pclear variable returned by getpar(4) is
                   going to show pressed (value = 1) and set the button as pressed here. And it
                   will remain in that state until restart. If pressed manually, it just does
                   unpress and release back to press.  Would be nice to show the press and release
                   with MIDI control, but it would probably have to be a timer issue and additional
                   complications for very little value. MIDI control does work, but we should not show
                   it on the gui button here with this method */
            //    LOOPER->looper_clear->value(rkr->efx_Looper->getpar(4));
                update_looper();
                break;
            case 280:
                CONVOLOTRON->convo_WD->value(Dry_Wet(rkr->efx_Convol->getpar(0)));
                CONVOLOTRON->convo_WD->redraw();
                break;
            case 281:
                CONVOLOTRON->convo_pan->value(rkr->efx_Convol->getpar(1) - 64);
                CONVOLOTRON->convo_pan->redraw();
                break;
            case 282:
                CONVOLOTRON->convo_level->value(rkr->efx_Convol->getpar(7));
                CONVOLOTRON->convo_level->redraw();
                break;
            case 283:
                CONVOLOTRON->convo_damp->value(rkr->efx_Convol->getpar(6));
                CONVOLOTRON->convo_damp->redraw();
                break;
            case 284:
                CONVOLOTRON->convo_fb->value(rkr->efx_Convol->getpar(10));
                CONVOLOTRON->convo_fb->redraw();
                break;
            case 285:
                CONVOLOTRON->convo_length->value(rkr->efx_Convol->getpar(3));
                CONVOLOTRON->convo_length->redraw();
                break;
            case 286:
                COILCRAFTER->coil_WD->value(rkr->efx_CoilCrafter->getpar(0));
                COILCRAFTER->coil_WD->redraw();
                break;
            case 287:
                COILCRAFTER->coil_tone->value(rkr->efx_CoilCrafter->getpar(7));
                COILCRAFTER->coil_tone->redraw();
                break;
            case 288:
                COILCRAFTER->coil_freq1->value(rkr->efx_CoilCrafter->getpar(3));
                COILCRAFTER->coil_freq1->redraw();
                break;
            case 289:
                COILCRAFTER->coil_q1->value(rkr->efx_CoilCrafter->getpar(4));
                COILCRAFTER->coil_q1->redraw();
                break;
            case 290:
                COILCRAFTER->coil_freq2->value(rkr->efx_CoilCrafter->getpar(5));
                COILCRAFTER->coil_freq2->redraw();
                break;
            case 291:
                COILCRAFTER->coil_q2->value(rkr->efx_CoilCrafter->getpar(6));
                COILCRAFTER->coil_q2->redraw();
                break;
            case 292:
                SHELFBOOST->shelf_gain->value(rkr->efx_ShelfBoost->getpar(0));
                SHELFBOOST->shelf_gain->redraw();
                break;
            case 293:
                SHELFBOOST->shelf_level->value(rkr->efx_ShelfBoost->getpar(4));
                SHELFBOOST->shelf_level->redraw();
                break;
            case 294:
                SHELFBOOST->shelf_freq1->value(rkr->efx_ShelfBoost->getpar(2));
                SHELFBOOST->shelf_freq1->redraw();
                break;
            case 295:
                SHELFBOOST->shelf_q1->value(rkr->efx_ShelfBoost->getpar(1));
                SHELFBOOST->shelf_q1->redraw();
                break;
            case 296:
                VOCODER->vo_WD->value(Dry_Wet(rkr->efx_Vocoder->getpar(0)));
                VOCODER->vo_WD->redraw();
                break;
            case 297:
                VOCODER->vo_pan->value(rkr->efx_Vocoder->getpar(1) - 64);
                VOCODER->vo_pan->redraw();
                break;
            case 298:
                VOCODER->vo_input->value(rkr->efx_Vocoder->getpar(4));
                VOCODER->vo_input->redraw();
                break;
            case 299:
                VOCODER->vo_mu->value(rkr->efx_Vocoder->getpar(2));
                VOCODER->vo_mu->redraw();
                break;
            case 300:
                VOCODER->vo_q->value(rkr->efx_Vocoder->getpar(3));
                VOCODER->vo_q->redraw();
                break;
            case 301:
                VOCODER->vo_ring->value(rkr->efx_Vocoder->getpar(6));
                VOCODER->vo_ring->redraw();
                break;
            case 302:
                VOCODER->vo_level->value(rkr->efx_Vocoder->getpar(5));
                VOCODER->vo_level->redraw();
                break;
            case 303:
                ECHOVERSE->echoverse_WD->value(Dry_Wet(rkr->efx_Echoverse->getpar(0)));
                ECHOVERSE->echoverse_WD->redraw();
                break;
            case 304:
                ECHOVERSE->echoverse_RV->value(rkr->efx_Echoverse->getpar(7));
                ECHOVERSE->echoverse_RV->redraw();
                break;
            case 305:
                ECHOVERSE->echoverse_pan->value(rkr->efx_Echoverse->getpar(1) - 64);
                ECHOVERSE->echoverse_pan->redraw();
                break;
            case 306:
                ECHOVERSE->echoverse_delay->value(rkr->efx_Echoverse->getpar(2));
                ECHOVERSE->echoverse_delay->redraw();
                break;
            case 307:
                ECHOVERSE->echoverse_LRdl->value(rkr->efx_Echoverse->getpar(3));
                ECHOVERSE->echoverse_LRdl->redraw();
                break;
            case 308:
                ECHOVERSE->echoverse_fb->value(rkr->efx_Echoverse->getpar(5));
                ECHOVERSE->echoverse_fb->redraw();
                break;
            case 309:
                ECHOVERSE->echoverse_damp->value(rkr->efx_Echoverse->getpar(6));
                ECHOVERSE->echoverse_damp->redraw();
                break;
            case 310:
                ECHOVERSE->echoverse_es->value(rkr->efx_Echoverse->getpar(9));
                ECHOVERSE->echoverse_es->redraw();
                break;
            case 311:
                ECHOVERSE->echoverse_angle->value(rkr->efx_Echoverse->getpar(4) - 64);
                ECHOVERSE->echoverse_angle->redraw();
                break;
            case 312:
                SUSTAINER->sus_gain->value(rkr->efx_Sustainer->getpar(0));
                SUSTAINER->sus_gain->redraw();
                break;
            case 313:
                SUSTAINER->sus_sus->value(rkr->efx_Sustainer->getpar(1));
                SUSTAINER->sus_sus->redraw();
                break;
            case 314:
                SEQUENCE->seq_WD->value(Dry_Wet(rkr->efx_Sequence->getpar(8)));
                SEQUENCE->seq_WD->redraw();
                break;
            case 315:
                SEQUENCE->seq_1->value(rkr->efx_Sequence->getpar(0));
                SEQUENCE->seq_1->redraw();
                break;
            case 316:
                SEQUENCE->seq_2->value(rkr->efx_Sequence->getpar(1));
                SEQUENCE->seq_2->redraw();
                break;
            case 317:
                SEQUENCE->seq_3->value(rkr->efx_Sequence->getpar(2));
                SEQUENCE->seq_3->redraw();
                break;
            case 318:
                SEQUENCE->seq_4->value(rkr->efx_Sequence->getpar(3));
                SEQUENCE->seq_4->redraw();
                break;
            case 319:
                SEQUENCE->seq_5->value(rkr->efx_Sequence->getpar(4));
                SEQUENCE->seq_5->redraw();
                break;
            case 320:
                SEQUENCE->seq_6->value(rkr->efx_Sequence->getpar(5));
                SEQUENCE->seq_6->redraw();
                break;
            case 321:
                SEQUENCE->seq_7->value(rkr->efx_Sequence->getpar(6));
                SEQUENCE->seq_7->redraw();
                break;
            case 322:
                SEQUENCE->seq_8->value(rkr->efx_Sequence->getpar(7));
                SEQUENCE->seq_8->redraw();
                break;
            case 323:
                SEQUENCE->seq_tempo->value(rkr->efx_Sequence->getpar(9));
                SEQUENCE->seq_tempo->redraw();
                break;
            case 324:
                SEQUENCE->seq_q->value(rkr->efx_Sequence->getpar(10) - 64);
                SEQUENCE->seq_q->redraw();
                break;
            case 325:
                SEQUENCE->seq_stdf->value(rkr->efx_Sequence->getpar(12));
                SEQUENCE->seq_stdf->redraw();
                break;
            case 326:
                SHIFTER->shifter_WD->value(Dry_Wet(rkr->efx_Shifter->getpar(0)));
                SHIFTER->shifter_WD->redraw();
                break;
            case 327:
                SHIFTER->shifter_int->value(rkr->efx_Shifter->getpar(6));
                SHIFTER->shifter_int->redraw();
                break;
            case 328:
                SHIFTER->shifter_gain->value(rkr->efx_Shifter->getpar(2) - 64);
                SHIFTER->shifter_gain->redraw();
                break;
            case 329:
                SHIFTER->shifter_pan->value(rkr->efx_Shifter->getpar(1) - 64);
                SHIFTER->shifter_pan->redraw();
                break;
            case 330:
                SHIFTER->shifter_attack->value(rkr->efx_Shifter->getpar(3));
                SHIFTER->shifter_attack->redraw();
                break;
            case 331:
                SHIFTER->shifter_decay->value(rkr->efx_Shifter->getpar(4));
                SHIFTER->shifter_decay->redraw();
                break;
            case 332:
                SHIFTER->shifter_thre->value(rkr->efx_Shifter->getpar(5));
                SHIFTER->shifter_thre->redraw();
                break;
            case 333:
                SHIFTER->shifter_whammy->value(rkr->efx_Shifter->getpar(9));
                SHIFTER->shifter_whammy->redraw();
                break;
            case 334:
                STOMPBOX->stomp_WD->value(rkr->efx_StompBox->getpar(0));
                STOMPBOX->stomp_WD->redraw();
                break;
            case 335:
                STOMPBOX->stomp_gain->value(rkr->efx_StompBox->getpar(4));
                STOMPBOX->stomp_gain->redraw();
                break;
            case 336:
                STOMPBOX->stomp_low->value(rkr->efx_StompBox->getpar(3) - 64);
                STOMPBOX->stomp_low->redraw();
                break;
            case 337:
                STOMPBOX->stomp_mid->value(rkr->efx_StompBox->getpar(2) - 64);
                STOMPBOX->stomp_mid->redraw();
                break;
            case 338:
                STOMPBOX->stomp_high->value(rkr->efx_StompBox->getpar(1) - 64);
                STOMPBOX->stomp_high->redraw();
                break;
            case 339:
                REVERBTRON->revtron_WD->value(Dry_Wet(rkr->efx_Reverbtron->getpar(0)));
                REVERBTRON->revtron_WD->redraw();
                break;
            case 340:
                REVERBTRON->revtron_pan->value(rkr->efx_Reverbtron->getpar(11) - 64);
                REVERBTRON->revtron_pan->redraw();
                break;
            case 341:
                REVERBTRON->revtron_level->value(rkr->efx_Reverbtron->getpar(7));
                REVERBTRON->revtron_level->redraw();
                break;
            case 342:
                REVERBTRON->revtron_damp->value(rkr->efx_Reverbtron->getpar(6));
                REVERBTRON->revtron_damp->redraw();
                break;
            case 343:
                REVERBTRON->revtron_fb->value(rkr->efx_Reverbtron->getpar(10) - 64);
                REVERBTRON->revtron_fb->redraw();
                break;
            case 344:
                REVERBTRON->revtron_length->value(rkr->efx_Reverbtron->getpar(3));
                REVERBTRON->revtron_length->redraw();
                break;
            case 345:
                REVERBTRON->revtron_strech->value(rkr->efx_Reverbtron->getpar(9) - 64);
                REVERBTRON->revtron_strech->redraw();
                break;
            case 346:
                REVERBTRON->revtron_idelay->value(rkr->efx_Reverbtron->getpar(5));
                REVERBTRON->revtron_idelay->redraw();
                break;
            case 347:
                REVERBTRON->revtron_fade->value(rkr->efx_Reverbtron->getpar(1));
                REVERBTRON->revtron_fade->redraw();
                break;
            case 348:
                ECHOTRON->echotron_WD->value(Dry_Wet(rkr->efx_Echotron->getpar(0)));
                ECHOTRON->echotron_WD->redraw();
                break;
            case 349:
                ECHOTRON->echotron_pan->value(rkr->efx_Echotron->getpar(11) - 64);
                ECHOTRON->echotron_pan->redraw();
                break;
            case 350:
                ECHOTRON->echotron_tempo->value(rkr->efx_Echotron->getpar(5));
                ECHOTRON->echotron_tempo->redraw();
                break;
            case 351:
                ECHOTRON->echotron_damp->value(rkr->efx_Echotron->getpar(6));
                ECHOTRON->echotron_damp->redraw();
                break;
            case 352:
                ECHOTRON->echotron_fb->value(rkr->efx_Echotron->getpar(10) - 64);
                ECHOTRON->echotron_fb->redraw();
                break;
            case 353:
                ECHOTRON->echotron_lrcross->value(rkr->efx_Echotron->getpar(7) - 64);
                ECHOTRON->echotron_lrcross->redraw();
                break;
            case 354:
                ECHOTRON->echotron_width->value(rkr->efx_Echotron->getpar(2));
                ECHOTRON->echotron_width->redraw();
                break;
            case 355:
                ECHOTRON->echotron_deep->value(rkr->efx_Echotron->getpar(1) - 64);
                ECHOTRON->echotron_deep->redraw();
                break;
            case 356:
                ECHOTRON->echotron_stdf->value(rkr->efx_Echotron->getpar(9));
                ECHOTRON->echotron_stdf->redraw();
                break;
            case 357:
                ECHOTRON->echotron_length->value(rkr->efx_Echotron->getpar(3));
                ECHOTRON->echotron_length->redraw();
                break;
            case 358:
                SHAR->shar_WD->value(Dry_Wet(rkr->efx_StereoHarm->getpar(0)));
                SHAR->shar_WD->redraw();
                break;
            case 359:
                SHAR->shar_intl->value(rkr->efx_StereoHarm->getpar(2));
                SHAR->shar_intl->redraw();
                break;
            case 360:
                SHAR->shar_chl->value(rkr->efx_StereoHarm->getpar(3));
                SHAR->shar_chl->redraw();
                break;
            case 361:
                SHAR->shar_ganl->value(rkr->efx_StereoHarm->getpar(1) - 64);
                SHAR->shar_ganl->redraw();
                break;
            case 362:
                SHAR->shar_intr->value(rkr->efx_StereoHarm->getpar(5));
                SHAR->shar_intr->redraw();
                break;
            case 363:
                SHAR->shar_chr->value(rkr->efx_StereoHarm->getpar(6));
                SHAR->shar_chr->redraw();
                break;
            case 364:
                SHAR->shar_ganr->value(rkr->efx_StereoHarm->getpar(4) - 64);
                SHAR->shar_ganr->redraw();
                break;
            case 365:
                SHAR->shar_lrc->value(rkr->efx_StereoHarm->getpar(11));
                SHAR->shar_lrc->redraw();
                break;
            case 366:
                SHAR->shar_note->value(rkr->efx_StereoHarm->getpar(8));
                SHAR->shar_note->redraw();
                Chord(1);   // update gui
                break;
            case 367:
                SHAR->shar_type->value(rkr->efx_StereoHarm->getpar(9));
                SHAR->shar_type->redraw();
                Chord(1);   // update gui
                break;
            case 368:
                COMPBAND->cband_WD->value(Dry_Wet(rkr->efx_CompBand->getpar(0)));
                COMPBAND->cband_WD->redraw();
                break;
            case 369:
                COMPBAND->cband_gain->value(rkr->efx_CompBand->getpar(12));
                COMPBAND->cband_gain->redraw();
                break;
            case 370:
                COMPBAND->cband_Lratio->value(rkr->efx_CompBand->getpar(1));
                COMPBAND->cband_Lratio->redraw();
                break;
            case 371:
                COMPBAND->cband_MLratio->value(rkr->efx_CompBand->getpar(2));
                COMPBAND->cband_MLratio->redraw();
                break;
            case 372:
                COMPBAND->cband_MHratio->value(rkr->efx_CompBand->getpar(3));
                COMPBAND->cband_MHratio->redraw();
                break;
            case 373:
                COMPBAND->cband_Hratio->value(rkr->efx_CompBand->getpar(4));
                COMPBAND->cband_Hratio->redraw();
                break;
            case 374:
                COMPBAND->cband_Lthres->value(rkr->efx_CompBand->getpar(5));
                COMPBAND->cband_Lthres->redraw();
                break;
            case 375:
                COMPBAND->cband_MLthres->value(rkr->efx_CompBand->getpar(6));
                COMPBAND->cband_MLthres->redraw();
                break;
            case 376:
                COMPBAND->cband_MHthres->value(rkr->efx_CompBand->getpar(7));
                COMPBAND->cband_MHthres->redraw();
                break;
            case 377:
                COMPBAND->cband_Hthres->value(rkr->efx_CompBand->getpar(8));
                COMPBAND->cband_Hthres->redraw();
                break;
            case 378:
                COMPBAND->cband_cross1->value(rkr->efx_CompBand->getpar(9));
                COMPBAND->cband_cross1->redraw();
                break;
            case 379:
                COMPBAND->cband_cross2->value(rkr->efx_CompBand->getpar(10));
                COMPBAND->cband_cross2->redraw();
                break;
            case 380:
                COMPBAND->cband_cross3->value(rkr->efx_CompBand->getpar(11));
                COMPBAND->cband_cross3->redraw();
                break;
            case 381:
                OTREM->otrem_dpth->value(rkr->efx_Opticaltrem->getpar(0));
                OTREM->otrem_dpth->redraw();
                break;
            case 382:
                OTREM->otrem_freq->value(rkr->efx_Opticaltrem->getpar(1));
                OTREM->otrem_freq->redraw();
                break;
            case 383:
                OTREM->otrem_rnd->value(rkr->efx_Opticaltrem->getpar(2));
                OTREM->otrem_rnd->redraw();
                break;
            case 384:
                OTREM->otrem_stdf->value(rkr->efx_Opticaltrem->getpar(4));
                OTREM->otrem_stdf->redraw();
                break;
            case 385:
                OTREM->otrem_pan->value(rkr->efx_Opticaltrem->getpar(5) - 64);
                OTREM->otrem_pan->redraw();
                break;
            case 386:
                VIBE->vibe_WD->value(Dry_Wet(rkr->efx_Vibe->getpar(6)));
                VIBE->vibe_WD->redraw();
                break;
            case 387:
                VIBE->vibe_width->value(rkr->efx_Vibe->getpar(0));
                VIBE->vibe_width->redraw();
                break;
            case 388:
                VIBE->vibe_dpth->value(rkr->efx_Vibe->getpar(8));
                VIBE->vibe_dpth->redraw();
                break;
            case 389:
                VIBE->vibe_freq->value(rkr->efx_Vibe->getpar(1));
                VIBE->vibe_freq->redraw();
                break;
            case 390:
                VIBE->vibe_rnd->value(rkr->efx_Vibe->getpar(2));
                VIBE->vibe_rnd->redraw();
                break;
            case 391:
                VIBE->vibe_stdf->value(rkr->efx_Vibe->getpar(4));
                VIBE->vibe_stdf->redraw();
                break;
            case 392:
                VIBE->vibe_fb->value(rkr->efx_Vibe->getpar(7) - 64);
                VIBE->vibe_fb->redraw();
                break;
            case 393:
                VIBE->vibe_LR->value(rkr->efx_Vibe->getpar(9));
                VIBE->vibe_LR->redraw();
                break;
            case 394:
                VIBE->vibe_pan->value(rkr->efx_Vibe->getpar(5) - 64);
                VIBE->vibe_pan->redraw();
                break;
            case 395:
                INFINIT->infinity_WD->value(Dry_Wet(rkr->efx_Infinity->getpar(0)));
                INFINIT->infinity_WD->redraw();
                break;
            case 396:
                INFINIT->infinity_Q->value(rkr->efx_Infinity->getpar(9));
                INFINIT->infinity_Q->redraw();
                break;
            case 397:
                INFINIT->infinity_pan->value(rkr->efx_Infinity->getpar(15));
                INFINIT->infinity_pan->redraw();
                break;
            case 398:
                INFINIT->infinity_stdf->value(rkr->efx_Infinity->getpar(13) - 64);
                INFINIT->infinity_stdf->redraw();
                break;
            case 399:
                INFINIT->infinity_start->value(rkr->efx_Infinity->getpar(10));
                INFINIT->infinity_start->redraw();
                break;
            case 400:
                INFINIT->infinity_end->value(rkr->efx_Infinity->getpar(11));
                INFINIT->infinity_end->redraw();
                break;
            case 401:
                INFINIT->infinity_rate->value(rkr->efx_Infinity->getpar(12));
                INFINIT->infinity_rate->redraw();
                break;
            case 402:
                INFINIT->infinity_subdiv->value(rkr->efx_Infinity->getpar(14));
                INFINIT->infinity_subdiv->redraw();
                break;
            case 403:
                ALIENWAH->Alienwah_delay->value(rkr->efx_Alienwah->getpar(8));
                ALIENWAH->Alienwah_delay->redraw();
                break;
            case 404:
                APHASER->aphaser_rnd->value(rkr->efx_APhaser->getpar(3));
                APHASER->aphaser_rnd->redraw();
                break;
            case 405:
                CABINET->Cabinet_output->value(rkr->efx_Cabinet->getpar(0)-64);
                CABINET->Cabinet_output->redraw();
                break;
            case 406:
                CHORUS->chorus_delay->value(rkr->efx_Chorus->getpar(7));
                CHORUS->chorus_delay->redraw();
                break;
            case 407:
                ECHO->echo_RV->value(rkr->efx_Echo->getpar(7));
                ECHO->echo_RV->redraw();
                break;
            case 408:
                ECHO->echo_delay->value(rkr->efx_Echo->getpar(2));
                ECHO->echo_delay->redraw();
                break;
            case 409:
                ECHO->echo_LRdl->value(rkr->efx_Echo->getpar(3));
                ECHO->echo_LRdl->redraw();
                break;
            case 410:
                ECHO->echo_damp->value(rkr->efx_Echo->getpar(6));
                ECHO->echo_damp->redraw();
                break;
            case 411:
                FLANGER->flanger_delay->value(rkr->efx_Flanger->getpar(7));
                FLANGER->flanger_delay->redraw();
                break;
            case 412:
                HAR->har_gan->value(rkr->efx_Har->getpar(2) - 64);
                HAR->har_gan->redraw();
                break;
            case 413:
                HAR->har_gan1->value(rkr->efx_Har->getpar(8) - 64);
                HAR->har_gan1->redraw();
                break;
            case 414:
                HAR->har_q1->value(rkr->efx_Har->getpar(9) - 64);
                HAR->har_q1->redraw();
                break;
            case 415:
                INFINIT->infinity_1->value(rkr->efx_Infinity->getpar(1) - 64);
                INFINIT->infinity_1->redraw();
                break;
            case 416:
                INFINIT->infinity_2->value(rkr->efx_Infinity->getpar(2) - 64);
                INFINIT->infinity_2->redraw();
                break;
            case 417:
                INFINIT->infinity_3->value(rkr->efx_Infinity->getpar(3) - 64);
                INFINIT->infinity_3->redraw();
                break;
            case 418:
                INFINIT->infinity_4->value(rkr->efx_Infinity->getpar(4) - 64);
                INFINIT->infinity_4->redraw();
                break;
            case 419:
                INFINIT->infinity_5->value(rkr->efx_Infinity->getpar(5) - 64);
                INFINIT->infinity_5->redraw();
                break;
            case 420:
                INFINIT->infinity_6->value(rkr->efx_Infinity->getpar(6) - 64);
                INFINIT->infinity_6->redraw();
                break;
            case 421:
                INFINIT->infinity_7->value(rkr->efx_Infinity->getpar(7) - 64);
                INFINIT->infinity_7->redraw();
                break;
            case 422:
                INFINIT->infinity_8->value(rkr->efx_Infinity->getpar(8) - 64);
                INFINIT->infinity_8->redraw();
                break;
            case 423:
                LOOPER->looper_Tempo->value(rkr->efx_Looper->getpar(14));
                LOOPER->looper_Tempo->redraw();
                break;
            case 424:
                MUSDELAY->musdelay_tempo->value(rkr->efx_MusDelay->getpar(10));
                MUSDELAY->musdelay_tempo->redraw();
                break;
            case 425:
                MUSDELAY->musdelay_damp->value(rkr->efx_MusDelay->getpar(6));
                MUSDELAY->musdelay_damp->redraw();
                break;
            case 426:
                MUTROMOJO->mutromojo_Rnd->value(rkr->efx_MuTroMojo->getpar(3));
                MUTROMOJO->mutromojo_Rnd->redraw();
                break;
            case 427:
                MUTROMOJO->mutromojo_St->value(rkr->efx_MuTroMojo->getpar(5) - 64);
                MUTROMOJO->mutromojo_St->redraw();
                break;
            case 428:
                MUTROMOJO->mutromojo_minfreq->value(rkr->efx_MuTroMojo->getpar(15));
                MUTROMOJO->mutromojo_minfreq->redraw();
                break;
            case 429:
                GATE->gate_ATime->value(rkr->efx_Gate->getpar(3));
                GATE->gate_ATime->redraw();
                break;
            case 430:
                GATE->gate_RTime->value(rkr->efx_Gate->getpar(4));
                GATE->gate_RTime->redraw();
                break;
            case 431:
                GATE->gate_range->value(rkr->efx_Gate->getpar(2));
                GATE->gate_range->redraw();
                break;
            case 432:
                GATE->gate_threshold->value(rkr->efx_Gate->getpar(1));
                GATE->gate_threshold->redraw();
                break;
            case 433:
                GATE->gate_hold->value(rkr->efx_Gate->getpar(7));
                GATE->gate_hold->redraw();
                break;
            case 434:
                GATE->gate_LPF->value(rkr->efx_Gate->getpar(5));
                GATE->gate_LPF->redraw();
                break;
            case 435:
                GATE->gate_HPF->value(rkr->efx_Gate->getpar(6));
                GATE->gate_HPF->redraw();
                break;
            case 436:
                PAN->pan_extra->value(rkr->efx_Pan->getpar(6));
                PAN->pan_extra->redraw();
                break;
            case 437:
                REVERB->reverb_time->value(rkr->efx_Rev->getpar(2));
                REVERB->reverb_time->redraw();
                break;
            case 438:
                REVERB->reverb_ldel->value(rkr->efx_Rev->getpar(3));
                REVERB->reverb_ldel->redraw();
                break;
            case 439:
                REVERB->reverb_ldelft->value(rkr->efx_Rev->getpar(4));
                REVERB->reverb_ldelft->redraw();
                break;
            case 440:
                REVERB->reverb_RS->value(rkr->efx_Rev->getpar(11));
                REVERB->reverb_RS->redraw();
                break;
            case 441:
                REVERB->reverb_damp->value(rkr->efx_Rev->getpar(9));
                REVERB->reverb_damp->redraw();
                break;
            case 442:
                REVERBTRON->revtron_diff->value(rkr->efx_Reverbtron->getpar(15));
                REVERBTRON->revtron_diff->redraw();
                break;
            case 443:
                REVERBTRON->revtron_LPF->value(rkr->efx_Reverbtron->getpar(14));
                REVERBTRON->revtron_LPF->redraw();
                break;
            case 444:
                HAR->har_note->value(rkr->efx_Har->getpar(6));
                HAR->har_note->redraw();
                Chord(0);   // update gui
                break;
            case 445:
                HAR->har_type->value(rkr->efx_Har->getpar(7));
                HAR->har_type->redraw();
                Chord(0);   // update gui
                break;
            case 446:
                SYNTHFILTER->synthfilter_rand->value(rkr->efx_Synthfilter->getpar(3));
                SYNTHFILTER->synthfilter_rand->redraw();
                break;
            case 447:
                HAR->har_SELECT->value(rkr->efx_Har->getpar(5));
                HAR->har_SELECT->redraw();
                break;
            case 448:
                SHAR->shar_SELECT->value(rkr->efx_StereoHarm->getpar(7));
                SHAR->shar_SELECT->redraw();
                break;
            case 449:
                DIST->dist_tipo->value(rkr->efx_Distorsion->getpar(5));
                DIST->dist_tipo->redraw();
                break;
            case 450:
                OVRD->ovrd_tipo->value(rkr->efx_Overdrive->getpar(5));
                OVRD->ovrd_tipo->redraw();
                break;
            case 451:
                DERELICT->derelict_tipo->value(rkr->efx_Derelict->getpar(5));
                DERELICT->derelict_tipo->redraw();
                break;
            case 452:
                DISTBAND->distband_tipoL->value(rkr->efx_DistBand->getpar(5));
                DISTBAND->distband_tipoL->redraw();
                break;
            case 453:
                DISTBAND->distband_tipoM->value(rkr->efx_DistBand->getpar(6));
                DISTBAND->distband_tipoM->redraw();
                break;
            case 454:
                DISTBAND->distband_tipoH->value(rkr->efx_DistBand->getpar(7));
                DISTBAND->distband_tipoH->redraw();
                break;
            case 455:
                STOMPBOX->stomp_mode->value(rkr->efx_StompBox->getpar(5));
                STOMPBOX->stomp_mode->redraw();
                break;
        }
    }


    rkr->Mvalue = 0;
}

void RKRGUI::ActOnOff()
{
    // turn efx on or off
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
        else
        {
            miralo = rkr->efx_order[rkr->Mnumeff[rkr->OnOffC]];
        }

        switch (miralo)
        {
            case 0:
                EQ->eq_activar->value(rkr->EQ1_Bypass);
                EQ->eq_activar->do_callback();
                break;
            case 1:
                COMPRESS->compress_activar->value(rkr->Compressor_Bypass);
                COMPRESS->compress_activar->do_callback();
                break;
            case 2:
                DIST->dist_activar->value(rkr->Distorsion_Bypass);
                DIST->dist_activar->do_callback();
                break;
            case 3:
                OVRD->ovrd_activar->value(rkr->Overdrive_Bypass);
                OVRD->ovrd_activar->do_callback();
                break;
            case 4:
                ECHO->echo_activar->value(rkr->Echo_Bypass);
                ECHO->echo_activar->do_callback();
                break;
            case 5:
                CHORUS->chorus_activar->value(rkr->Chorus_Bypass);
                CHORUS->chorus_activar->do_callback();
                break;
            case 6:
                PHASER->phaser_activar->value(rkr->Phaser_Bypass);
                PHASER->phaser_activar->do_callback();
                break;
            case 7:
                FLANGER->flanger_activar->value(rkr->Flanger_Bypass);
                FLANGER->flanger_activar->do_callback();
                break;
            case 8:
                REVERB->reverb_activar->value(rkr->Reverb_Bypass);
                REVERB->reverb_activar->do_callback();
                break;
            case 9:
                PEQ->eqp_activar->value(rkr->EQ2_Bypass);
                PEQ->eqp_activar->do_callback();
                break;
            case 10:
                WHAWHA->WhaWha_activar->value(rkr->WhaWha_Bypass);
                WHAWHA->WhaWha_activar->do_callback();
                break;
            case 11:
                ALIENWAH->Alienwah_activar->value(rkr->Alienwah_Bypass);
                ALIENWAH->Alienwah_activar->do_callback();
                break;
            case 12:
                CABINET->Cabinet_activar->value(rkr->Cabinet_Bypass);
                CABINET->Cabinet_activar->do_callback();
                break;
            case 13:
                PAN->pan_activar->value(rkr->Pan_Bypass);
                PAN->pan_activar->do_callback();
                break;
            case 14:
                HAR->har_activar->value(rkr->Harmonizer_Bypass);
                HAR->har_activar->do_callback();
                break;
            case 15:
                MUSDELAY->musdelay_activar->value(rkr->MusDelay_Bypass);
                MUSDELAY->musdelay_activar->do_callback();
                break;
            case 16:
                GATE->gate_activar->value(rkr->Gate_Bypass);
                GATE->gate_activar->do_callback();
                break;
            case 17:
                DERELICT->derelict_activar->value(rkr->Derelict_Bypass);
                DERELICT->derelict_activar->do_callback();
                break;
            case 18:
                APHASER->aphaser_activar->value(rkr->APhaser_Bypass);
                APHASER->aphaser_activar->do_callback();
                break;
            case 19:
                VALVE->valve_activar->value(rkr->Valve_Bypass);
                VALVE->valve_activar->do_callback();
                break;
            case 20:
                DFLANGE->dflange_activar->value(rkr->DFlange_Bypass);
                DFLANGE->dflange_activar->do_callback();
                break;
            case 21:
                RING->ring_activar->value(rkr->Ring_Bypass);
                RING->ring_activar->do_callback();
                break;
            case 22:
                EXCITER->exciter_activar->value(rkr->Exciter_Bypass);
                EXCITER->exciter_activar->do_callback();
                break;
            case 23:
                DISTBAND->distband_activar->value(rkr->DistBand_Bypass);
                DISTBAND->distband_activar->do_callback();
                break;
            case 24:
                ARPIE->arpie_activar->value(rkr->Arpie_Bypass);
                ARPIE->arpie_activar->do_callback();
                break;
            case 25:
                EXPANDER->expander_activar->value(rkr->Expander_Bypass);
                EXPANDER->expander_activar->do_callback();
                break;
            case 26:
                SHUFFLE->shuffle_activar->value(rkr->Shuffle_Bypass);
                SHUFFLE->shuffle_activar->do_callback();
                break;
            case 27:
                SYNTHFILTER->synthfilter_activar->value(rkr->Synthfilter_Bypass);
                SYNTHFILTER->synthfilter_activar->do_callback();
                break;
            case 28:
                VARYBAND->varyband_activar->value(rkr->VaryBand_Bypass);
                VARYBAND->varyband_activar->do_callback();
                break;
            case 29:
                CONVOLOTRON->convo_activar->value(rkr->Convol_Bypass);
                CONVOLOTRON->convo_activar->do_callback();
                break;
            case 30:
                LOOPER->looper_activar->value(rkr->Looper_Bypass);
                LOOPER->looper_activar->do_callback();
                break;
            case 31:
                MUTROMOJO->mutromojo_activar->value(rkr->MuTroMojo_Bypass);
                MUTROMOJO->mutromojo_activar->do_callback();
                break;
            case 32:
                ECHOVERSE->echoverse_activar->value(rkr->Echoverse_Bypass);
                ECHOVERSE->echoverse_activar->do_callback();
                break;
            case 33:
                COILCRAFTER->coil_activar->value(rkr->CoilCrafter_Bypass);
                COILCRAFTER->coil_activar->do_callback();
                break;
            case 34:
                SHELFBOOST->shelf_activar->value(rkr->ShelfBoost_Bypass);
                SHELFBOOST->shelf_activar->do_callback();
                break;
            case 35:
                VOCODER->vo_activar->value(rkr->Vocoder_Bypass);
                VOCODER->vo_activar->do_callback();
                break;
            case 36:
                SUSTAINER->sus_activar->value(rkr->Sustainer_Bypass);
                SUSTAINER->sus_activar->do_callback();
                break;
            case 37:
                SEQUENCE->seq_activar->value(rkr->Sequence_Bypass);
                SEQUENCE->seq_activar->do_callback();
                break;
            case 38:
                SHIFTER->shifter_activar->value(rkr->Shifter_Bypass);
                SHIFTER->shifter_activar->do_callback();
                break;
            case 39:
                STOMPBOX->stomp_activar->value(rkr->StompBox_Bypass);
                STOMPBOX->stomp_activar->do_callback();
                break;
            case 40:
                REVERBTRON->revtron_activar->value(rkr->Reverbtron_Bypass);
                REVERBTRON->revtron_activar->do_callback();
                break;
            case 41:
                ECHOTRON->echotron_activar->value(rkr->Echotron_Bypass);
                ECHOTRON->echotron_activar->do_callback();
                break;
            case 42:
                SHAR->shar_activar->value(rkr->StereoHarm_Bypass);
                SHAR->shar_activar->do_callback();
                break;
            case 43:
                COMPBAND->cband_activar->value(rkr->CompBand_Bypass);
                COMPBAND->cband_activar->do_callback();
                break;
            case 44:
                OTREM->otrem_activar->value(rkr->Opticaltrem_Bypass);
                OTREM->otrem_activar->do_callback();
                break;
            case 45:
                VIBE->vibe_activar->value(rkr->Vibe_Bypass);
                VIBE->vibe_activar->do_callback();
                break;
            case 46:
                INFINIT->infinity_activar->value(rkr->Infinity_Bypass);
                INFINIT->infinity_activar->do_callback();
                break;

            case 121:
                Tap_activar->value(rkr->Tap_Bypass);
                Tap_activar->do_callback();
                break;
            case 122:
                MIDI->midi_activar->value(rkr->MIDIConverter_Bypass);
                MIDI->midi_activar->do_callback();
                break;
            case 123:
                tuner_activar->value(rkr->Tuner_Bypass);
                tuner_activar->do_callback();
                break;
            case 124:
                ActivarGeneral->value(rkr->Bypass);
                ActivarGeneral->do_callback();
                break;
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
    EQ->image(InOut->image());
    COMPRESS->image(InOut->image());
    DIST->image(InOut->image());
    OVRD->image(InOut->image());
    ECHO->image(InOut->image());
    CHORUS->image(InOut->image());
    FLANGER->image(InOut->image());
    WHAWHA->image(InOut->image());
    ALIENWAH->image(InOut->image());
    PHASER->image(InOut->image());
    REVERB->image(InOut->image());
    PEQ->image(InOut->image());
    CABINET->image(InOut->image());
    PAN->image(InOut->image());
    HAR->image(InOut->image());
    MUSDELAY->image(InOut->image());
    GATE->image(InOut->image());
    DERELICT->image(InOut->image());
    APHASER->image(InOut->image());
    VALVE->image(InOut->image());
    DFLANGE->image(InOut->image());
    RING->image(InOut->image());
    EXCITER->image(InOut->image());
    DISTBAND->image(InOut->image());
    ARPIE->image(InOut->image());
    EXPANDER->image(InOut->image());
    SHUFFLE->image(InOut->image());
    SYNTHFILTER->image(InOut->image());
    VARYBAND->image(InOut->image());
    CONVOLOTRON->image(InOut->image());
    LOOPER->image(InOut->image());
    MUTROMOJO->image(InOut->image());
    ECHOVERSE->image(InOut->image());
    COILCRAFTER->image(InOut->image());
    SHELFBOOST->image(InOut->image());
    VOCODER->image(InOut->image());
    SUSTAINER->image(InOut->image());
    SEQUENCE->image(InOut->image());
    SHIFTER->image(InOut->image());
    STOMPBOX->image(InOut->image());
    REVERBTRON->image(InOut->image());
    ECHOTRON->image(InOut->image());
    SHAR->image(InOut->image());
    COMPBAND->image(InOut->image());
    OTREM->image(InOut->image());
    VIBE->image(InOut->image());
    INFINIT->image(InOut->image());

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

    for (i = 0; i < 10; i++)
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


    save_stat(3);
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
            for (int i = 0; i < rkr->NumParams; i++)
                MIDILearn->Epar->add(rkr->efx_params[i].Nom);
            break;

        case 1:
            for (int i = 0; i < rkr->NumParams; i++)
            {
                if (rkr->efx_params[i].Effect == 50)
                {
                    MIDILearn->Epar->add(rkr->efx_params[i].Nom);
                    rkr->ML_clist[k] = rkr->efx_params[i].Ato;
                    k++;
                }
            }

            for (int j = 0; j < 10; j++)
            {
                for (int i = 0; i < rkr->NumParams; i++)
                {
                    if (rkr->efx_params[i].Effect == rkr->efx_order[j])
                    {
                        MIDILearn->Epar->add(rkr->efx_params[i].Nom);
                        rkr->ML_clist[k] = rkr->efx_params[i].Ato;
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

void RKRGUI::DisAssigns()
{
    //DisAssigns
    char tmp[8];

    int the_one = 0;

    if ((int) MIDILearn->Epar->value())
    {
        if (rkr->ML_filter == 0)
        {
            the_one = rkr->efx_params[(int) MIDILearn->Epar->value() - 1].Ato;
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

    for (int i = 0; i < 10; i++)
    {
        rkr->new_order[i] = rkr->efx_order[i];
        rkr->saved_order[i] = rkr->efx_order[i];
        Order->Order_Bro->add(rkr->efx_names[Busca_Eff(rkr->efx_order[i])].Nom);
    }

    Fill_Avail(rkr->eff_filter);

    Order->Order_Bro->select(1);
}

void RKRGUI::Show_Next_Time()
{
    // popup for settings changes that will not take effect until restart
    if (rkr->mess_dis)
        return;

    Fl_Widget *w = fl_message_icon();
    w->parent()->copy_label(rkr->jackcliname);
    fl_message("This setting will be changed the next time you run rakarrack-plus");
}

void RKRGUI::update_looper()
{
    // update looper
    rkr->efx_Looper->getstate();

    LOOPER->looper_play->value(rkr->efx_Looper->progstate[0]);
    LOOPER->looper_play->redraw();
    LOOPER->looper_record->value(rkr->efx_Looper->progstate[2]);
    LOOPER->looper_record->redraw();
    LOOPER->looper_stop->value(rkr->efx_Looper->progstate[1]);
    LOOPER->looper_stop->redraw();

    if (rkr->efx_Looper->progstate[2])
    {
        LOOPER->Box_P->copy_label("Stop");
        LOOPER->looper_stop->copy_label("@square");
    }
    else
    {
        LOOPER->Box_P->copy_label("Pause");
        LOOPER->looper_stop->copy_label("@||");
    }

    LOOPER->looper_t1->value(rkr->efx_Looper->progstate[4]);
    LOOPER->looper_t1->redraw();
    LOOPER->looper_t2->value(rkr->efx_Looper->progstate[5]);
    LOOPER->looper_t2->redraw();
}

void RKRGUI::UpdateTGUI()
{
    // updates the efx based on global tempo settings
    if (rkr->Looper_Bypass)
    {
        LOOPER->looper_Tempo->value(rkr->efx_Looper->getpar(14));
        LOOPER->looper_Tempo->redraw();
    }

    if (rkr->Chorus_Bypass)
    {
        CHORUS->chorus_freq->value(rkr->efx_Chorus->getpar(2));
        CHORUS->chorus_freq->redraw();
    }

    if (rkr->Flanger_Bypass)
    {
        FLANGER->flanger_freq->value(rkr->efx_Flanger->getpar(2));
        FLANGER->flanger_freq->redraw();
    }

    if (rkr->Phaser_Bypass)
    {
        PHASER->phaser_freq->value(rkr->efx_Phaser->getpar(2));
        PHASER->phaser_freq->redraw();
    }

    if (rkr->Pan_Bypass)
    {
        PAN->pan_freq->value(rkr->efx_Pan->getpar(2));
        PAN->pan_freq->redraw();
    }

    if (rkr->WhaWha_Bypass)
    {
        WHAWHA->WhaWha_freq->value(rkr->efx_WhaWha->getpar(2));
        WHAWHA->WhaWha_freq->redraw();
    }

    if (rkr->Alienwah_Bypass)
    {
        ALIENWAH->Alienwah_freq->value(rkr->efx_Alienwah->getpar(2));
        ALIENWAH->Alienwah_freq->redraw();
    }

    if (rkr->MusDelay_Bypass)
    {
        MUSDELAY->musdelay_tempo->value(rkr->efx_MusDelay->getpar(10));
        MUSDELAY->musdelay_tempo->redraw();
    }

    if (rkr->APhaser_Bypass)
    {
        APHASER->aphaser_freq->value(rkr->efx_APhaser->getpar(2));
        APHASER->aphaser_freq->redraw();
    }

    if (rkr->DFlange_Bypass)
    {
        DFLANGE->dflange_freq->value(rkr->efx_DFlange->getpar(10));
        DFLANGE->dflange_freq->redraw();
    }

    if (rkr->Synthfilter_Bypass)
    {
        SYNTHFILTER->synthfilter_freq->value(rkr->efx_Synthfilter->getpar(2));
        SYNTHFILTER->synthfilter_freq->redraw();
    }

    if (rkr->MuTroMojo_Bypass)
    {
        MUTROMOJO->mutromojo_freq->value(rkr->efx_MuTroMojo->getpar(2));
        MUTROMOJO->mutromojo_freq->redraw();
    }

    if (rkr->VaryBand_Bypass)
    {
        VARYBAND->varyband_freq1->value(rkr->efx_VaryBand->getpar(1));
        VARYBAND->varyband_freq1->redraw();
    }

    if (rkr->VaryBand_Bypass)
    {
        VARYBAND->varyband_freq2->value(rkr->efx_VaryBand->getpar(4));
        VARYBAND->varyband_freq2->redraw();
    }

    if (rkr->Arpie_Bypass)
    {
        ARPIE->arpie_delay->value(rkr->efx_Arpie->getpar(2));
        ARPIE->arpie_delay->redraw();
    }

    if (rkr->Echoverse_Bypass)
    {
        ECHOVERSE->echoverse_delay->value(rkr->efx_Echoverse->getpar(2));
        ECHOVERSE->echoverse_delay->redraw();
    }
    if (rkr->Sequence_Bypass)
    {
        SEQUENCE->seq_tempo->value(rkr->efx_Sequence->getpar(9));
        SEQUENCE->seq_tempo->redraw();
    }
    if (rkr->Echo_Bypass)
    {
        ECHO->echo_delay->value(rkr->efx_Echo->getpar(2));
        ECHO->echo_delay->redraw();
    }
    if (rkr->Echotron_Bypass)
    {
        ECHOTRON->echotron_tempo->value(rkr->efx_Echotron->getpar(5));
        ECHOTRON->echotron_tempo->redraw();
    }
    if (rkr->Opticaltrem_Bypass)
    {
        OTREM->otrem_freq->value(rkr->efx_Opticaltrem->getpar(1));
        OTREM->otrem_freq->redraw();
    }
    if (rkr->Vibe_Bypass)
    {
        VIBE->vibe_freq->value(rkr->efx_Vibe->getpar(1));
        VIBE->vibe_freq->redraw();
    }
    if (rkr->Infinity_Bypass)
    {
        INFINIT->infinity_rate->value(rkr->efx_Infinity->getpar(12));
        INFINIT->infinity_rate->redraw();
    }
}

void RKRGUI::ActACI()
{
    //ActACI
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

    for (i = 0; i < rkr->NumEffects; i++)
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

    for (int i = 0; i < rkr->NumEffects; i++)
    {
        int k = 0;
        for (int j = 0; j < 10; j++)
        {
            if (rkr->new_order[j] == rkr->efx_names[i].Pos)
            {
                k = 1;
            }
        }

        if (!k)
        {
            if ((rkr->efx_names[i].Type & rkr->eff_filter) == filter)
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
        return;

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
            stecla = 1;
            return 1;
        }

        if ((Fl::event_key(45)) || (Fl::event_key(FL_KP + 45))) // -(minus) key
        {
            stecla = 2;
            return 1;
        }

        if (Fl::event_key(FL_F + 2)) // F2 key - decrease output volume
        {
            stecla = 3;
            return 1;
        }

        if (Fl::event_key(FL_F + 3)) // F3 key - increase output volume
        {
            stecla = 4;
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
        if (filetoload != NULL) // individual preset
        {
            printf("Saving file: %s\n", filetoload);
            got_sigusr1 = 0;
            rkr->savefile(filetoload);
        }
        return;
    }

    if (got_sigint == SIGINT)
    {
        printf("Got SIGTERM, quitting...\n");
        got_sigint = 0;
        Pexitprogram = 1;
    }
}

/**
 *  This pops up the midi learn window and sets the item (num) to be set with 
 *  midi learn. It is triggered by right mouse click on any efx parameter (slider, etc)
 *  that has midi learn capability.
 * 
 * @param num
 *  The efx parameter number that is to be used by midi learn.
 */
void RKRGUI::getMIDIControl(int num)
{
    /* Don't pop up the midi learn window unless the user checked the box
     * in Settings/Preferences/MIDI/Midi Learn - Really confusing when
     * it pops up but nothing happens when set. The menu item for Midi Learn
     * is set inactive unless the box is checked as well. For consistency here */
    if (!rkr->MIDIway)
        return;

    int i = 0;
    rkr->comemouse = 1;
    PrepareML();
    while (i < MIDILearn->Epar->size())
    {
        if ((rkr->ML_filter == 0) && (rkr->efx_params[i].Ato == num))
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
    rkr->SaveIntPreset(num, NewName);
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
    rkr->DelIntPreset(num, Rname);
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

    SelEff[0] = (int) (RND * rkr->NumEffects);

    for (int i = 1; i < 10; i++)
    {
        int l = 0;
        while (l == 0)
        {
            SelEff[i] = (int) (RND * rkr->NumEffects);
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


    for (int i = 0; i < 10; i++)
    {
        rkr->efx_order[i] = SelEff[i];
    }


    for (int i = 0; i < 10; i++)
    {
        switch (rkr->efx_order[i])
        {

            case 0: //EQ1
                if (i < numEff)
                    rkr->EQ1_Bypass = 1;
                else
                    rkr->EQ1_Bypass = 0;
                EQ->eq_activar->value(rkr->EQ1_Bypass);
                break;

            case 1://Compressor
                if (i < numEff)
                    rkr->Compressor_Bypass = 1;
                else
                    rkr->Compressor_Bypass = 0;
                COMPRESS->compress_activar->value(rkr->Compressor_Bypass);
                break;

            case 2://Distorsion
                if (i < numEff)
                    rkr->Distorsion_Bypass = 1;
                else
                    rkr->Distorsion_Bypass = 0;
                DIST->dist_activar->value(rkr->Distorsion_Bypass);
                break;

            case 3://Overdrive
                if (i < numEff)
                    rkr->Overdrive_Bypass = 1;
                else
                    rkr->Overdrive_Bypass = 0;
                OVRD->ovrd_activar->value(rkr->Overdrive_Bypass);
                break;

            case 4://Echo
                if (i < numEff)
                    rkr->Echo_Bypass = 1;
                else
                    rkr->Echo_Bypass = 0;
                ECHO->echo_activar->value(rkr->Echo_Bypass);
                break;

            case 5://Chorus
                if (i < numEff)
                    rkr->Chorus_Bypass = 1;
                else
                    rkr->Chorus_Bypass = 0;
                CHORUS->chorus_activar->value(rkr->Chorus_Bypass);
                break;

            case 6://Phaser
                if (i < numEff)
                    rkr->Phaser_Bypass = 1;
                else
                    rkr->Phaser_Bypass = 0;
                PHASER->phaser_activar->value(rkr->Phaser_Bypass);
                break;

            case 7://Flanger
                if (i < numEff)
                    rkr->Flanger_Bypass = 1;
                else
                    rkr->Flanger_Bypass = 0;
                FLANGER->flanger_activar->value(rkr->Flanger_Bypass);
                break;

            case 8://Reverb
                if (i < numEff)
                    rkr->Reverb_Bypass = 1;
                else
                    rkr->Reverb_Bypass = 0;
                REVERB->reverb_activar->value(rkr->Reverb_Bypass);
                break;

            case 9://EQ2
                if (i < numEff)
                    rkr->EQ2_Bypass = 1;
                else
                    rkr->EQ2_Bypass = 0;
                PEQ->eqp_activar->value(rkr->EQ2_Bypass);
                break;

            case 10://WahWah
                if (i < numEff)
                    rkr->WhaWha_Bypass = 1;
                else
                    rkr->WhaWha_Bypass = 0;
                WHAWHA->WhaWha_activar->value(rkr->WhaWha_Bypass);
                break;

            case 11://AlienWah
                if (i < numEff)
                    rkr->Alienwah_Bypass = 1;
                else
                    rkr->Alienwah_Bypass = 0;
                ALIENWAH->Alienwah_activar->value(rkr->Alienwah_Bypass);
                break;

            case 12://Cabinet
                if (i < numEff)
                    rkr->Cabinet_Bypass = 1;
                else
                    rkr->Cabinet_Bypass = 0;
                CABINET->Cabinet_activar->value(rkr->Cabinet_Bypass);
                break;

            case 13://Pan
                if (i < numEff)
                    rkr->Pan_Bypass = 1;
                else
                    rkr->Pan_Bypass = 0;
                PAN->pan_activar->value(rkr->Pan_Bypass);
                break;

            case 14://Har
                if (i < numEff)
                    rkr->Harmonizer_Bypass = 1;
                else
                    rkr->Harmonizer_Bypass = 0;
                HAR->har_activar->value(rkr->Harmonizer_Bypass);
                break;

            case 15://MsicalDelay
                if (i < numEff)
                    rkr->MusDelay_Bypass = 1;
                else
                    rkr->MusDelay_Bypass = 0;
                MUSDELAY->musdelay_activar->value(rkr->MusDelay_Bypass);
                break;

            case 16://NoiseGate
                if (i < numEff)
                    rkr->Gate_Bypass = 1;
                else
                    rkr->Gate_Bypass = 0;
                GATE->gate_activar->value(rkr->Gate_Bypass);
                break;

            case 17://Derelict
                if (i < numEff)
                    rkr->Derelict_Bypass = 1;
                else
                    rkr->Derelict_Bypass = 0;
                DERELICT->derelict_activar->value(rkr->Derelict_Bypass);
                break;

            case 18://APhaser
                if (i < numEff)
                    rkr->APhaser_Bypass = 1;
                else
                    rkr->APhaser_Bypass = 0;
                APHASER->aphaser_activar->value(rkr->APhaser_Bypass);
                break;

            case 19://Valve
                if (i < numEff)
                    rkr->Valve_Bypass = 1;
                else
                    rkr->Valve_Bypass = 0;
                VALVE->valve_activar->value(rkr->Valve_Bypass);
                break;

            case 20://Dual_Flange
                if (i < numEff)
                    rkr->DFlange_Bypass = 1;
                else
                    rkr->DFlange_Bypass = 0;
                DFLANGE->dflange_activar->value(rkr->DFlange_Bypass);
                break;

            case 21:// Ring
                if (i < numEff)
                    rkr->Ring_Bypass = 1;
                else
                    rkr->Ring_Bypass = 0;
                RING->ring_activar->value(rkr->Ring_Bypass);
                break;

            case 22:// Exciter
                if (i < numEff)
                    rkr->Exciter_Bypass = 1;
                else
                    rkr->Exciter_Bypass = 0;
                EXCITER->exciter_activar->value(rkr->Exciter_Bypass);
                break;

            case 23://DistBand
                if (i < numEff)
                    rkr->DistBand_Bypass = 1;
                else
                    rkr->DistBand_Bypass = 0;
                DISTBAND->distband_activar->value(rkr->DistBand_Bypass);
                break;

            case 24://Arpie
                if (i < numEff)
                    rkr->Arpie_Bypass = 1;
                else
                    rkr->Arpie_Bypass = 0;
                ARPIE->arpie_activar->value(rkr->Arpie_Bypass);
                break;

            case 25://Expander
                if (i < numEff)
                    rkr->Expander_Bypass = 1;
                else
                    rkr->Expander_Bypass = 0;
                EXPANDER->expander_activar->value(rkr->Expander_Bypass);
                break;

            case 26://Shuffle
                if (i < numEff)
                    rkr->Shuffle_Bypass = 1;
                else
                    rkr->Shuffle_Bypass = 0;
                SHUFFLE->shuffle_activar->value(rkr->Shuffle_Bypass);
                break;

            case 27://Synthfilter
                if (i < numEff)
                    rkr->Synthfilter_Bypass = 1;
                else
                    rkr->Synthfilter_Bypass = 0;
                SYNTHFILTER->synthfilter_activar->value(rkr->Synthfilter_Bypass);
                break;

            case 28://VaryBand
                if (i < numEff)
                    rkr->VaryBand_Bypass = 1;
                else
                    rkr->VaryBand_Bypass = 0;
                VARYBAND->varyband_activar->value(rkr->VaryBand_Bypass);
                break;

            case 29://Convolotron
                if (i < numEff)
                    rkr->Convol_Bypass = 1;
                else
                    rkr->Convol_Bypass = 0;
                CONVOLOTRON->convo_activar->value(rkr->Convol_Bypass);
                break;

            case 30://Looper
                if (i < numEff)
                    rkr->Looper_Bypass = 1;
                else
                    rkr->Looper_Bypass = 0;
                LOOPER->looper_activar->value(rkr->Looper_Bypass);
                update_looper();
                break;

            case 31://MuTroMojo
                if (i < numEff)
                    rkr->MuTroMojo_Bypass = 1;
                else
                    rkr->MuTroMojo_Bypass = 0;
                MUTROMOJO->mutromojo_activar->value(rkr->MuTroMojo_Bypass);
                break;

            case 32:// Echoverse
                if (i < numEff)
                    rkr->Echoverse_Bypass = 1;
                else
                    rkr->Echoverse_Bypass = 0;
                ECHOVERSE->echoverse_activar->value(rkr->Echoverse_Bypass);
                break;

            case 33:// CoilCrafter
                if (i < numEff)
                    rkr->CoilCrafter_Bypass = 1;
                else
                    rkr->CoilCrafter_Bypass = 0;
                COILCRAFTER->coil_activar->value(rkr->CoilCrafter_Bypass);
                break;

            case 34://ShelfBoost
                if (i < numEff)
                    rkr->ShelfBoost_Bypass = 1;
                else
                    rkr->ShelfBoost_Bypass = 0;
                SHELFBOOST->shelf_activar->value(rkr->ShelfBoost_Bypass);
                break;

            case 35://Vocoder
                if (i < numEff)
                    rkr->Vocoder_Bypass = 1;
                else
                    rkr->Vocoder_Bypass = 0;
                VOCODER->vo_activar->value(rkr->Vocoder_Bypass);
                break;

            case 36://Sustainer
                if (i < numEff)
                    rkr->Sustainer_Bypass = 1;
                else
                    rkr->Sustainer_Bypass = 0;
                SUSTAINER->sus_activar->value(rkr->Sustainer_Bypass);
                break;

            case 37://Sequence
                if (i < numEff)
                    rkr->Sequence_Bypass = 1;
                else
                    rkr->Sequence_Bypass = 0;
                SEQUENCE->seq_activar->value(rkr->Sequence_Bypass);
                break;

            case 38://Shifter
                if (i < numEff)
                    rkr->Shifter_Bypass = 1;
                else
                    rkr->Shifter_Bypass = 0;
                SHIFTER->shifter_activar->value(rkr->Shifter_Bypass);
                break;

            case 39://StompBox
                if (i < numEff)
                    rkr->StompBox_Bypass = 1;
                else
                    rkr->StompBox_Bypass = 0;
                STOMPBOX->stomp_activar->value(rkr->StompBox_Bypass);
                break;

            case 40://Reverbtron
                if (i < numEff)
                    rkr->Reverbtron_Bypass = 1;
                else
                    rkr->Reverbtron_Bypass = 0;
                REVERBTRON->revtron_activar->value(rkr->Reverbtron_Bypass);
                break;

            case 41://Echotron
                if (i < numEff)
                    rkr->Echotron_Bypass = 1;
                else
                    rkr->Echotron_Bypass = 0;
                ECHOTRON->echotron_activar->value(rkr->Echotron_Bypass);
                break;

            case 42://StereoHarm
                if (i < numEff)
                    rkr->StereoHarm_Bypass = 1;
                else
                    rkr->StereoHarm_Bypass = 0;
                SHAR->shar_activar->value(rkr->StereoHarm_Bypass);
                break;

            case 43://CompBand
                if (i < numEff)
                    rkr->CompBand_Bypass = 1;
                else
                    rkr->CompBand_Bypass = 0;
                COMPBAND->cband_activar->value(rkr->CompBand_Bypass);
                break;

            case 44://Opticaltrem
                if (i < numEff)
                    rkr->Opticaltrem_Bypass = 1;
                else
                    rkr->Opticaltrem_Bypass = 0;
                OTREM->otrem_activar->value(rkr->Opticaltrem_Bypass);
                break;

            case 45://Vibe
                if (i < numEff)
                    rkr->Vibe_Bypass = 1;
                else
                    rkr->Vibe_Bypass = 0;
                VIBE->vibe_activar->value(rkr->Vibe_Bypass);
                break;

            case 46://Infinity
                if (i < numEff)
                    rkr->Infinity_Bypass = 1;
                else
                    rkr->Infinity_Bypass = 0;
                INFINIT->infinity_activar->value(rkr->Infinity_Bypass);
                break;
        }

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
    int ok = rkr->loadbank(filename);

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
    int ok = rkr->savebank(filename);
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
    rkr->loadmiditable(filename);
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
    rkr->savemiditable(filename);
}
