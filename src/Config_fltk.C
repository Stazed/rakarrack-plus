/*
  rakarrack - a guitar effects software
  Config_fltk.C

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
 * File:   Config_fltk.C
 * Author: sspresto
 * 
 * Created on March 26, 2021, 11:06 AM
 */


#include "global.h"
#include "Config_fltk.h"
#include <FL/Fl.H>

Config_fltk::Config_fltk()
{
}

Config_fltk::~Config_fltk()
{
}

char *
Config_fltk::PrefNom(const char *dato)
{
    memset(tmpprefname, 0, sizeof (tmpprefname));

    if(nsm_preferences_file.empty())
    {
        sprintf(tmpprefname, "%s %s", jackcliname, dato);
    }
    else    // NSM use - always use PACKAGE name since there is only one per preferences file
    {
        sprintf(tmpprefname, "%s %s", PACKAGE, dato);
    }

    return (tmpprefname);
}

void
Config_fltk::load_preferences(Fl_Preferences &rakarrack)
{
    // ************* Settings/Look ******************
    rakarrack.get(PrefNom("Schema"), Schema, 2);

    // Fonts that look good - Cantarell Bold, Computer Modern Bright Bold, DejaVu Sans Condensed
    // Free Helvetian, FreeSans, Garuda, Ubuntu, Verana Sans
    rakarrack.get(PrefNom("Font"), font_type, 0);
    // Sanity check. Can happen when NSM session copied to another computer
    // that has fewer fonts loaded than source. Also if some fonts are removed.
    // Segfault if font type is out of range.
    if(font_type >= Fl::set_fonts(0)) // set_fonts returns number of fonts available
    {
        fprintf(stderr, "Invalid font type, reverting to default\n");
        font_type = 0;   // reset to default
    }
    
    rakarrack.get(PrefNom("FontSize"), font_size, C_DEFAULT_FONT_SIZE);
    rakarrack.get(PrefNom("Foreground Color"), fore_color, RKR_buttons_color);      // FL_DARK3 - Buttons
    rakarrack.get(PrefNom("Labels Color"), label_color, RKR_labels_color);          // FL_WHITE
    rakarrack.get(PrefNom("Leds Color"), leds_color, RKR_leds_color);               // FL_DARK_YELLOW
    rakarrack.get(PrefNom("Background Color"), back_color, RKR_background_color);   // FL_BLACK

    rakarrack.get(PrefNom("Enable Background Image"), EnableBackgroundImage, 0);

    char temp[256];
    sprintf(temp, "%s/blackbg.png", DATADIR);
    rakarrack.get(PrefNom("Background Image"), BackgroundImage, temp, 256);
    
    // Check if valid file. Revert to default if error.
    FILE *fn;
    if ((fn = fopen(BackgroundImage, "r")) == NULL)
    {
        memset(BackgroundImage, 0, sizeof (BackgroundImage));
        strcpy(BackgroundImage, temp);
        fprintf(stderr, "Invalid BackgroundImage file, reverting to default\n");
    }
    else
    {
        fclose(fn);
    }

    rakarrack.get(PrefNom("Hide Effects"), deachide, 0);
    rakarrack.get(PrefNom("Scale Window"), scalable, 0);
    // End Settings/Look

    // ************ Settings/Audio *******************
    if(nsm_preferences_file.empty())    // Not NSM - master Off by default = 0
    {
        rakarrack.get(PrefNom("FX_init_state"), init_state, 0);
    }
    else    // NSM - master On by default = 1
    {
        rakarrack.get(PrefNom("FX_init_state"), init_state, 1);
    }

    rakarrack.get(PrefNom("Filter DC Offset"), DC_Offset, 0);
    rakarrack.get(PrefNom("Preserve Gain/Master"), preserve_master, 0);
    rakarrack.get(PrefNom("Update Tap"), Tap_Updated, 0);
    rakarrack.get(PrefNom("Limiter Position"), flpos, 0);
    rakarrack.get(PrefNom("Limiter +6dB"), db6booster, 0);

    rakarrack.get(PrefNom("UpSampling"), upsample, 0);
    rakarrack.get(PrefNom("UpAmount"), UpAmo, 0);
    rakarrack.get(PrefNom("UpQuality"), UpQual, 4);
    rakarrack.get(PrefNom("DownQuality"), DownQual, 4);

    rakarrack.get(PrefNom("Looper Size"), looper_size, 1);
    rakarrack.get(PrefNom("Metronome Volume"), Metro_Vol, 50);  // Looper

    rakarrack.get(PrefNom("Calibration"), aFreq, 440.0f);
    rakarrack.get(PrefNom("Recognize Trigger"), rtrig, .6f);

    rakarrack.get(PrefNom("Recognize Optimization Harmonizer"), RCOpti_Harm, 0);
    rakarrack.get(PrefNom("Recognize Optimization Stereo Harm"), RCOpti_Stereo, 0);
    rakarrack.get(PrefNom("Recognize Optimization Ring"), RCOpti_Ring, 0);
    // End Settings/Audio

    // ************ Settings/Quality ******************
    rakarrack.get(PrefNom("Harmonizer Quality"), HarQual, 4);
    rakarrack.get(PrefNom("StereoHarm Quality"), SteQual, 4);
    
    rakarrack.get(PrefNom("Sequence Quality"), SeqQual, 4);
    rakarrack.get(PrefNom("Shifter Quality"), ShiQual, 4);

    rakarrack.get(PrefNom("Vocoder Bands"), VocBands, 32);

    rakarrack.get(PrefNom("Harmonizer Downsample"), Har_Down, 5);
    rakarrack.get(PrefNom("Harmonizer Up Quality"), Har_U_Q, 4);
    rakarrack.get(PrefNom("Harmonizer Down Quality"), Har_D_Q, 2);

    rakarrack.get(PrefNom("Reverbtron Downsample"), Rev_Down, 5);
    rakarrack.get(PrefNom("Reverbtron Up Quality"), Rev_U_Q, 4);
    rakarrack.get(PrefNom("Reverbtron Down Quality"), Rev_D_Q, 2);

    rakarrack.get(PrefNom("Convolotron Downsample"), Con_Down, 6);
    rakarrack.get(PrefNom("Convolotron Up Quality"), Con_U_Q, 4);
    rakarrack.get(PrefNom("Convolotron Down Quality"), Con_D_Q, 2);

    rakarrack.get(PrefNom("Sequence Downsample"), Seq_Down, 5);
    rakarrack.get(PrefNom("Sequence Up Quality"), Seq_U_Q, 4);
    rakarrack.get(PrefNom("Sequence Down Quality"), Seq_D_Q, 2);

    rakarrack.get(PrefNom("Shifter Downsample"), Shi_Down, 5);
    rakarrack.get(PrefNom("Shifter Up Quality"), Shi_U_Q, 4);
    rakarrack.get(PrefNom("Shifter Down Quality"), Shi_D_Q, 2);

    rakarrack.get(PrefNom("Vocoder Downsample"), Voc_Down, 5);
    rakarrack.get(PrefNom("Vocoder Up Quality"), Voc_U_Q, 4);
    rakarrack.get(PrefNom("Vocoder Down Quality"), Voc_D_Q, 2);

    rakarrack.get(PrefNom("StereoHarm Downsample"), Ste_Down, 5);
    rakarrack.get(PrefNom("StereoHarm Up Quality"), Ste_U_Q, 4);
    rakarrack.get(PrefNom("StereoHarm Down Quality"), Ste_D_Q, 2);

    rakarrack.get(PrefNom("Distortion Resampling"), Dist_res_amount, 2);
    rakarrack.get(PrefNom("Distortion Up Quality"), Dist_up_q, 4);
    rakarrack.get(PrefNom("Distortion Down Quality"), Dist_down_q, 2);

    rakarrack.get(PrefNom("Overdrive Resampling"), Ovrd_res_amount, 2);
    rakarrack.get(PrefNom("Overdrive Up Quality"), Ovrd_up_q, 4);
    rakarrack.get(PrefNom("Overdrive Down Quality"), Ovrd_down_q, 2);

    rakarrack.get(PrefNom("Derelict Resampling"), Dere_res_amount, 2);
    rakarrack.get(PrefNom("Derelict Up Quality"), Dere_up_q, 4);
    rakarrack.get(PrefNom("Derelict Down Quality"), Dere_down_q, 2);

    rakarrack.get(PrefNom("DistBand Resampling"), DBand_res_amount, 2);
    rakarrack.get(PrefNom("DistBand Up Quality"), DBand_up_q, 4);
    rakarrack.get(PrefNom("DistBand Down Quality"), DBand_down_q, 2);

    rakarrack.get(PrefNom("StompBox Resampling"), Stomp_res_amount, 2);
    rakarrack.get(PrefNom("StompBox Up Quality"), Stomp_up_q, 4);
    rakarrack.get(PrefNom("StompBox Down Quality"), Stomp_down_q, 2);
    // End Settings/Quality

    // ************ Settings/MIDI *****************
    // Alsa MIDI
    rakarrack.get(PrefNom("Auto Connect MIDI IN"), aconnect_MI, 0);
    rakarrack.get(PrefNom("MIDI IN Device"), MID, "", 40);

    rakarrack.get(PrefNom("MIDI IN Channel"), MIDI_In_Channel, 1);
    MIDI_In_Channel--;

    rakarrack.get(PrefNom("MIDI IN Harmonizer"), Harmonizer_MIDI_Channel, 1);
    Harmonizer_MIDI_Channel--;

    rakarrack.get(PrefNom("MIDI IN Stereo Harmonizer"), StereoHarm_MIDI_Channel, 1);
    StereoHarm_MIDI_Channel--;

    // MIDI Learn used On/Off
    rakarrack.get(PrefNom("MIDI Implementation"), MIDIway, 0);
    rakarrack.get(PrefNom("Auto Assign"), autoassign, 0);

    // Custom MIDI Table used On/OFF
    rakarrack.get(PrefNom("MIDI Table"), custom_midi_table, 0);
    
    // Custom MIDI Table last used file
    rakarrack.get(PrefNom("MIDI Table File"), custom_midi_table_file, -1);
    // End Settings/MIDI

    // *********** Settings/Jack *****************
    // Jack Out
    if(nsm_preferences_file.empty())
    {
        rakarrack.get(PrefNom("Auto Connect Jack"), aconnect_JA, 1);    // Out
    }
    else    // Using NSM - shut off auto connect out
    {
        // For NSM the default setting is Off for auto connect audio and we do not let the user change it.
        // All jack connections should be handled by NSM.
        aconnect_JA = 0;
    }

    rakarrack.get(PrefNom("Auto Connect Num"), cuan_jack, 2);       // Out
    
    memset(temp, 0, sizeof (temp));

    char j_names[128];

    static const char *jack_names[] ={"system:playback_1", "system:playback_2"};

    for (int i = 0; i < cuan_jack; i++)
    {
        memset(temp, 0, sizeof (temp));
        sprintf(temp, "Jack Port %d", i + 1);
        
        if (i < 2)
        {
            strcpy(j_names, jack_names[i]);
        }
        else
        {
            strcpy(j_names, "");
        }
        
        rakarrack.get(PrefNom(temp), jack_po[i].name, j_names, 128);
    }

    // Jack In
    if(nsm_preferences_file.empty())    // Not NSM
    {
        rakarrack.get(PrefNom("Auto Connect Jack In"), aconnect_JIA, 0);    // In
    }
    else    // Using NSM - shut off auto connect in
    {
        // For NSM the default setting is Off for auto connect audio and we do not let the user change it.
        // All jack connections should be handled by NSM.
        aconnect_JIA = 0;
    }

    rakarrack.get(PrefNom("Auto Connect In Num"), cuan_ijack, 1);

    memset(j_names, 0, sizeof (j_names));

    static const char *jack_inames[] ={"system:capture_1", "system:capture_2"};

    for (int i = 0; i < cuan_ijack; i++)
    {
        memset(temp, 0, sizeof (temp));
        sprintf(temp, "Jack Port In %d", i + 1);
        
        if (i < 1)
        {
            strcpy(j_names, jack_inames[i]);
        }
        else
        {
            strcpy(j_names, "");
        }
        
        rakarrack.get(PrefNom(temp), jack_poi[i].name, j_names, 128);
    }
    // End Settings/Jack

    // *************** Settings/Misc **********************
    rakarrack.get(PrefNom("Disable Warnings"), Disable_Warnings, 0);
    rakarrack.get(PrefNom("Tap Tempo Timeout"), t_timeout, 0);
    rakarrack.get(PrefNom("Enable Tooltips"), ena_tool, 1);
    rakarrack.get(PrefNom("Focus Delay"), Focus_Delay, 0);
    // End Settings/Misc

    // ******************* Settings/User *******************
    // Get user default bank file from Settings/Bank/ --Bank Filename
    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/Default.rkrb", DATADIR);
    rakarrack.get(PrefNom("Bank Filename"), BankFilename, temp, 127);
    
    // Get user bank directory
    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s", DATADIR);
    rakarrack.get(PrefNom("User Directory"), UDirFilename, temp, 127);
    global_user_directory = UDirFilename;
    
    rakarrack.get(PrefNom("UserName"), UserRealName, "", 127);
    // End Settings/User

    // ************** Window Sizes *****************
    rakarrack.get(PrefNom("Principal X"), Principal_X, 1);
    rakarrack.get(PrefNom("Principal Y"), Principal_Y, 1);
    rakarrack.get(PrefNom("Principal W"), Principal_W, 960);
    rakarrack.get(PrefNom("Principal H"), Principal_H, 540);

    rakarrack.get(PrefNom("BankWindow X"), BankWindow_X, 1);
    rakarrack.get(PrefNom("BankWindow Y"), BankWindow_Y, 1);
    rakarrack.get(PrefNom("BankWindow W"), BankWindow_W, 680);
    rakarrack.get(PrefNom("BankWindow H"), BankWindow_H, 425);

    rakarrack.get(PrefNom("Order X"), Order_X, 1);
    rakarrack.get(PrefNom("Order Y"), Order_Y, 1);
    rakarrack.get(PrefNom("Order W"), Order_W, 600);
    rakarrack.get(PrefNom("Order H"), Order_H, 330);

    rakarrack.get(PrefNom("MIDI Learn X"), MIDI_Learn_X, 1);
    rakarrack.get(PrefNom("MIDI Learn Y"), MIDI_Learn_Y, 1);
    rakarrack.get(PrefNom("MIDI Learn W"), MIDI_Learn_W, 600);
    rakarrack.get(PrefNom("MIDI Learn H"), MIDI_Learn_H, 360);

    rakarrack.get(PrefNom("Trigger X"), Trigger_X, 1);
    rakarrack.get(PrefNom("Trigger Y"), Trigger_Y, 1);
    rakarrack.get(PrefNom("Trigger W"), Trigger_W, 280);
    rakarrack.get(PrefNom("Trigger H"), Trigger_H, 165);

    rakarrack.get(PrefNom("Settings X"), Settings_X, 1);
    rakarrack.get(PrefNom("Settings Y"), Settings_Y, 1);
    rakarrack.get(PrefNom("Settings W"), Settings_W, 440);
    rakarrack.get(PrefNom("Settings H"), Settings_H, 410);

    rakarrack.get(PrefNom("Help X"), Help_X, 1);
    rakarrack.get(PrefNom("Help Y"), Help_Y, 1);
    rakarrack.get(PrefNom("Help W"), Help_W, 640);
    rakarrack.get(PrefNom("Help H"), Help_H, 480);
    rakarrack.get(PrefNom("Help TextSize"), Help_TextSize, 11);
    // End Window Sizes

    // Main window booster button
    rakarrack.get(PrefNom("Booster"), booster, 1.0f);   // On/Off

    // Last bank and preset selected
    rakarrack.get(PrefNom("Bank Selected"), active_bank, 3);
    rakarrack.get(PrefNom("Preset Num"), Preset_Number, 1);

    // MIDIConverter
    rakarrack.get(PrefNom("Midi Out Channel"), Midi_Out_Channel, 1);
    rakarrack.get(PrefNom("Trigger Adjust"), Trigger_Adjust, 4);
    rakarrack.get(PrefNom("Velocity Adjust"), Velocity_Adjust, 50);
    rakarrack.get(PrefNom("Converter Octave"), Converter_Octave, 2);
    rakarrack.get(PrefNom("MIDI Converter On/Off"), MIDI_Converter_On_Off, 0);

    // Metronome
    rakarrack.get(PrefNom("Internal Metronome Time"), Metronome_Time, 2);
    rakarrack.get(PrefNom("Internal Metronome Sound"), Metronome_Sound, 0);
    rakarrack.get(PrefNom("Internal Metronome Volume"), Metronome_Volume, 50);
    rakarrack.get(PrefNom("Internal Metronome Tempo"), Metronome_Tempo, 100);
    rakarrack.get(PrefNom("Internal Metronome Show"), sw_stat, 0);
    rakarrack.get(PrefNom("Internal Metronome On/Off"), Metronome_On_Off, 0);

    // Tuner
    rakarrack.get(PrefNom("Tuner On/Off"), Tuner_On_Off, 0);

    // Tap Tempo
    rakarrack.get(PrefNom("TapTempo Input"), Tap_Selection, 0);
    rakarrack.get(PrefNom("TapTempo Set"), Tap_SetValue, 0);
    rakarrack.get(PrefNom("TapTempo On/Off"), TapTempo_On_Off, 0);

    // Trigger
    rakarrack.get(PrefNom("Aux Source"), Aux_Source, 0);
    rakarrack.get(PrefNom("Aux Gain"), Aux_Gain, 0);
    rakarrack.get(PrefNom("Aux Threshold"), Aux_Threshold, 0);
    rakarrack.get(PrefNom("Aux MIDI"), Aux_MIDI, 1);
    rakarrack.get(PrefNom("Aux Minimum"), Aux_Minimum, 0);
    rakarrack.get(PrefNom("Aux Maximum"), Aux_Maximum, 127);
#ifdef NSM_SUPPORT
    rakarrack.get(PrefNom("NSM Gui Status"), NSM_gui_status, 0);
    if(NSM_gui_status)
    {
        global_gui_show = CONST_GUI_SHOW;
    }
    else
    {
        global_gui_show = CONST_GUI_HIDE;
    }
#endif
}

/**
 * User settings from Settings/Preferences.
 * Main Window non rack effect settings: Tuner, MIDI Converter, Tap Tempo, Metronome,
 * Booster button, last used bank, last used preset.
 * Also the previous state when shutdown, window sizes, images, etc.
 * From ~user/.fltk/github.com.Stazed.rakarrack.plus/rakarrack-plus.prefs
 */
void Config_fltk::load_previous_state()
{
    if(nsm_preferences_file.empty())
    {
        Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);
        load_preferences(rakarrack);
    }
    else    // Using NSM
    {
        Fl_Preferences rakarrack(nsm_preferences_file.c_str(), jack_client_name, NULL);
        load_preferences(rakarrack);
    }
}
