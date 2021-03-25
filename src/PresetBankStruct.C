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
    new_preset();   // load default parameters
}

/**
 * Clear and set all default parameters
 */
void
PresetBankStruct::new_preset()
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

    memset(Effect_Params, 0, sizeof (Effect_Params));

    for (int j = 0; j < C_MAX_EFFECTS; j++)
    {
        for (int k = 0; k < C_MAX_PARAMETERS; k++)
        {
            Effect_Params[j][k] = presets_default[j][k];
        }
    }

    // Clear MIDI learn
    memset(XUserMIDI, 0, sizeof (XUserMIDI));

    // Unused
    memset(XMIDIrangeMin, 0, sizeof (XMIDIrangeMin));
    memset(XMIDIrangeMax, 0, sizeof (XMIDIrangeMax));
}