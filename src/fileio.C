/*  rakarrack - Audio effects software

  fileio.C  -  File Input/Output functions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

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

#include <errno.h>
#include <algorithm>    // std::remove
#include "process.h"
#include <FL/fl_ask.H> // for error pop up

/**
 *  Individual preset file loading. Main menu File/Load Preset.
 *  Apply buffer holding effect parameters to the lv[][] array, bypass, filename.
 * 
 * @param buf
 *      Buffer holding effect parameters to be put into effect array, etc.
 *      This is a comma delimited buffer which comes from the formatted text file
 *      *.rkr types.
 * 
 * @param fx_index
 *      The effect index to process.
 */
void RKR::apply_effect_parameters(char *buf, int fx_index, PresetBankStruct &preset_loaded)
{
    // Copy buffer to std::string
    std::string s_buf(buf);

    // String vector to hold each comma delimited item
    std::vector<std::string> result;

    //create string stream from the string
    std::stringstream s_stream(s_buf);

    // Parse the s_buf string by comma delimiter into vector
    while(s_stream.good())
    {
        std::string substr;

        //get the string delimited by comma and push to string vector
        getline(s_stream, substr, ',');
        result.push_back(substr);
    }

    // Run through the effects for a match to the effect index
    for (int effect = 0; effect < C_NUMBER_EFFECTS; effect++)
    {
        if (effect == fx_index)
        {
            for(int params = 0; params < EFX_Param_Size[effect] + 1; params++)  // +1 for bypass
            {
                if(params < EFX_Param_Size[effect])   // Set the EFX parameters
                {
                    preset_loaded.lv[effect][params] =  atoi( result.at(params).c_str() );
                }
                else    // Set the bypass
                {
                    EFX_Bank_Active[effect] = atoi( result.at(params).c_str() );    // FIXME Active_Preset
                }
            }

            // Convolotron, Reverbtron, Echotron have file names to parse as well
            if(fx_index == EFX_CONVOLOTRON)
            {
                std::string s_name = result.at(EFX_Param_Size[effect] + 1);

                // Gotta remove the '\n' from the file name or error.
                s_name.erase(std::remove(s_name.begin(), s_name.end(), '\n'), s_name.end());
                strcpy(preset_loaded.ConvoFiname, s_name.c_str());
            }
            
            else if(fx_index == EFX_REVERBTRON)
            {
                std::string s_name = result.at(EFX_Param_Size[effect] + 1);

                // Gotta remove the '\n' from the file name or error.
                s_name.erase(std::remove(s_name.begin(), s_name.end(), '\n'), s_name.end());
                strcpy(preset_loaded.RevFiname, s_name.c_str());
            }

            else if(fx_index == EFX_ECHOTRON)
            {
                std::string s_name = result.at(EFX_Param_Size[effect] + 1);

                // Gotta remove the '\n' from the file name or error.
                s_name.erase(std::remove(s_name.begin(), s_name.end(), '\n'), s_name.end());
                strcpy(preset_loaded.EchoFiname, s_name.c_str());
            }
            
            return; // we found and processed what we were looking for
        }
    }
}

/**
 * Individual preset file saving and user defined inserted presets.
 * Get individual effect parameters and put them in the passed buffer. 
 * The buffer is comma delimited and ends with newline (\n).
 * This is used in human readable text files.
 * 
 * @param buf
 *      Buffer to hold effect parameters.
 * 
 * @param fx_index
 *      The effect index number to load.
 */
void RKR::get_effect_parameters(char *buf, int fx_index)
{
    // String buffer to hold everything
    std::string s_buf = "";
    
     // Run through the effects for a match to the effect index
    for (int effect = 0; effect < C_NUMBER_EFFECTS; effect++)
    {
        if (effect == fx_index)
        {
            for(int params = 0; params < EFX_Param_Size[effect] + 1; params++)  // +1 for bypass
            {
                if(params < EFX_Param_Size[effect])   // the effect parameters
                {
                    // add integer parameters converted to string
                    s_buf += NTS(Rack_Effects[effect]->getpar(params));

                    // add the delimiter
                    s_buf += ",";
                }
                else    // add the effect bypass
                {
                    s_buf += NTS(EFX_Active[effect]);
                }
            }

            // Convolotron, Reverbtron, Echotron have file names to add as well
            if(fx_index == EFX_CONVOLOTRON)
            {
                s_buf += ",";   // Add comma after bypass
                Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
                s_buf +=  Efx_Convolotron->Filename;
            }
            
            else if(fx_index == EFX_REVERBTRON)
            {
                s_buf += ",";   // Add comma after bypass
                Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
                s_buf += Efx_Reverbtron->Filename;
            }

            else if(fx_index == EFX_ECHOTRON)
            {
                s_buf += ",";   // Add comma after bypass
                Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
                s_buf += Efx_Echotron->Filename;
            }

            // Add final newline
            s_buf += "\n";

            // copy string to passed char array
            strcpy(buf, s_buf.c_str());
            
            return; // we found and processed what we were looking for
        }
    }
}

/**
 * Save individual presets from the menu, File/Save Preset and
 * the Save button of the main window.
 * 
 * This is a formatted text file, .rkr type.
 * 
 * @param filename
 *      The user defined file name for the preset to be saved.
 */
void
RKR::save_preset(const char *filename)
{
    FILE *fn;
    char buf[256];
    fn = fopen(filename, "w");

    if (errno == EACCES)
    {
        Handle_Message(3);
        fclose(fn);
        return;
    }

    // Program release version
    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%s\n", VERSION);
    fputs(buf, fn);

    // Author
    memset(buf, 0, sizeof (buf));

    if (strlen(Active_Preset.Author) != 0)
    {
        sprintf(buf, "%s\n", Active_Preset.Author);
    }
    else
    {
        if (UserRealName != NULL)
        {
            sprintf(buf, "%s\n", UserRealName);
        }
        else
        {
            sprintf(buf, "%s\n", getenv("USER"));
        }
    }

    fputs(buf, fn);

    // Preset Name
    fputs(Active_Preset.Preset_Name, fn);
    fputs("\n", fn);

    // Master control
    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%f,%f,%f,%d\n", Active_Preset.Input_Gain, Active_Preset.Master_Volume,
            Active_Preset.Fraction_Bypass, Active_Preset.FX_Master_Active);
    fputs(buf, fn);

    // Effect parameters
    for (int order = 0; order < C_NUMBER_ORDERED_EFFECTS; order++)
    {
        int effect = efx_order[order];
        memset(buf, 0, sizeof (buf));
        get_effect_parameters(buf, effect);
        fputs(buf, fn);
    }

    // Effect Order
    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            efx_order[0], efx_order[1], efx_order[2], efx_order[3],
            efx_order[4], efx_order[5], efx_order[6], efx_order[7],
            efx_order[8], efx_order[9]);

    fputs(buf, fn);

    // MIDI learn table
    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                Active_Preset.XUserMIDI[i][0], Active_Preset.XUserMIDI[i][1],
                Active_Preset.XUserMIDI[i][2], Active_Preset.XUserMIDI[i][3],
                Active_Preset.XUserMIDI[i][4], Active_Preset.XUserMIDI[i][5],
                Active_Preset.XUserMIDI[i][6], Active_Preset.XUserMIDI[i][7],
                Active_Preset.XUserMIDI[i][8], Active_Preset.XUserMIDI[i][9],
                Active_Preset.XUserMIDI[i][10], Active_Preset.XUserMIDI[i][10],
                Active_Preset.XUserMIDI[i][12], Active_Preset.XUserMIDI[i][13],
                Active_Preset.XUserMIDI[i][14], Active_Preset.XUserMIDI[i][15],
                Active_Preset.XUserMIDI[i][16], Active_Preset.XUserMIDI[i][17],
                Active_Preset.XUserMIDI[i][18], Active_Preset.XUserMIDI[i][19]);

        fputs(buf, fn);
    }

    fclose(fn);
}

