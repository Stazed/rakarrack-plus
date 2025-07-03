/*
  rakarrack - a guitar effects software

  process.h  -  mainloop functions
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

#ifndef PROCESS_H
#define PROCESS_H

//main class for processing engine
#include "global.h"

#include "Config_fltk.h"
#include "PresetBankStruct.h"
#include "FX/Effect.h"
#include "FX/Reverb.h"
#include "FX/Chorus.h"
#include "FX/Flanger.h"
#include "FX/Echo.h"
#include "FX/Phaser.h"
#include "FX/Distorsion.h"
#include "FX/Overdrive.h"
#include "FX/EQ.h"
#include "FX/Compressor.h"
#include "Limiter.h"
#include "FX/Alienwah.h"
#include "FX/Cabinet.h"
#include "FX/WahWah.h"
#include "FX/Pan.h"
#include "FX/Harmonizer.h"
#include "FX/MusicDelay.h"
#include "FX/Gate.h"
#include "Tuner.h"
#include "FX/MIDIConverter.h"
#include "RecognizeNote.h"
#include "RecChord.h"
#include "FX/Derelict.h"
#include "FX/APhaser.h"
#include "FX/Valve.h"
#include "FX/Dual_Flange.h"
#include "FX/Ring.h"
#include "FX/Exciter.h"
#include "FX/DistBand.h"
#include "FX/Arpie.h"
#include "FX/Expander.h"
#include "FX/Shuffle.h"
#include "FX/Synthfilter.h"
#include "FX/VaryBand.h"
#include "FX/Convolotron.h"
#include "EFX_common/Resample.h"
#include "FX/Looper.h"
#include "FX/MuTroMojo.h"
#include "FX/Echoverse.h"
#include "FX/CoilCrafter.h"
#include "FX/ShelfBoost.h"
#include "FX/Vocoder.h"
#include "FX/Sustainer.h"
#include "FX/Sequence.h"
#include "FX/Shifter.h"
#include "FX/StompBox.h"
#include "FX/Reverbtron.h"
#include "FX/Echotron.h"
#include "FX/StereoHarm.h"
#include "FX/CompBand.h"
#include "FX/Opticaltrem.h"
#include "FX/Vibe.h"
#include "FX/Infinity.h"
#include "FX/ResSolution.h"
#include "FX/ParametricEQ.h"
#include "EFX_common/beattracker.h"
#include <jack/jack.h>
#include <sstream>
#include <pthread.h>


/**
 * This is the "MIDI Implementation Chart" listed in Rakarrack-plus help.
 * For default MIDI processing (Not MIDI learn) this index directly corresponds
 * to the MIDI Control Change values (0 - 127). When MIDI learn is used, the
 * direct correspondence is no longer valid since each index item is mapped to
 * the user selected Learn. This index plus the MIDI_Learn_Index combined is
 * the total of MIDI controllable parameters.
 * 
 * The index labels related to rack effects parameters are the same as the 
 * effect parameter index, with the added MC_ prefix. 
 * Example: WahWah_Depth >> MC_WahWah_Depth.
 */
enum MIDI_Control_Default_Index
{
    MC_Bank_Select = 0,

    MC_WahWah_Depth,
    MC_Dere_Drive,
    MC_Dere_Level,
    MC_Dere_LPF,
    MC_Dere_HPF,
    MC_Dere_Color,

    MC_Output_Volume,           // 7

    MC_Dere_Suboctave,
    MC_Dist_Suboctave,

    MC_Unused_10,
    MC_Unused_11,

    MC_Balance_FX,              // 12

    MC_Unused_13,

    MC_Input_Volume,            // 14

    MC_Unused_15,
    MC_Unused_16,
    MC_Unused_17,
    MC_Unused_18,
    MC_Unused_19,

    MC_Alien_Depth,             // 20
    MC_Phaser_Depth,
    MC_Flanger_Depth,
    MC_Chorus_Depth,
    MC_Music_Gain_1,
    MC_Music_Gain_2,
    MC_Harm_Filter_Freq,
    MC_Harm_Interval,
    MC_WahWah_DryWet,
    MC_Overdrive_DryWet,
    MC_Dist_DryWet,             // 30
    MC_Harm_DryWet,

    MC_Program_Table,           // custom midi table - 32

    MC_Unused_33,
    MC_Unused_34,
    MC_Unused_35,
    MC_Unused_36,
    MC_Unused_37,
    MC_Unused_38,
    MC_Unused_39,
    MC_Unused_40,
    MC_Unused_41,
    MC_Unused_42,
    MC_Unused_43,
    MC_Unused_44,
    MC_Unused_45,

    MC_Echo_Pan,
    MC_Overdrive_Pan,
    MC_Dist_Pan,
    MC_Harm_Pan,
    MC_Chorus_Pan,              // 50
    MC_Flanger_Pan,
    MC_Chorus_DryWet,
    MC_Flanger_DryWet,
    MC_Phaser_DryWet,
    MC_Alien_DryWet,
    MC_Music_DryWet,
    MC_Reverb_DryWet,
    MC_Pan_DryWet,
    MC_Echo_DryWet,
    MC_Phaser_Pan,              // 60
    MC_Alien_Pan,
    MC_Music_Pan_1,
    MC_Reverb_Pan,

