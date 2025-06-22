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

#ifndef EFFECT_H
#define EFFECT_H

#include "../global.h"
#include "../EFX_common/FPreset.h"

struct User_Files
{
    std::string User_File_Name;         // Full path
    std::string User_File_Menu_Name;    // File name w/o extension and with *
    std::string User_File_Name_Clean;   // File name w/o path
};

enum
{
    NON_MIXER_XT = 0,
    CARLA
};

class Effect
{
public:

    Effect (double, uint32_t period) :
        period_master(period), outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    // Distortion, Overdrive, Derelict, DistBand, StompBox, Convolotron, Reverbtron
    Effect (int, int, int, double, uint32_t period) :
        period_master(period), outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    // Harmonizer, Sequence, Shifter, StereoHarm
    Effect (long int, int, int, int, double, uint32_t period) :
        period_master(period), outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    // Looper
    Effect (float, double, uint32_t period) :
        period_master(period), outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    // Vocoder
    Effect (float *, int, int, int, int, double, uint32_t period) :
        period_master(period), outvolume(0.5f), Ppreset(0), Fpre(NULL) {};

    virtual ~ Effect () {};
    
    virtual void setpreset (int) = 0;
    virtual void changepar (int, int) = 0;
    virtual int getpar (int) = 0;
    virtual void out (float *, float *) = 0;
    virtual void cleanup () = 0;
    virtual int get_number_efx_parameters () = 0;
    virtual void set_random_parameters() {};
    
    virtual void Dry_Wet_Mix(int efx_index, float volume,
        float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    
    void Vol2_Efx(float *efxoutl, float *efxoutr, float *smpl, float *smpr);
    
    void Vol3_Efx(float *efxoutl, float *efxoutr, float *smpl, float *smpr);

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    virtual void lv2_update_params(uint32_t)  = 0;
#endif // LV2
    virtual void LV2_parameters(std::string &, int) {};
    virtual std::string get_URI(int) { return ""; };
    virtual std::string get_name(int) { return ""; };
    void Carla_LV2_port(std::string &, int index, int value, const std::string &name, const std::string &symbol);

    uint32_t period_master;
    float outvolume;
    int Ppreset;
    class FPreset *Fpre;

};

#endif