/**
 * This parses the preset files *.rkr types, from the menu File/Load Preset
 * and the Load button from the main window.
 * 
 * @param filename
 *      The user selected filename.
 */
void
RKR::load_preset(const char *filename)
{
    FILE *fn;
    char buf[256];

    PresetBankStruct preset_loaded;
    
    if ((fn = fopen(filename, "r")) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(14, filename);
        return;
    }

    // This cycles through the first 14 lines which should always exist.
    // Will set fgets to the next item which is order for loading.
    for (int i = 0; i < 14; i++)
    {
        memset(buf, 0, sizeof (buf));
        
        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            File_To_Load.clear();
            Handle_Message(14, filename);
            file_error(fn);
            return;
        }
    }

    // Order
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(15, filename);
        file_error(fn);
        return;
    }

    // Parse the effect order into the order array
    int order[10];
    sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           &order[0], &order[1], &order[2], &order[3], &order[4],
           &order[5], &order[6], &order[7], &order[8], &order[9]);

    // Close the file to start at the top again...
    // This is done since the effect order comes after the individual
    // effect parameters in the file. We do not know which effects
    // the parameters apply until we get the order first, uuuuuggghhlllyyy!!!!
    fclose(fn);

    if ((fn = fopen(filename, "r")) == NULL)
    {
        return;
    }

    // Program version
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(16, filename);
        file_error(fn);
        return;
    }

    // Author
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(17, filename);
        file_error(fn);
        return;
    }

    for (int i = 0; i < 64; i++)
    {
        if (buf[i] > 20)        // remove LF '\n'
        {
            preset_loaded.Author[i] = buf[i];
        }
    }

    // Preset Name
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(18, filename);
        file_error(fn);
        return;
    }

    for (int i = 0; i < 64; i++)
    {
        if (buf[i] > 20)        // remove LF '\n'
        {
            preset_loaded.Preset_Name[i] = buf[i];
        }
    }

    // Master control
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(19, filename);
        file_error(fn);
        return;
    }

    float in_vol, out_vol; in_vol = out_vol = 0.0;
    float balance = 1.0f;

    sscanf(buf, "%f,%f,%f,%d\n", &in_vol, &out_vol, &balance, &FX_Master_Active_Reset);

    if (!preserve_master)
    {
        preset_loaded.Fraction_Bypass = balance;
        preset_loaded.Input_Gain = in_vol;
        preset_loaded.Master_Volume = out_vol;
    }
    else    // Use current Master
    {
        preset_loaded.Fraction_Bypass = Active_Preset.Fraction_Bypass;
        preset_loaded.Input_Gain = Active_Preset.Input_Gain;
        preset_loaded.Master_Volume = Active_Preset.Master_Volume;
    }

    // Effect parameters
    for (int i = 0; i < 10; i++)
    {
        int effect = order[i];  // This is why we had to load the order first!!!

        memset(buf, 0, sizeof (buf));

        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            File_To_Load.clear();
            Handle_Message(19, filename);
            file_error(fn);
            return;
        }

        apply_effect_parameters(buf, effect, preset_loaded);
    }

    // Effect order... again!!
    memset(buf, 0, sizeof (buf));

    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(15, filename);
        file_error(fn);
        return;
    }

    // Apply the order
    sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           &preset_loaded.lv[EFX_ORDER][0], &preset_loaded.lv[EFX_ORDER][1],
           &preset_loaded.lv[EFX_ORDER][2], &preset_loaded.lv[EFX_ORDER][3],
           &preset_loaded.lv[EFX_ORDER][4], &preset_loaded.lv[EFX_ORDER][5],
           &preset_loaded.lv[EFX_ORDER][6], &preset_loaded.lv[EFX_ORDER][7],
           &preset_loaded.lv[EFX_ORDER][8], &preset_loaded.lv[EFX_ORDER][9]);

    // MIDI learn table
    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));

        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            File_To_Load.clear();
            Handle_Message(20, filename);
            file_error(fn);
            return;
        }

        sscanf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
               &preset_loaded.XUserMIDI[i][0], &preset_loaded.XUserMIDI[i][1],
               &preset_loaded.XUserMIDI[i][2], &preset_loaded.XUserMIDI[i][3],
               &preset_loaded.XUserMIDI[i][4], &preset_loaded.XUserMIDI[i][5],
               &preset_loaded.XUserMIDI[i][6], &preset_loaded.XUserMIDI[i][7],
               &preset_loaded.XUserMIDI[i][8], &preset_loaded.XUserMIDI[i][9],
               &preset_loaded.XUserMIDI[i][10], &preset_loaded.XUserMIDI[i][10],
               &preset_loaded.XUserMIDI[i][12], &preset_loaded.XUserMIDI[i][13],
               &preset_loaded.XUserMIDI[i][14], &preset_loaded.XUserMIDI[i][15],
               &preset_loaded.XUserMIDI[i][16], &preset_loaded.XUserMIDI[i][17],
               &preset_loaded.XUserMIDI[i][18], &preset_loaded.XUserMIDI[i][19]);
    }

    fclose(fn);

    // Clear the Main Window preset
    new_preset();

    // Copy the loaded preset to Main window
    Active_Preset = preset_loaded;

    set_audio_paramters();
}

void
RKR::file_error(FILE *fn)
{
    printf("File Error!\n");
    
    fclose(fn);
    set_audio_paramters();
}

/**
 * Sets the individual effect parameters for each effect.
 * Sets the main rack effect order.
 * Sets which effects are active or inactive.
 */
