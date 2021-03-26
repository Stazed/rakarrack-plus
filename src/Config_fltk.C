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

#include <FL/Fl_Preferences.H>

#include "global.h"
#include "Config_fltk.h"

Config_fltk::Config_fltk()
{
}

Config_fltk::~Config_fltk()
{
}

/**
 *  Loads the user preferences set in the Settings/Preferences tabs:
 *  Look, Audio, Quality, MIDI, Jack, Misc, Bank.
 *  These settings are saved in:
 *  /home/username/.fltk/github.com.Stazed.rakarrrack.plus/rakarrack-plus.prefs
 * 
 *  This loads only items needed for audio/MIDI processing. Since the program
 *  can be run without GUI, the Look and other user settings not related to audio/MIDI
 *  do not need to be loaded here. This is for initial process setup. 
 * 
 *  The other non audio/MIDI user settings are loaded by the load_previous_state()
 *  function that is called when the RKRGUI class is created.
 */
void
Config_fltk::load_user_preferences()
{
    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);
    
    rakarrack.get(PrefNom("Disable Warnings"), Disable_Warnings, 0);
    rakarrack.get(PrefNom("Filter DC Offset"), DC_Offset, 0);
    rakarrack.get(PrefNom("UpSampling"), upsample, 0);
    rakarrack.get(PrefNom("UpQuality"), UpQual, 4);
    rakarrack.get(PrefNom("DownQuality"), DownQual, 4);
    rakarrack.get(PrefNom("UpAmount"), UpAmo, 0);

//    Adjust_Upsample();    // FIXME to process

    rakarrack.get(PrefNom("Looper Size"), looper_size, 1);
    rakarrack.get(PrefNom("Calibration"), aFreq, 440.0f);

    rakarrack.get(PrefNom("Vocoder Bands"), VocBands, 32);
    rakarrack.get(PrefNom("Recognize Trigger"), rtrig, .6f);

    rakarrack.get(PrefNom("Harmonizer Downsample"), Har_Down, 5);
    rakarrack.get(PrefNom("Harmonizer Up Quality"), Har_U_Q, 4);
    rakarrack.get(PrefNom("Harmonizer Down Quality"), Har_D_Q, 2);

    rakarrack.get(PrefNom("StereoHarm Downsample"), Ste_Down, 5);
    rakarrack.get(PrefNom("StereoHarm Up Quality"), Ste_U_Q, 4);
    rakarrack.get(PrefNom("StereoHarm Down Quality"), Ste_D_Q, 2);

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

    rakarrack.get(PrefNom("Harmonizer Quality"), HarQual, 4);
    rakarrack.get(PrefNom("StereoHarm Quality"), SteQual, 4);
    
    rakarrack.get(PrefNom("Sequence Quality"), SeqQual, 4);
    rakarrack.get(PrefNom("Shifter Quality"), ShiQual, 4);

    rakarrack.get(PrefNom("Auto Connect Jack"), aconnect_JA, 1);
    
    // Default on new installs - don't connect input - can cause feedback loop = 0
    rakarrack.get(PrefNom("Auto Connect Jack In"), aconnect_JIA, 0);

    rakarrack.get(PrefNom("Auto Connect Num"), cuan_jack, 2);
    rakarrack.get(PrefNom("Auto Connect In Num"), cuan_ijack, 1);
    
    // MIDI Learn used On/Off
    rakarrack.get(PrefNom("MIDI Implementation"), MIDIway, 0);
    
    // Custom MIDI Table used On/OFF
    rakarrack.get(PrefNom("MIDI Table"), custom_midi_table, 0);
    
    // Custom MIDI Table last used file
    rakarrack.get(PrefNom("MIDI Table File"), custom_midi_table_file, -1);


    char temp[256];
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

    // Get user default bank file from Settings/Bank/ --Bank Filename
    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/Default.rkrb", DATADIR);
    rakarrack.get(PrefNom("Bank Filename"), BankFilename, temp, 127);
    
    // Get user bank directory
    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/", DATADIR);
    rakarrack.get(PrefNom("User Directory"), UDirFilename, temp, 127);
    global_user_directory = UDirFilename;
}

char *
Config_fltk::PrefNom(const char *dato)
{
    memset(tmpprefname, 0, sizeof (tmpprefname));
    sprintf(tmpprefname, "%s %s", jackcliname, dato);
    return (tmpprefname);
}