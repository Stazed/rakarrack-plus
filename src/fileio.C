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


const int presets_default[C_NUMBER_EFFECTS][C_NUMBER_PARAMETERS] = {
    //EQ1
    {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0},
    //Compressor
    {-30, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Distortion
    {0, 64, 0, 87, 14, 6, 0, 3134, 157, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //Overdrive
    {84, 64, 35, 56, 40, 0, 0, 6703, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Echo
    {67, 64, 565, 64, 30, 59, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Chorus
    {64, 64, 33, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Phaser
    {64, 64, 11, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0},
    //Flanger
    {64, 64, 39, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Reverb
    {80, 64, 63, 24, 0, 0, 0, 4002, 27, 83, 1, 64, 0, 0, 0, 0, 0, 0, 0},
    //EQ2 Parametric
    {200, 64, 64, 800, 64, 64, 12000, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //WahWah
    {64, 64, 80, 0, 0, 64, 70, 90, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //AlienWah
    {64, 64, 80, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //Cabinet
    {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Pan
    {64, 64, 26, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Harmonizer
    {64, 64, 64, 12, 6000, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //MusicDelay
    {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0, 0, 0, 0},
    //NoiseGate
    {0, 0, 1, 2, 6703, 76, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Derelict
    {0, 64, 64, 83, 65, 15, 0, 2437, 169, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //APhaser
    {64, 20, 14, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0, 0, 0, 0},
    //Valve
    {0, 64, 64, 127, 64, 0, 5841, 61, 1, 0, 69, 1, 84, 0, 0, 0, 0, 0, 0},
    //Dual Flange
    {32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0, 0, 0, 0, 0},
    //Ring
    {0, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0, 0, 0, 0, 0},
    //Exciter
    {127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20, 0, 0, 0, 0, 0, 0},
    //DistBand
    {0, 64, 64, 56, 40, 0, 0, 0, 29, 35, 100, 0, 450, 1500, 1, 0, 0, 0, 0},
    //Arpie
    {0, 64, 126, 64, 30, 59, 0, 127, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0},
    //Expander
    {-50, 20, 50, 50, 3134, 76, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Shuffle
    {64, 10, 0, 0, 0, 600, 1200, 2000, 6000, -14, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    //Synthfilter
    {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0, 0, 0, 0},
    //VaryBand
    {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Convolotron
    {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Looper
    {64, 0, 1, 0, 1, 0, 64, 1, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0},
    //MuTroMojo
    {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450, 1, 1, 0},
    //Echoverse
    {64, 64, 90, 64, 64, 64, 64, 0, 1, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //CoilCrafter
    {32, 6, 1, 3300, 16, 4400, 42, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //ShelfBoost
    {127, 64, 16000, 1, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Vocoder
    {0, 64, 10, 70, 70, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Systainer
    {40, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Sequence
    {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3, 0, 0, 0, 0},
    //Shifter
    {0, 64, 64, 200, 200, -20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //StompBox
    {48, 32, 0, 32, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Reverbtron
    {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64, 0, 0, 20000, 0, 0, 0, 0},
    //Echotron
    {64, 45, 34, 4, 0, 76, 3, 41, 0, 96, -13, 64, 1, 1, 1, 1, 0, 0, 0},
    //StereoHarm
    {64, 64, 12, 0, 64, 12, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0},
    //CompBand
    {0, 4, 4, 4, 4, 0, 0, 0, 0, 200, 2000, 15000, 88, 0, 0, 0, 0, 0, 0},
    //Opticaltrem
    {127, 260, 10, 0, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Vibe
    {35, 120, 10, 0, 64, 64, 64, 64, 3, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //Infinity
    {64, 64, -64, 64, -64, 64, -64, 64, -64, 0, 0, 0, 14, 0, 0, 0, 0, 4, 0}
};

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
void RKR::apply_effect_parameters(char *buf, int fx_index)
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
                    lv[effect][params] =  atoi( result.at(params).c_str() );
                }
                else    // Set the bypass
                {
                    EFX_Bank_Active[effect] = atoi( result.at(params).c_str() );
                }
            }

            // Convolotron, Reverbtron, Echotron have file names to parse as well
            if(fx_index == EFX_CONVOLOTRON)
            {
                Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
                memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));

                std::string s_name = result.at(EFX_Param_Size[effect] + 1);

                // Gotta remove the '\n' from the file name or error.
                s_name.erase(std::remove(s_name.begin(), s_name.end(), '\n'), s_name.end());
                strcpy(Efx_Convolotron->Filename, s_name.c_str());
            }
            
            else if(fx_index == EFX_REVERBTRON)
            {
                Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
                memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));

                std::string s_name = result.at(EFX_Param_Size[effect] + 1);

                // Gotta remove the '\n' from the file name or error.
                s_name.erase(std::remove(s_name.begin(), s_name.end(), '\n'), s_name.end());
                strcpy(Efx_Reverbtron->Filename, s_name.c_str());
            }

            else if(fx_index == EFX_ECHOTRON)
            {
                Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
                memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));

                std::string s_name = result.at(EFX_Param_Size[effect] + 1);

                // Gotta remove the '\n' from the file name or error.
                s_name.erase(std::remove(s_name.begin(), s_name.end(), '\n'), s_name.end());
                strcpy(Efx_Echotron->Filename, s_name.c_str());
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

    if (strlen(Author) != 0)
    {
        sprintf(buf, "%s\n", Author);
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
    fputs(Preset_Name, fn);
    fputs("\n", fn);

    // Master control
    memset(buf, 0, sizeof (buf));
    sprintf(buf, "%f,%f,%f,%d\n", Input_Gain, Master_Volume, Fraction_Bypass, FX_Master_Active);
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
                XUserMIDI[i][0], XUserMIDI[i][1], XUserMIDI[i][2], XUserMIDI[i][3], XUserMIDI[i][4],
                XUserMIDI[i][5], XUserMIDI[i][6], XUserMIDI[i][7], XUserMIDI[i][8], XUserMIDI[i][9],
                XUserMIDI[i][10], XUserMIDI[i][10], XUserMIDI[i][12], XUserMIDI[i][13], XUserMIDI[i][14],
                XUserMIDI[i][15], XUserMIDI[i][16], XUserMIDI[i][17], XUserMIDI[i][18], XUserMIDI[i][19]);

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

    if ((fn = fopen(filename, "r")) == NULL)
    {
        File_To_Load.clear();
        Handle_Message(14, filename);
        return;
    }

    new_preset();

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
    memset(Author, 0, 64);
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
        if (buf[i] > 20)
        {
            Author[i] = buf[i];
        }
    }


    // Preset Name
    memset(Preset_Name, 0, 64);
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
        if (buf[i] > 20)
        {
            Preset_Name[i] = buf[i];
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

    if (!actuvol)
    {
        Fraction_Bypass = balance;
        Input_Gain = in_vol;
        Master_Volume = out_vol;
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
        apply_effect_parameters(buf, effect);
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
           &lv[EFX_ORDER][0], &lv[EFX_ORDER][1], &lv[EFX_ORDER][2], &lv[EFX_ORDER][3], &lv[EFX_ORDER][4],
           &lv[EFX_ORDER][5], &lv[EFX_ORDER][6], &lv[EFX_ORDER][7], &lv[EFX_ORDER][8], &lv[EFX_ORDER][9]);

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
               &XUserMIDI[i][0], &XUserMIDI[i][1], &XUserMIDI[i][2], &XUserMIDI[i][3], &XUserMIDI[i][4],
               &XUserMIDI[i][5], &XUserMIDI[i][6], &XUserMIDI[i][7], &XUserMIDI[i][8], &XUserMIDI[i][9],
               &XUserMIDI[i][10], &XUserMIDI[i][10], &XUserMIDI[i][12], &XUserMIDI[i][13], &XUserMIDI[i][14],
               &XUserMIDI[i][15], &XUserMIDI[i][16], &XUserMIDI[i][17], &XUserMIDI[i][18], &XUserMIDI[i][19]);
    }

    fclose(fn);
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
    FX_Master_Active = 0;
    
    // The main window effect order
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        efx_order[i] = lv[EFX_ORDER][i];
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
            EFX_Active[all_efx] = EFX_Bank_Active[all_efx];

            if(all_efx != EFX_LOOPER)
                Rack_Effects[all_efx]->cleanup();


            for (int efx_params = 0; efx_params < EFX_Param_Size[all_efx]; efx_params++)
            {
                if(all_efx == EFX_LOOPER)
                {
                    Efx_Looper->set_value(efx_params, lv[EFX_LOOPER][efx_params]);
                }
                else
                {
                    Rack_Effects[all_efx]->changepar(efx_params, lv[all_efx][efx_params]);
                }
            }

            // We have to reset the Echotron Taps because we limit it to get_file_length().
            // But, it is set before the loaded file and would be limited by the 
            // previous file. So, reset it again after the requested file is loaded.
            if(all_efx == EFX_ECHOTRON)
            {
                Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Taps, lv[all_efx][Echotron_Taps]);
            }

            if(all_efx == EFX_STEREOHARM)
            {
                if (lv[EFX_STEREOHARM][Sharm_MIDI])
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
                    EFX_Active[all_efx] = EFX_Bank_Active[all_efx];

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
                                Efx_Looper->set_value(efx_params, lv[EFX_LOOPER][efx_params]);
                            }
                            else
                            {
                                Rack_Effects[all_efx]->changepar(efx_params, lv[all_efx][efx_params]);
                            }
                        }

                        // We have to reset the Echotron Taps because we limit it to get_file_length().
                        // But, it is set before the loaded file and would be limited by the 
                        // previous file. So, reset it again after the requested file is loaded.
                        if(all_efx == EFX_ECHOTRON)
                        {
                            Rack_Effects[EFX_ECHOTRON]->changepar(Echotron_Taps, lv[all_efx][Echotron_Taps]);
                        }

                        if(all_efx == EFX_STEREOHARM)
                        {
                            if (lv[EFX_STEREOHARM][Sharm_MIDI])
                                RC_Stereo_Harm->cleanup();
                        }
                    }
                }
            }
        }
    }

    // Reset the main audio processing to requested state
    // Could be on or off
    FX_Master_Active = FX_Master_Active_Reset;

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

    FILE *fn;

    if ((fn = fopen(filename, "rb")) != NULL)
    {
        new_bank(Bank);

        while (1)
        {
            size_t ret = fread(&Bank, sizeof (Bank), 1, fn);

            if (feof(fn))
                break;

            if (ret != 1)
            {
                Handle_Message(22);
                new_bank(Bank);
                fclose(fn);
                return 0;
            }
        }

        fclose(fn);

        if (big_endian())
        {
            fix_endianess();
        }

        convert_IO();

        for(int i = 0; i < 62; i++)
        {
            revert_file_to_bank(Bank[i].lv, sizeof(Bank[i].lv));
        }

        modified = 0;
        new_bank_loaded = 1;
        return (1);
    }

    return (0);
};

int
RKR::save_bank(const char *filename)
{
    FILE *fn;

    if ((fn = fopen(filename, "wb")) != NULL)
    {
        copy_IO();

        for(int i = 0; i < 62; i++)
        {
            convert_bank_to_file(Bank[i].lv, sizeof(Bank[i].lv));
        }

        if (big_endian())
        {
            fix_endianess();
        }
        
        fwrite(&Bank, sizeof (Bank), 1, fn);
        
        if (big_endian())
        {
            fix_endianess();
        }

        fclose(fn);

        for(int i = 0; i < 62; i++)
        {
            revert_file_to_bank(Bank[i].lv, sizeof(Bank[i].lv));
        }

        modified = 0;
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
    
    // The user bank directory
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
            fix_endianess();
        }

        convert_IO();

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
RKR::copy_bank(struct Preset_Bank_Struct dest[], struct Preset_Bank_Struct source[])
{
    new_bank(dest);
    
    for (int i = 0; i < 62; i++)
    {
        strcpy(dest[i].Preset_Name, source[i].Preset_Name);
        strcpy(dest[i].Author, source[i].Author);

        strcpy(dest[i].ConvoFiname, source[i].ConvoFiname);
        strcpy(dest[i].RevFiname, source[i].RevFiname);
        strcpy(dest[i].EchoFiname, source[i].EchoFiname);

        dest[i].Input_Gain = source[i].Input_Gain;
        dest[i].Master_Volume = source[i].Master_Volume;
        dest[i].Balance = source[i].Balance;
        dest[i].Active = source[i].Active;

        // Set the parameter values
        for (int j = 0; j < C_NUMBER_EFFECTS; j++)
        {
            for (int k = 0; k < C_NUMBER_PARAMETERS; k++)
            {
                dest[i].lv[j][k] = source[i].lv[j][k];
            }
            dest[i].lv[j][C_BYPASS] = source[i].lv[j][C_BYPASS];
        }
        
        // Set the order
        for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
        {
            dest[i].lv[EFX_ORDER][k] = source[i].lv[EFX_ORDER][k];
        }

        // Copy the MIDI learn table
        memcpy(dest[i].XUserMIDI, source[i].XUserMIDI, sizeof(source[i].XUserMIDI));
    }
    
    modified = 0;
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

    // Clear the preset name
    memset(Preset_Name, 0, sizeof (char) * 64);
    
    // Clear special effect file names
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));

    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));

    // Clear the author
    memset(Author, 0, sizeof (char) * 64);
    strcpy(Author, UserRealName);
    
    // Set master controls
    Input_Gain = .5f;
    Master_Volume = .5f;
    Fraction_Bypass = 1.0f;
    FX_Master_Active = 0;
    
    // Clear the effects parameters array
    memset(lv, 0, sizeof (lv));

    // Set the default effect parameters
    for (int j = 0; j < C_NUMBER_EFFECTS; j++)
    {
        for (int k = 0; k < C_NUMBER_PARAMETERS; k++)
        {
            lv[j][k] = presets_default[j][k];
        }

        lv[j][C_BYPASS] = 0;
    }

    // Set the default order
    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        // Order from 0 to 10
        lv[EFX_ORDER][k] = efx_order[k] = k;
    }
    
    // Set all effects bypass to off
    for(int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        EFX_Bank_Active[i] = 0;
    }

    FX_Master_Active_Reset = 0;

    // Clear MIDI learn
    memset(XUserMIDI, 0, sizeof (XUserMIDI));

    // Apply all the above settings to each effect, etc.
    set_audio_paramters();
}

void
RKR::new_bank(struct Preset_Bank_Struct a_bank[])
{
    for (int i = 0; i < 62; i++)
    {
        memset(a_bank[i].Preset_Name, 0, sizeof (a_bank[i].Preset_Name));
        memset(a_bank[i].Author, 0, sizeof (a_bank[i].Author));
        strcpy(a_bank[i].Author, UserRealName);
        memset(a_bank[i].ConvoFiname, 0, sizeof (a_bank[i].ConvoFiname));
        memset(a_bank[i].RevFiname, 0, sizeof (a_bank[i].RevFiname));
        memset(a_bank[i].EchoFiname, 0, sizeof (a_bank[i].EchoFiname));

        a_bank[i].Input_Gain = .5f;
        a_bank[i].Master_Volume = .5f;
        a_bank[i].Balance = 1.0f;
        a_bank[i].Active = 0;
        memset(a_bank[i].lv, 0, sizeof (a_bank[i].lv));

        // Set the default presets
        for (int j = 0; j < C_NUMBER_EFFECTS; j++)
        {
            for (int k = 0; k < C_NUMBER_PARAMETERS; k++)
            {
                a_bank[i].lv[j][k] = presets_default[j][k];
            }
            a_bank[i].lv[j][C_BYPASS] = 0;
        }
        
        // Set the default order
        for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
        {
            a_bank[i].lv[EFX_ORDER][k] = k;   // Order from 0 to 10
        }

        memset(a_bank[i].XUserMIDI, 0, sizeof (a_bank[i].XUserMIDI));
    }
};

void
RKR::bank_to_preset(int i)
{
    // Do not need to copy this if no gui
    if(Gui_Shown)
    {
        memset(Preset_Name, 0, sizeof (char) * 64);
        strcpy(Preset_Name, Bank[i].Preset_Name);
        memset(Author, 0, sizeof (char) * 64);
        strcpy(Author, Bank[i].Author);
    }

    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Efx_Convolotron->Filename, 0, sizeof (Efx_Convolotron->Filename));
    strcpy(Efx_Convolotron->Filename, Bank[i].ConvoFiname);

    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Efx_Reverbtron->Filename, 0, sizeof (Efx_Reverbtron->Filename));
    strcpy(Efx_Reverbtron->Filename, Bank[i].RevFiname);

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Efx_Echotron->Filename, 0, sizeof (Efx_Echotron->Filename));
    strcpy(Efx_Echotron->Filename, Bank[i].EchoFiname);

    for (int j = 0; j < C_MAX_EFFECTS; j++)
    {
        for (int k = 0; k < C_MAX_PARAMETERS; k++)
        {
            lv[j][k] = Bank[i].lv[j][k];
        }
    }

    // Set the main rack effect order
    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        efx_order[k] = Bank[i].lv[EFX_ORDER][k];
    }
    
    // Set the bypass state for each effect
    for(int k = 0; k < C_NUMBER_EFFECTS; k++)
    {
        EFX_Bank_Active[k] = Bank[i].lv[k][C_BYPASS];
    }

    FX_Master_Active_Reset = FX_Master_Active;

    memcpy(XUserMIDI, Bank[i].XUserMIDI, sizeof (XUserMIDI));

    set_audio_paramters();

    if (actuvol == 0)
    {
        Input_Gain = Bank[i].Input_Gain;
        Master_Volume = Bank[i].Master_Volume;
        Fraction_Bypass = Bank[i].Balance;
    }

    if ((Tap_Updated) && (Tap_Active) && (Tap_TempoSet > 0) && (Tap_TempoSet < 601))
    {
        Update_tempo();
    }
}

void
RKR::preset_to_bank(int i)
{
    // Main window information
    memset(Bank[i].Preset_Name, 0, sizeof (Bank[i].Preset_Name));
    strcpy(Bank[i].Preset_Name, Preset_Name);
    memset(Bank[i].Author, 0, sizeof (Bank[i].Author));
    strcpy(Bank[i].Author, Author);
    
    // Special cases filenames for Convolotron, Echotron, Reverbtron
    Convolotron *Efx_Convolotron = static_cast<Convolotron*>(Rack_Effects[EFX_CONVOLOTRON]);
    memset(Bank[i].ConvoFiname, 0, sizeof (Bank[i].ConvoFiname));    
    strcpy(Bank[i].ConvoFiname, Efx_Convolotron->Filename);
    
    Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(Rack_Effects[EFX_REVERBTRON]);
    memset(Bank[i].RevFiname, 0, sizeof (Bank[i].RevFiname));
    strcpy(Bank[i].RevFiname, Efx_Reverbtron->Filename);

    Echotron *Efx_Echotron = static_cast<Echotron*>(Rack_Effects[EFX_ECHOTRON]);
    memset(Bank[i].EchoFiname, 0, sizeof (Bank[i].EchoFiname));
    strcpy(Bank[i].EchoFiname, Efx_Echotron->Filename);

    // Master effect
    Bank[i].Input_Gain = Input_Gain;
    Bank[i].Master_Volume = Master_Volume;
    Bank[i].Balance = Fraction_Bypass;

    // Load all effect parameters into the lv[][] array from current preset (main window)
    for (int k = 0; k < C_NUMBER_EFFECTS; k++)
    {
        for (int j = 0; j < EFX_Param_Size[k]; j++)
            lv[k][j] = Rack_Effects[k]->getpar(j);
    }
    
    // Get the main window effect order
    for (int j = 0; j < C_NUMBER_ORDERED_EFFECTS; j++)
        lv[EFX_ORDER][j] = efx_order[j];

    // Copy the lv[][] parameters to the Bank[].lv[][] structure
    for (int j = 0; j < C_MAX_EFFECTS; j++)
    {
        for (int k = 0; k < C_NUMBER_PARAMETERS; k++)    // bypass is not copied here
        {
            Bank[i].lv[j][k] = lv[j][k];
        }
    }

    // Copy the current bypass state to the Bank
    for(int j = 0; j < C_NUMBER_EFFECTS; j++)
    {
        Bank[i].lv[j][C_BYPASS] = EFX_Active[j];
    }

    // Copy MIDI learn to the Bank
    memcpy(Bank[i].XUserMIDI, XUserMIDI, sizeof (XUserMIDI));
}

void
RKR::copy_IO()
{
    for (int i = 0; i < 62; i++)
    {
        memset(Bank[i].cInput_Gain, 0, sizeof (Bank[i].cInput_Gain));
        sprintf(Bank[i].cInput_Gain, "%f", Bank[i].Input_Gain);
        memset(Bank[i].cMaster_Volume, 0, sizeof (Bank[i].cMaster_Volume));
        sprintf(Bank[i].cMaster_Volume, "%f", Bank[i].Master_Volume);
        memset(Bank[i].cBalance, 0, sizeof (Bank[i].cBalance));
        sprintf(Bank[i].cBalance, "%f", Bank[i].Balance);
    }
}

void
RKR::convert_IO()
{
    for (int i = 0; i < 62; i++)
    {
        sscanf(Bank[i].cInput_Gain, "%f", &Bank[i].Input_Gain);
        if (Bank[i].Input_Gain == 0.0) Bank[i].Input_Gain = 0.5f;

        sscanf(Bank[i].cMaster_Volume, "%f", &Bank[i].Master_Volume);
        if (Bank[i].Master_Volume == 0.0) Bank[i].Master_Volume = 0.5f;

        sscanf(Bank[i].cBalance, "%f", &Bank[i].Balance);
        if (Bank[i].Balance == 0.0) Bank[i].Balance = 1.0f;
    }
}

int
RKR::big_endian()
{
    long one = 1;
    return !(*((char *) (&one)));
}

void
RKR::fix_endianess()
{
    unsigned int data;

    for (int i = 0; i < 62; i++)
    {
        data = Bank[i].Active;
        data = SwapFourBytes(data);
        Bank[i].Active = data;

        for (int j = 0; j < 70; j++)
        {
            for (int k = 0; k < 20; k++)
            {
                data = Bank[i].lv[j][k];
                data = SwapFourBytes(data);
                Bank[i].lv[j][k] = data;
            }
        }

        for (int j = 0; j < 128; j++)
        {
            for (int k = 0; k < 20; k++)
            {
                data = Bank[i].XUserMIDI[j][k];
                data = SwapFourBytes(data);
                Bank[i].XUserMIDI[j][k] = data;
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
    
    if(strcmp(UDirFilename, DATADIR) != 0)
    {
        dir = opendir(UDirFilename);

        if (dir == NULL)
        {
            return;
        }
     }

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

int
RKR::set_midi_table(int item)
{
    if(item >= (int)Midi_Table_Vector.size())
        return 0;
    
    for (int i = 0; i < 128; i++)
    {
        MIDI_Table[i].bank = Midi_Table_Vector[item].MIDI_Table[i].bank;
        MIDI_Table[i].preset = Midi_Table_Vector[item].MIDI_Table[i].preset;
    }
    
    return 1;
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