/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Effect.h"

void
Effect::volume_adjust(int NumEffect, float volume, int period,
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

    if ((NumEffect == EFX_REVERB) || (NumEffect == EFX_MUSICAL_DELAY))
    {
        v2 *= v2;
    }

    for (int i = 0; i < period; i++)
    {
        efxoutl[i] = smpl[i] * v2 + efxoutl[i] * v1;
        efxoutr[i] = smpr[i] * v2 + efxoutr[i] * v1;
    }

    Vol2_Efx(period, efxoutl, efxoutr, smpl, smpr);
}

void
Effect::Vol2_Efx(int period, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    memcpy(smpl, efxoutl, period * sizeof (float));
    memcpy(smpr, efxoutr, period * sizeof (float));
}

void
Effect::Vol3_Efx(int period, float *efxoutl, float *efxoutr, float *smpl, float *smpr)
{
    float att = 2.0f;

    for (int i = 0; i < period; i++)
    {
        efxoutl[i] *= att;
        efxoutr[i] *= att;
    }

    Vol2_Efx(period, efxoutl, efxoutr, smpl, smpr);
}