    MC_Unused_64,

    MC_Music_Pan_2,
    MC_WahWah_Pan,
    MC_Pan_Pan,
    MC_Overdrive_Drive,
    MC_Dist_Drive,
    MC_Overdrive_Level,         // 70
    MC_Dist_Level,
    MC_Chorus_LFO_Tempo,
    MC_Flanger_LFO_Tempo,
    MC_Phaser_LFO_Tempo,
    MC_WahWah_LFO_Tempo,
    MC_Alien_LFO_Tempo,
    MC_Pan_LFO_Tempo,
    MC_Echo_Feedback,
    MC_Chorus_Feedback,
    MC_Flanger_Feedback,        // 80
    MC_Phaser_Feedback,
    MC_Alien_Feedback,
    MC_Music_Feedback_1,
    MC_Music_Feedback_2,
    MC_Overdrive_LPF,
    MC_Dist_LPF,
    MC_Reverb_LPF,
    MC_Overdrive_HPF,
    MC_Dist_HPF,
    MC_Reverb_HPF,              // 90
    MC_Chorus_LR_Cross,
    MC_Flanger_LR_Cross,
    MC_Phaser_LR_Cross,
    MC_Overdrive_LR_Cross,
    MC_Dist_LR_Cross,
    MC_Alien_LR_Cross,
    MC_Echo_LR_Cross,
    MC_Music_LR_Cross,
    MC_Chorus_LFO_Stereo,
    MC_Flanger_LFO_Stereo,      // 100
    MC_Phaser_LFO_Stereo,
    MC_WahWah_LFO_Stereo,
    MC_Alien_LFO_Stereo,
    MC_Pan_LFO_Stereo,
    MC_Chorus_LFO_Random,
    MC_Flanger_LFO_Random,
    MC_Phaser_LFO_Random,
    MC_WahWah_LFO_Random,
    MC_Alien_LFO_Random,
    MC_Pan_LFO_Random,          // 110
    MC_WahWah_Sense,
    MC_WahWah_ASI,
    MC_WahWah_Smooth,
    MC_Phaser_Phase,
    MC_Alien_Phase,

    MC_Multi_On_Off,            // 116

    MC_APhase_DryWet,
    MC_APhase_Distortion,
    MC_APhase_LFO_Tempo,
    MC_APhase_Depth,            // 120
    MC_APhase_Width,
    MC_APhase_Feedback,
    MC_APhase_Mismatch,
    MC_APhase_LFO_Stereo,
    MC_Dere_DryWet,
    MC_Dere_Pan,
    MC_Dere_LR_Cross            // 127
};