void
RKR::set_audio_paramters()
{
    // Shut off main audio processing while setting
    Active_Preset.FX_Master_Active = 0;

    // Copy the file names to the audio effects
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));
    strcpy(Efx_Convolotron->Filename, Active_Preset.ConvoFiname);

    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));
    strcpy(Efx_Reverbtron->Filename, Active_Preset.RevFiname);

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));
    strcpy(Efx_Echotron->Filename, Active_Preset.EchoFiname);
    
    // The main window effect order
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        efx_order[i] = Active_Preset.lv[EFX_ORDER][i];
    }
    
    // Looper is special
    Looper *Efx_Looper = static_cast<Looper*>(Rack_Effects[EFX_LOOPER]);
    
    // This is not optimized for audio processing.
    // It sets all parameters for all effects, even if they are not used.
    // This is beneficial for User editing and feedback, but not for fast preset changes
    if(Gui_Shown)
    {
        for (int all_efx = 0; all_efx < C_NUMBER_EFFECTS; all_efx++)
        {
            EFX_Active[all_efx] = EFX_Bank_Active[all_efx];     // FIXME Active_Preset

            if(all_efx != EFX_LOOPER)
                Rack_Effects[all_efx]->cleanup();


            for (int efx_params = 0; efx_params < EFX_Param_Size[all_efx]; efx_params++)
            {
                if(all_efx == EFX_LOOPER)
                {
                    Efx_Looper->set_value(efx_params, Active_Preset.lv[EFX_LOOPER][efx_params]);
                }
                else
                {
                    Rack_Effects[all_efx]->changepar(efx_params, Active_Preset.lv[all_efx][efx_params]);
                }
            }

            // We have to reset the Echotron Taps because we limit it to get_file_length().
            // But, it is set before the loaded file and would be limited by the 
            // previous file. So, reset it again after the requested file is loaded.
            if(all_efx == EFX_ECHOTRON)
            {
                Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Taps, Active_Preset.lv[all_efx][Echotron_Taps]);
            }

            if(all_efx == EFX_STEREOHARM)
            {
                if (Active_Preset.lv[EFX_STEREOHARM][Sharm_MIDI])
                    RC_Stereo_Harm->cleanup();
            }
        }
    }
    else    // No gui optimized audio processing
    {
        // These are specially processed and need to be shut of unless specifically set
        // from the ordered effects.
        EFX_Active[EFX_HARMONIZER] = 0;
        EFX_Active[EFX_STEREOHARM] = 0;
        EFX_Active[EFX_RING] = 0;

        // For each item on the main rack, check for a match to the efx_order
        for (int ordered_efx = 0; ordered_efx < C_NUMBER_ORDERED_EFFECTS; ordered_efx++)
        {
            for (int all_efx = 0; all_efx < C_NUMBER_EFFECTS; all_efx++)
            {
                // Is the effect one of the ordered?
                if(efx_order[ordered_efx] == all_efx)
                {
                    // Set the bypass for all ordered
                    EFX_Active[all_efx] = EFX_Bank_Active[all_efx];     // FIXME Active_Preset

                    // If the ordered effect is active, then set the parameters
                    // We do not need to set parameters for inactive effects
                    if(EFX_Active[all_efx])
                    {
                        if(all_efx != EFX_LOOPER)
                            Rack_Effects[all_efx]->cleanup();

                        for (int efx_params = 0; efx_params < EFX_Param_Size[all_efx]; efx_params++)
                        {
                            if(all_efx == EFX_LOOPER)
                            {
                                Efx_Looper->set_value(efx_params, Active_Preset.lv[EFX_LOOPER][efx_params]);
                            }
                            else
                            {
                                Rack_Effects[all_efx]->changepar(efx_params, Active_Preset.lv[all_efx][efx_params]);
                            }
                        }

                        // We have to reset the Echotron Taps because we limit it to get_file_length().
                        // But, it is set before the loaded file and would be limited by the 
                        // previous file. So, reset it again after the requested file is loaded.
                        if(all_efx == EFX_ECHOTRON)
                        {
                            Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Taps, Active_Preset.lv[all_efx][Echotron_Taps]);
                        }

                        if(all_efx == EFX_STEREOHARM)
                        {
                            if (Active_Preset.lv[EFX_STEREOHARM][Sharm_MIDI])
                                RC_Stereo_Harm->cleanup();
                        }
                    }
                }
            }
        }
    }

    // Reset the main audio processing to requested state
    // Could be on or off
    Active_Preset.FX_Master_Active = FX_Master_Active_Reset;

}

void
RKR::load_custom_MIDI_table_preset_names()
{
    memset(MIDI_Table_Bank_Preset_Names, 0, sizeof (MIDI_Table_Bank_Preset_Names));
    
    for (unsigned k = 0; k < Bank_Vector.size(); k++)
    {
        for (int j = 1; j <= 60; j++)
        {
            strcpy(MIDI_Table_Bank_Preset_Names[k][j].Preset_Name,  Bank_Vector[k].Bank[j].Preset_Name);
        }
    }
}


/**
 * Reverts the file ordering to program Bank[].lv[][] array ordering.
 * The Bank files (.rkrb types) effects indexing is saved in the order
 * of LV_File_Index. The program uses EFX_Index.
 * 
 * @param lv_revert
 *      A single Bank[].lv[][] array to be converted. This will alter the array.
 * 
 * @param size
 *      The size of the array sizeof().
 */
void
RKR::revert_file_to_bank(int lv_revert[C_MAX_EFFECTS][C_MAX_PARAMETERS], int size)
{
    // Table to revert LV_File_Index to EFX_Index.
    const int reversion[C_MAX_EFFECTS] =
    {
        EFX_REVERB,
        EFX_ECHO,
        EFX_CHORUS,
        EFX_FLANGER,
        EFX_PHASER,
        EFX_OVERDRIVE,
        EFX_DISTORTION,
        EFX_EQ,
        EFX_PARAMETRIC,
        EFX_COMPRESSOR,
        EFX_ORDER,           // 10
        EFX_WAHWAH,
        EFX_ALIENWAH,
        EFX_CABINET,
        EFX_PAN,
        EFX_HARMONIZER,
        EFX_MUSICAL_DELAY,
        EFX_NOISEGATE,
        EFX_DERELICT,
        EFX_ANALOG_PHASER,
        EFX_VALVE,           // 20
        EFX_DUAL_FLANGE,
        EFX_RING,
        EFX_EXCITER,
        EFX_DISTBAND,
        EFX_ARPIE,
        EFX_EXPANDER,
        EFX_SHUFFLE,
        EFX_SYNTHFILTER,
        EFX_VARYBAND,
        EFX_CONVOLOTRON,     // 30
        EFX_LOOPER,
        EFX_MUTROMOJO,
        EFX_ECHOVERSE,
        EFX_COILCRAFTER,
        EFX_SHELFBOOST,
        EFX_VOCODER,
        EFX_SUSTAINER,
        EFX_SEQUENCE,
        EFX_SHIFTER,
        EFX_STOMPBOX,        // 40
        EFX_REVERBTRON,
        EFX_ECHOTRON,
        EFX_STEREOHARM,
        EFX_COMPBAND,
        EFX_OPTICALTREM,
        EFX_VIBE,
        EFX_INFINITY,       // 47

        // offset by -1 for order moved to 69
        47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65, 66, 67, 68
    };

    // Temporary array to hold reverted 
    int temp[C_MAX_EFFECTS][C_MAX_PARAMETERS];
    memset(temp, 0, sizeof (temp));
    
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        // Find the reversion row of the bank
        int row = reversion[i];
        
        // Put the bank row into the temp by reversion row.
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            temp[row][j] = lv_revert[i][j];
        }
    }
    
    // Clear the bank array.
    memset(lv_revert, 0, sizeof (size));
    
    // Put the reverted temp array into the bank array.
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            lv_revert[i][j] = temp[i][j];
        }
    }
}

