/*
  rakarrack - a guitar effects software
  Config_fltk.h

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
 * File:   Config_fltk.h
 * Author: sspresto
 *
 * Created on March 26, 2021, 11:06 AM
 */

#ifndef CONFIG_FLTK_H
#define CONFIG_FLTK_H

class Config_fltk {
public:
    Config_fltk();
    virtual ~Config_fltk();
    
    void load_user_preferences();
    
    /**
     * Flag to indicate if user warnings should be disabled. From check box
     * in Settings/Misc.
     */
    int Disable_Warnings;

    /**
     * Flag for check box in Settings/Audio "Filter Input DC Offset"
     */
    int DC_Offset;

    float looper_size;

    float aFreq;

    int VocBands;

    float rtrig;
    
    /* Quality resampling */
    int Har_Down;
    int Har_U_Q;
    int Har_D_Q;

    int Rev_Down;
    int Rev_U_Q;
    int Rev_D_Q;

    int Con_Down;
    int Con_U_Q;
    int Con_D_Q;

    int Shi_Down;
    int Shi_U_Q;
    int Shi_D_Q;

    int Seq_Down;
    int Seq_U_Q;
    int Seq_D_Q;

    int Voc_Down;
    int Voc_U_Q;
    int Voc_D_Q;

    int Ste_Down;
    int Ste_U_Q;
    int Ste_D_Q;

    /* Master resampling */
    int upsample;
    int UpQual;
    int DownQual;
    int UpAmo;
    /* End Quality resampling */
    
    /* WaveShape Resampling */
    int Dist_res_amount;
    int Dist_up_q;
    int Dist_down_q;
    
    int Ovrd_res_amount;
    int Ovrd_up_q;
    int Ovrd_down_q;

    int Dere_res_amount;
    int Dere_up_q;
    int Dere_down_q;

    int DBand_res_amount;
    int DBand_up_q;
    int DBand_down_q;

    int Stomp_res_amount;
    int Stomp_up_q;
    int Stomp_down_q;
    /* End WaveShape */
    
    int HarQual;
    int SteQual;
    int SeqQual;
    int ShiQual;

    int aconnect_JA;
    int aconnect_JIA;

    int cuan_jack;
    int cuan_ijack;

    int MIDIway;

    /**
     * Flag to indicate the custom MIDI Program Change Table is active.
     * This is the check box in Settings/Preferences/MIDI - MIDI Program Change Table
     */
    int custom_midi_table;

    /**
     * The last loaded custom MIDI table file, if any. -1 if none.
     */
    int custom_midi_table_file;

    /**
     * The default user bank from Settings/Bank/Bank Filename. Also the
     * bank selected when the user clicks the 'U' button from the Bank Manager
     * and above the Bank button on the main window.
     */
    char BankFilename[128];

    char UDirFilename[128];

private:
    
    char tmpprefname[128];
    char jackcliname[64];

    struct JackPorts
    {
        char name[128];
        
        JackPorts():
            name() {}
    } jack_po[16],jack_poi[16];
    
    char *PrefNom (const char *dato);

};

#endif /* CONFIG_FLTK_H */