enum MIDI_Learn_Index
{
    MC_Unused_128 = 128,
    MC_Unused_129,
    MC_EQ_Gain,                 // 130
    MC_EQ_Q,
    MC_EQ_31_HZ,
    MC_EQ_63_HZ,
    MC_EQ_125_HZ,
    MC_EQ_250_HZ,
    MC_EQ_500_HZ,
    MC_EQ_1_KHZ,
    MC_EQ_2_KHZ,
    MC_EQ_4_KHZ,
    MC_EQ_8_KHZ,                // 140
    MC_EQ_16_KHZ,
    MC_Compress_Attack,
    MC_Compress_Release,
    MC_Compress_Ratio,
    MC_Compress_Knee,
    MC_Compress_Threshold,
    MC_Compress_Output,
    MC_Parametric_Gain,
    MC_Parametric_Low_Freq,
    MC_Parametric_Low_Gain,     // 150
    MC_Parametric_Low_Q,
    MC_Parametric_Mid_Freq,
    MC_Parametric_Mid_Gain,
    MC_Parametric_Mid_Q,
    MC_Parametric_High_Freq,
    MC_Parametric_High_Gain,
    MC_Parametric_High_Q,
    MC_DFlange_DryWet,
    MC_DFlange_Pan,
    MC_DFlange_LR_Cross,        // 160
    MC_DFlange_Depth,
    MC_DFlange_Width,
    MC_DFlange_Offset,
    MC_DFlange_Feedback,
    MC_DFlange_LPF,
    MC_DFlange_LFO_Tempo,
    MC_DFlange_LFO_Stereo,
    MC_DFlange_LFO_Random,
    MC_Valve_DryWet,
    MC_Valve_LR_Cross,          // 170
    MC_Valve_Pan,
    MC_Valve_Level,
    MC_Valve_Drive,
    MC_Valve_Distortion,
    MC_Valve_Presence,
    MC_Valve_LPF,
    MC_Valve_HPF,
    MC_Ring_DryWet,
    MC_Ring_LR_Cross,
    MC_Ring_Input,              // 180
    MC_Ring_Level,
    MC_Ring_Pan,
    MC_Ring_Depth,
    MC_Ring_Freq,
    MC_Ring_Sine,
    MC_Ring_Triangle,
    MC_Ring_Saw,
    MC_Ring_Square,
    MC_Exciter_Gain,
    MC_Exciter_LPF,             // 190
    MC_Exciter_HPF,
    MC_Exciter_Harm_1,
    MC_Exciter_Harm_2,
    MC_Exciter_Harm_3,
    MC_Exciter_Harm_4,
    MC_Exciter_Harm_5,
    MC_Exciter_Harm_6,
    MC_Exciter_Harm_7,
    MC_Exciter_Harm_8,
    MC_Exciter_Harm_9,          // 200
    MC_Exciter_Harm_10,
    MC_DistBand_DryWet,
    MC_DistBand_LR_Cross,
    MC_DistBand_Drive,
    MC_DistBand_Level,
    MC_DistBand_Gain_Low,
    MC_DistBand_Gain_Mid,
    MC_DistBand_Gain_Hi,
    MC_DistBand_Cross_1,
    MC_DistBand_Cross_2,        // 210
    MC_DistBand_Pan,
    MC_Arpie_DryWet,
    MC_Arpie_ArpeWD,
    MC_Arpie_Pan,
    MC_Arpie_Tempo,
    MC_Arpie_LR_Delay,
    MC_Arpie_LR_Cross,
    MC_Arpie_Feedback,
    MC_Arpie_Damp,
    MC_Expander_Attack,         // 220
    MC_Expander_Release,
    MC_Expander_Shape,
    MC_Expander_Threshold,
    MC_Expander_Gain,
    MC_Expander_LPF,
    MC_Expander_HPF,
    MC_Shuffle_DryWet,
    MC_Shuffle_Freq_L,
    MC_Shuffle_Gain_L,
    MC_Shuffle_Freq_ML,         // 230
    MC_Shuffle_Gain_ML,
    MC_Shuffle_Freq_MH,
    MC_Shuffle_Gain_MH,
    MC_Shuffle_Freq_H,
    MC_Shuffle_Gain_H,
    MC_Shuffle_Width,
    MC_Synthfilter_DryWet,
    MC_Synthfilter_Distort,
    MC_Synthfilter_LFO_Tempo,
    MC_Synthfilter_LFO_Stereo,  // 240
    MC_Synthfilter_Width,
    MC_Synthfilter_Feedback,
    MC_Synthfilter_Depth,
    MC_Synthfilter_Env_Sens,
    MC_Synthfilter_Attack,
    MC_Synthfilter_Release,
    MC_Synthfilter_Offset,
    MC_VaryBand_DryWet,
    MC_VaryBand_LFO_Tempo_1,
    MC_VaryBand_LFO_Stereo_1,   // 250
    MC_VaryBand_LFO_Tempo_2,
    MC_VaryBand_LFO_Stereo_2,
    MC_VaryBand_Cross_1,
    MC_VaryBand_Cross_2,
    MC_VaryBand_Cross_3,
    MC_MuTro_DryWet,
    MC_MuTro_LowPass,
    MC_MuTro_BandPass,
    MC_MuTro_HighPass,
    MC_MuTro_Depth,             // 260
    MC_MuTro_LFO_Tempo,
    MC_MuTro_Resonance,
    MC_MuTro_Range,
    MC_MuTro_Wah,
    MC_MuTro_Env_Sens,
    MC_MuTro_Env_Smooth,
    MC_Looper_DryWet,
    MC_Looper_Level_1,
    MC_Looper_Level_2,
    MC_Looper_Reverse,          // 270
    MC_Looper_AutoPlay,
    MC_Looper_Play,
    MC_Looper_Stop,
    MC_Looper_Record,
    MC_Looper_Rec_1,
    MC_Looper_Rec_2,
    MC_Looper_Track_1,
    MC_Looper_Track_2,
    MC_Looper_Clear,
    MC_Convo_DryWet,            // 280
    MC_Convo_Pan,
    MC_Convo_Level,
    MC_Convo_Damp,
    MC_Convo_Feedback,
    MC_Convo_Length,
    MC_Coil_Gain,
    MC_Coil_Tone,
    MC_Coil_Freq_1,
    MC_Coil_Q_1,
    MC_Coil_Freq_2,             // 290
    MC_Coil_Q_2,
    MC_Shelf_Gain,
    MC_Shelf_Level,
    MC_Shelf_Tone,
    MC_Shelf_Presence,
    MC_Vocoder_DryWet,
    MC_Vocoder_Pan,
    MC_Vocoder_Input,
    MC_Vocoder_Smear,
    MC_Vocoder_Q,               // 300
    MC_Vocoder_Ring,
    MC_Vocoder_Level,
    MC_Echoverse_DryWet,
    MC_Echoverse_Reverse,
    MC_Echoverse_Pan,
    MC_Echoverse_Tempo,
    MC_Echoverse_LR_Delay,
    MC_Echoverse_Feedback,
    MC_Echoverse_Damp,
    MC_Echoverse_Ext_Stereo,    // 310
    MC_Echoverse_Angle,
    MC_Sustain_Gain,
    MC_Sustain_Sustain,
    MC_Sequence_DryWet,
    MC_Sequence_Step_1,
    MC_Sequence_Step_2,
    MC_Sequence_Step_3,
    MC_Sequence_Step_4,
    MC_Sequence_Step_5,
    MC_Sequence_Step_6,         // 320
    MC_Sequence_Step_7,
    MC_Sequence_Step_8,
    MC_Sequence_Tempo,
    MC_Sequence_Resonance,
    MC_Sequence_Stdf,
    MC_Shifter_DryWet,
    MC_Shifter_Interval,
    MC_Shifter_Gain,
    MC_Shifter_Pan,
    MC_Shifter_Attack,          // 330
    MC_Shifter_Decay,
    MC_Shifter_Threshold,
    MC_Shifter_Whammy,
    MC_Stomp_Level,
    MC_Stomp_Gain,
    MC_Stomp_Bias,
    MC_Stomp_Mid,
    MC_Stomp_Tone,
    MC_Revtron_DryWet,
    MC_Revtron_Pan,             // 340
    MC_Revtron_Level,
    MC_Revtron_Damp,
    MC_Revtron_Feedback,
    MC_Revtron_Length,
    MC_Revtron_Stretch,
    MC_Revtron_I_Delay,
    MC_Revtron_Fade,
    MC_Echotron_DryWet,
    MC_Echotron_Pan,
    MC_Echotron_Tempo,          // 350
    MC_Echotron_Damp,
    MC_Echotron_Feedback,
    MC_Echotron_LR_Cross,
    MC_Echotron_LFO_Width,
    MC_Echotron_Depth,
    MC_Echotron_LFO_Stereo,
    MC_Echotron_Taps,
    MC_Sharm_DryWet,
    MC_Sharm_L_Interval,
    MC_Sharm_L_Chroma,          // 360
    MC_Sharm_L_Gain,
    MC_Sharm_R_Interval,
    MC_Sharm_R_Chroma,
    MC_Sharm_R_Gain,
    MC_Sharm_LR_Cross,
    MC_Sharm_Note,
    MC_Sharm_Chord,
    MC_CompBand_DryWet,
    MC_CompBand_Gain,
    MC_CompBand_Low_Ratio,      // 370
    MC_CompBand_Mid_1_Ratio,
    MC_CompBand_Mid_2_Ratio,
    MC_CompBand_High_Ratio,
    MC_CompBand_Low_Thresh,
    MC_CompBand_Mid_1_Thresh,
    MC_CompBand_Mid_2_Thresh,
    MC_CompBand_High_Thresh,
    MC_CompBand_Cross_1,
    MC_CompBand_Cross_2,
    MC_CompBand_Cross_3,        // 380
    MC_Optical_Depth,
    MC_Optical_LFO_Tempo,
    MC_Optical_LFO_Random,
    MC_Optical_LFO_Stereo,
    MC_Optical_Pan,
    MC_Vibe_DryWet,
    MC_Vibe_Width,
    MC_Vibe_Depth,
    MC_Vibe_LFO_Tempo,
    MC_Vibe_LFO_Random,         // 390
    MC_Vibe_LFO_Stereo,
    MC_Vibe_Feedback,
    MC_Vibe_LR_Cross,
    MC_Vibe_Pan,
    MC_Infinity_DryWet,
    MC_Infinity_Resonance,
    MC_Infinity_AutoPan,
    MC_Infinity_LR_Delay,
    MC_Infinity_Start,
    MC_Infinity_End,            // 400
    MC_Infinity_Tempo,
    MC_Infinity_Subdivision,
    MC_Alien_Delay,
    MC_APhase_LFO_Random,
    MC_Cabinet_Gain,
    MC_Chorus_Delay,
    MC_Echo_Reverse,
    MC_Echo_Delay,
    MC_Echo_LR_Delay,
    MC_Echo_Damp,               // 410
    MC_Flanger_Delay,
    MC_Harm_Gain,
    MC_Harm_Filter_Gain,
    MC_Harm_Filter_Q,
    MC_Infinity_Band_1,
    MC_Infinity_Band_2,
    MC_Infinity_Band_3,
    MC_Infinity_Band_4,
    MC_Infinity_Band_5,
    MC_Infinity_Band_6,         // 420    
    MC_Infinity_Band_7,
    MC_Infinity_Band_8,
    MC_Looper_Tempo,
    MC_Music_Tempo,
    MC_Music_Damp,
    MC_MuTro_LFO_Random,
    MC_MuTro_LFO_Stereo,
    MC_MuTro_St_Freq,
    MC_Gate_Attack,
    MC_Gate_Release,            // 430
    MC_Gate_Range,
    MC_Gate_Threshold,
    MC_Gate_Hold,
    MC_Gate_LPF,
    MC_Gate_HPF,
    MC_Pan_Ex_St_Amt,
    MC_Reverb_Time,
    MC_Reverb_I_Delay,
    MC_Reverb_Delay_FB,
    MC_Reverb_Room,             // 440
    MC_Reverb_Damp,
    MC_Revtron_Diffusion,
    MC_Revtron_LPF,
    MC_Harm_Note,
    MC_Harm_Chord,
    MC_Synthfilter_LFO_Random,
    MC_Harm_Select,
    MC_Sharm_Select,
    MC_Dist_Type,
    MC_Overdrive_Type,          // 450
    MC_Dere_Type,
    MC_DistBand_Type_Low,
    MC_DistBand_Type_Mid,
    MC_DistBand_Type_Hi,
    MC_Stomp_Mode,
    MC_Alien_LFO_Type,
    MC_APhase_LFO_Type,
    MC_Chorus_LFO_Type,
    MC_Flanger_LFO_Type,
    MC_DFlange_LFO_Type,        // 460
    MC_Echotron_LFO_Type,
    MC_MuTro_LFO_Type,
    MC_Optical_LFO_Type,
    MC_Pan_LFO_Type,
    MC_Phaser_LFO_Type,
    MC_Synthfilter_LFO_Type,
    MC_VaryBand_LFO_Type_1,
    MC_VaryBand_LFO_Type_2,
    MC_Vibe_LFO_Type,
    MC_WahWah_LFO_Type,         // 470
            