/**
 * Converts the program Bank[].lv[][] array ordering to file ordering.
 * The Bank files (.rkrb types) effects indexing is saved in the order
 * of LV_File_Index. This was originally part of legacy rakarrack which kept 
 * the lv[][] indexing the same for the program. The program otherwise
 * used the indexing of EFX_Index. This resulted in extra code having 
 * to individually map each effect when copying bank to preset. A
 * for() loop would be much easier if the two were aligned.
 * Originally this was all done with magic numbers. The conversion/reversion
 * is simply to align the lv[][] arrays with the program without having to 
 * change the file structure.
 * 
 * @param lv_convert
 *      A single Bank[].lv[][] array to be converted. This will alter the array.
 * 
 * @param size
 *      The size of the array sizeof().
 */
void
RKR::convert_bank_to_file(int lv_convert[C_MAX_EFFECTS][C_MAX_PARAMETERS], int size)
{
    // The effect order used for file saving lv[70][20] and Preset_Bank_Struct.lv[70][20].
    enum LV_File_Index
    {
        LV_REVERB = 0,
        LV_ECHO,
        LV_CHORUS,
        LV_FLANGER,
        LV_PHASER,
        LV_OVERDRIVE,
        LV_DISTORTION,
        LV_EQ,
        LV_PARAMETRIC,
        LV_COMPRESSOR,
        LV_ORDER,           // 10 - the order saved is EFX_Index order
        LV_WAHWAH,
        LV_ALIENWAH,
        LV_CABINET,
        LV_PAN,
        LV_HARMONIZER,
        LV_MUSICAL_DELAY,
        LV_NOISEGATE,
        LV_DERELICT,
        LV_ANALOG_PHASER,
        LV_VALVE,           // 20
        LV_DUAL_FLANGE,
        LV_RING,
        LV_EXCITER,
        LV_DISTBAND,
        LV_ARPIE,
        LV_EXPANDER,
        LV_SHUFFLE,
        LV_SYNTHFILTER,
        LV_VARYBAND,
        LV_CONVOLOTRON,     // 30
        LV_LOOPER,
        LV_MUTROMOJO,
        LV_ECHOVERSE,
        LV_COILCRAFTER,
        LV_SHELFBOOST,
        LV_VOCODER,
        LV_SUSTAINER,
        LV_SEQUENCE,
        LV_SHIFTER,
        LV_STOMPBOX,        // 40
        LV_REVERBTRON,
        LV_ECHOTRON,
        LV_STEREOHARM,
        LV_COMPBAND,
        LV_OPTICALTREM,
        LV_VIBE,
        LV_INFINITY        // 47
    };

    // Table to convert program EFX_Index to LV_File_Index.
    const int conversion[C_MAX_EFFECTS] = 
    {
        LV_EQ,             // 0
        LV_COMPRESSOR,
        LV_DISTORTION,
        LV_OVERDRIVE,
        LV_ECHO,
        LV_CHORUS,
        LV_PHASER,
        LV_FLANGER,
        LV_REVERB,
        LV_PARAMETRIC,
        LV_WAHWAH,         // 10
        LV_ALIENWAH,
        LV_CABINET,
        LV_PAN,
        LV_HARMONIZER,
        LV_MUSICAL_DELAY,
        LV_NOISEGATE,
        LV_DERELICT,
        LV_ANALOG_PHASER,
        LV_VALVE,
        LV_DUAL_FLANGE,    // 20
        LV_RING,
        LV_EXCITER,
        LV_DISTBAND,
        LV_ARPIE,
        LV_EXPANDER,
        LV_SHUFFLE,
        LV_SYNTHFILTER,
        LV_VARYBAND,
        LV_CONVOLOTRON,
        LV_LOOPER,         // 30
        LV_MUTROMOJO,
        LV_ECHOVERSE,
        LV_COILCRAFTER,
        LV_SHELFBOOST,
        LV_VOCODER,
        LV_SUSTAINER,
        LV_SEQUENCE,
        LV_SHIFTER,
        LV_STOMPBOX,
        LV_REVERBTRON,     // 40
        LV_ECHOTRON,
        LV_STEREOHARM,
        LV_COMPBAND,
        LV_OPTICALTREM,
        LV_VIBE,
        LV_INFINITY,       // 46

        // Offset by +1 for order inserted at 10
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
        59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,

        LV_ORDER           // 69
    };

    // Temporary array to hold converted
    int temp[C_MAX_EFFECTS][C_MAX_PARAMETERS];
    memset(temp, 0, sizeof (temp));
    
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        // Find the conversion row of the bank
        int row = conversion[i];
        
        // Put the bank row into the temp by conversion row.
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            temp[row][j] = lv_convert[i][j];
        }
    }
    
    // Clear the bank array.
    memset(lv_convert, 0, sizeof (size));
    
    // Put the converted temp array into the bank array.
    for(int i = 0; i < C_MAX_EFFECTS; i++)
    {
        for(int j = 0; j < C_MAX_PARAMETERS; j ++)
        {
            lv_convert[i][j] = temp[i][j];
        }
    }
}

/**
 * This parses the rakarrack bank files *.rkrb types.
 * This is no longer used because all files are loaded and checked
 * by load_bank_vector();
 * 
 * @param filename
 *      The bank file to parse.
 * 
 * @return 
 *      Zero (0) if an error occurs.
 *      One (1) if successful parsing.
 */
int
RKR::load_bank(const char *filename)
{
    int error_number = CheckOldBank(filename);
    
    if(error_number)
    {
        Handle_Message(error_number, filename);
        return (0);
    }
    
    PresetBankStruct Load_Bank[62];

    FILE *fn;

    if ((fn = fopen(filename, "rb")) != NULL)
    {
        new_bank(Load_Bank);

        while (1)
        {
            size_t ret = fread(&Load_Bank, sizeof (Load_Bank), 1, fn);

            if (feof(fn))
                break;

            if (ret != 1)
            {
                Handle_Message(22);
                new_bank(Bank);         // since we have a bad load, set the active bank to new
                fclose(fn);
                return 0;
            }
        }

        fclose(fn);

        if (big_endian())
        {
            fix_endianess(Load_Bank);
        }

        convert_IO(Load_Bank);

        for(int i = 0; i < 62; i++)
        {
            revert_file_to_bank(Load_Bank[i].lv, sizeof(Load_Bank[i].lv));
        }

        // copy the loaded bank to the active bank
        copy_bank(Bank, Load_Bank);

        bank_modified = 0;
        new_bank_loaded = 1;
        return (1);
    }

    return (0);
};

