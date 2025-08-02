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
#include "strlcpy.h"
#include "Config_fltk.h"
#include <FL/Fl.H>

Config_fltk::Config_fltk() :
    Schema(2),
    font_type(0),
    font_size(C_DEFAULT_FONT_SIZE),
    fore_color(RKR_buttons_color),
    label_color(RKR_labels_color),
    leds_color(RKR_leds_color),
    back_color(RKR_background_color),
    EnableBackgroundImage(0),
    BackgroundImage(),
    deachide(),
    scalable(),
    init_state(1),
    DC_Offset(),
    preserve_master(),
    Tap_Updated(),
    flpos(),
    db6booster(),
    upsample(),
    UpQual(4),
    DownQual(4),
    UpAmo(),
    looper_size(1),
    Metro_Vol(50),
    aFreq(440.0f),
    rtrig(0.6f),
    RCOpti_Harm(),
    RCOpti_Stereo(),
    RCOpti_Ring(),
    HarQual(4),
    SteQual(4),
    SeqQual(4),
    ShiQual(4),
    VocBands(32),
    Har_Down(5),
    Har_U_Q(4),
    Har_D_Q(2),
    Rev_Down(5),
    Rev_U_Q(4),
    Rev_D_Q(2),
    Con_Down(6),
    Con_U_Q(4),
    Con_D_Q(2),
    Seq_Down(5),
    Seq_U_Q(4),
    Seq_D_Q(2),
    Shi_Down(5),
    Shi_U_Q(4),
    Shi_D_Q(2),
    Voc_Down(5),
    Voc_U_Q(4),
    Voc_D_Q(2),
    Ste_Down(5),
    Ste_U_Q(4),
    Ste_D_Q(2),
    Dist_res_amount(2),
    Dist_up_q(4),
    Dist_down_q(2),
    Ovrd_res_amount(2),
    Ovrd_up_q(4),
    Ovrd_down_q(2),
    Dere_res_amount(2),
    Dere_up_q(4),
    Dere_down_q(2),
    DBand_res_amount(2),
    DBand_up_q(4),
    DBand_down_q(2),
    Stomp_res_amount(2),
    Stomp_up_q(4),
    Stomp_down_q(2),
    aconnect_MI(0),
    MID(),
    MIDI_In_Channel(1),
    Harmonizer_MIDI_Channel(1),
    StereoHarm_MIDI_Channel(1),
    MIDIway(),
    autoassign(),
    custom_midi_table(),
    custom_midi_table_file(-1),
    aconnect_JA(1),
    cuan_jack(2),
    aconnect_JIA(0),
    cuan_ijack(1),
    Disable_Warnings(),
    t_timeout(),
    ena_tool(1),
    Focus_Delay(),
    BankFilename(),
    UDirFilename(),
    UserRealName(),
    Principal_X(1),
    Principal_Y(1),
    Principal_W(960),
    Principal_H(540),
    BankWindow_X(1),
    BankWindow_Y(1),
    BankWindow_W(680),
    BankWindow_H(425),
    Order_X(1),
    Order_Y(1),
    Order_W(600),
    Order_H(330),
    MIDI_Learn_X(1),
    MIDI_Learn_Y(1),
    MIDI_Learn_W(600),
    MIDI_Learn_H(360),
    Trigger_X(1),
    Trigger_Y(1),
    Trigger_W(280),
    Trigger_H(165),
    Settings_X(1),
    Settings_Y(1),
    Settings_W(440),
    Settings_H(410),
    Help_X(1),
    Help_Y(1),
    Help_W(640),
    Help_H(480),
    Help_TextSize(11),
    Random_X(1),
    Random_Y(1),
    Random_W(240),
    Random_H(320),
    Delay_X(1),
    Delay_Y(1),
    Delay_W(800),
    Delay_H(265),
    booster(1.0f),
    active_bank(0),
    Preset_Number(1),
    Midi_Out_Channel(1),
    Trigger_Adjust(4),
    Velocity_Adjust(50),
    Converter_Octave(2),
    MIDI_Converter_On_Off(0),
    Use_FFT(0),
    Metronome_Time(2),
    Metronome_Sound(0),
    Metronome_Volume(50),
    Metronome_Tempo(100),
    sw_stat(),
    Metronome_On_Off(),
    Tuner_On_Off(),
    Tap_Selection(),
    Tap_SetValue(),
    TapTempo_On_Off(),
    Aux_Source(),
    Aux_Gain(),
    Aux_Threshold(),
    Aux_MIDI(1),
    Aux_Minimum(),
    Aux_Maximum(127),
    Rand_Parameters(),
    Rand_Active(),
    Rand_Current(),
    Rand_Max(6),
    Rand_Exclude(),
    Analyzer_On_Off(false),
    Scope_On_Off(false),
#ifdef NSM_SUPPORT
    NSM_single_state(),
    NSM_gui_status(),
#endif
    jackcliname(),
    tmpprefname()
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
        snprintf(tmpprefname, sizeof(tmpprefname), "%s %s", jackcliname, dato);
    }
    else    // NSM use - always use PACKAGE name since there is only one per preferences file
    {
        snprintf(tmpprefname, sizeof(tmpprefname), "%s %s", PACKAGE, dato);
    }

    return (tmpprefname);
}