    MC_Ressol_DryWet,
    MC_Ressol_Distortion,
    MC_Ressol_LFO_Tempo,
    MC_Ressol_LFO_Stereo,
    MC_Ressol_Width,
    MC_Ressol_Feedback,
    MC_Ressol_Mismatch,
    MC_Ressol_Depth,
    TOTAL_MC_Parameter_Size

};

/**
 * The effect types used by the "Put Order in your Rack" window.
 */ 
enum Efx_Type_Index
{
    Type_All                = 0,
    Type_Distortion         = 1,
    Type_Modulation         = 2,
    Type_Time               = 4,
    Type_Emulation          = 8,
    Type_Distortion_and_Emulation = 9,    // StompBox, Valve
    Type_Filters            = 16,
    Type_Dynamics           = 32,
    Type_Processing_and_EQ  = 64,
    Type_Synthesis          = 128
};

enum GUI_Refresh_Index
{
    GUI_Refresh_Off = 0,
    GUI_Refresh_Plus_Key,
    GUI_Refresh_Minus_Key,
    GUI_Refresh_F2_Key,
    GUI_Refresh_F3_Key,
    GUI_Refresh_Looper,
    GUI_Refresh_CTRL_S
};

/**
 * Magic number 25 is Bank Select (CC 0) and the Unused cases from the default MIDI control
 * (0, 10, 11, 13, (15 > 19), (33 > 45), 64) = 23 -- Unused
 * plus 128, 129 also Unused  = 23 + 2 = 25.
 * If any of these unused parameters are used and added to
 * C_MC_PARAMETER_SIZE, this value must be adjusted.
 */
