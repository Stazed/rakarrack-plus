/*
  rakarrack - Audio effects software

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

#include "Effect.h"

/**
 * Virtual function for applying the effect Dry/Wet mix.
 * This is used by all effect that have Dry/Wet, and do not mix it internally.
 * Effects that do not have Dry/Wet will override this function, except for EFX_CABINET, 
 * with a call to Vol2_Efx(). EFX_CABINET calls Vol3_Efx().
 * 
 * @param efx_index
 *      The effect index from global.h EFX_Index.
 * 
 * @param volume
 *      The effect current volume amount.
 * 
 * @param efxoutl
 *      Master out buffer left.
 * 
 * @param efxoutr
 *      Master out buffer right.
 * 
 * @param smpl
 *      Master buffer for temporary copying and making adjustments left.
 * 
 * @param smpr
 *       Master buffer for temporary copying and making adjustments right.
 */
void
Effect::Dry_Wet_Mix(int efx_index, float volume,
                      float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    float v1, v2; v1 = v2 = 1.0f;

    if (volume < 0.5f)
    {
        v1 = 1.0f;
        v2 = volume * 2.0f;
    }
    else
    {
        v1 = (1.0f - volume) * 2.0f;
        v2 = 1.0f;
    }

    if ((efx_index == EFX_REVERB) || (efx_index == EFX_MUSICAL_DELAY))
    {
        v2 *= v2;
    }

    for (unsigned i = 0; i < period_master; i++)
    {
        efxoutl[i] = smpl[i] * v2 + efxoutl[i] * v1;
        efxoutr[i] = smpr[i] * v2 + efxoutr[i] * v1;
    }

    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}

/**
 * Volume/Gain adjustment for effects that do not have Dry/Wet or mix externally.
 * 
 * @param efxoutl
 *      Master out buffer left.
 * 
 * @param efxoutr
 *      Master out buffer right.
 * 
 * @param smpl
 *      Master buffer for temporary copying and making adjustments left.
 * 
 * @param smpr
 *       Master buffer for temporary copying and making adjustments right.
 */
void
Effect::Vol2_Efx(float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    memcpy(smpl, efxoutl, period_master * sizeof (float));
    memcpy(smpr, efxoutr, period_master * sizeof (float));
}

/**
 * Gain adjustment used exclusively by EFX_CABINET.
 * 
 * @param efxoutl
 *      Master out buffer left.
 * 
 * @param efxoutr
 *      Master out buffer right.
 * 
 * @param smpl
 *      Master buffer for temporary copying and making adjustments left.
 * 
 * @param smpr
 *       Master buffer for temporary copying and making adjustments right.
 */
void
Effect::Vol3_Efx(float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    float att = 2.0f;

    for (unsigned i = 0; i < period_master; i++)
    {
        efxoutl[i] *= att;
        efxoutr[i] *= att;
    }

    Vol2_Efx(efxoutl, efxoutr, smpl, smpr);
}