int
RKR::save_bank(const char *filename)
{
    FILE *fn;
    
    PresetBankStruct Save_Bank[62];
    
    // Copy active bank
    copy_bank(Save_Bank, Bank);

    if ((fn = fopen(filename, "wb")) != NULL)
    {
        copy_IO(Save_Bank);

        for(int i = 0; i < 62; i++)
        {
            convert_bank_to_file(Save_Bank[i].lv, sizeof(Save_Bank[i].lv));
        }

        if (big_endian())
        {
            fix_endianess(Save_Bank);
        }
        
        fwrite(&Save_Bank, sizeof (Save_Bank), 1, fn);

        fclose(fn);

        bank_modified = 0;
        return (1);
    }

    if (errno == EACCES)
    {
        Handle_Message(3);
    }
    
    return (0);
}

void
RKR::load_bank_vector()
{
    Bank_Vector.clear();

    std::vector<std::string>file_name;
    DIR *dir;
    struct dirent *fs;
    
    dir = opendir(DATADIR);
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
        std::string full_path = DATADIR;
        
        // Check for trailing '/'
        if(full_path[full_path.size() - 1] != '/')
            full_path += "/";
        
        full_path += file_name[i];
        add_bank_item(full_path);
    }
    
    // Clear for the next directory
    file_name.clear();
    
    // The user bank directory if set
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        dir = opendir(UDirFilename);
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
    }
    
    // Sort alpha numeric
    std::sort( file_name.begin(), file_name.end() );
    
    for(unsigned i = 0; i < file_name.size (); i++)
    {
        std::string full_path = UDirFilename;
        
        // Check for trailing '/'
        if(full_path[full_path.size() - 1] != '/')
            full_path += "/";

        full_path += file_name[i];
        add_bank_item(full_path);
    }
    
}

void
RKR::add_bank_item(std::string filename)
{
    int error_number = CheckOldBank(filename.c_str());
    
    if(error_number)
    {
        Handle_Message(error_number, filename);
        return;
    }

    FILE *fn;

    BankArray Another_Bank;

    if ((fn = fopen(filename.c_str(), "rb")) != NULL)
    {
        new_bank(Another_Bank.Bank);

        while (1)
        {
            size_t ret = fread(&Another_Bank.Bank, sizeof (Another_Bank.Bank), 1, fn);

            if (feof(fn))
                break;

            if (ret != 1)
            {
                Handle_Message(28, filename);
                fclose(fn);
                return;
            }
        }

        fclose(fn);

        if (big_endian())
        {
            fix_endianess(Another_Bank.Bank);
        }

        convert_IO(Another_Bank.Bank);

        for(int i = 0; i < 62; i++)
        {
            revert_file_to_bank(Another_Bank.Bank[i].lv, sizeof(Another_Bank.Bank[i].lv));
        }

        Another_Bank.Bank_File_Name = filename;
        
        std::string clean_name = strrchr(filename.c_str(),'/')+1;     // get the file name W/O path
        clean_name = clean_name.substr(0, clean_name.size() - c_rkrb_ext_size);   // remove extension
        Another_Bank.Bank_Name_Clean = clean_name;

        // Add the CC value for bank select
        std::string menu_name = "[";
        menu_name += NTS(Bank_Vector.size());
        menu_name += "] ";

        // Add the file name
        menu_name += clean_name;

        Another_Bank.Bank_Menu_Name = menu_name;

        Bank_Vector.push_back(Another_Bank);
    }
}

void
RKR::copy_bank(struct PresetBankStruct dest[], struct PresetBankStruct source[])
{
    new_bank(dest);

    for (int i = 0; i < 62; i++)
    {
        dest[i] = source[i];
    }

    bank_modified = 0;
    new_bank_loaded = 1;
}

/**
 * Set all main window values to default.
 * Can be user triggered with the New button on the main window.
 */
void
RKR::new_preset()
{
    // Set all main rack as inactive for display
    for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
    {
        active[j] = 0;
    }

    Active_Preset.new_preset();

    // Set all effects bypass to off
    for(int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        EFX_Bank_Active[i] = 0;     // FIXME check this
    }

    // Set the Master to OFF
    FX_Master_Active_Reset = 0;

    // Apply all the above settings to each effect, etc.
    set_audio_paramters();
}

void
RKR::new_bank(struct PresetBankStruct _bank[])
{
    for (int i = 0; i < 62; i++)
    {
        _bank[i].new_preset();
    }
};

/**
 * Copies all the preset information from the selected bank window preset to
 * the main window, and audio effects. This sets the audio parameters as well as
 * the gui, if shown.
 * 
 * @param preset_number
 *      The selected preset from the bank window. Corresponds to the number preceding
 *      the preset name in brackets [1] to [60].
 */
void
RKR::active_bank_preset_to_main_window(int preset_number)
{
    // Hold the Master active status since set_audio_parameters() must shut off on setting
    FX_Master_Active_Reset = Active_Preset.FX_Master_Active;

    // Does the user want to preserve Master settings
    float Input_Gain = 0.0, Master_Volume = 0.0, Fraction_Bypass = 0.0;

    if(preserve_master)
    {
        Input_Gain = Active_Preset.Input_Gain;
        Master_Volume = Active_Preset.Master_Volume;
        Fraction_Bypass = Active_Preset.Fraction_Bypass;
    }

    // Copy the preset
    Active_Preset = Bank[preset_number];

    // Reset these if the user wants to preserve Master settings
    if (preserve_master)
    {
        Active_Preset.Input_Gain = Input_Gain;
        Active_Preset.Master_Volume = Master_Volume;
        Active_Preset.Fraction_Bypass = Fraction_Bypass;
    }

    // Set the main rack effect order
    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        efx_order[k] = Active_Preset.lv[EFX_ORDER][k];
    }

    // Set the bypass state for each effect
    for(int k = 0; k < C_NUMBER_EFFECTS; k++)
    {
        EFX_Bank_Active[k] = Active_Preset.lv[k][C_BYPASS]; // FIXME - active preset could be used
    }

    // Apply the effect parameters to the audio classes
    set_audio_paramters();

    if ((Tap_Updated) && (Tap_Active) && (Tap_TempoSet > 0) && (Tap_TempoSet < 601))
    {
        Update_tempo();
    }
}

/**
 * Copy all the current main window preset information to the selected bank preset.
 * This is triggered by right mouse button on a bank preset item.
 *
 * @param preset_number
 *      The bank preset number which was selected by right mouse button. Corresponds
 *      to the number preceding the preset name in brackets [1] to [60].
 */