const int C_MC_UNUSED_PARAMETERS = 25;

/**
 * The total Number of MIDI controllable parameters.
 * This is the total of listed items in the MIDI learn window.
 * If any new parameters are added, this must be adjusted.
 */
const int C_MC_PARAMETER_SIZE = TOTAL_MC_Parameter_Size - C_MC_UNUSED_PARAMETERS;


/**
 * The total MIDI control range used in switch() case function.
 */
const int C_MC_RANGE = C_MC_PARAMETER_SIZE + C_MC_UNUSED_PARAMETERS;

/**
 * For the random preset generator, the maximum allowed excluded effects.
 */
const int C_MAX_EXCLUDED = 36;

#ifdef SYSEX_SUPPORT
const unsigned char  EVENT_SYSEX            = 0xF0;
const unsigned char  EVENT_SYSEX_END        = 0xF7;
#endif

/* To be incremented if anything changes */
#ifdef RKR_PLUS_LV2
const int C_LV2_STATE_VERSION = 1;
#endif

class RKR
{

public:

    explicit RKR (uint32_t _sample_rate, uint32_t _period, int gui);
    ~RKR ();

    // process.C
    void initialize(bool re_initialize = false);
    void delete_everything();
    void reset_all_effects(bool need_state_restore = true);
    void reset_everything();
    void reset_join_thread();
#ifdef RKR_PLUS_LV2
    void set_client_name(std::string s_name);
#else
    void set_jack_client(jack_client_t *_jackclient);
#endif
    void load_user_preferences();
    void instantiate_effects();
    void initialize_arrays();
    void put_order_in_rack();
    int load_bank_from_vector(const std::string &filename);
    void Adjust_Upsample();
    void add_metro();
    void calculavol (int i);
    int checkforaux();
    void Control_Gain (float *origl, float *origr);
    void Control_Volume (const float *origl, const float *origr);
    void cleanup_efx ();
    void process_effects (float *origl, float *origr ,void *);

    // rkrMIDI.C
    void MIDI_control();
    void ConnectMIDI ();
    void InitMIDI ();
    void miramidi ();
    void midievents();
    void ActOnOff();
    void ActiveUn(int value);
    int checkonoff(int value);
    void Conecta ();
    void conectaaconnect ();
    void disconectaaconnect ();
#ifdef RKR_PLUS_LV2
    void lv2_process_midi_program_changes();
    void lv2_join_thread();
#else
    void jack_process_midievents (jack_midi_event_t *midievent);
#endif
    void lv2_process_midievents(const uint8_t* const msg);
    void lv2_set_bpm(float a_bpm);
    void process_midi_controller_events(int parameter, int value, int preset = C_CHANGE_PRESET_OFF);
    void start_sysex(void);
    bool append_sysex( unsigned char *a_data, long a_size );
    void parse_sysex();
    void sysex_save_preset();