void
Config_fltk::load_preferences(Fl_Preferences &rakarrack)
{
    // ************* Settings/Look ******************
    rakarrack.get(PrefNom("Schema"), Schema, 2);

// NTK_EXTENDED is ntk-unofficial which supports this
#if !defined NTK_EXTENDED && defined NTK_SUPPORT
    font_type = 0;  // NTK does not support this ATM
#else
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
#endif

    rakarrack.get(PrefNom("FontSize"), font_size, C_DEFAULT_FONT_SIZE);
    rakarrack.get(PrefNom("Foreground Color"), fore_color, RKR_buttons_color);
    rakarrack.get(PrefNom("Labels Color"), label_color, RKR_labels_color);
    rakarrack.get(PrefNom("Leds Color"), leds_color, RKR_leds_color);
    rakarrack.get(PrefNom("Background Color"), back_color, RKR_background_color);

    rakarrack.get(PrefNom("Enable Background Image"), EnableBackgroundImage, 0);

    char temp[256];
    snprintf(temp, sizeof(temp), "%s/blackbg.png", DATADIR);
    rakarrack.get(PrefNom("Background Image"), BackgroundImage, temp, 256);

    // Check if valid file. Revert to default if error.
    FILE *fn;
    if ((fn = fopen(BackgroundImage, "r")) == NULL)
    {
        memset(BackgroundImage, 0, sizeof (BackgroundImage));
        RKRP::strlcpy(BackgroundImage, temp, sizeof (BackgroundImage));
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
#ifdef RKR_PLUS_LV2
        rakarrack.get(PrefNom("FX_init_state"), init_state, 1); // On by default
#else
        rakarrack.get(PrefNom("FX_init_state"), init_state, 0);
#endif
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
        snprintf(temp, sizeof(temp), "Jack Port %d", i + 1);

        if (i < 2)
        {
            RKRP::strlcpy(j_names, jack_names[i], sizeof(j_names));
        }
        else
        {
            RKRP::strlcpy(j_names, "", sizeof(j_names));
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
        snprintf(temp, sizeof(temp), "Jack Port In %d", i + 1);

        if (i < 1)
        {
            RKRP::strlcpy(j_names, jack_inames[i], sizeof(j_names));
        }
        else
        {
            RKRP::strlcpy(j_names, "", sizeof(j_names));
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
    snprintf(temp, sizeof(temp), "%s/Default.rkrb", DATADIR);
    rakarrack.get(PrefNom("Bank Filename"), BankFilename, temp, 127);

    // Get user bank directory
    memset(temp, 0, sizeof (temp));
    snprintf(temp, sizeof(temp), "%s", UD_NOT_SET);
    rakarrack.get(PrefNom("User Directory"), UDirFilename, temp, 127);
    global_user_directory = UDirFilename;

    memset(temp, 0, sizeof (temp));
    rakarrack.get(PrefNom("UserName"), UserRealName, temp, 127);
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

    rakarrack.get(PrefNom("Random X"), Random_X, 1);
    rakarrack.get(PrefNom("Random Y"), Random_Y, 1);
    rakarrack.get(PrefNom("Random W"), Random_W, 240);
    rakarrack.get(PrefNom("Random H"), Random_H, 320);

    rakarrack.get(PrefNom("Delay X"), Delay_X, 1);
    rakarrack.get(PrefNom("Delay Y"), Delay_Y, 1);
    rakarrack.get(PrefNom("Delay W"), Delay_W, 800);
    rakarrack.get(PrefNom("Delay H"), Delay_H, 265);
    
    // End Window Sizes

    // Main window booster button
    rakarrack.get(PrefNom("Booster"), booster, 1.0f);   // On/Off

    // Last bank and preset selected
    rakarrack.get(PrefNom("Bank Selected"), active_bank, 0);
#ifndef RKR_PLUS_LV2
    rakarrack.get(PrefNom("Preset Num"), Preset_Number, 1);
#else
    Preset_Number = 1;  // For LV2 we don't load the preset
#endif
    // MIDIConverter
    rakarrack.get(PrefNom("Midi Out Channel"), Midi_Out_Channel, 1);
    rakarrack.get(PrefNom("Trigger Adjust"), Trigger_Adjust, 4);
    rakarrack.get(PrefNom("Velocity Adjust"), Velocity_Adjust, 50);
    rakarrack.get(PrefNom("Converter Octave"), Converter_Octave, 2);
    rakarrack.get(PrefNom("MIDI Converter On/Off"), MIDI_Converter_On_Off, 0);
    rakarrack.get(PrefNom("Use FFT"), Use_FFT, 0);

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

    // Random
    rakarrack.get(PrefNom("Rand Parameters"), Rand_Parameters, 0);
    rakarrack.get(PrefNom("Rand Active"), Rand_Active, 0);
    rakarrack.get(PrefNom("Rand Current"), Rand_Current, 0);
    rakarrack.get(PrefNom("Rand Max"), Rand_Max, 6);

    memset(temp, 0, sizeof (temp));
    rakarrack.get(PrefNom("Rand Exclude"), Rand_Exclude, temp, EFX_NUMBER_EFFECTS + 1);

#ifdef NSM_SUPPORT
    if(!nsm_preferences_file.empty())
    {
        rakarrack.get(PrefNom("NSM Single State"), NSM_single_state, 0);

        rakarrack.get(PrefNom("NSM Gui Status"), NSM_gui_status, 0);
        if(NSM_gui_status)
        {
            global_gui_show = CONST_GUI_SHOW;
        }
        else
        {
            global_gui_show = CONST_GUI_HIDE;
        }
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
#if defined FLTK_VERSION_1_4 && !defined NTK_SUPPORT
        Fl_Preferences rakarrack(nsm_preferences_file.c_str(), jack_client_name, NULL, Fl_Preferences::USER_L);
#else
        Fl_Preferences rakarrack(nsm_preferences_file.c_str(), jack_client_name, NULL);
#endif
        load_preferences(rakarrack);
    }
}