void
RKR::main_window_preset_to_active_bank(int preset_number)
{
    // Update all effect parameters into Active_Preset from audio classes
    // since they may have been changed by user.
    for (int k = 0; k < C_NUMBER_EFFECTS; k++)
    {
        for (int j = 0; j < EFX_Param_Size[k]; j++)
        {
            Active_Preset.lv[k][j] = Rack_Effects[k]->getpar(j);
        }
    }

    // Update main window effect order to Active_Preset since it may have been changed by user.
    for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
    {
        Active_Preset.lv[EFX_ORDER][j] = efx_order[j];
    }

    // Update filenames for Convolotron, Echotron, Reverbtron to Active_Preset
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Active_Preset.ConvoFiname, 0, sizeof (Active_Preset.ConvoFiname));
    strcpy(Active_Preset.ConvoFiname, Efx_Convolotron->Filename);
    
    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Active_Preset.RevFiname, 0, sizeof (Active_Preset.RevFiname));
    strcpy(Active_Preset.RevFiname, Efx_Reverbtron->Filename);

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Active_Preset.EchoFiname, 0, sizeof (Active_Preset.EchoFiname));
    strcpy(Active_Preset.EchoFiname, Efx_Echotron->Filename);

    // Copy the active preset to the bank
    Bank[preset_number] = Active_Preset;

    // Copy the current bypass state to the Bank
    for(int j = 0; j < C_NUMBER_EFFECTS; j++)
    {
        Bank[preset_number].lv[j][C_BYPASS] = EFX_Active[j];    // FIXME check this
    }
}

/**
 * Copy float into char array. Master Input, Volume, Balance. For precision in file saving?
 * 
 * @param _bank
 *      The bank to which the copy is made.
 */
void
RKR::copy_IO(struct PresetBankStruct _bank[])
{
    for (int i = 0; i < 62; i++)
    {
        memset(_bank[i].cInput_Gain, 0, sizeof (_bank[i].cInput_Gain));
        sprintf(_bank[i].cInput_Gain, "%f", _bank[i].Input_Gain);
        memset(_bank[i].cMaster_Volume, 0, sizeof (_bank[i].cMaster_Volume));
        sprintf(_bank[i].cMaster_Volume, "%f", _bank[i].Master_Volume);
        memset(_bank[i].cBalance, 0, sizeof (_bank[i].cBalance));
        sprintf(_bank[i].cBalance, "%f", _bank[i].Fraction_Bypass);
    }
}

/**
 * Convert char array to float. Master Input, Volume, Balance. For precision in file saving?
 * This is used for file loading. The converted amounts are used, not the saved floats.
 * 
 * @param _bank
 *      The bank used in the conversion.
 */
void
RKR::convert_IO(struct PresetBankStruct _bank[])
{
    for (int i = 0; i < 62; i++)
    {
        sscanf(_bank[i].cInput_Gain, "%f", &_bank[i].Input_Gain);
        if (_bank[i].Input_Gain == 0.0) _bank[i].Input_Gain = 0.5f;

        sscanf(_bank[i].cMaster_Volume, "%f", &_bank[i].Master_Volume);
        if (_bank[i].Master_Volume == 0.0) _bank[i].Master_Volume = 0.5f;

        sscanf(_bank[i].cBalance, "%f", &_bank[i].Fraction_Bypass);
        if (_bank[i].Fraction_Bypass == 0.0) _bank[i].Fraction_Bypass = 1.0f;
    }
}

int
RKR::big_endian()
{
    long one = 1;
    return !(*((char *) (&one)));
}

void
RKR::fix_endianess(struct PresetBankStruct _bank[])
{
    unsigned int data;

    for (int i = 0; i < 62; i++)
    {
        data = _bank[i].FX_Master_Active;
        data = SwapFourBytes(data);
        _bank[i].FX_Master_Active = data;

        for (int j = 0; j < 70; j++)
        {
            for (int k = 0; k < 20; k++)
            {
                data = _bank[i].lv[j][k];
                data = SwapFourBytes(data);
                _bank[i].lv[j][k] = data;
            }
        }

        for (int j = 0; j < 128; j++)
        {
            for (int k = 0; k < 20; k++)
            {
                data = _bank[i].XUserMIDI[j][k];
                data = SwapFourBytes(data);
                _bank[i].XUserMIDI[j][k] = data;
            }
        }
    }
}