    // fileio.C
    void apply_effect_parameters (std::string s_buf, int fx_index, PresetBankStruct &preset_loaded);
    void get_effect_parameters (std::string &s_buf, int fx_index);
    void export_to_nsm_mixer(const std::string &filename);
    void export_to_carla(const std::string &filename);
    void save_preset (const std::string &filename);
    void load_preset (const std::string &filename);
    void set_audio_paramters ();
    int rkr_save_state(std::string &s_buf);
    void rkr_restore_state(const std::string &s_buf);
    int LV2_save_preferences(std::string &s_buf);
    void LV2_restore_preferences(const std::string &s_buf);
    void check_preferences_changed(std::vector<int> &s_vect);
#ifdef RKR_PLUS_LV2
    void lv2_update_params(uint32_t period);
#endif
    void load_custom_MIDI_table_preset_names();
    void revert_file_to_bank(int lv_revert[C_MAX_EFFECTS][C_MAX_PARAMETERS], int size);
    void convert_bank_to_file(int lv_convert[C_MAX_EFFECTS][C_MAX_PARAMETERS], int size);
    int load_bank (const std::string &filename);
    int save_bank (const std::string &filename, PresetBankStruct _Bank[]);
    void load_bank_vector();
    void add_bank_item(const std::string &filename);
    void copy_bank(struct PresetBankStruct dest[], const struct PresetBankStruct source[]);
    void new_preset ();
    void new_bank (struct PresetBankStruct _bank[] );
    void active_bank_preset_to_main_window (int preset_number);
    void main_window_preset_to_active_bank (int preset_number);
    void refresh_active_preset();
    void copy_IO(struct PresetBankStruct _bank[]);
    void convert_IO(struct PresetBankStruct _bank[]);
    int big_endian();
    void fix_endianess(struct PresetBankStruct _bank[]);
    void save_skin (const std::string &filename);
    bool load_skin (const std::string &filename);
    void load_skin_error(FILE *fn);
    void dump_preset_names ();

    int CheckOldBank(const std::string &filename);
    void ConvertOldFile(const std::string &filename);

    void convert_reverb_file(const std::string &filename);
    int save_insert_preset(int num, const std::string &name);
    void delete_insert_preset(int num, const std::string &name);
    void save_MIDI_table (const std::string &filename);
    void load_MIDI_table (const std::string &filename);
    void load_MIDI_table_vector();
    void add_table_item(const std::string &filename);
    int set_midi_table(int item);
    void load_default_midi_table();
    int file_in_midi_table_vector(const std::string &filename);
    void load_convolotron_vector();
    void load_echotron_vector();
    void load_reverbtron_vector();
    bool does_file_exist(const std::string &fileName);

    // varios.C
    int Message (int prio, const std::string &labelwin, const std::string &message_text);
    void Handle_Message(int num, std::string filename = "");
    int Get_Bogomips();
    int TapTempo();
    void TapTempo_Timeout(int state);
    void Update_tempo();

    Config_fltk Config;

    // class pointers
    class Tuner *efx_Tuner;
    class MIDIConverter *efx_MIDIConverter;
    class metronome *M_Metronome;
    class beattracker *beat;

    class Recognize *HarmRecNote;
    class Recognize *StHarmRecNote;
    class Recognize *RingRecNote;
    class RecChord *RC_Harm;
    class RecChord *RC_Stereo_Harm;

    /**
     * Pointer array to all rack effects, indexed by EFX_Index.
     */
    class Effect *Rack_Effects[EFX_NUMBER_EFFECTS];
    
    /**
     * To hold the number of user controlled parameters for each effect.
     * Indexed by EFX_Index.
     */
    int EFX_Param_Size[EFX_NUMBER_EFFECTS];

    /**
     * Arrays to hold bypass flags for each effect, indexed by EFX_Index.
     */
    int EFX_Active[EFX_NUMBER_EFFECTS];

    class Limiter *efx_FLimiter;

    class Resample *U_Resample;
    class Resample *D_Resample;
    class Resample *A_Resample;
    class AnalogFilter *DC_Offsetl;
    class AnalogFilter *DC_Offsetr;

#ifdef RKR_PLUS_LV2
    pthread_t t_pgm;
#endif
    pthread_t t_init;

    jack_client_t *jackclient;
    char jackcliname[64];
    int Jack_Shut_Down;

    /**
     * Flag for check box in Settings/Audio "+6dB Final Limiter.
     */
    int db6booster;

    /**
     * Main window master FX On/Off temporary flag to hold Active state for resetting.
     */
    int FX_Master_Active_Reset;
    
    int MIDIConverter_Active;
    int Metro_Active;
    int Tuner_Active;
    int Tap_Active;
    int ACI_Active;     // Analog control (trigger window)

    /**
     * Flag to indicate the program should terminate (user request). Or,
     * for LV2, the custom GUI is to be hidden, either by host or user.
     * 1 to quit, 0 to continue. Used in Main() processing loop.
     */
    int Exit_Program;

