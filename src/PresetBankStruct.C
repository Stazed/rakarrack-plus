/*

  PresetBankStruct.C

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

/* 
 * File:   PresetBankStruct.C
 * Author: sspresto
 * 
 * Created on March 22, 2021, 7:29 PM
 */

#include "PresetBankStruct.h"

PresetBankStruct::PresetBankStruct()
{
    new_preset(true);   // true - load default parameters
}

/**
 * Copy constructor.
 * 
 * @param orig
 *      The origin from which the copy is being made.
 */
PresetBankStruct::PresetBankStruct(const PresetBankStruct& orig)
{
    new_preset(false);  // false - do not load default effect parameters.

    strcpy(Preset_Name, orig.Preset_Name);
    strcpy(Author, orig.Author);
    strcpy(Classe, orig.Classe);
    strcpy(Type, orig.Type);
    strcpy(ConvoFiname, orig.ConvoFiname);
    strcpy(cInput_Gain, orig.cInput_Gain);
    strcpy(cMaster_Volume, orig.cMaster_Volume);
    strcpy(cBalance, orig.cBalance);

    Input_Gain = orig.Input_Gain;
    Master_Volume = orig.Master_Volume;
    Fraction_Bypass = orig.Fraction_Bypass;
    FX_Master_Active = orig.FX_Master_Active;

    strcpy(RevFiname, orig.RevFiname);
    strcpy(EchoFiname, orig.EchoFiname);

    memset(lv, 0, sizeof (lv));

    for (int j = 0; j < C_MAX_EFFECTS; j++)
    {
        for (int k = 0; k < C_MAX_PARAMETERS; k++)
        {
            lv[j][k] = orig.lv[j][k];
        }
    }

    memcpy(XUserMIDI, orig.XUserMIDI, sizeof (XUserMIDI));

    // not used
    memcpy(XMIDIrangeMin, orig.XMIDIrangeMin, sizeof (XMIDIrangeMin));
    memcpy(XMIDIrangeMax, orig.XMIDIrangeMax, sizeof (XMIDIrangeMax));

}

/**
 * Clear and zero all parameters. If this is load_default then set default preset
 * parameter values from presets_default[][].
 * 
 * @param load_default
 *      Flag to indicate if the default effect parameters should be loaded.
 *      true - load defaults
 *      false - do not load defaults
 */
void
PresetBankStruct::new_preset(bool load_default)
{
    memset(Preset_Name, 0, sizeof (char) * 64);
    memset(Author, 0, sizeof (char) * 64);
    memset(Classe, 0, sizeof (char) * 36);
    memset(Type, 0, sizeof (char) * 4);
    memset(ConvoFiname, 0, sizeof (char) * 128);
    memset(cInput_Gain, 0, sizeof (char) * 64);
    memset(cMaster_Volume, 0, sizeof (char) * 64);
    memset(cBalance, 0, sizeof (char) * 64);

    // Always set these to defaults
    Input_Gain = 0.5f;
    Master_Volume = 0.5f;
    Fraction_Bypass = 1.0f;             // Balance
    FX_Master_Active = 0;

    memset(RevFiname, 0, sizeof (char) * 128);
    memset(EchoFiname, 0, sizeof (char) * 128);

    memset(lv, 0, sizeof (lv));

    // Set the default effect parameters unless this is a clear only
    if(load_default)
    {
        for (int j = 0; j < C_NUMBER_EFFECTS; j++)
        {
            for (int k = 0; k < C_NUMBER_PARAMETERS; k++)
            {
                lv[j][k] = presets_default[j][k];
            }

            lv[j][C_BYPASS] = 0;    // FIXME
        }
    }
    // Set the default order - FIXME should be included in load_default above
    for (int k = 0; k < C_NUMBER_ORDERED_EFFECTS; k++)
    {
        // Order from 0 to 10
        lv[EFX_ORDER][k] = k;
    }

    // Clear MIDI learn
    memset(XUserMIDI, 0, sizeof (XUserMIDI));

    // Unused
    memset(XMIDIrangeMin, 0, sizeof (XMIDIrangeMin));
    memset(XMIDIrangeMax, 0, sizeof (XMIDIrangeMax));
}