void
RKR::save_skin(char *filename)
{
    FILE *fn;
    char buf[256];
    fn = fopen(filename, "w");
    if (errno == EACCES)
    {
        Handle_Message(3);
        fclose(fn);
        return;
    }

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d\n", swidth, sheight);
    fputs(buf, fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d,%d,%d\n", sback_color, sfore_color, slabel_color, sleds_color);
    fputs(buf, fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%s", BackgroundImage);
    fputs(buf, fn);
    fputs("\n", fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d,%d\n", global_font_size, global_font_type);
    fputs(buf, fn);

    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%d\n", sschema);
    fputs(buf, fn);

    fclose(fn);

}

bool
RKR::load_skin(char *filename)
{
    char buf[256];
    FILE *fn;

    if ((fn = fopen(filename, "r")) == NULL)
    {
        Handle_Message(23, filename);
        return 0;
    }

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        Handle_Message(23, filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d,%d\n", &swidth, &sheight);

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        Handle_Message(23, filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d,%d,%d,%d\n", &sback_color, &sfore_color, &slabel_color, &sleds_color);

    memset(BackgroundImage, 0, sizeof (BackgroundImage));
    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        Handle_Message(23, filename);
        load_skin_error(fn);
        return 0;
    }
    for (unsigned int i = 0; i < 256; i++)
    {
        if (buf[i] > 20)
        {
            BackgroundImage[i] = buf[i];
        }
    }

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        Handle_Message(23, filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d,%d\n", &sfont_size, &sfont_type);

    memset(buf, 0, sizeof (buf));
    if (fgets(buf, sizeof buf, fn) == NULL)
    {
        Handle_Message(23, filename);
        load_skin_error(fn);
        return 0;
    }
    sscanf(buf, "%d\n", &sschema);

    fclose(fn);
    
    return 1; // no errors
}

void
RKR::load_skin_error(FILE *fn)
{
    printf("Load Skin File error\n");
    fclose(fn);
}

void
RKR::dump_preset_names(void)
{
    for (int i = 0; i < 62; i++)
    {
        fprintf
        (
            stderr,
            "RKR_BANK_NAME:%d:%s\n",
            i,
            Bank[i].Preset_Name
        );
    }
}

int
RKR::CheckOldBank(const char *filename)
{
    long Length = 0;
    FILE *fs;

    if ((fs = fopen(filename, "r")) != NULL)
    {
        fseek(fs, 0L, SEEK_END);
        Length = ftell(fs);
        fclose(fs);
        
        if (Length == 993488) return (31);
        
        if (Length == 1092688) return (0);

    }

    return (14);
}

void
RKR::ConvertOldFile(char * filename)
{
    char buff[255];
    memset(buff, 0, sizeof (buff));
    sprintf(buff, "rakconvert -c '%s'", filename);
    
    if (system(buff) == -1)
    {
        Handle_Message(24);
    }
}

void
RKR::convert_reverb_file(char * filename)
{
    char buff[255];
    memset(buff, 0, sizeof (buff));
    sprintf(buff, "rakverb -i '%s'", filename);
    printf("%s\n", buff);
    
    if (system(buff) == -1)
    {
        Handle_Message(25);
    }
}

int
RKR::save_insert_preset(int num, char *name)
{
    std::string insert_preset_location = "";
    
    // Did the user set a User Directory
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        insert_preset_location = UDirFilename;
        insert_preset_location += "InsertPresets.rkis";
    }
    else
    {
        Handle_Message(41);
        return 0;
    }

    FILE *fn;
    char tempfile[256];
    char buf[256];
    char sbuf[260];
    memset(tempfile, 0, sizeof (tempfile));
    sprintf(tempfile, "%s", insert_preset_location.c_str());

    if ((fn = fopen(tempfile, "a")) != NULL)
    {
        memset(buf, 0, sizeof (buf));
        get_effect_parameters(buf, num);
        memset(sbuf, 0, sizeof (sbuf));
        sprintf(sbuf, "%d,%s,%s", num, name, buf);
        fputs(sbuf, fn);
        fclose(fn);
    }
    
    return 1;
}

void
RKR::delete_insert_preset(int num, char *name)
{
    std::string insert_preset_location = "";
    
    // Did the user set a User Directory
    if(strcmp(global_user_directory.c_str(), DATADIR) != 0)
    {
        insert_preset_location = global_user_directory;
        insert_preset_location += "InsertPresets.rkis";
    }
    else
    {
        // This is really not going to happen, because if they
        // did not set the user directory, then they cannot have
        // any user insert presets to delete.
        return;
    }

    FILE *fn;
    FILE *fs;
    char *rname;
    int eff = 0;
    char orden[1024];
    char tempfile[256];
    char tempfile2[256];
    char buf[256];
    char rbuf[256];

    char *sbuf;
    memset(tempfile, 0, sizeof (tempfile));
    memset(tempfile2, 0, sizeof (tempfile2));
    memset(orden, 0, sizeof (orden));

    sprintf(tempfile, "%s", insert_preset_location.c_str());

    if ((fs = fopen(tempfile, "r")) == NULL) return;

    sprintf(tempfile2, "%s%s", getenv("HOME"), "/.rkrtemp");

    if ((fn = fopen(tempfile2, "w")) != NULL)
    {
        memset(buf, 0, sizeof (buf));
        while (fgets(buf, sizeof buf, fs) != NULL)
        {
            sbuf = buf;
            memset(rbuf, 0, sizeof (rbuf));
            sprintf(rbuf, "%s", buf);
            sscanf(buf, "%d", &eff);
            rname = strsep(&sbuf, ",");
            rname = strsep(&sbuf, ",");
            if ((eff == num)&&(strcmp(rname, name) == 0))
            {
                continue;
            }
            else fputs(rbuf, fn);
            memset(buf, 0, sizeof (buf));
        }
    }

    fclose(fs);
    fclose(fn);

    sprintf(orden, "mv %s %s\n", tempfile2, tempfile);

    if (system(orden) == -1)
    {
        Handle_Message(26);
    }
}

/**
 *  This adds to the list of internal presets that are user defined. It can result in
 *  duplicate preset names for the same effect. FIXME
 * 
 * @param filename
 *      The file that holds the internal presets to be added.
 * 
 * @return 
 *      Zero(0) if error.
 *      One(1) on success.
 */
bool
RKR::merge_insert_presets(char *filename)
{
    std::string insert_preset_location = "";
    
    // Did the user set a User Directory
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        insert_preset_location = UDirFilename;
        insert_preset_location += "InsertPresets.rkis";
    }
    else
    {
        Handle_Message(41);
        return 0;
    }
    
    FILE *fn;
    if ((fn = fopen(filename, "r")) == NULL)
    {
        Handle_Message(23 , filename);
        return 0;
    }
    
    fclose(fn);
        
    char orden[1024];
    char tempfile[256];
    char tempfile2[256];

    memset(tempfile, 0, sizeof (tempfile));
    memset(tempfile2, 0, sizeof (tempfile2));
    memset(orden, 0, sizeof (orden));

    sprintf(tempfile, "%s", insert_preset_location.c_str());
    sprintf(tempfile2, "%s%s", getenv("HOME"), "/.rkrtemp");

    sprintf(orden, "cat %s '%s' > %s\n", tempfile, filename, tempfile2);
    
    if (system(orden) == -1)
    {
        Handle_Message(27);
        return 0;
    }
    
    memset(orden, 0, sizeof (orden));

    sprintf(orden, "mv %s %s\n", tempfile2, tempfile);
    if (system(orden) == -1)
    {
        Handle_Message(27);
        return 0;
    }

    return 1;
}

void
RKR::save_MIDI_table(char *filename)
{
    FILE *fn;
    char buf[256];
    fn = fopen(filename, "w");

    if (errno == EACCES)
    {
        Handle_Message(3);
        fclose(fn);
        return;
    }

    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        sprintf(buf, "%d,%d\n", MIDI_Table[i].bank, MIDI_Table[i].preset);
        fputs(buf, fn);
    }

    fclose(fn);
    
    PG_table_modified = 0;
}

void
RKR::load_MIDI_table(char *filename)
{
    char buf[256];
    FILE *fn;

    if ((fn = fopen(filename, "r")) == NULL)
    {
        Handle_Message(14, filename);
        return;
    }

    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            Handle_Message(14, filename);
            break;
        }

        sscanf(buf, "%d,%d\n", &MIDI_Table[i].bank, &MIDI_Table[i].preset);
    }

    fclose(fn);
}


void
RKR::load_MIDI_table_vector()
{
    Midi_Table_Vector.clear();
    
    std::vector<std::string>file_name;
    
    DIR *dir;
    struct dirent *fs;
    
    // Has a user directory been set?
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        dir = opendir(UDirFilename);

        if (dir == NULL)
        {
            return;
        }
    }
    else
        return; // No user directory set

    // Get the bank files in the directory
    while ((fs = readdir(dir)))
    {
        if (strstr(fs->d_name, ".rmt") != NULL)
        {
            file_name.push_back (fs->d_name);
        }
    }

    closedir(dir);
    
    // Sort alpha numeric
    std::sort( file_name.begin(), file_name.end() );
    
    for(unsigned i = 0; i < file_name.size (); i++)
    {
 
        std::string full_path = UDirFilename;

        // Check for trailing '/'
        if(full_path[full_path.size() - 1] != '/')
            full_path += "/";

        full_path += file_name[i];

        add_table_item(full_path);
    }
}

void 
RKR::add_table_item(std::string filename)
{
    // printf("Add table item %s\n", filename.c_str());
    char buf[256];
    FILE *fn;
    
    MIDItableArray Another_Table;

    if ((fn = fopen(filename.c_str(), "r")) == NULL)
    {
        Handle_Message(14, filename);
        return;
    }

    for (int i = 0; i < 128; i++)
    {
        memset(buf, 0, sizeof (buf));
        if (fgets(buf, sizeof buf, fn) == NULL)
        {
            Handle_Message(14, filename);
            fclose(fn);
            return;
        }

        sscanf(buf, "%d,%d\n", &Another_Table.MIDI_Table[i].bank, &Another_Table.MIDI_Table[i].preset);
    }
    
    Another_Table.Table_File_Name = filename;
    
    // Add the Menu name
    std::string menu_name = strrchr(filename.c_str(),'/')+1;     // get the file name W/O path
    menu_name = menu_name.substr(0, menu_name.size() - 4);   // remove extension .rmt = 4
    Another_Table.Table_Menu_Name = menu_name;
    
    Midi_Table_Vector.push_back(Another_Table);

    fclose(fn);
}