    /**
     * Flag for indicating the re-initialization is in progress.
     */
    int Re_init_in_progress;

    bool sco_anal_need_init;
    bool need_bogomips_message;
    bool handle_bogomips_message;
    bool lv2_is_active;

    /**
     * The current user selected bank preset index from button press in Bank window or
     * changed by MIDI control program change.
     */
    int Selected_Preset;

    /**
     * The preset index to change the Selected_Preset, or flag to indicate no change
     * is necessary. Used by command line and MIDI control program change.
     * This is processed in GuiTimeout.
     */
    int Change_Preset;

    int hold_preset;
    
    /**
     * Flag to indicate that a MIDI CC bank change has occurred. For GUI update.
     */
    int Change_Bank;

    /**
     * Flag to indicate that the midi table should be changed, for gui refresh on CC 32 bank select fine.
     */
    int Change_MIDI_Table;

    /**
     * Flag to indicate if the user used the -l command line option to load a preset file.
     */
    int Command_Line_File;
    
    /**
     * The file name from command line -l option to load a single preset
     */
    std::string File_To_Load;
    
    /*
     * Flag to indicate the if the program is CLI only or the gui is shown.
     * CLI = 0: Gui Shown = 1.
     */
    int Gui_Shown;
    
    /**
     * Flag to indicate that given gui index item needs refresh. Uses GUI_Refresh_Index
     */
    int Gui_Refresh;
    
    /**
     Flag to shut off below mouse highlight and grab focus.
     Certain pop up dialogs, like fl_choice, fl_message, fl_alert will crash if the
     widget is modified. Some of them return user_data values of non zero
     which get caught by the below mouse function.
    */
    int Shut_Off_Below_Mouse;

    /**
     * Flag to indicate if a bank is set from the command line.
     */
    int Bank_Load_Command_Line;

    int saved_order[C_NUMBER_ORDERED_EFFECTS];
    int efx_order[C_NUMBER_ORDERED_EFFECTS];
    int new_order[C_NUMBER_ORDERED_EFFECTS];
    int availables[60];
    int active[12];

    int help_displayed;

    /**
     * Flag to indicate if a bank has been changed by the user.
     */
    int bank_modified;

    int comemouse;

    int IsCoIn;
    int Cyoin;
    int Pyoin;
    int Ccin;
    int Pcin;

    // MIDI Learn
    /**
     * Array of flags telling GUI which midi controlled items to update.
     * For FLTK slider, value, choice visual adjustments for user notification.
     */
    int Mcontrol[C_MC_RANGE];
    
    /**
     * Used for MIDI learn window flashing "Get MIDI Message" light timer.
     */
    int RControl;

    /**
     * Flag to indicate when a CC has been received, for the MIDI learn window.
     */
    int ControlGet;

    /**
     * A counter for the flashing "Get MIDI Message" light.
     */
    int CountWait;

    // End MIDI learn
    
    /**
     * Flag to indicate when a quality update (Settings/Quality) is occurring. To shut off
     * effect and MIDI control processing while the effect is deleted and added.  
     */
    bool quality_update;

    /**
     * Holds the effect type, Order window value from Efx_Type_Index.
     */
    int Effect_Type_Filter;
    
    /**
     * The counter for the bank window alert message to be cleared after file saving.
     */
    int Alert_Count;

    /* Master resampling */
    int upsample;

    int M_Metro_Sound;
    int change_scale;
    int flpos;

    int m_displayed;
    int Mvalue;
    int Mnumeff[32];
    int OnOffC;

    uint32_t JACK_SAMPLE_RATE;
    uint32_t JACK_PERIOD;
    uint32_t period_master;
    uint32_t sample_rate;
    float fSample_rate;
    float cSample_rate;

    /**
     * Buffer for analog filters to interpolate (shared).
     */
    float *interpbuf;

    // Skin
    int swidth;
    int sheight;
    int sschema;
    int slabel_color;
    int sfore_color;
    int sback_color;
    int sleds_color;
    int sfont_size;
    int sfont_type;

    int have_signal;
    int OnCounter;
    int t_periods;

    // Tap Tempo
    int tempocnt;
    int Tap_Display;
    int Tap_Selection;
    int Tap_TempoSet;
    int Tap_SetValue;
    int t_timeout;
    int jt_state;
    int note_old;
    int cents_old;

    int cpufp;

    // Jack port connection flags.
    /**
     * Flag to indicate the connection state of audio in ports.
     */
    int Jack_IN_Port_Connnection_Status;

    /**
     * Flag to indicate the connection state of audio out ports.
     */
    int Jack_OUT_Port_Connnection_Status;
    
    /**
     * Flag to indicate the connection state of audio auxiliary in port.
     */
    int Jack_AUX_Port_Connnection_Status;

    /**
     * Flag to indicate the connection state of MIDI in port.
     */
    int Jack_MIDI_IN_Port_Connnection_Status;

    /**
     * Flag to indicate the connection state of MIDI out port.
     */
    int Jack_MIDI_OUT_Port_Connnection_Status;