/**
 * Copy the requested MIDI table from vector to current active table.
 * 
 * @param item
 *      The MIDI table CC 32 (MIDI learnable) value sent or loaded in Settings/MIDI
 * 
 * @return 
 *      1 = within valid range of MIDI table vector.
 *      0 = Not in valid range.
 */
int
RKR::set_midi_table(int item)
{
    if(item < (int )Midi_Table_Vector.size())
    {
        for (int i = 0; i < 128; i++)
        {
            MIDI_Table[i].bank = Midi_Table_Vector[item].MIDI_Table[i].bank;
            MIDI_Table[i].preset = Midi_Table_Vector[item].MIDI_Table[i].preset;
        }

        return 1;
    }
    
    return 0;
}

void
RKR::load_default_midi_table()
{
    int load_default_midi_table = 1;
    if(custom_midi_table_file >= 0)
    {
        if(set_midi_table(custom_midi_table_file))
            load_default_midi_table = 0;
    }
    
    if(load_default_midi_table)
    {
        int k = 0;

        for (int i = 0; i < 128; i++)
        {
            if (i < 60)
                k = i;

            if ((i > 59)&&(i < 120))
                k = 1000 + i - 60;

            if (i > 119)
                k = 0;

            if (k < 1000)
            {
                MIDI_Table[i].bank = 0;
                MIDI_Table[i].preset = k;
            }

            if ((k > 999) && (k < 2000))
            {
                MIDI_Table[i].bank = 1;
                MIDI_Table[i].preset = k - 1000;
            }

            if ((k > 1999) && (k < 3000))
            {
                MIDI_Table[i].bank = 2;
                MIDI_Table[i].preset = k - 2000;
            }

            if ((k > 2999) && (k < 4000))
            {
                MIDI_Table[i].bank = 3;
                MIDI_Table[i].preset = k - 3000;
            }
        }
    }
}

int
RKR::file_in_midi_table_vector(std::string filename)
{
    for(unsigned i = 0; i < Midi_Table_Vector.size(); i++)
    {
        if(strcmp(filename.c_str(), Midi_Table_Vector[i].Table_File_Name.c_str()) == 0)
        {
            custom_midi_table_file = i;     // set the current file
            return 1;   // true
        }
    }

    return 0;   // false
}

void
RKR::load_convolotron_vector()
{
    Convolotron_WAV_Files.clear();

    std::vector<std::string>file_name;

    User_Files another_file;

    DIR *dir;
    struct dirent *fs;

    // Has a user directory been set?
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        dir = opendir(UDirFilename);

        if (dir == NULL)
        {
            Handle_Message(46, UDirFilename);
            return;
        }
    }
    else
        return; // No user directory set

    // Get the .wav files in the directory
    while ((fs = readdir(dir)))
    {
        if (strstr(fs->d_name, ".wav") != NULL)
        {
            file_name.push_back (fs->d_name);
        }
    }

    closedir(dir);

    for(unsigned i = 0; i < file_name.size (); i++)
    {
        another_file.User_File_Name_Clean = file_name[i];

        std::string full_path = UDirFilename;

        // Check for trailing '/'
        if(full_path[full_path.size() - 1] != '/')
            full_path += "/";

        full_path += file_name[i];
        another_file.User_File_Name = full_path;

        // Add the Menu name
        std::string menu_name = "* ";
        menu_name += file_name[i];
        menu_name = menu_name.substr(0, menu_name.size() - 4);   // remove extension .wav = 4

        another_file.User_File_Menu_Name = menu_name;

        Convolotron_WAV_Files.push_back(another_file);
    }

    // Copy to the audio effect class
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    Efx_Convolotron->set_user_files(Convolotron_WAV_Files);
}

void
RKR::load_echotron_vector()
{
    Echotron_DLY_Files.clear();

    std::vector<std::string>file_name;

    User_Files another_file;

    DIR *dir;
    struct dirent *fs;

    // Has a user directory been set?
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        dir = opendir(UDirFilename);

        if (dir == NULL)
        {
            Handle_Message(46, UDirFilename);
            return;
        }
    }
    else
        return; // No user directory set

    // Get the .wav files in the directory
    while ((fs = readdir(dir)))
    {
        if (strstr(fs->d_name, ".dly") != NULL)
        {
            file_name.push_back (fs->d_name);
        }
    }

    closedir(dir);

    for(unsigned i = 0; i < file_name.size (); i++)
    {
        another_file.User_File_Name_Clean = file_name[i];

        std::string full_path = UDirFilename;

        // Check for trailing '/'
        if(full_path[full_path.size() - 1] != '/')
            full_path += "/";

        full_path += file_name[i];
        another_file.User_File_Name = full_path;

        // Add the Menu name
        std::string menu_name = "* ";
        menu_name += file_name[i];
        menu_name = menu_name.substr(0, menu_name.size() - 4);   // remove extension .dly = 4

        another_file.User_File_Menu_Name = menu_name;

        Echotron_DLY_Files.push_back(another_file);
    }
    
    // Copy to the audio effect class
    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    Efx_Echotron->set_user_files(Echotron_DLY_Files);
}

void
RKR::load_reverbtron_vector()
{
    Reverbtron_RVB_Files.clear();

    std::vector<std::string>file_name;

    User_Files another_file;

    DIR *dir;
    struct dirent *fs;

    // Has a user directory been set?
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        dir = opendir(UDirFilename);

        if (dir == NULL)
        {
            Handle_Message(46, UDirFilename);
            return;
        }
    }
    else
        return; // No user directory set

    // Get the .wav files in the directory
    while ((fs = readdir(dir)))
    {
        if (strstr(fs->d_name, ".rvb") != NULL)
        {
            file_name.push_back (fs->d_name);
        }
    }

    closedir(dir);

    for(unsigned i = 0; i < file_name.size (); i++)
    {
        another_file.User_File_Name_Clean = file_name[i];

        std::string full_path = UDirFilename;

        // Check for trailing '/'
        if(full_path[full_path.size() - 1] != '/')
            full_path += "/";

        full_path += file_name[i];
        another_file.User_File_Name = full_path;

        // Add the Menu name
        std::string menu_name = "* ";
        menu_name += file_name[i];
        menu_name = menu_name.substr(0, menu_name.size() - 4);   // remove extension .rvb = 4

        another_file.User_File_Menu_Name = menu_name;

        Reverbtron_RVB_Files.push_back(another_file);
    }

    // Copy to the audio effect class
    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    Efx_Reverbtron->set_user_files(Reverbtron_RVB_Files);
}