    /**
     * Flag to indicate a Jack port connection state has changed.
     */
    int Jack_Port_Connnection_Changed;
    // End Jack port connections

    /**
     * Flag to indicate if the custom MIDI program change table was modified by the user.
     */
    int PG_table_modified;

    /**
     * The currently active bank displayed in the bank window.
     */
    int active_bank;

    int new_bank_loaded;

    int Aux_Gain;
    int Aux_Threshold;
    int Aux_MIDI;
    int Aux_Minimum;
    int Aux_Maximum;
    int Aux_Source;
    int last_auxvalue;

    int M_Metro_Tempo;
    int M_Metro_Bar;
    int mtc_counter;
    int ML_filter;
    int ML_clist[150];

    double Tap_timeB;
    double Tap_timeC;
    double jt_tempo;

    double tempobuf[6];


    double u_down;
    double u_up;

    timeval timeA;

    float booster;
    float cpuload;

#ifdef RKR_PLUS_LV2
    float *input_l;
    float *input_r;
#endif
    float *efxoutl;
    float *efxoutr;
    float *auxdata;
    float *auxresampled;
    float *anall;
    float *analr;
    float *smpl;
    float *smpr;
    float *denormal;
    float *m_ticks;

    float Log_I_Gain;
    float Log_M_Volume;
    float M_Metro_Vol;

    float val_sum;
    float old_il_sum;
    float old_ir_sum;
    float old_vl_sum;
    float old_vr_sum;
    float val_vl_sum;
    float val_vr_sum;
    float val_il_sum;
    float val_ir_sum;
    float old_a_sum;
    float val_a_sum;


    float bogomips;
    int looper_lqua;

    // Tuner
    float nfreq_old;
    float afreq_old;

    /**
     * The active preset displayed on the main window rack.
     */
    PresetBankStruct Active_Preset;

    /**
     * Any previously saved bank filename is saved here. Used for setting the
     * fltk file browser previous item.
     */
    std::string Bank_Saved;

    /**
     * The bank filename from the -b command line option.
     */
    std::string Command_Line_Bank;

    struct Effects_Names
    {
        char Nom[24];
        int Pos;
        int Type;
        
        Effects_Names():
            Nom(),
            Pos(),
            Type() {}
    } efx_names[70];

    struct MIDI_Control_Params
    {
        char Description[32];       // MIDI Learn window 
        int MC_params_index;
        int Effect_index;           // EFX_Index
        int Efx_param_index;
        int MC_offset;
        double MC_range;
        
        MIDI_Control_Params():
            Description(),
            MC_params_index(),
            Effect_index(),
            Efx_param_index(),
            MC_offset(),
            MC_range() {}
    } mc_efx_params[C_MC_PARAMETER_SIZE];

    /**
     * The currently active bank displayed on the Bank Window.
     */
    PresetBankStruct Bank[62];

    /**
     * Array to hold Banks from file loading.
     */
    struct BankArray
    {
        std::string Bank_File_Name;
        std::string Bank_Menu_Name;
        std::string Bank_Name_Clean;    // For MIDI table display
        PresetBankStruct Bank[62];
    };

    /**
     * Vector to hold all the available banks, default and user, in MIDI CC order for
     * MIDI control loading on Bank Select.
     */
    std::vector <BankArray> Bank_Vector;
    
    
    /**
     * MIDI Program Change Table bank and preset selection items.
     * This is the scroll table in Settings/Preferences/MIDI - MIDI Program Change Table
     */
    struct MIDI_table
    {
        int bank;
        int preset;
        
        MIDI_table():
            bank(),
            preset() {}
    };
    
    /**
     * The midi table array used for processing custom midi table program changes.
     */
    MIDI_table MIDI_Table[128];
    
    
    /**
     * Holds the midi table information for a midi table file.
     */
    struct MIDItableArray
    {
        std::string Table_File_Name;
        std::string Table_Menu_Name;
        MIDI_table MIDI_Table[128];
    };
    
    /**
     * Holds all midi table files and is loaded from the User Directory scan on program start.
     */
    std::vector <MIDItableArray> Midi_Table_Vector;

    /**
     * MIDI Program Change Table bank preset names.
     * This is the scroll table in Settings/Preferences/MIDI - MIDI Program Change Table
     */
    struct Bank_Names
    {
        char Preset_Name[64];
        
        Bank_Names():
            Preset_Name()
        {
            Preset_Name[0] = 0;
        }
    } MIDI_Table_Bank_Preset_Names[128][62];


    std::vector<User_Files> Convolotron_WAV_Files;
    std::vector<User_Files> Echotron_DLY_Files;
    std::vector<User_Files> Reverbtron_RVB_Files;

#ifndef RKR_PLUS_LV2
    // Alsa MIDI
    snd_seq_t *midi_in;
#endif
    
#ifdef SYSEX_SUPPORT
    // data for sysex
    std::vector<unsigned char> m_sysex;
    
    // flag for sysex messages
    int m_have_sysex_message;
    std::string m_preset_name;
    unsigned m_bank_number;
    unsigned m_preset_number;
#endif  // SYSEX_SUPPORT
};  // class RKR

#endif
